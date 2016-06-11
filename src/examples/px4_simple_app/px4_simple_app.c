/****************************************************************************
 *
 *   Copyright (c) 2012-2015 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file px4_simple_app.c
 * Minimal application example for PX4 autopilot
 *
 * @author Example User <mail@example.com>
 */
#include <px4_config.h>
#include <px4_tasks.h>
#include <px4_posix.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <string.h>
#include<math.h>
#include <uORB/uORB.h>
#include <uORB/topics/sensor_accel.h>
#include <uORB/topics/vehicle_attitude.h>

__EXPORT int px4_simple_app_main(int argc, char *argv[]);
int px4_simple_app_main(int argc, char *argv[])
{
    PX4_INFO("Let's start");
    char c;
    int sensor_sub_fd = orb_subscribe(ORB_ID(sensor_accel));
    orb_set_interval(sensor_sub_fd, 1000);
while ((c = getchar()) != 'q'){ //обязательно через аргументы командной строки? или это тоже вариант?

                struct sensor_accel_s raw;

                orb_copy(ORB_ID(sensor_accel), sensor_sub_fd, &raw);
                double a1,a2,osx,osy,osz;

                 osx=raw.x; osy=raw.y; osz=raw.z;
                 if (osx<0)
                      if (osz>=0) a1=30;
                      else a1 =-30;
                 else  a1=atan(osz/osx);
                      if (a1>30) a1=30;//если угол
                      else             //вне диапазона [-30;30]
                        if (a1<-30) a1=-30;
                 // ///////////////////////////////////////////////
                 if (osy<0)
                      if (osz>0) a2=30;
                       else a2=-30;
                 else  a2=atan(osz/osy);
                    if (a2>30) a2=30;             //если угол
                         else  if (a2<-30) a2=-30;//вне диапазона [-30;30]

                PX4_WARN("[px4_simple_app] Accelerometer:\t%8.4f\t%8.4f",
                      a1,a2);}
    return 0;
}
