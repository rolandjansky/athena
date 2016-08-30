/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResults_forward_gt5.h"
#include <cmath>

double MuGirlNS::AnnResults_forward_gt5::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) {
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
         return neuron0xb2b5b08();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_forward_gt5::value(int index, double* input) {
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
         return neuron0xb2b5b08();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_forward_gt5::neuron0xb2a5640() {
   return m_input0;
}

double MuGirlNS::AnnResults_forward_gt5::neuron0xb2b3c10() {
   return m_input1;
}

double MuGirlNS::AnnResults_forward_gt5::neuron0xb2b3e08() {
   return m_input2;
}

double MuGirlNS::AnnResults_forward_gt5::neuron0xb2b4000() {
   return m_input3;
}

double MuGirlNS::AnnResults_forward_gt5::neuron0xb2b41f8() {
   return m_input4;
}

double MuGirlNS::AnnResults_forward_gt5::neuron0xb2b43f0() {
   return m_input5;
}

double MuGirlNS::AnnResults_forward_gt5::neuron0xb2b45e8() {
   return m_input6;
}

double MuGirlNS::AnnResults_forward_gt5::neuron0xb2b47e0() {
   return m_input7;
}

double MuGirlNS::AnnResults_forward_gt5::neuron0xb2b49d8() {
   return m_input8;
}

double MuGirlNS::AnnResults_forward_gt5::neuron0xb2b4bd0() {
   return m_input9;
}

double MuGirlNS::AnnResults_forward_gt5::input0xb2b4ed8() {
   double input = -0.113942;
   input += synapse0xb2a5480();
   input += synapse0xb283818();
   input += synapse0xb2b5068();
   input += synapse0xb2b5090();
   input += synapse0xb2b50b8();
   input += synapse0xb2b50e0();
   input += synapse0xb2b5108();
   input += synapse0xb2b5130();
   input += synapse0xb2b5158();
   input += synapse0xb2b5180();
   return input;
}

double MuGirlNS::AnnResults_forward_gt5::neuron0xb2b4ed8() {
   double input = input0xb2b4ed8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_forward_gt5::input0xb2b51a8() {
   double input = -0.850014;
   input += synapse0xb2b5380();
   input += synapse0xb2b53a8();
   input += synapse0xb2b53d0();
   input += synapse0xb2b53f8();
   input += synapse0xb2b5420();
   input += synapse0xb2b5448();
   input += synapse0xb2b5470();
   input += synapse0xb2b5520();
   input += synapse0xb2b5548();
   input += synapse0xb2b5570();
   return input;
}

double MuGirlNS::AnnResults_forward_gt5::neuron0xb2b51a8() {
   double input = input0xb2b51a8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_forward_gt5::input0xb2b5598() {
   double input = -0.308066;
   input += synapse0xb2b5728();
   input += synapse0xb2b5750();
   input += synapse0xb2b5778();
   input += synapse0xb2b57a0();
   input += synapse0xb2b57c8();
   input += synapse0xb2b57f0();
   input += synapse0xb2b5818();
   input += synapse0xb2b5840();
   input += synapse0xb2b5868();
   input += synapse0xb2b5890();
   return input;
}

double MuGirlNS::AnnResults_forward_gt5::neuron0xb2b5598() {
   double input = input0xb2b5598();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_forward_gt5::input0xb2b58b8() {
   double input = 0.267674;
   input += synapse0xb2b5a90();
   input += synapse0xb2b5ab8();
   input += synapse0xb2b5ae0();
   input += synapse0xb1ea0a0();
   input += synapse0xb1ea0c8();
   input += synapse0xb1e9ea0();
   input += synapse0xb1e9ec8();
   input += synapse0xb1e9ef0();
   input += synapse0xb2b5498();
   input += synapse0xb2b54c0();
   return input;
}

double MuGirlNS::AnnResults_forward_gt5::neuron0xb2b58b8() {
   double input = input0xb2b58b8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_forward_gt5::input0xb2b5c10() {
   double input = -3.47994;
   input += synapse0xb2b5de8();
   input += synapse0xb2b5e10();
   input += synapse0xb2b5e38();
   input += synapse0xb2b5e60();
   input += synapse0xb2b5e88();
   input += synapse0xb2b5eb0();
   input += synapse0xb2b5ed8();
   input += synapse0xb2b5f00();
   input += synapse0xb2b5f28();
   input += synapse0xb2b5f50();
   return input;
}

double MuGirlNS::AnnResults_forward_gt5::neuron0xb2b5c10() {
   double input = input0xb2b5c10();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_forward_gt5::input0xb2b5f78() {
   double input = -8.87715;
   input += synapse0xb2b6150();
   input += synapse0xb2b6178();
   input += synapse0xb2b61a0();
   input += synapse0xb2b61c8();
   input += synapse0xb2b61f0();
   input += synapse0xb2b6218();
   input += synapse0xb2b6240();
   input += synapse0xb2b6268();
   input += synapse0xb2b6290();
   input += synapse0xb2b62b8();
   return input;
}

double MuGirlNS::AnnResults_forward_gt5::neuron0xb2b5f78() {
   double input = input0xb2b5f78();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_forward_gt5::input0xb2b6308() {
   double input = -4.19615;
   input += synapse0xb2b62e0();
   input += synapse0xb2b6568();
   input += synapse0xb2b6590();
   input += synapse0xb2b65b8();
   input += synapse0xb2b65e0();
   input += synapse0xb1ea240();
   return input;
}

double MuGirlNS::AnnResults_forward_gt5::neuron0xb2b6308() {
   double input = input0xb2b6308();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_forward_gt5::input0xb2b5b08() {
   double input = 0.0183369;
   input += synapse0xb1ea3f0();
   return input;
}

double MuGirlNS::AnnResults_forward_gt5::neuron0xb2b5b08() {
   double input = input0xb2b5b08();
   return (input * 1)+0;
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2a5480() {
   return (neuron0xb2a5640()*-1.27034);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb283818() {
   return (neuron0xb2b3c10()*1.20108);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5068() {
   return (neuron0xb2b3e08()*-0.474344);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5090() {
   return (neuron0xb2b4000()*-0.466293);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b50b8() {
   return (neuron0xb2b41f8()*-0.320479);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b50e0() {
   return (neuron0xb2b43f0()*-1.51252);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5108() {
   return (neuron0xb2b45e8()*0.484437);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5130() {
   return (neuron0xb2b47e0()*0.274694);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5158() {
   return (neuron0xb2b49d8()*-1.56555);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5180() {
   return (neuron0xb2b4bd0()*-1.52765);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5380() {
   return (neuron0xb2a5640()*-0.0721752);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b53a8() {
   return (neuron0xb2b3c10()*0.278694);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b53d0() {
   return (neuron0xb2b3e08()*0.89231);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b53f8() {
   return (neuron0xb2b4000()*-1.13282);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5420() {
   return (neuron0xb2b41f8()*0.950699);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5448() {
   return (neuron0xb2b43f0()*0.142714);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5470() {
   return (neuron0xb2b45e8()*-0.682881);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5520() {
   return (neuron0xb2b47e0()*0.271853);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5548() {
   return (neuron0xb2b49d8()*1.48671);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5570() {
   return (neuron0xb2b4bd0()*1.5257);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5728() {
   return (neuron0xb2a5640()*1.22033);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5750() {
   return (neuron0xb2b3c10()*0.0104743);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5778() {
   return (neuron0xb2b3e08()*0.555136);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b57a0() {
   return (neuron0xb2b4000()*0.596868);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b57c8() {
   return (neuron0xb2b41f8()*0.294158);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b57f0() {
   return (neuron0xb2b43f0()*0.519194);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5818() {
   return (neuron0xb2b45e8()*1.09105);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5840() {
   return (neuron0xb2b47e0()*0.373278);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5868() {
   return (neuron0xb2b49d8()*0.706134);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5890() {
   return (neuron0xb2b4bd0()*-0.349079);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5a90() {
   return (neuron0xb2a5640()*-0.0360728);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5ab8() {
   return (neuron0xb2b3c10()*0.0406789);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5ae0() {
   return (neuron0xb2b3e08()*0.313463);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb1ea0a0() {
   return (neuron0xb2b4000()*0.100318);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb1ea0c8() {
   return (neuron0xb2b41f8()*0.515923);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb1e9ea0() {
   return (neuron0xb2b43f0()*0.0184954);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb1e9ec8() {
   return (neuron0xb2b45e8()*-0.167979);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb1e9ef0() {
   return (neuron0xb2b47e0()*-16.6169);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5498() {
   return (neuron0xb2b49d8()*-0.00362407);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b54c0() {
   return (neuron0xb2b4bd0()*0.712581);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5de8() {
   return (neuron0xb2a5640()*-1.35239);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5e10() {
   return (neuron0xb2b3c10()*2.86605);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5e38() {
   return (neuron0xb2b3e08()*-0.341133);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5e60() {
   return (neuron0xb2b4000()*-3.15357);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5e88() {
   return (neuron0xb2b41f8()*0.389058);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5eb0() {
   return (neuron0xb2b43f0()*3.78825);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5ed8() {
   return (neuron0xb2b45e8()*-0.242437);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5f00() {
   return (neuron0xb2b47e0()*-0.820388);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5f28() {
   return (neuron0xb2b49d8()*0.0193752);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b5f50() {
   return (neuron0xb2b4bd0()*3.06309);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b6150() {
   return (neuron0xb2a5640()*0.0121544);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b6178() {
   return (neuron0xb2b3c10()*1.32354);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b61a0() {
   return (neuron0xb2b3e08()*1.36797);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b61c8() {
   return (neuron0xb2b4000()*0.818837);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b61f0() {
   return (neuron0xb2b41f8()*-2.29304);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b6218() {
   return (neuron0xb2b43f0()*-0.436681);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b6240() {
   return (neuron0xb2b45e8()*-2.42135);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b6268() {
   return (neuron0xb2b47e0()*7.83161);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b6290() {
   return (neuron0xb2b49d8()*0.0786319);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b62b8() {
   return (neuron0xb2b4bd0()*-5.28652);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b62e0() {
   return (neuron0xb2b4ed8()*-7.14905);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b6568() {
   return (neuron0xb2b51a8()*-4.62273);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b6590() {
   return (neuron0xb2b5598()*-3.67571);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b65b8() {
   return (neuron0xb2b58b8()*10.4962);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb2b65e0() {
   return (neuron0xb2b5c10()*7.46808);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb1ea240() {
   return (neuron0xb2b5f78()*-8.88072);
}

double MuGirlNS::AnnResults_forward_gt5::synapse0xb1ea3f0() {
   return (neuron0xb2b6308()*0.971539);
}

