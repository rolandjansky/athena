/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKD3PDMAKER_TRACKSTATEONSURFACEFILLERTOOL_H
#define TRACKD3PDMAKER_TRACKSTATEONSURFACEFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"
#include "Identifier/Identifier.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class AtlasDetectorID;
class PixelID;
class SCT_ID;
class TRT_ID;
class MdtIdHelper;
class CscIdHelper;
class RpcIdHelper;
class TgcIdHelper;
class ITRT_CalDbSvc;

namespace Trk {
  class TrackStateOnSurface;
  class IResidualPullCalculator;
}

class ITRT_DriftFunctionTool;

namespace D3PD {

class TrackStateOnSurfaceFillerTool
  : public D3PD::BlockFillerTool<Trk::TrackStateOnSurface>
{
public:
  typedef D3PD::BlockFillerTool<Trk::TrackStateOnSurface> Base;

  TrackStateOnSurfaceFillerTool (const std::string& type,
				 const std::string& name,
				 const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const Trk::TrackStateOnSurface &p);

private:

  void clearData();

  void synchronizeFlags();

  void setFillVariable(std::string varname, bool dofill);
  bool getFillVariable(std::string varname);

  std::map<std::string, bool> m_variables;

  const AtlasDetectorID* m_idHelper;
  const PixelID* m_pixId; 
  const SCT_ID* m_sctId;
  const TRT_ID* m_trtId;
  const MdtIdHelper* m_mdtId;
  const CscIdHelper* m_cscId;
  const RpcIdHelper* m_rpcId;
  const TgcIdHelper* m_tgcId;

  ServiceHandle<ITRT_CalDbSvc> m_trtcaldbSvc;
  ServiceHandle<ITRT_StrawNeighbourSvc> m_neighbourSvc ;
  ServiceHandle<ITRT_StrawStatusSummarySvc> m_TRTStrawSummarySvc; 
  ToolHandle < Trk::IResidualPullCalculator > m_residualPullCalculator;
  ToolHandle < ITRT_DriftFunctionTool > 	m_driftFunctionTool; 

  /* steering **/
  std::map<std::string, bool> m_fillType;

  /* NTuple variables: **/
  int *m_type; // Measurement: 0, Outlier: 5, Hole: 6.
  Identifier::value_type *m_id; // unique identifier for a cluster
  Identifier::value_type *m_detElementId; // unique identifier for a detector element
  int *m_detType;
  int *m_bec;
  int *m_board;
  int *m_chip;
  bool *m_isArgonStraw;
  int *m_straw_layer;
  int *m_straw;
  int *m_phiModule;
  int *m_layer;
  float *m_charge; // defined for Pixel only
  int *m_sizePhi; // defined for Si only
  int *m_sizeZ; // defined for Si only
  int *m_size; // defined for Si only
  int *m_isFake; // defined for Pixel only
  int *m_isGanged; // defined for Pixel only
  int *m_isSplit; // defined for Pixel only
  float *m_splitProb1; // defined for Pixel only
  float *m_splitProb2; // defined for Pixel only
  int *m_isCompetingRIO;
  float *m_locX;
  float *m_locY;
  float *m_locR;
  float *m_incidencePhi;
  float *m_incidenceTheta;
  float *m_err_locX;
  float *m_err_locY;
  float *m_cov_locXY;
  float *m_err_locR;
  float *m_x;
  float *m_y;
  float *m_z;
  float *m_trkX;
  float *m_trkY;
  float *m_trkZ;
  float *m_trkLocX;
  float *m_trkLocY;
  float *m_trkLocR;
  float *m_trkLocPhi;
  float *m_err_trkLocX;
  float *m_err_trkLocY;
  float *m_cov_trkLocXY;
  float *m_err_trkLocR;
  float *m_err_trkLocPhi;
  float *m_cov_trkLocRPhi;
  float *m_TOT;
  float *m_driftTimeToTCorrection;
  float *m_driftTimeHTCorrection;
  float *m_t0;
  int   *m_highThreshold;
  int   *m_bitPattern;
  float *m_drifttime;
  float *m_trailingEdge;
  float *m_leadingEdge;
  int   *m_highLevel;
  float *m_locBiasedResidualX;
  float *m_locBiasedResidualY;
  float *m_locBiasedResidualR;
  float *m_locBiasedPullX;
  float *m_locBiasedPullY;
  float *m_locBiasedPullR;
  float *m_locUnbiasedResidualX;
  float *m_locUnbiasedResidualY;
  float *m_locUnbiasedResidualR;
  float *m_locUnbiasedPullX;
  float *m_locUnbiasedPullY;
  float *m_locUnbiasedPullR;
  

  

}; // class TrackTrackStateOnSurfaceFillerTool

} // namespace D3PD

#endif // not TRACKD3PDMAKER_TRACKSTATEONSURFACEFILLERTOOL_H
