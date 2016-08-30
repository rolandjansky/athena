/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResultsBarrelBad_2.h"
#include <cmath>

double MuGirlNS::AnnResultsBarrelBad_2::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) 
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
         return neuron0x91169b8();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResultsBarrelBad_2::neuron0x9114918() 
{
   return m_input0;
}

double MuGirlNS::AnnResultsBarrelBad_2::neuron0x9114aa8() 
{
   return m_input1;
}

double MuGirlNS::AnnResultsBarrelBad_2::neuron0x9114ca0() 
{
   return m_input2;
}

double MuGirlNS::AnnResultsBarrelBad_2::neuron0x9114e98() 
{
   return m_input3;
}

double MuGirlNS::AnnResultsBarrelBad_2::neuron0x9115090() 
{
   return m_input4;
}

double MuGirlNS::AnnResultsBarrelBad_2::neuron0x9115288() 
{
   return m_input5;
}

double MuGirlNS::AnnResultsBarrelBad_2::neuron0x9115480() 
{
   return m_input6;
}

double MuGirlNS::AnnResultsBarrelBad_2::neuron0x9115678() 
{
   return m_input7;
}

double MuGirlNS::AnnResultsBarrelBad_2::neuron0x9115870() 
{
   return m_input8;
}

double MuGirlNS::AnnResultsBarrelBad_2::neuron0x9115a68() 
{
   return m_input9;
}

double MuGirlNS::AnnResultsBarrelBad_2::input0x9115d60() 
{
   double input = 1.31444;
   input += synapse0x90f6b48();
   input += synapse0x9115ef0();
   input += synapse0x9115f18();
   input += synapse0x9115f40();
   input += synapse0x9115f68();
   input += synapse0x9115f90();
   input += synapse0x9115fb8();
   input += synapse0x9115fe0();
   input += synapse0x9116008();
   input += synapse0x9116030();
   return input;
}

