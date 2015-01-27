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
#include "RecoToolInterfaces/ITrackIsolationTool.h"

class ITHistSvc;
class IMuonTPTool;

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

  std::string m_containerName;

  /// Athena configured tools
  ServiceHandle<ITHistSvc> m_histSvc;
  ToolHandle<xAOD::ITrackIsolationTool> m_iso_tool;
  std::vector<xAOD::Iso::IsolationType> m_iso_to_run;
};


#endif /* TRACKISOLATIONDECORALG_H_ */
