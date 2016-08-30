/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResults_overlapmag_gt5.h"
#include <cmath>

double MuGirlNS::AnnResults_overlapmag_gt5::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) {
   m_input0 = (in0 - 0)/1;
   m_input1 = (in1 - 0)/1;
   m_input2 = (in2 - 0)/1;
   m_input3 = (in3 - 0)/1;
   m_input4 = (in4 - 0)/1;
   m_input5 = (in5 - 0)/1;
   m_input6 = (in6 - 0)/1;
   m_input7 = (in7 - 0)/1;
   m_input8 = (in8 - 0)/1;
   m_input9 = (in9 - 0)/1;
   switch(index) {
     case 0:
         return neuron0xb310940();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_overlapmag_gt5::value(int index, double* input) {
   m_input0 = (input[0] - 0)/1;
   m_input1 = (input[1] - 0)/1;
   m_input2 = (input[2] - 0)/1;
   m_input3 = (input[3] - 0)/1;
   m_input4 = (input[4] - 0)/1;
   m_input5 = (input[5] - 0)/1;
   m_input6 = (input[6] - 0)/1;
   m_input7 = (input[7] - 0)/1;
   m_input8 = (input[8] - 0)/1;
   m_input9 = (input[9] - 0)/1;
   switch(index) {
     case 0:
         return neuron0xb310940();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_overlapmag_gt5::neuron0xb3004b0() {
   return m_input0;
}

double MuGirlNS::AnnResults_overlapmag_gt5::neuron0xb3006a8() {
   return m_input1;
}

double MuGirlNS::AnnResults_overlapmag_gt5::neuron0xb30ec18() {
   return m_input2;
}

double MuGirlNS::AnnResults_overlapmag_gt5::neuron0xb30ee10() {
   return m_input3;
}

double MuGirlNS::AnnResults_overlapmag_gt5::neuron0xb30f008() {
   return m_input4;
}

double MuGirlNS::AnnResults_overlapmag_gt5::neuron0xb30f200() {
   return m_input5;
}

double MuGirlNS::AnnResults_overlapmag_gt5::neuron0xb30f3f8() {
   return m_input6;
}

double MuGirlNS::AnnResults_overlapmag_gt5::neuron0xb30f5f0() {
   return m_input7;
}

double MuGirlNS::AnnResults_overlapmag_gt5::neuron0xb30f7e8() {
   return m_input8;
}

double MuGirlNS::AnnResults_overlapmag_gt5::neuron0xb30f9e0() {
   return m_input9;
}

double MuGirlNS::AnnResults_overlapmag_gt5::input0xb30fce8() {
   double input = 0.488026;
   input += synapse0xb2de630();
   input += synapse0xb30fe78();
   input += synapse0xb30fea0();
   input += synapse0xb30fec8();
   input += synapse0xb30fef0();
   input += synapse0xb30ff18();
   input += synapse0xb30ff40();
   input += synapse0xb30ff68();
   input += synapse0xb30ff90();
   input += synapse0xb30ffb8();
   return input;
}

double MuGirlNS::AnnResults_overlapmag_gt5::neuron0xb30fce8() {
   double input = input0xb30fce8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_overlapmag_gt5::input0xb30ffe0() {
   double input = 0.486915;
   input += synapse0xb3101b8();
   input += synapse0xb3101e0();
   input += synapse0xb310208();
   input += synapse0xb310230();
   input += synapse0xb310258();
   input += synapse0xb310280();
   input += synapse0xb3102a8();
   input += synapse0xb310358();
   input += synapse0xb310380();
   input += synapse0xb3103a8();
   return input;
}

double MuGirlNS::AnnResults_overlapmag_gt5::neuron0xb30ffe0() {
   double input = input0xb30ffe0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_overlapmag_gt5::input0xb3103d0() {
   double input = -1.63672;
   input += synapse0xb310560();
   input += synapse0xb310588();
   input += synapse0xb3105b0();
   input += synapse0xb3105d8();
   input += synapse0xb310600();
   input += synapse0xb310628();
   input += synapse0xb310650();
   input += synapse0xb310678();
   input += synapse0xb3106a0();
   input += synapse0xb3106c8();
   return input;
}

double MuGirlNS::AnnResults_overlapmag_gt5::neuron0xb3103d0() {
   double input = input0xb3103d0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_overlapmag_gt5::input0xb3106f0() {
   double input = 3.74443;
   input += synapse0xb3108c8();
   input += synapse0xb3108f0();
   input += synapse0xb310918();
   input += synapse0xb246d30();
   input += synapse0xb3102d0();
   input += synapse0xb3102f8();
   input += synapse0xb310320();
   input += synapse0xb310a48();
   input += synapse0xb310a70();
   input += synapse0xb310a98();
   return input;
}

double MuGirlNS::AnnResults_overlapmag_gt5::neuron0xb3106f0() {
   double input = input0xb3106f0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_overlapmag_gt5::input0xb310ac0() {
   double input = 0.536528;
   input += synapse0xb310c98();
   input += synapse0xb310cc0();
   input += synapse0xb310ce8();
   input += synapse0xb310d10();
   input += synapse0xb310d38();
   input += synapse0xb310d60();
   input += synapse0xb310d88();
   input += synapse0xb310db0();
   input += synapse0xb310dd8();
   input += synapse0xb310e00();
   return input;
}

double MuGirlNS::AnnResults_overlapmag_gt5::neuron0xb310ac0() {
   double input = input0xb310ac0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_overlapmag_gt5::input0xb310e28() {
   double input = -0.459131;
   input += synapse0xb311000();
   input += synapse0xb311028();
   input += synapse0xb311050();
   input += synapse0xb311078();
   input += synapse0xb3110a0();
   input += synapse0xb3110c8();
   input += synapse0xb3110f0();
   input += synapse0xb311118();
   input += synapse0xb311140();
   input += synapse0xb311168();
   return input;
}

double MuGirlNS::AnnResults_overlapmag_gt5::neuron0xb310e28() {
   double input = input0xb310e28();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_overlapmag_gt5::input0xb3111b8() {
   double input = 13.9917;
   input += synapse0xb311190();
   input += synapse0xb311418();
   input += synapse0xb311440();
   input += synapse0xb311468();
   input += synapse0xb311490();
   input += synapse0xb246ee8();
   return input;
}

double MuGirlNS::AnnResults_overlapmag_gt5::neuron0xb3111b8() {
   double input = input0xb3111b8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_overlapmag_gt5::input0xb310940() {
   double input = 0.0678308;
   input += synapse0xb247238();
   return input;
}

double MuGirlNS::AnnResults_overlapmag_gt5::neuron0xb310940() {
   double input = input0xb310940();
   return (input * 1)+0;
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb2de630() {
   return (neuron0xb3004b0()*10.4232);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb30fe78() {
   return (neuron0xb3006a8()*8.17448);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb30fea0() {
   return (neuron0xb30ec18()*2.93222);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb30fec8() {
   return (neuron0xb30ee10()*1.57266);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb30fef0() {
   return (neuron0xb30f008()*0.334653);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb30ff18() {
   return (neuron0xb30f200()*7.29903);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb30ff40() {
   return (neuron0xb30f3f8()*0.132811);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb30ff68() {
   return (neuron0xb30f5f0()*0.170317);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb30ff90() {
   return (neuron0xb30f7e8()*5.01362);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb30ffb8() {
   return (neuron0xb30f9e0()*1.51203);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb3101b8() {
   return (neuron0xb3004b0()*1.42064);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb3101e0() {
   return (neuron0xb3006a8()*1.29533);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310208() {
   return (neuron0xb30ec18()*1.25769);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310230() {
   return (neuron0xb30ee10()*0.17768);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310258() {
   return (neuron0xb30f008()*0.483507);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310280() {
   return (neuron0xb30f200()*1.0055);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb3102a8() {
   return (neuron0xb30f3f8()*1.85989);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310358() {
   return (neuron0xb30f5f0()*0.451216);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310380() {
   return (neuron0xb30f7e8()*0.850346);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb3103a8() {
   return (neuron0xb30f9e0()*-0.0927018);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310560() {
   return (neuron0xb3004b0()*-0.0525912);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310588() {
   return (neuron0xb3006a8()*4.01562);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb3105b0() {
   return (neuron0xb30ec18()*-13.7885);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb3105d8() {
   return (neuron0xb30ee10()*1.5298);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310600() {
   return (neuron0xb30f008()*-2.9449);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310628() {
   return (neuron0xb30f200()*-0.389252);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310650() {
   return (neuron0xb30f3f8()*6.69769);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310678() {
   return (neuron0xb30f5f0()*-0.375188);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb3106a0() {
   return (neuron0xb30f7e8()*-10.7067);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb3106c8() {
   return (neuron0xb30f9e0()*-0.0800102);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb3108c8() {
   return (neuron0xb3004b0()*-0.000265183);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb3108f0() {
   return (neuron0xb3006a8()*0.00735747);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310918() {
   return (neuron0xb30ec18()*0.0123381);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb246d30() {
   return (neuron0xb30ee10()*-0.0319753);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb3102d0() {
   return (neuron0xb30f008()*-3.53675);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb3102f8() {
   return (neuron0xb30f200()*0.00353823);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310320() {
   return (neuron0xb30f3f8()*-0.0166225);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310a48() {
   return (neuron0xb30f5f0()*0.870311);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310a70() {
   return (neuron0xb30f7e8()*0.000534847);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310a98() {
   return (neuron0xb30f9e0()*-100.679);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310c98() {
   return (neuron0xb3004b0()*4.84115);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310cc0() {
   return (neuron0xb3006a8()*2.04355);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310ce8() {
   return (neuron0xb30ec18()*2.77952);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310d10() {
   return (neuron0xb30ee10()*0.945679);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310d38() {
   return (neuron0xb30f008()*0.0070184);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310d60() {
   return (neuron0xb30f200()*4.10632);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310d88() {
   return (neuron0xb30f3f8()*3.9827);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310db0() {
   return (neuron0xb30f5f0()*-0.267383);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310dd8() {
   return (neuron0xb30f7e8()*2.72852);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb310e00() {
   return (neuron0xb30f9e0()*0.0811624);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb311000() {
   return (neuron0xb3004b0()*-4.04807);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb311028() {
   return (neuron0xb3006a8()*-3.17908);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb311050() {
   return (neuron0xb30ec18()*-3.44717);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb311078() {
   return (neuron0xb30ee10()*-0.211557);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb3110a0() {
   return (neuron0xb30f008()*-0.385898);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb3110c8() {
   return (neuron0xb30f200()*-3.86616);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb3110f0() {
   return (neuron0xb30f3f8()*-5.1984);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb311118() {
   return (neuron0xb30f5f0()*-0.274462);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb311140() {
   return (neuron0xb30f7e8()*-3.99088);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb311168() {
   return (neuron0xb30f9e0()*-0.773527);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb311190() {
   return (neuron0xb30fce8()*12.0553);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb311418() {
   return (neuron0xb30ffe0()*15.395);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb311440() {
   return (neuron0xb3103d0()*-12.9911);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb311468() {
   return (neuron0xb3106f0()*-95.1272);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb311490() {
   return (neuron0xb310ac0()*11.1796);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb246ee8() {
   return (neuron0xb310e28()*-2.01121);
}

double MuGirlNS::AnnResults_overlapmag_gt5::synapse0xb247238() {
   return (neuron0xb3111b8()*0.928735);
}

