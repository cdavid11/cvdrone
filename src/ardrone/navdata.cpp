// -------------------------------------------------------------------------
// CV Drone (= OpenCV + AR.Drone)
// Copyright(C) 2013 puku0x
// https://github.com/puku0x/cvdrone
//
// This source file is part of CV Drone library.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of EITHER:
// (1) The GNU Lesser General Public License as published by the Free
//     Software Foundation; either version 2.1 of the License, or (at
//     your option) any later version. The text of the GNU Lesser
//     General Public License is included with this library in the
//     file cvdrone-license-LGPL.txt.
// (2) The BSD-style license that is included with this library in
//     the file cvdrone-license-BSD.txt.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files
// cvdrone-license-LGPL.txt and cvdrone-license-BSD.txt for more details.
// -------------------------------------------------------------------------

#include "ardrone.h"

// --------------------------------------------------------------------------
// ARDrone::initNavdata()
// Description  : Initialize Navdata.
// Return value : SUCCESS: 1  FAILURE: 0 
// --------------------------------------------------------------------------
int ARDrone::initNavdata(void)
{
    // Open the IP address and port
    if (!sockNavdata.open(ip, ARDRONE_NAVDATA_PORT)) {
        CVDRONE_ERROR("UDPSocket::open(port=%d) was failed. (%s, %d)\n", ARDRONE_NAVDATA_PORT, __FILE__, __LINE__);
        return 0;
    }

    // Clear Navdata
    memset(&navdata, 0, sizeof(ARDRONE_NAVDATA));

    // Start Navdata
    sockNavdata.sendf("\x01\x00\x00\x00");

    // AR.Drone 2.0
    if (version.major == ARDRONE_VERSION_2) {
        // Disable BOOTSTRAP mode
        if (mutexCommand) pthread_mutex_lock(mutexCommand);
        sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
        sockCommand.sendf("AT*CONFIG=%d,\"general:navdata_demo\",\"TRUE\"\r", seq++);
        if (mutexCommand) pthread_mutex_unlock(mutexCommand);
        msleep(100);

        // Seed ACK
        sockCommand.sendf("AT*CTRL=%d,0\r", seq++);
    }
    // AR.Drone 1.0
    else {
        // Disable BOOTSTRAP mode
        if (mutexCommand) pthread_mutex_lock(mutexCommand);
        sockCommand.sendf("AT*CONFIG=%d,\"general:navdata_demo\",\"TRUE\"\r", seq++);
        if (mutexCommand) pthread_mutex_unlock(mutexCommand);

        // Send ACK
        sockCommand.sendf("AT*CTRL=%d,0\r", seq++);
    }

    // Create a mutex
    mutexNavdata = new pthread_mutex_t;
    pthread_mutex_init(mutexNavdata, NULL);

    // Create a thread
    threadNavdata = new pthread_t;
    if (pthread_create(threadNavdata, NULL, runNavdata, this) != 0) {
        CVDRONE_ERROR("pthread_create() was failed. (%s, %d)\n", __FILE__, __LINE__);
        return 0;
    }

    return 1;
}

// --------------------------------------------------------------------------
// ARDrone::loopNavdata()
// Description  : Thread function for Navdata.
// Return value : SUCCESS:0
// --------------------------------------------------------------------------
void ARDrone::loopNavdata(void)
{
    while (1) {
        // Get Navdata
        if (!getNavdata()) break;
        pthread_testcancel();
        msleep(60);
    }
}

// --------------------------------------------------------------------------
// ARDrone::getNavdata()
// Description  : Get current navigation data of AR.Drone.
// Return value : SUCCESS: 1  FAILURE: 0
// --------------------------------------------------------------------------
int ARDrone::getNavdata(void)
{
    // Send a request
    sockNavdata.sendf("\x01\x00\x00\x00");

    // Receive data
    int buf[512] = {'\0'};
    int size = sockNavdata.receive((void*)&buf, sizeof(buf));

    // Received something
    if (size > 0) {
        // Check header
        if (buf[0] == ARDRONE_NAVDATA_HEADER) {
            // Update Navdata
            if (mutexNavdata) pthread_mutex_lock(mutexNavdata);
            memcpy((void*)&navdata, (const void*)buf, sizeof(ARDRONE_NAVDATA));
            if (mutexNavdata) pthread_mutex_unlock(mutexNavdata);
        }
    }

    return 1;
}

