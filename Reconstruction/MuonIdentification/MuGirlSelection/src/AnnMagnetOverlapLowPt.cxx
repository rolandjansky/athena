/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnMagnetOverlapLowPt.h"
#include "math.h"

double MuGirlNS::AnnMagnetOverlapLowPt::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8) 
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
         return ((neuron0DEF7B60()*1)+0);
     default:
         return 0.;
   }
}

double MuGirlNS::AnnMagnetOverlapLowPt::neuron0DD81EB8() 
{
   return m_input0;
}

double MuGirlNS::AnnMagnetOverlapLowPt::neuron0DDA9BA8() 
{
   return m_input1;
}

double MuGirlNS::AnnMagnetOverlapLowPt::neuron0E155BC8() 
{
   return m_input2;
}

double MuGirlNS::AnnMagnetOverlapLowPt::neuron0E155D30() 
{
   return m_input3;
}

double MuGirlNS::AnnMagnetOverlapLowPt::neuron0E155E98() 
{
   return m_input4;
}

double MuGirlNS::AnnMagnetOverlapLowPt::neuron0DF2F7E0() 
{
   return m_input5;
}

double MuGirlNS::AnnMagnetOverlapLowPt::neuron0DF2F948() 
{
   return m_input6;
}

double MuGirlNS::AnnMagnetOverlapLowPt::neuron0DF2FAB0() 
{
   return m_input7;
}


double MuGirlNS::AnnMagnetOverlapLowPt::neuron0DF2FC18() 
{
   return m_input8;
}

