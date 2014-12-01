/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackStateOnSurfaceFillerTool.h"
 
#include "AthenaKernel/errorcheck.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/TrackStateDefs.h"
#include "TrkParameters/TrackParameters.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawNeighbourSvc.h"
#include "TRT_DriftFunctionTool/ITRT_DriftFunctionTool.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"



namespace D3PD {


TrackStateOnSurfaceFillerTool::TrackStateOnSurfaceFillerTool (const std::string& type,
				      const std::string& name,
				      const IInterface* parent)
  : D3PD::BlockFillerTool<Trk::TrackStateOnSurface> (type, name, parent),
    m_idHelper(0),
    m_pixId(0),
    m_sctId(0),
    m_trtId(0),
    m_trtcaldbSvc("TRT_CalDbSvc",name),
    m_neighbourSvc("TRT_StrawNeighbourSvc",name), 
    m_TRTStrawSummarySvc("InDetTRTStrawStatusSummarySvc",name),
    m_driftFunctionTool("TRT_DriftFunctionTool")
{
  m_fillType["FillPixelHits"] = true;
  m_fillType["FillSCTHits"] = true;
  m_fillType["FillTRTHits"] = true;
  m_fillType["FillMDTHits"] = true;
  m_fillType["FillRPCHits"] = true;
  m_fillType["FillTGCHits"] = true;
  m_fillType["FillCSCHits"] = true;
  m_fillType["FillPixelHoles"] = true;
  m_fillType["FillSCTHoles"] = true;
  m_fillType["FillTRTHoles"] = true;
  m_fillType["FillMDTHoles"] = true;
  m_fillType["FillCSCHoles"] = true;
  m_fillType["FillTGCHoles"] = true;
  m_fillType["FillRPCHoles"] = true;
  m_fillType["FillPulls"] = true;
  book().ignore(); // Avoid coverity warnings

  for(std::map<std::string, bool>::iterator itr=m_fillType.begin();
      itr!=m_fillType.end(); itr++)
  {
    itr->second = false;
    declareProperty(itr->first, itr->second);
  }
  declareProperty("ITRT_CalDbSvc",m_trtcaldbSvc);
  declareProperty("NeighbourSvc",m_neighbourSvc);
  declareProperty("TRTStrawSummarySvc",  m_TRTStrawSummarySvc);
  declareProperty("TRTDriftFunctionTool", m_driftFunctionTool);
  declareProperty("ResidualPullCalculator", m_residualPullCalculator);
}

StatusCode TrackStateOnSurfaceFillerTool::initialize(){

  CHECK( Base::initialize() );

  // need Atlas id-helpers to identify sub-detectors, take them from detStore
  if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
    REPORT_MESSAGE (MSG::ERROR) << "Could not get AtlasDetectorID helper";
    return StatusCode::FAILURE;
  }   

  if (detStore()->retrieve(m_pixId,"PixelID").isFailure() ){
    REPORT_MESSAGE (MSG::FATAL) << "Unable to retrieve pixel ID helper";
    return StatusCode::FAILURE;
  }

  if(detStore()->retrieve(m_sctId,"SCT_ID").isFailure() ){
    REPORT_MESSAGE (MSG::FATAL) << "Could not retrieve SCT helper";
    return StatusCode::FAILURE; 
  } 

  if(detStore()->retrieve(m_trtId,"TRT_ID").isFailure() ){
    REPORT_MESSAGE (MSG::FATAL) << "Could not retrieve TRT helper";
    return StatusCode::FAILURE; 
  } 

  if(detStore()->retrieve(m_mdtId).isFailure() ){
    REPORT_MESSAGE (MSG::FATAL) << "Could not retrieve MDT helper";
    return StatusCode::FAILURE; 
  }
  
  if(detStore()->retrieve(m_cscId).isFailure() ){
    REPORT_MESSAGE (MSG::FATAL) << "Could not retrieve CSC helper";
    return StatusCode::FAILURE; 
  }
  
  if(detStore()->retrieve(m_rpcId).isFailure() ){
    REPORT_MESSAGE (MSG::FATAL) << "Could not retrieve RPC helper";
    return StatusCode::FAILURE; 
  }
  
  if(detStore()->retrieve(m_tgcId).isFailure() ){
    REPORT_MESSAGE (MSG::FATAL) << "Could not retrieve TGC helper";
    return StatusCode::FAILURE; 
  }

  if(m_fillType["FillPulls"]){
    CHECK(m_residualPullCalculator.retrieve());
  }

  CHECK(m_trtcaldbSvc.retrieve());

  CHECK(m_neighbourSvc.retrieve());

  CHECK(m_TRTStrawSummarySvc.retrieve());

