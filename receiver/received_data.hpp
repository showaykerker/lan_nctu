#ifndef RECEIVED_DATA_HPP
#define RECEIVED_DATA_HPP

//S,190965,48,0.58,0.13,0.75,-1.48,-1.46,-2.45,E

class rec_data{
public:
	rec_data(void){}
	void clean(void){
		this->t = 0;
		this->val = 0;
		for(int i = 0;i<=2;i++){
			this->acc[i] = 0;
			this->gyr[i] = 0;
		}
		this->curr = 0;
	}

	unsigned long long int t = 0;
	uint8_t val = 0;
	float acc[3]={0};
	float gyr[3]={0};
	uint8_t curr = 0;
};
#endif
