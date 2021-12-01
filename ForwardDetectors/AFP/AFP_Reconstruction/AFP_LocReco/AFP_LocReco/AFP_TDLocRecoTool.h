/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDLOCRECOTOOL_H
#define AFP_TDLOCRECOTOOL_H

#include "AFP_LocRecoInterfaces/IAFPTDLocRecoTrackAlgTool.h"
#include "AFP_LocRecoInterfaces/IAFPTDLocRecoTool.h"

#include "xAODForward/AFPToFTrackContainer.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h" 

#include <string>
#include <vector>
#include <map>
#include <utility>

/// Algorithm reconstructing tracks from hits 
class AFP_TDLocRecoTool
  : virtual public ::IAFPTDLocRecoTool,
    public AthAlgTool
{
public:
  AFP_TDLocRecoTool(const std::string &type, const std::string &name, const IInterface *parent);

  /// Does nothing
  ~AFP_TDLocRecoTool() override {}

  StatusCode initialize() override;

  /// Run pixel clustering tool and next run track reconstruction tools
  StatusCode execute();

  /// Does nothing
  StatusCode finalize() override;

private:
  /// @brief This method creates new track containers and saves them to storegate
  ///
  /// New xAOD::AFPToFTrackContainer objects are dynamically created and
  /// pointers to them are saved in the #m_trackContainersWithNames
  /// . The objects are also recorded to storegate with name taken
  /// from #m_trackContainersWithNames
  StatusCode makeNewTrackContainers ();

  /// Vector of tool handles to be used for tracks reconstruction
  ToolHandleArray<IAFPTDLocRecoTrackAlgTool> m_recoToolsHandles; 


  /// Map of output track containers and names under which they are saved in storegate
  std::map<std::string, xAOD::AFPToFTrackContainer*> m_trackContainersWithNames;

  /// @brief Vector of pairs of track reconstruction tools associated with output containers
  ///
  /// This structure allows saving output of multiple track
  /// reconstruction algorithm to the same container if the same
  /// output container name is specified in the tools and to different
  /// containers if different names are used.
  std::vector<std::pair<ToolHandle<IAFPTDLocRecoTrackAlgTool>*, xAOD::AFPToFTrackContainer**> > m_recoToolsAndContainers;
};

#endif	//AFP_TDLOCRECOTOOL_h
