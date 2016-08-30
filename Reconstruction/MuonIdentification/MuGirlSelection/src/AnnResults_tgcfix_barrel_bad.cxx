/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResults_tgcfix_barrel_bad.h"
#include <cmath>

double MuGirlNS::AnnResults_tgcfix_barrel_bad::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) {
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
         return neuron0xaf11be0();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::neuron0xaf0fb68() {
   return m_input0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::neuron0xaf0fcf8() {
   return m_input1;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::neuron0xaf0fef0() {
   return m_input2;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::neuron0xaf100e8() {
   return m_input3;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::neuron0xaf102e0() {
   return m_input4;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::neuron0xaf104d8() {
   return m_input5;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::neuron0xaf106d0() {
   return m_input6;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::neuron0xaf108c8() {
   return m_input7;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::neuron0xaf10ac0() {
   return m_input8;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::neuron0xaf10cb8() {
   return m_input9;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::input0xaf10fb0() {
   double input = -0.38538;
   input += synapse0xaeede30();
   input += synapse0xad85d98();
   input += synapse0xaf11140();
   input += synapse0xaf11168();
   input += synapse0xaf11190();
   input += synapse0xaf111b8();
   input += synapse0xaf111e0();
   input += synapse0xaf11208();
   input += synapse0xaf11230();
   input += synapse0xaf11258();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::neuron0xaf10fb0() {
   double input = input0xaf10fb0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::input0xaf11280() {
   double input = -0.658157;
   input += synapse0xaf11458();
   input += synapse0xaf11480();
   input += synapse0xaf114a8();
   input += synapse0xaf114d0();
   input += synapse0xaf114f8();
   input += synapse0xaf11520();
   input += synapse0xaf11548();
   input += synapse0xaf115f8();
   input += synapse0xaf11620();
   input += synapse0xaf11648();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::neuron0xaf11280() {
   double input = input0xaf11280();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::input0xaf11670() {
   double input = 0.997877;
   input += synapse0xaf11800();
   input += synapse0xaf11828();
   input += synapse0xaf11850();
   input += synapse0xaf11878();
   input += synapse0xaf118a0();
   input += synapse0xaf118c8();
   input += synapse0xaf118f0();
   input += synapse0xaf11918();
   input += synapse0xaf11940();
   input += synapse0xaf11968();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::neuron0xaf11670() {
   double input = input0xaf11670();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::input0xaf11990() {
   double input = 1.13203;
   input += synapse0xaf11b68();
   input += synapse0xaf11b90();
   input += synapse0xaf11bb8();
   input += synapse0xad85b70();
   input += synapse0xad85b98();
   input += synapse0xad85bc0();
   input += synapse0xaf11570();
   input += synapse0xaf11598();
   input += synapse0xaf115c0();
   input += synapse0xaf11ce8();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::neuron0xaf11990() {
   double input = input0xaf11990();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::input0xaf11d10() {
   double input = 2.47033;
   input += synapse0xaf11ee8();
   input += synapse0xaf11f10();
   input += synapse0xaf11f38();
   input += synapse0xaf11f60();
   input += synapse0xaf11f88();
   input += synapse0xaf11fb0();
   input += synapse0xaf11fd8();
   input += synapse0xaf12000();
   input += synapse0xaf12028();
   input += synapse0xaf12050();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::neuron0xaf11d10() {
   double input = input0xaf11d10();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::input0xaf12078() {
   double input = -0.275141;
   input += synapse0xaf12250();
   input += synapse0xaf12278();
   input += synapse0xaf122a0();
   input += synapse0xaf122c8();
   input += synapse0xaf122f0();
   input += synapse0xaf12318();
   input += synapse0xaf12340();
   input += synapse0xaf12368();
   input += synapse0xaf12390();
   input += synapse0xaf123b8();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::neuron0xaf12078() {
   double input = input0xaf12078();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::input0xaf12408() {
   double input = 0.72516;
   input += synapse0xaf123e0();
   input += synapse0xaf12668();
   input += synapse0xaf12690();
   input += synapse0xaf126b8();
   input += synapse0xaf126e0();
   input += synapse0xaef1c70();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::neuron0xaf12408() {
   double input = input0xaf12408();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::input0xaf11be0() {
   double input = -3.39027;
   input += synapse0xad860b8();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::neuron0xaf11be0() {
   double input = input0xaf11be0();
   return (input * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaeede30() {
   return (neuron0xaf0fb68()*0.754723);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xad85d98() {
   return (neuron0xaf0fcf8()*0.582516);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11140() {
   return (neuron0xaf0fef0()*0.505405);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11168() {
   return (neuron0xaf100e8()*-0.321662);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11190() {
   return (neuron0xaf102e0()*0.0484778);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf111b8() {
   return (neuron0xaf104d8()*0.369492);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf111e0() {
   return (neuron0xaf106d0()*0.360939);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11208() {
   return (neuron0xaf108c8()*-0.201706);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11230() {
   return (neuron0xaf10ac0()*0.31912);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11258() {
   return (neuron0xaf10cb8()*0.116572);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11458() {
   return (neuron0xaf0fb68()*0.816908);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11480() {
   return (neuron0xaf0fcf8()*0.61098);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf114a8() {
   return (neuron0xaf0fef0()*0.512638);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf114d0() {
   return (neuron0xaf100e8()*0.436086);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf114f8() {
   return (neuron0xaf102e0()*-0.138833);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11520() {
   return (neuron0xaf104d8()*0.312113);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11548() {
   return (neuron0xaf106d0()*-0.13645);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf115f8() {
   return (neuron0xaf108c8()*0.106095);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11620() {
   return (neuron0xaf10ac0()*0.368284);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11648() {
   return (neuron0xaf10cb8()*-0.632387);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11800() {
   return (neuron0xaf0fb68()*0.044293);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11828() {
   return (neuron0xaf0fcf8()*-0.376425);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11850() {
   return (neuron0xaf0fef0()*0.981431);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11878() {
   return (neuron0xaf100e8()*0.63743);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf118a0() {
   return (neuron0xaf102e0()*-1.80921);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf118c8() {
   return (neuron0xaf104d8()*0.0912611);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf118f0() {
   return (neuron0xaf106d0()*0.216799);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11918() {
   return (neuron0xaf108c8()*-0.257658);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11940() {
   return (neuron0xaf10ac0()*0.856699);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11968() {
   return (neuron0xaf10cb8()*3.09521);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11b68() {
   return (neuron0xaf0fb68()*0.0700643);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11b90() {
   return (neuron0xaf0fcf8()*-0.261057);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11bb8() {
   return (neuron0xaf0fef0()*0.850544);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xad85b70() {
   return (neuron0xaf100e8()*0.223645);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xad85b98() {
   return (neuron0xaf102e0()*-0.75929);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xad85bc0() {
   return (neuron0xaf104d8()*0.0268005);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11570() {
   return (neuron0xaf106d0()*-0.0827455);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11598() {
   return (neuron0xaf108c8()*0.627142);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf115c0() {
   return (neuron0xaf10ac0()*0.451326);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11ce8() {
   return (neuron0xaf10cb8()*2.34862);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11ee8() {
   return (neuron0xaf0fb68()*-0.0112535);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11f10() {
   return (neuron0xaf0fcf8()*-0.167907);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11f38() {
   return (neuron0xaf0fef0()*0.294044);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11f60() {
   return (neuron0xaf100e8()*-1.14109);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11f88() {
   return (neuron0xaf102e0()*-0.849866);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11fb0() {
   return (neuron0xaf104d8()*-0.0199248);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf11fd8() {
   return (neuron0xaf106d0()*-0.0607811);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf12000() {
   return (neuron0xaf108c8()*2.87119);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf12028() {
   return (neuron0xaf10ac0()*0.000964336);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf12050() {
   return (neuron0xaf10cb8()*-10.39);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf12250() {
   return (neuron0xaf0fb68()*-0.517622);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf12278() {
   return (neuron0xaf0fcf8()*0.490577);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf122a0() {
   return (neuron0xaf0fef0()*0.750321);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf122c8() {
   return (neuron0xaf100e8()*0.37832);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf122f0() {
   return (neuron0xaf102e0()*0.325436);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf12318() {
   return (neuron0xaf104d8()*-2.08914);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf12340() {
   return (neuron0xaf106d0()*0.292317);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf12368() {
   return (neuron0xaf108c8()*0.191475);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf12390() {
   return (neuron0xaf10ac0()*0.525257);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf123b8() {
   return (neuron0xaf10cb8()*-0.122452);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf123e0() {
   return (neuron0xaf10fb0()*-0.0773631);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf12668() {
   return (neuron0xaf11280()*-0.564671);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf12690() {
   return (neuron0xaf11670()*-1.92999);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf126b8() {
   return (neuron0xaf11990()*2.67479);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaf126e0() {
   return (neuron0xaf11d10()*-0.684776);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xaef1c70() {
   return (neuron0xaf12078()*-0.0367436);
}

double MuGirlNS::AnnResults_tgcfix_barrel_bad::synapse0xad860b8() {
   return (neuron0xaf12408()*6.31873);
}

