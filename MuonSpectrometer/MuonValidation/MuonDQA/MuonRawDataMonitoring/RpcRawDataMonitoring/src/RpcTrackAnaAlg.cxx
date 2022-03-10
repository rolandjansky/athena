// C/C++
#include <iostream>
#include <set>
#include <string>
#include <sstream>
#include <typeinfo>
#include <fstream>
// root
#include "TObjArray.h"

// Athena
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/RpcDetectorElement.h"

// Local
#include "RpcTrackAnaAlg.h"

//================================================================================================
RpcTrackAnaAlg::RpcTrackAnaAlg (const std::string& name, ISvcLocator *pSvcLocator):
  AthMonitorAlgorithm(name,pSvcLocator)
{}

//================================================================================================
RpcTrackAnaAlg::~RpcTrackAnaAlg () {}

//================================================================================================
StatusCode RpcTrackAnaAlg::initialize ()
{  
  ATH_MSG_INFO(" RpcTrackAnaAlg initialize begin ");
  ATH_CHECK( AthMonitorAlgorithm::initialize());

  ATH_CHECK( detStore()->retrieve(m_muonMgr) );
  ATH_CHECK( m_idHelperSvc.retrieve());
  ATH_CHECK( m_eventInfo.initialize() );

  m_dataType = dataTypeStringToEnum(m_dataTypeStr);
  ATH_CHECK( m_lumiDataKey.initialize (m_useLumi) );
  ATH_CHECK( m_lbDurationDataKey.initialize (m_useLumi && m_dataType != DataType_t::monteCarlo) );

  ATH_CHECK( m_MuonRoIContainerKey.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_MuonContainerKey.initialize() );
  ATH_CHECK( m_rpcPrdKey.initialize() );

  ATH_CHECK( initRpcPanel() );
  ATH_CHECK( initTrigTag() );

  ATH_MSG_INFO(" initialize extrapolator ");
  ATH_CHECK(m_extrapolator.retrieve());

  ATH_MSG_INFO(" RpcTrackAnaAlg initialize END ");
  return StatusCode::SUCCESS;
}

//========================================================================================================
StatusCode RpcTrackAnaAlg::initRpcPanel()
{
  /*
    Iterate over all RpcDetectorElements and RpcReadoutElements
    and cache locally all panel
  */
  ATH_MSG_INFO( name() << " - RpcTrackAnaAlg::initRpcPanel - start" );

  int  nValidPanel = 0;
  ATH_MSG_INFO( "MuonGM::MuonDetectorManager::RpcDetElMaxHash= "<<MuonGM::MuonDetectorManager::RpcDetElMaxHash );
  const RpcIdHelper& rpcIdHelper = m_idHelperSvc->rpcIdHelper();

  m_StationNames[BI] = {};
  m_StationNames[BM1] = {2, 3, 8, 53}; // doubletR = 1
  m_StationNames[BM2] = {2, 3, 8, 53}; // doubletR = 2
  m_StationNames[BO1] = {4, 5, 9, 10}; // doubletR = 1
  m_StationNames[BO2] = {4, 5, 9, 10}; // doubletR = 2

  int panelIn = 0;
  std::vector<int> BMBO_StationNames = {2, 3, 4, 5, 8, 9, 10, 53};
  for(unsigned idetEl = 0; idetEl < MuonGM::MuonDetectorManager::RpcDetElMaxHash; ++idetEl) {
    const MuonGM::RpcDetectorElement *detEl = m_muonMgr->getRpcDetectorElement(idetEl);

    if(!detEl) {
      continue;
    }

    unsigned ngasgap = 2;
    for(int doubletZ = 1; doubletZ <= 3; ++doubletZ) {
      for(unsigned doubletPhi = 1; doubletPhi <= 2; ++doubletPhi) {
  
        const MuonGM::RpcReadoutElement *readoutEl = detEl->getRpcReadoutElement(doubletZ, doubletPhi);

        if(!readoutEl) {	  
          continue;
        }
    
        const Identifier  readEl_id   = readoutEl->identify();
        int doubletZMax = rpcIdHelper.doubletZMax(readEl_id);

        if (doubletZ > doubletZMax){
          continue;
        }

        int stName = rpcIdHelper.stationName(readEl_id);
        
        if (std::count(BMBO_StationNames.begin(), BMBO_StationNames.end(), stName)) {
            ngasgap = 2;
        }
        else {
            ngasgap = 3;
        }

        for(unsigned gasgap = 1; gasgap <= ngasgap; ++gasgap) {
          std::shared_ptr<GasGapData> gap = std::make_shared<GasGapData>(*m_idHelperSvc, readoutEl, doubletZ, doubletPhi, gasgap);

          if(!gap) {
            continue;
          }

          std::pair<int, int> st_dbR = std::make_pair(stName, gap->doubletR);
          std::map<std::pair<int, int>, std::vector<std::shared_ptr<GasGapData>>>::iterator it_gaps = m_gasGapData.find(st_dbR);

          if (it_gaps == m_gasGapData.end()) {
            std::vector<std::shared_ptr<GasGapData>> gasgap_group = {gap};
            m_gasGapData[st_dbR] = gasgap_group;
          }
          else {
            it_gaps->second.push_back(gap);
          }

          std::shared_ptr<RpcPanel> rpcPanel_eta = std::make_shared<RpcPanel>(*m_idHelperSvc, readoutEl, doubletZ, doubletPhi, gasgap, 0, panelIn);
          std::shared_ptr<RpcPanel> rpcPanel_phi = std::make_shared<RpcPanel>(*m_idHelperSvc, readoutEl, doubletZ, doubletPhi, gasgap, 1, panelIn);

          if(rpcPanel_eta->panel_valid) {
            m_rpcPanelMap.insert(std::map<Identifier, std::shared_ptr<RpcPanel>>::value_type(rpcPanel_eta->panelId, rpcPanel_eta));
            nValidPanel ++;
          }

          if(rpcPanel_phi->panel_valid) {
            m_rpcPanelMap.insert(std::map<Identifier, std::shared_ptr<RpcPanel>>::value_type(rpcPanel_phi->panelId, rpcPanel_phi));
            nValidPanel ++;
          }
          
          gap->RpcPanel_eta_phi = std::make_pair(rpcPanel_eta, rpcPanel_phi);          
        }
      }
    }
  }

  ATH_MSG_INFO( "nValidPanel = "<<nValidPanel );

  return StatusCode::SUCCESS;
}

