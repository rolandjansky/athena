/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLANNRESULTS_TGCFIX_BARREL_GOOD_H
#define MUGIRLANNRESULTS_TGCFIX_BARREL_GOOD_H

class AtlasDetectorID;
class Identifier;

namespace MuGirlNS
{


class AnnResults_tgcfix_barrel_good {
public:
   AnnResults_tgcfix_barrel_good()  : m_input0(-999.), m_input1(-999.), m_input2(-999.), m_input3(-999.), m_input4(-999.), m_input5(-999.), m_input6(-999.),
                                      m_input7(-999.), m_input8(-999.), m_input9(-999.) {}
   ~AnnResults_tgcfix_barrel_good() {}
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
   double neuron0xad24860();
   double neuron0xad24a10();
   double neuron0xad32f78();
   double neuron0xad33150();
   double neuron0xad33348();
   double neuron0xad33540();
   double neuron0xad33738();
   double neuron0xad33930();
   double neuron0xad33b28();
   double neuron0xad33d20();
   double input0xad34018();
   double neuron0xad34018();
   double input0xad34310();
   double neuron0xad34310();
   double input0xad34700();
   double neuron0xad34700();
   double input0xad34a20();
   double neuron0xad34a20();
   double input0xad34e18();
   double neuron0xad34e18();
   double input0xad35180();
   double neuron0xad35180();
   double input0xad35510();
   double neuron0xad35510();
   double input0xad34c70();
   double neuron0xad34c70();
   double synapse0xad02af0();
   double synapse0xad341a8();
   double synapse0xad341d0();
   double synapse0xad341f8();
   double synapse0xad34220();
   double synapse0xad34248();
   double synapse0xad34270();
   double synapse0xad34298();
   double synapse0xad342c0();
   double synapse0xad342e8();
   double synapse0xad344e8();
   double synapse0xad34510();
   double synapse0xad34538();
   double synapse0xad34560();
   double synapse0xad34588();
   double synapse0xad345b0();
   double synapse0xad345d8();
   double synapse0xad34688();
   double synapse0xad346b0();
   double synapse0xad346d8();
   double synapse0xad34890();
   double synapse0xad348b8();
   double synapse0xad348e0();
   double synapse0xad34908();
   double synapse0xad34930();
   double synapse0xad34958();
   double synapse0xad34980();
   double synapse0xad349a8();
   double synapse0xad349d0();
   double synapse0xad349f8();
   double synapse0xad34bf8();
   double synapse0xad34c20();
   double synapse0xad34c48();
   double synapse0xad34600();
   double synapse0xad34628();
   double synapse0xad34650();
   double synapse0xad34d78();
   double synapse0xad34da0();
   double synapse0xad34dc8();
   double synapse0xad34df0();
   double synapse0xad34ff0();
   double synapse0xad35018();
   double synapse0xad35040();
   double synapse0xad35068();
   double synapse0xad35090();
   double synapse0xad350b8();
   double synapse0xad350e0();
   double synapse0xad35108();
   double synapse0xad35130();
   double synapse0xad35158();
   double synapse0xad35358();
   double synapse0xad35380();
   double synapse0xad353a8();
   double synapse0xad353d0();
   double synapse0xad353f8();
   double synapse0xad35420();
   double synapse0xad35448();
   double synapse0xad35470();
   double synapse0xad35498();
   double synapse0xad354c0();
   double synapse0xad354e8();
   double synapse0xad35770();
   double synapse0xad35798();
   double synapse0xad357c0();
   double synapse0xad357e8();
   double synapse0xad24720();
   double synapse0xabd1838();
};
}
#endif // AnnResults_tgcfix_barrel_good_h

