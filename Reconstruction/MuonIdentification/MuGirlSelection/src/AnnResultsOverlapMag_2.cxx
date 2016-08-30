/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResultsOverlapMag_2.h"
#include <cmath>

double MuGirlNS::AnnResultsOverlapMag_2::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) {
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
         return neuron0x90c8a58();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResultsOverlapMag_2::neuron0x90b8748() {
   return m_input0;
}

double MuGirlNS::AnnResultsOverlapMag_2::neuron0x90b8940() {
   return m_input1;
}

double MuGirlNS::AnnResultsOverlapMag_2::neuron0x90c6dc8() {
   return m_input2;
}

double MuGirlNS::AnnResultsOverlapMag_2::neuron0x90c6f58() {
   return m_input3;
}

double MuGirlNS::AnnResultsOverlapMag_2::neuron0x90c7130() {
   return m_input4;
}

double MuGirlNS::AnnResultsOverlapMag_2::neuron0x90c7328() {
   return m_input5;
}

double MuGirlNS::AnnResultsOverlapMag_2::neuron0x90c7520() {
   return m_input6;
}

double MuGirlNS::AnnResultsOverlapMag_2::neuron0x90c7718() {
   return m_input7;
}

double MuGirlNS::AnnResultsOverlapMag_2::neuron0x90c7910() {
   return m_input8;
}

double MuGirlNS::AnnResultsOverlapMag_2::neuron0x90c7b08() {
   return m_input9;
}

double MuGirlNS::AnnResultsOverlapMag_2::input0x90c7e00() {
   double input = 4.56324;
   input += synapse0x8fcc168();
   input += synapse0x90c7f90();
   input += synapse0x90c7fb8();
   input += synapse0x90c7fe0();
   input += synapse0x90c8008();
   input += synapse0x90c8030();
   input += synapse0x90c8058();
   input += synapse0x90c8080();
   input += synapse0x90c80a8();
   input += synapse0x90c80d0();
   return input;
}

double MuGirlNS::AnnResultsOverlapMag_2::neuron0x90c7e00() {
   double input = input0x90c7e00();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsOverlapMag_2::input0x90c80f8() {
   double input = -5.08836;
   input += synapse0x90c82d0();
   input += synapse0x90c82f8();
   input += synapse0x90c8320();
   input += synapse0x90c8348();
   input += synapse0x90c8370();
   input += synapse0x90c8398();
   input += synapse0x90c83c0();
   input += synapse0x90c8470();
   input += synapse0x90c8498();
   input += synapse0x90c84c0();
   return input;
}

double MuGirlNS::AnnResultsOverlapMag_2::neuron0x90c80f8() {
   double input = input0x90c80f8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsOverlapMag_2::input0x90c84e8() {
   double input = 1.61501;
   input += synapse0x90c8678();
   input += synapse0x90c86a0();
   input += synapse0x90c86c8();
   input += synapse0x90c86f0();
   input += synapse0x90c8718();
   input += synapse0x90c8740();
   input += synapse0x90c8768();
   input += synapse0x90c8790();
   input += synapse0x90c87b8();
   input += synapse0x90c87e0();
   return input;
}

double MuGirlNS::AnnResultsOverlapMag_2::neuron0x90c84e8() {
   double input = input0x90c84e8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsOverlapMag_2::input0x90c8808() {
   double input = -2.03431;
   input += synapse0x90c89e0();
   input += synapse0x90c8a08();
   input += synapse0x90c8a30();
   input += synapse0x90c83e8();
   input += synapse0x90c8410();
   input += synapse0x90c8438();
   input += synapse0x90c8b60();
   input += synapse0x90c8b88();
   input += synapse0x90c8bb0();
   input += synapse0x90c8bd8();
   return input;
}

double MuGirlNS::AnnResultsOverlapMag_2::neuron0x90c8808() {
   double input = input0x90c8808();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsOverlapMag_2::input0x90c8c00() {
   double input = -0.100176;
   input += synapse0x90c8dd8();
   input += synapse0x90c8e00();
   input += synapse0x90c8e28();
   input += synapse0x90c8e50();
   input += synapse0x90c8e78();
   input += synapse0x90c8ea0();
   input += synapse0x90c8ec8();
   input += synapse0x90c8ef0();
   input += synapse0x90c8f18();
   input += synapse0x90c8f40();
   return input;
}

double MuGirlNS::AnnResultsOverlapMag_2::neuron0x90c8c00() {
   double input = input0x90c8c00();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsOverlapMag_2::input0x90c8f68() {
   double input = -0.112562;
   input += synapse0x90c9140();
   input += synapse0x90c9168();
   input += synapse0x90c9190();
   input += synapse0x90c91b8();
   input += synapse0x90c91e0();
   input += synapse0x90c9208();
   input += synapse0x90c9230();
   input += synapse0x90c9258();
   input += synapse0x90c9280();
   input += synapse0x90c92a8();
   return input;
}

double MuGirlNS::AnnResultsOverlapMag_2::neuron0x90c8f68() {
   double input = input0x90c8f68();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsOverlapMag_2::input0x90c92f8() {
   double input = 11.8033;
   input += synapse0x90c92d0();
   input += synapse0x90c9558();
   input += synapse0x90c9580();
   input += synapse0x90c95a8();
   input += synapse0x90c95d0();
   input += synapse0x8fcc280();
   return input;
}

double MuGirlNS::AnnResultsOverlapMag_2::neuron0x90c92f8() {
   double input = input0x90c92f8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsOverlapMag_2::input0x90c8a58() {
   double input = -0.0114334;
   input += synapse0x8fcc4c8();
   return input;
}

double MuGirlNS::AnnResultsOverlapMag_2::neuron0x90c8a58() {
   double input = input0x90c8a58();
   return (input * 1)+0;
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x8fcc168() {
   return (neuron0x90b8748()*-2.96954);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c7f90() {
   return (neuron0x90b8940()*-1.78463);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c7fb8() {
   return (neuron0x90c6dc8()*0.752632);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c7fe0() {
   return (neuron0x90c6f58()*0.0782881);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8008() {
   return (neuron0x90c7130()*-1.31511);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8030() {
   return (neuron0x90c7328()*5.36344);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8058() {
   return (neuron0x90c7520()*4.35145);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8080() {
   return (neuron0x90c7718()*-0.235013);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c80a8() {
   return (neuron0x90c7910()*-0.397269);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c80d0() {
   return (neuron0x90c7b08()*1.19874);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c82d0() {
   return (neuron0x90b8748()*2.61938);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c82f8() {
   return (neuron0x90b8940()*-1.69867);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8320() {
   return (neuron0x90c6dc8()*-0.810188);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8348() {
   return (neuron0x90c6f58()*-0.244932);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8370() {
   return (neuron0x90c7130()*-0.745771);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8398() {
   return (neuron0x90c7328()*2.15827);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c83c0() {
   return (neuron0x90c7520()*6.29934);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8470() {
   return (neuron0x90c7718()*-0.26957);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8498() {
   return (neuron0x90c7910()*-0.536684);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c84c0() {
   return (neuron0x90c7b08()*-0.891064);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8678() {
   return (neuron0x90b8748()*-0.0582424);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c86a0() {
   return (neuron0x90b8940()*0.377996);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c86c8() {
   return (neuron0x90c6dc8()*-0.72861);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c86f0() {
   return (neuron0x90c6f58()*2.97808);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8718() {
   return (neuron0x90c7130()*-4.60367);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8740() {
   return (neuron0x90c7328()*-0.276184);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8768() {
   return (neuron0x90c7520()*0.323611);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8790() {
   return (neuron0x90c7718()*7.51851);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c87b8() {
   return (neuron0x90c7910()*0.000612297);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c87e0() {
   return (neuron0x90c7b08()*-7.64261);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c89e0() {
   return (neuron0x90b8748()*-0.588787);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8a08() {
   return (neuron0x90b8940()*2.13436);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8a30() {
   return (neuron0x90c6dc8()*-2.4758);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c83e8() {
   return (neuron0x90c6f58()*1.12612);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8410() {
   return (neuron0x90c7130()*-2.62883);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8438() {
   return (neuron0x90c7328()*2.53926);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8b60() {
   return (neuron0x90c7520()*-5.84242);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8b88() {
   return (neuron0x90c7718()*0.266558);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8bb0() {
   return (neuron0x90c7910()*0.795102);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8bd8() {
   return (neuron0x90c7b08()*-0.0589487);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8dd8() {
   return (neuron0x90b8748()*-3.44803);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8e00() {
   return (neuron0x90b8940()*1.88479);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8e28() {
   return (neuron0x90c6dc8()*-0.718417);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8e50() {
   return (neuron0x90c6f58()*0.993228);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8e78() {
   return (neuron0x90c7130()*0.329013);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8ea0() {
   return (neuron0x90c7328()*3.83057);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8ec8() {
   return (neuron0x90c7520()*-2.04145);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8ef0() {
   return (neuron0x90c7718()*-0.130282);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8f18() {
   return (neuron0x90c7910()*-1.14689);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c8f40() {
   return (neuron0x90c7b08()*-0.659017);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c9140() {
   return (neuron0x90b8748()*-0.00699129);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c9168() {
   return (neuron0x90b8940()*-0.147112);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c9190() {
   return (neuron0x90c6dc8()*0.194403);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c91b8() {
   return (neuron0x90c6f58()*-0.963541);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c91e0() {
   return (neuron0x90c7130()*1.41761);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c9208() {
   return (neuron0x90c7328()*0.107275);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c9230() {
   return (neuron0x90c7520()*-0.108989);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c9258() {
   return (neuron0x90c7718()*1.94366);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c9280() {
   return (neuron0x90c7910()*-0.000364966);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c92a8() {
   return (neuron0x90c7b08()*-17.3838);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c92d0() {
   return (neuron0x90c7e00()*2.30272);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c9558() {
   return (neuron0x90c80f8()*-1.70314);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c9580() {
   return (neuron0x90c84e8()*-7.03835);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c95a8() {
   return (neuron0x90c8808()*-1.15009);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x90c95d0() {
   return (neuron0x90c8c00()*-4.62994);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x8fcc280() {
   return (neuron0x90c8f68()*-16.5145);
}

double MuGirlNS::AnnResultsOverlapMag_2::synapse0x8fcc4c8() {
   return (neuron0x90c92f8()*1.00423);
}

