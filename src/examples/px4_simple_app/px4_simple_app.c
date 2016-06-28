#include <px4_config.h>
#include <px4_tasks.h>
#include <px4_posix.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <string.h>
#include<math.h>
#include <uORB/topics/sensor_accel.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/uORB.h>
#include <uORB/topics/sensor_combined.h>

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
    int sensor_sub_fd = orb_subscribe(ORB_ID(sensor_combined));
    px4_pollfd_struct_t fds[] = {
            { .fd = sensor_sub_fd,   .events = POLLIN }
        };
    struct sensor_accel_s raw;
    while (!thread_should_exit) {

    int error_counter = 0;


        int poll_ret = px4_poll(fds, 1, 1000);


        if (poll_ret == 0) {

            PX4_ERR("[px4_simple_app] Got no data within a second");

        } else if (poll_ret < 0) {

            if (error_counter < 10 || error_counter % 50 == 0) {

                PX4_ERR("[px4_simple_app] ERROR return value from poll(): %d"
                       , poll_ret);
            }

            error_counter++;

        } else
        {

            if (fds[0].revents & POLLIN) {
                orb_copy(ORB_ID(sensor_accel), sensor_sub_fd, &raw);
                double a1,a2,osx,osy,osz;
        osx=raw.x; osy=raw.y; osz=raw.z;
                a1=atan2(osx,sqrt(osy*osy+osz*osz));
        a2=atan2(osy,sqrt(osx*osx+osz*osz));
        PX4_WARN("[px4_simple_app] a1\t%8.4f a2\t%8.4f",
                              a1,a2);
       }
    }
}
return 0;
}
