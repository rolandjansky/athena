/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnBarrelBadLowPt.h"
#include "math.h"

double MuGirlNS::AnnBarrelBadLowPt::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8)
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
         return ((neuron0C100948()*1)+0);
     default:
         return 0.;
    }
}

double MuGirlNS::AnnBarrelBadLowPt::neuron0BDE52A8()
{
    return m_input0;
}

double MuGirlNS::AnnBarrelBadLowPt::neuron0C03A508()
{
    return m_input1;
}

double MuGirlNS::AnnBarrelBadLowPt::neuron0C03A670() 
{
    return m_input2;
}

double MuGirlNS::AnnBarrelBadLowPt::neuron0C03A7D8() 
{
    return m_input3;
}

double MuGirlNS::AnnBarrelBadLowPt::neuron0C05AA80()
{
    return m_input4;
}

double MuGirlNS::AnnBarrelBadLowPt::neuron0C05ABA0()
{
    return m_input5;
}

double MuGirlNS::AnnBarrelBadLowPt::neuron0C05AD08()
{
    return m_input6;
}

double MuGirlNS::AnnBarrelBadLowPt::neuron0C05AE90()
{
    return m_input7;
}

double MuGirlNS::AnnBarrelBadLowPt::neuron0C0734C0()
{
    return m_input8;
}

