/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLNSANNTEST_ENDCAP_H
#define MUGIRLNSANNTEST_ENDCAP_H

class AtlasDetectorID;
class Identifier;

namespace MuGirlNS
{
class test_endcap {
public:
   test_endcap() : m_input0(-999.), m_input1(-999.), m_input2(-999.), m_input3(-999.), m_input4(-999.), m_input5(-999.), m_input6(-999.), m_input7(-999.) {}
   ~test_endcap() {}
   double value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7);
private:
   double m_input0;
   double m_input1;
   double m_input2;
   double m_input3;
   double m_input4;
   double m_input5;
   double m_input6;
   double m_input7;
   double neuron0A0A7EB8();
   double neuron09F384F0();
   double neuron09F38610();
   double neuron08027FB8();
   double neuron080280D8();
   double neuron08028240();
   double neuron080283A8();
   double neuron0A0DEAB0();
   double neuron0A0DEC18();
   double neuron0A0DED80();
   double neuron0A0DEEE8();
   double neuron0A1A8888();
   double neuron0A1A8B48();
   double neuron083098F8();
   double neuron08309C00();
   double neuron08309EA8();
   double synapse09F387C0();
   double synapse0A0A7FD8();
   double synapse08028960();
   double synapse08318130();
   double synapse09F2BD48();
   double synapse0A024E90();
   double synapse0A014E50();
   double synapse0834F9B0();
   double synapse0A00BDE0();
   double synapse09FF15C0();
   double synapse07ECAE28();
   double synapse08028638();
   double synapse0A052D50();
   double synapse08068FC8();
   double synapse09FF1BE0();
   double synapse0812A790();
   double synapse0A042E40();
   double synapse0A042EE8();
   double synapse0A03AE80();
   double synapse0A02CE40();
   double synapse0A02CEE8();
   double synapse0A014EC0();
   double synapse0A1A8828();
   double synapse0A1A8858();
   double synapse0A1A89C8();
   double synapse0A1A89F8();
   double synapse0A1A8A28();
   double synapse0A1A8A58();
   double synapse0A1A8A88();
   double synapse0A1A8AB8();
   double synapse0A1A8AE8();
   double synapse0A1A8B18();
   double synapse0A1A8CD0();
   double synapse0A1A8D00();
   double synapse0A1A8D30();
   double synapse0A1A8D60();
   double synapse0A1A8D90();
   double synapse0A1A8DC0();
   double synapse0A1A8DF0();
   double synapse083098C8();
   double synapse08309A80();
   double synapse08309AB0();
   double synapse08309AE0();
   double synapse08309B10();
   double synapse08309B40();
   double synapse08309B70();
   double synapse08309BA0();
   double synapse08309BD0();
   double synapse08309D88();
   double synapse08309DB8();
   double synapse08309DE8();
   double synapse08309E18();
   double synapse08309E48();
   double synapse08309E78();
   double synapse09F2A470();
};
}
#endif

