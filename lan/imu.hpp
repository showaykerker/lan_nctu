/************************************************
Jim Lindblom @ SparkFun Electronics
Original Creation Date: April 30, 2015
https://github.com/sparkfun/LSM9DS1_Breakout
************************************************/

#ifndef IMU_HPP
#define IMU_HPP

#include <Wire.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>
#include <Arduino.h>

class lanLSM9DS1{
	public:
		lanLSM9DS1(void){
			this->imu.settings.device.commInterface = IMU_MODE_I2C;
			this->imu.settings.device.mAddress = 0x1E;
			this->imu.settings.device.agAddress = 0x6B;
		}
		
		bool is_begin(void){
			return this->imu.begin();
		}
		
		float get_yz_acc(void){
			return pow(this->acc[1]*this->acc[1] + this->acc[2] * this->acc[2], 0.5);
		}
		
		uint8_t update(void){
			this->status_code = 0;
			if (this->imu.gyroAvailable()){
				this->imu.readGyro();
				this->gyr[0] = this->imu.calcGyro(imu.gx);
				this->gyr[1] = this->imu.calcGyro(imu.gy);
				this->gyr[2] = this->imu.calcGyro(imu.gz);
			}
			else{
				this->status_code += 1;
				for(int i = 0; i<=2; i++) this->gyr[i] = -1;
			}
			
			if (this->imu.accelAvailable()){
				this->imu.readAccel();
				this->acc[0] = this->imu.calcAccel(imu.ax);
				this->acc[1] = this->imu.calcAccel(imu.ay);
				this->acc[2] = this->imu.calcAccel(imu.az);
				this->RP[0] = atan2(this->imu.ay, this->imu.az) * 180.0 / PI;
				this->RP[1] = atan2(-this->imu.ax, sqrt(this->imu.ay*this->imu.ay+this->imu.az*this->imu.az)) * 180.0 / PI;
			}
			else{
				this->status_code += 2;
				for(int i = 0; i<=2; i++) this->acc[i] = -1;
				for(int i = 0; i<=1; i++) this->RP[i] = -1;
			}
			
			if (this->imu.magAvailable()){
				this->imu.readMag();
				this->mag[0] = this->imu.calcAccel(imu.mx);
				this->mag[1] = this->imu.calcAccel(imu.my);
				this->mag[2] = this->imu.calcAccel(imu.mz);
			}
			else{
				this->status_code += 4;
				for(int i = 0; i<=2; i++) this->mag[i] = -1;
			}
			
			return this->status_code;
		}
		float gyr[3]={0}, acc[3]={0}, mag[3]={0}, RP[2]={0};
	private:
		LSM9DS1 imu;
		uint8_t status_code = 0;
		
		
	
}

#endif