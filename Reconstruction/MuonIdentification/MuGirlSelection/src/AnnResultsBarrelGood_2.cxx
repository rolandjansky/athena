/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResultsBarrelGood_2.h"
#include <cmath>

double MuGirlNS::AnnResultsBarrelGood_2::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) 
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
         return neuron0xa3f6318();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResultsBarrelGood_2::neuron0xa3f42c0() 
{
   return m_input0;
}

double MuGirlNS::AnnResultsBarrelGood_2::neuron0xa3f4450() 
{
   return m_input1;
}

double MuGirlNS::AnnResultsBarrelGood_2::neuron0xa3f4628() 
{
   return m_input2;
}

double MuGirlNS::AnnResultsBarrelGood_2::neuron0xa3f4820() 
{
   return m_input3;
}

double MuGirlNS::AnnResultsBarrelGood_2::neuron0xa3f4a18() 
{
   return m_input4;
}

double MuGirlNS::AnnResultsBarrelGood_2::neuron0xa3f4c10() 
{
   return m_input5;
}

double MuGirlNS::AnnResultsBarrelGood_2::neuron0xa3f4e08() 
{
   return m_input6;
}

double MuGirlNS::AnnResultsBarrelGood_2::neuron0xa3f5000() 
{
   return m_input7;
}

double MuGirlNS::AnnResultsBarrelGood_2::neuron0xa3f51f8() 
{
   return m_input8;
}

double MuGirlNS::AnnResultsBarrelGood_2::neuron0xa3f53f0() 
{
   return m_input9;
}

double MuGirlNS::AnnResultsBarrelGood_2::input0xa3f56e8() 
{
   double input = -0.603935;
   input += synapse0xa3d2688();
   input += synapse0xa27a888();
   input += synapse0xa3f5878();
   input += synapse0xa3f58a0();
   input += synapse0xa3f58c8();
   input += synapse0xa3f58f0();
   input += synapse0xa3f5918();
   input += synapse0xa3f5940();
   input += synapse0xa3f5968();
   input += synapse0xa3f5990();
   return input;
}

