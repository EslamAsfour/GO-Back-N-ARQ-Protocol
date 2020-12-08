// Back-N-Go__Protocol.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <deque>
#include "Protocol.h"
#include <iostream>
#include <string>
using namespace std;
// Simulate the Physical Layer as a buffer
deque<frame> Physical_Buffer;
Seq_Num Physical_NBuffered = 0;

void Sender(Event_Type Newevent, char data = NULL);
void Receiver(Event_Type Newevent);
void SendData(Seq_Num FrameSeq, Seq_Num FrameExpected, char buffer[]);
void SendAck(Seq_Num FrameSeq, Seq_Num FrameExpected);

int main()
{
	string ToBeSent = "NetWork";
	for (int i = 0; i < MAX_SEQ; i++)
	{
		Sender(Network_Layer_Ready, ToBeSent[i]);
	}
	for (int i = MAX_SEQ; i < ToBeSent.length(); i++)
	{
		Receiver(Frame_Arrival);
		Sender(Frame_Arrival);
		Sender(Network_Layer_Ready, ToBeSent[i]);
	}
	Sender(Time_Out);
	for (int i = 0 ;  i < MAX_SEQ ; i++)
	{
		Receiver(Frame_Arrival);
		Sender(Frame_Arrival);
	}
    return 0;
}

// 2nd TestCase
/*
int main()
	{
	string ToBeSent = "NetWork";
	for (int i = 0; i < MAX_SEQ; i++)
	{
	Sender(Network_Layer_Ready, ToBeSent[i]);
	}
	for (int i = MAX_SEQ; i < ToBeSent.length(); i++)
	{
	Receiver(Frame_Arrival);
	Sender(Frame_Arrival);
	Sender(Network_Layer_Ready, ToBeSent[i]);
	}
	for (int i = 0 ;  i < MAX_SEQ ; i++)
	{
	Receiver(Frame_Arrival);
	Sender(Frame_Arrival);
	}
	return 0;
}

*/



// Sender can handle 3 Events 
//	  1- Network Layer Ready to send    (Needs Data)
//	  2- Physical Layer Have sent message
//    3- TimeOut  "Resend"

// "Char Data" is for the sending msg Event

void Sender(Event_Type Newevent, char data)
{
 // Static Variable to save the latest values when exiting the function
	static char Buffer[MAX_SEQ+1];
	static Seq_Num next_Frame_to_Send = 0;
	static Seq_Num ack_Expected = 0;
	static Seq_Num frame_expected = 0;
	static Seq_Num nBuffered = 0;

	switch (Newevent)
	{
	case Network_Layer_Ready:
		Buffer[next_Frame_to_Send] = data;
		nBuffered++;
		//Send Data Function
		SendData(next_Frame_to_Send, frame_expected, Buffer);
		inc(next_Frame_to_Send);
		break;
	// In the Case of Sender The Frames that he receives will be carrying the Ack
	case Frame_Arrival:
		frame Arrived = Physical_Buffer.back();
		Physical_Buffer.pop_back();
//	Check for the Ack 
		if (Arrived.Ack == ack_Expected)
		{
			cout << "Info= " << Buffer[ack_Expected] << " Acknowledged" << endl;
			nBuffered--;
			inc(ack_Expected);
		}
		else
			cout << "Expected Acknowledgment is Not Received" << endl;
		break;
	case Time_Out:
		cout << "-----------Time Out ----------- " << endl;
		next_Frame_to_Send = ack_Expected;
		for (int i = 1; i <= nBuffered; i++)
		{
			SendData(next_Frame_to_Send, frame_expected, Buffer);
			inc(next_Frame_to_Send);
		}
		break;
	}

}

/*
	Events Handled 
		1- Frame_Arrival
*/
void Receiver(Event_Type Newevent)
{

	static Seq_Num next_Frame_to_Send = 0;
	static Seq_Num frame_expected = 0;
	switch (Newevent) 
	{
		case Frame_Arrival:
			frame Arrived = Physical_Buffer.front();
			Physical_Buffer.pop_front();
			if (Arrived.seq_Num == frame_expected)
			{
				cout << "Frame Recieved Succ Seq_Num = " << Arrived.seq_Num << " Info= " << Arrived.Info << endl;
			// Send Ack for the received data
				SendAck(next_Frame_to_Send, frame_expected);
				inc(frame_expected);
				inc(next_Frame_to_Send);
			}
			else
			{
				cout << "Error in Received Data" << endl;
				SendAck(15, 15);  // AnyNumber As a flag for the NAck Frame
			}
			
		break;

	}
}




/*	Function Creates the Frame and Push it to the queue of the Physical Layer */
		// Input	1- Frame SeqNumber
		//			2- Ack
		//			3- Char
void SendData(Seq_Num FrameSeq , Seq_Num FrameExpected , char buffer[])
{
	frame Frame;
	Frame.Info = buffer[FrameSeq];
	Frame.seq_Num = FrameSeq;
	Frame.Ack = (FrameExpected + MAX_SEQ) % (MAX_SEQ + 1);
	Physical_Buffer.push_back (Frame);
	Physical_NBuffered++;
	cout << "Info = " << Frame.Info << "  Is sent" << endl;
}

void SendAck(Seq_Num FrameSeq, Seq_Num FrameExpected)
{
	frame Frame;
	Frame.Info = NULL;
	Frame.seq_Num = FrameSeq;
	Frame.Ack = FrameExpected;
	Physical_Buffer.push_back(Frame);
	Physical_NBuffered++;
	
}
