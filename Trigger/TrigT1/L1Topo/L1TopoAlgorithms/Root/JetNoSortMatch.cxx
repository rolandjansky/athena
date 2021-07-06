/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  JetNoSortMatch.cxx
//  TopoCore
//  Created by Veronica Sorin on 14/8/14.
//  Algorithm to generate ALL lists of JETs TOBs with cuts on etwide and narrow
//
#include "L1TopoAlgorithms/JetNoSortMatch.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/JetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(JetNoSortMatch)


// constructor
TCS::JetNoSortMatch::JetNoSortMatch(const std::string & name) :
   SortingAlg(name)
{
   defineParameter( "InputWidth", 64 ); // for FW
   defineParameter( "OutputWidth", 64 );
   defineParameter( "JetSize", 2 );
   defineParameter("MinET1",0);
   defineParameter("MinET2",0);
   m_jetsize = JetTOB::JS1;
}


TCS::JetNoSortMatch::~JetNoSortMatch()
{}

TCS::StatusCode
TCS::JetNoSortMatch::initialize() {
   m_numberOfJets = parameter("OutputWidth").value();
   m_jsize = parameter("JetSize").value();
   p_MinET1  = parameter("MinET1").value();
   p_MinET2  = parameter("MinET2").value();
	   
   TRG_MSG_INFO("MinET1          : " << p_MinET1);
   TRG_MSG_INFO("MinET2          : " << p_MinET2);
   return TCS::StatusCode::SUCCESS;
}




TCS::StatusCode
TCS::JetNoSortMatch::sort(const InputTOBArray & input, TOBArray & output) {
   const JetTOBArray & jets = dynamic_cast<const JetTOBArray&>(input);
   // because fw seems to have a differnt notation, for now 2 means JS1 8x8
   m_jetsize = m_jsize==2?JetTOB::JS1:JetTOB::JS2; 

   // fill output array with GenericTOBs builds from jets
   for(JetTOBArray::const_iterator cl = jets.begin(); cl!= jets.end(); ++cl ) {

     if( parType_t((*cl)-> Et2())  <= p_MinET1 ) continue; // E_T cut on 4x4, ET1 relates with FW ordering, Et2 is Etsmall
     if( parType_t((*cl)-> Et1())  <= p_MinET2 ) continue; // E_T cut on 8x8  , ET2 relates with FW ordering, Et1 is Etlarge

     output.push_back( GenericTOB(**cl, m_jetsize) );
   }


   // keep only max number of jets
   int par = m_numberOfJets ;
   unsigned int maxNumberOfJets = (unsigned int)(par<0?0:par);
   if(maxNumberOfJets>0) {
      while( output.size()> maxNumberOfJets ) {
         output.pop_back();
      }
   }   
   return TCS::StatusCode::SUCCESS;
}

