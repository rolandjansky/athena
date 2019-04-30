/*
  Copyright (C) 2019 CERN for the benefit of the ATLAS collaboration
*/
// $Id: TrackCaloClusterAuxContainer_v2.cxx $

// Local include(s):
#include "xAODPFlow/versions/TrackCaloClusterAuxContainer_v2.h"
 
namespace xAOD {
 
  TrackCaloClusterAuxContainer_v2::TrackCaloClusterAuxContainer_v2()
    : AuxContainerBase() {
	AUX_VARIABLE(pt);
	AUX_VARIABLE(eta);
	AUX_VARIABLE(phi);
	AUX_VARIABLE(m);
	AUX_VARIABLE(taste);
	AUX_VARIABLE( trackParticleLink );
	AUX_VARIABLE( iparticleLinks );
  }
  
  void TrackCaloClusterAuxContainer_v2::dump() const {
    std::cout<<" Dumping TrackCaloClusterAuxContainer_v2"<<std::endl;
    std::cout<<"pt:";      
    std::copy(pt.begin()   , pt.end()   , std::ostream_iterator<float>(std::cout, ", "));
    std::cout<<"eta:";                  
    std::copy(eta.begin()  , eta.end()  , std::ostream_iterator<float>(std::cout, ", "));
    std::cout<<"phi:";                  
    std::copy(phi.begin()  , phi.end()  , std::ostream_iterator<float>(std::cout, ", "));
    std::cout<<"m:";                    
    std::copy(m.begin()    , m.end()    , std::ostream_iterator<float>(std::cout, ", "));
    std::cout<<"taste:";
    std::copy(taste.begin(), taste.end(), std::ostream_iterator<int>(std::cout, ", "));
  }
  
} // namespace xAOD
