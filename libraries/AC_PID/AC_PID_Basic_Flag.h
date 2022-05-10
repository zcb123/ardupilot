#pragma once

/// @file	AC_PID_Basic.h
/// @brief	Generic PID algorithm, with EEPROM-backed storage of constants.

#include <AP_Common/AP_Common.h>
#include <AP_Param/AP_Param.h>
#include <AP_Logger/AP_Logger.h>
#include "AC_PID_Basic.h"
/// @class	AC_PID_Basic
/// @brief	Copter PID control class
class AC_PID_Basic_Flag : public AC_PID_Basic {
public:
    AC_PID_Basic_Flag(float initial_p, float initial_i, float initial_d, float initial_ff, float initial_imax, float initial_filt_E_hz, float initial_filt_D_hz, float dt);

    static const struct AP_Param::GroupInfo var_info[];

    float update(float target, float measurement,float flag) WARN_IF_UNUSED;

protected:
    float _error;       // time step in seconds
    float _error_min; // error limit in negative direction
    float _error_max; // error limit in positive direction
    AP_Float _D1_max;      // maximum first derivative of output
};
