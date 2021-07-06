/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  JetSelect.cxx
//  TopoCore
//  Created by Veronica Sorin on 14/8/14.
//  algorithm to create abbreviated jet lists
//
#include "L1TopoAlgorithms/JetSelect.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/JetTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(JetSelect)


// constructor
TCS::JetSelect::JetSelect(const std::string & name) :
   SortingAlg(name)
{
   defineParameter( "InputWidth", 64 ); // for FW
   defineParameter( "InputWidth1stStage", 16 ); // for FW
   defineParameter( "OutputWidth", 10 );
   defineParameter( "JetSize", 2 );
   defineParameter( "MinET", 0 );
   defineParameter( "MinEta", 0 );
   defineParameter( "MaxEta", 31);
   defineParameter( "DoEtaCut", 1);
   m_jetsize = JetTOB::JS1;
}


TCS::JetSelect::~JetSelect()
{}

TCS::StatusCode
TCS::JetSelect::initialize() {
   m_numberOfJets = parameter("OutputWidth").value();
   m_et = parameter("MinET").value();
   m_jsize = parameter("JetSize").value();
   m_minEta = parameter("MinEta").value();
   m_maxEta = parameter("MaxEta").value();
   m_doEtaCut = parameter("DoEtaCut").value();
   return TCS::StatusCode::SUCCESS;
}




TCS::StatusCode
TCS::JetSelect::sort(const InputTOBArray & input, TOBArray & output) {
   const JetTOBArray & jets = dynamic_cast<const JetTOBArray&>(input);
   // because fw seems to have a differnt notation, for now 2 means JS1 8x8
   m_jetsize = m_jsize==2?JetTOB::JS1:JetTOB::JS2; 

   // fill output array with GenericTOBs builds from jets
   for(JetTOBArray::const_iterator cl = jets.begin(); cl!= jets.end(); ++cl ) {
     unsigned int Et = m_jsize==2?parType_t((*cl)->Et1()):parType_t((*cl)->Et2()); 
     if( Et <= m_et ) continue; // ET cut
     if (m_doEtaCut && (parType_t(std::abs((*cl)-> eta())) < m_minEta)) continue; 
     if (m_doEtaCut && (parType_t(std::abs((*cl)-> eta())) > m_maxEta)) continue;      	

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

