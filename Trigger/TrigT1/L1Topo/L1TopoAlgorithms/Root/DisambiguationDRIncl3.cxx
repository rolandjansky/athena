/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * DisambiguationDRIncl3.cpp
 * Created by Joerg Stelzer / V Sorin on 9/16/14.
 * 
 * @brief algorithm calculates the dR distance between objects in three lists, check EM vs TAU , if those not match
 * check agains 3rd object, passed if no match with 3rd
 *
 * @param NumberLeading
**********************************/

#include <cmath>

#include "L1TopoAlgorithms/DisambiguationDRIncl3.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
#include "L1TopoSimulationUtils/Kinematics.h"

REGISTER_ALG_TCS(DisambiguationDRIncl3)


// not the best solution but we will move to athena where this comes for free
#define LOG std::cout << "TCS::DisambiguationDRIncl3:     "

TCS::DisambiguationDRIncl3::DisambiguationDRIncl3(const std::string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth1", 5);
   defineParameter("InputWidth2", 3);
   defineParameter("InputWidth3", 3);
   defineParameter("MaxTob1", 0); 
   defineParameter("MaxTob2", 0); 
   defineParameter("MaxTob3", 0);
   defineParameter("NumResultBits", 2);
   defineParameter("MinET1",1,0);
   defineParameter("MinET2",1,0);
   defineParameter("MinET3",1,0);
   defineParameter("DisambDRSqr",0,0);
   defineParameter("DisambDRSqrMin",0,0);
   defineParameter("DisambDRSqrMax",0,0);
   defineParameter("MinET1",1,1);
   defineParameter("MinET2",1,1);
   defineParameter("MinET3",1,1);
   defineParameter("DisambDRSqr",0,1);
   defineParameter("DisambDRSqrMin",0,1);
   defineParameter("DisambDRSqrMax",0,1);

   setNumberOutputBits(2);
}

TCS::DisambiguationDRIncl3::~DisambiguationDRIncl3(){}


TCS::StatusCode
TCS::DisambiguationDRIncl3::initialize() {
   p_NumberLeading1 = parameter("InputWidth1").value();
   p_NumberLeading2 = parameter("InputWidth2").value();
   p_NumberLeading3 = parameter("InputWidth3").value();

   if(parameter("MaxTob1").value() > 0) p_NumberLeading1 = parameter("MaxTob1").value();
   if(parameter("MaxTob2").value() > 0) p_NumberLeading2 = parameter("MaxTob2").value();
   if(parameter("MaxTob3").value() > 0) p_NumberLeading3 = parameter("MaxTob3").value();



   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_MinET1[i] = parameter("MinET1",i).value();
      p_MinET2[i] = parameter("MinET2",i).value();
      p_MinET3[i] = parameter("MinET3",i).value();

      p_DRCutMin[i] = parameter("DisambDRSqrMin", i).value();
      p_DRCutMax[i] = parameter("DisambDRSqrMax", i).value();

      p_DisambDR[i] = parameter("DisambDRSqr", i).value();

      TRG_MSG_INFO("MinET1          " << i << " : "<< p_MinET1[i]);
      TRG_MSG_INFO("MinET2          " << i << " : "<< p_MinET2[i]);
      TRG_MSG_INFO("MinET3          " << i << " : "<< p_MinET3[i]);

      TRG_MSG_INFO("DisambDRmin         " << i << " : "<< p_DRCutMin[i]);
      TRG_MSG_INFO("DisambDRmax       " << i << " : "<< p_DRCutMax[i]);
      TRG_MSG_INFO("DisambDR         : " << p_DisambDR[i]);


   }


   TRG_MSG_INFO("number output : " << numberOutputBits());


   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::DisambiguationDRIncl3::processBitCorrect( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{

      
   if( input.size() == 3) {

      
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1)
         {


            for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                 tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                 ++tob2) {


               // test DeltaR2Min, DeltaR2Max
               unsigned int deltaR2Cut = TSU::Kinematics::calcDeltaR2BW( *tob1, *tob2 );
               
               for( TCS::TOBArray::const_iterator tob3 = input[2]->begin();
                    tob3 != input[2]->end() ;
                    ++tob3) {
                   unsigned int deltaR13 = TSU::Kinematics::calcDeltaR2BW( *tob1, *tob3 );
                   unsigned int deltaR23 = TSU::Kinematics::calcDeltaR2BW( *tob2, *tob3 );
                   for(unsigned int i=0; i<numberOutputBits(); ++i) {
                       bool accept = false;
                       if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
                       if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
                       if( parType_t((*tob3)->Et()) <= p_MinET3[i]) continue; // ET cut
                       // test DeltaR2Min, DeltaR2Max
                       if (deltaR2Cut > p_DRCutMax[i]) continue;
                       if (deltaR2Cut <= p_DRCutMin[i]) continue;
                       //
                       accept = deltaR13 > p_DisambDR[i] && deltaR23 > p_DisambDR[i] ;
                       if( accept ) {
                           decision.setBit(i, true);
                           output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
                       }
                       TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " deltaR13 = " << deltaR13 << " deltaR23 = " << deltaR23);
                   }
               }
            }
         }
   } else {
       TCS_EXCEPTION("DisambiguationDRIncl3 alg must have  3 inputs, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::DisambiguationDRIncl3::process( const std::vector<TCS::TOBArray const *> & input,
                             const std::vector<TCS::TOBArray *> & output,
                             Decision & decision )
{

      
   if( input.size() == 3) {

      
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance(input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1)
         {


            for( TCS::TOBArray::const_iterator tob2 = input[1]->begin(); 
                 tob2 != input[1]->end() && distance(input[1]->begin(), tob2) < p_NumberLeading2;
                 ++tob2) {


               // test DeltaR2Min, DeltaR2Max
               unsigned int deltaR2Cut = TSU::Kinematics::calcDeltaR2( *tob1, *tob2 );
               
               for( TCS::TOBArray::const_iterator tob3 = input[2]->begin();
                    tob3 != input[2]->end() ;
                    ++tob3) {
                   unsigned int deltaR13 = TSU::Kinematics::calcDeltaR2( *tob1, *tob3 );
                   unsigned int deltaR23 = TSU::Kinematics::calcDeltaR2( *tob2, *tob3 );
                   for(unsigned int i=0; i<numberOutputBits(); ++i) {
                       bool accept = false;
                       if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
                       if( parType_t((*tob2)->Et()) <= p_MinET2[i]) continue; // ET cut
                       if( parType_t((*tob3)->Et()) <= p_MinET3[i]) continue; // ET cut
                       // test DeltaR2Min, DeltaR2Max
                       if (deltaR2Cut > p_DRCutMax[i]) continue;
                       if (deltaR2Cut <= p_DRCutMin[i]) continue;
                       //
                       accept = deltaR13 > p_DisambDR[i] && deltaR23 > p_DisambDR[i] ;
                       if( accept ) {
                           decision.setBit(i, true);
                           output[i]->push_back(TCS::CompositeTOB(*tob1, *tob2));
                       }
                       TRG_MSG_DEBUG("Decision " << i << ": " << (accept?"pass":"fail") << " deltaR13 = " << deltaR13 << " deltaR23 = " << deltaR23);
                   }
               }
            }
         }
   } else {
      TCS_EXCEPTION("DisambiguationDRIncl3 alg must have  3 inputs, but got " << input.size());
   }
   return TCS::StatusCode::SUCCESS;
}
