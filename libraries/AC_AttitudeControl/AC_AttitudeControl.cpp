#include "AC_AttitudeControl.h"
#include <AP_HAL/AP_HAL.h>

extern const AP_HAL::HAL& hal;

#if APM_BUILD_TYPE(APM_BUILD_ArduPlane)
 // default gains for Plane
 # define AC_ATTITUDE_CONTROL_INPUT_TC_DEFAULT  0.2f    // Soft
#else
 // default gains for Copter and Sub
 # define AC_ATTITUDE_CONTROL_INPUT_TC_DEFAULT  0.15f   // Medium
#endif

// table of user settable parameters
const AP_Param::GroupInfo AC_AttitudeControl::var_info[] = {

    // 0, 1 were RATE_RP_MAX, RATE_Y_MAX

    // @Param: SLEW_YAW
    // @DisplayName: Yaw target slew rate
    // @Description: Maximum rate the yaw target can be updated in Loiter, RTL, Auto flight modes
    // @Units: cdeg/s
    // @Range: 500 18000
    // @Increment: 100
    // @User: Advanced
    AP_GROUPINFO("SLEW_YAW", 2, AC_AttitudeControl, _slew_yaw, AC_ATTITUDE_CONTROL_SLEW_YAW_DEFAULT_CDS),

    // 3 was for ACCEL_RP_MAX

    // @Param: ACCEL_Y_MAX
    // @DisplayName: Acceleration Max for Yaw
    // @Description: Maximum acceleration in yaw axis
    // @Units: cdeg/s/s
    // @Range: 0 72000
    // @Values: 0:Disabled, 9000:VerySlow, 18000:Slow, 36000:Medium, 54000:Fast
    // @Increment: 1000
    // @User: Advanced
    AP_GROUPINFO("ACCEL_Y_MAX", 4, AC_AttitudeControl, _accel_yaw_max, AC_ATTITUDE_CONTROL_ACCEL_Y_MAX_DEFAULT_CDSS),

    // @Param: RATE_FF_ENAB
    // @DisplayName: Rate Feedforward Enable
    // @Description: Controls whether body-frame rate feedfoward is enabled or disabled
    // @Values: 0:Disabled, 1:Enabled
    // @User: Advanced
    AP_GROUPINFO("RATE_FF_ENAB", 5, AC_AttitudeControl, _rate_bf_ff_enabled, AC_ATTITUDE_CONTROL_RATE_BF_FF_DEFAULT),

    // @Param: ACCEL_R_MAX
    // @DisplayName: Acceleration Max for Roll
    // @Description: Maximum acceleration in roll axis
    // @Units: cdeg/s/s
    // @Range: 0 180000
    // @Increment: 1000
    // @Values: 0:Disabled, 30000:VerySlow, 72000:Slow, 108000:Medium, 162000:Fast
    // @User: Advanced
    AP_GROUPINFO("ACCEL_R_MAX", 6, AC_AttitudeControl, _accel_roll_max, AC_ATTITUDE_CONTROL_ACCEL_RP_MAX_DEFAULT_CDSS),

    // @Param: ACCEL_P_MAX
    // @DisplayName: Acceleration Max for Pitch
    // @Description: Maximum acceleration in pitch axis
    // @Units: cdeg/s/s
    // @Range: 0 180000
    // @Increment: 1000
    // @Values: 0:Disabled, 30000:VerySlow, 72000:Slow, 108000:Medium, 162000:Fast
    // @User: Advanced
    AP_GROUPINFO("ACCEL_P_MAX", 7, AC_AttitudeControl, _accel_pitch_max, AC_ATTITUDE_CONTROL_ACCEL_RP_MAX_DEFAULT_CDSS),

    // IDs 8,9,10,11 RESERVED (in use on Solo)

    // @Param: ANGLE_BOOST
    // @DisplayName: Angle Boost
    // @Description: Angle Boost increases output throttle as the vehicle leans to reduce loss of altitude
    // @Values: 0:Disabled, 1:Enabled
    // @User: Advanced
    AP_GROUPINFO("ANGLE_BOOST", 12, AC_AttitudeControl, _angle_boost_enabled, 1),

    // @Param: ANG_RLL_P
    // @DisplayName: Roll axis angle controller P gain
    // @Description: Roll axis angle controller P gain.  Converts the error between the desired roll angle and actual angle to a desired roll rate
    // @Range: 3.000 12.000
    // @Range{Sub}: 0.0 12.000
    // @User: Standard
    AP_SUBGROUPINFO(_p_angle_roll, "ANG_RLL_", 13, AC_AttitudeControl, AC_P),

    // @Param: ANG_PIT_P
    // @DisplayName: Pitch axis angle controller P gain
    // @Description: Pitch axis angle controller P gain.  Converts the error between the desired pitch angle and actual angle to a desired pitch rate
    // @Range: 3.000 12.000
    // @Range{Sub}: 0.0 12.000
    // @User: Standard
    AP_SUBGROUPINFO(_p_angle_pitch, "ANG_PIT_", 14, AC_AttitudeControl, AC_P),

    // @Param: ANG_YAW_P
    // @DisplayName: Yaw axis angle controller P gain
    // @Description: Yaw axis angle controller P gain.  Converts the error between the desired yaw angle and actual angle to a desired yaw rate
    // @Range: 3.000 12.000
    // @Range{Sub}: 0.0 6.000
    // @User: Standard
    AP_SUBGROUPINFO(_p_angle_yaw, "ANG_YAW_", 15, AC_AttitudeControl, AC_P),

    // @Param: ANG_LIM_TC
    // @DisplayName: Angle Limit (to maintain altitude) Time Constant
    // @Description: Angle Limit (to maintain altitude) Time Constant
    // @Range: 0.5 10.0
    // @User: Advanced
    AP_GROUPINFO("ANG_LIM_TC", 16, AC_AttitudeControl, _angle_limit_tc, AC_ATTITUDE_CONTROL_ANGLE_LIMIT_TC_DEFAULT),

    // @Param: RATE_R_MAX
    // @DisplayName: Angular Velocity Max for Roll
    // @Description: Maximum angular velocity in roll axis
    // @Units: deg/s
    // @Range: 0 1080
    // @Increment: 1
    // @Values: 0:Disabled, 360:Slow, 720:Medium, 1080:Fast
    // @User: Advanced
    AP_GROUPINFO("RATE_R_MAX", 17, AC_AttitudeControl, _ang_vel_roll_max, 0.0f),

    // @Param: RATE_P_MAX
    // @DisplayName: Angular Velocity Max for Pitch
    // @Description: Maximum angular velocity in pitch axis
    // @Units: deg/s
    // @Range: 0 1080
    // @Increment: 1
    // @Values: 0:Disabled, 360:Slow, 720:Medium, 1080:Fast
    // @User: Advanced
    AP_GROUPINFO("RATE_P_MAX", 18, AC_AttitudeControl, _ang_vel_pitch_max, 0.0f),

    // @Param: RATE_Y_MAX
    // @DisplayName: Angular Velocity Max for Yaw
    // @Description: Maximum angular velocity in yaw axis
    // @Units: deg/s
    // @Range: 0 1080
    // @Increment: 1
    // @Values: 0:Disabled, 360:Slow, 720:Medium, 1080:Fast
    // @User: Advanced
    AP_GROUPINFO("RATE_Y_MAX", 19, AC_AttitudeControl, _ang_vel_yaw_max, 0.0f),

    // @Param: INPUT_TC
    // @DisplayName: Attitude control input time constant
    // @Description: Attitude control input time constant.  Low numbers lead to sharper response, higher numbers to softer response
    // @Units: s
    // @Range: 0 1
    // @Increment: 0.01
    // @Values: 0.5:Very Soft, 0.2:Soft, 0.15:Medium, 0.1:Crisp, 0.05:Very Crisp
    // @User: Standard
    AP_GROUPINFO("INPUT_TC", 20, AC_AttitudeControl, _input_tc, AC_ATTITUDE_CONTROL_INPUT_TC_DEFAULT),

    AP_GROUPEND
};

// Ensure attitude controller have zero errors to relax rate controller output
// 确保姿态控制器具有零误差以释放速率控制器输出
/* 控制器初始化 */
void AC_AttitudeControl::relax_attitude_controllers()
{
    // Initialize the attitude variables to the current attitude
    /* 将目标姿态设置成当前姿态 */
    _ahrs.get_quat_body_to_ned(_attitude_target);
    _attitude_target.to_euler(_euler_angle_target.x, _euler_angle_target.y, _euler_angle_target.z);
    _attitude_ang_error.initialise();

    // Initialize the angular rate variables to the current rate
    _ang_vel_target = _ahrs.get_gyro();
    ang_vel_to_euler_rate(_euler_angle_target, _ang_vel_target, _euler_rate_target);
    _ang_vel_body = _ahrs.get_gyro();

    // Initialize remaining variables
    _thrust_error_angle = 0.0f;

    // Reset the PID filters
    get_rate_roll_pid().reset_filter();
    get_rate_pitch_pid().reset_filter();
    get_rate_yaw_pid().reset_filter();

    // Reset the I terms
    reset_rate_controller_I_terms();
}

