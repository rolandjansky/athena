/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BEAMEFFECTS_LONGBEAMSPOTVERTEXPOSITIONER_H
#define BEAMEFFECTS_LONGBEAMSPOTVERTEXPOSITIONER_H 1

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
#include "BeamSpotConditionsData/BeamSpotData.h"
namespace ATHRNG {
  class RNGWrapper;
}
namespace CLHEP {
  class HepRandomEngine;
}

namespace Simulation {

  /** @class LongBeamspotVertexPositioner

      This AthenaTool computes geometrical shifts for the initial GenEvent vertices.

      based on function in:  https://its.cern.ch/jira/ATLASSIM-265

      @author John.Chapman -at- cern.ch, Elmar.Ritsch -at- cern.ch
     */
  class LongBeamspotVertexPositioner : public extends<AthAlgTool, ILorentzVectorGenerator> {

    public:

      /** Constructor with parameters */
      LongBeamspotVertexPositioner( const std::string& t, const std::string& n,
                                    const IInterface* p );

      /** Destructor */
      virtual ~LongBeamspotVertexPositioner() = default;

      /** Athena algtool's Hooks */
      StatusCode initialize() override final;
      StatusCode finalize() override final;

      /** computes the vertex displacement */
      CLHEP::HepLorentzVector *generate() const override final;

    private:

      inline double heaviside(double val) const {return (val >= 0.0) ? 1.0 : 0.0;};
      double getZpos(CLHEP::HepRandomEngine*) const;
      double beamspotFunction(double z) const;
      Gaudi::Property<double> m_L{this, "LParameter", 150.}; //!< Parameter in the Z distribution of the beamspot - default 150.0 mm
      SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
      ServiceHandle<IAthRNGSvc>   m_rndGenSvc{this, "RandomSvc", "AthRNGSvc"};
      ATHRNG::RNGWrapper*             m_randomEngine{};             //!< Slot-local RNG

      Gaudi::Property<std::string>    m_randomEngineName{this, "RandomStream", "VERTEX"};         //!< Name of the random number stream
      Gaudi::Property<bool>             m_timeSmearing{this, "SimpleTimeSmearing", false};             //!< Do time smearing

  };

}

#endif //> !BEAMEFFECTS_LONGBEAMSPOTVERTEXPOSITIONER_H
