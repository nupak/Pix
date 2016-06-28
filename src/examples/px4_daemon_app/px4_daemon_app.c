#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <px4_config.h>
#include <nuttx/sched.h>
#include <systemlib/systemlib.h>
#include <systemlib/err.h>
#include <px4_config.h>
#include <nuttx/sched.h>
#include <systemlib/systemlib.h>
#include <systemlib/err.h>
#include <uORB/topics/sensor_accel.h>

static bool thread_should_exit = false;
static bool thread_running = false;
static int daemon_task;

__EXPORT int px4_daemon_app_main(int argc, char *argv[]);
int px4_daemon_thread_main(int argc, char *argv[]);
static void usage(const char *reason);
//_____________________________________________________
static void
usage(const char *reason)
{
    if (reason) {
        warnx("%s\n", reason);
    }

    warnx("usage: daemon {start|stop|status} [-p <additional params>]\n\n");
}
//______________________________________________________________________
int px4_daemon_app_main(int argc, char *argv[])
{
    if (argc < 2) {
        usage("missing command");
        return 1;
    }

    if (!strcmp(argv[1], "start")) {

        if (thread_running) {
            warnx("daemon already running\n");
            return 0;
        }

        thread_should_exit = false;
        daemon_task = px4_task_spawn_cmd("daemon",
                         SCHED_DEFAULT,
                         SCHED_PRIORITY_DEFAULT,
                         2000,
                         px4_daemon_thread_main,
                         (argv) ? (char *const *)&argv[2] : (char *const *)NULL);
        return 0;
    }

    if (!strcmp(argv[1], "stop")) {
        thread_should_exit = true;
        return 0;
    }

    if (!strcmp(argv[1], "status")) {
        if (thread_running) {
            warnx("\trunning\n");

        } else {
            warnx("\tnot started\n");
        }

        return 0;
    }

    usage("unrecognized command");
    return 1;
}

int px4_daemon_thread_main(int argc, char *argv[])
{

    warnx("[daemon] starting\n");

    thread_running = true;

    while (!thread_should_exit) {
    int sensor_sub_fd = orb_subscribe(ORB_ID(sensor_combined));
    px4_pollfd_struct_t fds[] = {
        { .fd = sensor_sub_fd,   .events = POLLIN },
    };

    return 0;
}
