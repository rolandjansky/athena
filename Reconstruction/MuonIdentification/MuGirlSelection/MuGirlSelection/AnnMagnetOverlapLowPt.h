/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLNSANNMAGNETOVERLAPLOWPT_H
#define MUGIRLNSANNMAGNETOVERLAPLOWPT_H

class AtlasDetectorID;
class Identifier;

namespace MuGirlNS
{

class AnnMagnetOverlapLowPt {
public:
   AnnMagnetOverlapLowPt() {}
   ~AnnMagnetOverlapLowPt() {}
   double value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8);
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
   double neuron0DD81EB8();
   double neuron0DDA9BA8();
   double neuron0E155BC8();
   double neuron0E155D30();
   double neuron0E155E98();
   double neuron0DF2F7E0();
   double neuron0DF2F948();
   double neuron0DF2FAB0();
   double neuron0DF2FC18();
   double neuron0DD79870();
   double neuron0DD79AC8();
   double neuron0E015B40();
   double neuron0E015E10();
   double neuron0DF9FAE8();
   double neuron0DF9FE20();
   double neuron0DEF78B8();
   double neuron0DEF7B60();
   double synapse0DD99EA0();
   double synapse0DE37B60();
   double synapse0DFAB488();
   double synapse0DC97FC8();
   double synapse0DD799D8();
   double synapse0DD79A08();
   double synapse0DD79A38();
   double synapse0DD79A68();
   double synapse0DD79A98();
   double synapse0DD79C50();
   double synapse0DD79C80();
   double synapse0DD79CB0();
   double synapse0E015A20();
   double synapse0E015A50();
   double synapse0E015A80();
   double synapse0E015AB0();
   double synapse0E015AE0();
   double synapse0E015B10();
   double synapse0E015C60();
   double synapse0E015C90();
   double synapse0E015CC0();
   double synapse0E015CF0();
   double synapse0E015D20();
   double synapse0E015D50();
   double synapse0E015D80();
   double synapse0E015DB0();
   double synapse0E015DE0();
   double synapse0E015F98();
   double synapse0E015FC8();
   double synapse0DF9F998();
   double synapse0DF9F9C8();
   double synapse0DF9F9F8();
   double synapse0DF9FA28();
   double synapse0DF9FA58();
   double synapse0DF9FA88();
   double synapse0DF9FAB8();
   double synapse0DF9FC70();
   double synapse0DF9FCA0();
   double synapse0DF9FCD0();
   double synapse0DF9FD00();
   double synapse0DF9FD30();
   double synapse0DF9FD60();
   double synapse0DF9FD90();
   double synapse0DF9FDC0();
   double synapse0DF9FDF0();
   double synapse0DF9FFA8();
   double synapse0DF9FFD8();
   double synapse0DEF7768();
   double synapse0DEF7798();
   double synapse0DEF77C8();
   double synapse0DEF77F8();
   double synapse0DEF7828();
   double synapse0DEF7858();
   double synapse0DEF7888();
   double synapse0DEF7A40();
   double synapse0DEF7A70();
   double synapse0DEF7AA0();
   double synapse0DEF7AD0();
   double synapse0DEF7B00();
   double synapse0DEF7B30();
   double synapse0DEF7CE8();
};
}
#endif

