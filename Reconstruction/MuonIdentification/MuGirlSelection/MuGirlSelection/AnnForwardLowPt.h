/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLNSANNFORWARDLOWPT_H
#define MUGIRLNSANNFORWARDLOWPT_H

class AtlasDetectorID;
class Identifier;

namespace MuGirlNS
{

class AnnForwardLowPt {
public:
   AnnForwardLowPt() {}
   ~AnnForwardLowPt() {}
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
   double neuron0C111ED8();
   double neuron0C196990();
   double neuron0C196AB0();
   double neuron0C196C18();
   double neuron0C085B00();
   double neuron0C085C68();
   double neuron0C085DD0();
   double neuron0C09E4C0();
   double neuron0C09E598();
   double neuron0C09E820();
   double neuron0C1328D0();
   double neuron0C132C08();
   double neuron0C0B6A88();
   double neuron0C0B6DC0();
   double neuron0C101818();
   double neuron0C101B50();
   double neuron0C12B100();
   double synapse0C09E988();
   double synapse0C09E9B8();
   double synapse0C09E9E8();
   double synapse0C09EA18();
   double synapse0C1327E0();
   double synapse0C132810();
   double synapse0C132840();
   double synapse0C132870();
   double synapse0C1328A0();
   double synapse0C132A58();
   double synapse0C132A88();
   double synapse0C132AB8();
   double synapse0C132AE8();
   double synapse0C132B18();
   double synapse0C132B48();
   double synapse0C132B78();
   double synapse0C132BA8();
   double synapse0C132BD8();
   double synapse0C132D48();
   double synapse0C132D78();
   double synapse0C132DA8();
   double synapse0C132DD8();
   double synapse0C0B6998();
   double synapse0C0B69C8();
   double synapse0C0B69F8();
   double synapse0C0B6A28();
   double synapse0C0B6A58();
   double synapse0C0B6C10();
   double synapse0C0B6C40();
   double synapse0C0B6C70();
   double synapse0C0B6CA0();
   double synapse0C0B6CD0();
   double synapse0C0B6D00();
   double synapse0C0B6D30();
   double synapse0C0B6D60();
   double synapse0C0B6D90();
   double synapse0C0B6F48();
   double synapse0C0B6F78();
   double synapse0C0B6FA8();
   double synapse0C0B6FD8();
   double synapse0C101728();
   double synapse0C101758();
   double synapse0C101788();
   double synapse0C1017B8();
   double synapse0C1017E8();
   double synapse0C1019A0();
   double synapse0C1019D0();
   double synapse0C101A00();
   double synapse0C101A30();
   double synapse0C101A60();
   double synapse0C101A90();
   double synapse0C101AC0();
   double synapse0C101AF0();
   double synapse0C101B20();
   double synapse0C101CD8();
   double synapse0C101D08();
   double synapse0C101D38();
   double synapse0C101D68();
   double synapse0C101D98();
   double synapse0C101DC8();
   double synapse0C101E18();
};
}
#endif

