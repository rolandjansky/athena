/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResults_tgcfix_endcap_nocsc.h"
#include <cmath>

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) 
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
         return neuron0x9c44df8();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::neuron0x9c34a08() 
{
   return m_input0;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::neuron0x9c34b98() 
{
   return m_input1;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::neuron0x9c43120()
{
   return m_input2;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::neuron0x9c432f8()
{
   return m_input3;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::neuron0x9c434d0()
{
   return m_input4;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::neuron0x9c436c8()
{
   return m_input5;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::neuron0x9c438c0()
{
   return m_input6;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::neuron0x9c43ab8()
{
   return m_input7;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::neuron0x9c43cb0()
{
   return m_input8;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::neuron0x9c43ea8()
{
   return m_input9;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::input0x9c441a0()
{
   double input = -0.485027;
   input += synapse0x9c12d90();
   input += synapse0x9c44330();
   input += synapse0x9c44358();
   input += synapse0x9c44380();
   input += synapse0x9c443a8();
   input += synapse0x9c443d0();
   input += synapse0x9c443f8();
   input += synapse0x9c44420();
   input += synapse0x9c44448();
   input += synapse0x9c44470();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::neuron0x9c441a0()
{
   double input = input0x9c441a0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::input0x9c44498()
{
   double input = 0.00710075;
   input += synapse0x9c44670();
   input += synapse0x9c44698();
   input += synapse0x9c446c0();
   input += synapse0x9c446e8();
   input += synapse0x9c44710();
   input += synapse0x9c44738();
   input += synapse0x9c44760();
   input += synapse0x9c44810();
   input += synapse0x9c44838();
   input += synapse0x9c44860();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::neuron0x9c44498()
{
   double input = input0x9c44498();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::input0x9c44888()
{
   double input = -3.19966;
   input += synapse0x9c44a18();
   input += synapse0x9c44a40();
   input += synapse0x9c44a68();
   input += synapse0x9c44a90();
   input += synapse0x9c44ab8();
   input += synapse0x9c44ae0();
   input += synapse0x9c44b08();
   input += synapse0x9c44b30();
   input += synapse0x9c44b58();
   input += synapse0x9c44b80();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::neuron0x9c44888()
{
   double input = input0x9c44888();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::input0x9c44ba8()
{
   double input = -0.429648;
   input += synapse0x9c44d80();
   input += synapse0x9c44da8();
   input += synapse0x9c44dd0();
   input += synapse0x9c44788();
   input += synapse0x9c447b0();
   input += synapse0x9c447d8();
   input += synapse0x9c44f00();
   input += synapse0x9c44f28();
   input += synapse0x9c44f50();
   input += synapse0x9c44f78();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::neuron0x9c44ba8()
{
   double input = input0x9c44ba8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::input0x9c44fa0()
{
   double input = -0.128976;
   input += synapse0x9c45178();
   input += synapse0x9c451a0();
   input += synapse0x9c451c8();
   input += synapse0x9c451f0();
   input += synapse0x9c45218();
   input += synapse0x9c45240();
   input += synapse0x9c45268();
   input += synapse0x9c45290();
   input += synapse0x9c452b8();
   input += synapse0x9c452e0();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::neuron0x9c44fa0()
{
   double input = input0x9c44fa0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::input0x9c45308()
{
   double input = -0.20202;
   input += synapse0x9c454e0();
   input += synapse0x9c45508();
   input += synapse0x9c45530();
   input += synapse0x9c45558();
   input += synapse0x9c45580();
   input += synapse0x9c455a8();
   input += synapse0x9c455d0();
   input += synapse0x9c455f8();
   input += synapse0x9c45620();
   input += synapse0x9c45648();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::neuron0x9c45308()
{
   double input = input0x9c45308();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::input0x9c45698()
{
   double input = -0.853502;
   input += synapse0x9c45670();
   input += synapse0x9c458f8();
   input += synapse0x9c45920();
   input += synapse0x9c45948();
   input += synapse0x9c45970();
   input += synapse0x9c12db8();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::neuron0x9c45698()
{
   double input = input0x9c45698();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::input0x9c44df8()
{
   double input = 0.000208154;
   input += synapse0x9ac93c0();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::neuron0x9c44df8()
{
   double input = input0x9c44df8();
   return (input * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c12d90()
{
   return (neuron0x9c34a08()*0.832115);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44330()
{
   return (neuron0x9c34b98()*-1.83742);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44358()
{
   return (neuron0x9c43120()*4.05483);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44380()
{
   return (neuron0x9c432f8()*0.650681);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c443a8()
{
   return (neuron0x9c434d0()*0.195157);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c443d0()
{
   return (neuron0x9c436c8()*1.31999);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c443f8()
{
   return (neuron0x9c438c0()*1.44597);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44420()
{
   return (neuron0x9c43ab8()*-0.487326);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44448()
{
   return (neuron0x9c43cb0()*2.69764);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44470()
{
   return (neuron0x9c43ea8()*-0.237154);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44670()
{
  return (neuron0x9c34a08()*0.863739);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44698()
{
  return (neuron0x9c34b98()*0.350039);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c446c0()
{
  return (neuron0x9c43120()*0.119028);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c446e8()
{
  return (neuron0x9c432f8()*0.11429);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44710()
{
  return (neuron0x9c434d0()*-0.42872);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44738()
{
  return (neuron0x9c436c8()*0.267354);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44760()
{
  return (neuron0x9c438c0()*0.210324);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44810()
{
  return (neuron0x9c43ab8()*-0.149199);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44838()
{
  return (neuron0x9c43cb0()*0.496566);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44860()
{
  return (neuron0x9c43ea8()*0.196562);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44a18()
{
  return (neuron0x9c34a08()*0.474007);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44a40()
{
  return (neuron0x9c34b98()*-0.203263);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44a68()
{
   return (neuron0x9c43120()*-0.273294);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44a90()
{
  return (neuron0x9c432f8()*-0.922347);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44ab8()
{
  return (neuron0x9c434d0()*-3.2256);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44ae0()
{
  return (neuron0x9c436c8()*-0.0302054);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44b08()
{
  return (neuron0x9c438c0()*0.585974);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44b30()
{ 
 return (neuron0x9c43ab8()*-0.312698);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44b58()
{
   return (neuron0x9c43cb0()*5.80676);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44b80()
{
  return (neuron0x9c43ea8()*2.34628);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44d80()
{
   return (neuron0x9c34a08()*-0.869256);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44da8()
{
  return (neuron0x9c34b98()*-0.62028);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44dd0()
{
   return (neuron0x9c43120()*0.011731);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44788()
{
   return (neuron0x9c432f8()*-0.427315);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c447b0()
{
  return (neuron0x9c434d0()*-0.151973);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c447d8()
{
  return (neuron0x9c436c8()*-0.592394);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44f00()
{
   return (neuron0x9c438c0()*-0.309494);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44f28()
{
  return (neuron0x9c43ab8()*0.195587);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44f50()
{
  return (neuron0x9c43cb0()*-0.675804);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c44f78()
{
 return (neuron0x9c43ea8()*0.188432);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c45178()
{
  return (neuron0x9c34a08()*-0.836408);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c451a0()
{
  return (neuron0x9c34b98()*-0.718084);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c451c8()
{
 return (neuron0x9c43120()*-0.34932);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c451f0()
{
  return (neuron0x9c432f8()*0.0344994);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c45218()
{
  return (neuron0x9c434d0()*0.0363419);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c45240()
{
 return (neuron0x9c436c8()*-0.81605);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c45268()
{
  return (neuron0x9c438c0()*-0.0499825);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c45290()
{
  return (neuron0x9c43ab8()*0.156419);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c452b8()
{
  return (neuron0x9c43cb0()*-1.09839);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c452e0()
{
  return (neuron0x9c43ea8()*0.473922);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c454e0()
{
  return (neuron0x9c34a08()*0.0348762);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c45508()
{
  return (neuron0x9c34b98()*-0.0401556);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c45530()
{
  return (neuron0x9c43120()*-0.00972562);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c45558()
{
 return (neuron0x9c432f8()*-0.335909);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c45580()
{
  return (neuron0x9c434d0()*-0.169109);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c455a8()
{
  return (neuron0x9c436c8()*-0.052587);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c455d0()
{
  return (neuron0x9c438c0()*-0.112423);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c455f8()
{
  return (neuron0x9c43ab8()*10.9005);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c45620()
{
  return (neuron0x9c43cb0()*0.00278116);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c45648()
{
  return (neuron0x9c43ea8()*-3.78503);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c45670()
{
  return (neuron0x9c441a0()*-4.90235);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c458f8()
{
  return (neuron0x9c44498()*-0.237822);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c45920()
{
  return (neuron0x9c44888()*6.17929);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c45948()
{
  return (neuron0x9c44ba8()*0.0378049);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c45970()
{
 return (neuron0x9c44fa0()*-1.30391);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9c12db8()
{
  return (neuron0x9c45308()*-8.09647);
}

double MuGirlNS::AnnResults_tgcfix_endcap_nocsc::synapse0x9ac93c0()
{  return (neuron0x9c45698()*1.82125);
}

