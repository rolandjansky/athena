/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANNRESULTS_FORWARD_GT5_H
#define ANNRESULTS_FORWARD_GT5_H

namespace MuGirlNS
{

class AnnResults_forward_gt5 {
public:
   AnnResults_forward_gt5() : m_input0(-999.), m_input1(-999.), m_input2(-999.), m_input3(-999.), m_input4(-999.), m_input5(-999.), m_input6(-999.),
                              m_input7(-999.), m_input8(-999.), m_input9(-999.) {}
   ~AnnResults_forward_gt5() {}
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
   double neuron0xb2a5640();
   double neuron0xb2b3c10();
   double neuron0xb2b3e08();
   double neuron0xb2b4000();
   double neuron0xb2b41f8();
   double neuron0xb2b43f0();
   double neuron0xb2b45e8();
   double neuron0xb2b47e0();
   double neuron0xb2b49d8();
   double neuron0xb2b4bd0();
   double input0xb2b4ed8();
   double neuron0xb2b4ed8();
   double input0xb2b51a8();
   double neuron0xb2b51a8();
   double input0xb2b5598();
   double neuron0xb2b5598();
   double input0xb2b58b8();
   double neuron0xb2b58b8();
   double input0xb2b5c10();
   double neuron0xb2b5c10();
   double input0xb2b5f78();
   double neuron0xb2b5f78();
   double input0xb2b6308();
   double neuron0xb2b6308();
   double input0xb2b5b08();
   double neuron0xb2b5b08();
   double synapse0xb2a5480();
   double synapse0xb283818();
   double synapse0xb2b5068();
   double synapse0xb2b5090();
   double synapse0xb2b50b8();
   double synapse0xb2b50e0();
   double synapse0xb2b5108();
   double synapse0xb2b5130();
   double synapse0xb2b5158();
   double synapse0xb2b5180();
   double synapse0xb2b5380();
   double synapse0xb2b53a8();
   double synapse0xb2b53d0();
   double synapse0xb2b53f8();
   double synapse0xb2b5420();
   double synapse0xb2b5448();
   double synapse0xb2b5470();
   double synapse0xb2b5520();
   double synapse0xb2b5548();
   double synapse0xb2b5570();
   double synapse0xb2b5728();
   double synapse0xb2b5750();
   double synapse0xb2b5778();
   double synapse0xb2b57a0();
   double synapse0xb2b57c8();
   double synapse0xb2b57f0();
   double synapse0xb2b5818();
   double synapse0xb2b5840();
   double synapse0xb2b5868();
   double synapse0xb2b5890();
   double synapse0xb2b5a90();
   double synapse0xb2b5ab8();
   double synapse0xb2b5ae0();
   double synapse0xb1ea0a0();
   double synapse0xb1ea0c8();
   double synapse0xb1e9ea0();
   double synapse0xb1e9ec8();
   double synapse0xb1e9ef0();
   double synapse0xb2b5498();
   double synapse0xb2b54c0();
   double synapse0xb2b5de8();
   double synapse0xb2b5e10();
   double synapse0xb2b5e38();
   double synapse0xb2b5e60();
   double synapse0xb2b5e88();
   double synapse0xb2b5eb0();
   double synapse0xb2b5ed8();
   double synapse0xb2b5f00();
   double synapse0xb2b5f28();
   double synapse0xb2b5f50();
   double synapse0xb2b6150();
   double synapse0xb2b6178();
   double synapse0xb2b61a0();
   double synapse0xb2b61c8();
   double synapse0xb2b61f0();
   double synapse0xb2b6218();
   double synapse0xb2b6240();
   double synapse0xb2b6268();
   double synapse0xb2b6290();
   double synapse0xb2b62b8();
   double synapse0xb2b62e0();
   double synapse0xb2b6568();
   double synapse0xb2b6590();
   double synapse0xb2b65b8();
   double synapse0xb2b65e0();
   double synapse0xb1ea240();
   double synapse0xb1ea3f0();
};
}
#endif // AnnResults_forward_gt5_h

