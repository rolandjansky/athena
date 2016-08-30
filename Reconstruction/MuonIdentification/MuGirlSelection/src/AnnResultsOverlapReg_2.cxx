/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResultsOverlapReg_2.h"
#include <cmath>

double MuGirlNS::AnnResultsOverlapReg_2::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) {
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
         return neuron0x90c8a88();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResultsOverlapReg_2::neuron0x90b8818() {
   return m_input0;
}

double MuGirlNS::AnnResultsOverlapReg_2::neuron0x90b89c8() {
   return m_input1;
}

double MuGirlNS::AnnResultsOverlapReg_2::neuron0x90b8bc0() {
   return m_input2;
}

double MuGirlNS::AnnResultsOverlapReg_2::neuron0x90b8db8() {
   return m_input3;
}

double MuGirlNS::AnnResultsOverlapReg_2::neuron0x90c7200() {
   return m_input4;
}

double MuGirlNS::AnnResultsOverlapReg_2::neuron0x90c73d8() {
   return m_input5;
}

double MuGirlNS::AnnResultsOverlapReg_2::neuron0x90c75b0() {
   return m_input6;
}

double MuGirlNS::AnnResultsOverlapReg_2::neuron0x90c7788() {
   return m_input7;
}

double MuGirlNS::AnnResultsOverlapReg_2::neuron0x90c7960() {
   return m_input8;
}

double MuGirlNS::AnnResultsOverlapReg_2::neuron0x90c7b38() {
   return m_input9;
}

double MuGirlNS::AnnResultsOverlapReg_2::input0x90c7e30() {
   double input = 0.0280473;
   input += synapse0x8dffbd0();
   input += synapse0x90c7fc0();
   input += synapse0x90c7fe8();
   input += synapse0x90c8010();
   input += synapse0x90c8038();
   input += synapse0x90c8060();
   input += synapse0x90c8088();
   input += synapse0x90c80b0();
   input += synapse0x90c80d8();
   input += synapse0x90c8100();
   return input;
}

double MuGirlNS::AnnResultsOverlapReg_2::neuron0x90c7e30() {
   double input = input0x90c7e30();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsOverlapReg_2::input0x90c8128() {
   double input = 2.63199;
   input += synapse0x90c8300();
   input += synapse0x90c8328();
   input += synapse0x90c8350();
   input += synapse0x90c8378();
   input += synapse0x90c83a0();
   input += synapse0x90c83c8();
   input += synapse0x90c83f0();
   input += synapse0x90c84a0();
   input += synapse0x90c84c8();
   input += synapse0x90c84f0();
   return input;
}

double MuGirlNS::AnnResultsOverlapReg_2::neuron0x90c8128() {
   double input = input0x90c8128();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsOverlapReg_2::input0x90c8518() {
   double input = -3.21103;
   input += synapse0x90c86a8();
   input += synapse0x90c86d0();
   input += synapse0x90c86f8();
   input += synapse0x90c8720();
   input += synapse0x90c8748();
   input += synapse0x90c8770();
   input += synapse0x90c8798();
   input += synapse0x90c87c0();
   input += synapse0x90c87e8();
   input += synapse0x90c8810();
   return input;
}

double MuGirlNS::AnnResultsOverlapReg_2::neuron0x90c8518() {
   double input = input0x90c8518();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsOverlapReg_2::input0x90c8838() {
   double input = 1.08354;
   input += synapse0x90c8a10();
   input += synapse0x90c8a38();
   input += synapse0x90c8a60();
   input += synapse0x90c8418();
   input += synapse0x90c8440();
   input += synapse0x90c8468();
   input += synapse0x90c8b90();
   input += synapse0x90c8bb8();
   input += synapse0x90c8be0();
   input += synapse0x90c8c08();
   return input;
}

double MuGirlNS::AnnResultsOverlapReg_2::neuron0x90c8838() {
   double input = input0x90c8838();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsOverlapReg_2::input0x90c8c30() {
   double input = -1.541;
   input += synapse0x90c8e08();
   input += synapse0x90c8e30();
   input += synapse0x90c8e58();
   input += synapse0x90c8e80();
   input += synapse0x90c8ea8();
   input += synapse0x90c8ed0();
   input += synapse0x90c8ef8();
   input += synapse0x90c8f20();
   input += synapse0x90c8f48();
   input += synapse0x90c8f70();
   return input;
}

double MuGirlNS::AnnResultsOverlapReg_2::neuron0x90c8c30() {
   double input = input0x90c8c30();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsOverlapReg_2::input0x90c8f98() {
   double input = -0.368113;
   input += synapse0x90c9170();
   input += synapse0x90c9198();
   input += synapse0x90c91c0();
   input += synapse0x90c91e8();
   input += synapse0x90c9210();
   input += synapse0x90c9238();
   input += synapse0x90c9260();
   input += synapse0x90c9288();
   input += synapse0x90c92b0();
   input += synapse0x90c92d8();
   return input;
}

double MuGirlNS::AnnResultsOverlapReg_2::neuron0x90c8f98() {
   double input = input0x90c8f98();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsOverlapReg_2::input0x90c9328() {
   double input = -8.8635;
   input += synapse0x90c9300();
   input += synapse0x90c9588();
   input += synapse0x90c95b0();
   input += synapse0x90c95d8();
   input += synapse0x90c9600();
   input += synapse0x8dffca8();
   return input;
}

double MuGirlNS::AnnResultsOverlapReg_2::neuron0x90c9328() {
   double input = input0x90c9328();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsOverlapReg_2::input0x90c8a88() {
   double input = -0.00432951;
   input += synapse0x90c7d58();
   return input;
}

double MuGirlNS::AnnResultsOverlapReg_2::neuron0x90c8a88() {
   double input = input0x90c8a88();
   return (input * 1)+0;
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x8dffbd0() {
   return (neuron0x90b8818()*-0.0194994);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c7fc0() {
   return (neuron0x90b89c8()*0.028098);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c7fe8() {
   return (neuron0x90b8bc0()*-0.0222735);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8010() {
   return (neuron0x90b8db8()*0.248886);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8038() {
   return (neuron0x90c7200()*-0.0197297);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8060() {
   return (neuron0x90c73d8()*-0.00786912);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8088() {
   return (neuron0x90c75b0()*0.023648);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c80b0() {
   return (neuron0x90c7788()*-6.2814);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c80d8() {
   return (neuron0x90c7960()*-0.00057941);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8100() {
   return (neuron0x90c7b38()*9.1253);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8300() {
   return (neuron0x90b8818()*-0.94726);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8328() {
   return (neuron0x90b89c8()*1.76179);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8350() {
   return (neuron0x90b8bc0()*0.0611604);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8378() {
   return (neuron0x90b8db8()*-0.552934);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c83a0() {
   return (neuron0x90c7200()*3.16589);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c83c8() {
   return (neuron0x90c73d8()*-0.111591);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c83f0() {
   return (neuron0x90c75b0()*0.0995783);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c84a0() {
   return (neuron0x90c7788()*-0.358615);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c84c8() {
   return (neuron0x90c7960()*-2.24042);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c84f0() {
   return (neuron0x90c7b38()*-2.13492);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c86a8() {
   return (neuron0x90b8818()*0.158137);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c86d0() {
   return (neuron0x90b89c8()*5.31995);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c86f8() {
   return (neuron0x90b8bc0()*-0.789418);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8720() {
   return (neuron0x90b8db8()*1.05384);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8748() {
   return (neuron0x90c7200()*0.239214);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8770() {
   return (neuron0x90c73d8()*-0.0447321);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8798() {
   return (neuron0x90c75b0()*-0.200567);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c87c0() {
   return (neuron0x90c7788()*-6.59977);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c87e8() {
   return (neuron0x90c7960()*0.00217899);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8810() {
   return (neuron0x90c7b38()*8.70633);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8a10() {
   return (neuron0x90b8818()*-0.0570944);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8a38() {
   return (neuron0x90b89c8()*2.0441);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8a60() {
   return (neuron0x90b8bc0()*-4.11972);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8418() {
   return (neuron0x90b8db8()*0.141189);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8440() {
   return (neuron0x90c7200()*-0.4822);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8468() {
   return (neuron0x90c73d8()*-0.166512);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8b90() {
   return (neuron0x90c75b0()*-2.70084);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8bb8() {
   return (neuron0x90c7788()*0.504176);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8be0() {
   return (neuron0x90c7960()*-0.206734);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8c08() {
   return (neuron0x90c7b38()*-4.25767);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8e08() {
   return (neuron0x90b8818()*-0.480854);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8e30() {
   return (neuron0x90b89c8()*-0.00403376);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8e58() {
   return (neuron0x90b8bc0()*0.740758);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8e80() {
   return (neuron0x90b8db8()*1.60795);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8ea8() {
   return (neuron0x90c7200()*3.35803);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8ed0() {
   return (neuron0x90c73d8()*1.55457);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8ef8() {
   return (neuron0x90c75b0()*0.972121);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8f20() {
   return (neuron0x90c7788()*-0.610735);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8f48() {
   return (neuron0x90c7960()*-0.341751);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c8f70() {
   return (neuron0x90c7b38()*3.51023);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c9170() {
   return (neuron0x90b8818()*-0.993656);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c9198() {
   return (neuron0x90b89c8()*0.755023);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c91c0() {
   return (neuron0x90b8bc0()*1.50397);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c91e8() {
   return (neuron0x90b8db8()*5.74479);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c9210() {
   return (neuron0x90c7200()*-1.15181);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c9238() {
   return (neuron0x90c73d8()*3.11818);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c9260() {
   return (neuron0x90c75b0()*2.83896);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c9288() {
   return (neuron0x90c7788()*0.361104);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c92b0() {
   return (neuron0x90c7960()*-0.772132);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c92d8() {
   return (neuron0x90c7b38()*-1.50362);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c9300() {
   return (neuron0x90c7e30()*10.4992);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c9588() {
   return (neuron0x90c8128()*-4.9559);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c95b0() {
   return (neuron0x90c8518()*4.51409);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c95d8() {
   return (neuron0x90c8838()*-3.90247);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c9600() {
   return (neuron0x90c8c30()*7.1136);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x8dffca8() {
   return (neuron0x90c8f98()*-5.80008);
}

double MuGirlNS::AnnResultsOverlapReg_2::synapse0x90c7d58() {
   return (neuron0x90c9328()*0.99731);
}

