#include "pm.hpp"

namespace {

constexpr u64 QLAUNCH_TITLE_ID{0x0100000000001000ULL};
u64 CURRENT_TITLE_ID{};

}

namespace pm {

auto Initialize() -> Result {
    auto rc = pmdmntInitialize();
    if (R_FAILED(rc)) {
        return rc;
    }

    return pminfoInitialize();
}

void Exit() {
    pminfoExit();
    pmdmntExit();
}

// SOURCE: https://github.com/retronx-team/sys-clk/blob/570f1e5fe10b253eff0c8fda1bb893bb620af052/sysmodule/src/process_management.cpp#L37
void getCurrentPidTid(u64* pid_out, u64* tid_out) {
    Result rc{};
    if (R_SUCCEEDED(rc = pmdmntGetApplicationProcessId(pid_out))) {
        if (0x20f == pminfoGetProgramId(tid_out, *pid_out)){
            *tid_out = QLAUNCH_TITLE_ID;
        }
    } else if (rc == 0x20f) {
        *tid_out = QLAUNCH_TITLE_ID;
    } else {
        *tid_out = CURRENT_TITLE_ID;
    }
}

auto PollCurrentPidTid(u64* pid_out, u64* tid_out) -> bool {
    getCurrentPidTid(pid_out, tid_out);

    if (*tid_out != CURRENT_TITLE_ID) {
        CURRENT_TITLE_ID = *tid_out;
        return true;
    }

    return false;
}

// sys-tune-enhanced does not appear to play nice in any way when changed while suspended.
// This method can be used in the while in the error screen to determine whether settings may be safely changed...
auto systuneIsRunning() -> bool {
    u64 pid = 0;
    const u64 &module_pid = 0x4200000000000000ULL;
    if (R_FAILED(pmdmntGetProcessId(&pid, module_pid)))
        return false;

    return pid > 0;
}

}
