/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnForwardLowPt.h"
#include "math.h"

double MuGirlNS::AnnForwardLowPt::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8) 
{
   m_input0 = (in0 - 0)/1;
   m_input1 = (in1 - 0)/1;
   m_input2 = (in2 - 0)/1;
   m_input3 = (in3 - 0)/1;
   m_input4 = (in4 - 0)/1;
   m_input5 = (in5 - 0)/1;
   m_input6 = (in6 - 0)/1;
   m_input7 = (in7 - 0)/1;
   m_input8 = (in8 - 0)/1;
   switch(index)
   {
     case 0:
         return ((neuron0C12B100()*1)+0);
     default:
         return 0.;
   }
}

double MuGirlNS::AnnForwardLowPt::neuron0C111ED8() 
{
   return m_input0;
}

double MuGirlNS::AnnForwardLowPt::neuron0C196990() 
{
   return m_input1;
}

double MuGirlNS::AnnForwardLowPt::neuron0C196AB0() 
{
   return m_input2;
}

double MuGirlNS::AnnForwardLowPt::neuron0C196C18() 
{
   return m_input3;
}

double MuGirlNS::AnnForwardLowPt::neuron0C085B00() 
{
   return m_input4;
}

double MuGirlNS::AnnForwardLowPt::neuron0C085C68() 
{
   return m_input5;
}

double MuGirlNS::AnnForwardLowPt::neuron0C085DD0() 
{
   return m_input6;
}

double MuGirlNS::AnnForwardLowPt::neuron0C09E4C0() 
{
   return m_input7;
}

double MuGirlNS::AnnForwardLowPt::neuron0C09E598() 
{
   return m_input8;
}

