/*
* Brian R Taylor
* brian.taylor@bolderflight.com
* 
* Copyright (c) 2020 Bolder Flight Systems
*/

#ifndef INCLUDE_NAVIGATION_EKF_15_STATE_H_
#define INCLUDE_NAVIGATION_EKF_15_STATE_H_

#include "types/types.h"
#include "navigation/utils.h"
#include "navigation/transforms.h"
#include "navigation/tilt_compass.h"
#include "global_defs/global_defs.h"

namespace navigation {

class Ekf15State {
 public:
  Ekf15State() {}
  /* Sensor characteristics setters */
  inline void accel_std_mps2(float val) {accel_std_mps2_ = val;}
  inline void accel_markov_bias_std_mps2(float val) {accel_markov_bias_std_mps2_ = val;}
  inline void accel_tau_s(float val) {accel_tau_s_ = val;}
  inline void gyro_std_radps(float val) {gyro_std_radps_ = val;}
  inline void gyro_markov_bias_std_radps(float val) {gyro_markov_bias_std_radps_ = val;}
  inline void gyro_tau_s(float val) {gyro_tau_s_ = val;}
  inline void gnss_pos_ne_std_m(float val) {gnss_pos_ne_std_m_ = val;}
  inline void gnss_pos_d_std_m(float val) {gnss_pos_d_std_m_ = val;}
  inline void gnss_vel_ne_std_mps(float val) {gnss_vel_ne_std_mps_ = val;}
  inline void gnss_vel_d_std_mps(float val) {gnss_vel_d_std_mps_ = val;}
  /* Sensor characteristics getters */
  inline float accel_std_mps2() {return accel_std_mps2_;}
  inline float accel_markov_bias_std_mps2() {return accel_markov_bias_std_mps2_;}
  inline float accel_tau_s() {return accel_tau_s_;}
  inline float gyro_std_radps() {return gyro_std_radps_;}
  inline float gyro_markov_bias_std_radps() {return gyro_markov_bias_std_radps_;}
  inline float gyro_tau_s() {return gyro_tau_s_;}
  inline float gnss_pos_ne_std_m() {return gnss_pos_ne_std_m_;}
  inline float gnss_pos_d_std_m() {return gnss_pos_d_std_m_;}
  inline float gnss_vel_ne_std_mps() {return gnss_vel_ne_std_mps_;}
  inline float gnss_vel_d_std_mps() {return gnss_vel_d_std_mps_;}
  /* Initial covariance setters */
  inline void init_pos_err_std_m(float val) {init_pos_err_std_m_ = val;}
  inline void init_vel_err_std_mps(float val) {init_vel_err_std_mps_ = val;}
  inline void init_att_err_std_rad(float val) {init_att_err_std_rad_ = val;}
  inline void init_heading_err_std_rad(float val) {init_heading_err_std_rad_ = val;}
  inline void init_accel_bias_std_mps2(float val) {init_accel_bias_std_mps2_ = val;}
  inline void init_gyro_bias_std_radps(float val) {init_gyro_bias_std_radps_ = val;}
  /* Initial covariance getters */
  inline float init_pos_err_std_m() {return init_pos_err_std_m_;}
  inline float init_vel_err_std_mps() {return init_vel_err_std_mps_;}
  inline float init_att_err_std_rad() {return init_att_err_std_rad_;}
  inline float init_heading_err_std_rad() {return init_heading_err_std_rad_;}
  inline float init_accel_bias_std_mps2() {return init_accel_bias_std_mps2_;}
  inline float init_gyro_bias_std_radps() {return init_gyro_bias_std_radps_;}
  /* Configure the EKF */
  void Configure() {
    /* Observation matrix */
    h_.block(0, 0, 5, 5) = Eigen::Matrix<float, 5, 5>::Identity();
    /* Process noise covariance */
    rw_.block(0, 0, 3, 3) = accel_std_mps2_ * accel_std_mps2_ * Eigen::Matrix<float, 3, 3>::Identity();
    rw_.block(3, 3, 3, 3) = gyro_std_radps_ * gyro_std_radps_ * Eigen::Matrix<float, 3, 3>::Identity();
    rw_.block(6, 6, 3, 3) = 2.0f * accel_markov_bias_std_mps2_ * accel_markov_bias_std_mps2_ / accel_tau_s_ * Eigen::Matrix<float, 3, 3>::Identity();
    rw_.block(9, 9, 3, 3) = 2.0f * gyro_markov_bias_std_radps_ * gyro_markov_bias_std_radps_ / gyro_tau_s_ * Eigen::Matrix<float, 3, 3>::Identity();
    /* Observation noise covariance */
    r_.block(0, 0, 2, 2) = gnss_pos_ne_std_m_ * gnss_pos_ne_std_m_ * Eigen::Matrix<float, 2, 2>::Identity();
    r_(2, 2) = gnss_pos_d_std_m_ * gnss_pos_d_std_m_;
    r_.block(3, 3, 2, 2) = gnss_vel_ne_std_mps_ * gnss_vel_ne_std_mps_ * Eigen::Matrix<float, 2, 2>::Identity();
    r_(5, 5) = gnss_vel_d_std_mps_ * gnss_vel_d_std_mps_;
    /* Initial covariance estimate */
    p_.block(0, 0, 3, 3) = init_pos_err_std_m_ * init_pos_err_std_m_ * Eigen::Matrix<float, 3, 3>::Identity();
    p_.block(3, 3, 3, 3) = init_vel_err_std_mps_ * init_vel_err_std_mps_ * Eigen::Matrix<float, 3, 3>::Identity();
    p_.block(6, 6, 2, 2) = init_att_err_std_rad_ * init_att_err_std_rad_ * Eigen::Matrix<float, 2, 2>::Identity();
    p_(8, 8) = init_heading_err_std_rad_ * init_heading_err_std_rad_;
    p_.block(9, 9, 3, 3) = init_accel_bias_std_mps2_ * init_accel_bias_std_mps2_ * Eigen::Matrix<float, 3, 3>::Identity();
    p_.block(12, 12, 3, 3) = init_gyro_bias_std_radps_ * init_gyro_bias_std_radps_ * Eigen::Matrix<float, 3, 3>::Identity();
    /* Markov bias matrices */
    accel_markov_bias_ = -1.0f / accel_tau_s_ * Eigen::Matrix<float, 3, 3>::Identity();
    gyro_markov_bias_ = -1.0f / gyro_tau_s_ * Eigen::Matrix<float, 3, 3>::Identity();
  }
  /* Initialize the EKF states */
  void Initialize(const types::Imuf &imu, const types::Mag3f &mag, const types::Gnssf &gnss) {
    /* Initialize position and velocity */
    ins_.lla_pos = gnss.lla_pos;
    ins_.ned_vel = gnss.ned_vel;
    /* Initialize sensor biases */
    gyro_bias_radps_ = imu.gyro.radps();
    /* New accelerations and rotation rates */
    ins_.gyro.radps(imu.gyro.radps() - gyro_bias_radps_);
    ins_.accel.mps2(imu.accel.mps2() - accel_bias_mps2_);
    /* Initialize pitch, roll, and heading */
    ins_.attitude.rad(TiltCompass(imu.accel.mps2(), mag.ut()));
    /* Euler to quaternion */
    quat_ = angle2quat(ins_.attitude.rad());
  }
  /* Perform a time update */
  types::Insf TimeUpdate(const types::Imuf &imu, const float dt_s) {
    /* A-priori accel and rotation rate estimate */
    ins_.accel.mps2(imu.accel.mps2() - accel_bias_mps2_);
    ins_.gyro.radps(imu.gyro.radps() - gyro_bias_radps_);
    /* Attitude update */
    delta_quat_.w() = 1.0f;
    delta_quat_.x() = 0.5f * ins_.gyro.radps()(0) * dt_s;
    delta_quat_.y() = 0.5f * ins_.gyro.radps()(1) * dt_s;
    delta_quat_.z() = 0.5f * ins_.gyro.radps()(2) * dt_s;
    quat_ = (quat_ * delta_quat_).normalized();
    /* Avoid quaternion sign flips */
    if (quat_.w() < 0) {
      quat_ = Eigen::Quaternionf(-quat_.w(), -quat_.x(), -quat_.y(), -quat_.z());
    }
    ins_.attitude.rad(quat2angle(quat_));
    /* Body to NED transformation from quat */
    t_b2ned = quat2dcm(quat_).transpose();
    /* Velocity update */
    ins_.ned_vel.mps(ins_.ned_vel.mps() + dt_s * (t_b2ned * ins_.accel.mps2() + GRAV_NED_MPS2_));
    /* Position update */
    ins_.lla_pos.rad_m(ins_.lla_pos.rad_m() + (dt_s * LlaRate(ins_.ned_vel.mps(), ins_.lla_pos.rad_m())).cast<double>());
    /* Jacobian */
    fs_.block(0, 3, 3, 3) = Eigen::Matrix<float, 3, 3>::Identity();
    fs_(5,2) = -2.0f * global::constants::G_MPS2<float> / constants::SEMI_MAJOR_AXIS_LENGTH_M;
    fs_.block(3, 6, 3, 3) = -2.0f * t_b2ned * Skew(ins_.accel.mps2());
    fs_.block(3, 9, 3, 3) = -t_b2ned;
    fs_.block(6, 6, 3, 3) = -Skew(ins_.gyro.radps());
    fs_.block(6, 12, 3, 3) = -0.5f * Eigen::Matrix<float, 3, 3>::Identity();
    fs_.block(9, 9, 3, 3) = accel_markov_bias_; // ... Accel Markov Bias
    fs_.block(12, 12, 3, 3) = gyro_markov_bias_; // ... Rotation Rate Markov Bias
    /* State transition matrix */
    phi_ = Eigen::Matrix<float, 15, 15>::Identity() + fs_ * dt_s;
    /* Process Noise Covariance (Discrete approximation) */
    gs_.block(3, 0, 3, 3) = -t_b2ned;
    gs_.block(6, 3, 3, 3) = -0.5f * Eigen::Matrix<float, 3, 3>::Identity();
    gs_.block(9, 6, 3, 3) = Eigen::Matrix<float, 3, 3>::Identity();
    gs_.block(12, 9, 3, 3) = Eigen::Matrix<float, 3, 3>::Identity();
    /* Discrete Process Noise */
    q_ = phi_ * dt_s * gs_ * rw_ * gs_.transpose();
    q_ = 0.5f * (q_ + q_.transpose());
    /* Covariance Time Update */
    p_ = phi_ * p_ * phi_.transpose() + q_;
    p_ = 0.5f * (p_ + p_.transpose());
    return ins_;
  }
  /* Perform a measurement update */
  types::Insf MeasurementUpdate(types::Gnssf &gnss) {
    /* Y, error between Measures and Outputs */
    y_.segment(0, 3) = lla2ned(gnss.lla_pos.rad_m(), ins_.lla_pos.rad_m()).cast<float>();
    y_.segment(3, 3) = gnss.ned_vel.mps() - ins_.ned_vel.mps();
    /* Innovation covariance */
    s_ = h_ * p_ * h_.transpose() + r_;
    /* Kalman gain */
    k_ = p_ * h_.transpose() * s_.inverse();
    /* Covariance update, P = (I + K * H) * P * (I + K * H)' + K * R * K' */
    p_ = (Eigen::Matrix<float, 15, 15>::Identity() - k_ * h_) * p_ * (Eigen::Matrix<float, 15, 15>::Identity() - k_ * h_).transpose() + k_ * r_ * k_.transpose();
    /* State update, x = K * y */
    x_ = k_ * y_;
    /* Position update */
    double denom = abs(1 - (constants::E2 * sin(ins_.lla_pos.lat.rad()) * sin(ins_.lla_pos.lat.rad())));
    double sqrt_denom = denom;
    double Rns = constants::SEMI_MAJOR_AXIS_LENGTH_M * (1 - constants::E2) / (denom * sqrt_denom); 
    double Rew = constants::SEMI_MAJOR_AXIS_LENGTH_M / sqrt_denom;
    ins_.lla_pos.alt.m(ins_.lla_pos.alt.m() - x_(2));
    ins_.lla_pos.lat.rad(ins_.lla_pos.lat.rad() + x_(0) / (Rew + ins_.lla_pos.alt.m()));
    ins_.lla_pos.lon.rad(ins_.lla_pos.lon.rad() + x_(1) / (Rns + ins_.lla_pos.alt.m()) / cos(ins_.lla_pos.lat.rad()));
    /* Velocity update */
    ins_.ned_vel.mps(ins_.ned_vel.mps() + x_.segment(3, 3));
    /* Attitude correction */
    delta_quat_.w() = 1.0f;
    delta_quat_.x() = x_(6);
    delta_quat_.y() = x_(7);
    delta_quat_.z() = x_(8);
    quat_ = (quat_ * delta_quat_).normalized();
    ins_.attitude.rad(quat2angle(quat_));
    /* Update biases from states */
    accel_bias_mps2_ += x_.segment(9, 3);
    gyro_bias_radps_ += x_.segment(12, 3);
    /* Update accelerometer and gyro */
    ins_.accel.mps2(ins_.accel.mps2() - x_.segment(9, 3));
    ins_.gyro.radps(ins_.gyro.radps() - x_.segment(12, 3));
    return ins_;
  }
  /*
  * Sensor characteristics - accel and gyro are modeled with a
  * Gauss-Markov model.
  */
  /* Standard deviation of accel noise */
  float accel_std_mps2_ = 0.05f;
  /* Standard deviation of accel Markov bias */
  float accel_markov_bias_std_mps2_ = 0.01f;
  /* Accel correlation time */
  float accel_tau_s_ = 100.0f;
  Eigen::Matrix3f accel_markov_bias_ = -1.0f / accel_tau_s_ * Eigen::Matrix<float, 3, 3>::Identity();
  /* Standard deviation of gyro noise */
  float gyro_std_radps_ = 0.00175f;
  /* Standard deviation of gyro Markov bias */
  float gyro_markov_bias_std_radps_ = 0.00025f;
  /* Gyro correlation time */
  float gyro_tau_s_ = 50.0f;
  Eigen::Matrix3f gyro_markov_bias_ = -1.0f / gyro_tau_s_ * Eigen::Matrix<float, 3, 3>::Identity();
  /* Standard deviation of the GNSS North and East position measurement */
  float gnss_pos_ne_std_m_ = 3.0f;
  /* Standard deviation of the GNSS Down position estimate */
  float gnss_pos_d_std_m_ = 6.0f;
  /* Standard deviation of the GNSS North and East velocity measurement */
  float gnss_vel_ne_std_mps_ = 0.5f;
  /* Standard deviation of the GNSS Down velocity measurement */
  float gnss_vel_d_std_mps_ = 1.0f;
  /*
  * Initial set of covariances
  */
  /* Standard deviation of the initial position error */
  float init_pos_err_std_m_ = 10.0f;
  /* Standard deviation of the initial velocity error */
  float init_vel_err_std_mps_ = 1.0f;
  /* Standard deviation of the initial attitude error */
  float init_att_err_std_rad_ = 0.34906f;
  /* Standard deviation of the initial heading error */
  float init_heading_err_std_rad_ = 3.14159f;
  /* Standard deviation of the initial accel bias */
  float init_accel_bias_std_mps2_ = 0.9810f;
  /* Standard deviation of the initial gyro bias */
  float init_gyro_bias_std_radps_ = 0.01745f;
  /*
  * Kalman filter matrices
  */
  /* Observation matrix */
  Eigen::Matrix<float, 6, 15> h_ = Eigen::Matrix<float, 6, 15>::Zero();
  /* Covariance of the observation noise */
  Eigen::Matrix<float, 6, 6> r_ = Eigen::Matrix<float, 6, 6>::Zero();
  /* Covariance of the Sensor Noise */
  Eigen::Matrix<float, 12, 12> rw_ = Eigen::Matrix<float, 12, 12>::Zero();
  /* Process Noise Covariance (Discrete approximation) */
  Eigen::Matrix<float,15,12> gs_ = Eigen::Matrix<float, 15, 12>::Zero();
  /* Innovation covariance */
  Eigen::Matrix<float, 6, 6> s_ = Eigen::Matrix<float, 6, 6>::Zero();
  /* Covariance estimate */
  Eigen::Matrix<float, 15, 15> p_ = Eigen::Matrix<float, 15, 15>::Zero();
  /* Discrete Process Noise */
  Eigen::Matrix<float, 15, 15> q_ = Eigen::Matrix<float, 15, 15>::Zero();
  /* Kalman gain */
  Eigen::Matrix<float, 15, 6> k_ = Eigen::Matrix<float, 15, 6>::Zero();
  /* Jacobian (state update matrix) */
  Eigen::Matrix<float, 15, 15> fs_ = Eigen::Matrix<float, 15, 15>::Zero();
  /* State transition */
  Eigen::Matrix<float,15,15> phi_ = Eigen::Matrix<float, 15, 15>::Zero();
  /* Error between measures and outputs */
  Eigen::Matrix<float, 6, 1> y_ = Eigen::Matrix<float, 6, 1>::Zero();
  /* State matrix */
  Eigen::Matrix<float, 15, 1> x_ = Eigen::Matrix<float, 15, 1>::Zero();
  /*
  * Constants
  */
  /* Graviational accel in NED */
  Eigen::Vector3f GRAV_NED_MPS2_ = (Eigen::Vector3f() << 0.0f, 0.0f, global::constants::G_MPS2<float>).finished();
  /*
  * Intermediates
  */
  /* Body to NED transform */
  Eigen::Matrix3f t_b2ned;
  /* Acceleration bias, m/s/s */
  Eigen::Vector3f accel_bias_mps2_ = Eigen::Vector3f::Zero();
  /* Rotation rate bias, rad/s */
  Eigen::Vector3f gyro_bias_radps_;
  /* Normalized accel */
  Eigen::Vector3f accel_norm_mps2_;
  /* Normalized mag */
  Eigen::Vector3f mag_norm_mps2_;
  /* Quaternion update */
  Eigen::Quaternionf delta_quat_;
  /* Quaternion */
  Eigen::Quaternionf quat_;
  /*
  * Data
  */
  types::Insf ins_;
};

}  // namespace navigation

#endif  // INCLUDE_NAVIGATION_EKF_15_STATE_H_