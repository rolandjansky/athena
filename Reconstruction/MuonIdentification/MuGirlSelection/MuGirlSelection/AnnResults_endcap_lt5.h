/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANNRESULTS_ENDCAP_LT5_H
#define ANNRESULTS_ENDCAP_LT5_H

namespace MuGirlNS
{

class AnnResults_endcap_lt5 {
public:
   AnnResults_endcap_lt5() : m_input0(-999.), m_input1(-999.), m_input2(-999.), m_input3(-999.), m_input4(-999.), m_input5(-999.), m_input6(-999.),
                             m_input7(-999.), m_input8(-999.), m_input9(-999.) {}
   ~AnnResults_endcap_lt5() {}
   double value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9);
   double value(int index, double* input);
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
   double m_input9;
   double neuron0x9f4d618();
   double neuron0x9f4d7a8();
   double neuron0x9f5d720();
   double neuron0x9f5d8d0();
   double neuron0x9f5dac8();
   double neuron0x9f5dcc0();
   double neuron0x9f5deb8();
   double neuron0x9f5e0b0();
   double neuron0x9f5e2a8();
   double neuron0x9f5e4a0();
   double input0x9f5e7a8();
   double neuron0x9f5e7a8();
   double input0x9f5ea78();
   double neuron0x9f5ea78();
   double input0x9f5ee68();
   double neuron0x9f5ee68();
   double input0x9f5f188();
   double neuron0x9f5f188();
   double input0x9f5f508();
   double neuron0x9f5f508();
   double input0x9f5f870();
   double neuron0x9f5f870();
   double input0x9f5fc00();
   double neuron0x9f5fc00();
   double input0x9f5f3d8();
   double neuron0x9f5f3d8();
   double synapse0x9f61678();
   double synapse0x9f3c100();
   double synapse0x9f5e938();
   double synapse0x9f5e960();
   double synapse0x9f5e988();
   double synapse0x9f5e9b0();
   double synapse0x9f5e9d8();
   double synapse0x9f5ea00();
   double synapse0x9f5ea28();
   double synapse0x9f5ea50();
   double synapse0x9f5ec50();
   double synapse0x9f5ec78();
   double synapse0x9f5eca0();
   double synapse0x9f5ecc8();
   double synapse0x9f5ecf0();
   double synapse0x9f5ed18();
   double synapse0x9f5ed40();
   double synapse0x9f5edf0();
   double synapse0x9f5ee18();
   double synapse0x9f5ee40();
   double synapse0x9f5eff8();
   double synapse0x9f5f020();
   double synapse0x9f5f048();
   double synapse0x9f5f070();
   double synapse0x9f5f098();
   double synapse0x9f5f0c0();
   double synapse0x9f5f0e8();
   double synapse0x9f5f110();
   double synapse0x9f5f138();
   double synapse0x9f5f160();
   double synapse0x9f5f360();
   double synapse0x9f5f388();
   double synapse0x9f5f3b0();
   double synapse0x9f3bec0();
   double synapse0x9f3bee8();
   double synapse0x9f3bf10();
   double synapse0x9f5ed68();
   double synapse0x9f5ed90();
   double synapse0x9f5edb8();
   double synapse0x9f5f4e0();
   double synapse0x9f5f6e0();
   double synapse0x9f5f708();
   double synapse0x9f5f730();
   double synapse0x9f5f758();
   double synapse0x9f5f780();
   double synapse0x9f5f7a8();
   double synapse0x9f5f7d0();
   double synapse0x9f5f7f8();
   double synapse0x9f5f820();
   double synapse0x9f5f848();
   double synapse0x9f5fa48();
   double synapse0x9f5fa70();
   double synapse0x9f5fa98();
   double synapse0x9f5fac0();
   double synapse0x9f5fae8();
   double synapse0x9f5fb10();
   double synapse0x9f5fb38();
   double synapse0x9f5fb60();
   double synapse0x9f5fb88();
   double synapse0x9f5fbb0();
   double synapse0x9f5fbd8();
   double synapse0x9f5fe60();
   double synapse0x9f5fe88();
   double synapse0x9f5feb0();
   double synapse0x9f5fed8();
   double synapse0x9f3c1e8();
   double synapse0x89ac398();
};
}
#endif // AnnResults_endcap_lt5_h