void AC_AttitudeControl::reset_rate_controller_I_terms()
{
    get_rate_roll_pid().reset_I();
    get_rate_pitch_pid().reset_I();
    get_rate_yaw_pid().reset_I();
}

// reset rate controller I terms smoothly to zero in 0.5 seconds
void AC_AttitudeControl::reset_rate_controller_I_terms_smoothly()
{
    get_rate_roll_pid().reset_I_smoothly();
    get_rate_pitch_pid().reset_I_smoothly();
    get_rate_yaw_pid().reset_I_smoothly();
}

// The attitude controller works around the concept of the desired attitude, target attitude
// and measured attitude. The desired attitude is the attitude input into the attitude controller
// that expresses where the higher level code would like the aircraft to move to. The target attitude is moved
// to the desired attitude with jerk, acceleration, and velocity limits. The target angular velocities are fed
// directly into the rate controllers. The angular error between the measured attitude and the target attitude is
// fed into the angle controller and the output of the angle controller summed at the input of the rate controllers.
// By feeding the target angular velocity directly into the rate controllers the measured and target attitudes
// remain very close together.
// 姿态控制器由期望姿态，目标姿态，与测量姿态组成。期望的姿态是姿态控制器的姿态输入，表示高层代码希望飞机移动到的位置。
// 目标姿态移动到具有加加速度、加速度和速度限制的期望姿态(经过限幅处理的输入期望姿态)。目标角速度直接输入到速率控制器中。
// 测量姿态与目标姿态之间的误差输入到角度控制器 并且 角度控制器输出与角速率控制器输入相加。通过将目标角速度直接输入到速率控制器，
// 使得测量值和目标姿态值之间保持地十分接近
// 
// All input functions below follow the same procedure
// 1. define the desired attitude the aircraft should attempt to achieve using the input variables
// 2. using the desired attitude and input variables, define the target angular velocity so that it should
//    move the target attitude towards the desired attitude
// 3. if _rate_bf_ff_enabled is not being used then make the target attitude
//    and target angular velocities equal to the desired attitude and desired angular velocities.
// 4. ensure _attitude_target, _euler_angle_target, _euler_rate_target and
//    _ang_vel_target have been defined. This ensures input modes can be changed without discontinuity.
// 5. attitude_controller_run_quat is then run to pass the target angular velocities to the rate controllers and
//    integrate them into the target attitude. Any errors between the target attitude and the measured attitude are
//    corrected by first correcting the thrust vector until the angle between the target thrust vector measured
//    trust vector drops below 2*AC_ATTITUDE_THRUST_ERROR_ANGLE. At this point the heading is also corrected.
// 下面所有的输入函数都遵循一样的流程
// 1. 使用输入变量定义飞行器应当达到的期望姿态
// 2. 使用期望姿态和输入变量定义目标角速度。以便从目标姿态运动到期望姿态。
// 3. 如果 _rate_bf_ff_enabled 没有使用，则令目标姿态和目标角速率等于期望姿态和期望角速度
// 4. 确保 _attitude_target, _euler_angle_target, _euler_rate_target and _ang_vel_target 已经定义了。
//    这保证了输入模式可以不连续地改变
// 5. attitude_controller_run_quat()函数随后运行。将目标角速度传递到速率控制器并且将其集成到目标姿态。
//    所有目标姿态与测量姿态的误差都被第一个校正推力向量所校正。直到目标推力向量与测量推力向量之间的角度下降到2*AC_ATTITUDE_THRUST_ERROR_ANGLE以下。
//    达到这点，航向也被纠正了

// Command a Quaternion attitude with feedforward and smoothing
// 通过前馈和平滑控制四元数姿态
/* 一般情况下不用 */
void AC_AttitudeControl::input_quaternion(Quaternion attitude_desired_quat)
{
#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
    // this function is not currently used, this is a reminder that we need to add a 6DoF implementation
    AP_HAL::panic("input_quaternion not implemented AC_AttitudeControl_Multi_6DoF");
#endif

    // calculate the attitude target euler angles
    _attitude_target.to_euler(_euler_angle_target.x, _euler_angle_target.y, _euler_angle_target.z);

    Quaternion attitude_error_quat = _attitude_target.inverse() * attitude_desired_quat;
    Vector3f attitude_error_angle;
    attitude_error_quat.to_axis_angle(attitude_error_angle);

    if (_rate_bf_ff_enabled) {
        // When acceleration limiting and feedforward are enabled, the sqrt controller is used to compute an euler
        // angular velocity that will cause the euler angle to smoothly stop at the input angle with limited deceleration
        // and an exponential decay specified by _input_tc at the end.
        _ang_vel_target.x = input_shaping_angle(wrap_PI(attitude_error_angle.x), _input_tc, get_accel_roll_max_radss(), _ang_vel_target.x, _dt);
        _ang_vel_target.y = input_shaping_angle(wrap_PI(attitude_error_angle.y), _input_tc, get_accel_pitch_max_radss(), _ang_vel_target.y, _dt);
        _ang_vel_target.z = input_shaping_angle(wrap_PI(attitude_error_angle.z), _input_tc, get_accel_yaw_max_radss(), _ang_vel_target.z, _dt);

        // Limit the angular velocity
        ang_vel_limit(_ang_vel_target, radians(_ang_vel_roll_max), radians(_ang_vel_pitch_max), radians(_ang_vel_yaw_max));
        // Convert body-frame angular velocity into euler angle derivative of desired attitude
        ang_vel_to_euler_rate(_euler_angle_target, _ang_vel_target, _euler_rate_target);
    } else {
        _attitude_target = attitude_desired_quat;

        // Set rate feedforward requests to zero
        _euler_rate_target.zero();
        _ang_vel_target.zero();
    }

    // Call quaternion attitude controller
    attitude_controller_run_quat();
}

// Command an euler roll and pitch angle and an euler yaw rate with angular velocity feedforward and smoothing
// input:target_roll
//       target_pitch
//       target_yaw_rate
void AC_AttitudeControl::input_euler_angle_roll_pitch_euler_rate_yaw(float euler_roll_angle_cd, float euler_pitch_angle_cd, float euler_yaw_rate_cds)
{
    // Convert from centidegrees on public interface to radians
    float euler_roll_angle = radians(euler_roll_angle_cd * 0.01f);
    float euler_pitch_angle = radians(euler_pitch_angle_cd * 0.01f);
    float euler_yaw_rate = radians(euler_yaw_rate_cds * 0.01f);

    // calculate the attitude target euler angles
    // 从四元数(_attitude_targeet)->欧拉角(_euler_angle_target)
    /* 上一时刻的欧拉角 */
    _attitude_target.to_euler(_euler_angle_target.x, _euler_angle_target.y, _euler_angle_target.z);

    // Add roll trim to compensate tail rotor thrust in heli (will return zero on multirotors)
    euler_roll_angle += get_roll_trim_rad();

    if (_rate_bf_ff_enabled) {
        // translate the roll pitch and yaw acceleration limits to the euler axis
        // 将横滚,俯仰,偏航加速度限制转化到欧拉轴
        const Vector3f euler_accel = euler_accel_limit(_euler_angle_target, Vector3f{get_accel_roll_max_radss(), get_accel_pitch_max_radss(), get_accel_yaw_max_radss()});

        // When acceleration limiting and feedforward are enabled, the sqrt controller is used to compute an euler
        // angular velocity that will cause the euler angle to smoothly stop at the input angle with limited deceleration
        // and an exponential decay specified by smoothing_gain at the end.
        // 当加速度限制与前馈被使能，平方根控制器则用于计算欧拉角速度
        // 使欧拉角平滑地停止在输入角度的角速度，并具有有限的减速度
        // 和最后由smoothing_gain指定的指数衰减
        /* 调用了平方根控制器，之后又根据最大加速度限制角速度输出 */
        _euler_rate_target.x = input_shaping_angle(wrap_PI(euler_roll_angle - _euler_angle_target.x), _input_tc, euler_accel.x, _euler_rate_target.x, _dt);
        _euler_rate_target.y = input_shaping_angle(wrap_PI(euler_pitch_angle - _euler_angle_target.y), _input_tc, euler_accel.y, _euler_rate_target.y, _dt);

        // When yaw acceleration limiting is enabled, the yaw input shaper constrains angular acceleration about the yaw axis, slewing
        // the output rate towards the input rate.
        // 根据欧拉角z轴加速度，限制z轴旋转角速度
        
        _euler_rate_target.z = input_shaping_ang_vel(_euler_rate_target.z, euler_yaw_rate, euler_accel.z, _dt);

        // Convert euler angle derivative of desired attitude into a body-frame angular velocity vector for feedforward
        // 将期望姿态的欧拉角微分转化到前馈用的机体坐标系下角速度向量
        /* 输入: _euler_angle_target ，_euler_rate_target
           输出: _ang_vel_target*/
        euler_rate_to_ang_vel(_euler_angle_target, _euler_rate_target, _ang_vel_target);
        // Limit the angular velocity
        ang_vel_limit(_ang_vel_target, radians(_ang_vel_roll_max), radians(_ang_vel_pitch_max), radians(_ang_vel_yaw_max));

        // Convert body-frame angular velocity into euler angle derivative of desired attitude

        /* 输入: _euler_angle_target _ang_vel_target
           输出: _euler_rate_target*/
        ang_vel_to_euler_rate(_euler_angle_target, _ang_vel_target, _euler_rate_target);
        /*最后输出经过角速度向量限幅后的目标欧拉角速率*/

    } else {
        // When feedforward is not enabled, the target euler angle is input into the target and the feedforward rate is zeroed.
        _euler_angle_target.x = euler_roll_angle;
        _euler_angle_target.y = euler_pitch_angle;
        _euler_angle_target.z += euler_yaw_rate * _dt;
        // Compute quaternion target attitude
        _attitude_target.from_euler(_euler_angle_target.x, _euler_angle_target.y, _euler_angle_target.z);

        // Set rate feedforward requests to zero
        _euler_rate_target.zero();
        _ang_vel_target.zero();
    }

    // Call quaternion attitude controller
    attitude_controller_run_quat();
}

