/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_SORTCLUSTERS_H
#define DCMATH_SORTCLUSTERS_H

#include <functional>
#include "TrkDriftCircleMath/Cluster.h"

namespace TrkDriftCircleMath {

  struct SortClustersByPosition : public std::binary_function<class Cluster, class Cluster, bool> {
    bool operator()(const Cluster& cl1, const Cluster& cl2 ) const {
      //      if( cl1.id().phi() < cl2.id().phi() ) return true;
      if( cl1.y() < cl2.y() ) return true;
      else if( cl1.y() == cl2.y() ){
	return cl1.x() < cl2.x();
      }else{
	return false;
      }
    }
 
  };

  struct SameCluster : public std::binary_function<class Cluster, class Cluster, bool> {
    bool operator()(const Cluster& cl1, const Cluster& cl2 ) const {
      //     if( cl1.id().phi() != cl2.id().phi() ) return false;
      if( cl1.y() == cl2.y() &&  cl1.x() == cl2.x() ) return true;
      return false;
    }
 
  };
}

#endif
