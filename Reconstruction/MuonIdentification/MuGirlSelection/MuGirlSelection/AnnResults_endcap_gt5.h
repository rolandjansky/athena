/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANNRESULTS_ENDCAP_GT5_H
#define ANNRESULTS_ENDCAP_GT5_H

namespace MuGirlNS
{
class AnnResults_endcap_gt5 {
public:
   AnnResults_endcap_gt5() : m_input0(-999.), m_input1(-999.), m_input2(-999.), m_input3(-999.), m_input4(-999.), m_input5(-999.), m_input6(-999.),
                             m_input7(-999.), m_input8(-999.), m_input9(-999.) {}
   ~AnnResults_endcap_gt5() {}
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
   double neuron0xa97c700();
   double neuron0xa97c8b0();
   double neuron0xa97caa8();
   double neuron0xa97cca0();
   double neuron0xa97ce98();
   double neuron0xa97d090();
   double neuron0xa97d288();
   double neuron0xa97d480();
   double neuron0xa97d678();
   double neuron0xa97d870();
   double input0xa97db78();
   double neuron0xa97db78();
   double input0xa97de70();
   double neuron0xa97de70();
   double input0xa97e260();
   double neuron0xa97e260();
   double input0xa97e580();
   double neuron0xa97e580();
   double input0xa97e900();
   double neuron0xa97e900();
   double input0xa97ec68();
   double neuron0xa97ec68();
   double input0xa97eff8();
   double neuron0xa97eff8();
   double input0xa97e7d0();
   double neuron0xa97e7d0();
   double synapse0xa8bc218();
   double synapse0xa97dd08();
   double synapse0xa97dd30();
   double synapse0xa97dd58();
   double synapse0xa97dd80();
   double synapse0xa97dda8();
   double synapse0xa97ddd0();
   double synapse0xa97ddf8();
   double synapse0xa97de20();
   double synapse0xa97de48();
   double synapse0xa97e048();
   double synapse0xa97e070();
   double synapse0xa97e098();
   double synapse0xa97e0c0();
   double synapse0xa97e0e8();
   double synapse0xa97e110();
   double synapse0xa97e138();
   double synapse0xa97e1e8();
   double synapse0xa97e210();
   double synapse0xa97e238();
   double synapse0xa97e3f0();
   double synapse0xa97e418();
   double synapse0xa97e440();
   double synapse0xa97e468();
   double synapse0xa97e490();
   double synapse0xa97e4b8();
   double synapse0xa97e4e0();
   double synapse0xa97e508();
   double synapse0xa97e530();
   double synapse0xa97e558();
   double synapse0xa97e758();
   double synapse0xa97e780();
   double synapse0xa97e7a8();
   double synapse0xa8889c0();
   double synapse0xa8889e8();
   double synapse0xa888a10();
   double synapse0xa97e160();
   double synapse0xa97e188();
   double synapse0xa97e1b0();
   double synapse0xa97e8d8();
   double synapse0xa97ead8();
   double synapse0xa97eb00();
   double synapse0xa97eb28();
   double synapse0xa97eb50();
   double synapse0xa97eb78();
   double synapse0xa97eba0();
   double synapse0xa97ebc8();
   double synapse0xa97ebf0();
   double synapse0xa97ec18();
   double synapse0xa97ec40();
   double synapse0xa97ee40();
   double synapse0xa97ee68();
   double synapse0xa97ee90();
   double synapse0xa97eeb8();
   double synapse0xa97eee0();
   double synapse0xa97ef08();
   double synapse0xa97ef30();
   double synapse0xa97ef58();
   double synapse0xa97ef80();
   double synapse0xa97efa8();
   double synapse0xa97efd0();
   double synapse0xa97f258();
   double synapse0xa97f280();
   double synapse0xa97f2a8();
   double synapse0xa97f2d0();
   double synapse0xa8bc240();
   double synapse0xa888ee8();
};
}
#endif // AnnResults_endcap_gt5_h

