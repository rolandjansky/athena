/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  ClusterSelect.cxx
//  TopoCore
//  Created by V Sorin, Joerg Stelzer 
//  Algorithm to select the abbreviated list of clusters , no order is applied
//
#include "L1TopoAlgorithms/ClusterSelect.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/ClusterTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(ClusterSelect)

// constructor
TCS::ClusterSelect::ClusterSelect(const std::string & name) : SortingAlg(name) {
   defineParameter( "InputWidth", 120 ); // for fw
   defineParameter( "InputWidth1stStage", 30 ); // for fw
   defineParameter( "OutputWidth", 6 );
   defineParameter( "MinET", 0 );
   defineParameter( "IsoMask", 0);
   defineParameter( "MinEta", 0 );
   defineParameter( "MaxEta", 63);
   defineParameter( "DoIsoCut", 1);
}


// destructor
TCS::ClusterSelect::~ClusterSelect() {}

TCS::StatusCode
TCS::ClusterSelect::initialize() {
   m_numberOfClusters = parameter("OutputWidth").value();
   m_et = parameter("MinET").value();
   m_iso = parameter("IsoMask").value();
   m_minEta = parameter("MinEta").value();
   m_maxEta = parameter("MaxEta").value();
   m_doIsoCut = parameter("DoIsoCut").value();
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::ClusterSelect::sort(const InputTOBArray & input, TOBArray & output) {

   const ClusterTOBArray & clusters = dynamic_cast<const ClusterTOBArray&>(input);

   // fill output array with GenericTOB buildt from clusters
   for(ClusterTOBArray::const_iterator cl = clusters.begin(); cl!= clusters.end(); ++cl ) {
      const GenericTOB gtob(**cl);

      if( parType_t((*cl)->Et()) <= m_et ) continue; // ET cut
      // isolation cut
      if (m_doIsoCut && (m_iso != 0 )) {
          if((parType_t((*cl)->isolation()) & m_iso) != m_iso ) continue;
      }
      // eta cut
      if (parType_t(std::abs((*cl)-> eta())) < m_minEta) continue; 
      if (parType_t(std::abs((*cl)-> eta())) > m_maxEta) continue;  
      
      output.push_back( gtob );
   }


   // keep only max number of clusters
   int par = m_numberOfClusters ;
   unsigned int maxNumberOfClusters = (unsigned int)(par<0?0:par);
   if(maxNumberOfClusters>0) {
      while( output.size()> maxNumberOfClusters ) {
         output.pop_back();
      }
   }
   return TCS::StatusCode::SUCCESS;
}