//========================================================================================================
StatusCode RpcTrackAnaAlg::initTrigTag()
{

  TString trigStr = m_trigTagList.value();
  std::unique_ptr<TObjArray> tagList(trigStr.Tokenize(";") );

  // TObjArray* tagList = TString(m_trigTagList.value()).Tokenize(",");
  std::set<TString> alllist;
  for(int i = 0 ; i < tagList->GetEntries(); i++){
    TString tagTrig = tagList->At(i)->GetName();
    if(alllist.find(tagTrig)!=alllist.end())continue;
    alllist.insert(tagTrig);
    std::unique_ptr<TObjArray> arr(tagTrig.Tokenize(";"));
    if(arr->GetEntries()==0)continue;
    TagDef def;
    def.eventTrig = TString(arr->At(0)->GetName());
    def.tagTrig = def.eventTrig;
    if(arr->GetEntries()==2)def.tagTrig = TString(arr->At(1)->GetName());
    m_trigTagDefs.push_back(def);
  }

  return StatusCode::SUCCESS;
}

//================================================================================================
StatusCode RpcTrackAnaAlg::fillHistograms(const EventContext& ctx) const
{
  using namespace Monitored;

  SG::ReadHandle<xAOD::EventInfo>         eventInfo(m_eventInfo, ctx);
  int e_lumiBlock                            = eventInfo->lumiBlock();

  if (!m_lumiDataKey.empty() && ! m_lbDurationDataKey.empty()) {

    SG::ReadCondHandle<LuminosityCondData>  lumi(m_lumiDataKey, ctx);
    SG::ReadCondHandle<LBDurationCondData>  dur(m_lbDurationDataKey, ctx);

    double e_lbAverageLuminosity      = lumi->lbAverageLuminosity();
    double e_lbDuration               = dur->lbDuration();

    if (e_lbAverageLuminosity < m_avrLumiThr || e_lbDuration < m_lbDuraThr ) {
      ATH_MSG_DEBUG( " This Luminosity block doesn't pass lbAverageLuminosity and luminosity block duration selection. ");
    
      return StatusCode::SUCCESS;
    }
  }

  if(m_plotMuonEff){
    ATH_CHECK( fillMuonExtrapolateEff(ctx) );
  }

  if(m_plotPRD) {
    ATH_CHECK( fillHistPRD(ctx) );
  }

  auto tool   = getGroup(m_packageName);
  auto evtLB  = Scalar<int>("evtLB", e_lumiBlock);
  auto run    = Scalar<int>("run",   eventInfo->runNumber());
  fill(tool, evtLB, run);
  
  return StatusCode::SUCCESS;
}

