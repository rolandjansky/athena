/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VertexBeamCondPositioner.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef BEAMEFFECTS_VERTEXBEAMCONPOSITIONER_H
#define BEAMEFFECTS_VERTEXBEAMCONPOSITIONER_H 1

// STL includes
#include <string>
#include <utility>
#include <vector>
#include <map>
// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "HepMC_Interfaces/ILorentzVectorGenerator.h"
// InDetBeamSpotService
#include "InDetBeamSpotService/IBeamCondSvc.h"

namespace ATHRNG {
  class RNGWrapper;
}

namespace Simulation {

  /** @class VertexBeamCondPositioner

      This AthenaTool computes geometrical shifts for the initial GenEvent vertices.

      based on:
      https://svnweb.cern.ch/trac/atlasoff/browser/Simulation/G4Atlas/G4AtlasUtilities/trunk/src/VertexPositioner.cxx

      @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
     */
  class VertexBeamCondPositioner : public extends<AthAlgTool, ILorentzVectorGenerator> {

    public:

      /** Constructor with parameters */
      VertexBeamCondPositioner( const std::string& t, const std::string& n, const IInterface* p );

      /** Destructor */
      ~VertexBeamCondPositioner();

      /** Athena algtool's Hooks */
      StatusCode  initialize() override final;
      StatusCode  finalize() override final;

      /** computes the vertex displacement */
      CLHEP::HepLorentzVector  *generate() const override final;

    private:

      ServiceHandle<IBeamCondSvc>     m_beamCondSvc;
      ServiceHandle<IAthRNGSvc>       m_rndGenSvc;
      ATHRNG::RNGWrapper*             m_randomEngine;             //!< Slot-local RNG

      std::string                     m_randomEngineName;         //!< Name of the random number stream
      bool                            m_timeSmearing;             //!< Do time smearing

  };

}

#endif //> !BEAMEFFECTS_VERTEXBEAMCONPOSITIONER_H
