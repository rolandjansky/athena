/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VertexBeamCondPositioner.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_HEPMC_LONGBEAMSPOTVERTEXPOSITIONER_H
#define ISF_HEPMC_LONGBEAMSPOTVERTEXPOSITIONER_H 1

// STL includes
#include <string>
#include <utility>
#include <vector>
#include <map>
// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
// ISF includes
#include "ISF_HepMC_Interfaces/ILorentzVectorGenerator.h"
// InDetBeamSpotService
#include "InDetBeamSpotService/IBeamCondSvc.h"

namespace CLHEP {
    class HepRandomEngine;
}

namespace ISF {

  /** @class LongBeamspotVertexPositioner

      This AthenaTool computes geometrical shifts for the initial GenEvent vertices.

      based on function in:  https://its.cern.ch/jira/ATLASSIM-265

      @author John.Chapman -at- cern.ch, Elmar.Ritsch -at- cern.ch
     */
  class LongBeamspotVertexPositioner : public AthAlgTool,
                                 virtual public ILorentzVectorGenerator {

    public:
      /** Constructor with parameters */
      LongBeamspotVertexPositioner( const std::string& t, const std::string& n, const IInterface* p );

      /** Destructor */
      ~LongBeamspotVertexPositioner();

      /** Athena algtool's Hooks */
      StatusCode  initialize();
      StatusCode  finalize();

      /** computes the vertex displacement */
      CLHEP::HepLorentzVector  *generate();

  private:

      inline double heaviside(double val) const {return (val >= 0.0) ? 1.0 : 0.0;};
      double getZpos() const;
      double beamspotFunction(double z) const;
      double m_L; //!< Parameter in the Z distribution of the beamspot
      ServiceHandle<IBeamCondSvc>     m_beamCondSvc;
      ServiceHandle<IAtRndmGenSvc>    m_rndGenSvc;
      CLHEP::HepRandomEngine*         m_randomEngine;

      std::string                     m_randomEngineName;         //!< Name of the random number stream
  };

}

#endif //> !ISF_HEPMC_VERTEXBEAMCONPOSITIONER_H