//================================================================================================
StatusCode RpcTrackAnaAlg::fillMuonExtrapolateEff(const EventContext& ctx) const
{
  using namespace Monitored;
  auto tool = getGroup(m_packageName);

  if (!m_MuonRoIContainerKey.empty()) {
    /* raw LVL1MuonRoIs */
    SG::ReadHandle<xAOD::MuonRoIContainer > muonRoIs( m_MuonRoIContainerKey, ctx);
    
    if(!muonRoIs.isValid()){
      ATH_MSG_ERROR("evtStore() does not contain muon L1 ROI Collection with name "<< m_MuonRoIContainerKey);
      return StatusCode::FAILURE;
    }
  }

  SG::ReadHandle<xAOD::MuonContainer> muons(m_MuonContainerKey, ctx);
  ATH_MSG_DEBUG(" muons size = "<< muons->size());

  if(!muons.isValid()){
    ATH_MSG_ERROR("evtStore() does not contain muon Collection with name "<< m_MuonContainerKey);
    return StatusCode::FAILURE; 
  }

  std::vector<MyMuon> mymuons;

  for(const xAOD::Muon* muon : *muons){
    // if(muon->muonType()!=xAOD::Muon::Combined)continue;
    // if(!(muon->quality() <= xAOD::Muon::Medium))continue;
    if(!(muon->quality() <= xAOD::Muon::Loose))continue;
    if(!(muon->pt() > m_minPt)) continue;

    MyMuon mymuon;
    /* fill basic info */
    mymuon.muon = muon;
    mymuon.fourvec.SetPtEtaPhiM(muon->pt(),muon->eta(),muon->phi(),m_muonMass.value());
    /* fill tag of tag-and-probe info */
    mymuon.tagged = triggerMatching(muon,m_trigTagDefs)==StatusCode::SUCCESS;
    mymuons.push_back( mymuon );
  }

  std::vector<GasGapResult> results;
  std::vector<MyMuon>::iterator mu1_it = mymuons.begin();
  std::vector<MyMuon>::iterator mu1_end = mymuons.end();
  for(; mu1_it!=mu1_end; ++mu1_it){
    std::vector<MyMuon>::iterator mu2_it = mu1_it;
    std::vector<MyMuon>::iterator mu2_end = mu1_end;

    if (!(mu1_it->isolated && mu1_it->isZmumu)){
      for(++mu2_it; mu2_it!=mu2_end; ++mu2_it){

        float dr =  (mu2_it->fourvec).DeltaR( mu1_it->fourvec );
        if( dr > m_isolationWindow.value() ){
          mu1_it->isolated = true;
          mu2_it->isolated = true;
        }

        // Z Muon
        if( mu1_it->muon->charge() == mu2_it->muon->charge() ) continue;

        double dimuon_mass = (mu2_it->fourvec + mu1_it->fourvec).M();
        if(dimuon_mass>m_zMass_lowLimit && dimuon_mass<m_zMass_upLimit) {
          mu1_it->isZmumu=true;
          mu2_it->isZmumu=true;
        }
      }
    }

    if ( mu1_it->isolated && mu1_it->isZmumu ) {
      const xAOD::TrackParticle* track = mu1_it->muon->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
      if(!track) continue;

      results.clear();
      ATH_CHECK(extrapolate2RPC(track,   Trk::anyDirection, results, BI));
      ATH_CHECK(readHitsPerGasgap(ctx, results) );
    }
  }

  return StatusCode::SUCCESS;
}