// Command an euler roll, pitch and yaw angle with angular velocity feedforward and smoothing
// 通过角速度前馈和平滑控制横滚、俯仰和偏航角(欧拉角) command sth with sth
void AC_AttitudeControl::input_euler_angle_roll_pitch_yaw(float euler_roll_angle_cd, float euler_pitch_angle_cd, float euler_yaw_angle_cd, bool slew_yaw)
{
    // Convert from centidegrees on public interface to radians
    float euler_roll_angle = radians(euler_roll_angle_cd * 0.01f);
    float euler_pitch_angle = radians(euler_pitch_angle_cd * 0.01f);
    float euler_yaw_angle = radians(euler_yaw_angle_cd * 0.01f);

    // calculate the attitude target euler angles
    _attitude_target.to_euler(_euler_angle_target.x, _euler_angle_target.y, _euler_angle_target.z);

    // Add roll trim to compensate tail rotor thrust in heli (will return zero on multirotors)
    euler_roll_angle += get_roll_trim_rad();

    if (_rate_bf_ff_enabled) {
        // translate the roll pitch and yaw acceleration limits to the euler axis
        const Vector3f euler_accel = euler_accel_limit(_euler_angle_target, Vector3f{get_accel_roll_max_radss(), get_accel_pitch_max_radss(), get_accel_yaw_max_radss()});

        // When acceleration limiting and feedforward are enabled, the sqrt controller is used to compute an euler
        // angular velocity that will cause the euler angle to smoothly stop at the input angle with limited deceleration
        // and an exponential decay specified by _input_tc at the end.
        _euler_rate_target.x = input_shaping_angle(wrap_PI(euler_roll_angle - _euler_angle_target.x), _input_tc, euler_accel.x, _euler_rate_target.x, _dt);
        _euler_rate_target.y = input_shaping_angle(wrap_PI(euler_pitch_angle - _euler_angle_target.y), _input_tc, euler_accel.y, _euler_rate_target.y, _dt);
        _euler_rate_target.z = input_shaping_angle(wrap_PI(euler_yaw_angle - _euler_angle_target.z), _input_tc, euler_accel.z, _euler_rate_target.z, _dt);
        if (slew_yaw) {
            _euler_rate_target.z = constrain_float(_euler_rate_target.z, -get_slew_yaw_rads(), get_slew_yaw_rads());
        }

        // Convert euler angle derivative of desired attitude into a body-frame angular velocity vector for feedforward
        euler_rate_to_ang_vel(_euler_angle_target, _euler_rate_target, _ang_vel_target);
        // Limit the angular velocity
        ang_vel_limit(_ang_vel_target, radians(_ang_vel_roll_max), radians(_ang_vel_pitch_max), radians(_ang_vel_yaw_max));
        // Convert body-frame angular velocity into euler angle derivative of desired attitude
        ang_vel_to_euler_rate(_euler_angle_target, _ang_vel_target, _euler_rate_target);
    } else {
        // When feedforward is not enabled, the target euler angle is input into the target and the feedforward rate is zeroed.
        _euler_angle_target.x = euler_roll_angle;
        _euler_angle_target.y = euler_pitch_angle;
        if (slew_yaw) {
            // Compute constrained angle error
            float angle_error = constrain_float(wrap_PI(euler_yaw_angle - _euler_angle_target.z), -get_slew_yaw_rads() * _dt, get_slew_yaw_rads() * _dt);
            // Update attitude target from constrained angle error
            _euler_angle_target.z = wrap_PI(angle_error + _euler_angle_target.z);
        } else {
            _euler_angle_target.z = euler_yaw_angle;
        }
        // Compute quaternion target attitude
        _attitude_target.from_euler(_euler_angle_target.x, _euler_angle_target.y, _euler_angle_target.z);

        // Set rate feedforward requests to zero
        _euler_rate_target.zero();
        _ang_vel_target.zero();
    }

    // Call quaternion attitude controller
    attitude_controller_run_quat();
}

// Command an euler roll, pitch, and yaw rate with angular velocity feedforward and smoothing
// 通过角速度前馈和平滑输出欧拉横滚，俯仰，偏航角速率
void AC_AttitudeControl::input_euler_rate_roll_pitch_yaw(float euler_roll_rate_cds, float euler_pitch_rate_cds, float euler_yaw_rate_cds)
{
    // Convert from centidegrees on public interface to radians
    float euler_roll_rate = radians(euler_roll_rate_cds * 0.01f);
    float euler_pitch_rate = radians(euler_pitch_rate_cds * 0.01f);
    float euler_yaw_rate = radians(euler_yaw_rate_cds * 0.01f);

    // calculate the attitude target euler angles
    _attitude_target.to_euler(_euler_angle_target.x, _euler_angle_target.y, _euler_angle_target.z);

    if (_rate_bf_ff_enabled) {
        // translate the roll pitch and yaw acceleration limits to the euler axis
        const Vector3f euler_accel = euler_accel_limit(_euler_angle_target, Vector3f{get_accel_roll_max_radss(), get_accel_pitch_max_radss(), get_accel_yaw_max_radss()});

        // When acceleration limiting is enabled, the input shaper constrains angular acceleration, slewing
        // the output rate towards the input rate.
        _euler_rate_target.x = input_shaping_ang_vel(_euler_rate_target.x, euler_roll_rate, euler_accel.x, _dt);
        _euler_rate_target.y = input_shaping_ang_vel(_euler_rate_target.y, euler_pitch_rate, euler_accel.y, _dt);
        _euler_rate_target.z = input_shaping_ang_vel(_euler_rate_target.z, euler_yaw_rate, euler_accel.z, _dt);

        // Convert euler angle derivative of desired attitude into a body-frame angular velocity vector for feedforward
        euler_rate_to_ang_vel(_euler_angle_target, _euler_rate_target, _ang_vel_target);
    } else {
        // When feedforward is not enabled, the target euler angle is input into the target and the feedforward rate is zeroed.
        // Pitch angle is restricted to +- 85.0 degrees to avoid gimbal lock discontinuities.
        _euler_angle_target.x = wrap_PI(_euler_angle_target.x + euler_roll_rate * _dt);
        _euler_angle_target.y = constrain_float(_euler_angle_target.y + euler_pitch_rate * _dt, radians(-85.0f), radians(85.0f));
        _euler_angle_target.z = wrap_2PI(_euler_angle_target.z + euler_yaw_rate * _dt);

        // Set rate feedforward requests to zero
        _euler_rate_target.zero();
        _ang_vel_target.zero();

        // Compute quaternion target attitude
        _attitude_target.from_euler(_euler_angle_target.x, _euler_angle_target.y, _euler_angle_target.z);
    }

    // Call quaternion attitude controller
    attitude_controller_run_quat();
}

// Command an angular velocity with angular velocity feedforward and smoothing
void AC_AttitudeControl::input_rate_bf_roll_pitch_yaw(float roll_rate_bf_cds, float pitch_rate_bf_cds, float yaw_rate_bf_cds)
{
    // Convert from centidegrees on public interface to radians
    float roll_rate_rads = radians(roll_rate_bf_cds * 0.01f);
    float pitch_rate_rads = radians(pitch_rate_bf_cds * 0.01f);
    float yaw_rate_rads = radians(yaw_rate_bf_cds * 0.01f);

    // calculate the attitude target euler angles
    _attitude_target.to_euler(_euler_angle_target.x, _euler_angle_target.y, _euler_angle_target.z);

    if (_rate_bf_ff_enabled) {
        // Compute acceleration-limited body frame rates
        // When acceleration limiting is enabled, the input shaper constrains angular acceleration about the axis, slewing
        // the output rate towards the input rate.
        _ang_vel_target.x = input_shaping_ang_vel(_ang_vel_target.x, roll_rate_rads, get_accel_roll_max_radss(), _dt);
        _ang_vel_target.y = input_shaping_ang_vel(_ang_vel_target.y, pitch_rate_rads, get_accel_pitch_max_radss(), _dt);
        _ang_vel_target.z = input_shaping_ang_vel(_ang_vel_target.z, yaw_rate_rads, get_accel_yaw_max_radss(), _dt);

        // Convert body-frame angular velocity into euler angle derivative of desired attitude
        ang_vel_to_euler_rate(_euler_angle_target, _ang_vel_target, _euler_rate_target);
    } else {
        // When feedforward is not enabled, the quaternion is calculated and is input into the target and the feedforward rate is zeroed.
        Quaternion attitude_target_update;
        attitude_target_update.from_axis_angle(Vector3f{roll_rate_rads * _dt, pitch_rate_rads * _dt, yaw_rate_rads * _dt});
        _attitude_target = _attitude_target * attitude_target_update;
        _attitude_target.normalize();

        // Set rate feedforward requests to zero
        _euler_rate_target.zero();
        _ang_vel_target.zero();
    }

    // Call quaternion attitude controller
    attitude_controller_run_quat();
}

