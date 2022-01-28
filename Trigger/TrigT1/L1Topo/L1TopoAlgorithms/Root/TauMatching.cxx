/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  TauMatching.cxx
//  TopoCore
//  Algorithm to select the abbreviated list of cTaus , no order is applied
//
#include "L1TopoAlgorithms/TauMatching.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/cTauTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(TauMatching)

// constructor
TCS::TauMatching::TauMatching(const std::string & name) : SortingAlg(name) {
   defineParameter( "InputWidth", 120 ); // for fw
   defineParameter( "InputWidth1stStage", 30 ); // for fw
   defineParameter( "OutputWidth", 6 );
   defineParameter( "MinET", 0 );
   defineParameter( "RCore", 0);
   defineParameter( "RHad", 0);
   defineParameter( "MinEta", 0 );
   defineParameter( "MaxEta", 63);
}


// destructor
TCS::TauMatching::~TauMatching() {}

TCS::StatusCode
TCS::TauMatching::initialize() {
   m_numberOfcTaus = parameter("OutputWidth").value();
   m_et = parameter("MinET").value();
   m_rCore = parameter("RCore").value();
   m_rHad = parameter("RHad").value();
   m_minEta = parameter("MinEta").value();
   m_maxEta = parameter("MaxEta").value();
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::TauMatching::sort(const InputTOBArray & input, TOBArray & output) {

   const cTauTOBArray & ctaus = dynamic_cast<const cTauTOBArray&>(input);

   // fill output array with GenericTOB buildt from ctaus
   for(cTauTOBArray::const_iterator ctau = ctaus.begin(); ctau!= ctaus.end(); ++ctau ) {
      const GenericTOB gtob(**ctau);

      if ((*ctau)->tobType() == TCS::ETAU)
	{std::cout << "I am eTau" << std::endl;}//Do something with eTau
      if ((*ctau)->tobType() == TCS::JTAU)
	{std::cout << "I am jTau" << std::endl;}//Do something with jTau
      
      //Currently do nothing!
      continue;
      
      output.push_back( gtob );
   }


   // keep only max number of ctaus
   int par = m_numberOfcTaus ;
   unsigned int maxNumberOfcTaus = (unsigned int)(par<0?0:par);
   if(maxNumberOfcTaus>0) {
      while( output.size()> maxNumberOfcTaus ) {
         output.pop_back();
      }
   }
   return TCS::StatusCode::SUCCESS;
}