//================================================================================================
StatusCode RpcTrackAnaAlg::fillHistPRD(const EventContext& ctx) const
{
  using namespace Monitored;
  //
  // Read RPC Prepare data
  //

  SG::ReadHandle<Muon::RpcPrepDataContainer> rpcContainer(m_rpcPrdKey, ctx);
  const RpcIdHelper& rpcIdHelper = m_idHelperSvc->rpcIdHelper();

  SG::ReadHandle<xAOD::EventInfo>    eventInfo(m_eventInfo, ctx);
  const int             i_lb      = eventInfo->lumiBlock();
  std::vector<double>   v_prdTime = {}; 

  auto prd_sec_all        = Scalar<int>("prd_sec",         0 );
  auto prd_layer_all      = Scalar<int>("prd_layer",       0 );
  auto prd_sec_1214       = Scalar<int>("prd_sec_1214",    0 );
  auto prd_layer_1214     = Scalar<int>("prd_layer_1214",  0 );

  auto prd_sec_all_eta    = Scalar<int>("prd_sec_eta",     0 );
  auto prd_layer_all_eta  = Scalar<int>("prd_layer_eta",   0 );
  auto prd_sec_all_phi    = Scalar<int>("prd_sec_phi",     0 );
  auto prd_layer_all_phi  = Scalar<int>("prd_layer_phi",   0 );

  auto i_prd_LB           = Scalar<int>("LB",              i_lb );
  auto i_panelIndex       = Scalar<int>("panelInd",        0 );

  auto i_prd_stationName  = Scalar<int>("StationName",  0 );
  auto i_prd_stationEta   = Scalar<int>("StationEta",   0 );
  auto i_prd_stationPhi   = Scalar<int>("StationPhi",   0 );
  auto i_prd_doubletR     = Scalar<int>("DoubletR",     0 );
  auto i_prd_doubletZ     = Scalar<int>("DoubletZ",     0 );
  auto i_prd_doubletPhi   = Scalar<int>("DoubletPhi",   0 );
  auto i_prd_gasGap       = Scalar<int>("GasGap",       0 );
  auto i_prd_measPhi      = Scalar<int>("MeasPhi",      -1);
  
  auto tool = getGroup(m_packageName);

  int panel_index;
  std::pair<int, int> sec_layer;

  //
  // loop on RpcPrepData container
  //
  for(const Muon::RpcPrepDataCollection *rpcCollection: *rpcContainer) {
    if(!rpcCollection) {
        continue;
    }
      
    //
    // loop on RpcPrepData
    //
    for(const Muon::RpcPrepData* rpcData: *rpcCollection) {
      if(!rpcData) {
        continue;
      }

      Identifier       id = rpcData->identify();
      const int   measphi = rpcIdHelper.measuresPhi(id);

      auto  temp_panel = std::make_unique<RpcPanel>(id,  rpcIdHelper);

      std::map<Identifier, std::shared_ptr<RpcPanel>>::const_iterator i_panel=m_rpcPanelMap.find(temp_panel->panelId);
      if (i_panel == m_rpcPanelMap.end()){
        ATH_MSG_WARNING( "The panelID corresponding prd hit does NOT link to a known Panel !!!" );
        continue;
      }
      else{
        panel_index = i_panel->second->panel_index;
      }

      sec_layer = temp_panel->getSectorLayer();
      prd_sec_all   = sec_layer.first;
      prd_layer_all = sec_layer.second;

      if (std::abs(sec_layer.first)==12 || std::abs(sec_layer.first)==14){
        prd_sec_1214   = sec_layer.first;
        prd_layer_1214 = sec_layer.second;
      }

      fill(tool, prd_sec_all,  prd_layer_all, prd_sec_1214,  prd_layer_1214);

      if (measphi == 0){
        prd_sec_all_eta   = sec_layer.first;
        prd_layer_all_eta = sec_layer.second;
        fill(tool, prd_sec_all_eta, prd_layer_all_eta);
      }
      else{
        prd_sec_all_phi   = sec_layer.first;
        prd_layer_all_phi = sec_layer.second;
        fill(tool, prd_sec_all_phi, prd_layer_all_phi);
      }

      i_panelIndex       = panel_index;
      fill(tool, i_prd_LB, i_panelIndex);
      
      i_prd_stationName  = i_panel->second->stationName;
      i_prd_stationEta   = i_panel->second->stationEta ;
      i_prd_stationPhi   = i_panel->second->stationPhi ;
      i_prd_doubletR     = i_panel->second->doubletR   ;
      i_prd_doubletZ     = i_panel->second->doubletZ   ;
      i_prd_doubletPhi   = i_panel->second->doubletPhi ;
      i_prd_gasGap       = i_panel->second->gasGap     ;
      i_prd_measPhi      = i_panel->second->measPhi    ;
      fill(tool, i_panelIndex, i_prd_LB, i_prd_stationName, i_prd_stationEta, i_prd_stationPhi, i_prd_doubletR, i_prd_doubletZ, i_prd_doubletPhi, i_prd_gasGap, i_prd_measPhi);

      v_prdTime.push_back(rpcData->time());
    }  // loop on RpcPrepData
  }  // loop on RpcPrepData container

  auto prdTimeCollection = Collection("prdTime",  v_prdTime);
  fill(tool, prdTimeCollection);

  ATH_MSG_DEBUG( " fillHistPRD finished " );
  return StatusCode::SUCCESS;
}

