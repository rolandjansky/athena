/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*********************************
 * InvariantMassThreeTOBsIncl.cxx
 * Created by Carlos Moreno based on InvariantMassInclusive1 by V Sorin
 * 
 * For questions contact atlas-trig-l1topo-algcom@cern.ch. 
 * @brief algorithm calculates the sqr of the INVMASS of three TOBs from one  list and applies invmass criteria
 *
 * @param NumberLeading 
 *
 * First version assuming same cuts in all TOBs. If we'd like to have a version similar to *INVM*-MU4ab-MU6ab-MU10ab
 * we'll need to change the following:
 *     p_NumberLeading* will need to be added to account for the different lists. This could help to have something like *INVM*-2MU4ab-MU6ab
 *     MinET2,3 will need to be added to apply the different cuts on the lists
**********************************/

// TO DO size of the input list to be possbly refined 

#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "TH1F.h"
#include "TH2F.h"

#include "L1TopoAlgorithms/InvariantMassThreeTOBsIncl.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/Decision.h"
// Bitwise implementation utils
#include "L1TopoSimulationUtils/L1TopoDataTypes.h"
#include "L1TopoSimulationUtils/Trigo.h"
#include "L1TopoSimulationUtils/Hyperbolic.h"
#include "L1TopoSimulationUtils/Kinematics.h"

//

REGISTER_ALG_TCS(InvariantMassThreeTOBsIncl)

using namespace std;

TCS::InvariantMassThreeTOBsIncl::InvariantMassThreeTOBsIncl(const string & name) : DecisionAlg(name)
{
   defineParameter("InputWidth", 3);
   defineParameter("MaxTob", 0); 
   defineParameter("NumResultBits", 6);
   defineParameter("RequireOneBarrel", 0);
   defineParameter("MinMSqr",  0, 0);
   defineParameter("MaxMSqr", 999, 0);
   defineParameter("MinMSqr",  0, 1);
   defineParameter("MaxMSqr",  999, 1);
   defineParameter("MinMSqr", 0, 2);
   defineParameter("MaxMSqr", 999, 2);
   defineParameter("MinMSqr", 0, 3);
   defineParameter("MaxMSqr", 999, 3);
   defineParameter("MinMSqr", 0, 4);
   defineParameter("MaxMSqr", 999, 4);
   defineParameter("MinMSqr", 0, 5);
   defineParameter("MaxMSqr", 999, 5);
   defineParameter("MinET1",0,0);
   defineParameter("MinET1",0,1);
   defineParameter("MinET1",0,2);
   defineParameter("MinET1",0,3);
   defineParameter("MinET1",0,4);
   defineParameter("MinET1",0,5);
 
   setNumberOutputBits(6);
}

TCS::InvariantMassThreeTOBsIncl::~InvariantMassThreeTOBsIncl(){}


TCS::StatusCode
TCS::InvariantMassThreeTOBsIncl::initialize() {
  if(parameter("MaxTob").value() > 0) {
      p_NumberLeading1 = parameter("MaxTob").value();
   } else {
      p_NumberLeading1 = parameter("InputWidth").value();
   }

   p_OneBarrel = parameter("RequireOneBarrel").value();

   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      p_InvMassMin[i] = parameter("MinMSqr", i).value();
      p_InvMassMax[i] = parameter("MaxMSqr", i).value();
   
      p_MinET1[i] = parameter("MinET1",i).value();
   }
   TRG_MSG_INFO("NumberLeading1 : " << p_NumberLeading1);
   TRG_MSG_INFO("RequireOneBarrel : " << p_OneBarrel);
   for(unsigned int i=0; i<numberOutputBits(); ++i) {
      TRG_MSG_INFO("InvMassMin   : " << p_InvMassMin[i]);
      TRG_MSG_INFO("InvMassMax   : " << p_InvMassMax[i]);
   
      TRG_MSG_INFO("MinET1          : " << p_MinET1[i]);
   }
   TRG_MSG_INFO("number output : " << numberOutputBits());

   for (unsigned int i=0; i<numberOutputBits();i++) {
       const int buf_len = 512;
       char hname_accept[buf_len], hname_reject[buf_len];
       int mass_min = sqrt(p_InvMassMin[i]);
       int mass_max = sqrt(p_InvMassMax[i]);
       // mass histograms
       snprintf(hname_accept, buf_len, "Accept_%s_%s_bit%d_%dM%d", name().c_str(), className().c_str(), i, mass_min, mass_max);
       snprintf(hname_reject, buf_len, "Reject_%s_%s_bit%d_%dM%d", name().c_str(), className().c_str(), i, mass_min, mass_max);
       registerHist(m_histAcceptINVThree[i] = new TH1F(hname_accept, hname_accept, 100, 0.0, 2*mass_max));
       registerHist(m_histRejectINVThree[i] = new TH1F(hname_reject, hname_reject, 100, 0.0, 2*mass_max));
   }

 
   return StatusCode::SUCCESS;
}



