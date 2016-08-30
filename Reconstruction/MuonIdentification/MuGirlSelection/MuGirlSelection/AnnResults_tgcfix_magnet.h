/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLANNRESULTS_TGCFIX_MAGNET_H
#define MUGIRLANNRESULTS_TGCFIX_MAGNET_H

class AtlasDetectorID;
class Identifier;

namespace MuGirlNS
{

class AnnResults_tgcfix_magnet {
public:
   AnnResults_tgcfix_magnet() : m_input0(-999.), m_input1(-999.), m_input2(-999.), m_input3(-999.), m_input4(-999.), m_input5(-999.), m_input6(-999.),
                                m_input7(-999.), m_input8(-999.), m_input9(-999.) {}
   ~AnnResults_tgcfix_magnet() {}
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
   double neuron0xa7c0ec8();
   double neuron0xa7c1078();
   double neuron0xa7c1270();
   double neuron0xa7c1468();
   double neuron0xa7c1660();
   double neuron0xa7c1858();
   double neuron0xa7c1a50();
   double neuron0xa7c1c48();
   double neuron0xa7c1e40();
   double neuron0xa7c2038();
   double input0xa7c2330();
   double neuron0xa7c2330();
   double input0xa7c2628();
   double neuron0xa7c2628();
   double input0xa7c2a18();
   double neuron0xa7c2a18();
   double input0xa7c2d38();
   double neuron0xa7c2d38();
   double input0xa7c30b8();
   double neuron0xa7c30b8();
   double input0xa7c3420();
   double neuron0xa7c3420();
   double input0xa7c37b0();
   double neuron0xa7c37b0();
   double input0xa7c2f88();
   double neuron0xa7c2f88();
   double synapse0xa79f1a8();
   double synapse0xa7c24c0();
   double synapse0xa7c24e8();
   double synapse0xa7c2510();
   double synapse0xa7c2538();
   double synapse0xa7c2560();
   double synapse0xa7c2588();
   double synapse0xa7c25b0();
   double synapse0xa7c25d8();
   double synapse0xa7c2600();
   double synapse0xa7c2800();
   double synapse0xa7c2828();
   double synapse0xa7c2850();
   double synapse0xa7c2878();
   double synapse0xa7c28a0();
   double synapse0xa7c28c8();
   double synapse0xa7c28f0();
   double synapse0xa7c29a0();
   double synapse0xa7c29c8();
   double synapse0xa7c29f0();
   double synapse0xa7c2ba8();
   double synapse0xa7c2bd0();
   double synapse0xa7c2bf8();
   double synapse0xa7c2c20();
   double synapse0xa7c2c48();
   double synapse0xa7c2c70();
   double synapse0xa7c2c98();
   double synapse0xa7c2cc0();
   double synapse0xa7c2ce8();
   double synapse0xa7c2d10();
   double synapse0xa7c2f10();
   double synapse0xa7c2f38();
   double synapse0xa7c2f60();
   double synapse0xa454dd8();
   double synapse0xa454e00();
   double synapse0xa454e28();
   double synapse0xa7c2918();
   double synapse0xa7c2940();
   double synapse0xa7c2968();
   double synapse0xa7c3090();
   double synapse0xa7c3290();
   double synapse0xa7c32b8();
   double synapse0xa7c32e0();
   double synapse0xa7c3308();
   double synapse0xa7c3330();
   double synapse0xa7c3358();
   double synapse0xa7c3380();
   double synapse0xa7c33a8();
   double synapse0xa7c33d0();
   double synapse0xa7c33f8();
   double synapse0xa7c35f8();
   double synapse0xa7c3620();
   double synapse0xa7c3648();
   double synapse0xa7c3670();
   double synapse0xa7c3698();
   double synapse0xa7c36c0();
   double synapse0xa7c36e8();
   double synapse0xa7c3710();
   double synapse0xa7c3738();
   double synapse0xa7c3760();
   double synapse0xa7c3788();
   double synapse0xa7c3a10();
   double synapse0xa7c3a38();
   double synapse0xa7c3a60();
   double synapse0xa7c3a88();
   double synapse0xa79f1d0();
   double synapse0xa455318();
};
}
#endif // AnnResults_tgcfix_magnet_h