//================================================================================================
StatusCode RpcTrackAnaAlg::triggerMatching(const xAOD::Muon* offline_muon, const std::vector<TagDef>& list_of_triggers ) const
{
  if( !m_TagAndProbe.value() )return StatusCode::SUCCESS;
  if( getTrigDecisionTool().empty() ) return StatusCode::SUCCESS;
  TVector3 muonvec; muonvec.SetPtEtaPhi(offline_muon->pt(),offline_muon->eta(),offline_muon->phi());
  
  for(const auto& tagTrig : list_of_triggers ){
    if( !getTrigDecisionTool()->isPassed( tagTrig.eventTrig.Data() ) ) continue;

    ATH_MSG_DEBUG("tagTrig.eventTrig = "<< tagTrig.eventTrig << ";  tagTrig.tagTrig = "<< tagTrig.tagTrig );
    ATH_MSG_DEBUG("m_MuonEFContainerName.value() = "<< m_MuonEFContainerName.value());
    
    std::vector< TrigCompositeUtils::LinkInfo<xAOD::MuonContainer> >  features = getTrigDecisionTool()->features<xAOD::MuonContainer>( tagTrig.tagTrig.Data() ,TrigDefs::Physics);
    
    for(const auto& aaa : features){
      ATH_CHECK( aaa.isValid() );
      auto trigmuon_link = aaa.link;
      auto trigmuon = *trigmuon_link;
      TVector3 trigvec; trigvec.SetPtEtaPhi(trigmuon->pt(),trigmuon->eta(),trigmuon->phi());
      if( trigvec.DeltaR( muonvec ) < m_trigMatchWindow.value() ) return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}

//========================================================================================================
StatusCode RpcTrackAnaAlg::extrapolate2RPC(const xAOD::TrackParticle *track, const Trk::PropDirection direction, std::vector<GasGapResult> & results, BarrelDL barrelDL) const
{
  /*
    get intersections of the muon with the RPC planes

    Iterate over all valid RPCDetectorElements and RPCReadoutElements:
     1) compute DR distance between track and center of ReadoutElement
        if this distance within tolerance - proceed 
     2) Next, compute:
          -- min DR distance between track and strips within this gas gap
          -- number of valid eta and phi strips within this gas gap
        if both results within their tolerances - proceed
     3) Extrapolate track to the surface of this gas gap:
          -- Check that extrapolation result valid
          -- Check that extrapolated position is in the gas gap surface bounds
        if both within checks valid - then save RPC extrapolation result
  */
  int doubletR = 1;
  if (barrelDL >= OUT ){
    return StatusCode::SUCCESS;
  }
  else if (barrelDL == BM2 || barrelDL == BO2 ){
    doubletR = 2;
  }

  using namespace Monitored;
  auto tool = getGroup(m_packageName);

  if(!track) {
    return StatusCode::FAILURE;
  }

  std::map<BarrelDL, std::vector<int>>::const_iterator dl_vec_it = m_StationNames.find(barrelDL);
  if (dl_vec_it == m_StationNames.end()){
    return StatusCode::FAILURE;
  }

  Trk::TrackParameters *trackParamLayer = nullptr;
  double minDR = 1.0; // A intial value
  
  const std::vector<int> dl_vec = dl_vec_it->second;
  std::vector<int>::const_iterator it_dl = dl_vec.begin();
  for(;it_dl != dl_vec.end(); it_dl++ ) {
    int stName = *it_dl;
    std::pair<int, int> st_dbR = std::make_pair(stName, doubletR);
    std::map<std::pair<int, int>, std::vector<std::shared_ptr<GasGapData>>>::const_iterator gasgapIt = m_gasGapData.find(st_dbR);
    if (gasgapIt == m_gasGapData.end()) {
      continue;
    }
    
    //
    // Iterate over RPC readout elements and compute intersections with each gas gap
    //
    for(const std::shared_ptr<GasGapData> &gap: gasgapIt->second) {
      ExResult result(gap->gapid, direction);

      // Compute track distance to the gas gap surface
      gap->computeTrackDistanceToGasGap(result, *track);

      if(result.minTrackGasGapDR > m_minDRTrackToGasGap) {
        continue;
      }

      //
      // Extrapolate track to the gas gap surface and check whether the track position is in bounds
      //
      Trk::TrackParameters *trackParamInGap = computeTrackIntersectionWithGasGap(result, track, gap);

      if(trackParamInGap == nullptr){
        continue;
      }
      
      if(!result.localTrackPosInBoundsTight){
        continue;
      }
      

      if (result.minTrackGasGapDR < minDR){
        minDR = result.minTrackGasGapDR;
        trackParamLayer = trackParamInGap;
      }
      ATH_MSG_DEBUG( name() << " extrapolated gasgap: " << gap->gapid_str );

      results.push_back(std::make_pair(result, gap));
    }
  }
 
  // Go to next layer of doublet
  BarrelDL nextDL = BarrelDL(barrelDL+1);

  // propgate the track parameter of the last doublet
  // if no track paramater, use the input track
  if      (trackParamLayer != nullptr  ) {
    return extrapolate2RPC(trackParamLayer, direction, results, nextDL);
  }
  else {
    return extrapolate2RPC(track, direction, results, nextDL);
  }
}

//========================================================================================================
Trk::TrackParameters*  RpcTrackAnaAlg::computeTrackIntersectionWithGasGap(ExResult &                result,
                                                            const xAOD::TrackParticle* track_particle,
                                                            const std::shared_ptr<GasGapData>         &gap) const
{
  const EventContext& ctx = Gaudi::Hive::currentContext(); 
  /*
    This function:  
    - constructs Identifier for specific gasgap
    - extrapolates muon to this gas gap
  */

  // Get surface of this gas gap and extrapolate track to this surface
  const Trk::SurfaceBounds &bounds          = gap->readoutEl->bounds(gap->gapid);
  const Trk::PlaneSurface &gapSurface       = gap->readoutEl->surface(gap->gapid);
  Trk::TrackParameters *detParameters = nullptr;

  if(m_useAODParticle) {
    detParameters = m_extrapolator->extrapolate(ctx,
                                                *track_particle,
                                                gapSurface,
                                                result.direction,
                                                false,
                                                Trk::muon).release();
  }
  else if (track_particle->track()) {
    detParameters = m_extrapolator->extrapolate(ctx,
                                                *(track_particle->track()),
                                                gapSurface,
                                                result.direction,
                                                true,
                                                Trk::muon).release();
  }
  else {
    return detParameters;
  }

  if(!detParameters) {
    return detParameters;
  }
  
  //
  // Transform global extrapolated track position on surface to local coordinates
  //
  const Amg::Vector3D local3dTrackPosition = gap->readoutEl->globalToLocalCoords(detParameters->position(), gap->gapid);
  const Amg::Vector2D local2dTrackPosition(local3dTrackPosition.y(), local3dTrackPosition.z());

  //
  // Check if the track position on surface is within tolerable bounds
  //
  const bool inbounds       = bounds.inside(local2dTrackPosition, m_boundsToleranceReadoutElement,      m_boundsToleranceReadoutElement);
  const bool inbounds_tight = bounds.inside(local2dTrackPosition, m_boundsToleranceReadoutElementTight, m_boundsToleranceReadoutElementTight);

  result.localTrackPosInBounds      = inbounds;
  result.localTrackPosInBoundsTight = inbounds_tight;
  result.localPos                   = local3dTrackPosition;
  result.globalPos                  = detParameters->position();

  return detParameters;
}

//========================================================================================================
StatusCode RpcTrackAnaAlg::extrapolate2RPC(const Trk::TrackParameters* trackParam, const Trk::PropDirection direction, std::vector<GasGapResult> & results, BarrelDL barrelDL) const
{
  /*
    get intersections of the muon with the RPC planes

    Iterate over all valid RPCDetectorElements and RPCReadoutElements:
     1) compute DR distance between track and center of ReadoutElement
        if this distance within tolerance - proceed 
     2) Next, compute:
          -- min DR distance between track and strips within this gas gap
          -- number of valid eta and phi strips within this gas gap
        if both results within their tolerances - proceed
     3) Extrapolate track to the surface of this gas gap:
          -- Check that extrapolation result valid
          -- Check that extrapolated position is in the gas gap surface bounds
        if both within checks valid - then save RPC extrapolation result
  */
  int doubletR = 1;
  if (barrelDL >= OUT ){
    return StatusCode::SUCCESS;
  }
  else if (barrelDL == BM2 || barrelDL == BO2 ){
    doubletR = 2;
  }

  using namespace Monitored;
  auto tool = getGroup(m_packageName);

  if(!trackParam) {
    return StatusCode::FAILURE;
  }

  std::map<BarrelDL, std::vector<int>>::const_iterator dl_vec_it = m_StationNames.find(barrelDL);
  if (dl_vec_it == m_StationNames.end()){
    return StatusCode::FAILURE;
  }

  Trk::TrackParameters *trackParamLayer = nullptr;
  double minDR = 1.0; // A intial value
  
  const std::vector<int> dl_vec = dl_vec_it->second;
  
  std::vector<int>::const_iterator it_dl = dl_vec.begin();
  for(;it_dl != dl_vec.end(); it_dl++ ) {
    int stName = *it_dl;
    std::pair<int, int> st_dbR = std::make_pair(stName, doubletR);
    std::map<std::pair<int, int>, std::vector<std::shared_ptr<GasGapData>>>::const_iterator gasgapIt = m_gasGapData.find(st_dbR);
    
    if (gasgapIt == m_gasGapData.end()) {
      continue;
    }
    
    //
    // Iterate over RPC readout elements and compute intersections with each gas gap
    //
    for(const std::shared_ptr<GasGapData> &gap: gasgapIt->second) {
      ExResult result(gap->gapid, direction);

      // Compute track distance to the gas gap surface
      gap->computeTrackDistanceToGasGap(result, trackParam);

      if(result.minTrackGasGapDR > m_minDRTrackToGasGap) {
        continue;
      }

      //
      // Extrapolate track to the gas gap surface and check whether the track position is in bounds
      //
      Trk::TrackParameters *trackParamInGap = computeTrackIntersectionWithGasGap(result,  trackParam, gap);
      if(trackParamInGap == nullptr){
        continue;
      }
      
      if(!result.localTrackPosInBoundsTight){
        continue;
      }

      ATH_MSG_DEBUG( name() << " extrapolated gasgap: " << gap->gapid_str );
      
      if (result.minTrackGasGapDR < minDR){
        minDR = result.minTrackGasGapDR;
        trackParamLayer = trackParamInGap;
      }

      results.push_back(std::make_pair(result, gap));
    }
  }
  
  if (trackParamLayer == nullptr){
    trackParamLayer =  const_cast<Trk::TrackParameters *>(trackParam);
  }
  BarrelDL nextDL = BarrelDL(barrelDL+1);

  return extrapolate2RPC(trackParamLayer, direction, results, nextDL);
}

//========================================================================================================
Trk::TrackParameters*  RpcTrackAnaAlg::computeTrackIntersectionWithGasGap(ExResult &                result,
                                                            const Trk::TrackParameters* trackParam,
                                                            const std::shared_ptr<GasGapData>         &gap) const
{
  const EventContext& ctx = Gaudi::Hive::currentContext(); 
  /*
    This function:  
    - constructs Identifier for specific gasgap
    - extrapolates muon to this gas gap
  */

  // Get surface of this gas gap and extrapolate track to this surface
  const Trk::SurfaceBounds &bounds    = gap->readoutEl->bounds(gap->gapid);
  const Trk::PlaneSurface &gapSurface = gap->readoutEl->surface(gap->gapid);
  Trk::TrackParameters *detParameters = m_extrapolator->extrapolate(ctx,
                                                                    *trackParam,
                                                                    gapSurface,
                                                                    result.direction,
                                                                    true,
                                                                    Trk::muon).release();

  if(!detParameters) {
    return detParameters;
  }
  
  //
  // Transform global extrapolated track position on surface to local coordinates
  //
  const Amg::Vector3D local3dTrackPosition = gap->readoutEl->globalToLocalCoords(detParameters->position(), gap->gapid);
  const Amg::Vector2D local2dTrackPosition(local3dTrackPosition.y(), local3dTrackPosition.z());

  //
  // Check if the track position on surface is within tolerable bounds
  //
  const bool inbounds       = bounds.inside(local2dTrackPosition, m_boundsToleranceReadoutElement,      m_boundsToleranceReadoutElement);
  const bool inbounds_tight = bounds.inside(local2dTrackPosition, m_boundsToleranceReadoutElementTight, m_boundsToleranceReadoutElementTight);

  result.localTrackPosInBounds      = inbounds;
  result.localTrackPosInBoundsTight = inbounds_tight;
  result.localPos                   = local3dTrackPosition;
  result.globalPos                  = detParameters->position();

  return detParameters;
}

//========================================================================================================
StatusCode RpcTrackAnaAlg::readHitsPerGasgap(const EventContext& ctx, std::vector<GasGapResult>& results) const
{
  using namespace Monitored;
  auto tool = getGroup(m_packageName);

  SG::ReadHandle<Muon::RpcPrepDataContainer> rpcContainer(m_rpcPrdKey, ctx);
  const RpcIdHelper& rpcIdHelper = m_idHelperSvc->rpcIdHelper();
  
  ATH_MSG_DEBUG(" RpcPrepDataContainer size = "<< rpcContainer->size());
  ATH_MSG_DEBUG(" results size = "<< results.size());
  
  std::vector<std::pair<GasGapResult, const Muon::RpcPrepData*>>  v_PRDHit_TrackMatched;
  for(GasGapResult &exr: results) {
    const std::shared_ptr<GasGapData> gap = exr.second;

    int NHit_perEvt_eta = 0;
    int NHit_perEvt_phi = 0;
    std::vector<const Muon::RpcPrepData*> view_hits_eta;
    std::vector<const Muon::RpcPrepData*> view_hits_phi;

    // loop on RpcPrepDataCollection
    for(const Muon::RpcPrepDataCollection *rpcCollection: *rpcContainer) {
      if(!rpcCollection) {
          continue;
      }

      // loop on RpcPrepData
      for(const Muon::RpcPrepData* rpcData: *rpcCollection) {
        if(!rpcData) {
          continue;
        }
  
        const Identifier id  = rpcData->identify();
        const int stationName = rpcIdHelper.stationName(id);
        const int stationEta  = rpcIdHelper.stationEta (id);
        const int stationPhi  = rpcIdHelper.stationPhi (id);
      
        const int doubletR    = rpcIdHelper.doubletR  (id);
        const int doubletZ    = rpcIdHelper.doubletZ  (id);
        const int doubletPhi  = rpcIdHelper.doubletPhi(id);
        const unsigned gasGap = rpcIdHelper.gasGap    (id);
        const int measuresPhi = rpcIdHelper.measuresPhi(id);

        // match hit to the gasgap
        if(stationName == gap->stationName &&
          stationPhi  == gap->stationPhi  &&
          stationEta  == gap->stationEta  &&
          doubletR    == gap->doubletR    &&
          gasGap      == gap->gasgap      &&
          doubletPhi  == gap->doubletPhi  &&
          doubletZ    == gap->doubletZ   ) {
          
          if (measuresPhi){
            NHit_perEvt_phi ++;
            view_hits_phi.push_back(rpcData);
          }
          else{
            NHit_perEvt_eta ++;
            view_hits_eta.push_back(rpcData);
          }

          v_PRDHit_TrackMatched.push_back(std::make_pair(exr, rpcData));
        }
      }
    }

    //Declare the quantities which should be monitored
    auto hitMulti_eta  = Scalar<int>("hitMultiplicity_eta", NHit_perEvt_eta);
    auto hitMulti_phi  = Scalar<int>("hitMultiplicity_phi", NHit_perEvt_phi);
    auto hitMulti      = Scalar<int>("hitMultiplicity",     0);
    auto i_panelIndex  = Scalar<int>("panelInd_hM",         0);
    auto i_passExtrap  = Scalar<bool>("muon_passExtrap",   false);

    fill(tool, hitMulti_eta, hitMulti_phi);
    
    int panel_ind = -1;

    panel_ind = gap->RpcPanel_eta_phi.first->panel_index;
    hitMulti = NHit_perEvt_eta;
    if(hitMulti>0) i_passExtrap = true;
    i_panelIndex = panel_ind;
    fill(tool, hitMulti, i_panelIndex, i_passExtrap);

    ATH_CHECK(fillClusterSize(view_hits_eta, panel_ind, 0)); // isPhi = 0


    panel_ind = gap->RpcPanel_eta_phi.second->panel_index;
    hitMulti = NHit_perEvt_phi;
    if(hitMulti>0) i_passExtrap = true;
    i_panelIndex = panel_ind;
    fill(tool, hitMulti, i_panelIndex, i_passExtrap);

    ATH_CHECK(fillClusterSize(view_hits_phi, panel_ind, 1)); // isPhi = 1
  }

  bool isOutTime = false;
  for(const std::pair<GasGapResult, const Muon::RpcPrepData*> &i_hit: v_PRDHit_TrackMatched) {    
    const std::shared_ptr<GasGapData> gap = i_hit.first.second;
    const Identifier id   = i_hit.second->identify();
    const int measuresPhi = rpcIdHelper.measuresPhi(id);

    isOutTime = false;
    if(fabs(i_hit.second->time()) > m_outtime){
      isOutTime = true;
    }

    auto isOutTime_prd        = Scalar<bool>("isOutTime_prd",         isOutTime);
    auto isOutTime_onTrack    = Scalar<bool>("isOutTime_prd_onTrack", isOutTime);
    auto i_panelIndex         = Scalar<int>("panelInd_prd",    0);
    auto i_panelIndex_onTrack = Scalar<int>("panelInd_prd_onTrack",    0);

    // hit within ±30 mm of the extrapolated muon track position
    Amg::Vector3D hitPos_global =  i_hit.second->globalPosition();
    const Amg::Vector3D hitPos_local = gap->readoutEl->globalToLocalCoords(hitPos_global, gap->gapid);

    float trackPos_localY = i_hit.first.first.localPos.y();
    float trackPos_localZ = i_hit.first.first.localPos.z();

    if (measuresPhi){
      int i_panel_phi = gap->RpcPanel_eta_phi.second->panel_index;
      i_panelIndex = i_panel_phi;
      fill(tool, i_panelIndex, isOutTime_prd);
      if (fabs(trackPos_localY-hitPos_local.y()) < m_diffHitTrackPostion){
        i_panelIndex_onTrack = i_panel_phi;
        fill(tool, i_panelIndex_onTrack, isOutTime_onTrack);
      }
    }
    else{
      int i_panel_eta = gap->RpcPanel_eta_phi.first->panel_index;
      i_panelIndex = i_panel_eta;
      fill(tool, i_panelIndex, isOutTime_prd);
      if (fabs(trackPos_localZ-hitPos_local.z()) < m_diffHitTrackPostion){
        i_panelIndex_onTrack = i_panel_eta;
        fill(tool, i_panelIndex_onTrack, isOutTime_onTrack);
      }
    }
  }

  return StatusCode::SUCCESS;
}

//==================================================================================
StatusCode RpcTrackAnaAlg::fillClusterSize(std::vector<const Muon::RpcPrepData*> &view_hits, const int panel_index, int isPhi) const
{
  using namespace Monitored;

  auto tool = getGroup(m_packageName);
  
  // Make clusters from hits that are close together in space and time
  std::vector<const Muon::RpcPrepData*> cluster_hits;
  while(!view_hits.empty()) {
    cluster_hits.clear();

    // Seed cluster with first (random) hit
    cluster_hits.push_back(view_hits.back());

    // Erase the selected first hit from the list
    view_hits.pop_back();

    // Collect all other hits which are close to the selected hit in time and space
    std::vector<const Muon::RpcPrepData*>::const_iterator hit = view_hits.begin();

    while(hit != view_hits.end()) {  
      const Muon::RpcPrepData* hit_ptr = *hit;

      if(IsNearbyHit(cluster_hits, hit_ptr)) {
	      cluster_hits.push_back(*hit);
	      view_hits.erase(hit);

	      // Start loop from the beginning since we have increased cluster size
	      hit = view_hits.begin();
      }
      else {
	      hit++;
      }
    }

    int cluster_size = cluster_hits.size();

    auto i_panelIndex  = Scalar<int>("panelInd_clust",     panel_index);
    auto i_clusterSize = Scalar<int>("clustMultiplicity",  cluster_size);
    fill(tool, i_panelIndex, i_clusterSize);

    if (isPhi == 1) {
      auto i_clusterSize_view = Scalar<int>("clustMultiplicity_phi",  cluster_size);
      fill(tool, i_clusterSize_view);
    }
    else {
      auto i_clusterSize_view = Scalar<int>("clustMultiplicity_eta",  cluster_size);
      fill(tool, i_clusterSize_view);
    }
  }

  return StatusCode::SUCCESS;
}

//====================================================================================
bool RpcTrackAnaAlg::IsNearbyHit(const std::vector<const Muon::RpcPrepData*> &cluster_hits, const Muon::RpcPrepData* hit) const
{
  const RpcIdHelper& rpcIdHelper = m_idHelperSvc->rpcIdHelper();

  // Check whether this hit is close to any hits in the cluster
  for(const Muon::RpcPrepData* it_hit : cluster_hits) {    
    if( abs(rpcIdHelper.strip(it_hit->identify()) - rpcIdHelper.strip(hit->identify())) < 2 && 
       fabs(it_hit->time() - hit->time()) < 6.5) {
      return true;
    }
  }

  return false;
}
