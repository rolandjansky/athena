/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/test_endcap.h"
#include "math.h"

double MuGirlNS::test_endcap::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7) {
   m_input0 = (in0 - 0)/1;
   m_input1 = (in1 - 0)/1;
   m_input2 = (in2 - 0)/1;
   m_input3 = (in3 - 0)/1;
   m_input4 = (in4 - 0)/1;
   m_input5 = (in5 - 0)/1;
   m_input6 = (in6 - 0)/1;
   m_input7 = (in7 - 0)/1;
   switch(index) {
     case 0:
         return ((neuron08309EA8()*1)+0);
     default:
         return 0.;
   }
}

double MuGirlNS::test_endcap::neuron0A0A7EB8() {
   return m_input0;
}

double MuGirlNS::test_endcap::neuron09F384F0() {
   return m_input1;
}

double MuGirlNS::test_endcap::neuron09F38610() {
   return m_input2;
}

double MuGirlNS::test_endcap::neuron08027FB8() {
   return m_input3;
}

double MuGirlNS::test_endcap::neuron080280D8() {
   return m_input4;
}

double MuGirlNS::test_endcap::neuron08028240() {
   return m_input5;
}

double MuGirlNS::test_endcap::neuron080283A8() {
   return m_input6;
}

double MuGirlNS::test_endcap::neuron0A0DEAB0() {
   return m_input7;
}