// Command an angular velocity with angular velocity smoothing using rate loops only with no attitude loop stabilization
void AC_AttitudeControl::input_rate_bf_roll_pitch_yaw_2(float roll_rate_bf_cds, float pitch_rate_bf_cds, float yaw_rate_bf_cds)
{
    // Convert from centidegrees on public interface to radians
    float roll_rate_rads = radians(roll_rate_bf_cds * 0.01f);
    float pitch_rate_rads = radians(pitch_rate_bf_cds * 0.01f);
    float yaw_rate_rads = radians(yaw_rate_bf_cds * 0.01f);

    // Compute acceleration-limited body frame rates
    // When acceleration limiting is enabled, the input shaper constrains angular acceleration about the axis, slewing
    // the output rate towards the input rate.
    _ang_vel_target.x = input_shaping_ang_vel(_ang_vel_target.x, roll_rate_rads, get_accel_roll_max_radss(), _dt);
    _ang_vel_target.y = input_shaping_ang_vel(_ang_vel_target.y, pitch_rate_rads, get_accel_pitch_max_radss(), _dt);
    _ang_vel_target.z = input_shaping_ang_vel(_ang_vel_target.z, yaw_rate_rads, get_accel_yaw_max_radss(), _dt);

    // Update the unused targets attitude based on current attitude to condition mode change
    _ahrs.get_quat_body_to_ned(_attitude_target);
    _attitude_target.to_euler(_euler_angle_target.x, _euler_angle_target.y, _euler_angle_target.z);
    // Convert body-frame angular velocity into euler angle derivative of desired attitude
    ang_vel_to_euler_rate(_euler_angle_target, _ang_vel_target, _euler_rate_target);
    _ang_vel_body = _ang_vel_target;
}

// Command an angular velocity with angular velocity smoothing using rate loops only with integrated rate error stabilization
void AC_AttitudeControl::input_rate_bf_roll_pitch_yaw_3(float roll_rate_bf_cds, float pitch_rate_bf_cds, float yaw_rate_bf_cds)
{
    // Convert from centidegrees on public interface to radians
    float roll_rate_rads = radians(roll_rate_bf_cds * 0.01f);
    float pitch_rate_rads = radians(pitch_rate_bf_cds * 0.01f);
    float yaw_rate_rads = radians(yaw_rate_bf_cds * 0.01f);

    // Update attitude error
    Vector3f attitude_error;
    _attitude_ang_error.to_axis_angle(attitude_error);

    Quaternion attitude_ang_error_update_quat;
    // limit the integrated error angle
    float err_mag = attitude_error.length();
    if (err_mag > AC_ATTITUDE_THRUST_ERROR_ANGLE) {
        attitude_error *= AC_ATTITUDE_THRUST_ERROR_ANGLE / err_mag;
        _attitude_ang_error.from_axis_angle(attitude_error);
    }

    Vector3f gyro_latest = _ahrs.get_gyro_latest();
    attitude_ang_error_update_quat.from_axis_angle(Vector3f{(_ang_vel_target.x-gyro_latest.x) * _dt, (_ang_vel_target.y-gyro_latest.y) * _dt, (_ang_vel_target.z-gyro_latest.z) * _dt});
    _attitude_ang_error = attitude_ang_error_update_quat * _attitude_ang_error;

    // Compute acceleration-limited body frame rates
    // When acceleration limiting is enabled, the input shaper constrains angular acceleration about the axis, slewing
    // the output rate towards the input rate.
    _ang_vel_target.x = input_shaping_ang_vel(_ang_vel_target.x, roll_rate_rads, get_accel_roll_max_radss(), _dt);
    _ang_vel_target.y = input_shaping_ang_vel(_ang_vel_target.y, pitch_rate_rads, get_accel_pitch_max_radss(), _dt);
    _ang_vel_target.z = input_shaping_ang_vel(_ang_vel_target.z, yaw_rate_rads, get_accel_yaw_max_radss(), _dt);

    // Retrieve quaternion body attitude
    Quaternion attitude_body;
    _ahrs.get_quat_body_to_ned(attitude_body);

    // Update the unused targets attitude based on current attitude to condition mode change
    _attitude_target = attitude_body * _attitude_ang_error;

    // calculate the attitude target euler angles
    _attitude_target.to_euler(_euler_angle_target.x, _euler_angle_target.y, _euler_angle_target.z);

    // Convert body-frame angular velocity into euler angle derivative of desired attitude
    ang_vel_to_euler_rate(_euler_angle_target, _ang_vel_target, _euler_rate_target);

    // Compute the angular velocity target from the integrated rate error
    _attitude_ang_error.to_axis_angle(attitude_error);
    _ang_vel_body = update_ang_vel_target_from_att_error(attitude_error);
    _ang_vel_body += _ang_vel_target;

    // ensure Quaternions stay normalized
    _attitude_ang_error.normalize();
}

// Command an angular step (i.e change) in body frame angle
// Used to command a step in angle without exciting the orthogonal axis during autotune
void AC_AttitudeControl::input_angle_step_bf_roll_pitch_yaw(float roll_angle_step_bf_cd, float pitch_angle_step_bf_cd, float yaw_angle_step_bf_cd)
{
    // Convert from centidegrees on public interface to radians
    float roll_step_rads = radians(roll_angle_step_bf_cd * 0.01f);
    float pitch_step_rads = radians(pitch_angle_step_bf_cd * 0.01f);
    float yaw_step_rads = radians(yaw_angle_step_bf_cd * 0.01f);

    // rotate attitude target by desired step
    Quaternion attitude_target_update;
    attitude_target_update.from_axis_angle(Vector3f{roll_step_rads, pitch_step_rads, yaw_step_rads});
    _attitude_target = _attitude_target * attitude_target_update;
    _attitude_target.normalize();

    // calculate the attitude target euler angles
    _attitude_target.to_euler(_euler_angle_target.x, _euler_angle_target.y, _euler_angle_target.z);

    // Set rate feedforward requests to zero
    _euler_rate_target.zero();
    _ang_vel_target.zero();

    // Call quaternion attitude controller
    attitude_controller_run_quat();
}

// Command a thrust vector and heading rate
void AC_AttitudeControl::input_thrust_vector_rate_heading(const Vector3f& thrust_vector, float heading_rate_cds)
{
    // Convert from centidegrees on public interface to radians
    const float heading_rate = radians(heading_rate_cds * 0.01f);

    // calculate the attitude target euler angles
    _attitude_target.to_euler(_euler_angle_target.x, _euler_angle_target.y, _euler_angle_target.z);

    // convert thrust vector to a quaternion attitude
    Quaternion thrust_vec_quat = attitude_from_thrust_vector(thrust_vector, 0.0f);

    // calculate the angle error in x and y.
    float thrust_vector_diff_angle;
    Quaternion thrust_vec_correction_quat;
    Vector3f attitude_error;
    float returned_thrust_vector_angle;
    thrust_vector_rotation_angles(thrust_vec_quat, _attitude_target, thrust_vec_correction_quat, attitude_error, returned_thrust_vector_angle, thrust_vector_diff_angle);

    if (_rate_bf_ff_enabled) {
        // When yaw acceleration limiting is enabled, the yaw input shaper constrains angular acceleration about the yaw axis, slewing
        // the output rate towards the input rate.
        _ang_vel_target.x = input_shaping_angle(attitude_error.x, _input_tc, get_accel_roll_max_radss(), _ang_vel_target.x, _dt);
        _ang_vel_target.y = input_shaping_angle(attitude_error.y, _input_tc, get_accel_pitch_max_radss(), _ang_vel_target.y, _dt);

        // When yaw acceleration limiting is enabled, the yaw input shaper constrains angular acceleration about the yaw axis, slewing
        // the output rate towards the input rate.
        _ang_vel_target.z = input_shaping_ang_vel(_ang_vel_target.z, heading_rate, get_accel_yaw_max_radss(), _dt);

        // Limit the angular velocity
        ang_vel_limit(_ang_vel_target, radians(_ang_vel_roll_max), radians(_ang_vel_pitch_max), MIN(radians(_ang_vel_yaw_max), get_slew_yaw_rads()));
    } else {
        Quaternion yaw_quat;
        yaw_quat.from_axis_angle(Vector3f{0.0f, 0.0f, heading_rate * _dt});
        _attitude_target = _attitude_target * thrust_vec_correction_quat * yaw_quat;

        // Set rate feedforward requests to zero
        _euler_rate_target.zero();
        _ang_vel_target.zero();
    }

    // Convert body-frame angular velocity into euler angle derivative of desired attitude
    ang_vel_to_euler_rate(_euler_angle_target, _ang_vel_target, _euler_rate_target);

    // Call quaternion attitude controller
    attitude_controller_run_quat();
}

