
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
#ifndef EVT_I_PYTHIAENGINE_HH
#define EVT_I_PYTHIAENGINE_HH

#include "EvtGenBase/EvtDecayBase.hh"
#include "EvtGenBase/EvtId.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtVector4R.hh"

#include "EvtGenModels/EvtAbsExternalGen.hh"

#include "EvtGenExternal/EvtPythiaRandom.hh"

#include "Pythia8/ParticleData.h"
#include "Pythia8/Pythia.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

// Description: Interface to the Pytha 8 external generator

class EvtPythiaEngine : public EvtAbsExternalGen {
  public:
    EvtPythiaEngine( std::string xmlDir = "./xmldoc",
                     bool convertPhysCodes = false, bool useEvtGenRandom = true );

    virtual ~EvtPythiaEngine();

    bool doDecay( EvtParticle* theMother ) override;

    void initialise() override;

  private:
    void updateParticleLists();
    void updatePhysicsParameters();

    void createPythiaParticle( EvtId& particleId, int PDGCode );
    bool validPDGCode( int PDGCode );
    void updatePythiaDecayTable( EvtId& particleId, int aliasInt, int PDGCode );
    void storeDaughterInfo( EvtParticle* theParticle, int startInt );

    void clearDaughterVectors();
    void clearPythiaModeMap();

    void createDaughterEvtParticles( EvtParticle* theParent );

    int getModeInt( EvtDecayBase* decayModel );

    std::unique_ptr<Pythia8::Pythia> m_genericPythiaGen;
    std::unique_ptr<Pythia8::Pythia> m_aliasPythiaGen;
    Pythia8::Pythia* m_thePythiaGenerator;

    std::vector<int> m_daugPDGVector;
    std::vector<EvtVector4R> m_daugP4Vector;

    typedef std::map<int, std::vector<int>> PythiaModeMap;
    PythiaModeMap m_pythiaModeMap;

    bool m_convertPhysCodes, m_initialised, m_useEvtGenRandom;

    std::unique_ptr<EvtPythiaRandom> m_evtgenRandom;

    std::map<int, int> m_addedPDGCodes;
};

#endif

