/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResultsEndcap_2.h"
#include <cmath>

double MuGirlNS::AnnResultsEndcap_2::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9,double in10) {
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
         return neuron0x9f19228();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f17d08() {
   return m_input0;
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f17e98() {
   return m_input1;
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f18090() {
   return m_input2;
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f18288() {
   return m_input3;
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f18480() {
   return m_input4;
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f18678() {
   return m_input5;
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f18870() {
   return m_input6;
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f18a68() {
   return m_input7;
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f18c60() {
   return m_input8;
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f18e58() {
   return m_input9;
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f19050() {
   return m_input10;
}

double MuGirlNS::AnnResultsEndcap_2::input0x9f19348() {
   double input = -0.245896;
   input += synapse0x9f17c18();
   input += synapse0x9f194d8();
   input += synapse0x9f19500();
   input += synapse0x9f19528();
   input += synapse0x9f19550();
   input += synapse0x9f19578();
   input += synapse0x9f195a0();
   input += synapse0x9f195c8();
   input += synapse0x9f195f0();
   input += synapse0x9f19618();
   input += synapse0x9f19640();
   return input;
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f19348() {
   double input = input0x9f19348();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsEndcap_2::input0x9f19668() {
   double input = 0.251141;
   input += synapse0x9f19840();
   input += synapse0x9f19868();
   input += synapse0x9f19890();
   input += synapse0x9f198b8();
   input += synapse0x9f198e0();
   input += synapse0x9f19908();
   input += synapse0x9f199b8();
   input += synapse0x9f199e0();
   input += synapse0x9f19a08();
   input += synapse0x9f19a30();
   input += synapse0x9f19a58();
   return input;
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f19668() {
   double input = input0x9f19668();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsEndcap_2::input0x9f19a80() {
   double input = 5.66502;
   input += synapse0x9f19c10();
   input += synapse0x9f19c38();
   input += synapse0x9f19c60();
   input += synapse0x9f19c88();
   input += synapse0x9f19cb0();
   input += synapse0x9f19cd8();
   input += synapse0x9f19d00();
   input += synapse0x9f19d28();
   input += synapse0x9f19d50();
   input += synapse0x9f19d78();
   input += synapse0x9f19da0();
   return input;
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f19a80() {
   double input = input0x9f19a80();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsEndcap_2::input0x9f19ed0() {
   double input = -0.284465;
   input += synapse0x9f1a0a8();
   input += synapse0x9f1a0d0();
   input += synapse0x9f1a0f8();
   input += synapse0x9f1a120();
   input += synapse0x9f1a148();
   input += synapse0x9f1a170();
   input += synapse0x9f1a198();
   input += synapse0x9f1a1c0();
   input += synapse0x9f1a1e8();
   input += synapse0x9f1a210();
   input += synapse0x9f1a238();
   return input;
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f19ed0() {
   double input = input0x9f19ed0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsEndcap_2::input0x9f1a260() {
   double input = 2.11799;
   input += synapse0x9f1a438();
   input += synapse0x9f1a460();
   input += synapse0x9f1a488();
   input += synapse0x9f1a4b0();
   input += synapse0x9f1a4d8();
   input += synapse0x9f1a500();
   input += synapse0x9f1a528();
   input += synapse0x9f1a550();
   input += synapse0x9f1a578();
   input += synapse0x9f1a5a0();
   input += synapse0x9f1a5c8();
   return input;
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f1a260() {
   double input = input0x9f1a260();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsEndcap_2::input0x9f1a5f0() {
   double input = 1.69603;
   input += synapse0x9f1a7c8();
   input += synapse0x9f1a7f0();
   input += synapse0x9f1a818();
   input += synapse0x9f1a840();
   input += synapse0x9f1a868();
   input += synapse0x9f1a890();
   input += synapse0x9f1a8b8();
   input += synapse0x9f1a8e0();
   input += synapse0x9f1a908();
   input += synapse0x9f1a930();
   input += synapse0x9da65e8();
   return input;
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f1a5f0() {
   double input = input0x9f1a5f0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsEndcap_2::input0x9f19dc8() {
   double input = 0.716561;
   input += synapse0x9da0308();
   input += synapse0x9da64f8();
   input += synapse0x9ef9a50();
   input += synapse0x9da67b0();
   input += synapse0x9da67d8();
   input += synapse0x9da6800();
   return input;
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f19dc8() {
   double input = input0x9f19dc8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsEndcap_2::input0x9f19228() {
   double input = 1.01202;
   input += synapse0x9f1ac38();
   return input;
}

double MuGirlNS::AnnResultsEndcap_2::neuron0x9f19228() {
   double input = input0x9f19228();
   return (input * 1)+0;
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f17c18() {
   return (neuron0x9f17d08()*1.0579);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f194d8() {
   return (neuron0x9f17e98()*-0.784273);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19500() {
   return (neuron0x9f18090()*0.597633);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19528() {
   return (neuron0x9f18288()*0.513618);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19550() {
   return (neuron0x9f18480()*0.20096);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19578() {
   return (neuron0x9f18678()*-0.25221);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f195a0() {
   return (neuron0x9f18870()*2.75718);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f195c8() {
   return (neuron0x9f18a68()*0.488725);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f195f0() {
   return (neuron0x9f18c60()*0.386945);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19618() {
   return (neuron0x9f18e58()*1.00209);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19640() {
   return (neuron0x9f19050()*0.160354);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19840() {
   return (neuron0x9f17d08()*-0.000145279);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19868() {
   return (neuron0x9f17e98()*-0.0539114);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19890() {
   return (neuron0x9f18090()*0.060635);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f198b8() {
   return (neuron0x9f18288()*0.0173249);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f198e0() {
   return (neuron0x9f18480()*-0.487842);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19908() {
   return (neuron0x9f18678()*0.207552);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f199b8() {
   return (neuron0x9f18870()*0.0104411);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f199e0() {
   return (neuron0x9f18a68()*0.0983321);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19a08() {
   return (neuron0x9f18c60()*-7.59119);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19a30() {
   return (neuron0x9f18e58()*-0.00169469);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19a58() {
   return (neuron0x9f19050()*11.6355);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19c10() {
   return (neuron0x9f17d08()*0.0303864);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19c38() {
   return (neuron0x9f17e98()*-1.84755);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19c60() {
   return (neuron0x9f18090()*-0.127549);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19c88() {
   return (neuron0x9f18288()*-1.75689);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19cb0() {
   return (neuron0x9f18480()*4.05112);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19cd8() {
   return (neuron0x9f18678()*-1.99791);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19d00() {
   return (neuron0x9f18870()*-0.0515667);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19d28() {
   return (neuron0x9f18a68()*0.0936783);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19d50() {
   return (neuron0x9f18c60()*0.376222);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19d78() {
   return (neuron0x9f18e58()*0.0087273);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f19da0() {
   return (neuron0x9f19050()*-3.8322);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a0a8() {
   return (neuron0x9f17d08()*0.568292);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a0d0() {
   return (neuron0x9f17e98()*0.310191);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a0f8() {
   return (neuron0x9f18090()*0.313064);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a120() {
   return (neuron0x9f18288()*-0.0424206);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a148() {
   return (neuron0x9f18480()*0.21687);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a170() {
   return (neuron0x9f18678()*-0.364653);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a198() {
   return (neuron0x9f18870()*0.121712);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a1c0() {
   return (neuron0x9f18a68()*0.0826275);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a1e8() {
   return (neuron0x9f18c60()*-0.222418);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a210() {
   return (neuron0x9f18e58()*0.224308);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a238() {
   return (neuron0x9f19050()*0.154258);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a438() {
   return (neuron0x9f17d08()*-2.99843);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a460() {
   return (neuron0x9f17e98()*-3.5748);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a488() {
   return (neuron0x9f18090()*-2.46459);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a4b0() {
   return (neuron0x9f18288()*-2.45421);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a4d8() {
   return (neuron0x9f18480()*1.28434);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a500() {
   return (neuron0x9f18678()*-0.226887);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a528() {
   return (neuron0x9f18870()*-2.16047);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a550() {
   return (neuron0x9f18a68()*-2.74669);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a578() {
   return (neuron0x9f18c60()*-0.425474);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a5a0() {
   return (neuron0x9f18e58()*5.82315);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a5c8() {
   return (neuron0x9f19050()*-0.267883);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a7c8() {
   return (neuron0x9f17d08()*0.24426);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a7f0() {
   return (neuron0x9f17e98()*1.51817);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a818() {
   return (neuron0x9f18090()*-1.14455);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a840() {
   return (neuron0x9f18288()*-0.641583);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a868() {
   return (neuron0x9f18480()*-0.0350036);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a890() {
   return (neuron0x9f18678()*-0.332256);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a8b8() {
   return (neuron0x9f18870()*-0.674681);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a8e0() {
   return (neuron0x9f18a68()*-0.927588);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a908() {
   return (neuron0x9f18c60()*-1.24803);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1a930() {
   return (neuron0x9f18e58()*-2.51705);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9da65e8() {
   return (neuron0x9f19050()*0.716312);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9da0308() {
   return (neuron0x9f19348()*6.09867);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9da64f8() {
   return (neuron0x9f19668()*-10.9276);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9ef9a50() {
   return (neuron0x9f19a80()*8.45849);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9da67b0() {
   return (neuron0x9f19ed0()*0.156716);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9da67d8() {
   return (neuron0x9f1a260()*1.10471);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9da6800() {
   return (neuron0x9f1a5f0()*6.60449);
}

double MuGirlNS::AnnResultsEndcap_2::synapse0x9f1ac38() {
   return (neuron0x9f19dc8()*-0.97531);
}

