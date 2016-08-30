/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResults_barrelg_gt5.h"
#include <cmath>

double MuGirlNS::AnnResults_barrelg_gt5::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) {
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
         return neuron0x98c4fa8();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_barrelg_gt5::value(int index, double* input) {
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
         return neuron0x98c4fa8();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_barrelg_gt5::neuron0x98b49c0() {
   return m_input0;
}

double MuGirlNS::AnnResults_barrelg_gt5::neuron0x98c3088() {
   return m_input1;
}

double MuGirlNS::AnnResults_barrelg_gt5::neuron0x98c3280() {
   return m_input2;
}

double MuGirlNS::AnnResults_barrelg_gt5::neuron0x98c3478() {
   return m_input3;
}

double MuGirlNS::AnnResults_barrelg_gt5::neuron0x98c3670() {
   return m_input4;
}

double MuGirlNS::AnnResults_barrelg_gt5::neuron0x98c3868() {
   return m_input5;
}

double MuGirlNS::AnnResults_barrelg_gt5::neuron0x98c3a60() {
   return m_input6;
}

double MuGirlNS::AnnResults_barrelg_gt5::neuron0x98c3c58() {
   return m_input7;
}

double MuGirlNS::AnnResults_barrelg_gt5::neuron0x98c3e50() {
   return m_input8;
}

double MuGirlNS::AnnResults_barrelg_gt5::neuron0x98c4048() {
   return m_input9;
}

double MuGirlNS::AnnResults_barrelg_gt5::input0x98c4350() {
   double input = -1.02126;
   input += synapse0x98b4800();
   input += synapse0x98c44e0();
   input += synapse0x98c4508();
   input += synapse0x98c4530();
   input += synapse0x98c4558();
   input += synapse0x98c4580();
   input += synapse0x98c45a8();
   input += synapse0x98c45d0();
   input += synapse0x98c45f8();
   input += synapse0x98c4620();
   return input;
}

double MuGirlNS::AnnResults_barrelg_gt5::neuron0x98c4350() {
   double input = input0x98c4350();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_barrelg_gt5::input0x98c4648() {
   double input = -10.0712;
   input += synapse0x98c4820();
   input += synapse0x98c4848();
   input += synapse0x98c4870();
   input += synapse0x98c4898();
   input += synapse0x98c48c0();
   input += synapse0x98c48e8();
   input += synapse0x98c4910();
   input += synapse0x98c49c0();
   input += synapse0x98c49e8();
   input += synapse0x98c4a10();
   return input;
}

double MuGirlNS::AnnResults_barrelg_gt5::neuron0x98c4648() {
   double input = input0x98c4648();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_barrelg_gt5::input0x98c4a38() {
   double input = 1.75394;
   input += synapse0x98c4bc8();
   input += synapse0x98c4bf0();
   input += synapse0x98c4c18();
   input += synapse0x98c4c40();
   input += synapse0x98c4c68();
   input += synapse0x98c4c90();
   input += synapse0x98c4cb8();
   input += synapse0x98c4ce0();
   input += synapse0x98c4d08();
   input += synapse0x98c4d30();
   return input;
}

double MuGirlNS::AnnResults_barrelg_gt5::neuron0x98c4a38() {
   double input = input0x98c4a38();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_barrelg_gt5::input0x98c4d58() {
   double input = -5.41095;
   input += synapse0x98c4f30();
   input += synapse0x98c4f58();
   input += synapse0x98c4f80();
   input += synapse0x9850c38();
   input += synapse0x9850c60();
   input += synapse0x9850a38();
   input += synapse0x9850a60();
   input += synapse0x9850a88();
   input += synapse0x98c4938();
   input += synapse0x98c4960();
   return input;
}

double MuGirlNS::AnnResults_barrelg_gt5::neuron0x98c4d58() {
   double input = input0x98c4d58();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_barrelg_gt5::input0x98c50b0() {
   double input = -4.8289;
   input += synapse0x98c5288();
   input += synapse0x98c52b0();
   input += synapse0x98c52d8();
   input += synapse0x98c5300();
   input += synapse0x98c5328();
   input += synapse0x98c5350();
   input += synapse0x98c5378();
   input += synapse0x98c53a0();
   input += synapse0x98c53c8();
   input += synapse0x98c53f0();
   return input;
}

double MuGirlNS::AnnResults_barrelg_gt5::neuron0x98c50b0() {
   double input = input0x98c50b0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_barrelg_gt5::input0x98c5418() {
   double input = -1.53674;
   input += synapse0x98c55f0();
   input += synapse0x98c5618();
   input += synapse0x98c5640();
   input += synapse0x98c5668();
   input += synapse0x98c5690();
   input += synapse0x98c56b8();
   input += synapse0x98c56e0();
   input += synapse0x98c5708();
   input += synapse0x98c5730();
   input += synapse0x98c5758();
   return input;
}

double MuGirlNS::AnnResults_barrelg_gt5::neuron0x98c5418() {
   double input = input0x98c5418();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_barrelg_gt5::input0x98c57a8() {
   double input = -0.340551;
   input += synapse0x98c5780();
   input += synapse0x98c5a08();
   input += synapse0x98c5a30();
   input += synapse0x98c5a58();
   input += synapse0x98c5a80();
   input += synapse0x9850d60();
   return input;
}

double MuGirlNS::AnnResults_barrelg_gt5::neuron0x98c57a8() {
   double input = input0x98c57a8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_barrelg_gt5::input0x98c4fa8() {
   double input = 1.39167;
   input += synapse0x9850fa8();
   return input;
}

double MuGirlNS::AnnResults_barrelg_gt5::neuron0x98c4fa8() {
   double input = input0x98c4fa8();
   return (input * 1)+0;
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98b4800() {
   return (neuron0x98b49c0()*0.0247781);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c44e0() {
   return (neuron0x98c3088()*2.4759);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4508() {
   return (neuron0x98c3280()*-0.980814);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4530() {
   return (neuron0x98c3478()*0.848109);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4558() {
   return (neuron0x98c3670()*0.0485995);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4580() {
   return (neuron0x98c3868()*0.468003);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c45a8() {
   return (neuron0x98c3a60()*-0.538351);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c45d0() {
   return (neuron0x98c3c58()*-0.177359);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c45f8() {
   return (neuron0x98c3e50()*1.58289);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4620() {
   return (neuron0x98c4048()*-0.624367);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4820() {
   return (neuron0x98b49c0()*0.0731708);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4848() {
   return (neuron0x98c3088()*1.21242);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4870() {
   return (neuron0x98c3280()*-0.713825);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4898() {
   return (neuron0x98c3478()*5.33643);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c48c0() {
   return (neuron0x98c3670()*0.68553);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c48e8() {
   return (neuron0x98c3868()*-0.0940834);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4910() {
   return (neuron0x98c3a60()*1.10916);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c49c0() {
   return (neuron0x98c3c58()*-1.36097);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c49e8() {
   return (neuron0x98c3e50()*-0.0108434);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4a10() {
   return (neuron0x98c4048()*5.19593);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4bc8() {
   return (neuron0x98b49c0()*-0.164354);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4bf0() {
   return (neuron0x98c3088()*-0.94458);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4c18() {
   return (neuron0x98c3280()*-0.26258);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4c40() {
   return (neuron0x98c3478()*-0.158653);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4c68() {
   return (neuron0x98c3670()*-0.105869);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4c90() {
   return (neuron0x98c3868()*0.136171);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4cb8() {
   return (neuron0x98c3a60()*0.165027);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4ce0() {
   return (neuron0x98c3c58()*-0.401393);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4d08() {
   return (neuron0x98c3e50()*-3.3129);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4d30() {
   return (neuron0x98c4048()*-0.608613);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4f30() {
   return (neuron0x98b49c0()*-4.68951);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4f58() {
   return (neuron0x98c3088()*-5.34908);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4f80() {
   return (neuron0x98c3280()*-3.07403);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x9850c38() {
   return (neuron0x98c3478()*-1.90195);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x9850c60() {
   return (neuron0x98c3670()*0.189478);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x9850a38() {
   return (neuron0x98c3868()*-0.292752);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x9850a60() {
   return (neuron0x98c3a60()*-0.823373);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x9850a88() {
   return (neuron0x98c3c58()*0.278872);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4938() {
   return (neuron0x98c3e50()*3.0998);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c4960() {
   return (neuron0x98c4048()*4.50634);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c5288() {
   return (neuron0x98b49c0()*-2.10822);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c52b0() {
   return (neuron0x98c3088()*-3.56813);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c52d8() {
   return (neuron0x98c3280()*-4.81161);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c5300() {
   return (neuron0x98c3478()*4.39627);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c5328() {
   return (neuron0x98c3670()*1.35995);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c5350() {
   return (neuron0x98c3868()*2.22297);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c5378() {
   return (neuron0x98c3a60()*5.93391);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c53a0() {
   return (neuron0x98c3c58()*-0.527615);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c53c8() {
   return (neuron0x98c3e50()*-0.248476);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c53f0() {
   return (neuron0x98c4048()*10.3856);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c55f0() {
   return (neuron0x98b49c0()*0.0233374);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c5618() {
   return (neuron0x98c3088()*0.00532508);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c5640() {
   return (neuron0x98c3280()*-0.0252192);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c5668() {
   return (neuron0x98c3478()*0.27621);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c5690() {
   return (neuron0x98c3670()*-0.550859);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c56b8() {
   return (neuron0x98c3868()*-0.0710493);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c56e0() {
   return (neuron0x98c3a60()*0.0208137);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c5708() {
   return (neuron0x98c3c58()*7.21536);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c5730() {
   return (neuron0x98c3e50()*0.000659023);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c5758() {
   return (neuron0x98c4048()*-3.28089);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c5780() {
   return (neuron0x98c4350()*2.33377);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c5a08() {
   return (neuron0x98c4648()*-2.36475);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c5a30() {
   return (neuron0x98c4a38()*-3.33611);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c5a58() {
   return (neuron0x98c4d58()*0.450815);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x98c5a80() {
   return (neuron0x98c50b0()*-0.53604);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x9850d60() {
   return (neuron0x98c5418()*8.64351);
}

double MuGirlNS::AnnResults_barrelg_gt5::synapse0x9850fa8() {
   return (neuron0x98c57a8()*-1.39056);
}

