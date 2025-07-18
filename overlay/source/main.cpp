#define TESLA_INIT_IMPL
#include "tune.h"
#include "gui_error.hpp"
#include "gui_main.hpp"
#include "sdmc/sdmc.hpp"
#include "pm/pm.hpp"
#include "config/config.hpp"

#include <tesla.hpp>

class SysTuneOverlay final : public tsl::Overlay {
  private:
    const char *msg = nullptr;
    Result fail     = 0;

  public:
    void initServices() override {
        if (R_FAILED(pm::Initialize())) {
            this->msg  = "Failed pm::Initialize()";
            return;
        }

        u64 pid{}, tid{};
        pm::getCurrentPidTid(&pid, &tid);

        // don't open sys-tune-enhanced if blacklisted title is active!
        if (config::get_title_blacklist(tid)) {
            this->msg =
                "Title is blacklisted!\n"
                "Exit to use sys-tune-enhanced";
            return;
        }

        // Also do not open if a non whitelisted title is active and whitelist mode is enabled!
        if (config::get_whitelist_mode() && !config::get_title_whitelist(tid)) {
            this->msg =
                "Whitelist Mode is active \n"
                "Title is not whitelisted!\n"
                "Exit to use sys-tune-enhanced";
            return;
        }

        Result rc = tuneInitialize();

        // not found can happen if the service isn't started
        // connection refused can happen is the service was terminated by pmshell
        if (R_VALUE(rc) == KERNELRESULT(NotFound) || R_VALUE(rc) == KERNELRESULT(ConnectionRefused)) {
            u64 pid = 0;
            const NcmProgramLocation programLocation{
                .program_id = 0x4200000000000000,
                .storageID  = NcmStorageId_None,
            };
            rc = pmshellInitialize();
            if (R_SUCCEEDED(rc)) {
                rc = pmshellLaunchProgram(0, &programLocation, &pid);
                pmshellExit();
            }
            if (R_FAILED(rc) || pid == 0) {
                this->fail = rc;
                this->msg  = "  Failed to\n"
                            "launch sysmodule";
                return;
            }
            svcSleepThread(500'000'000ULL);
            rc = tuneInitialize();
        }

        if (R_FAILED(rc)) {
            this->msg  = "Something went wrong:";
            this->fail = rc;
            return;
        }

        if (R_FAILED(sdmc::Open())) {
            this->msg  = "Failed sdmc::Open()";
            return;
        }

        u32 api;
        if (R_FAILED(tuneGetApiVersion(&api)) || api != TUNE_API_VERSION) {
            this->msg = "   Unsupported\n"
                        "sys-tune-enhanced version!";
        }
    }

    void exitServices() override {
        sdmc::Close();
        pm::Exit();
        tuneExit();
    }

    std::unique_ptr<tsl::Gui> loadInitialGui() override {
        if (this->msg) {
            return std::make_unique<ErrorGui>(this->msg, this->fail);
        } else {
            return std::make_unique<MainGui>();
        }
    }
};

int main(int argc, char **argv) {
    return tsl::loop<SysTuneOverlay>(argc, argv);
}
