/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnRegionOverlapLowPt.h"
#include "math.h"

double MuGirlNS::AnnRegionOverlapLowPt::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8) 
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
         return ((neuron0BFC6C98()*1)+0);
     default:
         return 0.;
   }
}

double MuGirlNS::AnnRegionOverlapLowPt::neuron0C064EC8() 
{
   return m_input0;
}

double MuGirlNS::AnnRegionOverlapLowPt::neuron0BFBDE18() 
{
   return m_input1;
}

double MuGirlNS::AnnRegionOverlapLowPt::neuron0BFBF970() 
{
   return m_input2;
}

double MuGirlNS::AnnRegionOverlapLowPt::neuron0BFBFA90() 
{
   return m_input3;
}

double MuGirlNS::AnnRegionOverlapLowPt::neuron0BFBFBF8() 
{
   return m_input4;
}

double MuGirlNS::AnnRegionOverlapLowPt::neuron0BFBFD60() 
{
   return m_input5;
}

double MuGirlNS::AnnRegionOverlapLowPt::neuron0BFBFEE8() 
{
   return m_input6;
}

double MuGirlNS::AnnRegionOverlapLowPt::neuron0BFF09B0() 
{
   return m_input7;
}

double MuGirlNS::AnnRegionOverlapLowPt::neuron0BFF0B18() 
{
   return m_input8;
}

