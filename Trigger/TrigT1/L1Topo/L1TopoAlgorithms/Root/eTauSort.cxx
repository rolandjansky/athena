/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//  eTauSort.cxx
//  TopoCore
//  algorithm to create sorted lists for eTaus, et order applied
//
#include "L1TopoAlgorithms/eTauSort.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/eTauTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(eTauSort)

bool SortByEtLargesteTau(TCS::GenericTOB* tob1, TCS::GenericTOB* tob2)
{
   return tob1->Et() > tob2->Et();
}


// constructor
TCS::eTauSort::eTauSort(const std::string & name) : SortingAlg(name) {
   defineParameter( "InputWidth", 120 ); // for FW
   defineParameter( "InputWidth1stStage", 30 ); // for FW
   defineParameter( "OutputWidth", 6);
   defineParameter( "RCoreMin", 0);
   defineParameter( "RHadMin", 0);
   defineParameter( "MinEta", 0);
   defineParameter( "MaxEta", 63);
}


// destructor
TCS::eTauSort::~eTauSort() {}


TCS::StatusCode
TCS::eTauSort::initialize() {
   m_numberOfeTaus = parameter("OutputWidth").value();
   m_minRCore = parameter("RCoreMin").value();
   m_minRHad = parameter("RHadMin").value();
   m_minEta = parameter("MinEta").value();
   m_maxEta = parameter("MaxEta").value();
   return TCS::StatusCode::SUCCESS;
}


TCS::StatusCode
TCS::eTauSort::sort(const InputTOBArray & input, TOBArray & output) {

   const eTauTOBArray & clusters = dynamic_cast<const eTauTOBArray&>(input);

   // fill output array with GenericTOB buildt from clusters
   for(eTauTOBArray::const_iterator etau = clusters.begin(); etau!= clusters.end(); ++etau ) {

      if (parType_t(std::abs((*etau)-> eta())) < m_minEta) continue; 
      if (parType_t(std::abs((*etau)-> eta())) > m_maxEta) continue;

      // Isolation cut
      if ( !isocut(m_minRCore, (*etau)-> rCore()) ) {continue;}
      if ( !isocut(m_minRHad, (*etau)-> rHad()) ) {continue;}

      const GenericTOB gtob(**etau);
      output.push_back( gtob );
   }

   // sort
   output.sort(SortByEtLargesteTau);
   

   // keep only max number of clusters
   int par = m_numberOfeTaus;
   unsigned int maxNumberOfeTaus = (unsigned int)(par<0?0:par);
   if(maxNumberOfeTaus>0) {
      while( output.size()> maxNumberOfeTaus ) {
         output.pop_back();
      }
   }
   return TCS::StatusCode::SUCCESS;
}

