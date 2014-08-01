/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWTRACKTOCALOTRACKEXTRAPOLATORTOOL_H
#define EFLOWTRACKTOCALOTRACKEXTRAPOLATORTOOL_H

/********************************************************************

NAME:     eflowTrackToCaloTrackExtrapolatorTool.h
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  25th January, 2005

********************************************************************/

#include "eflowRec/eflowTrackExtrapolatorBaseAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "CaloIdentifier/CaloCell_ID.h"

#include "RecoToolInterfaces/IExtrapolateToCaloTool.h"

static const InterfaceID IID_eflowTrackToCaloTrackExtrapolatorTool("eflowTrackToCaloTrackExtrapolatorTool", 1, 0);

class eflowTrackToCaloTrackExtrapolatorTool : virtual public eflowTrackExtrapolatorBaseAlgTool, public AthAlgTool {

  public:
  
  eflowTrackToCaloTrackExtrapolatorTool(const std::string& type,const std::string& name,const IInterface* parent);

  ~eflowTrackToCaloTrackExtrapolatorTool() {};

  static const InterfaceID& interfaceID();

  StatusCode initialize();
  Amg::Vector3D  execute(const xAOD::TrackParticle* track);
  std::vector<Amg::Vector3D> execute(const xAOD::TrackParticle* track, const std::vector<CaloCell_ID::CaloSample>& samples);
  bool isInBarrel(const xAOD::TrackParticle* track);
  StatusCode finalize();

  StatusCode setGeometry(CaloCell_ID::CaloSample subCalo);

 private:

  bool m_isTauMode;

  CaloCell_ID::CaloSample m_subCalo;

  std::string m_trackExtrapolatorToolName;
  ToolHandle<IExtrapolateToCaloTool> m_theTrackExtrapolatorTool;

  const Trk::TrackParameters* extrapolate(const xAOD::TrackParticle* track,
                                         CaloCell_ID::CaloSample targetLayer);
  const Trk::TrackParameters* extrapolate(const Trk::TrackParameters* startingPoint,
                                         CaloCell_ID::CaloSample targetLayer);
  std::vector<const Trk::TrackParameters*> extrapolate(const xAOD::TrackParticle* track,
                                                      const std::vector<CaloCell_ID::CaloSample>& targetLayers);
  Amg::Vector3D getPosition(const Trk::TrackParameters* extrapolatedParameters);
  std::vector<Amg::Vector3D > getPositions(std::vector<const Trk::TrackParameters*> extrapolatedParameters);
};

inline const InterfaceID& eflowTrackToCaloTrackExtrapolatorTool::interfaceID()
{ 
  return IID_eflowTrackToCaloTrackExtrapolatorTool; 
}

#endif
