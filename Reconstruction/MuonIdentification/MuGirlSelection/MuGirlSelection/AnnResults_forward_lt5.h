/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANNRESULTS_FORWARD_LT5_H
#define ANNRESULTS_FORWARD_LT5_H

namespace MuGirlNS
{

class AnnResults_forward_lt5 {
public:
   AnnResults_forward_lt5() : m_input0(-999.), m_input1(-999.), m_input2(-999.), m_input3(-999.), m_input4(-999.), m_input5(-999.), m_input6(-999.),
                              m_input7(-999.), m_input8(-999.), m_input9(-999.) {}
   ~AnnResults_forward_lt5() {}
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
   double neuron0xa41e978();
   double neuron0xa42d020();
   double neuron0xa42d218();
   double neuron0xa42d410();
   double neuron0xa42d608();
   double neuron0xa42d800();
   double neuron0xa42d9f8();
   double neuron0xa42dbf0();
   double neuron0xa42dde8();
   double neuron0xa42dfe0();
   double input0xa42e2e8();
   double neuron0xa42e2e8();
   double input0xa42e5b8();
   double neuron0xa42e5b8();
   double input0xa42e9a8();
   double neuron0xa42e9a8();
   double input0xa42ecc8();
   double neuron0xa42ecc8();
   double input0xa42f020();
   double neuron0xa42f020();
   double input0xa42f388();
   double neuron0xa42f388();
   double input0xa42f718();
   double neuron0xa42f718();
   double input0xa42ef18();
   double neuron0xa42ef18();
   double synapse0xa41e798();
   double synapse0xa3fcb80();
   double synapse0xa42e478();
   double synapse0xa42e4a0();
   double synapse0xa42e4c8();
   double synapse0xa42e4f0();
   double synapse0xa42e518();
   double synapse0xa42e540();
   double synapse0xa42e568();
   double synapse0xa42e590();
   double synapse0xa42e790();
   double synapse0xa42e7b8();
   double synapse0xa42e7e0();
   double synapse0xa42e808();
   double synapse0xa42e830();
   double synapse0xa42e858();
   double synapse0xa42e880();
   double synapse0xa42e930();
   double synapse0xa42e958();
   double synapse0xa42e980();
   double synapse0xa42eb38();
   double synapse0xa42eb60();
   double synapse0xa42eb88();
   double synapse0xa42ebb0();
   double synapse0xa42ebd8();
   double synapse0xa42ec00();
   double synapse0xa42ec28();
   double synapse0xa42ec50();
   double synapse0xa42ec78();
   double synapse0xa42eca0();
   double synapse0xa42eea0();
   double synapse0xa42eec8();
   double synapse0xa42eef0();
   double synapse0xa37d2f8();
   double synapse0xa37d320();
   double synapse0xa37d0f8();
   double synapse0xa37d120();
   double synapse0xa37d148();
   double synapse0xa42e8a8();
   double synapse0xa42e8d0();
   double synapse0xa42f1f8();
   double synapse0xa42f220();
   double synapse0xa42f248();
   double synapse0xa42f270();
   double synapse0xa42f298();
   double synapse0xa42f2c0();
   double synapse0xa42f2e8();
   double synapse0xa42f310();
   double synapse0xa42f338();
   double synapse0xa42f360();
   double synapse0xa42f560();
   double synapse0xa42f588();
   double synapse0xa42f5b0();
   double synapse0xa42f5d8();
   double synapse0xa42f600();
   double synapse0xa42f628();
   double synapse0xa42f650();
   double synapse0xa42f678();
   double synapse0xa42f6a0();
   double synapse0xa42f6c8();
   double synapse0xa42f6f0();
   double synapse0xa42f978();
   double synapse0xa42f9a0();
   double synapse0xa42f9c8();
   double synapse0xa42f9f0();
   double synapse0xa37d498();
   double synapse0xa37d648();
};
}
#endif // AnnResults_forward_lt5_h

