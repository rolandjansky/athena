/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  ClusterNoSort.cxx
//  TopoCore
//  Created by V Sorin, Joerg Stelzer 
//  Algorithm to  create the all list of clusters , no order is applied
//
#include "L1TopoAlgorithms/ClusterNoSort.h"
#include "L1TopoEvent/TOBArray.h"
#include "L1TopoEvent/ClusterTOBArray.h"
#include "L1TopoEvent/GenericTOB.h"
#include <algorithm>

REGISTER_ALG_TCS(ClusterNoSort)

// constructor
TCS::ClusterNoSort::ClusterNoSort(const std::string & name) : SortingAlg(name) {
   defineParameter( "InputWidth", 120 ); // for fw
   defineParameter( "OutputWidth", 120 );    
   defineParameter( "IsoMask", 0);
}


// destructor
TCS::ClusterNoSort::~ClusterNoSort() {}

TCS::StatusCode
TCS::ClusterNoSort::initialize() {
   m_numberOfClusters = parameter("OutputWidth").value();
   m_iso = parameter("IsoMask").value();
   return TCS::StatusCode::SUCCESS;
}

TCS::StatusCode
TCS::ClusterNoSort::sort(const InputTOBArray & input, TOBArray & output) {

   const ClusterTOBArray & clusters = dynamic_cast<const ClusterTOBArray&>(input);

   // fill output array with GenericTOB buildt from clusters
   for(ClusterTOBArray::const_iterator cl = clusters.begin(); cl!= clusters.end(); ++cl ) {
      const GenericTOB gtob(**cl);

      // isolation cut
      if (m_iso != 0 ) {      
	unsigned int isobit(0x1 << (m_iso-1));
        if((parType_t((*cl)->isolation()) & isobit) != isobit ) continue;
      }
      
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

