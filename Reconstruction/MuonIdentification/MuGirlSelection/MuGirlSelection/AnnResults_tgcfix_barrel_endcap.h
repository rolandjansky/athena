/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLANNRESULTS_TGCFIX_BARREL_ENDCAP_H
#define MUGIRLANNRESULTS_TGCFIX_BARREL_ENDCAP_H

class AtlasDetectorID;
class Identifier;

namespace MuGirlNS
{

class AnnResults_tgcfix_barrel_endcap {
public:
   AnnResults_tgcfix_barrel_endcap() : m_input0(-999.), m_input1(-999.), m_input2(-999.), m_input3(-999.), m_input4(-999.), m_input5(-999.), m_input6(-999.),
                                       m_input7(-999.), m_input8(-999.), m_input9(-999.) {}
   ~AnnResults_tgcfix_barrel_endcap() {}
   double value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9);
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
   double neuron0x92d7e90();
   double neuron0x92d8068();
   double neuron0x92d8260();
   double neuron0x92e6680();
   double neuron0x92e6858();
   double neuron0x92e6a30();
   double neuron0x92e6c08();
   double neuron0x92e6de0();
   double neuron0x92e6fd8();
   double neuron0x92e71d0();
   double input0x92e74c8();
   double neuron0x92e74c8();
   double input0x92e77c0();
   double neuron0x92e77c0();
   double input0x92e7bb0();
   double neuron0x92e7bb0();
   double input0x92e7ed0();
   double neuron0x92e7ed0();
   double input0x92e82c8();
   double neuron0x92e82c8();
   double input0x92e8630();
   double neuron0x92e8630();
   double input0x92e89c0();
   double neuron0x92e89c0();
   double input0x92e8120();
   double neuron0x92e8120();
   double synapse0x8fe4708();
   double synapse0x92e7658();
   double synapse0x92e7680();
   double synapse0x92e76a8();
   double synapse0x92e76d0();
   double synapse0x92e76f8();
   double synapse0x92e7720();
   double synapse0x92e7748();
   double synapse0x92e7770();
   double synapse0x92e7798();
   double synapse0x92e7998();
   double synapse0x92e79c0();
   double synapse0x92e79e8();
   double synapse0x92e7a10();
   double synapse0x92e7a38();
   double synapse0x92e7a60();
   double synapse0x92e7a88();
   double synapse0x92e7b38();
   double synapse0x92e7b60();
   double synapse0x92e7b88();
   double synapse0x92e7d40();
   double synapse0x92e7d68();
   double synapse0x92e7d90();
   double synapse0x92e7db8();
   double synapse0x92e7de0();
   double synapse0x92e7e08();
   double synapse0x92e7e30();
   double synapse0x92e7e58();
   double synapse0x92e7e80();
   double synapse0x92e7ea8();
   double synapse0x92e80a8();
   double synapse0x92e80d0();
   double synapse0x92e80f8();
   double synapse0x92e7ab0();
   double synapse0x92e7ad8();
   double synapse0x92e7b00();
   double synapse0x92e8228();
   double synapse0x92e8250();
   double synapse0x92e8278();
   double synapse0x92e82a0();
   double synapse0x92e84a0();
   double synapse0x92e84c8();
   double synapse0x92e84f0();
   double synapse0x92e8518();
   double synapse0x92e8540();
   double synapse0x92e8568();
   double synapse0x92e8590();
   double synapse0x92e85b8();
   double synapse0x92e85e0();
   double synapse0x92e8608();
   double synapse0x92e8808();
   double synapse0x92e8830();
   double synapse0x92e8858();
   double synapse0x92e8880();
   double synapse0x92e88a8();
   double synapse0x92e88d0();
   double synapse0x92e88f8();
   double synapse0x92e8920();
   double synapse0x92e8948();
   double synapse0x92e8970();
   double synapse0x92e8998();
   double synapse0x92e8c20();
   double synapse0x92e8c48();
   double synapse0x92e8c70();
   double synapse0x92e8c98();
   double synapse0x8fe46d8();
   double synapse0x92e73f0();
};
}
#endif // AnnResults_tgcfix_barrel_endcap_h

