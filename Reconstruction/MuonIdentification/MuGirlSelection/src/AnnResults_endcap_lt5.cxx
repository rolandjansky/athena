/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResults_endcap_lt5.h"
#include <cmath>

double MuGirlNS::AnnResults_endcap_lt5::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) {
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
         return neuron0x9f5f3d8();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_endcap_lt5::value(int index, double* input) {
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
         return neuron0x9f5f3d8();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_endcap_lt5::neuron0x9f4d618() {
   return m_input0;
}

double MuGirlNS::AnnResults_endcap_lt5::neuron0x9f4d7a8() {
   return m_input1;
}

double MuGirlNS::AnnResults_endcap_lt5::neuron0x9f5d720() {
   return m_input2;
}

double MuGirlNS::AnnResults_endcap_lt5::neuron0x9f5d8d0() {
   return m_input3;
}

double MuGirlNS::AnnResults_endcap_lt5::neuron0x9f5dac8() {
   return m_input4;
}

double MuGirlNS::AnnResults_endcap_lt5::neuron0x9f5dcc0() {
   return m_input5;
}

double MuGirlNS::AnnResults_endcap_lt5::neuron0x9f5deb8() {
   return m_input6;
}

double MuGirlNS::AnnResults_endcap_lt5::neuron0x9f5e0b0() {
   return m_input7;
}

double MuGirlNS::AnnResults_endcap_lt5::neuron0x9f5e2a8() {
   return m_input8;
}

double MuGirlNS::AnnResults_endcap_lt5::neuron0x9f5e4a0() {
   return m_input9;
}

double MuGirlNS::AnnResults_endcap_lt5::input0x9f5e7a8() {
   double input = 0.978729;
   input += synapse0x9f61678();
   input += synapse0x9f3c100();
   input += synapse0x9f5e938();
   input += synapse0x9f5e960();
   input += synapse0x9f5e988();
   input += synapse0x9f5e9b0();
   input += synapse0x9f5e9d8();
   input += synapse0x9f5ea00();
   input += synapse0x9f5ea28();
   input += synapse0x9f5ea50();
   return input;
}

double MuGirlNS::AnnResults_endcap_lt5::neuron0x9f5e7a8() {
   double input = input0x9f5e7a8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_endcap_lt5::input0x9f5ea78() {
   double input = -1.65317;
   input += synapse0x9f5ec50();
   input += synapse0x9f5ec78();
   input += synapse0x9f5eca0();
   input += synapse0x9f5ecc8();
   input += synapse0x9f5ecf0();
   input += synapse0x9f5ed18();
   input += synapse0x9f5ed40();
   input += synapse0x9f5edf0();
   input += synapse0x9f5ee18();
   input += synapse0x9f5ee40();
   return input;
}

double MuGirlNS::AnnResults_endcap_lt5::neuron0x9f5ea78() {
   double input = input0x9f5ea78();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_endcap_lt5::input0x9f5ee68() {
   double input = 1.04611;
   input += synapse0x9f5eff8();
   input += synapse0x9f5f020();
   input += synapse0x9f5f048();
   input += synapse0x9f5f070();
   input += synapse0x9f5f098();
   input += synapse0x9f5f0c0();
   input += synapse0x9f5f0e8();
   input += synapse0x9f5f110();
   input += synapse0x9f5f138();
   input += synapse0x9f5f160();
   return input;
}

double MuGirlNS::AnnResults_endcap_lt5::neuron0x9f5ee68() {
   double input = input0x9f5ee68();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_endcap_lt5::input0x9f5f188() {
   double input = 2.34615;
   input += synapse0x9f5f360();
   input += synapse0x9f5f388();
   input += synapse0x9f5f3b0();
   input += synapse0x9f3bec0();
   input += synapse0x9f3bee8();
   input += synapse0x9f3bf10();
   input += synapse0x9f5ed68();
   input += synapse0x9f5ed90();
   input += synapse0x9f5edb8();
   input += synapse0x9f5f4e0();
   return input;
}

double MuGirlNS::AnnResults_endcap_lt5::neuron0x9f5f188() {
   double input = input0x9f5f188();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_endcap_lt5::input0x9f5f508() {
   double input = -0.353843;
   input += synapse0x9f5f6e0();
   input += synapse0x9f5f708();
   input += synapse0x9f5f730();
   input += synapse0x9f5f758();
   input += synapse0x9f5f780();
   input += synapse0x9f5f7a8();
   input += synapse0x9f5f7d0();
   input += synapse0x9f5f7f8();
   input += synapse0x9f5f820();
   input += synapse0x9f5f848();
   return input;
}

double MuGirlNS::AnnResults_endcap_lt5::neuron0x9f5f508() {
   double input = input0x9f5f508();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_endcap_lt5::input0x9f5f870() {
   double input = 0.262444;
   input += synapse0x9f5fa48();
   input += synapse0x9f5fa70();
   input += synapse0x9f5fa98();
   input += synapse0x9f5fac0();
   input += synapse0x9f5fae8();
   input += synapse0x9f5fb10();
   input += synapse0x9f5fb38();
   input += synapse0x9f5fb60();
   input += synapse0x9f5fb88();
   input += synapse0x9f5fbb0();
   return input;
}

double MuGirlNS::AnnResults_endcap_lt5::neuron0x9f5f870() {
   double input = input0x9f5f870();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_endcap_lt5::input0x9f5fc00() {
   double input = -4.14551;
   input += synapse0x9f5fbd8();
   input += synapse0x9f5fe60();
   input += synapse0x9f5fe88();
   input += synapse0x9f5feb0();
   input += synapse0x9f5fed8();
   input += synapse0x9f3c1e8();
   return input;
}

double MuGirlNS::AnnResults_endcap_lt5::neuron0x9f5fc00() {
   double input = input0x9f5fc00();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_endcap_lt5::input0x9f5f3d8() {
   double input = 0.021409;
   input += synapse0x89ac398();
   return input;
}

double MuGirlNS::AnnResults_endcap_lt5::neuron0x9f5f3d8() {
   double input = input0x9f5f3d8();
   return (input * 1)+0;
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f61678() {
   return (neuron0x9f4d618()*0.208983);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f3c100() {
   return (neuron0x9f4d7a8()*0.505944);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5e938() {
   return (neuron0x9f5d720()*-0.21159);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5e960() {
   return (neuron0x9f5d8d0()*-0.665081);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5e988() {
   return (neuron0x9f5dac8()*1.84498);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5e9b0() {
   return (neuron0x9f5dcc0()*-0.243378);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5e9d8() {
   return (neuron0x9f5deb8()*-0.102409);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5ea00() {
   return (neuron0x9f5e0b0()*-0.388006);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5ea28() {
   return (neuron0x9f5e2a8()*-3.2384);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5ea50() {
   return (neuron0x9f5e4a0()*-5.33814);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5ec50() {
   return (neuron0x9f4d618()*0.145696);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5ec78() {
   return (neuron0x9f4d7a8()*1.17121);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5eca0() {
   return (neuron0x9f5d720()*-1.40609);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5ecc8() {
   return (neuron0x9f5d8d0()*-2.75653);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5ecf0() {
   return (neuron0x9f5dac8()*-0.184763);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5ed18() {
   return (neuron0x9f5dcc0()*0.0855743);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5ed40() {
   return (neuron0x9f5deb8()*0.0688484);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5edf0() {
   return (neuron0x9f5e0b0()*-0.753543);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5ee18() {
   return (neuron0x9f5e2a8()*0.00729715);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5ee40() {
   return (neuron0x9f5e4a0()*-0.7999);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5eff8() {
   return (neuron0x9f4d618()*-0.0163445);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f020() {
   return (neuron0x9f4d7a8()*0.0192244);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f048() {
   return (neuron0x9f5d720()*-0.0696996);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f070() {
   return (neuron0x9f5d8d0()*-0.0452008);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f098() {
   return (neuron0x9f5dac8()*-0.116828);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f0c0() {
   return (neuron0x9f5dcc0()*0.030647);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f0e8() {
   return (neuron0x9f5deb8()*0.0378761);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f110() {
   return (neuron0x9f5e0b0()*-2.26636);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f138() {
   return (neuron0x9f5e2a8()*-0.0023716);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f160() {
   return (neuron0x9f5e4a0()*25.8977);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f360() {
   return (neuron0x9f4d618()*-0.0256069);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f388() {
   return (neuron0x9f4d7a8()*-0.362867);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f3b0() {
   return (neuron0x9f5d720()*0.0422265);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f3bec0() {
   return (neuron0x9f5d8d0()*-1.82577);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f3bee8() {
   return (neuron0x9f5dac8()*1.4644);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f3bf10() {
   return (neuron0x9f5dcc0()*0.0121087);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5ed68() {
   return (neuron0x9f5deb8()*-0.852321);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5ed90() {
   return (neuron0x9f5e0b0()*3.23594);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5edb8() {
   return (neuron0x9f5e2a8()*0.00069652);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f4e0() {
   return (neuron0x9f5e4a0()*-0.387726);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f6e0() {
   return (neuron0x9f4d618()*0.862897);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f708() {
   return (neuron0x9f4d7a8()*0.303545);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f730() {
   return (neuron0x9f5d720()*0.407619);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f758() {
   return (neuron0x9f5d8d0()*-0.0948952);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f780() {
   return (neuron0x9f5dac8()*0.0106882);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f7a8() {
   return (neuron0x9f5dcc0()*0.923772);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f7d0() {
   return (neuron0x9f5deb8()*0.162903);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f7f8() {
   return (neuron0x9f5e0b0()*0.00496706);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f820() {
   return (neuron0x9f5e2a8()*0.926559);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5f848() {
   return (neuron0x9f5e4a0()*0.489633);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5fa48() {
   return (neuron0x9f4d618()*0.598361);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5fa70() {
   return (neuron0x9f4d7a8()*0.328894);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5fa98() {
   return (neuron0x9f5d720()*0.363646);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5fac0() {
   return (neuron0x9f5d8d0()*0.27932);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5fae8() {
   return (neuron0x9f5dac8()*0.431254);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5fb10() {
   return (neuron0x9f5dcc0()*0.432654);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5fb38() {
   return (neuron0x9f5deb8()*-0.163658);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5fb60() {
   return (neuron0x9f5e0b0()*0.0356056);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5fb88() {
   return (neuron0x9f5e2a8()*0.414324);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5fbb0() {
   return (neuron0x9f5e4a0()*0.363797);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5fbd8() {
   return (neuron0x9f5e7a8()*-6.85233);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5fe60() {
   return (neuron0x9f5ea78()*-4.08547);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5fe88() {
   return (neuron0x9f5ee68()*17.0123);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5feb0() {
   return (neuron0x9f5f188()*-11.2434);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f5fed8() {
   return (neuron0x9f5f508()*-3.52853);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x9f3c1e8() {
   return (neuron0x9f5f870()*-3.25024);
}

double MuGirlNS::AnnResults_endcap_lt5::synapse0x89ac398() {
   return (neuron0x9f5fc00()*0.956497);
}

