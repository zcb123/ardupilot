/// @file	AC_PID_Basic.cpp
/// @brief	Generic PID algorithm

#include <AP_Math/AP_Math.h>
#include <AP_InternalError/AP_InternalError.h>
#include "AC_PID_Basic_Flag.h"

#define AC_PID_Basic_Flag_FILT_E_HZ_DEFAULT 20.0f   // default input filter frequency
#define AC_PID_Basic_Flag_FILT_E_HZ_MIN     0.01f   // minimum input filter frequency
#define AC_PID_Basic_Flag_FILT_D_HZ_DEFAULT 10.0f   // default input filter frequency
#define AC_PID_Basic_Flag_FILT_D_HZ_MIN     0.005f  // minimum input filter frequency

const AP_Param::GroupInfo AC_PID_Basic_Flag::var_info[] = {
    // @Param: P
    // @DisplayName: PID Proportional Gain
    // @Description: P Gain which produces an output value that is proportional to the current error value
    AP_GROUPINFO("P",    0, AC_PID_Basic_Flag, _kp, 0),

    // @Param: I
    // @DisplayName: PID Integral Gain
    // @Description: I Gain which produces an output that is proportional to both the magnitude and the duration of the error
    AP_GROUPINFO("I",    1, AC_PID_Basic_Flag, _ki, 0),

    // @Param: IMAX
    // @DisplayName: PID Integral Maximum
    // @Description: The maximum/minimum value that the I term can output
    AP_GROUPINFO("IMAX", 2, AC_PID_Basic_Flag, _kimax, 0),

    // @Param: FLTE
    // @DisplayName: PID Error filter frequency in Hz
    // @Description: Error filter frequency in Hz
    // @Units: Hz
    AP_GROUPINFO("FLTE", 3, AC_PID_Basic_Flag, _filt_E_hz, AC_PID_Basic_Flag_FILT_E_HZ_DEFAULT),

    // @Param: D
    // @DisplayName: PID Derivative Gain
    // @Description: D Gain which produces an output that is proportional to the rate of change of the error
    AP_GROUPINFO("D",    4, AC_PID_Basic_Flag, _kd, 0),

    // @Param: FLTD
    // @DisplayName: D term filter frequency in Hz
    // @Description: D term filter frequency in Hz
    // @Units: Hz
    AP_GROUPINFO("FLTD", 5, AC_PID_Basic_Flag, _filt_D_hz, AC_PID_Basic_Flag_FILT_D_HZ_DEFAULT),

    // @Param: FF
    // @DisplayName: PID Feed Forward Gain
    // @Description: FF Gain which produces an output that is proportional to the magnitude of the target
    AP_GROUPINFO("FF",   6, AC_PID_Basic_Flag, _kff, 0),

    // @Param: D1_MAX
    // @DisplayName: 
    // @Description: 
    AP_GROUPINFO("D1_MAX",   7, AC_PID_Basic_Flag, _D1_max, 5.0f),
    
    AP_GROUPEND
};
AC_PID_Basic_Flag::AC_PID_Basic_Flag(float initial_p, float initial_i, float initial_d, float initial_ff, float initial_imax, float initial_filt_E_hz, float initial_filt_D_hz, float dt):
   AC_PID_Basic(initial_p,initial_i,initial_d,initial_ff,initial_imax,initial_filt_E_hz,initial_filt_D_hz,dt) 
{
    // load parameter values from eeprom
    AP_Param::setup_object_defaults(this, var_info);
   
}

float AC_PID_Basic_Flag::update(float target, float measurement,bool &limit_min,bool &limit_max,float flag)
{

    limit_min = false;
    limit_max = false;
    float res = 0.0;
    // calculate distance _error
    _error = target - measurement;

    if (is_negative(_error_min) && (_error < _error_min)) {
        _error = _error_min;
        target = measurement + _error;
        limit_min = true;
    } else if (is_positive(_error_max) && (_error > _error_max)) {
        _error = _error_max;
        target = measurement + _error;
        limit_max = true;
    }

    // MIN(_Dxy_max, _D2xy_max / _kxy_P) limits the max accel to the point where max jerk is exceeded
    // if(is_zero(flag)){
        res = update_all(target,measurement); 
        
        // AP::logger().Write("PIDF","TimeUs,FLAG,BACH","Qff",AP_HAL::micros64(),flag,1.1f);
    // }
    // else{
    //     res = sqrt_controller(_error, _kp, _D1_max.get(), _dt);
    //     AP::logger().Write("PIDF","TimeUs,FLAG,BACH","Qff",AP_HAL::micros64(),flag,2.1f);
    // }
    return res;

}