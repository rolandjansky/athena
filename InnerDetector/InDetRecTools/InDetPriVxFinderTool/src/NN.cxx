/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "NN.h"
#include <cmath>

double NN::value(int index,double in0,double in1,double in2,double in3,double in4,double in5) {
   input0 = (in0 - 12179.3)/28373;
   input1 = (in1 - 4851.2)/10068.7;
   input2 = (in2 - 3081.34)/5700.85;
   input3 = (in3 - 38999.7)/69592.8;
   input4 = (in4 - 1.19444e+09)/7.98268e+09;
   input5 = (in5 - 15.2873)/17.2206;
   switch(index) {
     case 0:
         return ((neuron0x8f1eda8()*1)+0);
     default:
         return 0.;
   }
}

double NN::neuron0x8eed3f8() {
   return input0;
}

double NN::neuron0x8f0ea18() {
   return input1;
}

double NN::neuron0x8f0ec10() {
   return input2;
}

double NN::neuron0x8f0ee08() {
   return input3;
}

double NN::neuron0x8f0f030() {
   return input4;
}

double NN::neuron0x8f1d488() {
   return input5;
}

double NN::input0x8f1d6a0() {
   double input = 3.15854;
   input += synapse0x8eed2f0();
   input += synapse0x8eab6b0();
   input += synapse0x8f0f1a0();
   input += synapse0x8f1d878();
   input += synapse0x8f1d8a0();
   input += synapse0x8f1d8c8();
   return input;
}

double NN::neuron0x8f1d6a0() {
   double input = input0x8f1d6a0();
   return ((1/(1+exp(-input))) * 1)+0;
}

double NN::input0x8f1d8f0() {
   double input = -1.35651;
   input += synapse0x8f1dae8();
   input += synapse0x8f1db10();
   input += synapse0x8f1db38();
   input += synapse0x8f1db60();
   input += synapse0x8f1db88();
   input += synapse0x8f1dbb0();
   return input;
}

double NN::neuron0x8f1d8f0() {
   double input = input0x8f1d8f0();
   return ((1/(1+exp(-input))) * 1)+0;
}

double NN::input0x8f1dbd8() {
   double input = -0.35515;
   input += synapse0x8f1ddd0();
   input += synapse0x8f1ddf8();
   input += synapse0x8f1de20();
   input += synapse0x8f1de48();
   input += synapse0x8f1de70();
   input += synapse0x8f1df20();
   return input;
}

double NN::neuron0x8f1dbd8() {
   double input = input0x8f1dbd8();
   return ((1/(1+exp(-input))) * 1)+0;
}

double NN::input0x8f1df48() {
   double input = 3.38892;
   input += synapse0x8f1e0f8();
   input += synapse0x8f1e120();
   input += synapse0x8f1e148();
   input += synapse0x8f1e170();
   input += synapse0x8f1e198();
   input += synapse0x8f1e1c0();
   return input;
}

double NN::neuron0x8f1df48() {
   double input = input0x8f1df48();
   return ((1/(1+exp(-input))) * 1)+0;
}

double NN::input0x8f1e1e8() {
   double input = 2.45184;
   input += synapse0x8f1e3e0();
   input += synapse0x8f1e408();
   input += synapse0x8f1e430();
   input += synapse0x8f1e458();
   input += synapse0x8f1e480();
   input += synapse0x8f1e4a8();
   return input;
}

double NN::neuron0x8f1e1e8() {
   double input = input0x8f1e1e8();
   return ((1/(1+exp(-input))) * 1)+0;
}

double NN::input0x8f1e4f0() {
   double input = 0.55311;
   input += synapse0x8f1e6c8();
   input += synapse0x8f1e6f0();
   input += synapse0x8f1e718();
   input += synapse0x8f1de98();
   input += synapse0x8f1dec0();
   return input;
}

double NN::neuron0x8f1e4f0() {
   double input = input0x8f1e4f0();
   return ((1/(1+exp(-input))) * 1)+0;
}

double NN::input0x8f1e848() {
   double input = -0.497036;
   input += synapse0x8f1ea20();
   input += synapse0x8f1ea48();
   input += synapse0x8f1ea70();
   input += synapse0x8f1ea98();
   input += synapse0x8f1eac0();
   return input;
}

double NN::neuron0x8f1e848() {
   double input = input0x8f1e848();
   return ((1/(1+exp(-input))) * 1)+0;
}

double NN::input0x8f1eae8() {
   double input = -0.284328;
   input += synapse0x8f1ece0();
   input += synapse0x8f1ed08();
   input += synapse0x8f1ed30();
   input += synapse0x8f1ed58();
   input += synapse0x8f1ed80();
   return input;
}

double NN::neuron0x8f1eae8() {
   double input = input0x8f1eae8();
   return ((1/(1+exp(-input))) * 1)+0;
}

double NN::input0x8f1eda8() {
   double input = 1.83262;
   input += synapse0x8f1eea8();
   input += synapse0x8f1eed0();
   input += synapse0x8f1eef8();
   return input;
}

double NN::neuron0x8f1eda8() {
   double input = input0x8f1eda8();
   return (input * 1)+0;
}

