/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkLinks/LinkToXAODNeutralParticle.h"
#include "xAODTracking/NeutralParticleContainer.h"
#include "xAODTracking/NeutralParticle.h"


namespace Trk
{
	LinkToXAODNeutralParticle::LinkToXAODNeutralParticle() : ElementLink<xAOD::NeutralParticleContainer>()
	{}

	LinkToXAODNeutralParticle::LinkToXAODNeutralParticle ( ElementLink<xAOD::NeutralParticleContainer>& link ) : ElementLink<xAOD::NeutralParticleContainer> ( link )
	{}

	LinkToXAODNeutralParticle::~LinkToXAODNeutralParticle()
	{}

        const NeutralParameters* LinkToXAODNeutralParticle::neutralParameters() const
	{
		if ( isValid() )
		{
			const xAOD::NeutralParticle * neut = this->cachedElement();
			if ( 0 != neut ) return dynamic_cast<const Trk::NeutralParameters* > ( &neut->perigeeParameters() );
			return 0;
		}
		return 0;
	}//end of parameters method

        Trk::LinkToXAODNeutralParticle* Trk::LinkToXAODNeutralParticle::clone() const
	{
		return new LinkToXAODNeutralParticle ( *this );
	}

	Trk::LinkToXAODNeutralParticle& Trk::LinkToXAODNeutralParticle::operator= ( const Trk::LinkToXAODNeutralParticle& rhs )
	{
		if ( this!=&rhs )
		{
			ElementLink<xAOD::NeutralParticleContainer>::operator= ( rhs );
		}
		return *this;
	}
}//end of namespace definitions
