/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResults_tgcfix_magnet.h"
#include <cmath>

double MuGirlNS::AnnResults_tgcfix_magnet::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) {
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
         return neuron0xa7c2f88();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_tgcfix_magnet::neuron0xa7c0ec8() {
   return m_input0;
}

double MuGirlNS::AnnResults_tgcfix_magnet::neuron0xa7c1078() {
   return m_input1;
}

double MuGirlNS::AnnResults_tgcfix_magnet::neuron0xa7c1270() {
   return m_input2;
}

double MuGirlNS::AnnResults_tgcfix_magnet::neuron0xa7c1468() {
   return m_input3;
}

double MuGirlNS::AnnResults_tgcfix_magnet::neuron0xa7c1660() {
   return m_input4;
}

double MuGirlNS::AnnResults_tgcfix_magnet::neuron0xa7c1858() {
   return m_input5;
}

double MuGirlNS::AnnResults_tgcfix_magnet::neuron0xa7c1a50() {
   return m_input6;
}

double MuGirlNS::AnnResults_tgcfix_magnet::neuron0xa7c1c48() {
   return m_input7;
}

double MuGirlNS::AnnResults_tgcfix_magnet::neuron0xa7c1e40() {
   return m_input8;
}

double MuGirlNS::AnnResults_tgcfix_magnet::neuron0xa7c2038() {
   return m_input9;
}

double MuGirlNS::AnnResults_tgcfix_magnet::input0xa7c2330() {
   double input = 0.947486;
   input += synapse0xa79f1a8();
   input += synapse0xa7c24c0();
   input += synapse0xa7c24e8();
   input += synapse0xa7c2510();
   input += synapse0xa7c2538();
   input += synapse0xa7c2560();
   input += synapse0xa7c2588();
   input += synapse0xa7c25b0();
   input += synapse0xa7c25d8();
   input += synapse0xa7c2600();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_magnet::neuron0xa7c2330() {
   double input = input0xa7c2330();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_magnet::input0xa7c2628() {
   double input = -1.07537;
   input += synapse0xa7c2800();
   input += synapse0xa7c2828();
   input += synapse0xa7c2850();
   input += synapse0xa7c2878();
   input += synapse0xa7c28a0();
   input += synapse0xa7c28c8();
   input += synapse0xa7c28f0();
   input += synapse0xa7c29a0();
   input += synapse0xa7c29c8();
   input += synapse0xa7c29f0();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_magnet::neuron0xa7c2628() {
   double input = input0xa7c2628();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_magnet::input0xa7c2a18() {
   double input = 0.105849;
   input += synapse0xa7c2ba8();
   input += synapse0xa7c2bd0();
   input += synapse0xa7c2bf8();
   input += synapse0xa7c2c20();
   input += synapse0xa7c2c48();
   input += synapse0xa7c2c70();
   input += synapse0xa7c2c98();
   input += synapse0xa7c2cc0();
   input += synapse0xa7c2ce8();
   input += synapse0xa7c2d10();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_magnet::neuron0xa7c2a18() {
   double input = input0xa7c2a18();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_magnet::input0xa7c2d38() {
   double input = -0.16048;
   input += synapse0xa7c2f10();
   input += synapse0xa7c2f38();
   input += synapse0xa7c2f60();
   input += synapse0xa454dd8();
   input += synapse0xa454e00();
   input += synapse0xa454e28();
   input += synapse0xa7c2918();
   input += synapse0xa7c2940();
   input += synapse0xa7c2968();
   input += synapse0xa7c3090();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_magnet::neuron0xa7c2d38() {
   double input = input0xa7c2d38();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_magnet::input0xa7c30b8() {
   double input = -0.0260841;
   input += synapse0xa7c3290();
   input += synapse0xa7c32b8();
   input += synapse0xa7c32e0();
   input += synapse0xa7c3308();
   input += synapse0xa7c3330();
   input += synapse0xa7c3358();
   input += synapse0xa7c3380();
   input += synapse0xa7c33a8();
   input += synapse0xa7c33d0();
   input += synapse0xa7c33f8();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_magnet::neuron0xa7c30b8() {
   double input = input0xa7c30b8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_magnet::input0xa7c3420() {
   double input = 1.05003;
   input += synapse0xa7c35f8();
   input += synapse0xa7c3620();
   input += synapse0xa7c3648();
   input += synapse0xa7c3670();
   input += synapse0xa7c3698();
   input += synapse0xa7c36c0();
   input += synapse0xa7c36e8();
   input += synapse0xa7c3710();
   input += synapse0xa7c3738();
   input += synapse0xa7c3760();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_magnet::neuron0xa7c3420() {
   double input = input0xa7c3420();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_magnet::input0xa7c37b0() {
   double input = -0.0173242;
   input += synapse0xa7c3788();
   input += synapse0xa7c3a10();
   input += synapse0xa7c3a38();
   input += synapse0xa7c3a60();
   input += synapse0xa7c3a88();
   input += synapse0xa79f1d0();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_magnet::neuron0xa7c37b0() {
   double input = input0xa7c37b0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_magnet::input0xa7c2f88() {
   double input = -0.00807048;
   input += synapse0xa455318();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_magnet::neuron0xa7c2f88() {
   double input = input0xa7c2f88();
   return (input * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa79f1a8() {
   return (neuron0xa7c0ec8()*-0.0269716);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c24c0() {
   return (neuron0xa7c1078()*-3.14352);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c24e8() {
   return (neuron0xa7c1270()*0.0275006);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2510() {
   return (neuron0xa7c1468()*-0.530119);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2538() {
   return (neuron0xa7c1660()*0.111744);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2560() {
   return (neuron0xa7c1858()*-0.037511);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2588() {
   return (neuron0xa7c1a50()*-1.0355);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c25b0() {
   return (neuron0xa7c1c48()*0.162454);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c25d8() {
   return (neuron0xa7c1e40()*-0.000965829);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2600() {
   return (neuron0xa7c2038()*-5.26712);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2800() {
   return (neuron0xa7c0ec8()*-0.541185);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2828() {
   return (neuron0xa7c1078()*-0.36156);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2850() {
   return (neuron0xa7c1270()*0.0113133);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2878() {
   return (neuron0xa7c1468()*-1.09615);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c28a0() {
   return (neuron0xa7c1660()*-1.6685);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c28c8() {
   return (neuron0xa7c1858()*0.785665);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c28f0() {
   return (neuron0xa7c1a50()*0.398815);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c29a0() {
   return (neuron0xa7c1c48()*0.281847);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c29c8() {
   return (neuron0xa7c1e40()*0.560182);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c29f0() {
   return (neuron0xa7c2038()*4.54682);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2ba8() {
   return (neuron0xa7c0ec8()*0.716645);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2bd0() {
   return (neuron0xa7c1078()*0.406415);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2bf8() {
   return (neuron0xa7c1270()*0.0941814);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2c20() {
   return (neuron0xa7c1468()*-0.299332);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2c48() {
   return (neuron0xa7c1660()*-0.362074);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2c70() {
   return (neuron0xa7c1858()*0.0750333);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2c98() {
   return (neuron0xa7c1a50()*-0.0459673);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2cc0() {
   return (neuron0xa7c1c48()*0.334867);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2ce8() {
   return (neuron0xa7c1e40()*0.265437);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2d10() {
   return (neuron0xa7c2038()*-0.1108);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2f10() {
   return (neuron0xa7c0ec8()*0.406553);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2f38() {
   return (neuron0xa7c1078()*0.683176);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2f60() {
   return (neuron0xa7c1270()*0.665308);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa454dd8() {
   return (neuron0xa7c1468()*-0.309949);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa454e00() {
   return (neuron0xa7c1660()*0.00454929);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa454e28() {
   return (neuron0xa7c1858()*1.11055);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2918() {
   return (neuron0xa7c1a50()*0.771259);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2940() {
   return (neuron0xa7c1c48()*0.0463815);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c2968() {
   return (neuron0xa7c1e40()*1.99226);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c3090() {
   return (neuron0xa7c2038()*0.632221);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c3290() {
   return (neuron0xa7c0ec8()*-1.68754);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c32b8() {
   return (neuron0xa7c1078()*1.41253);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c32e0() {
   return (neuron0xa7c1270()*2.07797);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c3308() {
   return (neuron0xa7c1468()*-0.593906);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c3330() {
   return (neuron0xa7c1660()*0.294603);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c3358() {
   return (neuron0xa7c1858()*-0.0273125);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c3380() {
   return (neuron0xa7c1a50()*1.36962);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c33a8() {
   return (neuron0xa7c1c48()*0.0468219);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c33d0() {
   return (neuron0xa7c1e40()*-0.102515);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c33f8() {
   return (neuron0xa7c2038()*1.32572);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c35f8() {
   return (neuron0xa7c0ec8()*-0.0211306);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c3620() {
   return (neuron0xa7c1078()*-0.0719471);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c3648() {
   return (neuron0xa7c1270()*0.0330605);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c3670() {
   return (neuron0xa7c1468()*-0.676411);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c3698() {
   return (neuron0xa7c1660()*-0.354012);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c36c0() {
   return (neuron0xa7c1858()*0.0223825);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c36e8() {
   return (neuron0xa7c1a50()*0.0373739);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c3710() {
   return (neuron0xa7c1c48()*3.73521);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c3738() {
   return (neuron0xa7c1e40()*0.000395958);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c3760() {
   return (neuron0xa7c2038()*-10.3877);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c3788() {
   return (neuron0xa7c2330()*-3.78751);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c3a10() {
   return (neuron0xa7c2628()*4.4724);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c3a38() {
   return (neuron0xa7c2a18()*-0.143705);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c3a60() {
   return (neuron0xa7c2d38()*-0.422643);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa7c3a88() {
   return (neuron0xa7c30b8()*1.73584);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa79f1d0() {
   return (neuron0xa7c3420()*-7.24015);
}

double MuGirlNS::AnnResults_tgcfix_magnet::synapse0xa455318() {
   return (neuron0xa7c37b0()*1.01092);
}

