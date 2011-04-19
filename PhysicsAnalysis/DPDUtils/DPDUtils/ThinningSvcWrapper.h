/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef THINNINGSVCWRAPPER_H
#define THINNINGSVCWRAPPER_H

/** 
    @class ThinningSvcWrapper
    @brief Wrapper class to use thinning service from python 
    @author Sven Menke
*/
#include "GaudiKernel/StatusCode.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "egammaEvent/ElectronContainer.h"
#include "muonEvent/MuonContainer.h"
#include "JetEvent/JetCollection.h"
#include "Particle/TrackParticleContainer.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "StoreGate/StoreGate.h" 
#include "StoreGate/DataHandle.h" 
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IThinningSvc.h"

class ThinningSvcWrapper
{
public:

  ThinningSvcWrapper() {}
    
  virtual ~ThinningSvcWrapper() { }

  template< class Container >
  StatusCode filter( const Container& coll, 
                     const std::vector<bool>& mask);
private:
  StatusCode filterINavigable4Momentum(const INavigable4MomentumCollection& coll,
				       const std::vector<bool>& mask) 
  {   return this->filter(coll,mask); }

  StatusCode filterElectron(const ElectronContainer& coll,
			    const std::vector<bool>& mask) 
  {   return this->filter(coll,mask); }

  StatusCode filterMuon(const Analysis::MuonContainer& coll,
			const std::vector<bool>& mask) 
  {   return this->filter(coll,mask); }

  StatusCode filterJet(const JetCollection& coll,
		       const std::vector<bool>& mask) 
  {   return this->filter(coll,mask); }

  StatusCode filterTrackParticle(const Rec::TrackParticleContainer& coll,
				 const std::vector<bool>& mask) 
  {   return this->filter(coll,mask); }

  StatusCode filterCaloCluster(const CaloClusterContainer& coll,
			       const std::vector<bool>& mask) 
  {   return this->filter(coll,mask); }
};

template< class Container >
StatusCode ThinningSvcWrapper::filter( const Container& coll, 
				       const std::vector<bool>& mask)
{
  static IThinningSvc* s_thinningsvc = 0;
  // retrieve ThinningSvc corresponding to the request
  if ( ! s_thinningsvc ) {
    
    StatusCode sc = Gaudi::svcLocator()->getService( "ThinningSvc", *pp_cast<IService>(&s_thinningsvc) );
    if ( sc.isFailure() ) {
      std::cerr << "ThinningSvc does not exist" << std::endl;
      return StatusCode::FAILURE;
    }
  }

//  for(unsigned int i=0;i<mask.size();i++) 
//    std::cerr << "[" << i << "] = " << (mask[i]?"True":"False") << std::endl;
 
  if ( !s_thinningsvc->filter( coll, mask ).isSuccess() )
    { std::cerr << "Could not thin container " << std::endl; }

  return StatusCode::SUCCESS;
}
 

#endif // THINNINGSVCWRAPPER_H
