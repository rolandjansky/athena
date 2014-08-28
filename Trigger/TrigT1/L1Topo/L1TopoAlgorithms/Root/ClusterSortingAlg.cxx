//  ClusterSortingAlg.cxx
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoAlgorithms/ClusterSortingAlg.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/ClusterTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(ClusterSortingAlg)

bool SortByEtLargestFirst(TCS::GenericTOB* tob1, TCS::GenericTOB* tob2)
{
   return tob1->Et() > tob2->Et();
}

bool SortByEtSmallestFirst(TCS::GenericTOB* tob1, TCS::GenericTOB* tob2)
{
   return tob1->Et() < tob2->Et();
}

// constructor
TCS::ClusterSortingAlg::ClusterSortingAlg(const std::string & name) : SortingAlg(name) {
   defineParameter( "NumberOfClusters", 0 );
   defineParameter( "ReverseOrder", 0 );
   defineParameter( "MinIsolation", 0);
   defineParameter( "MinEta", -49);
   defineParameter( "MaxEta", 49);
}


// destructor
TCS::ClusterSortingAlg::~ClusterSortingAlg() {}

TCS::StatusCode
TCS::ClusterSortingAlg::sort(const InputTOBArray & input, TOBArray & output) {
   const ClusterTOBArray & clusters = dynamic_cast<const ClusterTOBArray&>(input);

   // fill output array with GenericTOB buildt from clusters
   for(ClusterTOBArray::const_iterator cl = clusters.begin(); cl!= clusters.end(); ++cl ) {
      const GenericTOB gtob(**cl);

      // isolation cut
      if((*cl)->isolation() < parameter("minIsolation").value()) continue;
      
      output.push_back( gtob );
   }

   // sort, possibly in reversed order
   int reverseOrder = parameter("ReverseOrder").value();
   if(reverseOrder==1) {
      output.sort(SortByEtSmallestFirst);
   } else {
      output.sort(SortByEtLargestFirst);
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

