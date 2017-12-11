/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VertexBeamCondPositioner.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

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
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "HepMC_Interfaces/ILorentzVectorGenerator.h"
// InDetBeamSpotService
#include "InDetBeamSpotService/IBeamCondSvc.h"

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
      ~LongBeamspotVertexPositioner();

      /** Athena algtool's Hooks */
      StatusCode initialize() override final;
      StatusCode finalize() override final;

      /** computes the vertex displacement */
      CLHEP::HepLorentzVector *generate() const override final;

    private:

      inline double heaviside(double val) const {return (val >= 0.0) ? 1.0 : 0.0;};
      double getZpos() const;
      double beamspotFunction(double z) const;
      double m_L; //!< Parameter in the Z distribution of the beamspot
      ServiceHandle<IBeamCondSvc>     m_beamCondSvc;
      ServiceHandle<IAtRndmGenSvc>    m_rndGenSvc;
      CLHEP::HepRandomEngine*         m_randomEngine;

      std::string                     m_randomEngineName;         //!< Name of the random number stream
      bool                            m_timeSmearing;             //!< Do time smearing

  };

}

#endif //> !BEAMEFFECTS_LONGBEAMSPOTVERTEXPOSITIONER_H
