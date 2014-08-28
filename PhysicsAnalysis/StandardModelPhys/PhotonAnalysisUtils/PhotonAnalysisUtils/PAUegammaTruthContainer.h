/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHOTONANALYSISUTILS_PAUEGAMMATRUTHCONTAINER_H
#define PHOTONANALYSISUTILS_PAUEGAMMATRUTHCONTAINER_H

#include "PhotonAnalysisUtils/PAUegammaTruth.h"
#include "AthContainers/DataVector.h"
#include "SGTools/CLASS_DEF.h"

#include <iostream>
#include <map>

namespace PAU {
  
  class egammaTruthContainer : public DataVector<PAU::egammaTruth> {
  public:

    egammaTruthContainer(SG::OwnershipPolicy own = SG::OWN_ELEMENTS) 
      : DataVector<PAU::egammaTruth>(own) {} ;

    virtual ~egammaTruthContainer() {} ;

    void print() const {
      std::cout << "Number of PAU::egammaTruth objects = " << this->size() << std::endl ;
      for ( DataVector<PAU::egammaTruth>::const_iterator itr=begin() ; itr!=end() ; ++itr ) {
	std::cout << "\tE=" << (*itr)->e
		  << "\tpt=" << (*itr)->pt
		  << "\teta=" << (*itr)->eta
		  << "\tphi=" << (*itr)->phi
		  << std::endl ;
      }
    }
    
    void push_back(PAU::egammaTruth* g) {
      DataVector<PAU::egammaTruth>::push_back(g);
    }

    void push_back(const PAU::egammaTruth* g) {
      DataVector<PAU::egammaTruth>::push_back( const_cast<PAU::egammaTruth*>(g) );
    }

  };

}

CLASS_DEF(PAU::egammaTruthContainer, 1315176527, 1) 
SG_BASE(PAU::egammaTruthContainer, DataVector<PAU::egammaTruth> ); // < needed for auto-symlink feature

#endif // PHOTONANALYSISUTILS_PAUEGAMMATRUTHCONTAINER_H
