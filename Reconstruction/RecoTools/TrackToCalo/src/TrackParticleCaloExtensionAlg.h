/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackParticleCaloExtensionAlg.h
//   Header file for class TrackParticleCaloExtensionAlg
///////////////////////////////////////////////////////////////////
// Top algorithm class for extrapolating tracks to Calo
///////////////////////////////////////////////////////////////////


#ifndef TRACKPARTICLECALOEXTENSIONALG_H
#define TRACKPARTICLECALOEXTENSIONALG_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrackCaloClusterRecInterfaces/IParticleExtrapolationTool.h"

namespace Trk {
  class ITrackSelectorTool;
}
namespace Rec {
  class IParticleCaloCellAssociationTool;
}

class TrackParticleCaloExtensionAlg : public AthAlgorithm  {

public:

  // Constructor with parameters:
  TrackParticleCaloExtensionAlg(const std::string &name,ISvcLocator *pSvcLocator);

  // Basic algorithm methods:
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  ToolHandle < IParticleExtrapolationTool>       m_caloExtensionTool; //!< Tool to make the step-wise extrapolation
  ToolHandle <Rec::IParticleCaloCellAssociationTool> m_caloCellAssociationTool; //!< Tool to make the step-wise extrapolation
  ToolHandle <Trk::ITrackSelectorTool>               m_trackSelector; //!< Tool to select tracks

  std::string m_trackParicleContainerName;
  unsigned int m_nevents;
  unsigned int m_totalTracks;
  unsigned int m_processedTracks;
  unsigned int m_extensions;

};

#endif 