// Command a thrust vector, heading and heading rate
void AC_AttitudeControl::input_thrust_vector_heading(const Vector3f& thrust_vector, float heading_angle_cd, float heading_rate_cds)
{
    // a zero _angle_vel_yaw_max means that setting is disabled
    const float ang_vel_yaw_max_rads = is_zero(_ang_vel_yaw_max) ? get_slew_yaw_rads() : MIN(radians(_ang_vel_yaw_max), get_slew_yaw_rads());

    // Convert from centidegrees on public interface to radians
    float heading_rate = constrain_float(radians(heading_rate_cds * 0.01f), -ang_vel_yaw_max_rads, ang_vel_yaw_max_rads);
    float heading_angle = radians(heading_angle_cd * 0.01f);

    // calculate the attitude target euler angles
    _attitude_target.to_euler(_euler_angle_target.x, _euler_angle_target.y, _euler_angle_target.z);

    // convert thrust vector and heading to a quaternion attitude
    const Quaternion desired_attitude_quat = attitude_from_thrust_vector(thrust_vector, heading_angle);

    if (_rate_bf_ff_enabled) {
        // calculate the angle error in x and y.
        Vector3f attitude_error;
        float thrust_vector_diff_angle;
        Quaternion thrust_vec_correction_quat;
        float returned_thrust_vector_angle;
        thrust_vector_rotation_angles(desired_attitude_quat, _attitude_target, thrust_vec_correction_quat, attitude_error, returned_thrust_vector_angle, thrust_vector_diff_angle);

        // When yaw acceleration limiting is enabled, the yaw input shaper constrains angular acceleration about the yaw axis, slewing
        // the output rate towards the input rate.
        _ang_vel_target.x = input_shaping_angle(attitude_error.x, _input_tc, get_accel_roll_max_radss(), _ang_vel_target.x, _dt);
        _ang_vel_target.y = input_shaping_angle(attitude_error.y, _input_tc, get_accel_pitch_max_radss(), _ang_vel_target.y, _dt);
        _ang_vel_target.z = input_shaping_angle(attitude_error.z, _input_tc, get_accel_yaw_max_radss(), _ang_vel_target.z, heading_rate, get_slew_yaw_rads(), _dt);

        // Limit the angular velocity
        ang_vel_limit(_ang_vel_target, radians(_ang_vel_roll_max), radians(_ang_vel_pitch_max), ang_vel_yaw_max_rads);
    } else {
        // set persisted quaternion target attitude
        _attitude_target = desired_attitude_quat;

        // Set rate feedforward requests to zero
        _euler_rate_target.zero();
        _ang_vel_target.zero();
    }

    // Convert body-frame angular velocity into euler angle derivative of desired attitude
    ang_vel_to_euler_rate(_euler_angle_target, _ang_vel_target, _euler_rate_target);

    // Call quaternion attitude controller
    attitude_controller_run_quat();
}

Quaternion AC_AttitudeControl::attitude_from_thrust_vector(Vector3f thrust_vector, float heading_angle) const
{
    const Vector3f thrust_vector_up{0.0f, 0.0f, -1.0f};

    if (is_zero(thrust_vector.length_squared())) {
        thrust_vector = thrust_vector_up;
    } else {
        thrust_vector.normalize();
    }

    // the cross product of the desired and target thrust vector defines the rotation vector
    Vector3f thrust_vec_cross = thrust_vector_up % thrust_vector;

    // the dot product is used to calculate the angle between the target and desired thrust vectors
    const float thrust_vector_angle = acosf(constrain_float(thrust_vector_up * thrust_vector, -1.0f, 1.0f));

    // Normalize the thrust rotation vector
    const float thrust_vector_length = thrust_vec_cross.length();
    if (is_zero(thrust_vector_length) || is_zero(thrust_vector_angle)) {
        thrust_vec_cross = thrust_vector_up;
    } else {
        thrust_vec_cross /= thrust_vector_length;
    }

    Quaternion thrust_vec_quat;
    thrust_vec_quat.from_axis_angle(thrust_vec_cross, thrust_vector_angle);
    Quaternion yaw_quat;
    yaw_quat.from_axis_angle(Vector3f{0.0f, 0.0f, 1.0f}, heading_angle);
    return thrust_vec_quat*yaw_quat;
}

// Calculates the body frame angular velocities to follow the target attitude
// 为了跟随目标姿态，计算机体坐标系角速度
void AC_AttitudeControl::attitude_controller_run_quat()
{
    // 机体坐标系向地理坐标系的旋转四元数
    // 获取当前飞行器的姿态
    Quaternion attitude_body;
    _ahrs.get_quat_body_to_ned(attitude_body);

    // This vector represents the angular error to rotate the thrust vector using x and y and heading using z
    // 该向量表示姿态误差。这个角度误差使用x、y、航向使用z，旋转推力向量得来的
    /*
    * 输入: 
    *   attitude_body: 当前机体姿态
    *   _attitude_target: 没有前馈的情况下，直接来自于遥控器.有前馈时来自于上一时刻机体系目标角速度积分.
    * 输出:
    *   _attitude_target: 当偏航角误差过大时，会被函数限幅
    *   attitude_error: 姿态误差。当前姿态与目标姿态之间的误差
    *   _thrust_angle:推力角。来自于竖直向上推力向量与当前姿态下推力向量的夹角。rad ；-1 ~ +1
    *   _thrust_error_angle:推力角误差。当前姿态下推力向量与目标姿态下推力向量的夹角。rad ; -1 ~ +1
    */
    Vector3f attitude_error;
    thrust_heading_rotation_angles(_attitude_target, attitude_body, attitude_error, _thrust_angle, _thrust_error_angle);

    // Compute the angular velocity corrections in the body frame from the attitude error
    // 从机体系姿态误差中计算角速度校正量
    /* 角度控制环 */
    _ang_vel_body = update_ang_vel_target_from_att_error(attitude_error);

    // ensure angular velocity does not go over configured limits
    // 确保角速度不超过设置的限制
    ang_vel_limit(_ang_vel_body, radians(_ang_vel_roll_max), radians(_ang_vel_pitch_max), radians(_ang_vel_yaw_max));

    AP::logger().Write("ANGV","TimeUs,AERX,AERY,AERZ,TRUS","Qffff",AP_HAL::micros64(),_ang_vel_body.x,_ang_vel_body.y,_ang_vel_body.z,_thrust_error_angle);

    // rotation from the target frame to the body frame
    // 从目标坐标系(NED坐标系)旋转到机体坐标系
    Quaternion rotation_target_to_body = attitude_body.inverse() * _attitude_target;

    // target angle velocity vector in the body frame
    // 在机体坐标系下的目标角速度向量
    Vector3f ang_vel_body_feedforward = rotation_target_to_body * _ang_vel_target;

    AP::logger().Write("AFEW","TimeUs,AERX,AERY,AERZ","Qfff",AP_HAL::micros64(),ang_vel_body_feedforward.x,ang_vel_body_feedforward.y,ang_vel_body_feedforward.z);
    // Correct the thrust vector and smoothly add feedforward and yaw input
    // 校正推力向量，且平滑增加前馈和偏航角输入
    _feedforward_scalar = 1.0f;
    if (_thrust_error_angle > AC_ATTITUDE_THRUST_ERROR_ANGLE * 2.0f) {
        _ang_vel_body.z = _ahrs.get_gyro().z;
    } else if (_thrust_error_angle > AC_ATTITUDE_THRUST_ERROR_ANGLE) {
        _feedforward_scalar = (1.0f - (_thrust_error_angle - AC_ATTITUDE_THRUST_ERROR_ANGLE) / AC_ATTITUDE_THRUST_ERROR_ANGLE);
        _ang_vel_body.x += ang_vel_body_feedforward.x * _feedforward_scalar;
        _ang_vel_body.y += ang_vel_body_feedforward.y * _feedforward_scalar;
        _ang_vel_body.z += ang_vel_body_feedforward.z;  
        _ang_vel_body.z = _ahrs.get_gyro().z * (1.0 - _feedforward_scalar) + _ang_vel_body.z * _feedforward_scalar;     // 一阶低通滤波
    } else {
        _ang_vel_body += ang_vel_body_feedforward;
    }

    if (_rate_bf_ff_enabled) {
        // rotate target and normalize
        Quaternion attitude_target_update;
        attitude_target_update.from_axis_angle(Vector3f{_ang_vel_target.x * _dt, _ang_vel_target.y * _dt, _ang_vel_target.z * _dt});
        _attitude_target = _attitude_target * attitude_target_update;   // 从上一个时刻_attitude_target
        _attitude_target.normalize();
    }

    // ensure Quaternion stay normalised
    _attitude_target.normalize();

    // Record error to handle EKF resets
    _attitude_ang_error = attitude_body.inverse() * _attitude_target;
}

