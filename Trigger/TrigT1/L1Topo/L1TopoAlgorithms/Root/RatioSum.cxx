/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * RatioSum.cpp
 * Created by Joerg Stelzer on 11/16/12.
 *
 * @brief algorithm to select W tag-and-probe events using MET/sumET
 *
 * In addition, also apply cuts on Ht and MET.
 * For a more detailed description, see Sec. 4.3.2 of
 * ATL-COM-DAQ-2014-005 and the references therein.
 *
 **********************************/

#include <cmath>

#include "L1TopoAlgorithms/RatioSum.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"


REGISTER_ALG_TCS(RatioSum)

// not the best solution but we will move to athena where this comes for free
#define LOG std::cout << name() << ":     "


TCS::RatioSum::RatioSum(const std::string & name) : DecisionAlg(name)
{  
   defineParameter("InputWidth1", 9);
   defineParameter("InputWidth2", 9);
   defineParameter("InputWidth3", 9);
   defineParameter("MaxTob1", 0); 
   defineParameter("MaxTob2", 0);
   defineParameter("MaxTob3", 0); 
   defineParameter("NumResultBits", 2);
   defineParameter("UseCluster05Granularity",0);
   defineParameter("MinET2",0);
   defineParameter("EtaMin2",0);
   defineParameter("EtaMax2",49);
   defineParameter("MinET3",0);
   defineParameter("EtaMin3",0);
   defineParameter("EtaMax3",49);
   defineParameter("MinMET",0);
   defineParameter("HT",0);
   defineParameter("SUM",0);
   defineParameter("Ratio",0,0);
   defineParameter("Ratio",0,1);
   setNumberOutputBits(2);
}

TCS::RatioSum::~RatioSum()
{}


TCS::StatusCode
TCS::RatioSum::initialize() {
   p_NumberLeading1 = parameter("InputWidth1").value();
   p_NumberLeading2 = parameter("InputWidth2").value();
   p_NumberLeading3 = parameter("InputWidth3").value();

   if(parameter("MaxTob1").value() > 0) p_NumberLeading1 = parameter("MaxTob1").value();
   if(parameter("MaxTob2").value() > 0) p_NumberLeading2 = parameter("MaxTob2").value();
   if(parameter("MaxTob3").value() > 0) p_NumberLeading3 = parameter("MaxTob3").value();




   TRG_MSG_INFO("Maxtob 1          : " << p_NumberLeading1);
   TRG_MSG_INFO("Maxtob 2          : " << p_NumberLeading2);
   TRG_MSG_INFO("Maxtob 3          : " << p_NumberLeading3);

   p_MinMET  = parameter("MinMET").value();
   p_MinET2  = parameter("MinET2").value();
   p_MinET3  = parameter("MinET3").value();

   p_EtaMin2 = parameter("EtaMin2").value();
   p_EtaMax2 = parameter("EtaMax2").value();
   p_EtaMin3 = parameter("EtaMin3").value();
   p_EtaMax3 = parameter("EtaMax3").value();



   TRG_MSG_INFO("MinMET          : " << p_MinMET);
   TRG_MSG_INFO("MinET2          : " << p_MinET2);
   TRG_MSG_INFO("EtaMin2         : " << p_EtaMin2);
   TRG_MSG_INFO("EtaMax2         : " << p_EtaMax2);

   TRG_MSG_INFO("MinET3          : " << p_MinET3);
   TRG_MSG_INFO("EtaMin3         : " << p_EtaMin3);
   TRG_MSG_INFO("EtaMax3         : " << p_EtaMax3);



   p_HT = parameter("HT").value();
   p_SUM = parameter("SUM").value();
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
       p_Ratio[i] = parameter("Ratio", i).value();

       TRG_MSG_INFO("Ratio " << i << " : " << p_Ratio[i]);
   }    
   TRG_MSG_INFO("HT " << p_HT);
   TRG_MSG_INFO("SUM " << p_SUM);
   
   TRG_MSG_INFO("nummber output : " << numberOutputBits());
   return StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::RatioSum::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )

