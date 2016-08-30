/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResults_tgcfix_forward_nocsc.h"
#include <cmath>

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9)
{
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
   switch(index) 
   {
     case 0:
         return neuron0x99e7f90();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::neuron0x99e5ef8()
{
   return m_input0;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::neuron0x99e60a8() 
{
   return m_input1;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::neuron0x99e62a0() 
{
   return m_input2;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::neuron0x99e6498() 
{
   return m_input3;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::neuron0x99e6690() 
{
   return m_input4;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::neuron0x99e6888() 
{
   return m_input5;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::neuron0x99e6a80() 
{
   return m_input6;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::neuron0x99e6c78() 
{
   return m_input7;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::neuron0x99e6e70() 
{
   return m_input8;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::neuron0x99e7068() 
{
   return m_input9;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::input0x99e7360()
{
   double input = 0.25696;
   input += synapse0x99c41e0();
   input += synapse0x98a9bb0();
   input += synapse0x99e74f0();
   input += synapse0x99e7518();
   input += synapse0x99e7540();
   input += synapse0x99e7568();
   input += synapse0x99e7590();
   input += synapse0x99e75b8();
   input += synapse0x99e75e0();
   input += synapse0x99e7608();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::neuron0x99e7360() 
{
   double input = input0x99e7360();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::input0x99e7630() 
{
   double input = 0.140576;
   input += synapse0x99e7808();
   input += synapse0x99e7830();
   input += synapse0x99e7858();
   input += synapse0x99e7880();
   input += synapse0x99e78a8();
   input += synapse0x99e78d0();
   input += synapse0x99e78f8();
   input += synapse0x99e79a8();
   input += synapse0x99e79d0();
   input += synapse0x99e79f8();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::neuron0x99e7630() 
{
   double input = input0x99e7630();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::input0x99e7a20() 
{
   double input = -2.78607;
   input += synapse0x99e7bb0();
   input += synapse0x99e7bd8();
   input += synapse0x99e7c00();
   input += synapse0x99e7c28();
   input += synapse0x99e7c50();
   input += synapse0x99e7c78();
   input += synapse0x99e7ca0();
   input += synapse0x99e7cc8();
   input += synapse0x99e7cf0();
   input += synapse0x99e7d18();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::neuron0x99e7a20() 
{
   double input = input0x99e7a20();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::input0x99e7d40() 
{
   double input = -0.0219565;
   input += synapse0x99e7f18();
   input += synapse0x99e7f40();
   input += synapse0x99e7f68();
   input += synapse0x98a9970();
   input += synapse0x98a9998();
   input += synapse0x98a99c0();
   input += synapse0x99e7920();
   input += synapse0x99e7948();
   input += synapse0x99e7970();
   input += synapse0x99e8098();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::neuron0x99e7d40() 
{
   double input = input0x99e7d40();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::input0x99e80c0() 
{
   double input = -0.264097;
   input += synapse0x99e8298();
   input += synapse0x99e82c0();
   input += synapse0x99e82e8();
   input += synapse0x99e8310();
   input += synapse0x99e8338();
   input += synapse0x99e8360();
   input += synapse0x99e8388();
   input += synapse0x99e83b0();
   input += synapse0x99e83d8();
   input += synapse0x99e8400();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::neuron0x99e80c0() 
{
   double input = input0x99e80c0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::input0x99e8428() 
{
   double input = -0.195425;
   input += synapse0x99e8600();
   input += synapse0x99e8628();
   input += synapse0x99e8650();
   input += synapse0x99e8678();
   input += synapse0x99e86a0();
   input += synapse0x99e86c8();
   input += synapse0x99e86f0();
   input += synapse0x99e8718();
   input += synapse0x99e8740();
   input += synapse0x99e8768();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::neuron0x99e8428() 
{
   double input = input0x99e8428();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::input0x99e87b8() 
{
   double input = -0.0541489;
   input += synapse0x99e8790();
   input += synapse0x99e8a18();
   input += synapse0x99e8a40();
   input += synapse0x99e8a68();
   input += synapse0x99e8a90();
   input += synapse0x98a9c88();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::neuron0x99e87b8() 
{
   double input = input0x99e87b8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::input0x99e7f90() 
{
   double input = 0.118965;
   input += synapse0x98a9ed0();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::neuron0x99e7f90() 
{
   double input = input0x99e7f90();
   return (input * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99c41e0() 
{
   return (neuron0x99e5ef8()*0.00225841);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x98a9bb0() 
{
   return (neuron0x99e60a8()*0.054097);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e74f0() 
{
   return (neuron0x99e62a0()*-0.024863);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7518() 
{
   return (neuron0x99e6498()*-0.155975);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7540() 
{
   return (neuron0x99e6690()*-0.295137);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7568() 
{
   return (neuron0x99e6888()*-0.0411212);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7590() 
{
   return (neuron0x99e6a80()*-0.0423215);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e75b8() 
{
   return (neuron0x99e6c78()*25.2973);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e75e0() 
{
   return (neuron0x99e6e70()*0.000908364);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7608() 
{
   return (neuron0x99e7068()*-4.84896);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7808() 
{
   return (neuron0x99e5ef8()*-2.19583);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7830() 
{
   return (neuron0x99e60a8()*-1.65557);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7858() 
{
   return (neuron0x99e62a0()*0.371453);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7880() 
{
   return (neuron0x99e6498()*-0.188952);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e78a8() 
{
   return (neuron0x99e6690()*-0.520275);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e78d0() 
{
   return (neuron0x99e6888()*-1.89413);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e78f8() 
{
   return (neuron0x99e6a80()*-2.64352);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e79a8() 
{
   return (neuron0x99e6c78()*0.330015);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e79d0() 
{
   return (neuron0x99e6e70()*-2.23261);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e79f8() 
{
   return (neuron0x99e7068()*-0.397456);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7bb0() 
{
   return (neuron0x99e5ef8()*-3.13703);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7bd8() 
{
   return (neuron0x99e60a8()*0.862217);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7c00() 
{
   return (neuron0x99e62a0()*0.393494);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7c28() 
{
   return (neuron0x99e6498()*1.28337);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7c50() 
{
   return (neuron0x99e6690()*0.0971999);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7c78() 
{
   return (neuron0x99e6888()*2.82521);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7ca0() 
{
   return (neuron0x99e6a80()*3.07098);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7cc8() 
{
   return (neuron0x99e6c78()*0.0751352);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7cf0() 
{
   return (neuron0x99e6e70()*4.71879);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7d18() 
{
   return (neuron0x99e7068()*1.20187);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7f18() 
{
   return (neuron0x99e5ef8()*-3.73725);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7f40() 
{
   return (neuron0x99e60a8()*-0.869523);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7f68() 
{
   return (neuron0x99e62a0()*-1.66366);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x98a9970() 
{
   return (neuron0x99e6498()*-0.417388);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x98a9998() 
{
   return (neuron0x99e6690()*-0.00106686);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x98a99c0() 
{
   return (neuron0x99e6888()*-1.1694);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7920() 
{
   return (neuron0x99e6a80()*-2.48256);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7948() 
{
   return (neuron0x99e6c78()*-0.0256726);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e7970() 
{
   return (neuron0x99e6e70()*-2.90673);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8098() 
{
   return (neuron0x99e7068()*0.13011);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8298() 
{
   return (neuron0x99e5ef8()*-5.95298);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e82c0() 
{
   return (neuron0x99e60a8()*-0.00290598);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e82e8() 
{
   return (neuron0x99e62a0()*-6.92969);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8310() 
{
   return (neuron0x99e6498()*-1.1274);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8338() 
{
   return (neuron0x99e6690()*0.261173);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8360() 
{
   return (neuron0x99e6888()*-6.08959);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8388() 
{
   return (neuron0x99e6a80()*-0.791445);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e83b0() 
{
   return (neuron0x99e6c78()*-0.2147);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e83d8() 
{
   return (neuron0x99e6e70()*-4.7613);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8400() 
{
   return (neuron0x99e7068()*-0.870701);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8600() 
{
   return (neuron0x99e5ef8()*2.3195);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8628() 
{
   return (neuron0x99e60a8()*0.838704);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8650() 
{
   return (neuron0x99e62a0()*-0.365648);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8678() 
{
   return (neuron0x99e6498()*-0.165144);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e86a0() 
{
   return (neuron0x99e6690()*0.366379);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e86c8() 
{
   return (neuron0x99e6888()*0.74918);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e86f0() 
{
   return (neuron0x99e6a80()*1.59055);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8718() 
{
   return (neuron0x99e6c78()*0.131064);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8740() 
{
   return (neuron0x99e6e70()*0.292716);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8768() 
{
   return (neuron0x99e7068()*0.474537);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8790() 
{
   return (neuron0x99e7360()*-21.2939);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8a18() 
{
   return (neuron0x99e7630()*-2.72472);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8a40() 
{
   return (neuron0x99e7a20()*9.50817);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8a68() 
{
   return (neuron0x99e7d40()*-0.365215);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x99e8a90() 
{
   return (neuron0x99e80c0()*2.17793);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x98a9c88() 
{
   return (neuron0x99e8428()*1.01666);
}

double MuGirlNS::AnnResults_tgcfix_forward_nocsc::synapse0x98a9ed0() 
{
   return (neuron0x99e87b8()*0.848696);
}

