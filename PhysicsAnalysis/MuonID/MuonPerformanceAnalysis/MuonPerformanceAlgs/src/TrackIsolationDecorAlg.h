/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * TrackIsolationDecorAlg.h
 *
 *  Created on: Sep 18, 2014
 *      Author: goblirsc
 */

#ifndef TRACKISOLATIONDECORALG_H_
#define TRACKISOLATIONDECORALG_H_


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

# if !defined(XAOD_ANALYSIS)
#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "RecoToolInterfaces/ICaloTopoClusterIsolationTool.h"
# endif

class ITHistSvc;
class IMuonTPTool;

//  This is a dummy alg that just ensures that our isolation tools correctly get the geometry info they need...
//  Don't we all LOVE the transparent way athena works?


class TrackIsolationDecorAlg : public AthAlgorithm {

public:

  /// Constructor with parameters:
  TrackIsolationDecorAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor:
  ~TrackIsolationDecorAlg();

  /// Athena algorithm's Hooks
  StatusCode  initialize();
  StatusCode  execute();
  StatusCode  finalize();

private:

  /// Default constructor:
  TrackIsolationDecorAlg();

private:

  /// Athena configured tools
  ServiceHandle<ITHistSvc> m_histSvc;
# if !defined(XAOD_ANALYSIS)
  ToolHandle<xAOD::ITrackIsolationTool> m_track_iso_tool;
  ToolHandle<xAOD::ICaloTopoClusterIsolationTool> m_calo_iso_tool;
# endif
};


#endif /* TRACKISOLATIONDECORALG_H_ */
