/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuGirlSelection/test_barrel.h"
#include "math.h"

double MuGirlNS::test_barrel::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6) {
   m_input0 = (in0 - 0)/1;
   m_input1 = (in1 - 0)/1;
   m_input2 = (in2 - 0)/1;
   m_input3 = (in3 - 0)/1;
   m_input4 = (in4 - 0)/1;
   m_input5 = (in5 - 0)/1;
   m_input6 = (in6 - 0)/1;
   switch(index) {
     case 0:
         return ((neuron09EE5BE8()*1)+0);
     default:
         return 0.;
   }
}

double MuGirlNS::test_barrel::neuron09FAFC50() {
   return m_input0;
}

double MuGirlNS::test_barrel::neuron09FAFD70() {
   return m_input1;
}

double MuGirlNS::test_barrel::neuron09FAFED8() {
   return m_input2;
}

double MuGirlNS::test_barrel::neuron09CFD718() {
   return m_input3;
}

double MuGirlNS::test_barrel::neuron09CFD880() {
   return m_input4;
}

double MuGirlNS::test_barrel::neuron09CFD9E8() {
   return m_input5;
}

double MuGirlNS::test_barrel::neuron09CFDB50() {
   return m_input6;
}

double MuGirlNS::test_barrel::neuron09F50698() {
   double input = 0.528174;
   input += synapse09E986D0();
   input += synapse082DEA40();
   input += synapse09EE7050();
   input += synapse082DE648();
   input += synapse0835BCF0();
   input += synapse0835BB38();
   input += synapse0835B080();
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::test_barrel::neuron09F507B8() {
   double input = -0.0424666;
   input += synapse09C2B118();
   input += synapse09E9BA18();
   input += synapse0835B8A8();
   input += synapse09EBEA18();
   input += synapse081A61B8();
   input += synapse081A6098();
   input += synapse081A5F98();
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::test_barrel::neuron09F50920() {
   double input = 0.980374;
   input += synapse09EBE0E8();
   input += synapse09EBDC40();
   input += synapse09EBDCE8();
   input += synapse09F50A88();
   input += synapse09F50AB8();
   input += synapse09F50AE8();
   input += synapse09F50B18();
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::test_barrel::neuron09F50B48() {
   double input = -0.0183378;
   input += synapse09F50C68();
   input += synapse09F50C98();
   input += synapse09F50CC8();
   input += synapse09F50CF8();
   input += synapse09F50D28();
   input += synapse09F50D58();
   input += synapse09F50D88();
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::test_barrel::neuron09F50DB8() {
   double input = 0.946593;
   input += synapse09F50F20();
   input += synapse09F50F50();
   input += synapse09F50F80();
   input += synapse09F50FB0();
   input += synapse09EE5618();
   input += synapse09EE5648();
   input += synapse09EE5678();
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::test_barrel::neuron09EE56A8() {
   double input = 2.43425;
   input += synapse09EE5810();
   input += synapse09EE5840();
   input += synapse09EE5870();
   input += synapse09EE58A0();
   input += synapse09EE58D0();
   input += synapse09EE5900();
   input += synapse09EE5930();
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::test_barrel::neuron09EE5960() {
   double input = -3.54065;
   input += synapse09EE5AC8();
   input += synapse09EE5AF8();
   input += synapse09EE5B28();
   input += synapse09EE5B58();
   input += synapse09EE5B88();
   input += synapse09EE5BB8();
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::test_barrel::neuron09EE5BE8() {
   double input = 0.022945;
   input += synapse09EE5D50();
   return input;
}

double MuGirlNS::test_barrel::synapse09E986D0() {
   return (neuron09FAFC50()*-1.06454);
}

double MuGirlNS::test_barrel::synapse082DEA40() {
   return (neuron09FAFD70()*-0.49213);
}

double MuGirlNS::test_barrel::synapse09EE7050() {
   return (neuron09FAFED8()*-1.05182);
}

double MuGirlNS::test_barrel::synapse082DE648() {
   return (neuron09CFD718()*0.46885);
}

double MuGirlNS::test_barrel::synapse0835BCF0() {
   return (neuron09CFD880()*-0.102444);
}

double MuGirlNS::test_barrel::synapse0835BB38() {
   return (neuron09CFD9E8()*0.60509);
}

double MuGirlNS::test_barrel::synapse0835B080() {
   return (neuron09CFDB50()*-0.186763);
}

double MuGirlNS::test_barrel::synapse09C2B118() {
   return (neuron09FAFC50()*1.16985);
}

double MuGirlNS::test_barrel::synapse09E9BA18() {
   return (neuron09FAFD70()*0.369734);
}

double MuGirlNS::test_barrel::synapse0835B8A8() {
   return (neuron09FAFED8()*0.195828);
}

double MuGirlNS::test_barrel::synapse09EBEA18() {
   return (neuron09CFD718()*-0.173203);
}

double MuGirlNS::test_barrel::synapse081A61B8() {
   return (neuron09CFD880()*-0.236311);
}

double MuGirlNS::test_barrel::synapse081A6098() {
   return (neuron09CFD9E8()*0.281883);
}

double MuGirlNS::test_barrel::synapse081A5F98() {
   return (neuron09CFDB50()*0.415947);
}

double MuGirlNS::test_barrel::synapse09EBE0E8() {
   return (neuron09FAFC50()*-0.128186);
}

double MuGirlNS::test_barrel::synapse09EBDC40() {
   return (neuron09FAFD70()*-0.258945);
}

double MuGirlNS::test_barrel::synapse09EBDCE8() {
   return (neuron09FAFED8()*1.5016);
}

double MuGirlNS::test_barrel::synapse09F50A88() {
   return (neuron09CFD718()*-0.149238);
}

double MuGirlNS::test_barrel::synapse09F50AB8() {
   return (neuron09CFD880()*0.0680185);
}

double MuGirlNS::test_barrel::synapse09F50AE8() {
   return (neuron09CFD9E8()*0.0839475);
}

double MuGirlNS::test_barrel::synapse09F50B18() {
   return (neuron09CFDB50()*-1.844);
}

double MuGirlNS::test_barrel::synapse09F50C68() {
   return (neuron09FAFC50()*-1.00046);
}

double MuGirlNS::test_barrel::synapse09F50C98() {
   return (neuron09FAFD70()*0.978925);
}

double MuGirlNS::test_barrel::synapse09F50CC8() {
   return (neuron09FAFED8()*-1.41529);
}

double MuGirlNS::test_barrel::synapse09F50CF8() {
   return (neuron09CFD718()*0.382288);
}

double MuGirlNS::test_barrel::synapse09F50D28() {
   return (neuron09CFD880()*-0.238889);
}

double MuGirlNS::test_barrel::synapse09F50D58() {
   return (neuron09CFD9E8()*-1.00629);
}

double MuGirlNS::test_barrel::synapse09F50D88() {
   return (neuron09CFDB50()*0.854151);
}

double MuGirlNS::test_barrel::synapse09F50F20() {
   return (neuron09FAFC50()*-1.90433);
}

double MuGirlNS::test_barrel::synapse09F50F50() {
   return (neuron09FAFD70()*0.0149389);
}

double MuGirlNS::test_barrel::synapse09F50F80() {
   return (neuron09FAFED8()*-1.08126);
}

double MuGirlNS::test_barrel::synapse09F50FB0() {
   return (neuron09CFD718()*-0.115777);
}

double MuGirlNS::test_barrel::synapse09EE5618() {
   return (neuron09CFD880()*-3.41185);
}

double MuGirlNS::test_barrel::synapse09EE5648() {
   return (neuron09CFD9E8()*4.80456);
}

double MuGirlNS::test_barrel::synapse09EE5678() {
   return (neuron09CFDB50()*1.92175);
}

double MuGirlNS::test_barrel::synapse09EE5810() {
   return (neuron09FAFC50()*-0.166945);
}

double MuGirlNS::test_barrel::synapse09EE5840() {
   return (neuron09FAFD70()*-0.0599336);
}

double MuGirlNS::test_barrel::synapse09EE5870() {
   return (neuron09FAFED8()*0.131673);
}

double MuGirlNS::test_barrel::synapse09EE58A0() {
   return (neuron09CFD718()*0.258485);
}

double MuGirlNS::test_barrel::synapse09EE58D0() {
   return (neuron09CFD880()*4.88331);
}

double MuGirlNS::test_barrel::synapse09EE5900() {
   return (neuron09CFD9E8()*-0.330691);
}

double MuGirlNS::test_barrel::synapse09EE5930() {
   return (neuron09CFDB50()*-0.776003);
}

double MuGirlNS::test_barrel::synapse09EE5AC8() {
   return (neuron09F50698()*-1.06576);
}

double MuGirlNS::test_barrel::synapse09EE5AF8() {
   return (neuron09F507B8()*-2.03241);
}

double MuGirlNS::test_barrel::synapse09EE5B28() {
   return (neuron09F50920()*-2.98167);
}

double MuGirlNS::test_barrel::synapse09EE5B58() {
   return (neuron09F50B48()*-2.95125);
}

double MuGirlNS::test_barrel::synapse09EE5B88() {
   return (neuron09F50DB8()*4.84219);
}

double MuGirlNS::test_barrel::synapse09EE5BB8() {
   return (neuron09EE56A8()*-2.573);
}

double MuGirlNS::test_barrel::synapse09EE5D50() {
   return (neuron09EE5960()*2.95474);
}

