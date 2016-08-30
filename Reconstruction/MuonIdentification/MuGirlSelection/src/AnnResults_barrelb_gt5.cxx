/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResults_barrelb_gt5.h"
#include <cmath>

double MuGirlNS::AnnResults_barrelb_gt5::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) {
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
         return neuron0x9d41ed8();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_barrelb_gt5::value(int index, double* input) {
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
         return neuron0x9d41ed8();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_barrelb_gt5::neuron0x9d3fe28() {
   return m_input0;
}

double MuGirlNS::AnnResults_barrelb_gt5::neuron0x9d3ffb8() {
   return m_input1;
}

double MuGirlNS::AnnResults_barrelb_gt5::neuron0x9d401b0() {
   return m_input2;
}

double MuGirlNS::AnnResults_barrelb_gt5::neuron0x9d403a8() {
   return m_input3;
}

double MuGirlNS::AnnResults_barrelb_gt5::neuron0x9d405a0() {
   return m_input4;
}

double MuGirlNS::AnnResults_barrelb_gt5::neuron0x9d40798() {
   return m_input5;
}

double MuGirlNS::AnnResults_barrelb_gt5::neuron0x9d40990() {
   return m_input6;
}

double MuGirlNS::AnnResults_barrelb_gt5::neuron0x9d40b88() {
   return m_input7;
}

double MuGirlNS::AnnResults_barrelb_gt5::neuron0x9d40d80() {
   return m_input8;
}

double MuGirlNS::AnnResults_barrelb_gt5::neuron0x9d40f78() {
   return m_input9;
}

double MuGirlNS::AnnResults_barrelb_gt5::input0x9d41280() {
   double input = -0.799993;
   input += synapse0x9d1e028();
   input += synapse0x9d41410();
   input += synapse0x9d41438();
   input += synapse0x9d41460();
   input += synapse0x9d41488();
   input += synapse0x9d414b0();
   input += synapse0x9d414d8();
   input += synapse0x9d41500();
   input += synapse0x9d41528();
   input += synapse0x9d41550();
   return input;
}

double MuGirlNS::AnnResults_barrelb_gt5::neuron0x9d41280() {
   double input = input0x9d41280();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_barrelb_gt5::input0x9d41578() {
   double input = -0.142843;
   input += synapse0x9d41750();
   input += synapse0x9d41778();
   input += synapse0x9d417a0();
   input += synapse0x9d417c8();
   input += synapse0x9d417f0();
   input += synapse0x9d41818();
   input += synapse0x9d41840();
   input += synapse0x9d418f0();
   input += synapse0x9d41918();
   input += synapse0x9d41940();
   return input;
}

double MuGirlNS::AnnResults_barrelb_gt5::neuron0x9d41578() {
   double input = input0x9d41578();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_barrelb_gt5::input0x9d41968() {
   double input = 0.00519352;
   input += synapse0x9d41af8();
   input += synapse0x9d41b20();
   input += synapse0x9d41b48();
   input += synapse0x9d41b70();
   input += synapse0x9d41b98();
   input += synapse0x9d41bc0();
   input += synapse0x9d41be8();
   input += synapse0x9d41c10();
   input += synapse0x9d41c38();
   input += synapse0x9d41c60();
   return input;
}

double MuGirlNS::AnnResults_barrelb_gt5::neuron0x9d41968() {
   double input = input0x9d41968();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_barrelb_gt5::input0x9d41c88() {
   double input = 14.0287;
   input += synapse0x9d41e60();
   input += synapse0x9d41e88();
   input += synapse0x9d41eb0();
   input += synapse0x9c91220();
   input += synapse0x9c91248();
   input += synapse0x9c91270();
   input += synapse0x9d41868();
   input += synapse0x9d41890();
   input += synapse0x9d418b8();
   input += synapse0x9d41fe0();
   return input;
}

double MuGirlNS::AnnResults_barrelb_gt5::neuron0x9d41c88() {
   double input = input0x9d41c88();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_barrelb_gt5::input0x9d42008() {
   double input = -0.946083;
   input += synapse0x9d421e0();
   input += synapse0x9d42208();
   input += synapse0x9d42230();
   input += synapse0x9d42258();
   input += synapse0x9d42280();
   input += synapse0x9d422a8();
   input += synapse0x9d422d0();
   input += synapse0x9d422f8();
   input += synapse0x9d42320();
   input += synapse0x9d42348();
   return input;
}

double MuGirlNS::AnnResults_barrelb_gt5::neuron0x9d42008() {
   double input = input0x9d42008();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_barrelb_gt5::input0x9d42370() {
   double input = 5.18153;
   input += synapse0x9d42548();
   input += synapse0x9d42570();
   input += synapse0x9d42598();
   input += synapse0x9d425c0();
   input += synapse0x9d425e8();
   input += synapse0x9d42610();
   input += synapse0x9d42638();
   input += synapse0x9d42660();
   input += synapse0x9d42688();
   input += synapse0x9d426b0();
   return input;
}

double MuGirlNS::AnnResults_barrelb_gt5::neuron0x9d42370() {
   double input = input0x9d42370();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_barrelb_gt5::input0x9d42700() {
   double input = 3.45474;
   input += synapse0x9d426d8();
   input += synapse0x9d42960();
   input += synapse0x9d42988();
   input += synapse0x9d429b0();
   input += synapse0x9d429d8();
   input += synapse0x9c915c0();
   return input;
}

double MuGirlNS::AnnResults_barrelb_gt5::neuron0x9d42700() {
   double input = input0x9d42700();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_barrelb_gt5::input0x9d41ed8() {
   double input = 1.02046;
   input += synapse0x9c91770();
   return input;
}

double MuGirlNS::AnnResults_barrelb_gt5::neuron0x9d41ed8() {
   double input = input0x9d41ed8();
   return (input * 1)+0;
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d1e028() {
   return (neuron0x9d3fe28()*-0.00589753);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41410() {
   return (neuron0x9d3ffb8()*0.0536601);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41438() {
   return (neuron0x9d401b0()*-0.0514457);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41460() {
   return (neuron0x9d403a8()*0.105707);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41488() {
   return (neuron0x9d405a0()*0.703502);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d414b0() {
   return (neuron0x9d40798()*0.0442629);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d414d8() {
   return (neuron0x9d40990()*-0.0657803);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41500() {
   return (neuron0x9d40b88()*-6.95133);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41528() {
   return (neuron0x9d40d80()*0.00117886);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41550() {
   return (neuron0x9d40f78()*2.43097);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41750() {
   return (neuron0x9d3fe28()*0.492287);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41778() {
   return (neuron0x9d3ffb8()*0.0404367);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d417a0() {
   return (neuron0x9d401b0()*-5.1505);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d417c8() {
   return (neuron0x9d403a8()*4.5922);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d417f0() {
   return (neuron0x9d405a0()*9.6575);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41818() {
   return (neuron0x9d40798()*-1.62695);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41840() {
   return (neuron0x9d40990()*0.724732);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d418f0() {
   return (neuron0x9d40b88()*-1.93044);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41918() {
   return (neuron0x9d40d80()*-0.0603937);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41940() {
   return (neuron0x9d40f78()*-10.2197);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41af8() {
   return (neuron0x9d3fe28()*0.578096);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41b20() {
   return (neuron0x9d3ffb8()*0.472313);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41b48() {
   return (neuron0x9d401b0()*0.0643838);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41b70() {
   return (neuron0x9d403a8()*-0.210741);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41b98() {
   return (neuron0x9d405a0()*-0.486372);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41bc0() {
   return (neuron0x9d40798()*0.702564);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41be8() {
   return (neuron0x9d40990()*0.284051);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41c10() {
   return (neuron0x9d40b88()*-0.0346037);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41c38() {
   return (neuron0x9d40d80()*0.0981152);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41c60() {
   return (neuron0x9d40f78()*0.0107176);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41e60() {
   return (neuron0x9d3fe28()*-0.438172);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41e88() {
   return (neuron0x9d3ffb8()*-4.83983);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41eb0() {
   return (neuron0x9d401b0()*0.383324);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9c91220() {
   return (neuron0x9d403a8()*-2.18817);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9c91248() {
   return (neuron0x9d405a0()*-2.22418);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9c91270() {
   return (neuron0x9d40798()*-0.732905);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41868() {
   return (neuron0x9d40990()*-1.86995);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41890() {
   return (neuron0x9d40b88()*0.902315);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d418b8() {
   return (neuron0x9d40d80()*0.110943);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d41fe0() {
   return (neuron0x9d40f78()*2.01603);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d421e0() {
   return (neuron0x9d3fe28()*-1.66756);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d42208() {
   return (neuron0x9d3ffb8()*0.925983);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d42230() {
   return (neuron0x9d401b0()*-0.684623);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d42258() {
   return (neuron0x9d403a8()*1.34026);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d42280() {
   return (neuron0x9d405a0()*0.881004);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d422a8() {
   return (neuron0x9d40798()*4.56147);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d422d0() {
   return (neuron0x9d40990()*-0.890313);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d422f8() {
   return (neuron0x9d40b88()*-0.98905);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d42320() {
   return (neuron0x9d40d80()*0.482899);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d42348() {
   return (neuron0x9d40f78()*2.22517);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d42548() {
   return (neuron0x9d3fe28()*-2.51083);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d42570() {
   return (neuron0x9d3ffb8()*2.80658);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d42598() {
   return (neuron0x9d401b0()*1.22902);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d425c0() {
   return (neuron0x9d403a8()*-9.78009);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d425e8() {
   return (neuron0x9d405a0()*-1.10791);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d42610() {
   return (neuron0x9d40798()*2.58229);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d42638() {
   return (neuron0x9d40990()*-5.31145);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d42660() {
   return (neuron0x9d40b88()*-0.180557);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d42688() {
   return (neuron0x9d40d80()*1.76483);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d426b0() {
   return (neuron0x9d40f78()*-13.9693);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d426d8() {
   return (neuron0x9d41280()*-5.71811);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d42960() {
   return (neuron0x9d41578()*7.97102);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d42988() {
   return (neuron0x9d41968()*3.73083);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d429b0() {
   return (neuron0x9d41c88()*3.0114);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9d429d8() {
   return (neuron0x9d42008()*-4.94071);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9c915c0() {
   return (neuron0x9d42370()*1.5181);
}

double MuGirlNS::AnnResults_barrelb_gt5::synapse0x9c91770() {
   return (neuron0x9d42700()*-1.02741);
}

