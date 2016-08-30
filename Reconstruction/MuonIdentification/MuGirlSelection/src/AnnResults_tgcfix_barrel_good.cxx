/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResults_tgcfix_barrel_good.h"
#include <cmath>

double MuGirlNS::AnnResults_tgcfix_barrel_good::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) {
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
         return neuron0xad34c70();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::neuron0xad24860() {
   return m_input0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::neuron0xad24a10() {
   return m_input1;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::neuron0xad32f78() {
   return m_input2;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::neuron0xad33150() {
   return m_input3;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::neuron0xad33348() {
   return m_input4;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::neuron0xad33540() {
   return m_input5;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::neuron0xad33738() {
   return m_input6;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::neuron0xad33930() {
   return m_input7;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::neuron0xad33b28() {
   return m_input8;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::neuron0xad33d20() {
   return m_input9;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::input0xad34018() {
   double input = -0.602486;
   input += synapse0xad02af0();
   input += synapse0xad341a8();
   input += synapse0xad341d0();
   input += synapse0xad341f8();
   input += synapse0xad34220();
   input += synapse0xad34248();
   input += synapse0xad34270();
   input += synapse0xad34298();
   input += synapse0xad342c0();
   input += synapse0xad342e8();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::neuron0xad34018() {
   double input = input0xad34018();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::input0xad34310() {
   double input = -0.330107;
   input += synapse0xad344e8();
   input += synapse0xad34510();
   input += synapse0xad34538();
   input += synapse0xad34560();
   input += synapse0xad34588();
   input += synapse0xad345b0();
   input += synapse0xad345d8();
   input += synapse0xad34688();
   input += synapse0xad346b0();
   input += synapse0xad346d8();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::neuron0xad34310() {
   double input = input0xad34310();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::input0xad34700() {
   double input = -0.569631;
   input += synapse0xad34890();
   input += synapse0xad348b8();
   input += synapse0xad348e0();
   input += synapse0xad34908();
   input += synapse0xad34930();
   input += synapse0xad34958();
   input += synapse0xad34980();
   input += synapse0xad349a8();
   input += synapse0xad349d0();
   input += synapse0xad349f8();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::neuron0xad34700() {
   double input = input0xad34700();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::input0xad34a20() {
   double input = -0.206462;
   input += synapse0xad34bf8();
   input += synapse0xad34c20();
   input += synapse0xad34c48();
   input += synapse0xad34600();
   input += synapse0xad34628();
   input += synapse0xad34650();
   input += synapse0xad34d78();
   input += synapse0xad34da0();
   input += synapse0xad34dc8();
   input += synapse0xad34df0();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::neuron0xad34a20() {
   double input = input0xad34a20();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::input0xad34e18() {
   double input = -0.530453;
   input += synapse0xad34ff0();
   input += synapse0xad35018();
   input += synapse0xad35040();
   input += synapse0xad35068();
   input += synapse0xad35090();
   input += synapse0xad350b8();
   input += synapse0xad350e0();
   input += synapse0xad35108();
   input += synapse0xad35130();
   input += synapse0xad35158();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::neuron0xad34e18() {
   double input = input0xad34e18();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::input0xad35180() {
   double input = 1.81267;
   input += synapse0xad35358();
   input += synapse0xad35380();
   input += synapse0xad353a8();
   input += synapse0xad353d0();
   input += synapse0xad353f8();
   input += synapse0xad35420();
   input += synapse0xad35448();
   input += synapse0xad35470();
   input += synapse0xad35498();
   input += synapse0xad354c0();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::neuron0xad35180() {
   double input = input0xad35180();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::input0xad35510() {
   double input = -1.22106;
   input += synapse0xad354e8();
   input += synapse0xad35770();
   input += synapse0xad35798();
   input += synapse0xad357c0();
   input += synapse0xad357e8();
   input += synapse0xad24720();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::neuron0xad35510() {
   double input = input0xad35510();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::input0xad34c70() {
   double input = 0.0249457;
   input += synapse0xabd1838();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::neuron0xad34c70() {
   double input = input0xad34c70();
   return (input * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad02af0() {
   return (neuron0xad24860()*-0.0161247);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad341a8() {
   return (neuron0xad24a10()*0.0092086);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad341d0() {
   return (neuron0xad32f78()*-0.0586873);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad341f8() {
   return (neuron0xad33150()*0.32462);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34220() {
   return (neuron0xad33348()*0.589804);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34248() {
   return (neuron0xad33540()*0.0678606);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34270() {
   return (neuron0xad33738()*-0.0348024);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34298() {
   return (neuron0xad33930()*-3.07584);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad342c0() {
   return (neuron0xad33b28()*0.000548206);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad342e8() {
   return (neuron0xad33d20()*8.77402);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad344e8() {
   return (neuron0xad24860()*2.72032);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34510() {
   return (neuron0xad24a10()*0.0916214);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34538() {
   return (neuron0xad32f78()*0.104138);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34560() {
   return (neuron0xad33150()*0.574662);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34588() {
   return (neuron0xad33348()*0.301812);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad345b0() {
   return (neuron0xad33540()*2.06195);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad345d8() {
   return (neuron0xad33738()*1.16976);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34688() {
   return (neuron0xad33930()*-0.392562);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad346b0() {
   return (neuron0xad33b28()*1.05134);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad346d8() {
   return (neuron0xad33d20()*0.0976389);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34890() {
   return (neuron0xad24860()*-0.0827215);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad348b8() {
   return (neuron0xad24a10()*-1.16321);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad348e0() {
   return (neuron0xad32f78()*0.260206);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34908() {
   return (neuron0xad33150()*-1.26428);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34930() {
   return (neuron0xad33348()*-0.824729);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34958() {
   return (neuron0xad33540()*0.224537);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34980() {
   return (neuron0xad33738()*0.221213);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad349a8() {
   return (neuron0xad33930()*1.86986);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad349d0() {
   return (neuron0xad33b28()*0.0158733);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad349f8() {
   return (neuron0xad33d20()*-6.20281);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34bf8() {
   return (neuron0xad24860()*-1.83056);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34c20() {
   return (neuron0xad24a10()*-0.560744);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34c48() {
   return (neuron0xad32f78()*-0.351955);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34600() {
   return (neuron0xad33150()*-0.0260483);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34628() {
   return (neuron0xad33348()*-0.379593);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34650() {
   return (neuron0xad33540()*-0.882262);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34d78() {
   return (neuron0xad33738()*-1.49693);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34da0() {
   return (neuron0xad33930()*0.0115191);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34dc8() {
   return (neuron0xad33b28()*-0.743092);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34df0() {
   return (neuron0xad33d20()*0.0545159);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad34ff0() {
   return (neuron0xad24860()*-0.747657);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad35018() {
   return (neuron0xad24a10()*-0.180551);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad35040() {
   return (neuron0xad32f78()*0.0336204);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad35068() {
   return (neuron0xad33150()*0.0827893);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad35090() {
   return (neuron0xad33348()*-0.146086);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad350b8() {
   return (neuron0xad33540()*-0.371001);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad350e0() {
   return (neuron0xad33738()*0.22027);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad35108() {
   return (neuron0xad33930()*-0.321206);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad35130() {
   return (neuron0xad33b28()*-0.440842);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad35158() {
   return (neuron0xad33d20()*-0.484479);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad35358() {
   return (neuron0xad24860()*-0.27145);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad35380() {
   return (neuron0xad24a10()*-0.0937007);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad353a8() {
   return (neuron0xad32f78()*0.378187);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad353d0() {
   return (neuron0xad33150()*0.844881);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad353f8() {
   return (neuron0xad33348()*0.316707);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad35420() {
   return (neuron0xad33540()*0.018378);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad35448() {
   return (neuron0xad33738()*-0.128633);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad35470() {
   return (neuron0xad33930()*0.307481);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad35498() {
   return (neuron0xad33b28()*-2.34051);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad354c0() {
   return (neuron0xad33d20()*-1.52242);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad354e8() {
   return (neuron0xad34018()*6.23478);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad35770() {
   return (neuron0xad34310()*-1.78403);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad35798() {
   return (neuron0xad34700()*-3.14448);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad357c0() {
   return (neuron0xad34a20()*0.592162);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad357e8() {
   return (neuron0xad34e18()*-0.067503);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xad24720() {
   return (neuron0xad35180()*-3.34933);
}

double MuGirlNS::AnnResults_tgcfix_barrel_good::synapse0xabd1838() {
   return (neuron0xad35510()*1.01317);
}