double MuGirlNS::test_endcap::neuron0A0DEC18() {
   double input = -0.276757;
   input += synapse09F387C0();
   input += synapse0A0A7FD8();
   input += synapse08028960();
   input += synapse08318130();
   input += synapse09F2BD48();
   input += synapse0A024E90();
   input += synapse0A014E50();
   input += synapse0834F9B0();
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::test_endcap::neuron0A0DED80() {
   double input = 1.75344;
   input += synapse0A00BDE0();
   input += synapse09FF15C0();
   input += synapse07ECAE28();
   input += synapse08028638();
   input += synapse0A052D50();
   input += synapse08068FC8();
   input += synapse09FF1BE0();
   input += synapse0812A790();
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::test_endcap::neuron0A0DEEE8() {
   double input = -3.21899;
   input += synapse0A042E40();
   input += synapse0A042EE8();
   input += synapse0A03AE80();
   input += synapse0A02CE40();
   input += synapse0A02CEE8();
   input += synapse0A014EC0();
   input += synapse0A1A8828();
   input += synapse0A1A8858();
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::test_endcap::neuron0A1A8888() {
   double input = 0.0548879;
   input += synapse0A1A89C8();
   input += synapse0A1A89F8();
   input += synapse0A1A8A28();
   input += synapse0A1A8A58();
   input += synapse0A1A8A88();
   input += synapse0A1A8AB8();
   input += synapse0A1A8AE8();
   input += synapse0A1A8B18();
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::test_endcap::neuron0A1A8B48() {
   double input = -0.421769;
   input += synapse0A1A8CD0();
   input += synapse0A1A8D00();
   input += synapse0A1A8D30();
   input += synapse0A1A8D60();
   input += synapse0A1A8D90();
   input += synapse0A1A8DC0();
   input += synapse0A1A8DF0();
   input += synapse083098C8();
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::test_endcap::neuron083098F8() {
   double input = -1.05155;
   input += synapse08309A80();
   input += synapse08309AB0();
   input += synapse08309AE0();
   input += synapse08309B10();
   input += synapse08309B40();
   input += synapse08309B70();
   input += synapse08309BA0();
   input += synapse08309BD0();
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::test_endcap::neuron08309C00() {
   double input = -2.80572;
   input += synapse08309D88();
   input += synapse08309DB8();
   input += synapse08309DE8();
   input += synapse08309E18();
   input += synapse08309E48();
   input += synapse08309E78();
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::test_endcap::neuron08309EA8() {
   double input = 0.00879196;
   input += synapse09F2A470();
   return input;
}

double MuGirlNS::test_endcap::synapse09F387C0() {
   return (neuron0A0A7EB8()*0.051643);
}

double MuGirlNS::test_endcap::synapse0A0A7FD8() {
   return (neuron09F384F0()*0.0256409);
}

double MuGirlNS::test_endcap::synapse08028960() {
   return (neuron09F38610()*0.193592);
}

double MuGirlNS::test_endcap::synapse08318130() {
   return (neuron08027FB8()*-0.204353);
}

double MuGirlNS::test_endcap::synapse09F2BD48() {
   return (neuron080280D8()*0.141989);
}

double MuGirlNS::test_endcap::synapse0A024E90() {
   return (neuron08028240()*-0.107669);
}

double MuGirlNS::test_endcap::synapse0A014E50() {
   return (neuron080283A8()*-0.196949);
}

double MuGirlNS::test_endcap::synapse0834F9B0() {
   return (neuron0A0DEAB0()*-0.398509);
}

double MuGirlNS::test_endcap::synapse0A00BDE0() {
   return (neuron0A0A7EB8()*0.273462);
}

double MuGirlNS::test_endcap::synapse09FF15C0() {
   return (neuron09F384F0()*0.494256);
}

double MuGirlNS::test_endcap::synapse07ECAE28() {
   return (neuron09F38610()*-1.09012);
}

double MuGirlNS::test_endcap::synapse08028638() {
   return (neuron08027FB8()*0.338448);
}

double MuGirlNS::test_endcap::synapse0A052D50() {
   return (neuron080280D8()*0.736825);
}

double MuGirlNS::test_endcap::synapse08068FC8() {
   return (neuron08028240()*-0.164232);
}

double MuGirlNS::test_endcap::synapse09FF1BE0() {
   return (neuron080283A8()*-0.579881);
}

double MuGirlNS::test_endcap::synapse0812A790() {
   return (neuron0A0DEAB0()*0.1394);
}

double MuGirlNS::test_endcap::synapse0A042E40() {
   return (neuron0A0A7EB8()*0.454094);
}

double MuGirlNS::test_endcap::synapse0A042EE8() {
   return (neuron09F384F0()*-0.00127459);
}

double MuGirlNS::test_endcap::synapse0A03AE80() {
   return (neuron09F38610()*0.142487);
}

double MuGirlNS::test_endcap::synapse0A02CE40() {
   return (neuron08027FB8()*-0.732526);
}

double MuGirlNS::test_endcap::synapse0A02CEE8() {
   return (neuron080280D8()*1.99908);
}

double MuGirlNS::test_endcap::synapse0A014EC0() {
   return (neuron08028240()*0.0266314);
}

double MuGirlNS::test_endcap::synapse0A1A8828() {
   return (neuron080283A8()*-0.106015);
}

double MuGirlNS::test_endcap::synapse0A1A8858() {
   return (neuron0A0DEAB0()*-2.53762);
}

double MuGirlNS::test_endcap::synapse0A1A89C8() {
   return (neuron0A0A7EB8()*-0.516762);
}

double MuGirlNS::test_endcap::synapse0A1A89F8() {
   return (neuron09F384F0()*-0.372972);
}

double MuGirlNS::test_endcap::synapse0A1A8A28() {
   return (neuron09F38610()*-0.242599);
}

double MuGirlNS::test_endcap::synapse0A1A8A58() {
   return (neuron08027FB8()*-0.248692);
}

double MuGirlNS::test_endcap::synapse0A1A8A88() {
   return (neuron080280D8()*0.477753);
}

double MuGirlNS::test_endcap::synapse0A1A8AB8() {
   return (neuron08028240()*-0.125501);
}

double MuGirlNS::test_endcap::synapse0A1A8AE8() {
   return (neuron080283A8()*-0.00958739);
}

double MuGirlNS::test_endcap::synapse0A1A8B18() {
   return (neuron0A0DEAB0()*-0.0513397);
}

double MuGirlNS::test_endcap::synapse0A1A8CD0() {
   return (neuron0A0A7EB8()*0.267375);
}

double MuGirlNS::test_endcap::synapse0A1A8D00() {
   return (neuron09F384F0()*-1.52057);
}

double MuGirlNS::test_endcap::synapse0A1A8D30() {
   return (neuron09F38610()*-1.95305);
}

double MuGirlNS::test_endcap::synapse0A1A8D60() {
   return (neuron08027FB8()*0.32936);
}

double MuGirlNS::test_endcap::synapse0A1A8D90() {
   return (neuron080280D8()*0.0724725);
}

double MuGirlNS::test_endcap::synapse0A1A8DC0() {
   return (neuron08028240()*0.107776);
}

double MuGirlNS::test_endcap::synapse0A1A8DF0() {
   return (neuron080283A8()*-0.668082);
}

double MuGirlNS::test_endcap::synapse083098C8() {
   return (neuron0A0DEAB0()*-0.0654198);
}

double MuGirlNS::test_endcap::synapse08309A80() {
   return (neuron0A0A7EB8()*0.341193);
}

double MuGirlNS::test_endcap::synapse08309AB0() {
   return (neuron09F384F0()*0.988105);
}

double MuGirlNS::test_endcap::synapse08309AE0() {
   return (neuron09F38610()*-0.0922375);
}

double MuGirlNS::test_endcap::synapse08309B10() {
   return (neuron08027FB8()*-0.858843);
}

double MuGirlNS::test_endcap::synapse08309B40() {
   return (neuron080280D8()*0.245905);
}

double MuGirlNS::test_endcap::synapse08309B70() {
   return (neuron08028240()*-0.84272);
}

double MuGirlNS::test_endcap::synapse08309BA0() {
   return (neuron080283A8()*-0.490458);
}

double MuGirlNS::test_endcap::synapse08309BD0() {
   return (neuron0A0DEAB0()*-0.391158);
}

double MuGirlNS::test_endcap::synapse08309D88() {
   return (neuron0A0DEC18()*-3.17558);
}

double MuGirlNS::test_endcap::synapse08309DB8() {
   return (neuron0A0DED80()*-0.945348);
}

double MuGirlNS::test_endcap::synapse08309DE8() {
   return (neuron0A0DEEE8()*3.34826);
}

double MuGirlNS::test_endcap::synapse08309E18() {
   return (neuron0A1A8888()*0.268172);
}

double MuGirlNS::test_endcap::synapse08309E48() {
   return (neuron0A1A8B48()*1.19759);
}

double MuGirlNS::test_endcap::synapse08309E78() {
   return (neuron083098F8()*0.46225);
}

double MuGirlNS::test_endcap::synapse09F2A470() {
   return (neuron08309C00()*1.5532);
}

