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
#include "RpcRawDataMonitoring/RpcOccupancyAnalysis.h"

//================================================================================================
RpcOccupancyAnalysis::RpcOccupancyAnalysis (const std::string& name, ISvcLocator *pSvcLocator):
  AthMonitorAlgorithm(name,pSvcLocator)
{}

//================================================================================================
RpcOccupancyAnalysis::~RpcOccupancyAnalysis () {}

//================================================================================================
StatusCode RpcOccupancyAnalysis::initialize ()
{  
  ATH_CHECK( AthMonitorAlgorithm::initialize());

  ATH_CHECK( detStore()->retrieve(m_muonMgr) );
  ATH_CHECK( m_idHelperSvc.retrieve());
  ATH_CHECK( m_eventInfo.initialize() );
  ATH_CHECK( m_rpcPrdKey.initialize() );

  m_dataType = dataTypeStringToEnum(m_dataTypeStr);
  ATH_CHECK( m_lumiDataKey.initialize (m_useLumi) );
  ATH_CHECK( m_lbDurationDataKey.initialize (m_useLumi && m_dataType != DataType_t::monteCarlo) );

  ATH_CHECK( initRpcPanel() );

  // Time Tag
  // All:  whole range of readout window, 8BCs, = 200ns
  // B3 :  3 BCs before BC0, = 75 ns
  // C1 :  BC0, = 25 ns
  // A3 :  3 BCs after BC0, = 75 ns
  std::vector<std::string> timeTags = {"All", "B3", "C1", "A3"};
  m_timeTagGroups = Monitored::buildToolMap<int>(m_tools, "RpcOccupancyAnalysis", timeTags);

  ATH_MSG_INFO(" RpcOccupancyAnalysis initialize END ");
  return StatusCode::SUCCESS;
}

//========================================================================================================
StatusCode RpcOccupancyAnalysis::initRpcPanel()
{
  /*
    Iterate over all RpcDetectorElements and RpcReadoutElements
    and cache locally all panel
  */
  ATH_MSG_INFO( name() << " - RpcOccupancyAnalysis::initRpcPanel - start" );
  const RpcIdHelper& rpcIdHelper = m_idHelperSvc->rpcIdHelper();

  int  nValidPanel = 0;
  int panelIn = 0;

  for(unsigned idetEl = 0; idetEl < MuonGM::MuonDetectorManager::RpcDetElMaxHash; ++idetEl) {
    const MuonGM::RpcDetectorElement *detEl = m_muonMgr->getRpcDetectorElement(idetEl);

    if(!detEl) {
      continue;
    }

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

        for(unsigned gasgap = 1; gasgap <= 2; ++gasgap) {
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
        }
      }
    }
  }

  ATH_MSG_INFO( "nValidPanel = "<<nValidPanel );

  return StatusCode::SUCCESS;
}

//================================================================================================
StatusCode RpcOccupancyAnalysis::fillHistograms(const EventContext& ctx) const
{
  using namespace Monitored;

  SG::ReadHandle<xAOD::EventInfo>         eventInfo(m_eventInfo, ctx);
  int e_lumiBlock                            = eventInfo->lumiBlock();

  if (!m_lumiDataKey.empty() && ! m_lbDurationDataKey.empty()) {

    SG::ReadCondHandle<LuminosityCondData>  lumi(m_lumiDataKey, ctx);
    SG::ReadCondHandle<LBDurationCondData>  dur(m_lbDurationDataKey, ctx);

    double e_lbAverageLuminosity               = lumi->lbAverageLuminosity();
    double e_lbDuration                        = dur->lbDuration();
    
    if (e_lbAverageLuminosity < m_avrLumiThr || e_lbDuration < m_lbDuraThr ) {
      ATH_MSG_DEBUG( " This Luminosity block doesn't pass lbAverageLuminosity and luminosity block duration selection. ");
    
      return StatusCode::SUCCESS;
    }
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
StatusCode RpcOccupancyAnalysis::fillHistPRD(const EventContext& ctx) const
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

  auto i_panelIndex_geo   = Scalar<int>("panelInd_geo", 0 );
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
      fill(m_tools[m_timeTagGroups.at("All")], i_prd_LB, i_panelIndex);

      if(std::abs(rpcData->time()) < 12.5) {
        fill(m_tools[m_timeTagGroups.at("C1")], i_prd_LB, i_panelIndex);
      }
      else if(-87.5 < rpcData->time() && rpcData->time() < -12.5) {
        fill(m_tools[m_timeTagGroups.at("B3")], i_prd_LB, i_panelIndex);
      }
      else if (12.5 < rpcData->time() && rpcData->time() < 87.5) {
        fill(m_tools[m_timeTagGroups.at("A3")], i_prd_LB, i_panelIndex);
      }
      
      i_panelIndex_geo   = panel_index;
      i_prd_stationName  = i_panel->second->stationName;
      i_prd_stationEta   = i_panel->second->stationEta ;
      i_prd_stationPhi   = i_panel->second->stationPhi ;
      i_prd_doubletR     = i_panel->second->doubletR   ;
      i_prd_doubletZ     = i_panel->second->doubletZ   ;
      i_prd_doubletPhi   = i_panel->second->doubletPhi ;
      i_prd_gasGap       = i_panel->second->gasGap     ;
      i_prd_measPhi      = i_panel->second->measPhi    ;
      fill(tool, i_panelIndex_geo, i_prd_stationName, i_prd_stationEta, i_prd_stationPhi, i_prd_doubletR, i_prd_doubletZ, i_prd_doubletPhi, i_prd_gasGap, i_prd_measPhi);

      v_prdTime.push_back(rpcData->time());
    }       // loop on RpcPrepData
  }  // loop on RpcPrepData container

  auto prdTimeCollection = Collection("prdTime",  v_prdTime);
  fill(tool, prdTimeCollection);

  ATH_MSG_DEBUG( " fillHistPRD finished " );
  return StatusCode::SUCCESS;
}