// thrust_heading_rotation_angles - calculates two ordered rotations to move the attitude_body quaternion to the attitude_target quaternion.
// The maximum error in the yaw axis is limited based on the angle yaw P value and acceleration.
// thrust_heading_rotation_angles - 计算两个有序旋转。将姿态四元数旋转到姿态目标四元数
// 偏航轴上的最大误差是基于偏航角P值和加速度限制的
// 目标姿态:attitude_target
// 当前姿态:attitude_body
// 目标姿态与当前姿态之间的误差:attitude_error
// 
void AC_AttitudeControl::thrust_heading_rotation_angles(Quaternion& attitude_target, const Quaternion& attitude_body, Vector3f& attitude_error, float& thrust_angle, float& thrust_error_angle) const
{
    Quaternion thrust_vector_correction;
    thrust_vector_rotation_angles(attitude_target, attitude_body, thrust_vector_correction, attitude_error, thrust_angle, thrust_error_angle);

    // Todo: Limit roll an pitch error based on output saturation and maximum error.

    // Limit Yaw Error based on maximum acceleration - Update to include output saturation and maximum error.
    // Currently the limit is based on the maximum acceleration using the linear part of the SQRT controller.
    // This should be updated to be based on an angle limit, saturation, or unlimited based on user defined parameters.
    // 基于最大加速度的限制偏航角误差 - 输出饱和度与最大误差更新。
    // 当前限制是基于线性部分的SQRT控制器的最大加速度。
    // 这应当基于角度限制，饱和度，或者用户定义的不限制来更新。
    /*
    * yaw角控制值不为零，并且z轴误差>AC_ATTITUDE_ACCEL_Y_CONTROLLER_MAX_RADSS / _p_angle_yaw.kP()则进入
    */
    /* 偏航角误差限幅 */
    Quaternion yaw_vec_correction_quat;
    if (!is_zero(_p_angle_yaw.kP()) && fabsf(attitude_error.z) > AC_ATTITUDE_ACCEL_Y_CONTROLLER_MAX_RADSS / _p_angle_yaw.kP()) {
        attitude_error.z = constrain_float(wrap_PI(attitude_error.z), -AC_ATTITUDE_ACCEL_Y_CONTROLLER_MAX_RADSS / _p_angle_yaw.kP(), AC_ATTITUDE_ACCEL_Y_CONTROLLER_MAX_RADSS / _p_angle_yaw.kP());
        yaw_vec_correction_quat.from_axis_angle(Vector3f{0.0f, 0.0f, attitude_error.z});
        attitude_target = attitude_body * thrust_vector_correction * yaw_vec_correction_quat;
    }
}

// thrust_vector_rotation_angles - calculates two ordered rotations to move the attitude_body quaternion to the attitude_target quaternion.
// The first rotation corrects the thrust vector and the second rotation corrects the heading vector.
// thrust_vector_rotation_angles - 计算两个有序旋转以将姿态四元数移动到姿态目标四元数
// 第一次旋转校正推力矢量，第二次旋转校正航向矢量。
/*
* 输出: attitude_error:当前姿态与目标姿态之间的姿态误差。
*      thrust_angle:推力角。竖直向上推力向量与当前姿态下推力向量的夹角。rad ；-1 ~ +1
*      _thrust_error_angle:推力角误差。当前姿态下推力向量与目标姿态下推力向量的夹角。rad ; -1 ~ +1
*/
void AC_AttitudeControl::thrust_vector_rotation_angles(const Quaternion& attitude_target, const Quaternion& attitude_body, Quaternion& thrust_vector_correction, Vector3f& attitude_error, float& thrust_angle, float& thrust_error_angle) const
{
    Vector3f att_body;
    attitude_body.to_axis_angle(att_body);
    AP::logger().Write("ATTB","TimeUs,ATBX,ATBY,ATBZ","Qfff",AP_HAL::micros64(),att_body.x,att_body.y,att_body.z);
    // The direction of thrust is [0,0,-1] is any body-fixed frame, inc. body frame and target frame.
    // 在任何机体/目标坐标系下的推力方向
    const Vector3f thrust_vector_up{0.0f, 0.0f, -1.0f};

    // attitude_target and attitute_body are passive rotations from target / body frames to the NED frame
    // attitude_target 和 attitute_body 是从目标/机体坐标到NED坐标系下的被动旋转

    // Rotating [0,0,-1] by attitude_target expresses (gets a view of) the target thrust vector in the inertial frame
    // 通过attitude_target表达式旋转[0,0,-1]向量。获得惯性系下目标推力矢量 (惯性系等于机体系)
    /* 获取到达目标姿态所需的向上升力 */
    Vector3f att_target_thrust_vec = attitude_target * thrust_vector_up; // target thrust vector

    // Rotating [0,0,-1] by attitude_target expresses (gets a view of) the current thrust vector in the inertial frame
    /* 获取当前姿态的推力矢量 */
    Vector3f att_body_thrust_vec = attitude_body * thrust_vector_up; // current thrust vector

    // the dot product is used to calculate the current lean angle for use of external functions
    /* 获取当前姿态下的推力角 */
    /* 当推力向量与姿态推力向量平行时，thrust_angle = 0 */
    /* 只打航向时,thrust_angle = 0 */
    thrust_angle = acosf(constrain_float(thrust_vector_up * att_body_thrust_vec,-1.0f,1.0f));

    // the cross product of the desired and target thrust vector defines the rotation vector
    /* 这里的 % 是向量叉乘，运算符重载 */
    /* 只打航向时叉乘为0 */
    Vector3f thrust_vec_cross = att_body_thrust_vec % att_target_thrust_vec;

    // the dot product is used to calculate the angle between the target and desired thrust vectors
    /* 获取当前姿态推力矢量与目标姿态推力矢量之间的夹角，即推力矢量误差角*/
    /* 只打航向角时,thrust_error_angle等于|att_body_thrust_vec|*|att_target_thrust_vec| */
    /* 地理系下的推力误差角 */
    thrust_error_angle = acosf(constrain_float(att_body_thrust_vec * att_target_thrust_vec, -1.0f, 1.0f));
    
    // Normalize the thrust rotation vector
    /* 如果推力矢量长度为零，即当前姿态推力矢量与目标推力矢量平行(这里如果出现姿态推力矢量与目标推力矢量反向，则有极其严重后果)。或者没有
       或者推力矢量误差角为零(两向量垂直)，则推力矢量叉乘默认向上，即 thrust_vec_cross = thrust_vector_up 。否则，归一化推力矢量叉乘向量。*/ 
    /* 若目标姿态与当前姿态一致则叉乘向量长度为0，姿态无需校正 */
    float thrust_vector_length = thrust_vec_cross.length();
    if (is_zero(thrust_vector_length) || is_zero(thrust_error_angle)) { //姿态推力向量与目标推力向量同向或者相差180度都会进入
        thrust_vec_cross = thrust_vector_up;
    } else {
        thrust_vec_cross /= thrust_vector_length;   //归一化,确保thrust_vec_cross是单位向量，这样可以直接用轴角法转化成四元数
    }

    // thrust_vector_correction is defined relative to the body frame but its axis `thrust_vec_cross` was computed in
    // the inertial frame. First rotate it by the inverse of attitude_body to express it back in the body frame
    // 推力向量校正是相对于机体坐标系定义的，但它的轴“推力向量交叉”是在惯性系中计算的。 先通过attitude_body的共轭(逆旋转)旋转到机体系
    /* 转换到机体系 */
    thrust_vec_cross = attitude_body.inverse() * thrust_vec_cross;
    thrust_vector_correction.from_axis_angle(thrust_vec_cross, thrust_error_angle);

    // calculate the angle error in x and y.
    /* 转化成角度误差 */
    Vector3f rotation;
    thrust_vector_correction.to_axis_angle(rotation);
    attitude_error.x = rotation.x;
    attitude_error.y = rotation.y;

    // calculate the remaining rotation required after thrust vector is rotated transformed to the body frame
    // heading_vector_correction 
    // 计算推力矢量旋转变换到机体坐标系后所需的剩余旋转
    /* 获取航向校正四元数 */
    // 为啥这里得修一下偏航角? 
    AP::logger().Write("TVRA","TimeUs,AERX,AERY,AERZ,TRUS","Qffff",AP_HAL::micros64(),attitude_error.x,attitude_error.y,attitude_error.z,_thrust_error_angle);
    /* attitude_target往反方向旋转attitude_body，再往反方向旋转thrust_vector_correction */
    Quaternion heading_vec_correction_quat = thrust_vector_correction.inverse() * attitude_body.inverse() * attitude_target;    //表示目标姿态与当前姿态、推力校正向量之间的差

    Vector3f hvcq_axis_angle;
    thrust_vector_correction.inverse().to_axis_angle(hvcq_axis_angle);
    AP::logger().Write("TVCQ","TimeUs,AERX,AERY,AERZ","Qfff",AP_HAL::micros64(),hvcq_axis_angle.x,hvcq_axis_angle.y,hvcq_axis_angle.z);

    Vector3f attbody_inverse_axis_angle;
    attitude_body.inverse().to_axis_angle(attbody_inverse_axis_angle);
    AP::logger().Write("ATBI","TimeUs,AERX,AERY,AERZ","Qfff",AP_HAL::micros64(),attbody_inverse_axis_angle.x,attbody_inverse_axis_angle.y,attbody_inverse_axis_angle.z);
   
    Vector3f att_target_axis_angle;
    attitude_body.inverse().to_axis_angle(att_target_axis_angle);
    AP::logger().Write("ATRI","TimeUs,AERX,AERY,AERZ","Qfff",AP_HAL::micros64(),att_target_axis_angle.x,att_target_axis_angle.y,att_target_axis_angle.z);
    
    
    // calculate the angle error in z (x and y should be zero here).
    // 计算z轴角度误差(在这儿x，y应当为0)
    heading_vec_correction_quat.to_axis_angle(rotation);
    attitude_error.z = rotation.z;

    AP::logger().Write("HDRO","TimeUs,AERX,AERY,AERZ,TRUS","Qffff",AP_HAL::micros64(),attitude_error.x,attitude_error.y,attitude_error.z,_thrust_error_angle);
}

