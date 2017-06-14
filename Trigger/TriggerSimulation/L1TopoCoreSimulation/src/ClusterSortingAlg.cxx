//
//  ClusterSortingAlg.cxx
//  TopoCore
//
//  Created by Joerg Stelzer on 11/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.
//

#include "L1TopoCoreSimulation/ClusterSortingAlg.h"
#include "L1TopoCoreSimulation/TOBArray.h"
#include "L1TopoCoreSimulation/ClusterTOBArray.h"
#include "L1TopoCoreSimulation/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_NS(TCS,ClusterSortingAlg)

bool SortByEtLargestFirst(TCS::GenericTOB* tob1, TCS::GenericTOB* tob2)
{
   return tob1->Et() > tob2->Et();
}

bool SortByEtSmallestFirst(TCS::GenericTOB* tob1, TCS::GenericTOB* tob2)
{
   return tob1->Et() < tob2->Et();
}


// constructor
TCS::ClusterSortingAlg::ClusterSortingAlg(const std::string & name) : SortingAlg(name)
{
   defineParameter( "NumberOfClusters", 0 );
   defineParameter( "ReverseOrder", 0 );
}


TCS::StatusCode
TCS::ClusterSortingAlg::sort(const InputTOBArray & input, TOBArray & output) {
   const ClusterTOBArray & clusters = dynamic_cast<const ClusterTOBArray&>(input);

   // fill output array with GenericTOBs builds from clusters
   for(ClusterTOBArray::const_iterator cl = clusters.begin(); cl!= clusters.end(); ++cl ) {
      output.push_back( GenericTOB(**cl)  );
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

