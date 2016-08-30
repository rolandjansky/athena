/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLANNRESULTS_TGCFIX_BARREL_BAD_H
#define MUGIRLANNRESULTS_TGCFIX_BARREL_BAD_H

class AtlasDetectorID;
class Identifier;


namespace MuGirlNS
{

class AnnResults_tgcfix_barrel_bad {
public:
   AnnResults_tgcfix_barrel_bad() : m_input0(-999.), m_input1(-999.), m_input2(-999.), m_input3(-999.), m_input4(-999.), m_input5(-999.), m_input6(-999.),
                                    m_input7(-999.), m_input8(-999.), m_input9(-999.) {}
   ~AnnResults_tgcfix_barrel_bad() {}
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
   double neuron0xaf0fb68();
   double neuron0xaf0fcf8();
   double neuron0xaf0fef0();
   double neuron0xaf100e8();
   double neuron0xaf102e0();
   double neuron0xaf104d8();
   double neuron0xaf106d0();
   double neuron0xaf108c8();
   double neuron0xaf10ac0();
   double neuron0xaf10cb8();
   double input0xaf10fb0();
   double neuron0xaf10fb0();
   double input0xaf11280();
   double neuron0xaf11280();
   double input0xaf11670();
   double neuron0xaf11670();
   double input0xaf11990();
   double neuron0xaf11990();
   double input0xaf11d10();
   double neuron0xaf11d10();
   double input0xaf12078();
   double neuron0xaf12078();
   double input0xaf12408();
   double neuron0xaf12408();
   double input0xaf11be0();
   double neuron0xaf11be0();
   double synapse0xaeede30();
   double synapse0xad85d98();
   double synapse0xaf11140();
   double synapse0xaf11168();
   double synapse0xaf11190();
   double synapse0xaf111b8();
   double synapse0xaf111e0();
   double synapse0xaf11208();
   double synapse0xaf11230();
   double synapse0xaf11258();
   double synapse0xaf11458();
   double synapse0xaf11480();
   double synapse0xaf114a8();
   double synapse0xaf114d0();
   double synapse0xaf114f8();
   double synapse0xaf11520();
   double synapse0xaf11548();
   double synapse0xaf115f8();
   double synapse0xaf11620();
   double synapse0xaf11648();
   double synapse0xaf11800();
   double synapse0xaf11828();
   double synapse0xaf11850();
   double synapse0xaf11878();
   double synapse0xaf118a0();
   double synapse0xaf118c8();
   double synapse0xaf118f0();
   double synapse0xaf11918();
   double synapse0xaf11940();
   double synapse0xaf11968();
   double synapse0xaf11b68();
   double synapse0xaf11b90();
   double synapse0xaf11bb8();
   double synapse0xad85b70();
   double synapse0xad85b98();
   double synapse0xad85bc0();
   double synapse0xaf11570();
   double synapse0xaf11598();
   double synapse0xaf115c0();
   double synapse0xaf11ce8();
   double synapse0xaf11ee8();
   double synapse0xaf11f10();
   double synapse0xaf11f38();
   double synapse0xaf11f60();
   double synapse0xaf11f88();
   double synapse0xaf11fb0();
   double synapse0xaf11fd8();
   double synapse0xaf12000();
   double synapse0xaf12028();
   double synapse0xaf12050();
   double synapse0xaf12250();
   double synapse0xaf12278();
   double synapse0xaf122a0();
   double synapse0xaf122c8();
   double synapse0xaf122f0();
   double synapse0xaf12318();
   double synapse0xaf12340();
   double synapse0xaf12368();
   double synapse0xaf12390();
   double synapse0xaf123b8();
   double synapse0xaf123e0();
   double synapse0xaf12668();
   double synapse0xaf12690();
   double synapse0xaf126b8();
   double synapse0xaf126e0();
   double synapse0xaef1c70();
   double synapse0xad860b8();
};
}
#endif // AnnResults_tgcfix_barrel_bad_h

