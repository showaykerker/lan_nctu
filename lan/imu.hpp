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

#define abs(a) (a<0)?(-a):(a)
#define ZC_LEN 1000
#define LPF_YZ_ACC_PREV 0.9
#define ZC_IGNORE_LEN 5

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
		
		float get_d_acc_yz(void){
			return this->d_acc_yz;
		}
		
		float get_delay_time(void){
			return this->zc_delay_time_so_far;
		}
		
		uint8_t update(void){
			this->status_code = 0;
						
			if (this->imu.accelAvailable()){
				this->imu.readAccel();
				this->acc[0] = this->imu.calcAccel(imu.ax);
				this->acc[1] = this->imu.calcAccel(imu.ay);
				this->acc[2] = this->imu.calcAccel(imu.az);
				this->RP[0] = atan2(this->imu.ay, this->imu.az) * 180.0 / PI;
				this->RP[1] = atan2(-this->imu.ax, sqrt(this->imu.ay*this->imu.ay+this->imu.az*this->imu.az)) * 180.0 / PI;
				
				// Assume sampling rate is the same. Calculate acc difference based on this assumption.
				float curr_acc_yz = pow(this->acc[1]*this->acc[1] + this->acc[2] * this->acc[2], 0.5);
				this->d_acc_yz = abs(curr_acc_yz-this->acc_yz);
				this->acc_yz = curr_acc_yz;
				// Map d_acc_yz to 0~255 than apply a simple low-pass filter in main function.
				
				this->find_zero(curr_acc_yz); // Finding Zero-to-Zero Length
				
			}
			else{
				this->status_code += 1;
				for(int i = 0; i<=2; i++) this->acc[i] = -1;
				for(int i = 0; i<=1; i++) this->RP[i] = -1;
			}
			
			/*
			if (this->imu.gyroAvailable()){
				this->imu.readGyro();
				this->gyr[0] = this->imu.calcGyro(imu.gx);
				this->gyr[1] = this->imu.calcGyro(imu.gy);
				this->gyr[2] = this->imu.calcGyro(imu.gz);
			}
			else{
				this->status_code += 2;
				for(int i = 0; i<=2; i++) this->gyr[i] = -1;
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
			*/
			
			return this->status_code;
		}
		float gyr[3]={0}, acc[3]={0}, mag[3]={0}, RP[2]={0};
		
	private:
		LSM9DS1 imu;
		uint8_t status_code = 0;
		float acc_yz = 0.0, d_acc_yz = 0.0;
		
		// Find Zero Value for Delay Time Calculation.
		float zc_last_val = 0.0;
		uint16_t zc_i, zc_last_i = 0;
		uint16_t zc_delay_time_so_far = 1;
		
		void find_zero(float curr){
			curr = (curr - 0.98) * 100;
			float now_zc_val = this->zc_last_val * LPF_YZ_ACC_PREV + curr * (1-LPF_YZ_ACC_PREV);
			//Serial.print((now_zc_val)); Serial.print(",");
			//Serial.print((this->zc_last_val)); Serial.print(",");
			//Serial.print(this->zc_delay_time_so_far);
			
			
			// Check if this time step is the target, 
			// if not, simply use the previous value.
			if ((now_zc_val-1) * this->zc_last_val < 0){	// if so, update zc_delay_time_so_far
				uint16_t dist = 0;
				if (this->zc_i >= this->zc_last_i)
					dist = this->zc_i - this->zc_last_i;
				else 
					dist = ZC_LEN - (this->zc_last_i - this->zc_i);
				if(dist > ZC_IGNORE_LEN){
					this->zc_delay_time_so_far = dist;
					this->zc_last_i = this->zc_i;
				}
			}
			this->zc_last_val = now_zc_val;
			this->zc_i = (this->zc_i + 1) %ZC_LEN;
		}
		
		
	
}

#endif