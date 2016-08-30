/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/AnnResults_tgcfix_barrel_endcap.h"
#include <cmath>

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9) {
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
         return neuron0x92e8120();
     default:
         return 0.;
   }
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::neuron0x92d7e90() {
   return m_input0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::neuron0x92d8068() {
   return m_input1;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::neuron0x92d8260() {
   return m_input2;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::neuron0x92e6680() {
   return m_input3;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::neuron0x92e6858() {
   return m_input4;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::neuron0x92e6a30() {
   return m_input5;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::neuron0x92e6c08() {
   return m_input6;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::neuron0x92e6de0() {
   return m_input7;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::neuron0x92e6fd8() {
   return m_input8;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::neuron0x92e71d0() {
   return m_input9;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::input0x92e74c8() {
   double input = -1.02681;
   input += synapse0x8fe4708();
   input += synapse0x92e7658();
   input += synapse0x92e7680();
   input += synapse0x92e76a8();
   input += synapse0x92e76d0();
   input += synapse0x92e76f8();
   input += synapse0x92e7720();
   input += synapse0x92e7748();
   input += synapse0x92e7770();
   input += synapse0x92e7798();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::neuron0x92e74c8() {
   double input = input0x92e74c8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::input0x92e77c0() {
   double input = 0.09313;
   input += synapse0x92e7998();
   input += synapse0x92e79c0();
   input += synapse0x92e79e8();
   input += synapse0x92e7a10();
   input += synapse0x92e7a38();
   input += synapse0x92e7a60();
   input += synapse0x92e7a88();
   input += synapse0x92e7b38();
   input += synapse0x92e7b60();
   input += synapse0x92e7b88();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::neuron0x92e77c0() {
   double input = input0x92e77c0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::input0x92e7bb0() {
   double input = -0.593413;
   input += synapse0x92e7d40();
   input += synapse0x92e7d68();
   input += synapse0x92e7d90();
   input += synapse0x92e7db8();
   input += synapse0x92e7de0();
   input += synapse0x92e7e08();
   input += synapse0x92e7e30();
   input += synapse0x92e7e58();
   input += synapse0x92e7e80();
   input += synapse0x92e7ea8();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::neuron0x92e7bb0() {
   double input = input0x92e7bb0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::input0x92e7ed0() {
   double input = -0.230025;
   input += synapse0x92e80a8();
   input += synapse0x92e80d0();
   input += synapse0x92e80f8();
   input += synapse0x92e7ab0();
   input += synapse0x92e7ad8();
   input += synapse0x92e7b00();
   input += synapse0x92e8228();
   input += synapse0x92e8250();
   input += synapse0x92e8278();
   input += synapse0x92e82a0();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::neuron0x92e7ed0() {
   double input = input0x92e7ed0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::input0x92e82c8() {
   double input = -0.515241;
   input += synapse0x92e84a0();
   input += synapse0x92e84c8();
   input += synapse0x92e84f0();
   input += synapse0x92e8518();
   input += synapse0x92e8540();
   input += synapse0x92e8568();
   input += synapse0x92e8590();
   input += synapse0x92e85b8();
   input += synapse0x92e85e0();
   input += synapse0x92e8608();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::neuron0x92e82c8() {
   double input = input0x92e82c8();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::input0x92e8630() {
   double input = -0.21741;
   input += synapse0x92e8808();
   input += synapse0x92e8830();
   input += synapse0x92e8858();
   input += synapse0x92e8880();
   input += synapse0x92e88a8();
   input += synapse0x92e88d0();
   input += synapse0x92e88f8();
   input += synapse0x92e8920();
   input += synapse0x92e8948();
   input += synapse0x92e8970();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::neuron0x92e8630() {
   double input = input0x92e8630();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::input0x92e89c0() {
   double input = -2.29253;
   input += synapse0x92e8998();
   input += synapse0x92e8c20();
   input += synapse0x92e8c48();
   input += synapse0x92e8c70();
   input += synapse0x92e8c98();
   input += synapse0x8fe46d8();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::neuron0x92e89c0() {
   double input = input0x92e89c0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::input0x92e8120() {
   double input = -0.0273398;
   input += synapse0x92e73f0();
   return input;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::neuron0x92e8120() {
   double input = input0x92e8120();
   return (input * 1)+0;
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x8fe4708() {
   return (neuron0x92d7e90()*0.0122067);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7658() {
   return (neuron0x92d8068()*6.08645);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7680() {
   return (neuron0x92d8260()*-2.94121);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e76a8() {
   return (neuron0x92e6680()*0.0742109);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e76d0() {
   return (neuron0x92e6858()*-1.40949);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e76f8() {
   return (neuron0x92e6a30()*0.0874522);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7720() {
   return (neuron0x92e6c08()*-0.0310115);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7748() {
   return (neuron0x92e6de0()*-3.85078);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7770() {
   return (neuron0x92e6fd8()*-0.00549704);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7798() {
   return (neuron0x92e71d0()*10.3);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7998() {
   return (neuron0x92d7e90()*-0.183478);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e79c0() {
   return (neuron0x92d8068()*-0.409587);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e79e8() {
   return (neuron0x92d8260()*0.514016);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7a10() {
   return (neuron0x92e6680()*-0.0449901);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7a38() {
   return (neuron0x92e6858()*0.202079);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7a60() {
   return (neuron0x92e6a30()*-0.700472);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7a88() {
   return (neuron0x92e6c08()*0.337438);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7b38() {
   return (neuron0x92e6de0()*0.115989);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7b60() {
   return (neuron0x92e6fd8()*-0.809089);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7b88() {
   return (neuron0x92e71d0()*-0.394034);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7d40() {
   return (neuron0x92d7e90()*0.827072);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7d68() {
   return (neuron0x92d8068()*-2.63998);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7d90() {
   return (neuron0x92d8260()*-0.589074);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7db8() {
   return (neuron0x92e6680()*-0.0821489);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7de0() {
   return (neuron0x92e6858()*-0.32805);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7e08() {
   return (neuron0x92e6a30()*1.75137);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7e30() {
   return (neuron0x92e6c08()*0.18173);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7e58() {
   return (neuron0x92e6de0()*-0.469942);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7e80() {
   return (neuron0x92e6fd8()*0.0212614);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7ea8() {
   return (neuron0x92e71d0()*0.904013);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e80a8() {
   return (neuron0x92d7e90()*0.645689);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e80d0() {
   return (neuron0x92d8068()*0.118943);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e80f8() {
   return (neuron0x92d8260()*0.487758);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7ab0() {
   return (neuron0x92e6680()*0.100212);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7ad8() {
   return (neuron0x92e6858()*0.168796);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e7b00() {
   return (neuron0x92e6a30()*0.46807);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8228() {
   return (neuron0x92e6c08()*0.280404);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8250() {
   return (neuron0x92e6de0()*-0.0118906);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8278() {
   return (neuron0x92e6fd8()*0.382559);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e82a0() {
   return (neuron0x92e71d0()*-0.401109);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e84a0() {
   return (neuron0x92d7e90()*-0.0131894);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e84c8() {
   return (neuron0x92d8068()*-0.0636924);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e84f0() {
   return (neuron0x92d8260()*0.00408371);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8518() {
   return (neuron0x92e6680()*0.427624);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8540() {
   return (neuron0x92e6858()*0.0153154);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8568() {
   return (neuron0x92e6a30()*-0.0101253);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8590() {
   return (neuron0x92e6c08()*0.061929);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e85b8() {
   return (neuron0x92e6de0()*-3.58087);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e85e0() {
   return (neuron0x92e6fd8()*0.000141123);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8608() {
   return (neuron0x92e71d0()*7.64682);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8808() {
   return (neuron0x92d7e90()*0.670156);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8830() {
   return (neuron0x92d8068()*0.12833);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8858() {
   return (neuron0x92d8260()*0.29076);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8880() {
   return (neuron0x92e6680()*-0.276209);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e88a8() {
   return (neuron0x92e6858()*-0.333541);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e88d0() {
   return (neuron0x92e6a30()*0.461733);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e88f8() {
   return (neuron0x92e6c08()*0.397181);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8920() {
   return (neuron0x92e6de0()*-0.314542);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8948() {
   return (neuron0x92e6fd8()*0.211592);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8970() {
   return (neuron0x92e71d0()*-0.457647);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8998() {
   return (neuron0x92e74c8()*3.25984);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8c20() {
   return (neuron0x92e77c0()*-2.1038);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8c48() {
   return (neuron0x92e7bb0()*1.47566);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8c70() {
   return (neuron0x92e7ed0()*-1.96258);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e8c98() {
   return (neuron0x92e82c8()*7.68049);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x8fe46d8() {
   return (neuron0x92e8630()*-2.6009);
}

double MuGirlNS::AnnResults_tgcfix_barrel_endcap::synapse0x92e73f0() {
   return (neuron0x92e89c0()*1.02821);
}