// --------------------------------------------------------------------------
// ARDrone::getRoll()
// Description  : Get current role angle of AR.Drone.
// Return value : Role angle [rad]
// --------------------------------------------------------------------------
double ARDrone::getRoll(void)
{
    // Get the data
    if (mutexNavdata) pthread_mutex_lock(mutexNavdata);
    double roll = navdata.demo.phi * 0.001 * DEG_TO_RAD;
    if (mutexNavdata) pthread_mutex_unlock(mutexNavdata);

    return roll;
}

// --------------------------------------------------------------------------
// ARDrone::getPitch()
// Description  : Get current pitch angle of AR.Drone.
// Return value : Pitch angle [rad]
// --------------------------------------------------------------------------
double ARDrone::getPitch(void)
{
    // Get the data
    if (mutexNavdata) pthread_mutex_lock(mutexNavdata);
    double pitch = -navdata.demo.theta * 0.001 * DEG_TO_RAD;
    if (mutexNavdata) pthread_mutex_unlock(mutexNavdata);

    return pitch;
}

// --------------------------------------------------------------------------
// ARDrone::getYaw()
// Description  : Get current yaw angle of AR.Drone.
// Return value : Yaw angle [rad]
// --------------------------------------------------------------------------
double ARDrone::getYaw(void)
{
    // Get the data
    if (mutexNavdata) pthread_mutex_lock(mutexNavdata);
    double yaw = -navdata.demo.psi * 0.001 * DEG_TO_RAD;
    if (mutexNavdata) pthread_mutex_unlock(mutexNavdata);

    return yaw;
}

// --------------------------------------------------------------------------
// ARDrone::getAltitude()
// Description  : Get current altitude of AR.Drone.
// Return value : Altitude [m]
// --------------------------------------------------------------------------
double ARDrone::getAltitude(void)
{
    // Get the data
    if (mutexNavdata) pthread_mutex_lock(mutexNavdata);
    double altitude = navdata.demo.altitude * 0.001;
    if (mutexNavdata) pthread_mutex_unlock(mutexNavdata);

    return altitude;
}

// --------------------------------------------------------------------------
// ARDrone::getVelocity(X velocity[m/s], Y velocity[m/s], Z velocity[m/s])
// Description  : Get estimated velocity of AR.Drone.
// Return value : Velocity [m/s]
// --------------------------------------------------------------------------
double ARDrone::getVelocity(double *vx, double *vy, double *vz)
{
    // Get the data
    if (mutexNavdata) pthread_mutex_lock(mutexNavdata);
    double velocity_x =  navdata.demo.vx * 0.001;
    double velocity_y = -navdata.demo.vy * 0.001;
    double velocity_z = -navdata.demo.vz * 0.001;
    double velocity = sqrt(velocity_x*velocity_x + velocity_y*velocity_y + velocity_z*velocity_z);
    if (mutexNavdata) pthread_mutex_unlock(mutexNavdata);

    // Velocities
    if (vx) *vx = velocity_x;
    if (vy) *vy = velocity_y;
    if (vz) *vz = velocity_z;

    return velocity;
}

// --------------------------------------------------------------------------
// ARDrone::getBatteryPercentage()
// Description  : Get current battery percentage of AR.Drone.
// Return value : Battery percentage [%]
// --------------------------------------------------------------------------
int ARDrone::getBatteryPercentage(void)
{
    // Get the data
    if (mutexNavdata) pthread_mutex_lock(mutexNavdata);
    int battery = navdata.demo.vbat_flying_percentage;
    if (mutexNavdata) pthread_mutex_unlock(mutexNavdata);

    return battery;
}

// --------------------------------------------------------------------------
// ARDrone::onGround()
// Description  : Check whether AR.Drone is on ground.
// Return value : YES:1 NO:0
// --------------------------------------------------------------------------
int ARDrone::onGround(void)
{
    // Get the data
    if (mutexNavdata) pthread_mutex_lock(mutexNavdata);
    int on_ground = (navdata.ardrone_state & ARDRONE_FLY_MASK) ? 0 : 1;
    if (mutexNavdata) pthread_mutex_unlock(mutexNavdata);

    return on_ground;
}

// --------------------------------------------------------------------------
// ARDrone::finalizeNavdata()
// Description  : Finalize Navdata.
// Return value : NONE
// --------------------------------------------------------------------------
void ARDrone::finalizeNavdata(void)
{
    // Destroy the thread
    if (threadNavdata) {
        pthread_cancel(*threadNavdata);
        pthread_join(*threadNavdata, NULL);
        delete threadNavdata;
        threadNavdata = NULL;
    }

    // Delete the mutex
    if (mutexNavdata) {
        pthread_mutex_destroy(mutexNavdata);
        delete mutexNavdata;
        mutexNavdata = NULL;
    }

    // Close the socket
    sockNavdata.close();
}