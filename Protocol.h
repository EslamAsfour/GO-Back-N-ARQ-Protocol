#pragma once
#define MAX_SEQ  3

typedef unsigned int Seq_Num;


typedef struct {
	Seq_Num Ack;
	char Info;
	Seq_Num seq_Num;
}frame;

typedef enum {Frame_Arrival , cksum_Error , Time_Out , Network_Layer_Ready}Event_Type;





#define inc(k) if (k < MAX_SEQ) k=k+1; else k=0