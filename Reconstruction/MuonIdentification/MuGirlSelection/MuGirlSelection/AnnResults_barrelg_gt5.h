/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANNRESULTS_BARRELG_GT5_H
#define ANNRESULTS_BARRELG_GT5_H

namespace MuGirlNS
{


class AnnResults_barrelg_gt5 { 
public:
   AnnResults_barrelg_gt5() {}
   ~AnnResults_barrelg_gt5() {}
   double value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8,double in9);
   double value(int index, double* input);
private:
   double m_input0;
   double m_input1;
   double m_input2;
   double m_input3;
   double m_input4;
   double m_input5;
   double m_input6;
   double m_input7;
   double m_input8;
   double m_input9;
   double neuron0x98b49c0();
   double neuron0x98c3088();
   double neuron0x98c3280();
   double neuron0x98c3478();
   double neuron0x98c3670();
   double neuron0x98c3868();
   double neuron0x98c3a60();
   double neuron0x98c3c58();
   double neuron0x98c3e50();
   double neuron0x98c4048();
   double input0x98c4350();
   double neuron0x98c4350();
   double input0x98c4648();
   double neuron0x98c4648();
   double input0x98c4a38();
   double neuron0x98c4a38();
   double input0x98c4d58();
   double neuron0x98c4d58();
   double input0x98c50b0();
   double neuron0x98c50b0();
   double input0x98c5418();
   double neuron0x98c5418();
   double input0x98c57a8();
   double neuron0x98c57a8();
   double input0x98c4fa8();
   double neuron0x98c4fa8();
   double synapse0x98b4800();
   double synapse0x98c44e0();
   double synapse0x98c4508();
   double synapse0x98c4530();
   double synapse0x98c4558();
   double synapse0x98c4580();
   double synapse0x98c45a8();
   double synapse0x98c45d0();
   double synapse0x98c45f8();
   double synapse0x98c4620();
   double synapse0x98c4820();
   double synapse0x98c4848();
   double synapse0x98c4870();
   double synapse0x98c4898();
   double synapse0x98c48c0();
   double synapse0x98c48e8();
   double synapse0x98c4910();
   double synapse0x98c49c0();
   double synapse0x98c49e8();
   double synapse0x98c4a10();
   double synapse0x98c4bc8();
   double synapse0x98c4bf0();
   double synapse0x98c4c18();
   double synapse0x98c4c40();
   double synapse0x98c4c68();
   double synapse0x98c4c90();
   double synapse0x98c4cb8();
   double synapse0x98c4ce0();
   double synapse0x98c4d08();
   double synapse0x98c4d30();
   double synapse0x98c4f30();
   double synapse0x98c4f58();
   double synapse0x98c4f80();
   double synapse0x9850c38();
   double synapse0x9850c60();
   double synapse0x9850a38();
   double synapse0x9850a60();
   double synapse0x9850a88();
   double synapse0x98c4938();
   double synapse0x98c4960();
   double synapse0x98c5288();
   double synapse0x98c52b0();
   double synapse0x98c52d8();
   double synapse0x98c5300();
   double synapse0x98c5328();
   double synapse0x98c5350();
   double synapse0x98c5378();
   double synapse0x98c53a0();
   double synapse0x98c53c8();
   double synapse0x98c53f0();
   double synapse0x98c55f0();
   double synapse0x98c5618();
   double synapse0x98c5640();
   double synapse0x98c5668();
   double synapse0x98c5690();
   double synapse0x98c56b8();
   double synapse0x98c56e0();
   double synapse0x98c5708();
   double synapse0x98c5730();
   double synapse0x98c5758();
   double synapse0x98c5780();
   double synapse0x98c5a08();
   double synapse0x98c5a30();
   double synapse0x98c5a58();
   double synapse0x98c5a80();
   double synapse0x9850d60();
   double synapse0x9850fa8();
};
}
#endif // AnnResults_barrelg_gt5_h

