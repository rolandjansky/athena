/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnEndCapLowPt.h"
#include "math.h"

double MuGirlNS::AnnEndCapLowPt::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8) 
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
         return ((neuron0BEAAF60()*1)+0);
     default:
         return 0.;
   }
}

double MuGirlNS::AnnEndCapLowPt::neuron0BE8E650() 
{
   return m_input0;
}

double MuGirlNS::AnnEndCapLowPt::neuron0BE8DC70() 
{
   return m_input1;
}

double MuGirlNS::AnnEndCapLowPt::neuron0BDE6990() 
{
   return m_input2;
}

double MuGirlNS::AnnEndCapLowPt::neuron0BE37108() 
{
   return m_input3;
}

double MuGirlNS::AnnEndCapLowPt::neuron0BE37270() 
{
   return m_input4;
}

double MuGirlNS::AnnEndCapLowPt::neuron0BDB6008() 
{
   return m_input5;
}

double MuGirlNS::AnnEndCapLowPt::neuron0BDB6190() 
{
   return m_input6;
}

double MuGirlNS::AnnEndCapLowPt::neuron0DB7ECD0() 
{
   return m_input7;
}

double MuGirlNS::AnnEndCapLowPt::neuron0DB7EE38() 
{
   return m_input8;
}

double MuGirlNS::AnnEndCapLowPt::neuron0C4D8CC8() 
{
   double input = 0.0310509;
   input += synapse0C21E028();
   input += synapse0C17DCF0();
   input += synapse0BE90E98();
   input += synapse0BE90E68();
   input += synapse0BE90E38();
   input += synapse0C4D8DE8();
   input += synapse0C4D8E18();
   input += synapse0C4D8E48();
   input += synapse0C4D8E78();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnEndCapLowPt::neuron0C4D8EA8() 
{
   double input = -1.0897;
   input += synapse0C4D8FC8();
   input += synapse0BE27578();
   input += synapse0BE275A8();
   input += synapse0BE275D8();
   input += synapse0BE27608();
   input += synapse0BE27638();
   input += synapse0BE27668();
   input += synapse0BE27698();
   input += synapse0BE276C8();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnEndCapLowPt::neuron0BE276F8() 
{
   double input = -0.146021;
   input += synapse0BE27838();
   input += synapse0BE9B798();
   input += synapse0BE9B7C8();
   input += synapse0BE9B7F8();
   input += synapse0BE9B828();
   input += synapse0BE9B858();
   input += synapse0BE9B888();
   input += synapse0BE9B8B8();
   input += synapse0BE9B8E8();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnEndCapLowPt::neuron0BE9B918() 
{
   double input = 1.64571;
   input += synapse0BE9BAA0();
   input += synapse0BE33620();
   input += synapse0BE33650();
   input += synapse0BE33680();
   input += synapse0BE336B0();
   input += synapse0BE336E0();
   input += synapse0BE33710();
   input += synapse0BE33740();
   input += synapse0BE33770();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnEndCapLowPt::neuron0BE337A0() 
{
   double input = 1.01008;
   input += synapse0BE33928();
   input += synapse0BDE9008();
   input += synapse0BDE9038();
   input += synapse0BDE9068();
   input += synapse0BDE9098();
   input += synapse0BDE90C8();
   input += synapse0BDE90F8();
   input += synapse0BDE9128();
   input += synapse0BDE9158();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnEndCapLowPt::neuron0BDE9188() 
{
   double input = 0.152126;
   input += synapse0BDE92F0();
   input += synapse0BDE9320();
   input += synapse0BE8E728();
   input += synapse0BE8E758();
   input += synapse0BE8E788();
   input += synapse0BE8E7B8();
   input += synapse0BE8E7E8();
   input += synapse0BE8E818();
   input += synapse0BE8E848();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnEndCapLowPt::neuron0BE8E878() 
{
   double input = -3.10152;
   input += synapse0BE8E9E0();
   input += synapse0BE8EA10();
   input += synapse0BE8EA40();
   input += synapse0BE8EA70();
   input += synapse0BEAAF00();
   input += synapse0BEAAF30();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnEndCapLowPt::neuron0BEAAF60() 
{
   double input = -0.0918224;
   input += synapse0BEAB0E8();
   return input;
}

double MuGirlNS::AnnEndCapLowPt::synapse0C21E028() 
{
   return (neuron0BE8E650()*0.346734);
}

double MuGirlNS::AnnEndCapLowPt::synapse0C17DCF0() 
{
   return (neuron0BE8DC70()*-0.0244319);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE90E98() 
{
   return (neuron0BDE6990()*0.849589);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE90E68() 
{
   return (neuron0BE37108()*0.014242);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE90E38() 
{
   return (neuron0BE37270()*0.016892);
}

double MuGirlNS::AnnEndCapLowPt::synapse0C4D8DE8() 
{
   return (neuron0BDB6008()*0.342429);
}

double MuGirlNS::AnnEndCapLowPt::synapse0C4D8E18() 
{
   return (neuron0BDB6190()*0.68418);
}

double MuGirlNS::AnnEndCapLowPt::synapse0C4D8E48() 
{
   return (neuron0DB7ECD0()*-0.269735);
}

double MuGirlNS::AnnEndCapLowPt::synapse0C4D8E78() 
{
   return (neuron0DB7EE38()*0.344141);
}

double MuGirlNS::AnnEndCapLowPt::synapse0C4D8FC8() 
{
   return (neuron0BE8E650()*-0.163533);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE27578() 
{
   return (neuron0BE8DC70()*0.70741);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE275A8() 
{
   return (neuron0BDE6990()*-0.122956);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE275D8() 
{
   return (neuron0BE37108()*0.331573);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE27608() 
{
   return (neuron0BE37270()*-0.197392);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE27638() 
{
   return (neuron0BDB6008()*0.249755);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE27668() 
{
   return (neuron0BDB6190()*1.25283);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE27698() 
{
   return (neuron0DB7ECD0()*-0.57898);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE276C8() 
{
   return (neuron0DB7EE38()*0.0732253);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE27838() 
{
   return (neuron0BE8E650()*-0.17476);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE9B798() 
{
   return (neuron0BE8DC70()*0.389967);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE9B7C8() 
{
   return (neuron0BDE6990()*-0.689895);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE9B7F8() 
{
   return (neuron0BE37108()*-0.233199);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE9B828() 
{
   return (neuron0BE37270()*0.268112);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE9B858() 
{
   return (neuron0BDB6008()*0.498902);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE9B888() 
{
   return (neuron0BDB6190()*0.899733);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE9B8B8() 
{
   return (neuron0DB7ECD0()*0.198325);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE9B8E8() 
{
   return (neuron0DB7EE38()*0.155223);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE9BAA0() 
{
   return (neuron0BE8E650()*0.022489);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE33620() 
{
   return (neuron0BE8DC70()*0.490492);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE33650() 
{
   return (neuron0BDE6990()*-0.573932);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE33680() 
{
   return (neuron0BE37108()*0.340287);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE336B0() 
{
   return (neuron0BE37270()*0.676453);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE336E0() 
{
   return (neuron0BDB6008()*-0.199586);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE33710() 
{
   return (neuron0BDB6190()*-0.26395);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE33740() 
{
   return (neuron0DB7ECD0()*0.4689);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE33770() 
{
   return (neuron0DB7EE38()*0.0395493);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE33928() 
{
   return (neuron0BE8E650()*-0.159739);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BDE9008() 
{
   return (neuron0BE8DC70()*-0.411858);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BDE9038() 
{
   return (neuron0BDE6990()*-0.220773);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BDE9068() 
{
   return (neuron0BE37108()*-0.570657);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BDE9098() 
{
   return (neuron0BE37270()*0.321087);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BDE90C8() 
{
   return (neuron0BDB6008()*0.234944);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BDE90F8() 
{
   return (neuron0BDB6190()*-1.72046);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BDE9128() 
{
   return (neuron0DB7ECD0()*0.516699);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BDE9158() 
{
   return (neuron0DB7EE38()*0.0639727);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BDE92F0() 
{
   return (neuron0BE8E650()*1.08941);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BDE9320() 
{
   return (neuron0BE8DC70()*-0.165909);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE8E728() 
{
   return (neuron0BDE6990()*0.365998);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE8E758() 
{
   return (neuron0BE37108()*-0.02338);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE8E788() 
{
   return (neuron0BE37270()*0.397761);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE8E7B8() 
{
   return (neuron0BDB6008()*0.828881);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE8E7E8() 
{
   return (neuron0BDB6190()*0.674604);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE8E818() 
{
   return (neuron0DB7ECD0()*-0.398899);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE8E848() 
{
   return (neuron0DB7EE38()*0.462317);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE8E9E0() 
{
   return (neuron0C4D8CC8()*-0.713764);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE8EA10() 
{
   return (neuron0C4D8EA8()*1.99337);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE8EA40() 
{
   return (neuron0BE276F8()*0.835898);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BE8EA70() 
{
   return (neuron0BE9B918()*-1.08856);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BEAAF00() 
{
   return (neuron0BE337A0()*-1.64656);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BEAAF30() 
{
   return (neuron0BDE9188()*0.573119);
}

double MuGirlNS::AnnEndCapLowPt::synapse0BEAB0E8() 
{
   return (neuron0BE8E878()*2.71961);
}

