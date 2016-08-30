/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLNSANNENDCAPLOWPT_H
#define MUGIRLNSANNENDCAPLOWPT_H

class AtlasDetectorID;
class Identifier;

namespace MuGirlNS
{

class AnnEndCapLowPt {
public:
   AnnEndCapLowPt() {}
   ~AnnEndCapLowPt() {}
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
   double neuron0BE8E650();
   double neuron0BE8DC70();
   double neuron0BDE6990();
   double neuron0BE37108();
   double neuron0BE37270();
   double neuron0BDB6008();
   double neuron0BDB6190();
   double neuron0DB7ECD0();
   double neuron0DB7EE38();
   double neuron0C4D8CC8();
   double neuron0C4D8EA8();
   double neuron0BE276F8();
   double neuron0BE9B918();
   double neuron0BE337A0();
   double neuron0BDE9188();
   double neuron0BE8E878();
   double neuron0BEAAF60();
   double synapse0C21E028();
   double synapse0C17DCF0();
   double synapse0BE90E98();
   double synapse0BE90E68();
   double synapse0BE90E38();
   double synapse0C4D8DE8();
   double synapse0C4D8E18();
   double synapse0C4D8E48();
   double synapse0C4D8E78();
   double synapse0C4D8FC8();
   double synapse0BE27578();
   double synapse0BE275A8();
   double synapse0BE275D8();
   double synapse0BE27608();
   double synapse0BE27638();
   double synapse0BE27668();
   double synapse0BE27698();
   double synapse0BE276C8();
   double synapse0BE27838();
   double synapse0BE9B798();
   double synapse0BE9B7C8();
   double synapse0BE9B7F8();
   double synapse0BE9B828();
   double synapse0BE9B858();
   double synapse0BE9B888();
   double synapse0BE9B8B8();
   double synapse0BE9B8E8();
   double synapse0BE9BAA0();
   double synapse0BE33620();
   double synapse0BE33650();
   double synapse0BE33680();
   double synapse0BE336B0();
   double synapse0BE336E0();
   double synapse0BE33710();
   double synapse0BE33740();
   double synapse0BE33770();
   double synapse0BE33928();
   double synapse0BDE9008();
   double synapse0BDE9038();
   double synapse0BDE9068();
   double synapse0BDE9098();
   double synapse0BDE90C8();
   double synapse0BDE90F8();
   double synapse0BDE9128();
   double synapse0BDE9158();
   double synapse0BDE92F0();
   double synapse0BDE9320();
   double synapse0BE8E728();
   double synapse0BE8E758();
   double synapse0BE8E788();
   double synapse0BE8E7B8();
   double synapse0BE8E7E8();
   double synapse0BE8E818();
   double synapse0BE8E848();
   double synapse0BE8E9E0();
   double synapse0BE8EA10();
   double synapse0BE8EA40();
   double synapse0BE8EA70();
   double synapse0BEAAF00();
   double synapse0BEAAF30();
   double synapse0BEAB0E8();
};
}
#endif