TCS::StatusCode
TCS::InvariantMassThreeTOBsIncl::processBitCorrect( const vector<TCS::TOBArray const *> & input,
						 const vector<TCS::TOBArray *> & output,
						 Decision & decision )
{

   if(input.size() == 1) {     
     
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1) 
         {
            
            TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
            for( ;
                 tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < p_NumberLeading1;
                 ++tob2) {

	      TCS::TOBArray::const_iterator tob3 = tob2; ++tob3;
	      for( ;
		   tob3 != input[0]->end() && distance( input[0]->begin(), tob3) < p_NumberLeading1;
		   ++tob3) {

		// OneBarrel
		if (p_OneBarrel && parType_t(abs((*tob1)->eta())) > 10 && parType_t(abs((*tob2)->eta())) > 10 && parType_t(abs((*tob3)->eta())) > 10 ) continue;
               
		// Inv Mass calculation
		unsigned int invmass2_12 = TSU::Kinematics::calcInvMassBW( *tob1, *tob2 );
		unsigned int invmass2_13 = TSU::Kinematics::calcInvMassBW( *tob1, *tob3 );
		unsigned int invmass2_23 = TSU::Kinematics::calcInvMassBW( *tob2, *tob3 );
		unsigned int invmass2 = invmass2_12 + invmass2_13 + invmass2_23;
		for(unsigned int i=0; i<numberOutputBits(); ++i) {
		  bool accept = false;
		  if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
		  if( parType_t((*tob2)->Et()) <= p_MinET1[i]) continue; // ET cut
		  if( parType_t((*tob3)->Et()) <= p_MinET1[i]) continue; // ET cut

		  accept = invmass2 >= p_InvMassMin[i] && invmass2 <= p_InvMassMax[i]; //
		  const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
		  const bool fillReject = fillHistos() and not fillAccept;
		  const bool alreadyFilled = decision.bit(i);
		  if( accept ) {
		    vector<TCS::GenericTOB*> TOBvector;
		    TOBvector.push_back( *tob1 );
		    TOBvector.push_back( *tob2 );
		    TOBvector.push_back( *tob3 );
		    decision.setBit(i, true);
		    output[i]->push_back( TCS::CompositeTOB(TOBvector) ); //??? Is this going to work? Originally CompositeTOB(*tob1, *tob2)
		    TOBvector.clear();
		  }
		  if(fillAccept and not alreadyFilled) {
		    m_histAcceptINVThree[i]->Fill(sqrt(invmass2));
		  } else if(fillReject) {
		    m_histRejectINVThree[i]->Fill(sqrt(invmass2));
		  }
		  TRG_MSG_INFO("Decision " << i << ": " << (accept?"pass":"fail") << " invmass2 = " << invmass2);
		  
		}
	      }
            }
         }
   } else {

      TCS_EXCEPTION("InvariantMassThreeTOBsIncl alg must have 1 input list, but got " << input.size());

   }

   return TCS::StatusCode::SUCCESS;

}

TCS::StatusCode
TCS::InvariantMassThreeTOBsIncl::process( const vector<TCS::TOBArray const *> & input,
					  const vector<TCS::TOBArray *> & output,
					  Decision & decision )
{

   if(input.size() == 1) {     
      for( TOBArray::const_iterator tob1 = input[0]->begin(); 
           tob1 != input[0]->end() && distance( input[0]->begin(), tob1) < p_NumberLeading1;
           ++tob1) 
         {
            

            TCS::TOBArray::const_iterator tob2 = tob1; ++tob2;      
            for( ;
                 tob2 != input[0]->end() && distance( input[0]->begin(), tob2) < p_NumberLeading1;
                 ++tob2) {

	      TCS::TOBArray::const_iterator tob3 = tob2; tob3++;
	      for( ;
		   tob3 != input[0]->end() && distance( input[0]->begin(), tob3) < p_NumberLeading1;
		   ++tob3) {

		// OneBarrel
		if (p_OneBarrel && parType_t(abs((*tob1)->eta())) > 10 && parType_t(abs((*tob2)->eta())) > 10 && parType_t(abs((*tob3)->eta())) > 10 ) continue;
		
		// Inv Mass calculation
             
		unsigned int invmass2_12 = TSU::Kinematics::calcInvMass( *tob1, *tob2 );
		unsigned int invmass2_13 = TSU::Kinematics::calcInvMass( *tob1, *tob3 );
		unsigned int invmass2_23 = TSU::Kinematics::calcInvMass( *tob2, *tob3 );
		unsigned int invmass2 = invmass2_12 + invmass2_13 + invmass2_23;
		for(unsigned int i=0; i<numberOutputBits(); ++i) {
		  bool accept = false;
		  if( parType_t((*tob1)->Et()) <= p_MinET1[i]) continue; // ET cut
		  if( parType_t((*tob2)->Et()) <= p_MinET1[i]) continue; // ET cut
		  if( parType_t((*tob3)->Et()) <= p_MinET1[i]) continue; // ET cut

		  accept = invmass2 >= p_InvMassMin[i] && invmass2 <= p_InvMassMax[i]; // 
		  const bool fillAccept = fillHistos() and (fillHistosBasedOnHardware() ? getDecisionHardwareBit(i) : accept);
		  const bool fillReject = fillHistos() and not fillAccept;
		  const bool alreadyFilled = decision.bit(i);
		  if( accept ) {
		    vector<TCS::GenericTOB*> TOBvector;
		    TOBvector.push_back( *tob1 );
		    TOBvector.push_back( *tob2 );
		    TOBvector.push_back( *tob3 );
		    decision.setBit(i, true);
		    output[i]->push_back( TCS::CompositeTOB(TOBvector) ); // Check that this won't crush
		    TOBvector.clear();
		  }
		  if(fillAccept and not alreadyFilled) {
		    m_histAcceptINVThree[i]->Fill(sqrt(invmass2));
		  } else if(fillReject) {
		    m_histRejectINVThree[i]->Fill(sqrt(invmass2));
		  }
		  TRG_MSG_INFO("Decision " << i << ": " << (accept?"pass":"fail") << " invmass2 = " << invmass2);
		  
		}
	      }
	    }
	 }
   } else {

      TCS_EXCEPTION("InvariantMassThreeTOBsIncl alg must have either 1 input list, but got " << input.size());

   }

   return TCS::StatusCode::SUCCESS;
}
