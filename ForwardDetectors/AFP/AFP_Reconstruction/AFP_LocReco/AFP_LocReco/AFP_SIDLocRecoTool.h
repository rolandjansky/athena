/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIDLOCRECOTOOL_H
#define AFP_SIDLOCRECOTOOL_H

#include "AFP_LocRecoInterfaces/IAFPSiDLocRecoTrackAlgTool.h"
#include "AFP_LocRecoInterfaces/IAFPSiDLocRecoTool.h"
#include "AFP_SiClusterTools/IAFPSiClusterTool.h"

#include "xAODForward/AFPTrackContainer.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h" 

#include <string>
#include <vector>
#include <map>
#include <utility>

/// Algorithm reconstructing tracks from hits clusters
class AFP_SIDLocRecoTool
  : virtual public ::IAFPSiDLocRecoTool,
    public AthAlgTool
{
public:
  AFP_SIDLocRecoTool(const std::string &type, const std::string &name, const IInterface *parent);

  /// Does nothing
  ~AFP_SIDLocRecoTool() override {}

  StatusCode initialize() override;

  /// Run pixel clustering tool and next run track reconstruction tools
  StatusCode execute();

  /// Does nothing
  StatusCode finalize() override;

private:
  /// @brief This method creates new track containers and saves them to storegate
  ///
  /// New xAOD::AFPTrackContainer objects are dynamically created and
  /// pointers to them are saved in the #m_trackContainersWithNames
  /// . The objects are also recorded to storegate with name taken
  /// from #m_trackContainersWithNames
  StatusCode makeNewTrackContainers ();

  /// Vector of tool handles to be used for tracks reconstruction
  ToolHandleArray<IAFPSiDLocRecoTrackAlgTool> m_recoToolsHandles; 

  /// Tool handle to pixels clustering algorithm
  ToolHandle<IAFPSiClusterTool> m_recoClusters;

  /// Map of output track containers and names under which they are saved in storegate
  std::map<std::string, xAOD::AFPTrackContainer*> m_trackContainersWithNames;

  /// @brief Vector of pairs of track reconstruction tools associated with output containers
  ///
  /// This structure allows saving output of multiple track
  /// reconstruction algorithm to the same container if the same
  /// output container name is specified in the tools and to different
  /// containers if different names are used.
  std::vector<std::pair<ToolHandle<IAFPSiDLocRecoTrackAlgTool>*, xAOD::AFPTrackContainer**> > m_recoToolsAndContainers;
};

#endif	//AFP_TDLOCRECO_h