double MuGirlNS::AnnMagnetOverlapLowPt::neuron0DD79870() {
   double input = 0.341988;
   input += synapse0DD99EA0();
   input += synapse0DE37B60();
   input += synapse0DFAB488();
   input += synapse0DC97FC8();
   input += synapse0DD799D8();
   input += synapse0DD79A08();
   input += synapse0DD79A38();
   input += synapse0DD79A68();
   input += synapse0DD79A98();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnMagnetOverlapLowPt::neuron0DD79AC8() 
{
   double input = -0.291262;
   input += synapse0DD79C50();
   input += synapse0DD79C80();
   input += synapse0DD79CB0();
   input += synapse0E015A20();
   input += synapse0E015A50();
   input += synapse0E015A80();
   input += synapse0E015AB0();
   input += synapse0E015AE0();
   input += synapse0E015B10();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnMagnetOverlapLowPt::neuron0E015B40() 
{
   double input = 0.392514;
   input += synapse0E015C60();
   input += synapse0E015C90();
   input += synapse0E015CC0();
   input += synapse0E015CF0();
   input += synapse0E015D20();
   input += synapse0E015D50();
   input += synapse0E015D80();
   input += synapse0E015DB0();
   input += synapse0E015DE0();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnMagnetOverlapLowPt::neuron0E015E10() 
{
   double input = 0.874124;
   input += synapse0E015F98();
   input += synapse0E015FC8();
   input += synapse0DF9F998();
   input += synapse0DF9F9C8();
   input += synapse0DF9F9F8();
   input += synapse0DF9FA28();
   input += synapse0DF9FA58();
   input += synapse0DF9FA88();
   input += synapse0DF9FAB8();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnMagnetOverlapLowPt::neuron0DF9FAE8() 
{
   double input = -4.57813;
   input += synapse0DF9FC70();
   input += synapse0DF9FCA0();
   input += synapse0DF9FCD0();
   input += synapse0DF9FD00();
   input += synapse0DF9FD30();
   input += synapse0DF9FD60();
   input += synapse0DF9FD90();
   input += synapse0DF9FDC0();
   input += synapse0DF9FDF0();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnMagnetOverlapLowPt::neuron0DF9FE20() 
{
   double input = 0.315346;
   input += synapse0DF9FFA8();
   input += synapse0DF9FFD8();
   input += synapse0DEF7768();
   input += synapse0DEF7798();
   input += synapse0DEF77C8();
   input += synapse0DEF77F8();
   input += synapse0DEF7828();
   input += synapse0DEF7858();
   input += synapse0DEF7888();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnMagnetOverlapLowPt::neuron0DEF78B8() 
{
   double input = -0.177243;
   input += synapse0DEF7A40();
   input += synapse0DEF7A70();
   input += synapse0DEF7AA0();
   input += synapse0DEF7AD0();
   input += synapse0DEF7B00();
   input += synapse0DEF7B30();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnMagnetOverlapLowPt::neuron0DEF7B60() 
{
   double input = 1.05476;
   input += synapse0DEF7CE8();
   return input;
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DD99EA0() 
{
   return (neuron0DD81EB8()*-0.432144);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DE37B60() 
{
   return (neuron0DDA9BA8()*-0.329838);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DFAB488() 
{
   return (neuron0E155BC8()*0.0386445);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DC97FC8() 
{
   return (neuron0E155D30()*0.0753477);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DD799D8() 
{
   return (neuron0E155E98()*0.289595);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DD79A08() 
{
   return (neuron0DF2F7E0()*0.282289);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DD79A38() 
{
   return (neuron0DF2F948()*-0.305938);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DD79A68() 
{
   return (neuron0DF2FAB0()*0.0506597);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DD79A98() 
{
   return (neuron0DF2FC18()*-0.666055);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DD79C50() 
{
   return (neuron0DD81EB8()*0.966646);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DD79C80() 
{
   return (neuron0DDA9BA8()*0.4831);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DD79CB0() 
{
   return (neuron0E155BC8()*-0.6071);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0E015A20() 
{
   return (neuron0E155D30()*1.94292);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0E015A50() 
{
   return (neuron0E155E98()*-1.32501);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0E015A80() 
{
   return (neuron0DF2F7E0()*-0.917323);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0E015AB0() 
{
   return (neuron0DF2F948()*-0.0678082);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0E015AE0() 
{
   return (neuron0DF2FAB0()*-0.196683);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0E015B10() 
{
   return (neuron0DF2FC18()*-0.0274368);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0E015C60() 
{
   return (neuron0DD81EB8()*0.551007);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0E015C90() 
{
   return (neuron0DDA9BA8()*0.339136);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0E015CC0() 
{
   return (neuron0E155BC8()*0.768281);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0E015CF0() 
{
   return (neuron0E155D30()*0.584605);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0E015D20() 
{
   return (neuron0E155E98()*-0.269418);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0E015D50() 
{
   return (neuron0DF2F7E0()*0.130768);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0E015D80()
{
   return (neuron0DF2F948()*0.0472244);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0E015DB0() 
{
   return (neuron0DF2FAB0()*0.203025);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0E015DE0() 
{
   return (neuron0DF2FC18()*0.258649);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0E015F98() 
{
   return (neuron0DD81EB8()*0.78668);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0E015FC8() 
{
   return (neuron0DDA9BA8()*-0.685358);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DF9F998() 
{
   return (neuron0E155BC8()*-0.32519);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DF9F9C8() 
{
   return (neuron0E155D30()*-1.65687);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DF9F9F8() 
{
   return (neuron0E155E98()*1.12708);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DF9FA28() 
{
   return (neuron0DF2F7E0()*0.183758);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DF9FA58() 
{
   return (neuron0DF2F948()*-2.50976);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DF9FA88() 
{
   return (neuron0DF2FAB0()*-0.851389);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DF9FAB8() 
{
   return (neuron0DF2FC18()*0.111615);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DF9FC70() 
{
   return (neuron0DD81EB8()*-0.0191435);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DF9FCA0() 
{
   return (neuron0DDA9BA8()*0.303104);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DF9FCD0() 
{
   return (neuron0E155BC8()*0.142028);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DF9FD00() 
{
   return (neuron0E155D30()*0.402163);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DF9FD30() 
{
   return (neuron0E155E98()*-1.98746);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DF9FD60() 
{
   return (neuron0DF2F7E0()*0.356545);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DF9FD90() 
{
   return (neuron0DF2F948()*0.733864);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DF9FDC0() 
{
   return (neuron0DF2FAB0()*-0.128114);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DF9FDF0() 
{
   return (neuron0DF2FC18()*0.0190146);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DF9FFA8() 
{
   return (neuron0DD81EB8()*-1.92939);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DF9FFD8() 
{
   return (neuron0DDA9BA8()*-0.755011);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DEF7768() 
{
   return (neuron0E155BC8()*-1.90659);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DEF7798() 
{
   return (neuron0E155D30()*-0.550615);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DEF77C8() 
{
   return (neuron0E155E98()*0.195827);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DEF77F8() 
{
   return (neuron0DF2F7E0()*0.643722);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DEF7828() 
{
   return (neuron0DF2F948()*0.655654);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DEF7858() 
{
   return (neuron0DF2FAB0()*-0.225639);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DEF7888() 
{
   return (neuron0DF2FC18()*-0.264483);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DEF7A40() 
{
   return (neuron0DD79870()*0.55211);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DEF7A70() 
{
   return (neuron0DD79AC8()*-1.57917);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DEF7AA0() 
{
   return (neuron0E015B40()*0.201015);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DEF7AD0() 
{
   return (neuron0E015E10()*1.8728);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DEF7B00() 
{
   return (neuron0DF9FAE8()*-1.78091);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DEF7B30() 
{
   return (neuron0DF9FE20()*1.40748);
}

double MuGirlNS::AnnMagnetOverlapLowPt::synapse0DEF7CE8() 
{
   return (neuron0DEF78B8()*-1.73434);
}

