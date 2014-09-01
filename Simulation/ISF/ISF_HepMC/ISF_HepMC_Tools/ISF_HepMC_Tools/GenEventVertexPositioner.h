/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenEventVertexPositioner.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_HEPMC_GENEVENTVERTEXPOSITIONER_H
#define ISF_HEPMC_GENEVENTVERTEXPOSITIONER_H 1

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
// ISF includes
#include "ISF_HepMC_Interfaces/IGenEventManipulator.h"


namespace ISF {

  // forward declarations
  class ILorentzVectorGenerator;


  /** custom dataype definition for cleaner code */
  typedef ToolHandleArray<ILorentzVectorGenerator>    VertexShifters;


  /** @class GenEventVertexPositioner
  
      This tool takes a HepMC::GenEvent and applies geometrical modifications,
      such as random vertex smearing, beam tilt, etc.
      
      based on:
      https://svnweb.cern.ch/trac/atlasoff/browser/Simulation/G4Atlas/G4AtlasUtilities/trunk/src/VertexPositioner.cxx
      
      @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
     */
  class GenEventVertexPositioner : public AthAlgTool,
                                   virtual public IGenEventManipulator { 
      
    public: 
      /** Constructor with parameters */
      GenEventVertexPositioner( const std::string& t, const std::string& n, const IInterface* p );

      /** Athena algtool's Hooks */
      StatusCode  initialize();
      StatusCode  finalize();
	  
      /** modifies (displaces) the given GenEvent */
      StatusCode  manipulate(HepMC::GenEvent& ge);
	  
	private:
      /** Vertex Shifters applied in the given order */
      VertexShifters                 m_vertexShifters;
  }; 
  
}

#endif //> !ISF_HEPMC_GENEVENTVERTEXPOSITIONER_H