// calculates the velocity correction from an angle error. The angular velocity has acceleration and
// deceleration limits including basic jerk limiting using _input_tc
// 默认 desired_ang_vel = 0 ; max_ang_vel = 0;
float AC_AttitudeControl::input_shaping_angle(float error_angle, float input_tc, float accel_max, float target_ang_vel, float desired_ang_vel, float max_ang_vel, float dt)
{
    // Calculate the velocity as error approaches zero with acceleration limited by accel_max_radss
    desired_ang_vel += sqrt_controller(error_angle, 1.0f / MAX(input_tc, 0.01f), accel_max, dt);
    if (is_positive(max_ang_vel)) {
        desired_ang_vel = constrain_float(desired_ang_vel, -max_ang_vel, max_ang_vel);
    }

    // Acceleration is limited directly to smooth the beginning of the curve.
    return input_shaping_ang_vel(target_ang_vel, desired_ang_vel, accel_max, dt);
}

// limits the acceleration and deceleration of a velocity request
float AC_AttitudeControl::input_shaping_ang_vel(float target_ang_vel, float desired_ang_vel, float accel_max, float dt)
{
    // Acceleration is limited directly to smooth the beginning of the curve.
    // 通过加速度直接限制平滑速度曲线
    if (is_positive(accel_max)) {
        float delta_ang_vel = accel_max * dt;
        return constrain_float(desired_ang_vel, target_ang_vel - delta_ang_vel, target_ang_vel + delta_ang_vel);
    } else {
        return desired_ang_vel;
    }
}

// calculates the expected angular velocity correction from an angle error based on the AC_AttitudeControl settings.
// This function can be used to predict the delay associated with angle requests.
void AC_AttitudeControl::input_shaping_rate_predictor(const Vector2f &error_angle, Vector2f& target_ang_vel, float dt) const
{
    if (_rate_bf_ff_enabled) {
        // translate the roll pitch and yaw acceleration limits to the euler axis
        target_ang_vel.x = input_shaping_angle(wrap_PI(error_angle.x), _input_tc, get_accel_roll_max_radss(), target_ang_vel.x, dt);
        target_ang_vel.y = input_shaping_angle(wrap_PI(error_angle.y), _input_tc, get_accel_pitch_max_radss(), target_ang_vel.y, dt);
    } else {
        target_ang_vel.x = _p_angle_roll.get_p(wrap_PI(error_angle.x));
        target_ang_vel.y = _p_angle_pitch.get_p(wrap_PI(error_angle.y));
    }
    // Limit the angular velocity correction
    Vector3f ang_vel(target_ang_vel.x, target_ang_vel.y, 0.0f);
    ang_vel_limit(ang_vel, radians(_ang_vel_roll_max), radians(_ang_vel_pitch_max), 0.0f);

    target_ang_vel.x = ang_vel.x;
    target_ang_vel.y = ang_vel.y;
}

// limits angular velocity
void AC_AttitudeControl::ang_vel_limit(Vector3f& euler_rad, float ang_vel_roll_max, float ang_vel_pitch_max, float ang_vel_yaw_max) const
{
    if (is_zero(ang_vel_roll_max) || is_zero(ang_vel_pitch_max)) {
        if (!is_zero(ang_vel_roll_max)) {
            euler_rad.x = constrain_float(euler_rad.x, -ang_vel_roll_max, ang_vel_roll_max);
        }
        if (!is_zero(ang_vel_pitch_max)) {
            euler_rad.y = constrain_float(euler_rad.y, -ang_vel_pitch_max, ang_vel_pitch_max);
        }
    } else {
        Vector2f thrust_vector_ang_vel(euler_rad.x / ang_vel_roll_max, euler_rad.y / ang_vel_pitch_max);
        float thrust_vector_length = thrust_vector_ang_vel.length();
        if (thrust_vector_length > 1.0f) {
            euler_rad.x = thrust_vector_ang_vel.x * ang_vel_roll_max / thrust_vector_length;
            euler_rad.y = thrust_vector_ang_vel.y * ang_vel_pitch_max / thrust_vector_length;
        }
    }
    if (!is_zero(ang_vel_yaw_max)) {
        euler_rad.z = constrain_float(euler_rad.z, -ang_vel_yaw_max, ang_vel_yaw_max);
    }
}

// translates body frame acceleration limits to the euler axis
Vector3f AC_AttitudeControl::euler_accel_limit(const Vector3f &euler_rad, const Vector3f &euler_accel)
{
    float sin_phi = constrain_float(fabsf(sinf(euler_rad.x)), 0.1f, 1.0f);
    float cos_phi = constrain_float(fabsf(cosf(euler_rad.x)), 0.1f, 1.0f);
    float sin_theta = constrain_float(fabsf(sinf(euler_rad.y)), 0.1f, 1.0f);

    Vector3f rot_accel;
    if (is_zero(euler_accel.x) || is_zero(euler_accel.y) || is_zero(euler_accel.z) || is_negative(euler_accel.x) || is_negative(euler_accel.y) || is_negative(euler_accel.z)) {
        rot_accel.x = euler_accel.x;
        rot_accel.y = euler_accel.y;
        rot_accel.z = euler_accel.z;
    } else {
        rot_accel.x = euler_accel.x;
        rot_accel.y = MIN(euler_accel.y / cos_phi, euler_accel.z / sin_phi);
        rot_accel.z = MIN(MIN(euler_accel.x / sin_theta, euler_accel.y / sin_phi), euler_accel.z / cos_phi);
    }
    return rot_accel;
}

// Sets attitude target to vehicle attitude and sets all rates to zero
// If reset_rate is false rates are not reset to allow the rate controllers to run
void AC_AttitudeControl::reset_target_and_rate(bool reset_rate)
{
    // move attitude target to current attitude
    _ahrs.get_quat_body_to_ned(_attitude_target);

    if (reset_rate) {
        // Convert euler angle derivative of desired attitude into a body-frame angular velocity vector for feedforward
        _ang_vel_target.zero();
        _euler_angle_target.zero();
    }
}

// Sets yaw target to vehicle heading and sets yaw rate to zero
// If reset_rate is false rates are not reset to allow the rate controllers to run
void AC_AttitudeControl::reset_yaw_target_and_rate(bool reset_rate)
{
    // move attitude target to current heading
    float yaw_shift = _ahrs.yaw - _euler_angle_target.z;
    Quaternion _attitude_target_update;
    _attitude_target_update.from_axis_angle(Vector3f{0.0f, 0.0f, yaw_shift});
    _attitude_target = _attitude_target_update * _attitude_target;

    if (reset_rate) {
        // set yaw rate to zero
        _euler_rate_target.z = 0.0f;

        // Convert euler angle derivative of desired attitude into a body-frame angular velocity vector for feedforward
        euler_rate_to_ang_vel(_euler_angle_target, _euler_rate_target, _ang_vel_target);
    }
}

// Shifts the target attitude to maintain the current error in the event of an EKF reset
void AC_AttitudeControl::inertial_frame_reset()
{
    // Retrieve quaternion body attitude
    Quaternion attitude_body;
    _ahrs.get_quat_body_to_ned(attitude_body);

    // Recalculate the target quaternion
    _attitude_target = attitude_body * _attitude_ang_error;

    // calculate the attitude target euler angles
    _attitude_target.to_euler(_euler_angle_target.x, _euler_angle_target.y, _euler_angle_target.z);
}

// Convert a 321-intrinsic euler angle derivative to an angular velocity vector
// 将 321 欧拉角速度转换为惯性系角速度矢量
/* 将欧拉角速率正交分解到机体坐标系上 */
void AC_AttitudeControl::euler_rate_to_ang_vel(const Vector3f& euler_rad, const Vector3f& euler_rate_rads, Vector3f& ang_vel_rads)
{
    float sin_theta = sinf(euler_rad.y);
    float cos_theta = cosf(euler_rad.y);
    float sin_phi = sinf(euler_rad.x);
    float cos_phi = cosf(euler_rad.x);

    ang_vel_rads.x = euler_rate_rads.x - sin_theta * euler_rate_rads.z;
    ang_vel_rads.y = cos_phi * euler_rate_rads.y + sin_phi * cos_theta * euler_rate_rads.z;
    ang_vel_rads.z = -sin_phi * euler_rate_rads.y + cos_theta * cos_phi * euler_rate_rads.z;
}