double MuGirlNS::AnnRegionOverlapLowPt::neuron0BFF0DA0() 
{
   double input = 2.596;
   input += synapse0BFF0F28();
   input += synapse0BFF0F58();
   input += synapse0BFF0F88();
   input += synapse0BFF0FB8();
   input += synapse0BFF77E0();
   input += synapse0BFF7810();
   input += synapse0BFF7840();
   input += synapse0BFF7870();
   input += synapse0BFF78A0();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnRegionOverlapLowPt::neuron0BFF78D0() 
{
   double input = -0.933564;
   input += synapse0BFF7A38();
   input += synapse0BFF7A68();
   input += synapse0BFF7A98();
   input += synapse0BFF7AC8();
   input += synapse0BFF7AF8();
   input += synapse0BFF7B28();
   input += synapse0BFF7B58();
   input += synapse0BFF7B88();
   input += synapse0BFF7BB8();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnRegionOverlapLowPt::neuron0BFF7BE8() 
{
   double input = 0.245434;
   input += synapse0BFF7D28();
   input += synapse0BFF7D58();
   input += synapse0BFF7D88();
   input += synapse0BFF7DB8();
   input += synapse0BFF7DE8();
   input += synapse0BFF7E18();
   input += synapse0BFF7E48();
   input += synapse0BFC6008();
   input += synapse0BFC6038();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnRegionOverlapLowPt::neuron0BFC6068() 
{
   double input = -0.174573;
   input += synapse0BFC61D0();
   input += synapse0BFC6200();
   input += synapse0BFC6230();
   input += synapse0BFC6260();
   input += synapse0BFC6290();
   input += synapse0BFC62C0();
   input += synapse0BFC62F0();
   input += synapse0BFC6320();
   input += synapse0BFC6350();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnRegionOverlapLowPt::neuron0BFC6380() 
{
   double input = -0.403107;
   input += synapse0BFC6508();
   input += synapse0BFC6538();
   input += synapse0BFC6568();
   input += synapse0BFC6598();
   input += synapse0BFC65C8();
   input += synapse0BFC65F8();
   input += synapse0BFC6628();
   input += synapse0BFC6658();
   input += synapse0BFC6688();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnRegionOverlapLowPt::neuron0BFC66B8() 
{
   double input = 0.203573;
   input += synapse0BFC6840();
   input += synapse0BFC6870();
   input += synapse0BFC68A0();
   input += synapse0BFC68D0();
   input += synapse0BFC6900();
   input += synapse0BFC6930();
   input += synapse0BFC6960();
   input += synapse0BFC6990();
   input += synapse0BFC69C0();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnRegionOverlapLowPt::neuron0BFC69F0() 
{
   double input = 0.269349;
   input += synapse0BFC6B78();
   input += synapse0BFC6BA8();
   input += synapse0BFC6BD8();
   input += synapse0BFC6C08();
   input += synapse0BFC6C38();
   input += synapse0BFC6C68();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnRegionOverlapLowPt::neuron0BFC6C98() 
{
   double input = 0.015739;
   input += synapse0BFC6E20();
   return input;
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF0F28() 
{
   return (neuron0C064EC8()*0.158251);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF0F58() 
{
   return (neuron0BFBDE18()*-0.980022);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF0F88() 
{
   return (neuron0BFBF970()*0.0888378);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF0FB8() 
{
   return (neuron0BFBFA90()*-0.298961);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF77E0() 
{
   return (neuron0BFBFBF8()*-1.89208);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7810() 
{
   return (neuron0BFBFD60()*-0.413466);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7840() 
{
   return (neuron0BFBFEE8()*-0.269909);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7870() 
{
   return (neuron0BFF09B0()*4.95464);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF78A0() 
{
   return (neuron0BFF0B18()*-0.00852786);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7A38() 
{
   return (neuron0C064EC8()*2.31558);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7A68() 
{
   return (neuron0BFBDE18()*0.641499);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7A98() 
{
   return (neuron0BFBF970()*-0.202315);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7AC8() 
{
   return (neuron0BFBFA90()*0.702126);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7AF8() 
{
   return (neuron0BFBFBF8()*-0.0040018);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7B28() 
{
   return (neuron0BFBFD60()*-0.0085422);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7B58() 
{
   return (neuron0BFBFEE8()*-1.54181);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7B88() 
{
   return (neuron0BFF09B0()*-0.589289);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7BB8() 
{
   return (neuron0BFF0B18()*-0.295504);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7D28() 
{
   return (neuron0C064EC8()*-1.04665);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7D58() 
{
   return (neuron0BFBDE18()*-0.957233);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7D88() 
{
   return (neuron0BFBF970()*1.27932);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7DB8() 
{
   return (neuron0BFBFA90()*-1.18071);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7DE8() 
{
   return (neuron0BFBFBF8()*-0.31394);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7E18() 
{
   return (neuron0BFBFD60()*-0.477645);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFF7E48() 
{
   return (neuron0BFBFEE8()*-1.77192);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6008() 
{
   return (neuron0BFF09B0()*1.11668);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6038() 
{
   return (neuron0BFF0B18()*0.137778);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC61D0() 
{
   return (neuron0C064EC8()*-0.682118);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6200() 
{
   return (neuron0BFBDE18()*-2.74832);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6230() 
{
   return (neuron0BFBF970()*-0.660352);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6260() 
{
   return (neuron0BFBFA90()*0.305044);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6290() 
{
   return (neuron0BFBFBF8()*0.148867);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC62C0() 
{
   return (neuron0BFBFD60()*-0.546469);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC62F0()
{
   return (neuron0BFBFEE8()*0.332595);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6320()
{
   return (neuron0BFF09B0()*0.321192);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6350() 
{
   return (neuron0BFF0B18()*-0.201266);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6508()
{
   return (neuron0C064EC8()*-0.0765709);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6538() 
{
   return (neuron0BFBDE18()*0.449279);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6568() 
{
   return (neuron0BFBF970()*0.177748);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6598() 
{
   return (neuron0BFBFA90()*0.30329);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC65C8() 
{
   return (neuron0BFBFBF8()*-0.362502);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC65F8() 
{
   return (neuron0BFBFD60()*-0.361123);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6628() 
{
   return (neuron0BFBFEE8()*-0.240617);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6658() 
{
   return (neuron0BFF09B0()*0.286611);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6688() 
{
   return (neuron0BFF0B18()*1.12073);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6840() 
{
   return (neuron0C064EC8()*0.698443);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6870() 
{
   return (neuron0BFBDE18()*-0.291604);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC68A0() 
{
   return (neuron0BFBF970()*0.178713);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC68D0() 
{
   return (neuron0BFBFA90()*0.22294);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6900() 
{
   return (neuron0BFBFBF8()*-0.102036);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6930() 
{
   return (neuron0BFBFD60()*0.265785);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6960() 
{
   return (neuron0BFBFEE8()*0.291596);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6990() 
{
   return (neuron0BFF09B0()*-0.251716);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC69C0() 
{
   return (neuron0BFF0B18()*0.541563);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6B78() 
{
   return (neuron0BFF0DA0()*-5.27045);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6BA8() 
{
   return (neuron0BFF78D0()*0.653685);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6BD8() 
{
   return (neuron0BFF7BE8()*-2.1948);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6C08() 
{
   return (neuron0BFC6068()*-0.539018);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6C38() 
{
   return (neuron0BFC6380()*-0.23106);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6C68() 
{
   return (neuron0BFC66B8()*0.191838);
}

double MuGirlNS::AnnRegionOverlapLowPt::synapse0BFC6E20() 
{
   return (neuron0BFC69F0()*1.31648);
}

