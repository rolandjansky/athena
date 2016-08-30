/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResults_overlapreg_gt5.h"
#include <cmath>

double MuGirlNS::AnnResults_overlapreg_gt5::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) {
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
         return neuron0x9f5d870();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_overlapreg_gt5::value(int index, double* input) {
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
         return neuron0x9f5d870();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_overlapreg_gt5::neuron0x9f5b8a8() {
   return m_input0;
}

double MuGirlNS::AnnResults_overlapreg_gt5::neuron0x9f5ba38() {
   return m_input1;
}

double MuGirlNS::AnnResults_overlapreg_gt5::neuron0x9f5bc10() {
   return m_input2;
}

double MuGirlNS::AnnResults_overlapreg_gt5::neuron0x9f5bde8() {
   return m_input3;
}

double MuGirlNS::AnnResults_overlapreg_gt5::neuron0x9f5bfe0() {
   return m_input4;
}

double MuGirlNS::AnnResults_overlapreg_gt5::neuron0x9f5c1d8() {
   return m_input5;
}

double MuGirlNS::AnnResults_overlapreg_gt5::neuron0x9f5c3d0() {
   return m_input6;
}

double MuGirlNS::AnnResults_overlapreg_gt5::neuron0x9f5c5a8() {
   return m_input7;
}

double MuGirlNS::AnnResults_overlapreg_gt5::neuron0x9f5c7a0() {
   return m_input8;
}

double MuGirlNS::AnnResults_overlapreg_gt5::neuron0x9f5c998() {
   return m_input9;
}

double MuGirlNS::AnnResults_overlapreg_gt5::input0x9f5cca0() {
   double input = -9.49285;
   input += synapse0x9eaffc0();
   input += synapse0x9f5ce30();
   input += synapse0x9f5ce58();
   input += synapse0x9f5ce80();
   input += synapse0x9f5cea8();
   input += synapse0x9f5ced0();
   input += synapse0x9f5cef8();
   input += synapse0x9f5cf20();
   input += synapse0x9f5cf48();
   input += synapse0x9f5cf70();
   return input;
}

double MuGirlNS::AnnResults_overlapreg_gt5::neuron0x9f5cca0() {
   double input = input0x9f5cca0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_overlapreg_gt5::input0x9f5cf98() {
   double input = 1.93015;
   input += synapse0x9f5d170();
   input += synapse0x9f5d198();
   input += synapse0x9f5d1c0();
   input += synapse0x9f5d1e8();
   input += synapse0x9f5d210();
   input += synapse0x9f5d238();
   input += synapse0x9f5d260();
   input += synapse0x9f5d288();
   input += synapse0x9f5d2b0();
   input += synapse0x9f5d2d8();
   return input;
}

double MuGirlNS::AnnResults_overlapreg_gt5::neuron0x9f5cf98() {
   double input = input0x9f5cf98();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_overlapreg_gt5::input0x9f5d300() {
   double input = 13.0617;
   input += synapse0x9f5d490();
   input += synapse0x9f5d4b8();
   input += synapse0x9f5d4e0();
   input += synapse0x9f5d508();
   input += synapse0x9f5d530();
   input += synapse0x9f5d558();
   input += synapse0x9f5d580();
   input += synapse0x9f5d5a8();
   input += synapse0x9f5d5d0();
   input += synapse0x9f5d5f8();
   return input;
}

double MuGirlNS::AnnResults_overlapreg_gt5::neuron0x9f5d300() {
   double input = input0x9f5d300();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_overlapreg_gt5::input0x9f5d620() {
   double input = 3.04471;
   input += synapse0x9f5d7f8();
   input += synapse0x9f5d820();
   input += synapse0x9f5d848();
   input += synapse0x9eafd80();
   input += synapse0x9eafda8();
   input += synapse0x9eafdd0();
   input += synapse0x9f5d978();
   input += synapse0x9f5d9a0();
   input += synapse0x9f5d9c8();
   input += synapse0x9f5d9f0();
   return input;
}

double MuGirlNS::AnnResults_overlapreg_gt5::neuron0x9f5d620() {
   double input = input0x9f5d620();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_overlapreg_gt5::input0x9f5da18() {
   double input = 0.657825;
   input += synapse0x9f5dbf0();
   input += synapse0x9f5dc18();
   input += synapse0x9f5dc40();
   input += synapse0x9f5dc68();
   input += synapse0x9f5dc90();
   input += synapse0x9f5dcb8();
   input += synapse0x9f5dce0();
   input += synapse0x9f5dd08();
   input += synapse0x9f5dd30();
   input += synapse0x9f5dd58();
   return input;
}

double MuGirlNS::AnnResults_overlapreg_gt5::neuron0x9f5da18() {
   double input = input0x9f5da18();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_overlapreg_gt5::input0x9f5dd80() {
   double input = -10.4404;
   input += synapse0x9f5df58();
   input += synapse0x9f5df80();
   input += synapse0x9f5dfa8();
   input += synapse0x9f5dfd0();
   input += synapse0x9f5dff8();
   input += synapse0x9f5e020();
   input += synapse0x9f5e048();
   input += synapse0x9f5e070();
   input += synapse0x9f5e098();
   input += synapse0x9f5e0c0();
   return input;
}

double MuGirlNS::AnnResults_overlapreg_gt5::neuron0x9f5dd80() {
   double input = input0x9f5dd80();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_overlapreg_gt5::input0x9f5e110() {
   double input = 1.9609;
   input += synapse0x9f5e0e8();
   input += synapse0x9f5e370();
   input += synapse0x9f5e398();
   input += synapse0x9f5e3c0();
   input += synapse0x9f5e3e8();
   input += synapse0x9eaff90();
   return input;
}

double MuGirlNS::AnnResults_overlapreg_gt5::neuron0x9f5e110() {
   double input = input0x9f5e110();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_overlapreg_gt5::input0x9f5d870() {
   double input = 0.986122;
   input += synapse0x9f5cbb8();
   return input;
}

double MuGirlNS::AnnResults_overlapreg_gt5::neuron0x9f5d870() {
   double input = input0x9f5d870();
   return (input * 1)+0;
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9eaffc0() {
   return (neuron0x9f5b8a8()*0.0330172);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5ce30() {
   return (neuron0x9f5ba38()*-0.143372);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5ce58() {
   return (neuron0x9f5bc10()*0.0313588);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5ce80() {
   return (neuron0x9f5bde8()*0.165777);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5cea8() {
   return (neuron0x9f5bfe0()*8.82302);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5ced0() {
   return (neuron0x9f5c1d8()*-0.0594471);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5cef8() {
   return (neuron0x9f5c3d0()*0.0158838);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5cf20() {
   return (neuron0x9f5c5a8()*-7.78588);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5cf48() {
   return (neuron0x9f5c7a0()*0.00139039);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5cf70() {
   return (neuron0x9f5c998()*10.2723);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d170() {
   return (neuron0x9f5b8a8()*-1.8811);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d198() {
   return (neuron0x9f5ba38()*2.73609);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d1c0() {
   return (neuron0x9f5bc10()*2.69321);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d1e8() {
   return (neuron0x9f5bde8()*0.357609);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d210() {
   return (neuron0x9f5bfe0()*1.67701);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d238() {
   return (neuron0x9f5c1d8()*-2.13632);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d260() {
   return (neuron0x9f5c3d0()*2.90766);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d288() {
   return (neuron0x9f5c5a8()*-0.0649781);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d2b0() {
   return (neuron0x9f5c7a0()*-2.22339);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d2d8() {
   return (neuron0x9f5c998()*-1.73718);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d490() {
   return (neuron0x9f5b8a8()*8.51709);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d4b8() {
   return (neuron0x9f5ba38()*-3.02264);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d4e0() {
   return (neuron0x9f5bc10()*-0.881901);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d508() {
   return (neuron0x9f5bde8()*-7.38126);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d530() {
   return (neuron0x9f5bfe0()*-0.780459);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d558() {
   return (neuron0x9f5c1d8()*-9.02281);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d580() {
   return (neuron0x9f5c3d0()*-0.00177816);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d5a8() {
   return (neuron0x9f5c5a8()*1.59912);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d5d0() {
   return (neuron0x9f5c7a0()*1.10792);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d5f8() {
   return (neuron0x9f5c998()*-4.55596);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d7f8() {
   return (neuron0x9f5b8a8()*-2.59229);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d820() {
   return (neuron0x9f5ba38()*-1.16675);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d848() {
   return (neuron0x9f5bc10()*1.99058);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9eafd80() {
   return (neuron0x9f5bde8()*1.16879);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9eafda8() {
   return (neuron0x9f5bfe0()*-0.322282);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9eafdd0() {
   return (neuron0x9f5c1d8()*-1.74469);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d978() {
   return (neuron0x9f5c3d0()*-4.66503);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d9a0() {
   return (neuron0x9f5c5a8()*0.426262);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d9c8() {
   return (neuron0x9f5c7a0()*0.640504);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5d9f0() {
   return (neuron0x9f5c998()*-0.349729);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5dbf0() {
   return (neuron0x9f5b8a8()*-0.148914);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5dc18() {
   return (neuron0x9f5ba38()*0.599463);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5dc40() {
   return (neuron0x9f5bc10()*-0.279866);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5dc68() {
   return (neuron0x9f5bde8()*0.622459);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5dc90() {
   return (neuron0x9f5bfe0()*-0.744749);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5dcb8() {
   return (neuron0x9f5c1d8()*0.0741598);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5dce0() {
   return (neuron0x9f5c3d0()*-2.0665);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5dd08() {
   return (neuron0x9f5c5a8()*0.464);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5dd30() {
   return (neuron0x9f5c7a0()*-0.770133);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5dd58() {
   return (neuron0x9f5c998()*-0.579591);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5df58() {
   return (neuron0x9f5b8a8()*-3.66671);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5df80() {
   return (neuron0x9f5ba38()*-6.03007);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5dfa8() {
   return (neuron0x9f5bc10()*6.10797);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5dfd0() {
   return (neuron0x9f5bde8()*12.9387);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5dff8() {
   return (neuron0x9f5bfe0()*-3.80841);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5e020() {
   return (neuron0x9f5c1d8()*5.02474);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5e048() {
   return (neuron0x9f5c3d0()*-0.187807);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5e070() {
   return (neuron0x9f5c5a8()*-2.48222);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5e098() {
   return (neuron0x9f5c7a0()*-0.0463522);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5e0c0() {
   return (neuron0x9f5c998()*5.90506);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5e0e8() {
   return (neuron0x9f5cca0()*-7.48881);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5e370() {
   return (neuron0x9f5cf98()*3.94863);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5e398() {
   return (neuron0x9f5d300()*2.16808);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5e3c0() {
   return (neuron0x9f5d620()*1.96393);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5e3e8() {
   return (neuron0x9f5da18()*3.71986);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9eaff90() {
   return (neuron0x9f5dd80()*-2.68137);
}

double MuGirlNS::AnnResults_overlapreg_gt5::synapse0x9f5cbb8() {
   return (neuron0x9f5e110()*-0.985528);
}

