/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Filename: ParticleLevelMuonObjectSelector.h
// Description:
// Author: Fabian Wilk
// Created: Sun Feb 22 15:16:10 2015

#ifndef _TOP_PARTICLE_LEVEL_MUONOBJECTSELECTOR_H_
#define _TOP_PARTICLE_LEVEL_MUONOBJECTSELECTOR_H_

#include "TopParticleLevel/ObjectSelectorBase.h"

#include "xAODTruth/TruthParticleContainer.h"

namespace top {

    class ParticleLevelMuonObjectSelector
	: public ObjectSelectorBase<xAOD::TruthParticle> {
    public:
	// Struct used to store the cut values used by the object selector. It
	// is important that the object has a sensible default config because it
	// will be default constructed for the trivial constructor of the object
	// selector.
	struct Options {
	    Options( double ptMin = 25.e3,
		     double etaMax = 2.5,
		     bool notFromHadron = true,
		     bool tauIsHadron = false ) :
		pt_min( ptMin ),
		eta_max( etaMax ),
		not_from_hadron( notFromHadron ),
		tau_is_hadron( tauIsHadron ){}
	    double  pt_min;
	    double  eta_max;
	    bool    not_from_hadron;
	    bool    tau_is_hadron;
	};
    public:
	ParticleLevelMuonObjectSelector( Options opt = Options() );
	virtual bool apply( const xAOD::TruthParticle & truthParticle );
    private:
	Options m_opt;
    };

}

#endif /* _TOP_PARTICLE_LEVEL_MUONOBJECTSELECTOR_H_ */
