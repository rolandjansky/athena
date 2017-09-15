/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLEASSOCIATIONALGS_TRACKPARTICLECLUSTERASSOCIATIONALG_H
#define TRACKPARTICLEASSOCIATIONALGS_TRACKPARTICLECLUSTERASSOCIATIONALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>


namespace Rec {
  class IParticleCaloClusterAssociationTool;
}
namespace Trk {
  class ITrackSelectorTool;
}

class TrackParticleClusterAssociationAlg : public AthAlgorithm
{
 public:
  TrackParticleClusterAssociationAlg(const std::string& name, ISvcLocator* pSvcLocator);

  ~TrackParticleClusterAssociationAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  ToolHandle<Rec::IParticleCaloClusterAssociationTool> m_caloClusterAssociationTool;
  ToolHandle <Trk::ITrackSelectorTool>              m_trackSelector; //!< Tool to select tracks

  std::string m_trackParticleCollectionName;
  double m_ptCut;
  std::string m_outputPostFix;
  std::string m_caloClusters;
};


#endif
