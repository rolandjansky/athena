/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * DisambiguationDRIncl2.cpp
 * Created by Joerg Stelzer / V Sorin on 9/16/14.
 * 
 * @brief algorithm calculates the dR distance between objects in two lists, 
 * but minimum value uses > instead of >= as dr algos
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/DisambiguationDRIncl2.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
#include "L1TopoSimulationUtils/Kinematics.h"

REGISTER_ALG_TCS(DisambiguationDRIncl2)


// not the best solution but we will move to athena where this comes for free
#define LOG std::cout << "TCS::DisambiguationDRIncl2:     "


TCS::DisambiguationDRIncl2::DisambiguationDRIncl2(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth1", 9);
   defineParameter("InputWidth2", 9);
   defineParameter("MaxTob1", 0); 
   defineParameter("MaxTob2", 0); 
   defineParameter("NumResultBits", 2);
   defineParameter("ClusterOnly",0);
   defineParameter("MinET1",1,0);
   defineParameter("MinET2",1,0);
   defineParameter("DisambDRSqrMin",0,0);
   defineParameter("DisambDRSqrMax",0,0);
   defineParameter("MinET1",1,1);
   defineParameter("MinET2",1,1);
   defineParameter("DisambDRSqrMin",0,1);
   defineParameter("DisambDRSqrMax",0,1);
 
   setNumberOutputBits(2);
}

TCS::DisambiguationDRIncl2::~DisambiguationDRIncl2(){}


TCS::StatusCode
TCS::DisambiguationDRIncl2::initialize() {
   p_NumberLeading1 = parameter("InputWidth1").value();
   p_NumberLeading2 = parameter("InputWidth2").value();
   if(parameter("MaxTob1").value() > 0) p_NumberLeading1 = parameter("MaxTob1").value();
   if(parameter("MaxTob2").value() > 0) p_NumberLeading2 = parameter("MaxTob2").value();




   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_MinET1[i] = parameter("MinET1",i).value();
      p_MinET2[i] = parameter("MinET2",i).value();

      p_DRCutMin[i] = parameter("DisambDRSqrMin", i).value();
      p_DRCutMax[i] = parameter("DisambDRSqrMax", i).value();

      TRG_MSG_INFO("MinET1          " << i << " : "<< p_MinET1[i]);
      TRG_MSG_INFO("MinET2          " << i << " : "<< p_MinET2[i]);
      TRG_MSG_INFO("DisambDRmin         " << i << " : "<< p_DRCutMin[i]);
      TRG_MSG_INFO("DisambDRmax       " << i << " : "<< p_DRCutMax[i]);


   }


   TRG_MSG_INFO("number output : " << numberOutputBits());


   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::DisambiguationDRIncl2::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{

      
   if( input.size() == 2) {

      
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1)
         {


            for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                 tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                 ++tob2) {
                // test DeltaR2Min, DeltaR2Max
                unsigned int deltaR2Cut = TSU::Kinematics::calcDeltaR2BW( *tob1, *tob2 );
                for(unsigned int i=0; i<numberOutputBits(); ++i) {
                    bool accept = false;
                    if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
                    if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
                    accept = deltaR2Cut > p_DRCutMin[i] && deltaR2Cut <= p_DRCutMax[i] ;
               
                     if( accept ) {
                       decision.setBit(i, true);
                       output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
                     }
	             TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " deltaR = " << deltaR2Cut );


               }
 
            }


            
      }

   } else {

      TCS_EXCEPTION("DisambiguationDRIncl2 alg must have  2 inputs, but got " << input.size());

   }
   return TCS::StatusCode::SUCCESS;

}

TCS::StatusCode
TCS::DisambiguationDRIncl2::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{

      
   if( input.size() == 2) {

      
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1)
         {
             for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                  tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                  ++tob2) {
                 // test DeltaR2Min, DeltaR2Max
                 unsigned int deltaR2Cut = TSU::Kinematics::calcDeltaR2( *tob1, *tob2 );
                 for(unsigned int i=0; i<numberOutputBits(); ++i) {
                     bool accept = false;
                     if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
                     if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
                     accept = deltaR2Cut > p_DRCutMin[i] && deltaR2Cut <= p_DRCutMax[i] ;
                     if( accept ) {
                         decision.setBit(i, true);
                         output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
                     }
                     TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " deltaR = " << deltaR2Cut );
                 }
             }
         }
   } else {
      TCS_EXCEPTION("DisambiguationDRIncl2 alg must have  2 inputs, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}
