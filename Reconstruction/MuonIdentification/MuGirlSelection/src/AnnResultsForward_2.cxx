/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResultsForward_2.h"
#include <cmath>

double MuGirlNS::AnnResultsForward_2::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9,double in10) {
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
   m_input10 = (in10 - 0)/1;
   switch(index) {
     case 0:
         return neuron0x9dd5e18();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd3008() {
   return m_input0;
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd3150() {
   return m_input1;
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd3348() {
   return m_input2;
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd3540() {
   return m_input3;
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd3738() {
   return m_input4;
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd3930() {
   return m_input5;
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd3b28() {
   return m_input6;
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd3d20() {
   return m_input7;
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd3f18() {
   return m_input8;
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd4110() {
   return m_input9;
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd4308() {
   return m_input10;
}

double MuGirlNS::AnnResultsForward_2::input0x9dd4600() {
   double input = -4.05155;
   input += synapse0x9dd2f18();
   input += synapse0x9dd4790();
   input += synapse0x9dd47b8();
   input += synapse0x9dd47e0();
   input += synapse0x9dd4808();
   input += synapse0x9dd4830();
   input += synapse0x9dd4858();
   input += synapse0x9dd4880();
   input += synapse0x9dd48a8();
   input += synapse0x9dd48d0();
   input += synapse0x9dd48f8();
   return input;
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd4600() {
   double input = input0x9dd4600();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsForward_2::input0x9dd4920() {
   double input = -0.220061;
   input += synapse0x9dd4af8();
   input += synapse0x9dd4b20();
   input += synapse0x9dd4b48();
   input += synapse0x9dd4b70();
   input += synapse0x9dd4b98();
   input += synapse0x9dd4bc0();
   input += synapse0x9dd4c70();
   input += synapse0x9dd4c98();
   input += synapse0x9dd4cc0();
   input += synapse0x9dd4ce8();
   input += synapse0x9dd4d10();
   return input;
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd4920() {
   double input = input0x9dd4920();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsForward_2::input0x9dd4d38() {
   double input = -1.38026;
   input += synapse0x9dd4ec8();
   input += synapse0x9dd4ef0();
   input += synapse0x9dd4f18();
   input += synapse0x9dd4f40();
   input += synapse0x9dd4f68();
   input += synapse0x9dd4f90();
   input += synapse0x9dd4fb8();
   input += synapse0x9dd4fe0();
   input += synapse0x9dd5008();
   input += synapse0x9dd5030();
   input += synapse0x9dd5058();
   return input;
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd4d38() {
   double input = input0x9dd4d38();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsForward_2::input0x9dd5188() {
   double input = 0.301598;
   input += synapse0x9dd5360();
   input += synapse0x9dd5388();
   input += synapse0x9dd53b0();
   input += synapse0x9dd53d8();
   input += synapse0x9dd5400();
   input += synapse0x9dd5428();
   input += synapse0x9dd5450();
   input += synapse0x9dd5478();
   input += synapse0x9dd54a0();
   input += synapse0x9dd54c8();
   input += synapse0x9dd54f0();
   return input;
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd5188() {
   double input = input0x9dd5188();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsForward_2::input0x9dd5518() {
   double input = 0.429778;
   input += synapse0x9dd56f0();
   input += synapse0x9dd5718();
   input += synapse0x9dd5740();
   input += synapse0x9dd5768();
   input += synapse0x9dd5790();
   input += synapse0x9dd57b8();
   input += synapse0x9dd57e0();
   input += synapse0x9dd5808();
   input += synapse0x9dd5830();
   input += synapse0x9dd5858();
   input += synapse0x9dd5880();
   return input;
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd5518() {
   double input = input0x9dd5518();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsForward_2::input0x9dd58a8() {
   double input = 0.621569;
   input += synapse0x9dd5a80();
   input += synapse0x9dd5aa8();
   input += synapse0x9dd5ad0();
   input += synapse0x9dd5af8();
   input += synapse0x9dd5b20();
   input += synapse0x9dd5b48();
   input += synapse0x9dd5b70();
   input += synapse0x9dd5b98();
   input += synapse0x9dd5bc0();
   input += synapse0x9dd5be8();
   input += synapse0x9db4c88();
   return input;
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd58a8() {
   double input = input0x9dd58a8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsForward_2::input0x9dd5080() {
   double input = -2.49997;
   input += synapse0x9c84358();
   input += synapse0x9c84610();
   input += synapse0x9c84638();
   input += synapse0x9c84660();
   input += synapse0x9c84688();
   input += synapse0x9c846b0();
   return input;
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd5080() {
   double input = input0x9dd5080();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsForward_2::input0x9dd5e18() {
   double input = 0.076582;
   input += synapse0x9dd6010();
   return input;
}

double MuGirlNS::AnnResultsForward_2::neuron0x9dd5e18() {
   double input = input0x9dd5e18();
   return (input * 1)+0;
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd2f18() {
   return (neuron0x9dd3008()*1.60495);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4790() {
   return (neuron0x9dd3150()*0.743549);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd47b8() {
   return (neuron0x9dd3348()*10.2096);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd47e0() {
   return (neuron0x9dd3540()*-2.18734);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4808() {
   return (neuron0x9dd3738()*-0.0577286);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4830() {
   return (neuron0x9dd3930()*8.16218);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4858() {
   return (neuron0x9dd3b28()*-2.3229);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4880() {
   return (neuron0x9dd3d20()*0.851998);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd48a8() {
   return (neuron0x9dd3f18()*0.0107173);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd48d0() {
   return (neuron0x9dd4110()*4.79255);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd48f8() {
   return (neuron0x9dd4308()*6.87709);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4af8() {
   return (neuron0x9dd3008()*2.59607);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4b20() {
   return (neuron0x9dd3150()*-0.00268601);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4b48() {
   return (neuron0x9dd3348()*0.0487962);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4b70() {
   return (neuron0x9dd3540()*0.480496);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4b98() {
   return (neuron0x9dd3738()*-0.21103);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4bc0() {
   return (neuron0x9dd3930()*-0.642495);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4c70() {
   return (neuron0x9dd3b28()*1.11166);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4c98() {
   return (neuron0x9dd3d20()*1.14347);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4cc0() {
   return (neuron0x9dd3f18()*0.173208);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4ce8() {
   return (neuron0x9dd4110()*0.516082);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4d10() {
   return (neuron0x9dd4308()*0.171586);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4ec8() {
   return (neuron0x9dd3008()*0.0959152);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4ef0() {
   return (neuron0x9dd3150()*0.0225035);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4f18() {
   return (neuron0x9dd3348()*-0.061323);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4f40() {
   return (neuron0x9dd3540()*-1.0599);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4f68() {
   return (neuron0x9dd3738()*-1.75148);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4f90() {
   return (neuron0x9dd3930()*-0.559628);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4fb8() {
   return (neuron0x9dd3b28()*0.0154136);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd4fe0() {
   return (neuron0x9dd3d20()*0.0280752);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5008() {
   return (neuron0x9dd3f18()*54.8532);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5030() {
   return (neuron0x9dd4110()*0.00332437);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5058() {
   return (neuron0x9dd4308()*-7.9722);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5360() {
   return (neuron0x9dd3008()*1.88544);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5388() {
   return (neuron0x9dd3150()*0.618526);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd53b0() {
   return (neuron0x9dd3348()*0.898377);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd53d8() {
   return (neuron0x9dd3540()*-0.0933299);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5400() {
   return (neuron0x9dd3738()*0.229644);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5428() {
   return (neuron0x9dd3930()*0.481876);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5450() {
   return (neuron0x9dd3b28()*0.387184);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5478() {
   return (neuron0x9dd3d20()*0.826003);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd54a0() {
   return (neuron0x9dd3f18()*0.212529);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd54c8() {
   return (neuron0x9dd4110()*2.2663);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd54f0() {
   return (neuron0x9dd4308()*1.14004);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd56f0() {
   return (neuron0x9dd3008()*-2.84755);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5718() {
   return (neuron0x9dd3150()*0.208962);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5740() {
   return (neuron0x9dd3348()*-0.69768);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5768() {
   return (neuron0x9dd3540()*-0.754823);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5790() {
   return (neuron0x9dd3738()*0.391111);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd57b8() {
   return (neuron0x9dd3930()*0.0286709);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd57e0() {
   return (neuron0x9dd3b28()*-3.60081);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5808() {
   return (neuron0x9dd3d20()*-1.15733);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5830() {
   return (neuron0x9dd3f18()*-0.32845);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5858() {
   return (neuron0x9dd4110()*-0.762982);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5880() {
   return (neuron0x9dd4308()*-0.554945);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5a80() {
   return (neuron0x9dd3008()*0.0681338);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5aa8() {
   return (neuron0x9dd3150()*1.36065);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5ad0() {
   return (neuron0x9dd3348()*2.68902);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5af8() {
   return (neuron0x9dd3540()*-0.0248656);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5b20() {
   return (neuron0x9dd3738()*-0.315095);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5b48() {
   return (neuron0x9dd3930()*4.73404);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5b70() {
   return (neuron0x9dd3b28()*1.86547);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5b98() {
   return (neuron0x9dd3d20()*0.610135);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5bc0() {
   return (neuron0x9dd3f18()*0.495843);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd5be8() {
   return (neuron0x9dd4110()*1.38005);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9db4c88() {
   return (neuron0x9dd4308()*-3.2295);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9c84358() {
   return (neuron0x9dd4600()*13.7481);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9c84610() {
   return (neuron0x9dd4920()*-1.53361);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9c84638() {
   return (neuron0x9dd4d38()*-39.5114);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9c84660() {
   return (neuron0x9dd5188()*-1.78124);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9c84688() {
   return (neuron0x9dd5518()*-0.871145);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9c846b0() {
   return (neuron0x9dd58a8()*-5.21679);
}

double MuGirlNS::AnnResultsForward_2::synapse0x9dd6010() {
   return (neuron0x9dd5080()*0.947472);
}

