/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Filename: ParticleLevelJetObjectSelector.h
// Description:
// Author: Fabian Wilk
// Created: Sun Feb 22 15:49:37 2015

#ifndef _TOP_PARTICLE_LEVEL_JETOBJECTSELECTOR_H_
#define _TOP_PARTICLE_LEVEL_JETOBJECTSELECTOR_H_

#include "TopParticleLevel/ObjectSelectorBase.h"

#include "xAODJet/JetContainer.h"

namespace top {

    class ParticleLevelJetObjectSelector
	: public ObjectSelectorBase<xAOD::Jet> {
    public:
	// Struct used to store the cut values used by the object selector. It
	// is important that the object has a sensible default config because it
	// will be default constructed for the trivial constructor of the object
	// selector.
	struct Options {
	    Options( double ptMin = 25.e3, double etaMax = 2.5 ) :
		pt_min( ptMin ), eta_max( etaMax ){}
	    double pt_min;
	    double eta_max;
	};
    public:
	ParticleLevelJetObjectSelector( Options opt = Options() );
	virtual bool apply( const xAOD::Jet & jet );
    private:
	Options m_opt;
    };

}


#endif /* _TOP_PARTICLE_LEVEL_JETOBJECTSELECTOR_H_ */