// Convert an angular velocity vector to a 321-intrinsic euler angle derivative
// Returns false if the vehicle is pitched 90 degrees up or down
// 将角速度向量转化到321欧拉角速度微分
// 如果飞行器九十度头朝上或者朝下返回false
/* 将机体角速率分解到欧拉角速率方向上 ，三个轴的欧拉角速率不两两垂直*/
bool AC_AttitudeControl::ang_vel_to_euler_rate(const Vector3f& euler_rad, const Vector3f& ang_vel_rads, Vector3f& euler_rate_rads)
{
    float sin_theta = sinf(euler_rad.y);
    float cos_theta = cosf(euler_rad.y);
    float sin_phi = sinf(euler_rad.x);
    float cos_phi = cosf(euler_rad.x);

    // When the vehicle pitches all the way up or all the way down, the euler angles become discontinuous. In this case, we just return false.
    if (is_zero(cos_theta)) {
        return false;
    }

    euler_rate_rads.x = ang_vel_rads.x + sin_phi * (sin_theta / cos_theta) * ang_vel_rads.y + cos_phi * (sin_theta / cos_theta) * ang_vel_rads.z;
    euler_rate_rads.y = cos_phi * ang_vel_rads.y - sin_phi * ang_vel_rads.z;
    euler_rate_rads.z = (sin_phi / cos_theta) * ang_vel_rads.y + (cos_phi / cos_theta) * ang_vel_rads.z;
    return true;
}

// Update rate_target_ang_vel using attitude_error_rot_vec_rad
Vector3f AC_AttitudeControl::update_ang_vel_target_from_att_error(const Vector3f &attitude_error_rot_vec_rad)
{
    Vector3f rate_target_ang_vel;
    // Compute the roll angular velocity demand from the roll angle error
    if (_use_sqrt_controller && !is_zero(get_accel_roll_max_radss())) {
        rate_target_ang_vel.x = sqrt_controller(attitude_error_rot_vec_rad.x, _p_angle_roll.kP(), constrain_float(get_accel_roll_max_radss() / 2.0f, AC_ATTITUDE_ACCEL_RP_CONTROLLER_MIN_RADSS, AC_ATTITUDE_ACCEL_RP_CONTROLLER_MAX_RADSS), _dt);
    } else {
        rate_target_ang_vel.x = _p_angle_roll.kP() * attitude_error_rot_vec_rad.x;
    }

    // Compute the pitch angular velocity demand from the pitch angle error
    if (_use_sqrt_controller && !is_zero(get_accel_pitch_max_radss())) {
        rate_target_ang_vel.y = sqrt_controller(attitude_error_rot_vec_rad.y, _p_angle_pitch.kP(), constrain_float(get_accel_pitch_max_radss() / 2.0f, AC_ATTITUDE_ACCEL_RP_CONTROLLER_MIN_RADSS, AC_ATTITUDE_ACCEL_RP_CONTROLLER_MAX_RADSS), _dt);
    } else {
        rate_target_ang_vel.y = _p_angle_pitch.kP() * attitude_error_rot_vec_rad.y;
    }

    // Compute the yaw angular velocity demand from the yaw angle error
    if (_use_sqrt_controller && !is_zero(get_accel_yaw_max_radss())) {
        rate_target_ang_vel.z = sqrt_controller(attitude_error_rot_vec_rad.z, _p_angle_yaw.kP(), constrain_float(get_accel_yaw_max_radss() / 2.0f, AC_ATTITUDE_ACCEL_Y_CONTROLLER_MIN_RADSS, AC_ATTITUDE_ACCEL_Y_CONTROLLER_MAX_RADSS), _dt);
    } else {
        rate_target_ang_vel.z = _p_angle_yaw.kP() * attitude_error_rot_vec_rad.z;
    }
    return rate_target_ang_vel;
}

// Enable or disable body-frame feed forward
void AC_AttitudeControl::accel_limiting(bool enable_limits)
{
    if (enable_limits) {
        // If enabling limits, reload from eeprom or set to defaults
        if (is_zero(_accel_roll_max)) {
            _accel_roll_max.load();
        }
        if (is_zero(_accel_pitch_max)) {
            _accel_pitch_max.load();
        }
        if (is_zero(_accel_yaw_max)) {
            _accel_yaw_max.load();
        }
    } else {
        _accel_roll_max = 0.0f;
        _accel_pitch_max = 0.0f;
        _accel_yaw_max = 0.0f;
    }
}

// Return tilt angle limit for pilot input that prioritises altitude hold over lean angle
float AC_AttitudeControl::get_althold_lean_angle_max() const
{
    // convert to centi-degrees for public interface
    return MAX(ToDeg(_althold_lean_angle_max), AC_ATTITUDE_CONTROL_ANGLE_LIMIT_MIN) * 100.0f;
}

// Return roll rate step size in centidegrees/s that results in maximum output after 4 time steps
float AC_AttitudeControl::max_rate_step_bf_roll()
{
    float alpha = MIN(get_rate_roll_pid().get_filt_E_alpha(), get_rate_roll_pid().get_filt_D_alpha());
    float alpha_remaining = 1 - alpha;
    // todo: When a thrust_max is available we should replace 0.5f with 0.5f * _motors.thrust_max
    float throttle_hover = constrain_float(_motors.get_throttle_hover(), 0.1f, 0.5f);
    return 2.0f * throttle_hover * AC_ATTITUDE_RATE_RP_CONTROLLER_OUT_MAX / ((alpha_remaining * alpha_remaining * alpha_remaining * alpha * get_rate_roll_pid().kD()) / _dt + get_rate_roll_pid().kP());
}

// Return pitch rate step size in centidegrees/s that results in maximum output after 4 time steps
float AC_AttitudeControl::max_rate_step_bf_pitch()
{
    float alpha = MIN(get_rate_pitch_pid().get_filt_E_alpha(), get_rate_pitch_pid().get_filt_D_alpha());
    float alpha_remaining = 1 - alpha;
    // todo: When a thrust_max is available we should replace 0.5f with 0.5f * _motors.thrust_max
    float throttle_hover = constrain_float(_motors.get_throttle_hover(), 0.1f, 0.5f);
    return 2.0f * throttle_hover * AC_ATTITUDE_RATE_RP_CONTROLLER_OUT_MAX / ((alpha_remaining * alpha_remaining * alpha_remaining * alpha * get_rate_pitch_pid().kD()) / _dt + get_rate_pitch_pid().kP());
}

// Return yaw rate step size in centidegrees/s that results in maximum output after 4 time steps
float AC_AttitudeControl::max_rate_step_bf_yaw()
{
    float alpha = MIN(get_rate_yaw_pid().get_filt_E_alpha(), get_rate_yaw_pid().get_filt_D_alpha());
    float alpha_remaining = 1 - alpha;
    // todo: When a thrust_max is available we should replace 0.5f with 0.5f * _motors.thrust_max
    float throttle_hover = constrain_float(_motors.get_throttle_hover(), 0.1f, 0.5f);
    return 2.0f * throttle_hover * AC_ATTITUDE_RATE_YAW_CONTROLLER_OUT_MAX / ((alpha_remaining * alpha_remaining * alpha_remaining * alpha * get_rate_yaw_pid().kD()) / _dt + get_rate_yaw_pid().kP());
}

bool AC_AttitudeControl::pre_arm_checks(const char *param_prefix,
                                        char *failure_msg,
                                        const uint8_t failure_msg_len)
{
    // validate AC_P members:
    const struct {
        const char *pid_name;
        AC_P &p;
    } ps[] = {
        { "ANG_PIT", get_angle_pitch_p() },
        { "ANG_RLL", get_angle_roll_p() },
        { "ANG_YAW", get_angle_yaw_p() }
    };
    for (uint8_t i=0; i<ARRAY_SIZE(ps); i++) {
        // all AC_P's must have a positive P value:
        if (!is_positive(ps[i].p.kP())) {
            hal.util->snprintf(failure_msg, failure_msg_len, "%s_%s_P must be > 0", param_prefix, ps[i].pid_name);
            return false;
        }
    }

    // validate AC_PID members:
    const struct {
        const char *pid_name;
        AC_PID &pid;
    } pids[] = {
        { "RAT_RLL", get_rate_roll_pid() },
        { "RAT_PIT", get_rate_pitch_pid() },
        { "RAT_YAW", get_rate_yaw_pid() },
    };
    for (uint8_t i=0; i<ARRAY_SIZE(pids); i++) {
        // if the PID has a positive FF then we just ensure kP and
        // kI aren't negative
        AC_PID &pid = pids[i].pid;
        const char *pid_name = pids[i].pid_name;
        if (is_positive(pid.ff())) {
            // kP and kI must be non-negative:
            if (is_negative(pid.kP())) {
                hal.util->snprintf(failure_msg, failure_msg_len, "%s_%s_P must be >= 0", param_prefix, pid_name);
                return false;
            }
            if (is_negative(pid.kI())) {
                hal.util->snprintf(failure_msg, failure_msg_len, "%s_%s_I must be >= 0", param_prefix, pid_name);
                return false;
            }
        } else {
            // kP and kI must be positive:
            if (!is_positive(pid.kP())) {
                hal.util->snprintf(failure_msg, failure_msg_len, "%s_%s_P must be > 0", param_prefix, pid_name);
                return false;
            }
            if (!is_positive(pid.kI())) {
                hal.util->snprintf(failure_msg, failure_msg_len, "%s_%s_I must be > 0", param_prefix, pid_name);
                return false;
            }
        }
        // never allow a negative D term (but zero is allowed)
        if (is_negative(pid.kD())) {
            hal.util->snprintf(failure_msg, failure_msg_len, "%s_%s_D must be >= 0", param_prefix, pid_name);
            return false;
        }
    }
    return true;
}
