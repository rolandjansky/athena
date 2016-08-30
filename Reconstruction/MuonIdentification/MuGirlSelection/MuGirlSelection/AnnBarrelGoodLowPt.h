/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AnnBarrelGoodLowPt.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUGIRLNSANNBARRELGOODLOWPT_H
#define MUGIRLNSANNBARRELGOODLOWPT_H

class AtlasDetectorID;
class Identifier;

namespace MuGirlNS
{

  class AnnBarrelGoodLowPt
    {
    public:
      AnnBarrelGoodLowPt() {};
      virtual ~AnnBarrelGoodLowPt () {};

      virtual double value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8);

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
   double neuron0BDE51C0();
   double neuron0DB2E240();
   double neuron0DB9CEE0();
   double neuron0DB647E0();
   double neuron0DB64948();
   double neuron0DB64AB0();
   double neuron0DB64C18();
   double neuron0C1DE7E0();
   double neuron0C1DE8B8();
   double neuron0C1DEA20();
   double neuron0C1DED28();
   double neuron0DBA3A20();
   double neuron0DBA3CF0();
   double neuron0DBA4998();
   double neuron0DBA4CD0();
   double neuron0DE5F3C8();
   double neuron0DE5F670();
   double synapse0DB4C178();
   double synapse0C1DEBA8();
   double synapse0C1DEBD8();
   double synapse0C1DEC08();
   double synapse0C1DEC38();
   double synapse0C1DEC68();
   double synapse0C1DEC98();
   double synapse0C1DECC8();
   double synapse0C1DECF8();
   double synapse0DBA3870();
   double synapse0DBA38A0();
   double synapse0DBA38D0();
   double synapse0DBA3900();
   double synapse0DBA3930();
   double synapse0DBA3960();
   double synapse0DBA3990();
   double synapse0DBA39C0();
   double synapse0DBA39F0();
   double synapse0DBA3B40();
   double synapse0DBA3B70();
   double synapse0DBA3BA0();
   double synapse0DBA3BD0();
   double synapse0DBA3C00();
   double synapse0DBA3C30();
   double synapse0DBA3C60();
   double synapse0DBA3C90();
   double synapse0DBA3CC0();
   double synapse0DBA3E78();
   double synapse0DBA3EA8();
   double synapse0DBA3ED8();
   double synapse0DBA4878();
   double synapse0DBA48A8();
   double synapse0DBA48D8();
   double synapse0DBA4908();
   double synapse0DBA4938();
   double synapse0DBA4968();
   double synapse0DBA4B20();
   double synapse0DBA4B50();
   double synapse0DBA4B80();
   double synapse0DBA4BB0();
   double synapse0DBA4BE0();
   double synapse0DBA4C10();
   double synapse0DBA4C40();
   double synapse0DBA4C70();
   double synapse0DBA4CA0();
   double synapse0DBA4E58();
   double synapse0DBA4E88();
   double synapse0DBA4EB8();
   double synapse0DBA4EE8();
   double synapse0DBA4F18();
   double synapse0DBA4F48();
   double synapse0DBA4F78();
   double synapse0DBA4FA8();
   double synapse0DBA4FD8();
   double synapse0DE5F550();
   double synapse0DE5F580();
   double synapse0DE5F5B0();
   double synapse0DE5F5E0();
   double synapse0DE5F610();
   double synapse0DE5F640();
   double synapse0DE5F7F8();
    };
} // end of namespace

#endif
