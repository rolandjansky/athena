/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResults_endcap_gt5.h"
#include <cmath>

double MuGirlNS::AnnResults_endcap_gt5::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) {
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
         return neuron0xa97e7d0();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_endcap_gt5::value(int index, double* input) {
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
         return neuron0xa97e7d0();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_endcap_gt5::neuron0xa97c700() {
   return m_input0;
}

double MuGirlNS::AnnResults_endcap_gt5::neuron0xa97c8b0() {
   return m_input1;
}

double MuGirlNS::AnnResults_endcap_gt5::neuron0xa97caa8() {
   return m_input2;
}

double MuGirlNS::AnnResults_endcap_gt5::neuron0xa97cca0() {
   return m_input3;
}

double MuGirlNS::AnnResults_endcap_gt5::neuron0xa97ce98() {
   return m_input4;
}

double MuGirlNS::AnnResults_endcap_gt5::neuron0xa97d090() {
   return m_input5;
}

double MuGirlNS::AnnResults_endcap_gt5::neuron0xa97d288() {
   return m_input6;
}

double MuGirlNS::AnnResults_endcap_gt5::neuron0xa97d480() {
   return m_input7;
}

double MuGirlNS::AnnResults_endcap_gt5::neuron0xa97d678() {
   return m_input8;
}

double MuGirlNS::AnnResults_endcap_gt5::neuron0xa97d870() {
   return m_input9;
}

double MuGirlNS::AnnResults_endcap_gt5::input0xa97db78() {
   double input = 1.01423;
   input += synapse0xa8bc218();
   input += synapse0xa97dd08();
   input += synapse0xa97dd30();
   input += synapse0xa97dd58();
   input += synapse0xa97dd80();
   input += synapse0xa97dda8();
   input += synapse0xa97ddd0();
   input += synapse0xa97ddf8();
   input += synapse0xa97de20();
   input += synapse0xa97de48();
   return input;
}

double MuGirlNS::AnnResults_endcap_gt5::neuron0xa97db78() {
   double input = input0xa97db78();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_endcap_gt5::input0xa97de70() {
   double input = -0.356425;
   input += synapse0xa97e048();
   input += synapse0xa97e070();
   input += synapse0xa97e098();
   input += synapse0xa97e0c0();
   input += synapse0xa97e0e8();
   input += synapse0xa97e110();
   input += synapse0xa97e138();
   input += synapse0xa97e1e8();
   input += synapse0xa97e210();
   input += synapse0xa97e238();
   return input;
}

double MuGirlNS::AnnResults_endcap_gt5::neuron0xa97de70() {
   double input = input0xa97de70();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_endcap_gt5::input0xa97e260() {
   double input = -0.554218;
   input += synapse0xa97e3f0();
   input += synapse0xa97e418();
   input += synapse0xa97e440();
   input += synapse0xa97e468();
   input += synapse0xa97e490();
   input += synapse0xa97e4b8();
   input += synapse0xa97e4e0();
   input += synapse0xa97e508();
   input += synapse0xa97e530();
   input += synapse0xa97e558();
   return input;
}

double MuGirlNS::AnnResults_endcap_gt5::neuron0xa97e260() {
   double input = input0xa97e260();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_endcap_gt5::input0xa97e580() {
   double input = -7.90082;
   input += synapse0xa97e758();
   input += synapse0xa97e780();
   input += synapse0xa97e7a8();
   input += synapse0xa8889c0();
   input += synapse0xa8889e8();
   input += synapse0xa888a10();
   input += synapse0xa97e160();
   input += synapse0xa97e188();
   input += synapse0xa97e1b0();
   input += synapse0xa97e8d8();
   return input;
}

double MuGirlNS::AnnResults_endcap_gt5::neuron0xa97e580() {
   double input = input0xa97e580();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_endcap_gt5::input0xa97e900() {
   double input = -1.39622;
   input += synapse0xa97ead8();
   input += synapse0xa97eb00();
   input += synapse0xa97eb28();
   input += synapse0xa97eb50();
   input += synapse0xa97eb78();
   input += synapse0xa97eba0();
   input += synapse0xa97ebc8();
   input += synapse0xa97ebf0();
   input += synapse0xa97ec18();
   input += synapse0xa97ec40();
   return input;
}

double MuGirlNS::AnnResults_endcap_gt5::neuron0xa97e900() {
   double input = input0xa97e900();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_endcap_gt5::input0xa97ec68() {
   double input = 1.21362;
   input += synapse0xa97ee40();
   input += synapse0xa97ee68();
   input += synapse0xa97ee90();
   input += synapse0xa97eeb8();
   input += synapse0xa97eee0();
   input += synapse0xa97ef08();
   input += synapse0xa97ef30();
   input += synapse0xa97ef58();
   input += synapse0xa97ef80();
   input += synapse0xa97efa8();
   return input;
}

double MuGirlNS::AnnResults_endcap_gt5::neuron0xa97ec68() {
   double input = input0xa97ec68();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_endcap_gt5::input0xa97eff8() {
   double input = -5.87227;
   input += synapse0xa97efd0();
   input += synapse0xa97f258();
   input += synapse0xa97f280();
   input += synapse0xa97f2a8();
   input += synapse0xa97f2d0();
   input += synapse0xa8bc240();
   return input;
}

double MuGirlNS::AnnResults_endcap_gt5::neuron0xa97eff8() {
   double input = input0xa97eff8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_endcap_gt5::input0xa97e7d0() {
   double input = -0.00159594;
   input += synapse0xa888ee8();
   return input;
}

double MuGirlNS::AnnResults_endcap_gt5::neuron0xa97e7d0() {
   double input = input0xa97e7d0();
   return (input * 1)+0;
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa8bc218() {
   return (neuron0xa97c700()*0.0448529);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97dd08() {
   return (neuron0xa97c8b0()*0.0734371);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97dd30() {
   return (neuron0xa97caa8()*0.0196207);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97dd58() {
   return (neuron0xa97cca0()*0.128492);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97dd80() {
   return (neuron0xa97ce98()*-0.974558);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97dda8() {
   return (neuron0xa97d090()*-0.093788);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97ddd0() {
   return (neuron0xa97d288()*-0.117792);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97ddf8() {
   return (neuron0xa97d480()*0.273772);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97de20() {
   return (neuron0xa97d678()*0.0154291);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97de48() {
   return (neuron0xa97d870()*-6.33643);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e048() {
   return (neuron0xa97c700()*-2.21907);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e070() {
   return (neuron0xa97c8b0()*-0.921307);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e098() {
   return (neuron0xa97caa8()*-0.954765);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e0c0() {
   return (neuron0xa97cca0()*-0.267974);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e0e8() {
   return (neuron0xa97ce98()*0.187287);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e110() {
   return (neuron0xa97d090()*-1.82509);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e138() {
   return (neuron0xa97d288()*-1.20898);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e1e8() {
   return (neuron0xa97d480()*0.39846);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e210() {
   return (neuron0xa97d678()*-0.644851);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e238() {
   return (neuron0xa97d870()*-0.478644);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e3f0() {
   return (neuron0xa97c700()*-0.397969);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e418() {
   return (neuron0xa97c8b0()*-0.15273);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e440() {
   return (neuron0xa97caa8()*-0.632224);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e468() {
   return (neuron0xa97cca0()*-0.528778);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e490() {
   return (neuron0xa97ce98()*-0.288916);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e4b8() {
   return (neuron0xa97d090()*-0.848024);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e4e0() {
   return (neuron0xa97d288()*-0.105509);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e508() {
   return (neuron0xa97d480()*0.213153);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e530() {
   return (neuron0xa97d678()*-0.369972);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e558() {
   return (neuron0xa97d870()*0.0374842);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e758() {
   return (neuron0xa97c700()*0.195427);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e780() {
   return (neuron0xa97c8b0()*0.904536);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e7a8() {
   return (neuron0xa97caa8()*-0.583958);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa8889c0() {
   return (neuron0xa97cca0()*2.64569);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa8889e8() {
   return (neuron0xa97ce98()*-3.74288);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa888a10() {
   return (neuron0xa97d090()*-0.015219);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e160() {
   return (neuron0xa97d288()*0.607817);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e188() {
   return (neuron0xa97d480()*2.51233);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e1b0() {
   return (neuron0xa97d678()*-0.00301315);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97e8d8() {
   return (neuron0xa97d870()*2.26701);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97ead8() {
   return (neuron0xa97c700()*2.64798);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97eb00() {
   return (neuron0xa97c8b0()*-0.0714551);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97eb28() {
   return (neuron0xa97caa8()*-1.06036);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97eb50() {
   return (neuron0xa97cca0()*1.18848);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97eb78() {
   return (neuron0xa97ce98()*-0.685287);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97eba0() {
   return (neuron0xa97d090()*-2.6606);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97ebc8() {
   return (neuron0xa97d288()*0.741311);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97ebf0() {
   return (neuron0xa97d480()*-0.294646);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97ec18() {
   return (neuron0xa97d678()*0.396433);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97ec40() {
   return (neuron0xa97d870()*0.150299);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97ee40() {
   return (neuron0xa97c700()*0.423918);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97ee68() {
   return (neuron0xa97c8b0()*-0.283634);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97ee90() {
   return (neuron0xa97caa8()*-0.0213517);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97eeb8() {
   return (neuron0xa97cca0()*-1.87098);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97eee0() {
   return (neuron0xa97ce98()*1.99982);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97ef08() {
   return (neuron0xa97d090()*0.00496505);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97ef30() {
   return (neuron0xa97d288()*-0.281833);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97ef58() {
   return (neuron0xa97d480()*-0.206447);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97ef80() {
   return (neuron0xa97d678()*11.8459);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97efa8() {
   return (neuron0xa97d870()*9.40452);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97efd0() {
   return (neuron0xa97db78()*-7.6566);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97f258() {
   return (neuron0xa97de70()*-0.103521);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97f280() {
   return (neuron0xa97e260()*0.678033);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97f2a8() {
   return (neuron0xa97e580()*7.55255);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa97f2d0() {
   return (neuron0xa97e900()*-6.12325);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa8bc240() {
   return (neuron0xa97ec68()*8.39086);
}

double MuGirlNS::AnnResults_endcap_gt5::synapse0xa888ee8() {
   return (neuron0xa97eff8()*1.01685);
}