  CHECK(m_driftFunctionTool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode TrackStateOnSurfaceFillerTool::book(){

  synchronizeFlags();

  if(getFillVariable("type"))                  CHECK( addVariable("type", m_type) );
  if(getFillVariable("id"))                    CHECK( addVariable("id", m_id) );
  if(getFillVariable("detElementId"))          CHECK( addVariable("detElementId", m_detElementId) );
  if(getFillVariable("detType"))               CHECK( addVariable("detType", m_detType) );
  if(getFillVariable("bec"))                   CHECK( addVariable("bec", m_bec) );
  if(getFillVariable("isArgonStraw"))          CHECK( addVariable("isArgonStraw", m_isArgonStraw) );
  if(getFillVariable("chip"))                  CHECK( addVariable("chip", m_chip) );
  if(getFillVariable("board"))                 CHECK( addVariable("board", m_board) );
  if(getFillVariable("layer"))                 CHECK( addVariable("layer", m_layer) );
  if(getFillVariable("straw_layer"))           CHECK( addVariable("straw_layer", m_straw_layer) );
  if(getFillVariable("straw"))                 CHECK( addVariable("straw", m_straw) );
  if(getFillVariable("charge"))                CHECK( addVariable("charge", m_charge) );
  if(getFillVariable("sizePhi"))               CHECK( addVariable("sizePhi", m_sizePhi) );
  if(getFillVariable("sizeZ"))                 CHECK( addVariable("sizeZ", m_sizeZ) );
  if(getFillVariable("size"))                  CHECK( addVariable("size", m_size) );
  if(getFillVariable("isFake"))                CHECK( addVariable ("isFake", m_isFake) );
  if(getFillVariable("isGanged"))              CHECK( addVariable ("isGanged", m_isGanged) );
  if(getFillVariable("isSplit"))               CHECK( addVariable ("isSplit", m_isSplit) );
  if(getFillVariable("splitProb1"))            CHECK( addVariable ("splitProb1", m_splitProb1) );
  if(getFillVariable("splitProb2"))            CHECK( addVariable ("splitProb2", m_splitProb2) );
  if(getFillVariable("isCompetingRIO"))        CHECK( addVariable("isCompetingRIO", m_isCompetingRIO) );
  if(getFillVariable("locX"))                  CHECK( addVariable("locX", m_locX) );
  if(getFillVariable("locY"))                  CHECK( addVariable("locY", m_locY) );
  if(getFillVariable("locR"))                  CHECK( addVariable("locR", m_locR) );
  if(getFillVariable("incidencePhi"))          CHECK( addVariable("incidencePhi", m_incidencePhi) );
  if(getFillVariable("incidenceTheta"))        CHECK( addVariable("incidenceTheta", m_incidenceTheta) );
  if(getFillVariable("err_locX"))              CHECK( addVariable("err_locX", m_err_locX) );
  if(getFillVariable("err_locY"))              CHECK( addVariable("err_locY", m_err_locY) );
  if(getFillVariable("cov_locXY"))             CHECK( addVariable("cov_locXY", m_cov_locXY) );
  if(getFillVariable("err_locR"))              CHECK( addVariable("err_locR", m_err_locR) );
  if(getFillVariable("x"))                     CHECK( addVariable("x", m_x) );
  if(getFillVariable("y"))                     CHECK( addVariable("y", m_y) );
  if(getFillVariable("z"))                     CHECK( addVariable("z", m_z) );
  if(getFillVariable("trkX"))                  CHECK( addVariable("trkX", m_trkX) );
  if(getFillVariable("trkY"))                  CHECK( addVariable("trkY", m_trkY) );
  if(getFillVariable("trkZ"))                  CHECK( addVariable("trkZ", m_trkZ) );
  if(getFillVariable("trkLocX"))               CHECK( addVariable("trkLocX", m_trkLocX) );
  if(getFillVariable("trkLocY"))               CHECK( addVariable("trkLocY", m_trkLocY) );
  if(getFillVariable("trkLocR"))               CHECK( addVariable("trkLocR", m_trkLocR) );
  if(getFillVariable("trkLocPhi"))             CHECK( addVariable("trkLocPhi", m_trkLocPhi) );
  if(getFillVariable("err_trkLocX"))           CHECK( addVariable("err_trkLocX", m_err_trkLocX) );
  if(getFillVariable("err_trkLocY"))           CHECK( addVariable("err_trkLocY", m_err_trkLocY) );
  if(getFillVariable("cov_trkLocXY"))          CHECK( addVariable("cov_trkLocXY", m_cov_trkLocXY) );
  if(getFillVariable("err_trkLocR"))           CHECK( addVariable("err_trkLocR", m_err_trkLocR) );
  if(getFillVariable("err_trkLocPhi"))         CHECK( addVariable("err_trkLocPhi", m_err_trkLocPhi) );
  if(getFillVariable("cov_trkLocRPhi"))        CHECK( addVariable("cov_trkLocRPhi", m_cov_trkLocRPhi) );
  if(getFillVariable("TOT"))                   CHECK( addVariable("TOT", m_TOT) );
  if(getFillVariable("driftTimeToTCorrection"))CHECK( addVariable("driftTimeToTCorrection", m_driftTimeToTCorrection) );
  if(getFillVariable("driftTimeHTCorrection")) CHECK( addVariable("driftTimeHTCorrection", m_driftTimeHTCorrection) );
  if(getFillVariable("t0"))                    CHECK( addVariable("t0", m_t0) );
  if(getFillVariable("bitPattern"))            CHECK( addVariable("bitPattern", m_bitPattern) );
  if(getFillVariable("drifttime"))             CHECK( addVariable("drifttime", m_drifttime) );
  if(getFillVariable("highThreshold"))         CHECK( addVariable("highThreshold", m_highThreshold) );
  if(getFillVariable("trailingEdge"))          CHECK( addVariable("trailingEdge", m_trailingEdge) );
  if(getFillVariable("leadingEdge"))           CHECK( addVariable("leadingEdge", m_leadingEdge) );
  if(getFillVariable("phiModule"))             CHECK( addVariable("phiModule", m_phiModule) );
  if(getFillVariable("highLevel"))             CHECK( addVariable("highLevel", m_highLevel) );
  if(getFillVariable("locBiasedResidualX"))    CHECK( addVariable("locBiasedResidualX", m_locBiasedResidualX) );
  if(getFillVariable("locBiasedResidualY"))    CHECK( addVariable("locBiasedResidualY", m_locBiasedResidualY) );
  if(getFillVariable("locBiasedResidualR"))    CHECK( addVariable("locBiasedResidualR", m_locBiasedResidualR) );
  if(getFillVariable("locBiasedPullX"))        CHECK( addVariable("locBiasedPullX", m_locBiasedPullX) );
  if(getFillVariable("locBiasedPullY"))        CHECK( addVariable("locBiasedPullY", m_locBiasedPullY) );
  if(getFillVariable("locBiasedPullR"))        CHECK( addVariable("locBiasedPullR", m_locBiasedPullR) );
  if(getFillVariable("locUnbiasedResidualX"))  CHECK( addVariable("locUnbiasedResidualX", m_locUnbiasedResidualX) );
  if(getFillVariable("locUnbiasedResidualY"))  CHECK( addVariable("locUnbiasedResidualY", m_locUnbiasedResidualY) );
  if(getFillVariable("locUnbiasedResidualR"))  CHECK( addVariable("locUnbiasedResidualR", m_locUnbiasedResidualR) );
  if(getFillVariable("locUnbiasedPullX"))      CHECK( addVariable("locUnbiasedPullX", m_locUnbiasedPullX) );
  if(getFillVariable("locUnbiasedPullY"))      CHECK( addVariable("locUnbiasedPullY", m_locUnbiasedPullY) );
  if(getFillVariable("locUnbiasedPullR"))      CHECK( addVariable("locUnbiasedPullR", m_locUnbiasedPullR) );

  return StatusCode::SUCCESS;
}

StatusCode TrackStateOnSurfaceFillerTool::fill (const Trk::TrackStateOnSurface& tso)
{
  this->clearData();

  if(getFillVariable("type")) *m_type = (int)tso.types().to_ulong();

  const Trk::MeasurementBase *measurement = tso.measurementOnTrack();
  const Trk::TrackParameters *tp = tso.trackParameters();

  if(measurement){
    // local position
    const Trk::LocalParameters pars = measurement->localParameters();
    if(getFillVariable("locX")) *m_locX = pars[Trk::locX];
    if(getFillVariable("locY")) *m_locY = pars.contains(Trk::locY)?pars[Trk::locY]:-99999.;
    if(getFillVariable("locR")) *m_locR = pars.contains(Trk::locR)?pars[Trk::locR]:-99999.;
     
    //local error matrix
    const Amg::MatrixX& err = measurement->localCovariance();
    if(getFillVariable("err_locX")) *m_err_locX =sqrt( err(Trk::locX,Trk::locX) );
    if(getFillVariable("err_locY")) *m_err_locY = pars.contains(Trk::locY)?sqrt( err(Trk::locY,Trk::locY) ):-99999.;
    if(getFillVariable("cov_locXY")) *m_cov_locXY = pars.contains(Trk::locY)?err(Trk::locX, Trk::locY):-99999.;
    if(getFillVariable("err_locR")) *m_err_locR = sqrt( err(Trk::locR,Trk::locR) );

    // global position
    const Amg::Vector3D pos = measurement->globalPosition();
    if(getFillVariable("x")) *m_x = pos[Trk::x];
    if(getFillVariable("y")) *m_y = pos[Trk::y];
    if(getFillVariable("z")) *m_z = pos[Trk::z];
  }

  if(tp){
    Amg::VectorX lp = tp->parameters();
    if(getFillVariable("trkLocX")) *m_trkLocX=lp[Trk::locX];
    if(getFillVariable("trkLocY")) *m_trkLocY=lp[Trk::locY];
    if(getFillVariable("trkLocR")) *m_trkLocR=lp[Trk::locR];
    if(getFillVariable("trkLocPhi")) *m_trkLocPhi=lp[Trk::locPhi];

    const AmgSymMatrix(5)* err = tp->covariance();
 
    if(err){
      if(getFillVariable("err_trkLocX")) *m_err_trkLocX=sqrt((*err)(Trk::locX,Trk::locX));
      if(getFillVariable("err_trkLocY")) *m_err_trkLocY=sqrt((*err)(Trk::locY,Trk::locY));
      if(getFillVariable("cov_trkLocXY")) *m_cov_trkLocXY=(*err)(Trk::locX, Trk::locY);
      if(getFillVariable("err_trkLocR")) *m_err_trkLocR=sqrt((*err)(Trk::locR,Trk::locR));
      if(getFillVariable("err_trkLocPhi")) *m_err_trkLocPhi=sqrt((*err)(Trk::locPhi,Trk::locPhi));
      if(getFillVariable("cov_trkLocRPhi")) *m_cov_trkLocRPhi=(*err)(Trk::locR,Trk::locPhi);
    }
  }

  if(tp && measurement){
    if(m_fillType["FillPulls"]){
      const Trk::ResidualPull *biased = m_residualPullCalculator->residualPull(measurement, tp, Trk::ResidualPull::Biased);
      const Trk::ResidualPull *unbiased = m_residualPullCalculator->residualPull(measurement, tp, Trk::ResidualPull::Unbiased);
      if(getFillVariable("locBiasedResidualX") && biased) *m_locBiasedResidualX = (biased->dimension()>Trk::locX ? biased->residual()[Trk::locX] : -99999);
      if(getFillVariable("locBiasedResidualY") && biased) *m_locBiasedResidualY = (biased->dimension()>Trk::locY ? biased->residual()[Trk::locY] : -99999);
      if(getFillVariable("locBiasedResidualR") && biased) *m_locBiasedResidualR = (biased->dimension()>Trk::locR ? biased->residual()[Trk::locR] : -99999);
      if(getFillVariable("locBiasedPullX") && biased) *m_locBiasedPullX = (biased->dimension()>Trk::locX ? biased->pull()[Trk::locX] : -99999);
      if(getFillVariable("locBiasedPullY") && biased) *m_locBiasedPullY = (biased->dimension()>Trk::locY ? biased->pull()[Trk::locY] : -99999);
      if(getFillVariable("locBiasedPullR") && biased) *m_locBiasedPullR = (biased->dimension()>Trk::locR ? biased->pull()[Trk::locR] : -99999);
      if(getFillVariable("locUnbiasedResidualX") && unbiased) *m_locUnbiasedResidualX = (unbiased->dimension()>Trk::locX ? unbiased->residual()[Trk::locX] : -99999);
      if(getFillVariable("locUnbiasedResidualY") && unbiased) *m_locUnbiasedResidualY = (unbiased->dimension()>Trk::locY ? unbiased->residual()[Trk::locY] : -99999);
      if(getFillVariable("locUnbiasedResidualR") && unbiased) *m_locUnbiasedResidualR = (unbiased->dimension()>Trk::locR ? unbiased->residual()[Trk::locR] : -99999);
      if(getFillVariable("locUnbiasedPullX") && unbiased) *m_locUnbiasedPullX = (unbiased->dimension()>Trk::locX ? unbiased->pull()[Trk::locX] : -99999);
      if(getFillVariable("locUnbiasedPullY") && unbiased) *m_locUnbiasedPullY = (unbiased->dimension()>Trk::locY ? unbiased->pull()[Trk::locY] : -99999);
      if(getFillVariable("locUnbiasedPullR") && unbiased) *m_locUnbiasedPullR = (unbiased->dimension()>Trk::locR ? unbiased->pull()[Trk::locR] : -99999);
      delete biased;
      delete unbiased;
    }
  }

  if(getFillVariable("isCompetingRIO")) *m_isCompetingRIO = 0;
  const Trk::RIO_OnTrack *rio = dynamic_cast<const Trk::RIO_OnTrack*>(measurement);
  if(!rio){
    const Trk::CompetingRIOsOnTrack *crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(measurement);
    if(crot){
      rio = &crot->rioOnTrack( crot->indexOfMaxAssignProb() );
      if(getFillVariable("isCompetingRIO")) *m_isCompetingRIO = 1;
    }
  }

  if(rio){
  
    Identifier id = rio->identify();
    if(getFillVariable("id")) *m_id = id.get_compact();

    const Trk::TrkDetElementBase *de = rio->detectorElement();
    if(de){
      Identifier detId = de->identify();

      if ( detId.is_valid() ) {
        if(getFillVariable("detElementId")) *m_detElementId = detId.get_compact();
        if (m_idHelper->is_pixel(detId) ) {
          if(getFillVariable("detType")) *m_detType = Trk::TrackState::Pixel;
          if(getFillVariable("bec")) *m_bec = (int)m_pixId->barrel_ec(id);
          if(getFillVariable("layer")) *m_layer = (int)m_pixId->layer_disk(id);
          const InDet::PixelCluster *clus = dynamic_cast<const InDet::PixelCluster*>(rio->prepRawData());
          if(clus){
            const InDet::SiWidth cw = clus->width();
            if(getFillVariable("sizePhi")) *m_sizePhi = (int)cw.colRow()[0];
            if(getFillVariable("sizeZ")) *m_sizeZ = (int)cw.colRow()[1];
            if(getFillVariable("size")) *m_size = (int)clus->rdoList().size();
            if(getFillVariable("charge")) *m_charge = clus->totalCharge();
            if(getFillVariable("isFake")) *m_isFake = (int)clus->isFake();
            if(getFillVariable("isGanged")) *m_isGanged = (int)clus->gangedPixel();
            if(getFillVariable("isSplit")) *m_isSplit = (int)clus->isSplit();
            if(getFillVariable("splitProb1")) *m_splitProb1= clus->splitProbability1();
            if(getFillVariable("splitProb2")) *m_splitProb2 = clus->splitProbability2();
            if(getFillVariable("TOT")) *m_TOT = clus->totalToT();
          }
        } else if (m_idHelper->is_sct(detId)) {
          if(getFillVariable("detType")) *m_detType = Trk::TrackState::SCT;
          if(getFillVariable("bec")) *m_bec = (int)m_sctId->barrel_ec(id);
          if(getFillVariable("layer")) *m_layer = (int)m_sctId->layer_disk(id);
          const InDet::SCT_Cluster *clus = dynamic_cast<const InDet::SCT_Cluster*>(rio->prepRawData());
          if(clus){
            const InDet::SiWidth cw = clus->width();
            if(getFillVariable("sizePhi")) *m_sizePhi = (int)cw.colRow()[0];
          }
        } else if (m_idHelper->is_trt(detId)) {
          if(getFillVariable("detType")) *m_detType = Trk::TrackState::TRT;
          if(getFillVariable("trkX")) *m_trkX = de->center(id).x();
          if(getFillVariable("trkY")) *m_trkY = de->center(id).y();
          if(getFillVariable("trkZ")) *m_trkZ = de->center(id).z();
          if(getFillVariable("bec")) *m_bec = (int)m_trtId->barrel_ec(id);
          if(getFillVariable("layer")) *m_layer = (int)m_trtId->layer_or_wheel(id);
          if(getFillVariable("straw_layer")) *m_straw_layer = (int)m_trtId->straw_layer(id);
          if(getFillVariable("straw")) *m_straw = (int)m_trtId->straw(id);
          int chip=0;
          int board=-1;
          m_neighbourSvc->getChip(id,chip);
          if(abs((int)m_trtId->barrel_ec(id))<2){
            board=m_neighbourSvc->chipToBoardBarrel(chip,m_trtId->layer_or_wheel(id));
          }
          else {
            if (chip<12)
              board=0;
            else {
              chip=chip-20;
              board=1;
            }
          }
          if(getFillVariable("board")) *m_board = board;
          if(getFillVariable("chip")) *m_chip = chip;
          if(getFillVariable("isArgonStraw")) {
            if (!m_TRTStrawSummarySvc.empty()) {
              if (m_TRTStrawSummarySvc->getStatusHT(id) != TRTCond::StrawStatus::Good) {
                *m_isArgonStraw = true;
              }
            }
          }
          if(getFillVariable("phiModule")) *m_phiModule = (int)m_trtId->phi_module(id);
          const InDet::TRT_DriftCircle *driftCircle = dynamic_cast<const InDet::TRT_DriftCircle *>(rio->prepRawData());

          if(driftCircle){
            unsigned int word = driftCircle->getWord();
            Identifier surfaceID = rio->identify();
            TRTCond::RtRelation const *rtr = m_trtcaldbSvc->getRtRelation(surfaceID);
            double tot = driftCircle->timeOverThreshold();
            if(getFillVariable("TOT")) *m_TOT = tot;
            if(getFillVariable("driftTimeToTCorrection")) *m_driftTimeToTCorrection = m_driftFunctionTool->driftTimeToTCorrection(tot,surfaceID);
            if(getFillVariable("driftTimeHTCorrection")){ 
              if(driftCircle->highLevel()) *m_driftTimeHTCorrection = m_driftFunctionTool->driftTimeHTCorrection(surfaceID);
              else *m_driftTimeHTCorrection = 0;
            }
            if(getFillVariable("highLevel")) *m_highLevel = driftCircle->highLevel();
            if(getFillVariable("highThreshold")) {
              unsigned int theWord = word & 0x04020100; //HT bits mask
              *m_highThreshold = 0;
              //this is the agreed-upon binning for HT bit patterns...
              if (theWord == 0x04000000) //pattern 1
                *m_highThreshold = 1;
              else if (theWord == 0x00020000) //pattern 2
                *m_highThreshold = 2;
              else if (theWord == 0x00000100) //pattern 3
                *m_highThreshold = 3;
              else if (theWord == 0x04020000) //pattern 4
                *m_highThreshold = 4;
              else if (theWord == 0x00020100) //pattern 5
                *m_highThreshold = 5;
              else if (theWord == 0x04000100) //pattern 6
                *m_highThreshold = 6;
              else if (theWord == 0x04020100) //pattern 7
                *m_highThreshold = 7;
            }
            if(getFillVariable("bitPattern")) *m_bitPattern = word;
            if(getFillVariable("leadingEdge")) *m_leadingEdge = driftCircle->rawDriftTime();
            if(getFillVariable("trailingEdge")) *m_trailingEdge = driftCircle->rawDriftTime()+tot;
            if(getFillVariable("t0")) {
              if(tp){                
                Identifier surfaceID = rio->identify();
                *m_t0 = m_trtcaldbSvc->getT0(surfaceID);
              }
            }
            if(getFillVariable("drifttime")){
              if(tp){
                *m_drifttime = rtr->drifttime(fabs(*m_locR));
              }
            }
          }
        } else if (m_idHelper->is_mdt(detId)) {
          if(getFillVariable("detType")) *m_detType = Trk::TrackState::MDT;
        } else if (m_idHelper->is_csc(detId)) {
          if(getFillVariable("detType")) *m_detType = Trk::TrackState::CSC;
        } else if (m_idHelper->is_rpc(detId)) {
          if(getFillVariable("detType")) *m_detType = Trk::TrackState::RPC;
        } else if (m_idHelper->is_tgc(detId)) {
          if(getFillVariable("detType")) *m_detType = Trk::TrackState::TGC;
        }
      }
    }else{
      REPORT_MESSAGE (MSG::WARNING) << "Pointer to detector element is not valid";
    }
  }

  if(tp){
    if(tso.types()[Trk::TrackStateOnSurface::Hole]){
      const Trk::Surface& surface = tp->associatedSurface();
      const Trk::TrkDetElementBase* detElement = surface.associatedDetectorElement();
      if(detElement){
        Identifier holeId = detElement->identify();
        if (m_idHelper->is_pixel(holeId) ) {
          if(getFillVariable("detType")) *m_detType = Trk::TrackState::Pixel;
          if(getFillVariable("bec")) *m_bec = (int)m_pixId->barrel_ec(holeId);
          if(getFillVariable("layer")) *m_layer = (int)m_pixId->layer_disk(holeId);
        }else if (m_idHelper->is_sct(holeId)) {
          if(getFillVariable("detType")) *m_detType = Trk::TrackState::SCT;
          if(getFillVariable("bec")) *m_bec = (int)m_sctId->barrel_ec(holeId);
          if(getFillVariable("layer")) *m_layer = (int)m_sctId->layer_disk(holeId);
        }else if (m_idHelper->is_trt(holeId)) {
          if(getFillVariable("detType")) *m_detType = Trk::TrackState::TRT;
          if(getFillVariable("bec")) *m_bec = (int)m_trtId->barrel_ec(holeId);
          if(getFillVariable("layer")) *m_layer = (int)m_trtId->layer_or_wheel(holeId);
          if(getFillVariable("straw_layer")) *m_straw_layer = (int)m_trtId->straw_layer(holeId);
          if(getFillVariable("straw")) *m_straw = (int)m_trtId->straw(holeId);
          if(getFillVariable("phiModule")) *m_phiModule = (int)m_trtId->phi_module(holeId);
        } else if (m_idHelper->is_mdt(holeId)) {
          if(getFillVariable("detType")) *m_detType = Trk::TrackState::MDT;
        } else if (m_idHelper->is_csc(holeId)) {
          if(getFillVariable("detType")) *m_detType = Trk::TrackState::CSC;
        } else if (m_idHelper->is_rpc(holeId)) {
          if(getFillVariable("detType")) *m_detType = Trk::TrackState::RPC;
        } else if (m_idHelper->is_tgc(holeId)) {
          if(getFillVariable("detType")) *m_detType = Trk::TrackState::TGC;
        }
      }
    }
  }

  if(measurement && tp && rio){
    const Trk::TrkDetElementBase *de = rio->detectorElement();
    if(de){
      Identifier detId = de->identify();
      if ( detId.is_valid() ) {
        if (m_idHelper->is_pixel(detId) || m_idHelper->is_sct(detId)) {
          const InDetDD::SiDetectorElement *side = dynamic_cast<const InDetDD::SiDetectorElement *>(de);
          if(side){
            Amg::Vector3D mytrack = tp->momentum();
            Amg::Vector3D mynormal = side->normal();
            Amg::Vector3D myphiax = side->phiAxis();
            Amg::Vector3D myetaax = side->etaAxis();
            float trketacomp = mytrack.dot(myetaax);
            float trkphicomp = mytrack.dot(myphiax);
            float trknormcomp = mytrack.dot(mynormal);  
            if(getFillVariable("incidencePhi")) *m_incidencePhi = atan2(trkphicomp,trknormcomp);
            if(getFillVariable("incidenceTheta")) *m_incidenceTheta = atan2(trketacomp,trknormcomp);
          }
        }
      }
    }else{
      REPORT_MESSAGE (MSG::WARNING) << "Pointer to detector element is not valid";
    }
  }

  return StatusCode::SUCCESS;
}

void TrackStateOnSurfaceFillerTool::clearData(){

  if(getFillVariable("type"))                  *m_type = 0;
  if(getFillVariable("id"))                    *m_id = 0;
  if(getFillVariable("detElementId"))          *m_detElementId = 0;
  if(getFillVariable("detType"))               *m_detType = Trk::TrackState::unidentified;
  if(getFillVariable("bec"))                   *m_bec = -999;
  if(getFillVariable("board"))                 *m_board = 0;
  if(getFillVariable("chip"))                  *m_chip = 0;
  if(getFillVariable("isArgonStraw"))          *m_isArgonStraw = 0;
  if(getFillVariable("layer"))                 *m_layer = -1;
  if(getFillVariable("charge"))                *m_charge = 0;
  if(getFillVariable("sizePhi"))               *m_sizePhi = 0;
  if(getFillVariable("sizeZ"))                 *m_sizeZ = 0;
  if(getFillVariable("size"))                  *m_size = 0;
  if(getFillVariable("isFake"))                *m_isFake = 0;
  if(getFillVariable("isGanged"))              *m_isGanged = 0;
  if(getFillVariable("isSplit"))               *m_isSplit = 0;
  if(getFillVariable("splitProb1"))            *m_splitProb1 = 0;
  if(getFillVariable("splitProb2"))            *m_splitProb2 = 0;
  if(getFillVariable("isCompetingRIO"))        *m_isCompetingRIO = 0;
  if(getFillVariable("locX"))                  *m_locX = -99999.;
  if(getFillVariable("locY"))                  *m_locY = -99999.; 
  if(getFillVariable("locR"))                  *m_locR = -99999.; 
  if(getFillVariable("incidencePhi"))          *m_incidencePhi = -999.;
  if(getFillVariable("incidenceTheta"))        *m_incidenceTheta = -999.;
  if(getFillVariable("err_locX"))              *m_err_locX = -999.;
  if(getFillVariable("err_locY"))              *m_err_locY = -999.;
  if(getFillVariable("cov_locXY"))             *m_cov_locXY = -999.;
  if(getFillVariable("err_locR"))              *m_err_locR = -999.;
  if(getFillVariable("x"))                     *m_x = -99999.;
  if(getFillVariable("y"))                     *m_y = -99999.;
  if(getFillVariable("z"))                     *m_z = -99999.;
  if(getFillVariable("trkX"))                  *m_trkX = -99999.;
  if(getFillVariable("trkY"))                  *m_trkY = -99999.;
  if(getFillVariable("trkZ"))                  *m_trkZ = -99999.;
  if(getFillVariable("trkLocX"))               *m_trkLocX = -99999.;
  if(getFillVariable("trkLocY"))               *m_trkLocY = -99999.;
  if(getFillVariable("trkLocR"))               *m_trkLocR = -99999.;
  if(getFillVariable("trkLocPhi"))             *m_trkLocPhi = -99999.;
  if(getFillVariable("err_trkLocX"))           *m_err_trkLocX = -999.;
  if(getFillVariable("err_trkLocY"))           *m_err_trkLocY = -999.;
  if(getFillVariable("cov_trkLocXY"))          *m_cov_trkLocXY = -999.;
  if(getFillVariable("err_trkLocR"))           *m_err_trkLocR = -999.;
  if(getFillVariable("err_trkLocPhi"))         *m_err_trkLocPhi = -999.;
  if(getFillVariable("cov_trkLocRPhi"))        *m_cov_trkLocRPhi = -999.;
  if(getFillVariable("TOT"))                   *m_TOT = 0;
  if(getFillVariable("driftTimeToTCorrection"))*m_driftTimeToTCorrection = 0;
  if(getFillVariable("driftTimeHTCorrection")) *m_driftTimeHTCorrection = 0;
  if(getFillVariable("t0"))                    *m_t0 = 0;
  if(getFillVariable("bitPattern"))            *m_bitPattern = 0;
  if(getFillVariable("drifttime"))             *m_drifttime = 0;
  if(getFillVariable("highThreshold"))         *m_highThreshold = 0;
  if(getFillVariable("trailingEdge"))          *m_trailingEdge = 0;
  if(getFillVariable("leadingEdge"))           *m_leadingEdge = 0;
  if(getFillVariable("phiModule"))             *m_phiModule = 0;
  if(getFillVariable("highLevel"))             *m_highLevel = 0;
  if(getFillVariable("locBiasedResidualX"))    *m_locBiasedResidualX = -99999;
  if(getFillVariable("locBiasedResidualY"))    *m_locBiasedResidualY = -99999;
  if(getFillVariable("locBiasedResidualR"))    *m_locBiasedResidualR = -99999;
  if(getFillVariable("locBiasedPullX"))        *m_locBiasedPullX = -99999;
  if(getFillVariable("locBiasedPullY"))        *m_locBiasedPullY = -99999;
  if(getFillVariable("locBiasedPullR"))        *m_locBiasedPullR = -99999;
  if(getFillVariable("locUnbiasedResidualX"))  *m_locUnbiasedResidualX = -99999;
  if(getFillVariable("locUnbiasedResidualY"))  *m_locUnbiasedResidualY = -99999;
  if(getFillVariable("locUnbiasedResidualR"))  *m_locUnbiasedResidualR = -99999;
  if(getFillVariable("locUnbiasedPullX"))      *m_locUnbiasedPullX = -99999;
  if(getFillVariable("locUnbiasedPullY"))      *m_locUnbiasedPullY = -99999;
  if(getFillVariable("locUnbiasedPullR"))      *m_locUnbiasedPullR = -99999;
}

void TrackStateOnSurfaceFillerTool::synchronizeFlags(){

  unsigned int numHitTypesFilled=0;
  bool fillPixelHits = m_fillType["FillPixelHits"]; if (fillPixelHits) numHitTypesFilled++; 
  bool fillSCTHits = m_fillType["FillSCTHits"]; if (fillSCTHits) numHitTypesFilled++; 
  bool fillTRTHits = m_fillType["FillTRTHits"]; if (fillTRTHits) numHitTypesFilled++; 
  bool fillMDTHits = m_fillType["FillMDTHits"]; if (fillMDTHits) numHitTypesFilled++; 
  bool fillCSCHits = m_fillType["FillCSCHits"]; if (fillCSCHits) numHitTypesFilled++; 
  bool fillRPCHits = m_fillType["FillRPCHits"]; if (fillRPCHits) numHitTypesFilled++; 
  bool fillTGCHits = m_fillType["FillTGCHits"]; if (fillTGCHits) numHitTypesFilled++; 
  bool fillPixelHoles = m_fillType["FillPixelHoles"];
  bool fillSCTHoles = m_fillType["FillSCTHoles"];
  bool fillTRTHoles = m_fillType["FillTRTHoles"];
  bool fillMDTHoles = m_fillType["FillMDTHoles"];
  bool fillCSCHoles = m_fillType["FillCSCHoles"];
  bool fillRPCHoles = m_fillType["FillRPCHoles"];
  bool fillTGCHoles = m_fillType["FillTGCHoles"];

  bool fillPulls = m_fillType["FillPulls"];

  bool fillHits = numHitTypesFilled>0;
  //fillPixelHits || fillSCTHits || fillTRTHits || fillMDTHits || fillCSCHits || fillRPCHits || fillTGCHits;
  bool fillHoles = fillPixelHoles || fillSCTHoles || fillTRTHoles || fillMDTHoles || fillCSCHoles || fillRPCHoles || fillTGCHoles;

  bool fillPixel = fillPixelHits || fillPixelHoles;
  bool fillSCT = fillSCTHits || fillSCTHoles;
  bool fillTRT = fillTRTHits || fillTRTHoles;
  bool fillMDT = fillMDTHits || fillMDTHoles;
  bool fillCSC = fillCSCHits || fillCSCHoles;
  bool fillRPC = fillRPCHits || fillRPCHoles;
  bool fillTGC = fillTGCHits || fillTGCHoles;

  bool fillSilicon = fillPixel || fillSCT;
  bool fillMuonStrips = fillCSC || fillRPC || fillTGC;

  setFillVariable("type", fillHits && fillHoles);
  setFillVariable("id", fillHits);
  setFillVariable("detElementId", fillHoles || fillHits);
  setFillVariable("detType", numHitTypesFilled>0);
  setFillVariable("bec", fillHoles || fillHits);  
  setFillVariable("board", fillHoles || fillHits);
  setFillVariable("chip", fillHoles || fillHits);
  setFillVariable("isArgonStraw", fillHoles || fillHits);
  setFillVariable("straw_layer", fillHoles || fillHits);
  setFillVariable("straw", fillHoles || fillHits);
  setFillVariable("layer", fillHoles || fillHits);
  setFillVariable("charge", fillPixel && fillHits);
  setFillVariable("sizePhi", fillSilicon && fillHits);
  setFillVariable("sizeZ", fillPixel && fillHits);
  setFillVariable("size", fillPixel && fillHits);
  setFillVariable("isFake", fillPixel && fillHits);
  setFillVariable("isGanged", fillPixel && fillHits);
  setFillVariable("isSplit", fillPixel && fillHits);
  setFillVariable("splitProb1", fillPixel && fillHits);
  setFillVariable("splitProb2", fillPixel && fillHits);
  setFillVariable("isCompetingRIO", fillHits);
  setFillVariable("locX", (fillSilicon || fillMuonStrips || fillTRT) && fillHits);
  setFillVariable("locY", (fillSilicon || fillMuonStrips || fillTRT) && fillHits);
  setFillVariable("locR", (fillTRT || fillMDT) && fillHits);
  setFillVariable("incidencePhi", fillSilicon && fillHits);
  setFillVariable("incidenceTheta", fillSilicon && fillHits);
  setFillVariable("err_locX", (fillSilicon || fillMuonStrips || fillTRT) && fillHits);
  setFillVariable("err_locY", (fillSilicon || fillMuonStrips || fillTRT) && fillHits);
  setFillVariable("cov_locXY", (fillSilicon || fillMuonStrips) && fillHits);
  setFillVariable("err_locR", (fillTRT || fillMDT) && fillHits);
  setFillVariable("TOT", fillTRT && fillHits);
  setFillVariable("driftTimeToTCorrection", fillTRT && fillHits);
  setFillVariable("driftTimeHTCorrection", fillTRT && fillHits);
  setFillVariable("t0", fillTRT && fillHits);
  setFillVariable("bitPattern", fillTRT && fillHits);
  setFillVariable("drifttime", fillTRT && fillHits);
  setFillVariable("highThreshold", fillTRT && fillHits);
  setFillVariable("trailingEdge", fillTRT && fillHits);
  setFillVariable("leadingEdge", fillTRT && fillHits);
  setFillVariable("phiModule", fillTRT && fillHits);
  setFillVariable("highLevel", fillTRT && fillHits);
  setFillVariable("x", (fillSilicon || fillMuonStrips || fillTRT) && fillHits);
  setFillVariable("y", (fillSilicon || fillMuonStrips || fillTRT) && fillHits);
  setFillVariable("z", (fillSilicon || fillMuonStrips || fillTRT) && fillHits);
  setFillVariable("trkX", (fillSilicon || fillMuonStrips || fillTRT) && (fillHoles || fillHits));
  setFillVariable("trkY", (fillSilicon || fillMuonStrips || fillTRT) && (fillHoles || fillHits));
  setFillVariable("trkZ", (fillSilicon || fillMuonStrips || fillTRT) && (fillHoles || fillHits));
  setFillVariable("trkLocX", (fillSilicon || fillMuonStrips || fillTRT) && (fillHoles || fillHits));
  setFillVariable("trkLocY", (fillSilicon || fillMuonStrips || fillTRT) && (fillHoles || fillHits));
  setFillVariable("trkLocR", (fillTRT || fillMDT) && (fillHoles || fillHits));
  setFillVariable("trkLocPhi", fillTRT && (fillHoles || fillHits));
  setFillVariable("err_trkLocX", (fillSilicon || fillMuonStrips || fillTRT) && (fillHoles || fillHits));
  setFillVariable("err_trkLocY", (fillSilicon || fillMuonStrips || fillTRT) && (fillHoles || fillHits));
  setFillVariable("cov_trkLocXY", (fillSilicon || fillMuonStrips || fillTRT) && (fillHoles || fillHits));
  setFillVariable("err_trkLocR", (fillTRT || fillMDT) && (fillHoles || fillHits));
  setFillVariable("err_trkLocPhi", (fillTRT || fillMDT) && (fillHoles || fillHits));
  setFillVariable("cov_trkLocRPhi", (fillTRT || fillMDT) && (fillHoles || fillHits));
  setFillVariable("locBiasedResidualX", (fillSilicon) && fillHits && fillPulls);
  setFillVariable("locBiasedResidualY", fillSilicon && fillHits && fillPulls);
  setFillVariable("locBiasedResidualR", (fillTRT||fillMDT) && fillHits && fillPulls);
  setFillVariable("locBiasedPullX", (fillSilicon) && fillHits && fillPulls);
  setFillVariable("locBiasedPullY", fillSilicon && fillHits && fillPulls);
  setFillVariable("locBiasedPullR", (fillTRT||fillMDT) && fillHits && fillPulls);
  setFillVariable("locUnbiasedResidualX", (fillSilicon) && fillHits && fillPulls);
  setFillVariable("locUnbiasedResidualY", fillSilicon && fillHits && fillPulls);
  setFillVariable("locUnbiasedResidualR", (fillTRT||fillMDT) && fillHits && fillPulls);
  setFillVariable("locUnbiasedPullX", (fillSilicon) && fillHits && fillPulls);
  setFillVariable("locUnbiasedPullY", fillSilicon && fillHits && fillPulls);
  setFillVariable("locUnbiasedPullR", (fillTRT||fillMDT) && fillHits && fillPulls);
}

void TrackStateOnSurfaceFillerTool::setFillVariable(std::string varname, bool dofill){

  std::map<std::string, bool>::iterator itr = m_variables.find(varname);
  if(itr!=m_variables.end()){
    itr->second = dofill;
  }else{
    m_variables.insert(std::make_pair(varname, dofill));
  }
}

bool TrackStateOnSurfaceFillerTool::getFillVariable(std::string varname){

  std::map<std::string, bool>::iterator itr = m_variables.find(varname);
  if(itr!=m_variables.end()){
    return itr->second;
  }
  return false;
}

} // namespace D3PD