double MuGirlNS::AnnBarrelBadLowPt::neuron0C073748()
{
    double input = 0.587402;
    input += synapse0C0738D0();
    input += synapse0C073900();
    input += synapse0C073930();
    input += synapse0C073960();
    input += synapse0C073990();
    input += synapse0C0739C0();
    input += synapse0C0739F0();
    input += synapse0C073A20();
    input += synapse0C1797E0();
    if (input<-700) return 0;
    return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnBarrelBadLowPt::neuron0C179810()
{
    double input = 1.37726;
    input += synapse0C179998();
    input += synapse0C1799C8();
    input += synapse0C1799F8();
    input += synapse0C179A28();
    input += synapse0C179A58();
    input += synapse0C179A88();
    input += synapse0C179AB8();
    input += synapse0C179AE8();
    input += synapse0C179B18();
    if (input<-700) return 0;
    return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnBarrelBadLowPt::neuron0C179B48()
{
    double input = -0.497501;
    input += synapse0C179C88();
    input += synapse0C179CB8();
    input += synapse0C179CE8();
    input += synapse0C179D18();
    input += synapse0C179D48();
    input += synapse0C1077E0();
    input += synapse0C107810();
    input += synapse0C107840();
    input += synapse0C107870();
    if (input<-700) return 0;
    return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnBarrelBadLowPt::neuron0C1078A0() 
{
    double input = 0.0609029;
    input += synapse0C107A08();
    input += synapse0C107A38();
    input += synapse0C107A68();
    input += synapse0C107A98();
    input += synapse0C107AC8();
    input += synapse0C107AF8();
    input += synapse0C107B28();
    input += synapse0C107B58();
    input += synapse0C107B88();
    if (input<-700) return 0;
    return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnBarrelBadLowPt::neuron0C107BB8() 
{
    double input = -0.690956;
    input += synapse0C107D40();
    input += synapse0C107D70();
    input += synapse0C107DA0();
    input += synapse0C08B9A8();
    input += synapse0C08B9D8();
    input += synapse0C08BA08();
    input += synapse0C08BA38();
    input += synapse0C08BA68();
    input += synapse0C08BA98();
    if (input<-700) return 0;
    return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnBarrelBadLowPt::neuron0C08BAC8() 
{
    double input = -0.3835;
    input += synapse0C08BC30();
    input += synapse0C08BC60();
    input += synapse0C08BC90();
    input += synapse0C08BCC0();
    input += synapse0C08BCF0();
    input += synapse0C08BD20();
    input += synapse0C08BD50();
    input += synapse0C08BD80();
    input += synapse0C08BDB0();
    if (input<-700) return 0;
    return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnBarrelBadLowPt::neuron0C08BDE0()
{
    double input = 0.0376321;
    input += synapse0C08BF68();
    input += synapse0C08BF98();
    input += synapse0C08BFC8();
    input += synapse0C1008B8();
    input += synapse0C1008E8();
    input += synapse0C100918();
    if (input<-700) return 0;
    return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnBarrelBadLowPt::neuron0C100948()
{
    double input = -0.345347;
    input += synapse0C100AD0();
    return input;
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C0738D0() 
{
    return (neuron0BDE52A8()*-0.0632052);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C073900() 
{
   return (neuron0C03A508()*-0.526127);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C073930() 
{
   return (neuron0C03A670()*0.449858);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C073960() 
{
   return (neuron0C03A7D8()*-0.102819);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C073990() 
{
   return (neuron0C05AA80()*-0.228163);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C0739C0()
{
   return (neuron0C05ABA0()*-0.188192);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C0739F0() 
{
   return (neuron0C05AD08()*-1.54023);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C073A20() 
{
   return (neuron0C05AE90()*0.274362);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C1797E0() 
{
   return (neuron0C0734C0()*-0.0324846);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C179998() 
{
   return (neuron0BDE52A8()*-0.0242623);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C1799C8() 
{
   return (neuron0C03A508()*-0.998307);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C1799F8() 
{
   return (neuron0C03A670()*-0.237694);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C179A28()
{
   return (neuron0C03A7D8()*-1.13688);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C179A58() 
{
   return (neuron0C05AA80()*0.782296);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C179A88() 
{
   return (neuron0C05ABA0()*0.0228912);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C179AB8() 
{
   return (neuron0C05AD08()*-0.0421859);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C179AE8() 
{
   return (neuron0C05AE90()*1.82754);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C179B18() 
{
   return (neuron0C0734C0()*0.0174023);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C179C88() 
{
   return (neuron0BDE52A8()*-0.318869);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C179CB8() 
{
   return (neuron0C03A508()*-0.320695);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C179CE8() 
{
   return (neuron0C03A670()*0.0269851);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C179D18() 
{
   return (neuron0C03A7D8()*0.406595);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C179D48() 
{
   return (neuron0C05AA80()*-0.229686);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C1077E0() 
{
   return (neuron0C05ABA0()*0.644444);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C107810() 
{
   return (neuron0C05AD08()*0.841932);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C107840() 
{
   return (neuron0C05AE90()*-0.503833);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C107870() 
{
   return (neuron0C0734C0()*-0.0539549);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C107A08() 
{
   return (neuron0BDE52A8()*0.161203);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C107A38() 
{
   return (neuron0C03A508()*-0.346847);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C107A68() 
{
   return (neuron0C03A670()*0.574448);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C107A98() 
{
   return (neuron0C03A7D8()*-0.163012);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C107AC8() 
{
   return (neuron0C05AA80()*-0.163523);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C107AF8() 
{
   return (neuron0C05ABA0()*-0.400232);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C107B28() 
{
   return (neuron0C05AD08()*0.384238);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C107B58() 
{
   return (neuron0C05AE90()*0.137967);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C107B88() 
{
   return (neuron0C0734C0()*0.514861);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C107D40() 
{
   return (neuron0BDE52A8()*-0.159306);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C107D70() 
{
   return (neuron0C03A508()*1.11703);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C107DA0() 
{
   return (neuron0C03A670()*0.262329);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C08B9A8() 
{
   return (neuron0C03A7D8()*0.564692);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C08B9D8()
{
   return (neuron0C05AA80()*0.517576);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C08BA08() 
{
   return (neuron0C05ABA0()*-0.190423);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C08BA38() 
{
   return (neuron0C05AD08()*1.29835);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C08BA68() 
{
   return (neuron0C05AE90()*-0.342531);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C08BA98() 
{
   return (neuron0C0734C0()*0.191965);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C08BC30() 
{
   return (neuron0BDE52A8()*0.375419);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C08BC60() 
{
   return (neuron0C03A508()*-0.0155126);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C08BC90() 
{
   return (neuron0C03A670()*-0.167518);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C08BCC0() 
{
   return (neuron0C03A7D8()*-0.115438);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C08BCF0() 
{
   return (neuron0C05AA80()*0.0160799);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C08BD20() 
{
   return (neuron0C05ABA0()*0.393937);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C08BD50() 
{
   return (neuron0C05AD08()*-0.0506472);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C08BD80() 
{
   return (neuron0C05AE90()*-0.0879896);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C08BDB0() 
{
   return (neuron0C0734C0()*1.1226);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C08BF68() 
{
   return (neuron0C073748()*-2.3524);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C08BF98() 
{
   return (neuron0C179810()*-1.83519);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C08BFC8() 
{
   return (neuron0C179B48()*1.48413);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C1008B8() 
{
   return (neuron0C1078A0()*-0.332807);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C1008E8() 
{
   return (neuron0C107BB8()*1.57882);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C100918() 
{
   return (neuron0C08BAC8()*-0.440207);
}

double MuGirlNS::AnnBarrelBadLowPt::synapse0C100AD0()
{
   return (neuron0C08BDE0()*1.42698);
}

