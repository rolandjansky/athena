/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Filename: UpgradeLeptonObjectSelector.h
// Description:
// Author: Mark Owen
// Created: 19 Jan 2017

#ifndef _TOP_UPGRADE_LEPTONOBJECTSELECTOR_H_
#define _TOP_UPGRADE_LEPTONOBJECTSELECTOR_H_

#include "TopParticleLevel/ObjectSelectorBase.h"

#include "xAODTruth/TruthParticleContainer.h"

namespace top {
  
  class UpgradeLeptonObjectSelector
    : public ObjectSelectorBase<xAOD::TruthParticle> {
  public:
    // Struct used to store the cut values used by the object selector. It
    // is important that the object has a sensible default config because it
    // will be default constructed for the trivial constructor of the object
    // selector.
    struct Options {
    Options( double ptMin = 25.e3,
	     double etaMax = 2.5 ) :
      pt_min( ptMin ),
	eta_max( etaMax ) {}
      double  pt_min;
      double  eta_max;
      
    };
  public:
    UpgradeLeptonObjectSelector( Options opt = Options() );
    virtual bool apply( const xAOD::TruthParticle & truthParticle );
  private:
    Options m_opt;
  };
  
}

#endif /* _TOP_UPGRADE_LEPTONOBJECTSELECTOR_H_ */
