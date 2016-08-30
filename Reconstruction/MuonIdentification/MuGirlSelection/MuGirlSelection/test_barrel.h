/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLNSTEST_BARREL_H
#define MUGIRLNSTEST_BARREL_H

class AtlasDetectorID;
class Identifier;

namespace MuGirlNS
{
class test_barrel {
public:
   test_barrel() : m_input0(-999.), m_input1(-999.), m_input2(-999.), m_input3(-999.), m_input4(-999.), m_input5(-999.), m_input6(-999.) {}
   ~test_barrel() {}
   double value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6);
private:
   double m_input0;
   double m_input1;
   double m_input2;
   double m_input3;
   double m_input4;
   double m_input5;
   double m_input6;
   double neuron09FAFC50();
   double neuron09FAFD70();
   double neuron09FAFED8();
   double neuron09CFD718();
   double neuron09CFD880();
   double neuron09CFD9E8();
   double neuron09CFDB50();
   double neuron09F50698();
   double neuron09F507B8();
   double neuron09F50920();
   double neuron09F50B48();
   double neuron09F50DB8();
   double neuron09EE56A8();
   double neuron09EE5960();
   double neuron09EE5BE8();
   double synapse09E986D0();
   double synapse082DEA40();
   double synapse09EE7050();
   double synapse082DE648();
   double synapse0835BCF0();
   double synapse0835BB38();
   double synapse0835B080();
   double synapse09C2B118();
   double synapse09E9BA18();
   double synapse0835B8A8();
   double synapse09EBEA18();
   double synapse081A61B8();
   double synapse081A6098();
   double synapse081A5F98();
   double synapse09EBE0E8();
   double synapse09EBDC40();
   double synapse09EBDCE8();
   double synapse09F50A88();
   double synapse09F50AB8();
   double synapse09F50AE8();
   double synapse09F50B18();
   double synapse09F50C68();
   double synapse09F50C98();
   double synapse09F50CC8();
   double synapse09F50CF8();
   double synapse09F50D28();
   double synapse09F50D58();
   double synapse09F50D88();
   double synapse09F50F20();
   double synapse09F50F50();
   double synapse09F50F80();
   double synapse09F50FB0();
   double synapse09EE5618();
   double synapse09EE5648();
   double synapse09EE5678();
   double synapse09EE5810();
   double synapse09EE5840();
   double synapse09EE5870();
   double synapse09EE58A0();
   double synapse09EE58D0();
   double synapse09EE5900();
   double synapse09EE5930();
   double synapse09EE5AC8();
   double synapse09EE5AF8();
   double synapse09EE5B28();
   double synapse09EE5B58();
   double synapse09EE5B88();
   double synapse09EE5BB8();
   double synapse09EE5D50();
};
}
#endif