double MuGirlNS::AnnResultsBarrelBad_2::neuron0x9115d60() 
{
   double input = input0x9115d60();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsBarrelBad_2::input0x9116058() 
{
   double input = 0.572019;
   input += synapse0x9116230();
   input += synapse0x9116258();
   input += synapse0x9116280();
   input += synapse0x91162a8();
   input += synapse0x91162d0();
   input += synapse0x91162f8();
   input += synapse0x9116320();
   input += synapse0x91163d0();
   input += synapse0x91163f8();
   input += synapse0x9116420();
   return input;
}

double MuGirlNS::AnnResultsBarrelBad_2::neuron0x9116058() 
{
   double input = input0x9116058();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsBarrelBad_2::input0x9116448() 
{
   double input = 0.799983;
   input += synapse0x91165d8();
   input += synapse0x9116600();
   input += synapse0x9116628();
   input += synapse0x9116650();
   input += synapse0x9116678();
   input += synapse0x91166a0();
   input += synapse0x91166c8();
   input += synapse0x91166f0();
   input += synapse0x9116718();
   input += synapse0x9116740();
   return input;
}

double MuGirlNS::AnnResultsBarrelBad_2::neuron0x9116448() 
{
   double input = input0x9116448();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsBarrelBad_2::input0x9116768() 
{
   double input = -1.85379;
   input += synapse0x9116940();
   input += synapse0x9116968();
   input += synapse0x9116990();
   input += synapse0x8fc3368();
   input += synapse0x8fc3138();
   input += synapse0x8fc3160();
   input += synapse0x8fc3188();
   input += synapse0x9116348();
   input += synapse0x9116370();
   input += synapse0x9116398();
   return input;
}

double MuGirlNS::AnnResultsBarrelBad_2::neuron0x9116768() 
{
   double input = input0x9116768();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsBarrelBad_2::input0x9116ac0() 
{
   double input = 0.360864;
   input += synapse0x9116c98();
   input += synapse0x9116cc0();
   input += synapse0x9116ce8();
   input += synapse0x9116d10();
   input += synapse0x9116d38();
   input += synapse0x9116d60();
   input += synapse0x9116d88();
   input += synapse0x9116db0();
   input += synapse0x9116dd8();
   input += synapse0x9116e00();
   return input;
}

double MuGirlNS::AnnResultsBarrelBad_2::neuron0x9116ac0() 
{
   double input = input0x9116ac0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsBarrelBad_2::input0x9116e28() 
{
   double input = 0.2545;
   input += synapse0x9117000();
   input += synapse0x9117028();
   input += synapse0x9117050();
   input += synapse0x9117078();
   input += synapse0x91170a0();
   input += synapse0x91170c8();
   input += synapse0x91170f0();
   input += synapse0x9117118();
   input += synapse0x9117140();
   input += synapse0x9117168();
   return input;
}

double MuGirlNS::AnnResultsBarrelBad_2::neuron0x9116e28() 
{
   double input = input0x9116e28();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsBarrelBad_2::input0x91171b8() 
{
   double input = -3.14867;
   input += synapse0x9117190();
   input += synapse0x9117418();
   input += synapse0x9117440();
   input += synapse0x9117468();
   input += synapse0x9117490();
   input += synapse0x90f6a80();
   return input;
}

double MuGirlNS::AnnResultsBarrelBad_2::neuron0x91171b8() 
{
   double input = input0x91171b8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResultsBarrelBad_2::input0x91169b8() 
{
   double input = -3.08697;
   input += synapse0x8fc3698();
   return input;
}

double MuGirlNS::AnnResultsBarrelBad_2::neuron0x91169b8() 
{
   double input = input0x91169b8();
   return (input * 1)+0;
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x90f6b48() 
{
   return (neuron0x9114918()*-1.14805);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9115ef0() 
{
   return (neuron0x9114aa8()*-1.09396);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9115f18() 
{
   return (neuron0x9114ca0()*0.816899);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9115f40() 
{
   return (neuron0x9114e98()*0.367129);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9115f68() 
{
   return (neuron0x9115090()*0.297896);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9115f90() 
{
   return (neuron0x9115288()*-0.664152);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9115fb8() 
{
   return (neuron0x9115480()*-0.445676);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9115fe0() 
{
   return (neuron0x9115678()*0.0438097);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116008() 
{
   return (neuron0x9115870()*0.696588);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116030() 
{
   return (neuron0x9115a68()*-0.278339);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116230() 
{
   return (neuron0x9114918()*0.00702466);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116258() 
{
   return (neuron0x9114aa8()*-0.0573754);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116280() 
{
   return (neuron0x9114ca0()*-0.0915328);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x91162a8() 
{
   return (neuron0x9114e98()*0.495367);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x91162d0() 
{
   return (neuron0x9115090()*0.742757);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x91162f8() 
{
   return (neuron0x9115288()*0.169065);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116320() 
{
   return (neuron0x9115480()*0.0426446);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x91163d0() 
{
   return (neuron0x9115678()*0.63178);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x91163f8() 
{
   return (neuron0x9115870()*0.00442552);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116420() 
{
   return (neuron0x9115a68()*-0.869303);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x91165d8() 
{
   return (neuron0x9114918()*-0.0121221);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116600() 
{
   return (neuron0x9114aa8()*0.00236914);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116628() 
{
   return (neuron0x9114ca0()*-0.431721);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116650() 
{
   return (neuron0x9114e98()*0.0842467);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116678() 
{
   return (neuron0x9115090()*1.13396);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x91166a0() 
{
   return (neuron0x9115288()*-0.0430772);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x91166c8() 
{
   return (neuron0x9115480()*0.185478);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x91166f0() 
{
   return (neuron0x9115678()*0.352134);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116718() 
{
   return (neuron0x9115870()*-0.0123065);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116740() 
{
   return (neuron0x9115a68()*10.5041);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116940() 
{
   return (neuron0x9114918()*0.0147275);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116968() 
{
   return (neuron0x9114aa8()*0.0369692);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116990() 
{
   return (neuron0x9114ca0()*0.322918);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x8fc3368() 
{
   return (neuron0x9114e98()*0.269697);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x8fc3138() 
{
   return (neuron0x9115090()*0.645176);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x8fc3160() 
{
   return (neuron0x9115288()*0.0904885);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x8fc3188() 
{
   return (neuron0x9115480()*-0.244916);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116348() 
{
   return (neuron0x9115678()*-0.0479291);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116370() 
{
   return (neuron0x9115870()*0.013127);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116398() 
{
   return (neuron0x9115a68()*5.28933);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116c98() 
{
   return (neuron0x9114918()*-0.0452925);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116cc0() 
{
   return (neuron0x9114aa8()*-1.26925);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116ce8() 
{
   return (neuron0x9114ca0()*0.152384);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116d10() 
{
   return (neuron0x9114e98()*-0.249522);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116d38() 
{
   return (neuron0x9115090()*0.0219326);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116d60() 
{
   return (neuron0x9115288()*0.0490194);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116d88() 
{
   return (neuron0x9115480()*0.353428);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116db0() 
{
   return (neuron0x9115678()*-0.263505);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116dd8() 
{
   return (neuron0x9115870()*-1.04121);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9116e00() 
{
   return (neuron0x9115a68()*-0.32492);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9117000() 
{
   return (neuron0x9114918()*0.0107322);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9117028() 
{
   return (neuron0x9114aa8()*-0.140368);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9117050() 
{
   return (neuron0x9114ca0()*0.759216);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9117078() 
{
   return (neuron0x9114e98()*0.561685);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x91170a0() 
{
   return (neuron0x9115090()*-3.35129);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x91170c8() 
{
   return (neuron0x9115288()*0.0142184);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x91170f0() 
{
   return (neuron0x9115480()*-0.136842);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9117118() 
{
   return (neuron0x9115678()*-1.04893);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9117140() 
{
   return (neuron0x9115870()*0.0120179);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9117168() 
{
   return (neuron0x9115a68()*2.61825);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9117190() 
{
   return (neuron0x9115d60()*-0.361242);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9117418() 
{
   return (neuron0x9116058()*-3.94767);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9117440() 
{
   return (neuron0x9116448()*6.95758);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9117468() 
{
   return (neuron0x9116768()*5.61079);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x9117490() 
{
   return (neuron0x9116ac0()*-0.748571);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x90f6a80() 
{
   return (neuron0x9116e28()*2.99312);
}

double MuGirlNS::AnnResultsBarrelBad_2::synapse0x8fc3698() 
{
   return (neuron0x91171b8()*4.08941);
}

