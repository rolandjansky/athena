/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLEASSOCIATIONALGS_TRACKPARTICLECELLASSOCIATIONALG_H
#define TRACKPARTICLEASSOCIATIONALGS_TRACKPARTICLECELLASSOCIATIONALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>


namespace Rec {
  class IParticleCaloCellAssociationTool;
}
namespace Trk {
  class ITrackSelectorTool;
}

class TrackParticleCellAssociationAlg : public AthAlgorithm
{
 public:
  TrackParticleCellAssociationAlg(const std::string& name, ISvcLocator* pSvcLocator);

  ~TrackParticleCellAssociationAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  ToolHandle<Rec::IParticleCaloCellAssociationTool> m_caloCellAssociationTool;
  ToolHandle <Trk::ITrackSelectorTool>              m_trackSelector; //!< Tool to select tracks

  std::string m_trackParticleCollectionName;
  double m_ptCut;
  std::string m_outputPostFix;
};


#endif
