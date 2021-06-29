/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// Source file for class SiCombinatorialTrackFinderData_xk
/////////////////////////////////////////////////////////////////////////////////

#include "SiSPSeededTrackFinderData/SiCombinatorialTrackFinderData_xk.h"
#include "AthenaKernel/getMessageSvc.h"

namespace InDet {

  SiCombinatorialTrackFinderData_xk::SiCombinatorialTrackFinderData_xk() {
  }

  void SiCombinatorialTrackFinderData_xk::setTools(const Trk::IPatternParametersPropagator* propTool,
                                                   const Trk::IPatternParametersUpdator* updatorTool,
                                                   const Trk::IRIO_OnTrackCreator* rioTool,
                                                   const IInDetConditionsTool* pixCondTool,
                                                   const IInDetConditionsTool* sctCondTool,
                                                   const Trk::MagneticFieldProperties* fieldProp,
                                                   const Trk::IBoundaryCheckTool* boundaryCheckTool)
  {
    // Set SiTools and conditions
    //
    m_tools.setTools(propTool,
                     updatorTool,
                     rioTool);
    m_tools.setTools(pixCondTool,
                     sctCondTool);
    m_tools.setTools(fieldProp);    
    m_tools.setTools(boundaryCheckTool);

    // Set tool to trajectory
    //
    m_trajectory.setTools(&m_tools);
    
    m_initialized = true;
  }

  void SiCombinatorialTrackFinderData_xk::setFieldCondObj(const  AtlasFieldCacheCondObj* fieldCondObj) 
  {
    m_tools.setFieldCondObj(fieldCondObj);
  }
    
  void SiCombinatorialTrackFinderData_xk::setPixContainer(const InDet::PixelClusterContainer* pixcont) {
    m_pixcontainer = pixcont;
  }

  void SiCombinatorialTrackFinderData_xk::setSctContainer(const InDet::SCT_ClusterContainer* sctcont) {
    m_sctcontainer = sctcont;
  }

  void SiCombinatorialTrackFinderData_xk::setResultCode(const ResultCode code) {
    m_resultCode = code;
  }

  void SiCombinatorialTrackFinderData_xk::setFlagToReturnFailedTrack(const bool flag) {
    if( flag &&  (! m_simpleTrack) ) {
       MsgStream log(Athena::getMessageSvc(), "SiCombinatorialTrackFinderData_xk");
       log << MSG::WARNING << "not simpleTrack, keep flagToReturnFailedTrack as false" << endmsg;
       m_flagToReturnFailedTrack = false;
       return;
    }
    m_flagToReturnFailedTrack = flag;
  }

  const InDet::PixelClusterContainer* SiCombinatorialTrackFinderData_xk::pixContainer() const {
    return m_pixcontainer;
  }

  const InDet::SCT_ClusterContainer* SiCombinatorialTrackFinderData_xk::sctContainer() const {
    return m_sctcontainer;
  }

  const Trk::PRDtoTrackMap* SiCombinatorialTrackFinderData_xk::PRDtoTrackMap() const {
    return m_tools.PRDtoTrackMap();
  }

  bool SiCombinatorialTrackFinderData_xk::isInitialized() const {
    return m_initialized;
  }

  SiTrajectory_xk& SiCombinatorialTrackFinderData_xk::trajectory() {
    return m_trajectory;
  }

  Trk::TrackInfo& SiCombinatorialTrackFinderData_xk::trackinfo() {
    return m_trackinfo;
  }

  InDet::SiTools_xk& SiCombinatorialTrackFinderData_xk::tools() {
    return m_tools;
  }

  std::list<Trk::Track*>& SiCombinatorialTrackFinderData_xk::tracks() {
    return m_tracks;
  }

  int& SiCombinatorialTrackFinderData_xk::nprint() {
    return m_nprint;
  }

  int& SiCombinatorialTrackFinderData_xk::inputseeds() {
    return m_inputseeds;
  }

  int& SiCombinatorialTrackFinderData_xk::goodseeds() {
    return m_goodseeds;
  }

  int& SiCombinatorialTrackFinderData_xk::findtracks() {
    return m_findtracks;
  }

  int& SiCombinatorialTrackFinderData_xk::inittracks() {
    return m_inittracks;
  }

  int& SiCombinatorialTrackFinderData_xk::roadbug() {
    return m_roadbug;
  }

  std::array<bool,SiCombinatorialTrackFinderData_xk::kNCombStats>& SiCombinatorialTrackFinderData_xk::statistic()  {
    return m_statistic;
  }

  bool& SiCombinatorialTrackFinderData_xk::heavyIon() {
    return m_heavyIon;
  }

  int& SiCombinatorialTrackFinderData_xk::cosmicTrack() {
    return m_cosmicTrack;
  }

  int& SiCombinatorialTrackFinderData_xk::nclusmin() {
    return m_nclusmin;
  }

  int& SiCombinatorialTrackFinderData_xk::nclusminb() {
    return m_nclusminb;
  }

  int& SiCombinatorialTrackFinderData_xk::nwclusmin() {
    return m_nwclusmin;
  }

  int& SiCombinatorialTrackFinderData_xk::nholesmax() {
    return m_nholesmax;
  }

  int& SiCombinatorialTrackFinderData_xk::dholesmax() {
    return m_dholesmax;
  }

  bool& SiCombinatorialTrackFinderData_xk::simpleTrack() {
    return m_simpleTrack;
  }

  bool SiCombinatorialTrackFinderData_xk::flagToReturnFailedTrack() {
    return m_flagToReturnFailedTrack;
  }

  SiCombinatorialTrackFinderData_xk::ResultCode SiCombinatorialTrackFinderData_xk::resultCode() {
    return m_resultCode;
  }

  double& SiCombinatorialTrackFinderData_xk::pTmin() {
    return m_pTmin;
  }

  double& SiCombinatorialTrackFinderData_xk::pTminBrem() {
    return m_pTminBrem;
  }

  double& SiCombinatorialTrackFinderData_xk::xi2max() {
    return m_xi2max;
  }

  double& SiCombinatorialTrackFinderData_xk::xi2maxNoAdd() {
    return m_xi2maxNoAdd;
  }

  double& SiCombinatorialTrackFinderData_xk::xi2maxlink() {
    return m_xi2maxlink;
  }

  void SiCombinatorialTrackFinderData_xk::setPRDtoTrackMap(const Trk::PRDtoTrackMap* prd_to_track_map) {
    m_tools.setPRDtoTrackMap(prd_to_track_map);
  }
  bool SiCombinatorialTrackFinderData_xk::findPatternHoleSearchOutcome (Trk::Track* theTrack, InDet::PatternHoleSearchOutcome & outcome) const {
    auto found = m_holeSearchOutcomes.find(theTrack);
    if (found == m_holeSearchOutcomes.end()){
      return false; 
    }
    outcome = found->second; 
    return true; 
  }
  void SiCombinatorialTrackFinderData_xk::addPatternHoleSearchOutcome (Trk::Track* theTrack, const InDet::PatternHoleSearchOutcome & outcome){
    m_holeSearchOutcomes.emplace(theTrack,outcome); 
  }

} // end of name space
