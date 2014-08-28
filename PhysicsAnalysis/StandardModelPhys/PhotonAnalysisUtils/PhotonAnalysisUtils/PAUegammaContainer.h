/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHOTONANALYSISUTILS_PAUEGAMMACONTAINER_H
#define PHOTONANALYSISUTILS_PAUEGAMMACONTAINER_H

#include "PhotonAnalysisUtils/PAUegamma.h"
#include "AthContainers/DataVector.h"
#include "SGTools/CLASS_DEF.h"

#include <iostream>
#include <map>

namespace PAU {
  
  class egammaContainer : public DataVector<PAU::egamma> {
  public:
    egammaContainer(SG::OwnershipPolicy own = SG::OWN_ELEMENTS) : DataVector<PAU::egamma>(own) {} ;
    virtual ~egammaContainer() {} ;
    void print() const {
      std::cout << "Number of PAU::egamma objects = " << this->size() << std::endl ;
      for ( DataVector<PAU::egamma>::const_iterator itr=begin() ; itr!=end() ; ++itr ) {
	std::cout << "\tE=" << (*itr)->e() 
		  << "\tpt=" << (*itr)->pt() 
		  << "\teta=" << (*itr)->eta() 
		  << "\tphi=" << (*itr)->phi() 
		  << std::endl ;
      }
    }
    PAU::egamma* getPAUegamma(const Analysis::Photon* ph)   { return m_mapPhotonToPAUegamma[ph] ; }
    PAU::egamma* getPAUegamma(const Analysis::Electron* el) { return m_mapElectronToPAUegamma[el] ; }
    void push_back(PAU::egamma* g) {
      DataVector<PAU::egamma>::push_back(g);
      if (g->photon())    m_mapPhotonToPAUegamma[g->photon()]     = g ;
      if (g->electron())  m_mapElectronToPAUegamma[g->electron()] = g ;
    }
    void push_back(const PAU::egamma* g) {
      DataVector<PAU::egamma>::push_back( const_cast<PAU::egamma*>(g) );
      if (g->photon())    m_mapPhotonToPAUegamma[g->photon()]     = const_cast<PAU::egamma*>(g) ;
      if (g->electron())  m_mapElectronToPAUegamma[g->electron()] = const_cast<PAU::egamma*>(g) ;
    }
  private:
    std::map<const Analysis::Photon*,PAU::egamma*>   m_mapPhotonToPAUegamma ;
    std::map<const Analysis::Electron*,PAU::egamma*> m_mapElectronToPAUegamma ;
  };

}

CLASS_DEF(PAU::egammaContainer, 1319397003, 1) 
SG_BASE(PAU::egammaContainer, DataVector<PAU::egamma> ); // < needed for auto-symlink feature

#endif // PHOTONANALYSISUTILS_PAUEGAMMACONTAINER_H
