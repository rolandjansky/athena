/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWTRACKEXTRAPOLATORTOOL_H
#define EFLOWTRACKEXTRAPOLATORTOOL_H

/********************************************************************

 NAME:     eflowTrackExtrapolatorTool.h
 PACKAGE:  offline/Reconstruction/eflowRec

 AUTHORS:  T.Guttenberger
 CREATED:  17th September, 2014

 ********************************************************************/

#include "eflowRec/eflowTrackExtrapolatorBaseAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "CaloIdentifier/CaloCell_ID.h"

#include "TrkExInterfaces/IExtrapolator.h"

class eflowTrackCaloPoints;

static const InterfaceID IID_eflowTrackExtrapolatorTool("eflowTrackExtrapolatorTool", 1, 0);

class eflowTrackExtrapolatorTool: virtual public eflowTrackExtrapolatorBaseAlgTool,
                                  public AthAlgTool {
public:
  eflowTrackExtrapolatorTool(const std::string& type, const std::string& name,
                             const IInterface* parent);
  ~eflowTrackExtrapolatorTool() { }

  static const InterfaceID& interfaceID();

  StatusCode initialize();
  eflowTrackCaloPoints* execute(const xAOD::TrackParticle* track);
  StatusCode finalize();

private:
  eflowCalo::LAYER translate(int detectorID);

  ToolHandle<Trk::IExtrapolator> m_theTrackExtrapolatorTool;

  /* Count number of tracks seen for summary report in finalize */
  int m_tracksProcessed;
};

inline const InterfaceID& eflowTrackExtrapolatorTool::interfaceID() {
  return IID_eflowTrackExtrapolatorTool;
}

#endif
