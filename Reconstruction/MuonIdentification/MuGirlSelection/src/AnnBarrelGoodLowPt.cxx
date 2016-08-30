/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AnnBarrelGoodLowPt.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MuGirlSelection/AnnBarrelGoodLowPt.h"
#include "math.h"


double MuGirlNS::AnnBarrelGoodLowPt::value(int index,double in0,double in1,double in2,double in3,double in4,double in5,double in6,double in7,double in8)
{
   m_input0 = (in0 - 0)/1;
   m_input1 = (in1 - 0)/1;
   m_input2 = (in2 - 0)/1;
   m_input3 = (in3 - 0)/1;
   m_input4 = (in4 - 0)/1;
   m_input5 = (in5 - 0)/1;
   m_input6 = (in6 - 0)/1;
   m_input7 = (in7 - 0)/1;
   m_input8 = (in8 - 0)/1;
   switch(index) 
   {
     case 0:
         return ((neuron0DE5F670()*1)+0);
     default:
         return 0.;
   }
}
double MuGirlNS::AnnBarrelGoodLowPt::neuron0BDE51C0() 
{
   return m_input0;
}

double MuGirlNS::AnnBarrelGoodLowPt::neuron0DB2E240() 
{
   return m_input1;
}

double MuGirlNS::AnnBarrelGoodLowPt::neuron0DB9CEE0() 
{
   return m_input2;
}

double MuGirlNS::AnnBarrelGoodLowPt::neuron0DB647E0() 
{
   return m_input3;
}

double MuGirlNS::AnnBarrelGoodLowPt::neuron0DB64948() 
{
   return m_input4;
}

double MuGirlNS::AnnBarrelGoodLowPt::neuron0DB64AB0() 
{
   return m_input5;
}

double MuGirlNS::AnnBarrelGoodLowPt::neuron0DB64C18() 
{
   return m_input6;
}

double MuGirlNS::AnnBarrelGoodLowPt::neuron0C1DE7E0() 
{
   return m_input7;
}

double MuGirlNS::AnnBarrelGoodLowPt::neuron0C1DE8B8() 
{
   return m_input8;
}

