/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLNSANNBARRELBADLOWPT_H
#define MUGIRLNSANNBARRELBADLOWPT_H

class AtlasDetectorID;
class Identifier;

namespace MuGirlNS
{
class AnnBarrelBadLowPt {
public:
   AnnBarrelBadLowPt() {}
   virtual ~AnnBarrelBadLowPt() {}
   virtual double value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8);
private:
   double m_input0;
   double m_input1;
   double m_input2;
   double m_input3;
   double m_input4;
   double m_input5;
   double m_input6;
   double m_input7;
   double m_input8;
   double neuron0BDE52A8();
   double neuron0C03A508();
   double neuron0C03A670();
   double neuron0C03A7D8();
   double neuron0C05AA80();
   double neuron0C05ABA0();
   double neuron0C05AD08();
   double neuron0C05AE90();
   double neuron0C0734C0();
   double neuron0C073748();
   double neuron0C179810();
   double neuron0C179B48();
   double neuron0C1078A0();
   double neuron0C107BB8();
   double neuron0C08BAC8();
   double neuron0C08BDE0();
   double neuron0C100948();
   double synapse0C0738D0();
   double synapse0C073900();
   double synapse0C073930();
   double synapse0C073960();
   double synapse0C073990();
   double synapse0C0739C0();
   double synapse0C0739F0();
   double synapse0C073A20();
   double synapse0C1797E0();
   double synapse0C179998();
   double synapse0C1799C8();
   double synapse0C1799F8();
   double synapse0C179A28();
   double synapse0C179A58();
   double synapse0C179A88();
   double synapse0C179AB8();
   double synapse0C179AE8();
   double synapse0C179B18();
   double synapse0C179C88();
   double synapse0C179CB8();
   double synapse0C179CE8();
   double synapse0C179D18();
   double synapse0C179D48();
   double synapse0C1077E0();
   double synapse0C107810();
   double synapse0C107840();
   double synapse0C107870();
   double synapse0C107A08();
   double synapse0C107A38();
   double synapse0C107A68();
   double synapse0C107A98();
   double synapse0C107AC8();
   double synapse0C107AF8();
   double synapse0C107B28();
   double synapse0C107B58();
   double synapse0C107B88();
   double synapse0C107D40();
   double synapse0C107D70();
   double synapse0C107DA0();
   double synapse0C08B9A8();
   double synapse0C08B9D8();
   double synapse0C08BA08();
   double synapse0C08BA38();
   double synapse0C08BA68();
   double synapse0C08BA98();
   double synapse0C08BC30();
   double synapse0C08BC60();
   double synapse0C08BC90();
   double synapse0C08BCC0();
   double synapse0C08BCF0();
   double synapse0C08BD20();
   double synapse0C08BD50();
   double synapse0C08BD80();
   double synapse0C08BDB0();
   double synapse0C08BF68();
   double synapse0C08BF98();
   double synapse0C08BFC8();
   double synapse0C1008B8();
   double synapse0C1008E8();
   double synapse0C100918();
   double synapse0C100AD0();
};
}
#endif