double MuGirlNS::AnnForwardLowPt::neuron0C09E820() 
{
   double input = -1.03584;
   input += synapse0C09E988();
   input += synapse0C09E9B8();
   input += synapse0C09E9E8();
   input += synapse0C09EA18();
   input += synapse0C1327E0();
   input += synapse0C132810();
   input += synapse0C132840();
   input += synapse0C132870();
   input += synapse0C1328A0();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnForwardLowPt::neuron0C1328D0() 
{
   double input = -0.606553;
   input += synapse0C132A58();
   input += synapse0C132A88();
   input += synapse0C132AB8();
   input += synapse0C132AE8();
   input += synapse0C132B18();
   input += synapse0C132B48();
   input += synapse0C132B78();
   input += synapse0C132BA8();
   input += synapse0C132BD8();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnForwardLowPt::neuron0C132C08() 
{
   double input = 0.0786299;
   input += synapse0C132D48();
   input += synapse0C132D78();
   input += synapse0C132DA8();
   input += synapse0C132DD8();
   input += synapse0C0B6998();
   input += synapse0C0B69C8();
   input += synapse0C0B69F8();
   input += synapse0C0B6A28();
   input += synapse0C0B6A58();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnForwardLowPt::neuron0C0B6A88() 
{
   double input = 0.658156;
   input += synapse0C0B6C10();
   input += synapse0C0B6C40();
   input += synapse0C0B6C70();
   input += synapse0C0B6CA0();
   input += synapse0C0B6CD0();
   input += synapse0C0B6D00();
   input += synapse0C0B6D30();
   input += synapse0C0B6D60();
   input += synapse0C0B6D90();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnForwardLowPt::neuron0C0B6DC0() 
{
   double input = -0.0707987;
   input += synapse0C0B6F48();
   input += synapse0C0B6F78();
   input += synapse0C0B6FA8();
   input += synapse0C0B6FD8();
   input += synapse0C101728();
   input += synapse0C101758();
   input += synapse0C101788();
   input += synapse0C1017B8();
   input += synapse0C1017E8();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnForwardLowPt::neuron0C101818() 
{
   double input = -0.333467;
   input += synapse0C1019A0();
   input += synapse0C1019D0();
   input += synapse0C101A00();
   input += synapse0C101A30();
   input += synapse0C101A60();
   input += synapse0C101A90();
   input += synapse0C101AC0();
   input += synapse0C101AF0();
   input += synapse0C101B20();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnForwardLowPt::neuron0C101B50() 
{
   double input = -3.71794;
   input += synapse0C101CD8();
   input += synapse0C101D08();
   input += synapse0C101D38();
   input += synapse0C101D68();
   input += synapse0C101D98();
   input += synapse0C101DC8();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnForwardLowPt::neuron0C12B100() 
{
   double input = -0.0753863;
   input += synapse0C101E18();
   return input;
}

double MuGirlNS::AnnForwardLowPt::synapse0C09E988() 
{
   return (neuron0C111ED8()*0.499253);
}

double MuGirlNS::AnnForwardLowPt::synapse0C09E9B8() 
{
   return (neuron0C196990()*-0.39075);
}

double MuGirlNS::AnnForwardLowPt::synapse0C09E9E8() 
{
   return (neuron0C196AB0()*-0.226807);
}

double MuGirlNS::AnnForwardLowPt::synapse0C09EA18() 
{
   return (neuron0C196C18()*-0.857606);
}

double MuGirlNS::AnnForwardLowPt::synapse0C1327E0() 
{
   return (neuron0C085B00()*2.45063);
}

double MuGirlNS::AnnForwardLowPt::synapse0C132810() 
{
   return (neuron0C085C68()*-0.213188);
}

double MuGirlNS::AnnForwardLowPt::synapse0C132840() 
{
   return (neuron0C085DD0()*0.30709);
}

double MuGirlNS::AnnForwardLowPt::synapse0C132870() 
{
   return (neuron0C09E4C0()*-0.108834);
}

double MuGirlNS::AnnForwardLowPt::synapse0C1328A0() 
{
   return (neuron0C09E598()*0.00995678);
}

double MuGirlNS::AnnForwardLowPt::synapse0C132A58() 
{
   return (neuron0C111ED8()*-0.148643);
}

double MuGirlNS::AnnForwardLowPt::synapse0C132A88() 
{
   return (neuron0C196990()*-0.704317);
}

double MuGirlNS::AnnForwardLowPt::synapse0C132AB8() 
{
   return (neuron0C196AB0()*-0.0499677);
}

double MuGirlNS::AnnForwardLowPt::synapse0C132AE8() 
{
   return (neuron0C196C18()*0.314179);
}

double MuGirlNS::AnnForwardLowPt::synapse0C132B18() 
{
   return (neuron0C085B00()*2.80571);
}

double MuGirlNS::AnnForwardLowPt::synapse0C132B48() 
{
   return (neuron0C085C68()*-0.236707);
}

double MuGirlNS::AnnForwardLowPt::synapse0C132B78() 
{
   return (neuron0C085DD0()*1.56175);
}

double MuGirlNS::AnnForwardLowPt::synapse0C132BA8() 
{
   return (neuron0C09E4C0()*-0.763496);
}

double MuGirlNS::AnnForwardLowPt::synapse0C132BD8() 
{
   return (neuron0C09E598()*-0.0607138);
}

double MuGirlNS::AnnForwardLowPt::synapse0C132D48() 
{
   return (neuron0C111ED8()*0.127267);
}

double MuGirlNS::AnnForwardLowPt::synapse0C132D78() 
{
   return (neuron0C196990()*0.11724);
}

double MuGirlNS::AnnForwardLowPt::synapse0C132DA8() 
{
   return (neuron0C196AB0()*0.544048);
}

double MuGirlNS::AnnForwardLowPt::synapse0C132DD8() 
{
   return (neuron0C196C18()*0.0372102);
}

double MuGirlNS::AnnForwardLowPt::synapse0C0B6998() 
{
   return (neuron0C085B00()*-0.280155);
}

double MuGirlNS::AnnForwardLowPt::synapse0C0B69C8() 
{
   return (neuron0C085C68()*-0.217733);
}

double MuGirlNS::AnnForwardLowPt::synapse0C0B69F8() 
{
   return (neuron0C085DD0()*0.301456);
}

double MuGirlNS::AnnForwardLowPt::synapse0C0B6A28() 
{
   return (neuron0C09E4C0()*-0.360104);
}

double MuGirlNS::AnnForwardLowPt::synapse0C0B6A58() 
{
   return (neuron0C09E598()*-0.651347);
}

double MuGirlNS::AnnForwardLowPt::synapse0C0B6C10() 
{
   return (neuron0C111ED8()*-0.0716189);
}

double MuGirlNS::AnnForwardLowPt::synapse0C0B6C40() 
{
   return (neuron0C196990()*0.920144);
}

double MuGirlNS::AnnForwardLowPt::synapse0C0B6C70() 
{
   return (neuron0C196AB0()*0.184487);
}

double MuGirlNS::AnnForwardLowPt::synapse0C0B6CA0() 
{
   return (neuron0C196C18()*-0.178441);
}

double MuGirlNS::AnnForwardLowPt::synapse0C0B6CD0() 
{
   return (neuron0C085B00()*-0.49349);
}

double MuGirlNS::AnnForwardLowPt::synapse0C0B6D00() 
{
   return (neuron0C085C68()*-0.582252);
}

double MuGirlNS::AnnForwardLowPt::synapse0C0B6D30() 
{
   return (neuron0C085DD0()*-0.978331);
}

double MuGirlNS::AnnForwardLowPt::synapse0C0B6D60() 
{
   return (neuron0C09E4C0()*0.288213);
}

double MuGirlNS::AnnForwardLowPt::synapse0C0B6D90() 
{
   return (neuron0C09E598()*-0.0732074);
}

double MuGirlNS::AnnForwardLowPt::synapse0C0B6F48() 
{
   return (neuron0C111ED8()*1.14386);
}

double MuGirlNS::AnnForwardLowPt::synapse0C0B6F78() 
{
   return (neuron0C196990()*0.686925);
}

double MuGirlNS::AnnForwardLowPt::synapse0C0B6FA8() 
{
   return (neuron0C196AB0()*-0.697928);
}

double MuGirlNS::AnnForwardLowPt::synapse0C0B6FD8() 
{
   return (neuron0C196C18()*0.293238);
}

double MuGirlNS::AnnForwardLowPt::synapse0C101728() 
{
   return (neuron0C085B00()*0.312101);
}

double MuGirlNS::AnnForwardLowPt::synapse0C101758() 
{
   return (neuron0C085C68()*0.574962);
}

double MuGirlNS::AnnForwardLowPt::synapse0C101788() 
{
   return (neuron0C085DD0()*1.73638);
}

double MuGirlNS::AnnForwardLowPt::synapse0C1017B8() 
{
   return (neuron0C09E4C0()*-0.40674);
}

double MuGirlNS::AnnForwardLowPt::synapse0C1017E8() 
{
   return (neuron0C09E598()*-0.393543);
}

double MuGirlNS::AnnForwardLowPt::synapse0C1019A0() 
{
   return (neuron0C111ED8()*0.0751364);
}

double MuGirlNS::AnnForwardLowPt::synapse0C1019D0() 
{
   return (neuron0C196990()*-0.996166);
}

double MuGirlNS::AnnForwardLowPt::synapse0C101A00() 
{
   return (neuron0C196AB0()*-0.427391);
}

double MuGirlNS::AnnForwardLowPt::synapse0C101A30() 
{
   return (neuron0C196C18()*0.00366549);
}

double MuGirlNS::AnnForwardLowPt::synapse0C101A60() 
{
   return (neuron0C085B00()*-0.605326);
}

double MuGirlNS::AnnForwardLowPt::synapse0C101A90() 
{
   return (neuron0C085C68()*0.182912);
}

double MuGirlNS::AnnForwardLowPt::synapse0C101AC0() 
{
   return (neuron0C085DD0()*-0.417895);
}

double MuGirlNS::AnnForwardLowPt::synapse0C101AF0() 
{
   return (neuron0C09E4C0()*-0.0742235);
}

double MuGirlNS::AnnForwardLowPt::synapse0C101B20() 
{
   return (neuron0C09E598()*0.877931);
}

double MuGirlNS::AnnForwardLowPt::synapse0C101CD8() 
{
   return (neuron0C09E820()*2.04669);
}

double MuGirlNS::AnnForwardLowPt::synapse0C101D08() 
{
   return (neuron0C1328D0()*1.81615);
}

double MuGirlNS::AnnForwardLowPt::synapse0C101D38() 
{
   return (neuron0C132C08()*-0.7936);
}

double MuGirlNS::AnnForwardLowPt::synapse0C101D68() 
{
   return (neuron0C0B6A88()*-1.13528);
}

double MuGirlNS::AnnForwardLowPt::synapse0C101D98() 
{
   return (neuron0C0B6DC0()*0.508559);
}

double MuGirlNS::AnnForwardLowPt::synapse0C101DC8() 
{
   return (neuron0C101818()*-0.820638);
}

double MuGirlNS::AnnForwardLowPt::synapse0C101E18() 
{
   return (neuron0C101B50()*2.17279);
}