double MuGirlNS::AnnResultsBarrelGood_2::neuron0xa3f56e8() 
{
   double input = input0xa3f56e8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsBarrelGood_2::input0xa3f59b8() 
{
   double input = 1.98895;
   input += synapse0xa3f5b90();
   input += synapse0xa3f5bb8();
   input += synapse0xa3f5be0();
   input += synapse0xa3f5c08();
   input += synapse0xa3f5c30();
   input += synapse0xa3f5c58();
   input += synapse0xa3f5c80();
   input += synapse0xa3f5d30();
   input += synapse0xa3f5d58();
   input += synapse0xa3f5d80();
   return input;
}

double MuGirlNS::AnnResultsBarrelGood_2::neuron0xa3f59b8() 
{
   double input = input0xa3f59b8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsBarrelGood_2::input0xa3f5da8() 
{
   double input = 0.817483;
   input += synapse0xa3f5f38();
   input += synapse0xa3f5f60();
   input += synapse0xa3f5f88();
   input += synapse0xa3f5fb0();
   input += synapse0xa3f5fd8();
   input += synapse0xa3f6000();
   input += synapse0xa3f6028();
   input += synapse0xa3f6050();
   input += synapse0xa3f6078();
   input += synapse0xa3f60a0();
   return input;
}

double MuGirlNS::AnnResultsBarrelGood_2::neuron0xa3f5da8() 
{
   double input = input0xa3f5da8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsBarrelGood_2::input0xa3f60c8() 
{
   double input = -1.03975;
   input += synapse0xa3f62a0();
   input += synapse0xa3f62c8();
   input += synapse0xa3f62f0();
   input += synapse0xa27a648();
   input += synapse0xa27a670();
   input += synapse0xa27a698();
   input += synapse0xa3f5ca8();
   input += synapse0xa3f5cd0();
   input += synapse0xa3f5cf8();
   input += synapse0xa3f6420();
   return input;
}

double MuGirlNS::AnnResultsBarrelGood_2::neuron0xa3f60c8() 
{
   double input = input0xa3f60c8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsBarrelGood_2::input0xa3f6448() 
{
   double input = 0.529599;
   input += synapse0xa3f6620();
   input += synapse0xa3f6648();
   input += synapse0xa3f6670();
   input += synapse0xa3f6698();
   input += synapse0xa3f66c0();
   input += synapse0xa3f66e8();
   input += synapse0xa3f6710();
   input += synapse0xa3f6738();
   input += synapse0xa3f6760();
   input += synapse0xa3f6788();
   return input;
}

double MuGirlNS::AnnResultsBarrelGood_2::neuron0xa3f6448() 
{
   double input = input0xa3f6448();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsBarrelGood_2::input0xa3f67b0() 
{
   double input = 0.125329;
   input += synapse0xa3f6988();
   input += synapse0xa3f69b0();
   input += synapse0xa3f69d8();
   input += synapse0xa3f6a00();
   input += synapse0xa3f6a28();
   input += synapse0xa3f6a50();
   input += synapse0xa3f6a78();
   input += synapse0xa3f6aa0();
   input += synapse0xa3f6ac8();
   input += synapse0xa3f6af0();
   return input;
}

double MuGirlNS::AnnResultsBarrelGood_2::neuron0xa3f67b0() 
{
   double input = input0xa3f67b0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsBarrelGood_2::input0xa3f6b40() 
{
   double input = -2.3166;
   input += synapse0xa3f6b18();
   input += synapse0xa3f6da0();
   input += synapse0xa3f6dc8();
   input += synapse0xa3f6df0();
   input += synapse0xa3f6e18();
   input += synapse0xa3d64a8();
   return input;
}

double MuGirlNS::AnnResultsBarrelGood_2::neuron0xa3f6b40() 
{
   double input = input0xa3f6b40();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsBarrelGood_2::input0xa3f6318() 
{
   double input = -0.0334988;
   input += synapse0xa27aba8();
   return input;
}

double MuGirlNS::AnnResultsBarrelGood_2::neuron0xa3f6318() 
{
   double input = input0xa3f6318();
   return (input * 1)+0;
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3d2688() 
{
   return (neuron0xa3f42c0()*-0.0147152);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa27a888() 
{
   return (neuron0xa3f4450()*-0.0029026);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5878() 
{
   return (neuron0xa3f4628()*-0.0216581);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f58a0() 
{
   return (neuron0xa3f4820()*0.0513135);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f58c8() 
{
   return (neuron0xa3f4a18()*-0.223546);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f58f0() 
{
   return (neuron0xa3f4c10()*0.0301963);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5918() 
{
   return (neuron0xa3f4e08()*0.00916522);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5940() 
{
   return (neuron0xa3f5000()*-1.73125);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5968() 
{
   return (neuron0xa3f51f8()*-0.00461116);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5990() 
{
   return (neuron0xa3f53f0()*12.2754);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5b90() 
{
   return (neuron0xa3f42c0()*-0.708153);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5bb8() 
{
   return (neuron0xa3f4450()*0.0865497);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5be0() 
{
   return (neuron0xa3f4628()*-0.21998);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5c08() 
{
   return (neuron0xa3f4820()*-2.55075);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5c30() 
{
   return (neuron0xa3f4a18()*2.28659);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5c58() 
{
   return (neuron0xa3f4c10()*0.702587);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5c80() 
{
   return (neuron0xa3f4e08()*0.102327);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5d30() 
{
   return (neuron0xa3f5000()*0.190261);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5d58() 
{
   return (neuron0xa3f51f8()*-1.44674);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5d80() 
{
   return (neuron0xa3f53f0()*-0.0120912);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5f38() 
{
   return (neuron0xa3f42c0()*0.0103638);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5f60() 
{
   return (neuron0xa3f4450()*-0.409068);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5f88() 
{
   return (neuron0xa3f4628()*-0.198007);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5fb0() 
{
   return (neuron0xa3f4820()*-0.0773921);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5fd8() 
{
   return (neuron0xa3f4a18()*-2.20577);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6000() 
{
   return (neuron0xa3f4c10()*0.188826);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6028() 
{
   return (neuron0xa3f4e08()*0.924827);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6050() 
{
   return (neuron0xa3f5000()*-2.9558);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6078() 
{
   return (neuron0xa3f51f8()*-0.00867474);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f60a0() 
{
   return (neuron0xa3f53f0()*-1.75668);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f62a0() 
{
   return (neuron0xa3f42c0()*0.0233583);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f62c8() 
{
   return (neuron0xa3f4450()*-0.158637);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f62f0() 
{
   return (neuron0xa3f4628()*-0.183158);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa27a648() 
{
   return (neuron0xa3f4820()*0.191393);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa27a670() 
{
   return (neuron0xa3f4a18()*0.294718);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa27a698() 
{
   return (neuron0xa3f4c10()*0.0575986);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5ca8() 
{
   return (neuron0xa3f4e08()*0.360165);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5cd0() 
{
   return (neuron0xa3f5000()*-2.70007);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f5cf8() 
{
   return (neuron0xa3f51f8()*-0.000856649);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6420() 
{
   return (neuron0xa3f53f0()*7.75836);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6620() 
{
   return (neuron0xa3f42c0()*0.937058);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6648() 
{
   return (neuron0xa3f4450()*0.488759);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6670() 
{
   return (neuron0xa3f4628()*0.14882);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6698() 
{
   return (neuron0xa3f4820()*0.122607);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f66c0() 
{
   return (neuron0xa3f4a18()*0.331085);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f66e8() 
{
   return (neuron0xa3f4c10()*0.495193);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6710() 
{
   return (neuron0xa3f4e08()*-0.130008);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6738() 
{
   return (neuron0xa3f5000()*0.302722);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6760() 
{
   return (neuron0xa3f51f8()*0.380862);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6788() 
{
   return (neuron0xa3f53f0()*-0.113001);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6988() 
{
   return (neuron0xa3f42c0()*-0.914668);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f69b0() 
{
   return (neuron0xa3f4450()*-0.0934048);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f69d8() 
{
   return (neuron0xa3f4628()*-0.391689);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6a00() 
{
   return (neuron0xa3f4820()*-0.583726);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6a28() 
{
   return (neuron0xa3f4a18()*0.095532);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6a50() 
{
   return (neuron0xa3f4c10()*-1.43522);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6a78() 
{
   return (neuron0xa3f4e08()*-0.517902);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6aa0() 
{
   return (neuron0xa3f5000()*-0.341342);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6ac8() 
{
   return (neuron0xa3f51f8()*-0.822738);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6af0() 
{
   return (neuron0xa3f53f0()*-0.326122);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6b18() 
{
   return (neuron0xa3f56e8()*11.3912);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6da0() 
{
   return (neuron0xa3f59b8()*-4.43006);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6dc8() 
{
   return (neuron0xa3f5da8()*-6.17703);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6df0() 
{
   return (neuron0xa3f60c8()*8.92182);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3f6e18() 
{
   return (neuron0xa3f6448()*-1.70477);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa3d64a8() 
{
   return (neuron0xa3f67b0()*-0.119718);
}

double MuGirlNS::AnnResultsBarrelGood_2::synapse0xa27aba8() 
{
   return (neuron0xa3f6b40()*1.02958);
}