double NN::synapse0x8eed2f0() {
   return (neuron0x8eed3f8()*-10.0479);
}

double NN::synapse0x8eab6b0() {
   return (neuron0x8f0ea18()*-3.58819);
}

double NN::synapse0x8f0f1a0() {
   return (neuron0x8f0ec10()*-4.14678);
}

double NN::synapse0x8f1d878() {
   return (neuron0x8f0ee08()*-4.11811);
}

double NN::synapse0x8f1d8a0() {
   return (neuron0x8f0f030()*-2.21092);
}

double NN::synapse0x8f1d8c8() {
   return (neuron0x8f1d488()*-2.52906);
}

double NN::synapse0x8f1dae8() {
   return (neuron0x8eed3f8()*-4.42327);
}

double NN::synapse0x8f1db10() {
   return (neuron0x8f0ea18()*-0.858581);
}

double NN::synapse0x8f1db38() {
   return (neuron0x8f0ec10()*-2.45253);
}

double NN::synapse0x8f1db60() {
   return (neuron0x8f0ee08()*-1.01795);
}

double NN::synapse0x8f1db88() {
   return (neuron0x8f0f030()*-1.28071);
}

double NN::synapse0x8f1dbb0() {
   return (neuron0x8f1d488()*-0.403271);
}

double NN::synapse0x8f1ddd0() {
   return (neuron0x8eed3f8()*-5.59811);
}

double NN::synapse0x8f1ddf8() {
   return (neuron0x8f0ea18()*-6.63516);
}

double NN::synapse0x8f1de20() {
   return (neuron0x8f0ec10()*4.60976);
}

double NN::synapse0x8f1de48() {
   return (neuron0x8f0ee08()*-10.5752);
}

double NN::synapse0x8f1de70() {
   return (neuron0x8f0f030()*-1.12288);
}

double NN::synapse0x8f1df20() {
   return (neuron0x8f1d488()*1.58035);
}

double NN::synapse0x8f1e0f8() {
   return (neuron0x8eed3f8()*1.3974);
}

double NN::synapse0x8f1e120() {
   return (neuron0x8f0ea18()*1.65807);
}

double NN::synapse0x8f1e148() {
   return (neuron0x8f0ec10()*0.808975);
}

double NN::synapse0x8f1e170() {
   return (neuron0x8f0ee08()*1.61184);
}

double NN::synapse0x8f1e198() {
   return (neuron0x8f0f030()*0.0294601);
}

double NN::synapse0x8f1e1c0() {
   return (neuron0x8f1d488()*7.59528);
}

double NN::synapse0x8f1e3e0() {
   return (neuron0x8eed3f8()*5.19404);
}

double NN::synapse0x8f1e408() {
   return (neuron0x8f0ea18()*1.03161);
}

double NN::synapse0x8f1e430() {
   return (neuron0x8f0ec10()*2.46463);
}

double NN::synapse0x8f1e458() {
   return (neuron0x8f0ee08()*8.29404);
}

double NN::synapse0x8f1e480() {
   return (neuron0x8f0f030()*0.0457888);
}

double NN::synapse0x8f1e4a8() {
   return (neuron0x8f1d488()*-2.58503);
}

double NN::synapse0x8f1e6c8() {
   return (neuron0x8f1d6a0()*-0.070371);
}

double NN::synapse0x8f1e6f0() {
   return (neuron0x8f1d8f0()*0.936618);
}

double NN::synapse0x8f1e718() {
   return (neuron0x8f1dbd8()*-5.67059);
}

double NN::synapse0x8f1de98() {
   return (neuron0x8f1df48()*-2.68732);
}

double NN::synapse0x8f1dec0() {
   return (neuron0x8f1e1e8()*6.87944);
}

double NN::synapse0x8f1ea20() {
   return (neuron0x8f1d6a0()*-7.6595);
}

double NN::synapse0x8f1ea48() {
   return (neuron0x8f1d8f0()*3.01675);
}

double NN::synapse0x8f1ea70() {
   return (neuron0x8f1dbd8()*9.7394);
}

double NN::synapse0x8f1ea98() {
   return (neuron0x8f1df48()*3.07623);
}

double NN::synapse0x8f1eac0() {
   return (neuron0x8f1e1e8()*-7.21309);
}

double NN::synapse0x8f1ece0() {
   return (neuron0x8f1d6a0()*0.0490007);
}

double NN::synapse0x8f1ed08() {
   return (neuron0x8f1d8f0()*-4.12839);
}

double NN::synapse0x8f1ed30() {
   return (neuron0x8f1dbd8()*-5.98721);
}

double NN::synapse0x8f1ed58() {
   return (neuron0x8f1df48()*1.92555);
}

double NN::synapse0x8f1ed80() {
   return (neuron0x8f1e1e8()*2.72995);
}

double NN::synapse0x8f1eea8() {
   return (neuron0x8f1e4f0()*-2.02084);
}

double NN::synapse0x8f1eed0() {
   return (neuron0x8f1e848()*-1.82265);
}

double NN::synapse0x8f1eef8() {
   return (neuron0x8f1eae8()*1.20043);
}

