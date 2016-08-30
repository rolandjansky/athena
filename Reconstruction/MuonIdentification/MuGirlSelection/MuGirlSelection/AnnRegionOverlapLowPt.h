/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLNSANNREGIONOVERLAPLOWPT_H
#define MUGIRLNSANNREGIONOVERLAPLOWPT_H

class AtlasDetectorID;
class Identifier;

namespace MuGirlNS
{

class AnnRegionOverlapLowPt {
public:
   AnnRegionOverlapLowPt() {}
   ~AnnRegionOverlapLowPt() {}
   double value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8);
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
   double neuron0C064EC8();
   double neuron0BFBDE18();
   double neuron0BFBF970();
   double neuron0BFBFA90();
   double neuron0BFBFBF8();
   double neuron0BFBFD60();
   double neuron0BFBFEE8();
   double neuron0BFF09B0();
   double neuron0BFF0B18();
   double neuron0BFF0DA0();
   double neuron0BFF78D0();
   double neuron0BFF7BE8();
   double neuron0BFC6068();
   double neuron0BFC6380();
   double neuron0BFC66B8();
   double neuron0BFC69F0();
   double neuron0BFC6C98();
   double synapse0BFF0F28();
   double synapse0BFF0F58();
   double synapse0BFF0F88();
   double synapse0BFF0FB8();
   double synapse0BFF77E0();
   double synapse0BFF7810();
   double synapse0BFF7840();
   double synapse0BFF7870();
   double synapse0BFF78A0();
   double synapse0BFF7A38();
   double synapse0BFF7A68();
   double synapse0BFF7A98();
   double synapse0BFF7AC8();
   double synapse0BFF7AF8();
   double synapse0BFF7B28();
   double synapse0BFF7B58();
   double synapse0BFF7B88();
   double synapse0BFF7BB8();
   double synapse0BFF7D28();
   double synapse0BFF7D58();
   double synapse0BFF7D88();
   double synapse0BFF7DB8();
   double synapse0BFF7DE8();
   double synapse0BFF7E18();
   double synapse0BFF7E48();
   double synapse0BFC6008();
   double synapse0BFC6038();
   double synapse0BFC61D0();
   double synapse0BFC6200();
   double synapse0BFC6230();
   double synapse0BFC6260();
   double synapse0BFC6290();
   double synapse0BFC62C0();
   double synapse0BFC62F0();
   double synapse0BFC6320();
   double synapse0BFC6350();
   double synapse0BFC6508();
   double synapse0BFC6538();
   double synapse0BFC6568();
   double synapse0BFC6598();
   double synapse0BFC65C8();
   double synapse0BFC65F8();
   double synapse0BFC6628();
   double synapse0BFC6658();
   double synapse0BFC6688();
   double synapse0BFC6840();
   double synapse0BFC6870();
   double synapse0BFC68A0();
   double synapse0BFC68D0();
   double synapse0BFC6900();
   double synapse0BFC6930();
   double synapse0BFC6960();
   double synapse0BFC6990();
   double synapse0BFC69C0();
   double synapse0BFC6B78();
   double synapse0BFC6BA8();
   double synapse0BFC6BD8();
   double synapse0BFC6C08();
   double synapse0BFC6C38();
   double synapse0BFC6C68();
   double synapse0BFC6E20();
};
}

#endif