{
//Only difference between this method and process()
//is the replacement of the function fabs() --> abs()
//Note, however, that this algorithm benefits from MetSort
//which is a sorting algorithm that does have a distinct bitwise implementation

   if(input.size()!=3) {
      TCS_EXCEPTION("RatioSum alg must have exactly 3 input lists, but got " << input.size());
   }
   // Note (from Murrough, ATR-14913, 2016-08-25):
   // summing Et in ints so it might be losing 0.5 GeV counts from EM/Tau lists
   unsigned int sumET = 0;
   unsigned int sumET2 = 0;
   unsigned int nLeadingele = p_NumberLeading3;

   const TCS::GenericTOB & met = (*input[0])[0];

   // loop over all jets
   unsigned int objC(0);
   for( TCS::GenericTOB * tob : *input[1]) {

      if( parType_t(abs(tob->eta())) > p_EtaMax2 ) continue; // Eta cut
      if( parType_t(abs(tob->eta())) < p_EtaMin2 ) continue; // Eta cut
      if( tob->Et() <= p_MinET2 ) continue; // E_T cut

      TRG_MSG_DEBUG("Jet : ET = " << tob->Et());
      ++objC;
      sumET2 += tob->Et();

   }

   sumET = sumET2;

   // loop over the third collection (EM/tau)
   for( TOBArray::const_iterator tob1 = input[2]->begin();
           tob1 != input[2]->end() && distance( input[2]->begin(), tob1) < nLeadingele;
           ++tob1)
         {

          if( parType_t(abs((*tob1)->eta())) > p_EtaMax3 ) continue; // Eta cut
          if( parType_t(abs((*tob1)->eta())) < p_EtaMin3 ) continue; // Eta cut
          if( (*tob1)->Et() <= p_MinET3 ) continue; // E_T cut
          sumET += (*tob1)->Et() ;

   }

   for(unsigned int i=0; i<numberOutputBits(); ++i) {

       bool accept = (objC!=0 && met.Et() > p_MinMET &&
                      sumET!=sumET2 && // in practice, require an EM TOB with Et > 0 (see ATR-14913)
                      sumET2 > p_HT &&
                      sumET > p_SUM &&
                      10*met.Et() >= p_Ratio[i]*sumET);

    decision.setBit( i, accept );

    if(accept)
        output[i]->push_back( CompositeTOB( GenericTOB::createOnHeap( GenericTOB(sumET,0,0) ) ));

    TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " HT = " << sumET2 << " SUM = " << sumET);

   }

   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::RatioSum::process( const std::vector<TCS::TOBArray const *> & input,
                     const std::vector<TCS::TOBArray *> & output,
                     Decision & decision )
{

   if(input.size()!=3) {
      TCS_EXCEPTION("RatioSum alg must have exactly 3 input lists, but got " << input.size());
   }
   // Note (from Murrough, ATR-14913, 2016-08-25):
   // summing Et in ints so it might be losing 0.5 GeV counts from EM/Tau lists
   unsigned int sumET = 0;
   unsigned int sumET2 = 0;   
   unsigned int nLeadingele = p_NumberLeading3;

   const TCS::GenericTOB & met = (*input[0])[0];

   // loop over all jets
   unsigned int objC(0);
   for( TCS::GenericTOB * tob : *input[1]) {

      if( parType_t(fabs(tob->eta())) > p_EtaMax2 ) continue; // Eta cut
      if( parType_t(fabs(tob->eta())) < p_EtaMin2 ) continue; // Eta cut
      if( tob->Et() <= p_MinET2 ) continue; // E_T cut

      TRG_MSG_DEBUG("Jet : ET = " << tob->Et());
      ++objC;
      sumET2 += tob->Et();  
}
 
   sumET = sumET2;

   // loop over the third collection (EM/tau)
   for( TOBArray::const_iterator tob1 = input[2]->begin(); 
           tob1 != input[2]->end() && distance( input[2]->begin(), tob1) < nLeadingele;
           ++tob1) 
         {
	
          if( parType_t(fabs((*tob1)->eta())) > p_EtaMax3 ) continue; // Eta cut
          if( parType_t(fabs((*tob1)->eta())) < p_EtaMin3 ) continue; // Eta cut
          if( (*tob1)->Et() <= p_MinET3 ) continue; // E_T cut
          sumET += (*tob1)->Et() ;

   }

   for(unsigned int i=0; i<numberOutputBits(); ++i) {

       bool accept = (objC!=0 && met.Et() > p_MinMET &&
                      sumET!=sumET2 && // in practice, require an EM TOB with Et > 0 (see ATR-14913)
                      sumET2 > p_HT &&
                      sumET > p_SUM &&
                      10*met.Et() >= p_Ratio[i]*sumET);

    decision.setBit( i, accept );

    if(accept)
        output[i]->push_back( CompositeTOB( GenericTOB::createOnHeap( GenericTOB(sumET,0,0) ) ));

    TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " HT = " << sumET2 << " SUM = " << sumET);

   }

   return TCS::StatusCode::SUCCESS;
}
