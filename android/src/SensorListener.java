// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright The XCSoar Project

package org.xcsoar;

/**
 * Handler for sensor values obtained from Java code.
 */
public interface SensorListener {
  void onConnected(int connected);

  /**
   * @param geoidAltitude is the GPS altitude above Geoid (true) or
   * above the WGS84 ellipsoid (false)?
   */
  void onLocationSensor(long time, int n_satellites,
                        double longitude, double latitude,
                        boolean hasAltitude, boolean geoidAltitude,
                        double altitude,
                        boolean hasBearing, double bearing,
                        boolean hasSpeed, double speed,
                        boolean hasAccuracy, double accuracy);

  void onAccelerationSensor1(double acceleration);
  void onAccelerationSensor(float ddx, float ddy, float ddz);
  void onRotationSensor(float dtheta_x, float dtheta_y, float dtheta_z);
  void onMagneticFieldSensor(float h_x, float h_y, float h_z);
  void onBarometricPressureSensor(float pressure, float sensor_noise_variance);
  void onPressureAltitudeSensor(float altitude);
  void onI2CbaroSensor(int index, int sensorType, int pressure);
  void onVarioSensor(float vario);
  void onHeartRateSensor(int bpm);
    /**
   * @param[in] has_cht_temp Is the Engine Cylinder Head Temperature sensor present?
   * @param[in] cht_temp Engine Cylinder Head Temperature.
   * @param[in] has_egt_temp Is the Engine Exhaust Gas Temperature sensor present?
   * @param[in] egt_temp Engine Exhaust Gas Temperature.
   * @param[in] has_revs_per_sec Is the Engine Revolutions Per Second sensor present?
   * @param[in] revs_per_sec Engine Revolutions Per Second, rotations per seconds of the camshaft.
   */
  void onEngineSensors(boolean has_cht_temp,
                       int cht_temp,
                       boolean has_egt_temp,
                       int egt_temp,
                       boolean has_revs_per_sec,
                       int revs_per_sec);

  void onVoltageValues(int temp_adc, int voltage_index, int volt_adc);

  void onNunchukValues(int joy_x, int joy_y,
                       int accel_x, int accel_y, int accel_z,
                       int switches);

  void onGliderLinkTraffic(long gid, String callsign,
                           double longitude, double latitude, double altitude,
                           double gspeed, double vspeed,
                           double bearing);

  void onTemperature(double temperature_kelvin);

  void onBatteryPercent(double battery_percent);

  /**
   * The state has changed, and AndroidSensor.getState() will provide
   * the new value.
   */
  void onSensorStateChanged();

  /**
   * An error has occurred, and the sensor is now defunct.
   *
   * @param msg a human-readable error message
   */
  void onSensorError(String msg);
}
