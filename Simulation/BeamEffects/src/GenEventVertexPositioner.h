/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenEventVertexPositioner.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef BEAMEFFECTS_GENEVENTVERTEXPOSITIONER_H
#define BEAMEFFECTS_GENEVENTVERTEXPOSITIONER_H 1

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "HepMC_Interfaces/IGenEventManipulator.h"


namespace Simulation {

  // forward declarations
  class ILorentzVectorGenerator;

  /** @class GenEventVertexPositioner

      This tool takes a HepMC::GenEvent and applies geometrical modifications,
      such as random vertex smearing, beam tilt, etc.

      based on:
      https://svnweb.cern.ch/trac/atlasoff/browser/Simulation/G4Atlas/G4AtlasUtilities/trunk/src/VertexPositioner.cxx

      @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
     */
  class GenEventVertexPositioner : public extends<AthAlgTool, IGenEventManipulator> {

    public:
      /** Constructor with parameters */
      GenEventVertexPositioner( const std::string& t, const std::string& n, const IInterface* p );

      /** Athena algtool's Hooks */
      StatusCode  initialize() override final;
      StatusCode  finalize() override final;

      /** modifies (displaces) the given GenEvent */
      StatusCode  manipulate(HepMC::GenEvent& ge) const override final;

    private:
      /** Vertex Shifters applied in the given order */
      ToolHandleArray<ILorentzVectorGenerator> m_vertexShifters;
  };

}

#endif //> !BEAMEFFECTS_GENEVENTVERTEXPOSITIONER_H
