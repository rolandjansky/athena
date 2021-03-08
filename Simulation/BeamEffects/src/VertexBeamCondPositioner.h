/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BEAMEFFECTS_VERTEXBEAMCONPOSITIONER_H
#define BEAMEFFECTS_VERTEXBEAMCONPOSITIONER_H 1

// STL includes
#include <string>
#include <utility>
// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "HepMC_Interfaces/ILorentzVectorGenerator.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
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
      ~VertexBeamCondPositioner() = default;

      /** Athena algtool's Hooks */
      StatusCode  initialize() override final;
      StatusCode  finalize() override final;

      /** computes the vertex displacement */
      CLHEP::HepLorentzVector  *generate() const override final;

    private:

      SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
      ServiceHandle<IAthRNGSvc> m_rndGenSvc{this, "RandomSvc", "AthRNGSvc"};
      ATHRNG::RNGWrapper*           m_randomEngine{};             //!< Slot-local RNG

      Gaudi::Property<std::string>  m_randomEngineName{this, "RandomStream", "VERTEX"};         //!< Name of the random number stream
      Gaudi::Property<bool>           m_timeSmearing{this, "SimpleTimeSmearing", false};             //!< Do time smearing

  };

}

#endif //> !BEAMEFFECTS_VERTEXBEAMCONPOSITIONER_H
