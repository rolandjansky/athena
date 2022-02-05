/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  eEmSelect.cxx
//  TopoCore
//  Algorithm to select the abbreviated list of eEMs , no order is applied
//
#include "L1TopoAlgorithms/eEmSelect.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/eEmTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(eEmSelect)

// constructor
TCS::eEmSelect::eEmSelect(const std::string & name) : SortingAlg(name) {
   defineParameter( "InputWidth", 120 ); // for fw
   defineParameter( "InputWidth1stStage", 30 ); // for fw
   defineParameter( "OutputWidth", 6 );
   defineParameter( "MinET", 0 );
   defineParameter( "REtaMin", 0);
   defineParameter( "RHadMin", 0);
   defineParameter( "WsTotMin", 0);
   defineParameter( "MinEta", 0 );
   defineParameter( "MaxEta", 63);
}


// destructor
TCS::eEmSelect::~eEmSelect() {}

TCS::StatusCode
TCS::eEmSelect::initialize() {
   m_numberOfeEms = parameter("OutputWidth").value();
   m_et = parameter("MinET").value();
   m_minEta = parameter("MinEta").value();
   m_maxEta = parameter("MaxEta").value();
   m_maxEta = parameter("MaxEta").value();
   m_minREta = parameter("REtaMin").value();
   m_minRHad = parameter("RHadMin").value();
   m_minWsTot = parameter("WsTotMin").value();
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::eEmSelect::sort(const InputTOBArray & input, TOBArray & output) {

   const eEmTOBArray & eems = dynamic_cast<const eEmTOBArray&>(input);

   // fill output array with GenericTOB buildt from eEms
   for(eEmTOBArray::const_iterator eem = eems.begin(); eem!= eems.end(); ++eem ) {

      if( parType_t((*eem)->Et()) <= m_et ) continue; // ET cut
      // isolation cut
      if ( !isocut(m_minREta, (*eem)-> Reta()) ) {continue;}
      if ( !isocut(m_minRHad, (*eem)-> Rhad()) ) {continue;}
      if ( !isocut(m_minWsTot, (*eem)-> Wstot()) ) {continue;}
      
      // eta cut
      if (parType_t(std::abs((*eem)-> eta())) < m_minEta) continue; 
      if (parType_t(std::abs((*eem)-> eta())) > m_maxEta) continue;  
      
      const GenericTOB gtob(**eem);
      output.push_back( gtob );
   }


   // keep only max number of eEms
   int par = m_numberOfeEms ;
   unsigned int maxNumberOfeEms = (unsigned int)(par<0?0:par);
   if(maxNumberOfeEms>0) {
      while( output.size()> maxNumberOfeEms ) {
         output.pop_back();
      }
   }
   return TCS::StatusCode::SUCCESS;
}