double MuGirlNS::AnnBarrelGoodLowPt::neuron0C1DEA20() 
{
   double input = -0.0287506;
   input += synapse0DB4C178();
   input += synapse0C1DEBA8();
   input += synapse0C1DEBD8();
   input += synapse0C1DEC08();
   input += synapse0C1DEC38();
   input += synapse0C1DEC68();
   input += synapse0C1DEC98();
   input += synapse0C1DECC8();
   input += synapse0C1DECF8();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnBarrelGoodLowPt::neuron0C1DED28() 
{
   double input = -0.407028;
   input += synapse0DBA3870();
   input += synapse0DBA38A0();
   input += synapse0DBA38D0();
   input += synapse0DBA3900();
   input += synapse0DBA3930();
   input += synapse0DBA3960();
   input += synapse0DBA3990();
   input += synapse0DBA39C0();
   input += synapse0DBA39F0();
   if (input<-700) return 0; 
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnBarrelGoodLowPt::neuron0DBA3A20() 
{
   double input = 0.147366;
   input += synapse0DBA3B40();
   input += synapse0DBA3B70();
   input += synapse0DBA3BA0();
   input += synapse0DBA3BD0();
   input += synapse0DBA3C00();
   input += synapse0DBA3C30();
   input += synapse0DBA3C60();
   input += synapse0DBA3C90();
   input += synapse0DBA3CC0();
   if (input<-700) return 0; 
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnBarrelGoodLowPt::neuron0DBA3CF0() 
{
   double input = 2.65078;
   input += synapse0DBA3E78();
   input += synapse0DBA3EA8();
   input += synapse0DBA3ED8();
   input += synapse0DBA4878();
   input += synapse0DBA48A8();
   input += synapse0DBA48D8();
   input += synapse0DBA4908();
   input += synapse0DBA4938();
   input += synapse0DBA4968();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnBarrelGoodLowPt::neuron0DBA4998() 
{
   double input = -0.441187;
   input += synapse0DBA4B20();
   input += synapse0DBA4B50();
   input += synapse0DBA4B80();
   input += synapse0DBA4BB0();
   input += synapse0DBA4BE0();
   input += synapse0DBA4C10();
   input += synapse0DBA4C40();
   input += synapse0DBA4C70();
   input += synapse0DBA4CA0();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnBarrelGoodLowPt::neuron0DBA4CD0() {
   double input = 1.9847;
   input += synapse0DBA4E58();
   input += synapse0DBA4E88();
   input += synapse0DBA4EB8();
   input += synapse0DBA4EE8();
   input += synapse0DBA4F18();
   input += synapse0DBA4F48();
   input += synapse0DBA4F78();
   input += synapse0DBA4FA8();
   input += synapse0DBA4FD8();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnBarrelGoodLowPt::neuron0DE5F3C8() 
{
   double input = -0.215194;
   input += synapse0DE5F550();
   input += synapse0DE5F580();
   input += synapse0DE5F5B0();
   input += synapse0DE5F5E0();
   input += synapse0DE5F610();
   input += synapse0DE5F640();
   if (input<-700) return 0;
   return ((1/(1+exp(-input)))*1)+0;
}

double MuGirlNS::AnnBarrelGoodLowPt::neuron0DE5F670() 
{
   double input = 0.0299278;
   input += synapse0DE5F7F8();
   return input;
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DB4C178()
{
   return (neuron0BDE51C0()*-0.391581);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0C1DEBA8() 
{
   return (neuron0DB2E240()*-0.314075);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0C1DEBD8() 
{
   return (neuron0DB9CEE0()*0.191768);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0C1DEC08() 
{
   return (neuron0DB647E0()*-0.182003);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0C1DEC38() 
{
   return (neuron0DB64948()*0.166065);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0C1DEC68() 
{
   return (neuron0DB64AB0()*0.115159);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0C1DEC98() 
{
   return (neuron0DB64C18()*0.334121);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0C1DECC8() 
{
   return (neuron0C1DE7E0()*-0.473659);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0C1DECF8() 
{
   return (neuron0C1DE8B8()*0.968751);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA3870() 
{
   return (neuron0BDE51C0()*-1.48123);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA38A0() 
{
   return (neuron0DB2E240()*-0.418054);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA38D0() 
{
   return (neuron0DB9CEE0()*-0.262636);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA3900() 
{
   return (neuron0DB647E0()*0.284963);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA3930() 
{
   return (neuron0DB64948()*-0.0910348);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA3960() 
{
   return (neuron0DB64AB0()*-0.759238);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA3990() 
{
   return (neuron0DB64C18()*-0.732791);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA39C0() 
{
   return (neuron0C1DE7E0()*0.0887397);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA39F0() 
{
   return (neuron0C1DE8B8()*0.57245);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA3B40() 
{
   return (neuron0BDE51C0()*-1.6841);
}
double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA3B70() 
{
   return (neuron0DB2E240()*0.528272);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA3BA0() 
{
   return (neuron0DB9CEE0()*0.128633);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA3BD0() 
{
   return (neuron0DB647E0()*0.215498);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA3C00() 
{
   return (neuron0DB64948()*0.269092);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA3C30() 
{
   return (neuron0DB64AB0()*0.495982);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA3C60() 
{
   return (neuron0DB64C18()*-0.215724);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA3C90() 
{
   return (neuron0C1DE7E0()*0.342254);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA3CC0() 
{
   return (neuron0C1DE8B8()*-0.147173);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA3E78() 
{
   return (neuron0BDE51C0()*0.367798);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA3EA8() 
{
   return (neuron0DB2E240()*-0.466772);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA3ED8() 
{
   return (neuron0DB9CEE0()*1.27136);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4878() 
{
   return (neuron0DB647E0()*-0.997831);
}


double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA48A8() 
{
   return (neuron0DB64948()*-2.33123);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA48D8() 
{
   return (neuron0DB64AB0()*-0.360161);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4908() 
{
   return (neuron0DB64C18()*-1.56387);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4938() 
{
   return (neuron0C1DE7E0()*2.84331);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4968() 
{
   return (neuron0C1DE8B8()*-0.00187518);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4B20() 
{
   return (neuron0BDE51C0()*0.528316);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4B50() 
{
   return (neuron0DB2E240()*-1.10629);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4B80() 
{
   return (neuron0DB9CEE0()*0.374618);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4BB0() 
{
   return (neuron0DB647E0()*-0.328697);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4BE0() 
{
   return (neuron0DB64948()*-0.719118);
}


double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4C10() 
{
   return (neuron0DB64AB0()*-1.37625);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4C40() 
{
   return (neuron0DB64C18()*-1.08389);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4C70() 
{
   return (neuron0C1DE7E0()*0.700709);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4CA0() 
{
   return (neuron0C1DE8B8()*0.0317535);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4E58() 
{
   return (neuron0BDE51C0()*0.189919);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4E88() 
{
   return (neuron0DB2E240()*1.48444);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4EB8() 
{
   return (neuron0DB9CEE0()*-0.449789);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4EE8() 
{
   return (neuron0DB647E0()*1.07427);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4F18() 
{
   return (neuron0DB64948()*0.637973);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4F48() 
{
   return (neuron0DB64AB0()*-2.27395);
}
double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4F78() 
{
   return (neuron0DB64C18()*-0.769155);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4FA8() 
{
   return (neuron0C1DE7E0()*1.06215);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DBA4FD8() 
{
   return (neuron0C1DE8B8()*-0.0156051);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DE5F550() 
{
   return (neuron0C1DEA20()*0.0308387);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DE5F580() 
{
   return (neuron0C1DED28()*-0.311269);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DE5F5B0() 
{
   return (neuron0DBA3A20()*-0.354995);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DE5F5E0() 
{
   return (neuron0DBA3CF0()*-2.28687);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DE5F610() 
{
   return (neuron0DBA4998()*-2.10965);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DE5F640() 
{
   return (neuron0DBA4CD0()*-5.58966);
}

double MuGirlNS::AnnBarrelGoodLowPt::synapse0DE5F7F8() 
{
   return (neuron0DE5F3C8()*2.11327);
}
