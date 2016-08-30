/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResults_forward_lt5.h"
#include <cmath>

double MuGirlNS::AnnResults_forward_lt5::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) {
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
         return neuron0xa42ef18();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_forward_lt5::value(int index, double* input) {
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
         return neuron0xa42ef18();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_forward_lt5::neuron0xa41e978() {
   return m_input0;
}

double MuGirlNS::AnnResults_forward_lt5::neuron0xa42d020() {
   return m_input1;
}

double MuGirlNS::AnnResults_forward_lt5::neuron0xa42d218() {
   return m_input2;
}

double MuGirlNS::AnnResults_forward_lt5::neuron0xa42d410() {
   return m_input3;
}

double MuGirlNS::AnnResults_forward_lt5::neuron0xa42d608() {
   return m_input4;
}

double MuGirlNS::AnnResults_forward_lt5::neuron0xa42d800() {
   return m_input5;
}

double MuGirlNS::AnnResults_forward_lt5::neuron0xa42d9f8() {
   return m_input6;
}

double MuGirlNS::AnnResults_forward_lt5::neuron0xa42dbf0() {
   return m_input7;
}

double MuGirlNS::AnnResults_forward_lt5::neuron0xa42dde8() {
   return m_input8;
}

double MuGirlNS::AnnResults_forward_lt5::neuron0xa42dfe0() {
   return m_input9;
}

double MuGirlNS::AnnResults_forward_lt5::input0xa42e2e8() {
   double input = -0.00948697;
   input += synapse0xa41e798();
   input += synapse0xa3fcb80();
   input += synapse0xa42e478();
   input += synapse0xa42e4a0();
   input += synapse0xa42e4c8();
   input += synapse0xa42e4f0();
   input += synapse0xa42e518();
   input += synapse0xa42e540();
   input += synapse0xa42e568();
   input += synapse0xa42e590();
   return input;
}

double MuGirlNS::AnnResults_forward_lt5::neuron0xa42e2e8() {
   double input = input0xa42e2e8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_forward_lt5::input0xa42e5b8() {
   double input = 3.29228;
   input += synapse0xa42e790();
   input += synapse0xa42e7b8();
   input += synapse0xa42e7e0();
   input += synapse0xa42e808();
   input += synapse0xa42e830();
   input += synapse0xa42e858();
   input += synapse0xa42e880();
   input += synapse0xa42e930();
   input += synapse0xa42e958();
   input += synapse0xa42e980();
   return input;
}

double MuGirlNS::AnnResults_forward_lt5::neuron0xa42e5b8() {
   double input = input0xa42e5b8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_forward_lt5::input0xa42e9a8() {
   double input = -5.00939;
   input += synapse0xa42eb38();
   input += synapse0xa42eb60();
   input += synapse0xa42eb88();
   input += synapse0xa42ebb0();
   input += synapse0xa42ebd8();
   input += synapse0xa42ec00();
   input += synapse0xa42ec28();
   input += synapse0xa42ec50();
   input += synapse0xa42ec78();
   input += synapse0xa42eca0();
   return input;
}

double MuGirlNS::AnnResults_forward_lt5::neuron0xa42e9a8() {
   double input = input0xa42e9a8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_forward_lt5::input0xa42ecc8() {
   double input = -1.06;
   input += synapse0xa42eea0();
   input += synapse0xa42eec8();
   input += synapse0xa42eef0();
   input += synapse0xa37d2f8();
   input += synapse0xa37d320();
   input += synapse0xa37d0f8();
   input += synapse0xa37d120();
   input += synapse0xa37d148();
   input += synapse0xa42e8a8();
   input += synapse0xa42e8d0();
   return input;
}

double MuGirlNS::AnnResults_forward_lt5::neuron0xa42ecc8() {
   double input = input0xa42ecc8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_forward_lt5::input0xa42f020() {
   double input = 5.43156;
   input += synapse0xa42f1f8();
   input += synapse0xa42f220();
   input += synapse0xa42f248();
   input += synapse0xa42f270();
   input += synapse0xa42f298();
   input += synapse0xa42f2c0();
   input += synapse0xa42f2e8();
   input += synapse0xa42f310();
   input += synapse0xa42f338();
   input += synapse0xa42f360();
   return input;
}

double MuGirlNS::AnnResults_forward_lt5::neuron0xa42f020() {
   double input = input0xa42f020();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_forward_lt5::input0xa42f388() {
   double input = 0.370354;
   input += synapse0xa42f560();
   input += synapse0xa42f588();
   input += synapse0xa42f5b0();
   input += synapse0xa42f5d8();
   input += synapse0xa42f600();
   input += synapse0xa42f628();
   input += synapse0xa42f650();
   input += synapse0xa42f678();
   input += synapse0xa42f6a0();
   input += synapse0xa42f6c8();
   return input;
}

double MuGirlNS::AnnResults_forward_lt5::neuron0xa42f388() {
   double input = input0xa42f388();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_forward_lt5::input0xa42f718() {
   double input = 0.192077;
   input += synapse0xa42f6f0();
   input += synapse0xa42f978();
   input += synapse0xa42f9a0();
   input += synapse0xa42f9c8();
   input += synapse0xa42f9f0();
   input += synapse0xa37d498();
   return input;
}

double MuGirlNS::AnnResults_forward_lt5::neuron0xa42f718() {
   double input = input0xa42f718();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_forward_lt5::input0xa42ef18() {
   double input = 0.045857;
   input += synapse0xa37d648();
   return input;
}

double MuGirlNS::AnnResults_forward_lt5::neuron0xa42ef18() {
   double input = input0xa42ef18();
   return (input * 1)+0;
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa41e798() {
   return (neuron0xa41e978()*0.395239);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa3fcb80() {
   return (neuron0xa42d020()*0.0953844);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e478() {
   return (neuron0xa42d218()*0.0442248);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e4a0() {
   return (neuron0xa42d410()*-0.513906);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e4c8() {
   return (neuron0xa42d608()*-0.237661);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e4f0() {
   return (neuron0xa42d800()*0.333435);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e518() {
   return (neuron0xa42d9f8()*0.477496);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e540() {
   return (neuron0xa42dbf0()*-0.165418);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e568() {
   return (neuron0xa42dde8()*0.682876);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e590() {
   return (neuron0xa42dfe0()*0.0207972);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e790() {
   return (neuron0xa41e978()*-1.02575);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e7b8() {
   return (neuron0xa42d020()*-3.80704);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e7e0() {
   return (neuron0xa42d218()*3.86579);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e808() {
   return (neuron0xa42d410()*10.3897);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e830() {
   return (neuron0xa42d608()*1.61179);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e858() {
   return (neuron0xa42d800()*-2.57238);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e880() {
   return (neuron0xa42d9f8()*-2.08523);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e930() {
   return (neuron0xa42dbf0()*-0.961077);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e958() {
   return (neuron0xa42dde8()*0.00553506);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e980() {
   return (neuron0xa42dfe0()*1.05042);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42eb38() {
   return (neuron0xa41e978()*-0.520139);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42eb60() {
   return (neuron0xa42d020()*-1.82552);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42eb88() {
   return (neuron0xa42d218()*0.836749);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42ebb0() {
   return (neuron0xa42d410()*-0.0221988);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42ebd8() {
   return (neuron0xa42d608()*-6.92682);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42ec00() {
   return (neuron0xa42d800()*2.65251);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42ec28() {
   return (neuron0xa42d9f8()*1.38158);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42ec50() {
   return (neuron0xa42dbf0()*3.3783);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42ec78() {
   return (neuron0xa42dde8()*-0.0010252);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42eca0() {
   return (neuron0xa42dfe0()*9.2952);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42eea0() {
   return (neuron0xa41e978()*-3.64531);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42eec8() {
   return (neuron0xa42d020()*0.803155);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42eef0() {
   return (neuron0xa42d218()*0.904193);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa37d2f8() {
   return (neuron0xa42d410()*0.00316786);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa37d320() {
   return (neuron0xa42d608()*0.118197);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa37d0f8() {
   return (neuron0xa42d800()*2.84872);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa37d120() {
   return (neuron0xa42d9f8()*0.894388);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa37d148() {
   return (neuron0xa42dbf0()*0.0770134);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e8a8() {
   return (neuron0xa42dde8()*2.81897);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42e8d0() {
   return (neuron0xa42dfe0()*2.0494);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f1f8() {
   return (neuron0xa41e978()*0.0531924);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f220() {
   return (neuron0xa42d020()*-2.45311);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f248() {
   return (neuron0xa42d218()*-4.59669);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f270() {
   return (neuron0xa42d410()*-8.15312);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f298() {
   return (neuron0xa42d608()*-0.277929);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f2c0() {
   return (neuron0xa42d800()*0.165425);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f2e8() {
   return (neuron0xa42d9f8()*-0.534725);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f310() {
   return (neuron0xa42dbf0()*1.33487);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f338() {
   return (neuron0xa42dde8()*0.012897);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f360() {
   return (neuron0xa42dfe0()*5.14631);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f560() {
   return (neuron0xa41e978()*0.0861188);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f588() {
   return (neuron0xa42d020()*-0.2037);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f5b0() {
   return (neuron0xa42d218()*-0.064111);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f5d8() {
   return (neuron0xa42d410()*-0.637119);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f600() {
   return (neuron0xa42d608()*-1.19075);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f628() {
   return (neuron0xa42d800()*0.0290597);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f650() {
   return (neuron0xa42d9f8()*0.166533);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f678() {
   return (neuron0xa42dbf0()*16.622);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f6a0() {
   return (neuron0xa42dde8()*0.000575795);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f6c8() {
   return (neuron0xa42dfe0()*-2.03964);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f6f0() {
   return (neuron0xa42e2e8()*0.336087);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f978() {
   return (neuron0xa42e5b8()*-2.96136);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f9a0() {
   return (neuron0xa42e9a8()*11.894);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f9c8() {
   return (neuron0xa42ecc8()*-6.1839);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa42f9f0() {
   return (neuron0xa42f020()*-2.39427);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa37d498() {
   return (neuron0xa42f388()*-8.27484);
}

double MuGirlNS::AnnResults_forward_lt5::synapse0xa37d648() {
   return (neuron0xa42f718()*0.878042);
}

