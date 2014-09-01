/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VertexBeamCondPositioner.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_HEPMC_VERTEXBEAMCONPOSITIONER_H
#define ISF_HEPMC_VERTEXBEAMCONPOSITIONER_H 1

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

  /** @class VertexBeamCondPositioner
  
      This AthenaTool computes geometrical shifts for the initial GenEvent vertices.
      
      based on:
      https://svnweb.cern.ch/trac/atlasoff/browser/Simulation/G4Atlas/G4AtlasUtilities/trunk/src/VertexPositioner.cxx
      
      @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
     */
  class VertexBeamCondPositioner : public AthAlgTool,
                                 virtual public ILorentzVectorGenerator { 
      
    public: 
      /** Constructor with parameters */
      VertexBeamCondPositioner( const std::string& t, const std::string& n, const IInterface* p );

      /** Destructor */
      ~VertexBeamCondPositioner();

      /** Athena algtool's Hooks */
      StatusCode  initialize();
      StatusCode  finalize();
	  
      /** computes the vertex displacement */
      CLHEP::HepLorentzVector  *generate();
	  
	private:
      ServiceHandle<IBeamCondSvc>     m_beamCondSvc;
      ServiceHandle<IAtRndmGenSvc>    m_rndGenSvc;
      CLHEP::HepRandomEngine*         m_randomEngine;

      std::string                     m_randomEngineName;         //!< Name of the random number stream
  }; 
  
}

#endif //> !ISF_HEPMC_VERTEXBEAMCONPOSITIONER_H
