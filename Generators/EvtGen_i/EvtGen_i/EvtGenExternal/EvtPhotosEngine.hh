
/***********************************************************************
* Copyright 1998-2022 CERN for the benefit of the EvtGen authors       *
*                                                                      *
* This file is part of EvtGen.                                         *
*                                                                      *
* EvtGen is free software: you can redistribute it and/or modify       *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation, either version 3 of the License, or    *
* (at your option) any later version.                                  *
*                                                                      *
* EvtGen is distributed in the hope that it will be useful,            *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU General Public License for more details.                         *
*                                                                      *
* You should have received a copy of the GNU General Public License    *
* along with EvtGen.  If not, see <https://www.gnu.org/licenses/>.     *
***********************************************************************/

#ifndef EVT_I_PHOTOSENGINE_HH
#define EVT_I_PHOTOSENGINE_HH

#include "EvtGenBase/EvtHepMCEvent.hh"
#include "EvtGenBase/EvtId.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtVector4R.hh"

#include "EvtGenModels/EvtAbsExternalGen.hh"
#ifdef HEPMC3
#include "HepMC3/Units.h"

#include "Photos/PhotosHepMC3Event.h"
#include "Photos/PhotosHepMC3Particle.h"
#else
#include "Photos/PhotosHepMCEvent.h"
#include "Photos/PhotosHepMCParticle.h"
#include "Photos/PhotosParticle.h"
#endif

#include <string>

// Description: Interface to the PHOTOS external generator

class EvtPhotosEngine : public EvtAbsExternalGen {
  public:
    EvtPhotosEngine( std::string photonType = "gamma",
                     bool useEvtGenRandom = true );

    bool doDecay( EvtParticle* theMother ) override;

    void initialise() override;

  private:
    std::string m_photonType;
    EvtId m_gammaId;
    int m_gammaPDG;
    double m_mPhoton;
    bool m_initialised;

    GenParticlePtr createGenParticle( EvtParticle* theParticle, bool incoming );
    int getNumberOfPhotons( const GenVertexPtr theVertex ) const;
};

#endif
