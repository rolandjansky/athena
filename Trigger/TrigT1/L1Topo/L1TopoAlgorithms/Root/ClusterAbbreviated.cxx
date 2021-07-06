/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  ClusterAbbreviated.cxx
//  TopoCore
//  Created by V Sorin, Joerg Stelzer 
//  Algorithm to select the abbreviated list of clusters , no order is applied
//
#include "L1TopoAlgorithms/ClusterAbbreviated.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/ClusterTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(ClusterAbbreviated)

// constructor
TCS::ClusterAbbreviated::ClusterAbbreviated(const std::string & name) : SortingAlg(name) {
   defineParameter( "NumberOfClusters", 0 );
   defineParameter( "MinIsolation", 0);
   defineParameter( "MinEta", 0 );
   defineParameter( "MaxEta", 49);
}


// destructor
TCS::ClusterAbbreviated::~ClusterAbbreviated() {}

TCS::StatusCode
TCS::ClusterAbbreviated::sort(const InputTOBArray & input, TOBArray & output) {

   const ClusterTOBArray & clusters = dynamic_cast<const ClusterTOBArray&>(input);

   // fill output array with GenericTOB buildt from clusters
   for(ClusterTOBArray::const_iterator cl = clusters.begin(); cl!= clusters.end(); ++cl ) {
      const GenericTOB gtob(**cl);

      // isolation cut
      if((*cl)->isolation() < parameter("MinIsolation").value()) continue;
      // eta cut
      if (parType_t(std::abs((*cl)-> eta())) < parameter("MinEta").value()) continue; 
      if (parType_t(std::abs((*cl)-> eta())) > parameter("MaxEta").value()) continue;  
      
      output.push_back( gtob );
   }


   // keep only max number of clusters
   int par = parameter("NumberOfClusters").value();
   unsigned int maxNumberOfClusters = (unsigned int)(par<0?0:par);
   if(maxNumberOfClusters>0) {
      while( output.size()> maxNumberOfClusters ) {
         output.pop_back();
      }
   }
   return TCS::StatusCode::SUCCESS;
}

