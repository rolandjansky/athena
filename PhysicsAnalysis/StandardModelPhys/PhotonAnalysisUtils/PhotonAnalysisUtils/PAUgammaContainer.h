/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHOTONANALYSISUTILS_PAUGAMMACONTAINER_H
#define PHOTONANALYSISUTILS_PAUGAMMACONTAINER_H

#include "PhotonAnalysisUtils/PAUgamma.h"
#include "AthContainers/DataVector.h"
#include "SGTools/CLASS_DEF.h"

#include <iostream>
#include <map>

namespace PAU {
  
  class gammaContainer : public DataVector<PAU::gamma> {
  public:
    gammaContainer(SG::OwnershipPolicy own = SG::OWN_ELEMENTS) : DataVector<PAU::gamma>(own) {} ;
    virtual ~gammaContainer() {} ;
    void print() const {
      std::cout << "Number of PAU::gamma objects = " << this->size() << std::endl ;
      for ( DataVector<PAU::gamma>::const_iterator itr=begin() ; itr!=end() ; ++itr ) {
	std::cout << "\tE=" << (*itr)->e() 
		  << "\tpt=" << (*itr)->pt() 
		  << "\teta=" << (*itr)->eta() 
		  << "\tphi=" << (*itr)->phi() 
		  << std::endl ;
      }
    }
    PAU::gamma* getPAUgamma(const Analysis::Photon* ph)   { return m_mapPhotonToPAUgamma[ph] ; }
    PAU::gamma* getPAUgamma(const Analysis::Electron* el) { return m_mapElectronToPAUgamma[el] ; }
    void push_back(PAU::gamma* g) {
      DataVector<PAU::gamma>::push_back(g);
      if (g->photon())    m_mapPhotonToPAUgamma[g->photon()]     = g ;
      if (g->electron())  m_mapElectronToPAUgamma[g->electron()] = g ;
    }
    void push_back(const PAU::gamma* g) {
      DataVector<PAU::gamma>::push_back( const_cast<PAU::gamma*>(g) );
      if (g->photon())    m_mapPhotonToPAUgamma[g->photon()]     = const_cast<PAU::gamma*>(g) ;
      if (g->electron())  m_mapElectronToPAUgamma[g->electron()] = const_cast<PAU::gamma*>(g) ;
    }
  private:
    std::map<const Analysis::Photon*,PAU::gamma*>   m_mapPhotonToPAUgamma ;
    std::map<const Analysis::Electron*,PAU::gamma*> m_mapElectronToPAUgamma ;
  };

}

CLASS_DEF(PAU::gammaContainer, 1079214445, 1) 
SG_BASE(PAU::gammaContainer, DataVector<PAU::gamma> ); // < needed for auto-symlink feature

#endif // PHOTONANALYSISUTILS_PAUGAMMACONTAINER_H
