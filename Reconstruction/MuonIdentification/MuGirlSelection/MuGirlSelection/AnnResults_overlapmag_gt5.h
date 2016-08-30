/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANNRESULTS_OVERLAPMAG_GT5_H
#define ANNRESULTS_OVERLAPMAG_GT5_H

namespace MuGirlNS
{

class AnnResults_overlapmag_gt5 { 
public:
   AnnResults_overlapmag_gt5() {}
   ~AnnResults_overlapmag_gt5() {}
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
   double neuron0xb3004b0();
   double neuron0xb3006a8();
   double neuron0xb30ec18();
   double neuron0xb30ee10();
   double neuron0xb30f008();
   double neuron0xb30f200();
   double neuron0xb30f3f8();
   double neuron0xb30f5f0();
   double neuron0xb30f7e8();
   double neuron0xb30f9e0();
   double input0xb30fce8();
   double neuron0xb30fce8();
   double input0xb30ffe0();
   double neuron0xb30ffe0();
   double input0xb3103d0();
   double neuron0xb3103d0();
   double input0xb3106f0();
   double neuron0xb3106f0();
   double input0xb310ac0();
   double neuron0xb310ac0();
   double input0xb310e28();
   double neuron0xb310e28();
   double input0xb3111b8();
   double neuron0xb3111b8();
   double input0xb310940();
   double neuron0xb310940();
   double synapse0xb2de630();
   double synapse0xb30fe78();
   double synapse0xb30fea0();
   double synapse0xb30fec8();
   double synapse0xb30fef0();
   double synapse0xb30ff18();
   double synapse0xb30ff40();
   double synapse0xb30ff68();
   double synapse0xb30ff90();
   double synapse0xb30ffb8();
   double synapse0xb3101b8();
   double synapse0xb3101e0();
   double synapse0xb310208();
   double synapse0xb310230();
   double synapse0xb310258();
   double synapse0xb310280();
   double synapse0xb3102a8();
   double synapse0xb310358();
   double synapse0xb310380();
   double synapse0xb3103a8();
   double synapse0xb310560();
   double synapse0xb310588();
   double synapse0xb3105b0();
   double synapse0xb3105d8();
   double synapse0xb310600();
   double synapse0xb310628();
   double synapse0xb310650();
   double synapse0xb310678();
   double synapse0xb3106a0();
   double synapse0xb3106c8();
   double synapse0xb3108c8();
   double synapse0xb3108f0();
   double synapse0xb310918();
   double synapse0xb246d30();
   double synapse0xb3102d0();
   double synapse0xb3102f8();
   double synapse0xb310320();
   double synapse0xb310a48();
   double synapse0xb310a70();
   double synapse0xb310a98();
   double synapse0xb310c98();
   double synapse0xb310cc0();
   double synapse0xb310ce8();
   double synapse0xb310d10();
   double synapse0xb310d38();
   double synapse0xb310d60();
   double synapse0xb310d88();
   double synapse0xb310db0();
   double synapse0xb310dd8();
   double synapse0xb310e00();
   double synapse0xb311000();
   double synapse0xb311028();
   double synapse0xb311050();
   double synapse0xb311078();
   double synapse0xb3110a0();
   double synapse0xb3110c8();
   double synapse0xb3110f0();
   double synapse0xb311118();
   double synapse0xb311140();
   double synapse0xb311168();
   double synapse0xb311190();
   double synapse0xb311418();
   double synapse0xb311440();
   double synapse0xb311468();
   double synapse0xb311490();
   double synapse0xb246ee8();
   double synapse0xb247238();
};
}
#endif // AnnResults_overlapmag_gt5_h

