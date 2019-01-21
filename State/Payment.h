/*
 * Payment.h
 *
 * Created: 17-1-2019 12:05:31
 *  Author: Wesley
 */ 


#ifndef PAYMENT_H_
#define PAYMENT_H_

namespace Payment
{
	enum class State
	{
		Idle			= 0x00,
		AwaitingCoin    = 0x01,
		Scan			= 0x02,
		VerifyPayment	= 0x03,		
	};

}

#endif /* PAYMENT_H_ */