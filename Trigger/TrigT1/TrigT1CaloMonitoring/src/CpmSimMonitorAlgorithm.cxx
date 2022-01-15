/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CpmSimMonitorAlgorithm.h"
#include "TrigT1CaloUtils/CoordToHardware.h"


CpmSimMonitorAlgorithm::CpmSimMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator),
    m_overlapPresent(true), //fill histograms is a const - this variable is read only anyhow
    m_cpCmxTool("LVL1::L1CPCMXTools/L1CPCMXTools"),
    m_cpmTool("LVL1::L1CPMTools/L1CPMTools"),
    m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool")
{
}

StatusCode CpmSimMonitorAlgorithm::initialize() {

  ATH_MSG_DEBUG("CpmSimMonitorAlgorith::initialize");
  ATH_MSG_DEBUG("Package Name "<< m_packageName);
  
  // Container names
  ATH_MSG_DEBUG("m_TriggerTowerLocation"<< m_triggerTowerLocation); 
  ATH_MSG_DEBUG("m_cpmTowerLocation"<< m_cpmTowerLocation); 
  ATH_MSG_DEBUG("m_cpmTowerLocationOverlap"<< m_cpmTowerLocationOverlap); 
  ATH_MSG_DEBUG("m_cpmTobRoiLocation"<< m_cpmTobRoiLocation); 

  // Initialise all the readhandles that are needed
  ATH_CHECK(m_triggerTowerLocation.initialize());
  ATH_CHECK(m_cpmTowerLocation.initialize());
  ATH_CHECK(m_cpmTowerLocationOverlap.initialize());
  ATH_CHECK(m_cpmTobRoiLocation.initialize());
  ATH_CHECK(m_rodHeaderLocation.initialize());
  ATH_CHECK(m_cmxCpTobLocation.initialize());
  ATH_CHECK(m_cmxCpHitsLocation.initialize());

  // retrieve any tools if needed
  ATH_CHECK(m_cpCmxTool.retrieve());
  ATH_CHECK(m_cpmTool.retrieve());
  ATH_CHECK(m_errorTool.retrieve());

  // steering parameters
  ATH_MSG_DEBUG("m_crates"<<m_crates ); 
  ATH_MSG_DEBUG("m_modules"<<m_modules ); 
  ATH_MSG_DEBUG("m_maxSlices"<<m_maxSlices );
  ATH_MSG_DEBUG("m_cmxs"<<m_cmxs ); 
  
  return AthMonitorAlgorithm::initialize();
}

StatusCode CpmSimMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {


  ATH_MSG_DEBUG("CpmSimMonitorAlgorithm::fillHistograms");

  std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> variables;
    
  //Retrieve Trigger Towers from SG
  SG::ReadHandle<xAOD::TriggerTowerContainer> triggerTowerTES(m_triggerTowerLocation, ctx);
  ATH_CHECK(triggerTowerTES.isValid());

  // Retrieve Core CPM Towers from SG
  SG::ReadHandle<xAOD::CPMTowerContainer> cpmTowerTES(m_cpmTowerLocation, ctx);
  ATH_CHECK(cpmTowerTES.isValid());

  // Retrieve Overlap CPM Towers from SG
  SG::ReadHandle<xAOD::CPMTowerContainer> cpmTowerOvTES(m_cpmTowerLocationOverlap, ctx);
  ATH_CHECK(cpmTowerOvTES.isValid());

  // Retrieve CPM TOB RoIs from SG
  SG::ReadHandle<xAOD::CPMTobRoIContainer> cpmRoiTES(m_cpmTobRoiLocation, ctx);
  ATH_CHECK(cpmRoiTES.isValid());

  // Retrieve ROD Headers from SG
  SG::ReadHandle<xAOD::RODHeaderContainer> rodTES(m_rodHeaderLocation, ctx);
  ATH_CHECK(rodTES.isValid());

  // Retrieve CMX-CP TOBs from SG
  SG::ReadHandle<xAOD::CMXCPTobContainer> cmxCpTobTES(m_cmxCpTobLocation, ctx);
  ATH_CHECK(cmxCpTobTES.isValid());

  // Retrieve CMX-CP Hits from SG
  SG::ReadHandle<xAOD::CMXCPHitsContainer> cmxCpHitsTES(m_cmxCpHitsLocation, ctx);
  ATH_CHECK(cmxCpHitsTES.isValid());

  // Maps to simplify comparisons
  TriggerTowerMapEm ttMapEm;
  TriggerTowerMapEm ttMapHad;
  CpmTowerMap cpMap;
  CpmTowerMap ovMap;
  CpmTobRoiMap crMap;
  CmxCpTobMap cbMap;
  CmxCpHitsMap cmMap;

  const xAOD::TriggerTowerContainer* triggerTowerTESptr = triggerTowerTES.cptr(); 
  ATH_CHECK(setupMap(triggerTowerTESptr, ttMapEm, ttMapHad));

  const xAOD::CPMTowerContainer* cpmTowerTESptr = cpmTowerTES.cptr();
  ATH_CHECK(setupMap(cpmTowerTESptr, cpMap));

  const xAOD::CPMTowerContainer* cpmTowerOvTESptr = cpmTowerOvTES.cptr();
  ATH_CHECK(setupMap(cpmTowerOvTESptr, ovMap));

  const xAOD::CPMTobRoIContainer* cpmRoiTESptr = cpmRoiTES.cptr();
  ATH_CHECK(setupMap(cpmRoiTESptr, crMap));
  std::vector<int> parityMap(m_crates * m_cmxs * m_modules);

  const xAOD::CMXCPTobContainer* cmxCpTobTESptr=cmxCpTobTES.cptr();
  ATH_CHECK(setupMap(cmxCpTobTESptr, cbMap, &parityMap));

  const xAOD::CMXCPHitsContainer* cmxCpHitsTESptr=cmxCpHitsTES.cptr();
  ATH_CHECK(setupMap(cmxCpHitsTESptr, cmMap));

  // Vectors for error overview bits;
  const int vecsizeCpm = 2 * m_crates * m_modules;
  const int vecsizeCmx = 2 * m_crates * m_cmxs;
  ErrorVector errorsCPM(vecsizeCpm);
  ErrorVector errorsCMX(vecsizeCmx);

  // Compare Trigger Towers and CPM Towers from data
  bool overlap = false;
  bool mismatchCoreEm = false;
  bool mismatchCoreHad = false;
  bool mismatchOverlapEm = false;
  bool mismatchOverlapHad = false;

  mismatchCoreEm = compareEm(ttMapEm, cpMap, errorsCPM, overlap);
  mismatchCoreHad = compareHad(ttMapHad, cpMap, errorsCPM, overlap);
  if (m_overlapPresent) {
    overlap = true;
    mismatchOverlapEm = compareEm(ttMapEm, ovMap, errorsCPM, overlap);
    mismatchOverlapHad = compareHad(ttMapHad, ovMap, errorsCPM, overlap); 
  }

  // Compare RoIs simulated from CPM Towers with CPM RoIs from data

  std::unique_ptr<xAOD::CPMTobRoIContainer> cpmRoiSIM = std::make_unique<xAOD::CPMTobRoIContainer>();
  std::unique_ptr<xAOD::CPMTobRoIAuxContainer> cpmRoiSIMAux = std::make_unique<xAOD::CPMTobRoIAuxContainer>();

  cpmRoiSIM.get()->setStore(cpmRoiSIMAux.get());
  if (mismatchCoreEm || mismatchCoreHad || mismatchOverlapEm ||
      mismatchOverlapHad) {
    simulate(&cpMap, &ovMap, cpmRoiSIM.get());
  } else {
    simulate(&cpMap, cpmRoiSIM.get());
  }

  CpmTobRoiMap crSimMap;
  ATH_CHECK(setupMap(cpmRoiSIM.get(), crSimMap));
  const xAOD::RODHeaderContainer* rodTESptr = rodTES.cptr();
  compare(crSimMap, crMap, errorsCPM, rodTESptr);
  crSimMap.clear();

  // Compare CMX-CP TOBs simulated from CPM RoIs with CMX-CP TOBs from data

  std::unique_ptr<xAOD::CMXCPTobContainer> cmxCpTobSIM = std::make_unique<xAOD::CMXCPTobContainer>();
  std::unique_ptr<xAOD::CMXCPTobAuxContainer> cmxCpTobSIMAux = std::make_unique<xAOD::CMXCPTobAuxContainer>();

  cmxCpTobSIM.get()->setStore(cmxCpTobSIMAux.get());
  simulate(cpmRoiTESptr, cmxCpTobSIM.get());

  CmxCpTobMap cbSimMap;
  ATH_CHECK(setupMap(cmxCpTobSIM.get(), cbSimMap));
  compare(cbSimMap, cbMap, parityMap, errorsCPM, errorsCMX, rodTESptr);
  cbSimMap.clear();

  // Compare Local sums simulated from CMX-CP TOBs with Local sums from data

  std::unique_ptr<xAOD::CMXCPHitsContainer> cmxLocalSIM = std::make_unique<xAOD::CMXCPHitsContainer>();
  std::unique_ptr<xAOD::CMXCPHitsAuxContainer> cmxLocalSIMAux = std::make_unique<xAOD::CMXCPHitsAuxContainer>();
  cmxLocalSIM.get()->setStore(cmxLocalSIMAux.get());
  simulate(cmxCpTobTESptr, cmxLocalSIM.get(), xAOD::CMXCPHits::LOCAL);

  CmxCpHitsMap cmxLocalSimMap;
  ATH_CHECK(setupMap(cmxLocalSIM.get(), cmxLocalSimMap));
  compare(cmxLocalSimMap, cmMap, errorsCMX, xAOD::CMXCPHits::LOCAL);
  cmxLocalSimMap.clear();

  // Compare Local sums with Remote sums from data

  compare(cmMap, cmMap, errorsCMX, xAOD::CMXCPHits::REMOTE_0);

  // Compare Total sums simulated from Remote sums with Total sums from data

  std::unique_ptr<xAOD::CMXCPHitsContainer> cmxTotalSIM = std::make_unique<xAOD::CMXCPHitsContainer>();
  std::unique_ptr<xAOD::CMXCPHitsAuxContainer> cmxTotalSIMAux = std::make_unique<xAOD::CMXCPHitsAuxContainer>();

  cmxTotalSIM->setStore(cmxTotalSIMAux.get());
  simulate(cmxCpHitsTESptr, cmxTotalSIM.get());

  CmxCpHitsMap cmxTotalSimMap;
  ATH_CHECK(setupMap(cmxTotalSIM.get(), cmxTotalSimMap));
  compare(cmxTotalSimMap, cmMap, errorsCMX, xAOD::CMXCPHits::TOTAL);
  cmxTotalSimMap.clear();

  // Compare Topo output information simulated from CMX-CP TOBs with Topo info
  // from data

  std::unique_ptr<xAOD::CMXCPHitsContainer> cmxTopoSIM = std::make_unique<xAOD::CMXCPHitsContainer>();
  std::unique_ptr<xAOD::CMXCPHitsAuxContainer> cmxTopoSIMAux = std::make_unique<xAOD::CMXCPHitsAuxContainer>();
  cmxTopoSIM.get()->setStore(cmxTopoSIMAux.get());
  simulate(cmxCpTobTESptr, cmxTopoSIM.get(), xAOD::CMXCPHits::TOPO_CHECKSUM);

  CmxCpHitsMap cmxTopoSimMap;
  ATH_CHECK(setupMap(cmxTopoSIM.get(), cmxTopoSimMap));
  compare(cmxTopoSimMap, cmMap, errorsCMX, xAOD::CMXCPHits::TOPO_CHECKSUM);
  cmxTopoSimMap.clear();

  // Update error summary plots
  Monitored::Scalar<int> cpmErrorLoc = Monitored::Scalar<int>("cpmErrorLoc", 0);
  Monitored::Scalar<int> cpmError = Monitored::Scalar<int>("cpmError", 0);
  Monitored::Scalar<int> cpmErrorLoc_SimNeData = Monitored::Scalar<int>("cpmErrorLoc_SimNeData", 0);
  Monitored::Scalar<int> cpmError_SimNeData = Monitored::Scalar<int>("cpmError_SimNeData", 0);
  Monitored::Scalar<int> cpmErrorSummary = Monitored::Scalar<int>("cpmErrorSummary", 0);
  // event numbers
  Monitored::Scalar<int> em_tt_x = Monitored::Scalar<int>("em_tt_x", 0);
  Monitored::Scalar<int> em_tt_y = Monitored::Scalar<int>("em_tt_y", 0);
  Monitored::Scalar<int> em_tt_w = Monitored::Scalar<int>("em_tt_w", 0);
  Monitored::Scalar<int> had_tt_x = Monitored::Scalar<int>("had_tt_x", 0);
  Monitored::Scalar<int> had_tt_y = Monitored::Scalar<int>("had_tt_y", 0);
  Monitored::Scalar<int> had_tt_w = Monitored::Scalar<int>("had_tt_w", 0);
  Monitored::Scalar<int> em_roi_x = Monitored::Scalar<int>("em_roi_x", 0);
  Monitored::Scalar<int> em_roi_y = Monitored::Scalar<int>("em_roi_y", 0);
  Monitored::Scalar<int> em_roi_w = Monitored::Scalar<int>("em_roi_w", 0);  
  Monitored::Scalar<int> tau_roi_x = Monitored::Scalar<int>("tau_roi_x", 0);
  Monitored::Scalar<int> tau_roi_y = Monitored::Scalar<int>("tau_roi_y", 0);
  Monitored::Scalar<int> tau_roi_w = Monitored::Scalar<int>("tau_roi_w", 0);
  Monitored::Scalar<int> cmx_tob_left_x = Monitored::Scalar<int>("cmx_tob_left_x", 0);
  Monitored::Scalar<int> cmx_tob_left_y = Monitored::Scalar<int>("cmx_tob_left_y", 0);
  Monitored::Scalar<int> cmx_tob_left_w = Monitored::Scalar<int>("cmx_tob_left_w", 0);
  Monitored::Scalar<int> cmx_tob_right_x = Monitored::Scalar<int>("cmx_tob_right_x", 0);
  Monitored::Scalar<int> cmx_tob_right_y = Monitored::Scalar<int>("cmx_tob_right_y", 0);
  Monitored::Scalar<int> cmx_tob_right_w = Monitored::Scalar<int>("cmx_tob_right_w", 0);
  Monitored::Scalar<int> cmx_thresh_x = Monitored::Scalar<int>("cmx_thresh_x", 0);
  Monitored::Scalar<int> cmx_thresh_y = Monitored::Scalar<int>("cmx_thresh_y", 0);
  Monitored::Scalar<int> cmx_thresh_w = Monitored::Scalar<int>("cmx_thresh_w", 0);

  //
  ErrorVector crateErr(m_crates);
  const int cpmBins = m_crates * m_modules;
  const int cmxBins = m_crates * m_cmxs; 
  int errCounter[7][72]={};
  for (int err = 0; err < NumberOfSummaryBins; ++err) {
    int error = 0;
    for (int loc = 0; loc < cpmBins; ++loc) {
      if ((errorsCPM[loc] >> err) & 0x1) {
	cpmErrorLoc=loc;
	cpmError=err;
	fill(m_packageName,cpmErrorLoc,cpmError);
      }
      const int eventNumber = ctx.eventID().event_number();
      if ((errorsCPM[loc + cpmBins] >> err) & 0x1) {
	cpmErrorLoc_SimNeData=loc;
	cpmError_SimNeData=err;
	fill(m_packageName,cpmErrorLoc_SimNeData,cpmError_SimNeData);
        error = 1;
        crateErr[loc / m_modules] |= (1 << err);	
	if (err<7) {
	  if (err==0) {
	    em_tt_w=eventNumber;
	    em_tt_y=loc;
	    em_roi_x=errCounter[err][loc];
	    fill(m_packageName,em_tt_x,em_tt_y,em_tt_w);
	  } else if (err==1) {
	    had_tt_w=eventNumber;
	    had_tt_y=loc;
	    had_tt_x=errCounter[err][loc];
	    fill(m_packageName,had_tt_x,had_tt_y,had_tt_w);
	  } else if (err==2) {
	    em_roi_w=eventNumber;
	    em_roi_y=loc;
	    em_roi_x=errCounter[err][loc];
	    fill(m_packageName,em_roi_x,em_roi_y,em_roi_w);
	  } else if (err==3) {
	    tau_roi_w=eventNumber;
	    tau_roi_y=loc;
	    tau_roi_x=errCounter[err][loc];
	    fill(m_packageName,tau_roi_x,tau_roi_y,tau_roi_w);
	  } else if (err==4) {
	    cmx_tob_left_w=eventNumber;
	    cmx_tob_left_y=loc;
	    cmx_tob_left_x=errCounter[err][loc];
	    fill(m_packageName,cmx_tob_left_x,cmx_tob_left_y,cmx_tob_left_w);
	  } else if (err==5) {
	    cmx_tob_right_w=eventNumber;
	    cmx_tob_right_y=loc;
	    cmx_tob_right_x=errCounter[err][loc];
	    fill(m_packageName,cmx_tob_right_x,cmx_tob_right_y,cmx_tob_right_w);
	  }
	  errCounter[err][loc]+=1;
	}
      }
      if (loc < cmxBins) {
        if ((errorsCMX[loc] >> err) & 0x1) {
	  cpmErrorLoc=loc+cpmBins;
	  cpmError=err;
	  fill(m_packageName,cpmErrorLoc,cpmError);
        }
        if ((errorsCMX[loc + cmxBins] >> err) & 0x1) {
	  cpmErrorLoc_SimNeData=loc+cpmBins;
	  cpmError_SimNeData=err;
	  fill(m_packageName,cpmErrorLoc_SimNeData,cpmError_SimNeData);
	  //
          error = 1;
          crateErr[loc / m_cmxs] |= (1 << err);
          int offset = 0;
          if (err == RemoteSumMismatch || err == TotalSumMismatch)
            offset = 8;
          else if (err == TopoMismatch) {
            offset = 16;
	  }
	  cmx_thresh_w=eventNumber;
	  cmx_thresh_y=loc+offset;
	  cmx_thresh_x=errCounter[6][loc+offset];
	  fill(m_packageName,cmx_thresh_x,cmx_thresh_y,cmx_thresh_w);
	  errCounter[6][loc+offset]+=1;
        }
      }
    }
    if (error) {
      cpmErrorSummary=err;
      fill(m_packageName,cpmErrorSummary);    
    }


  } // summary bind

  
  fill(m_packageName,variables);
  variables.clear();

  return StatusCode::SUCCESS;
}

bool CpmSimMonitorAlgorithm::compareEm(const TriggerTowerMapEm &ttMap,
                         const CpmTowerMap &cpMap, ErrorVector &errors,
                         bool overlap) const {

  ATH_MSG_DEBUG("Compare Trigger Towers and CPM Towers");

  Monitored::Scalar<float> eta_em_PpmEqCor = Monitored::Scalar<float>("eta_em_PpmEqCor", 0.);
  Monitored::Scalar<float> phi_em_PpmEqCor = Monitored::Scalar<float>("phi_em_PpmEqCor", 0.);

  Monitored::Scalar<float> eta_em_PpmNeCor = Monitored::Scalar<float>("eta_em_PpmNeCor", 0.);
  Monitored::Scalar<float> phi_em_PpmNeCor = Monitored::Scalar<float>("phi_em_PpmNeCor", 0.);

  Monitored::Scalar<float> eta_em_PpmNoCor = Monitored::Scalar<float>("eta_em_PpmNoCor", 0.);
  Monitored::Scalar<float> phi_em_PpmNoCor = Monitored::Scalar<float>("phi_em_PpmNoCor", 0.);

  Monitored::Scalar<float> eta_em_CoreNoPpm = Monitored::Scalar<float>("eta_em_CoreNoPpm", 0.);
  Monitored::Scalar<float> phi_em_CoreNoPpm = Monitored::Scalar<float>("phi_em_CoreNoPpm", 0.);

  //
  Monitored::Scalar<float> eta_em_PpmEqOverlap = Monitored::Scalar<float>("eta_em_PpmEqOverlap", 0.);
  Monitored::Scalar<float> phi_em_PpmEqOverlap = Monitored::Scalar<float>("phi_em_PpmEqOverlap", 0.);

  Monitored::Scalar<float> eta_em_PpmNeOverlap = Monitored::Scalar<float>("eta_em_PpmNeOverlap", 0.);
  Monitored::Scalar<float> phi_em_PpmNeOverlap = Monitored::Scalar<float>("phi_em_PpmNeOverlap", 0.);

  Monitored::Scalar<float> eta_em_PpmNoOverlap = Monitored::Scalar<float>("eta_em_PpmNoOverlap", 0.);
  Monitored::Scalar<float> phi_em_PpmNoOverlap = Monitored::Scalar<float>("phi_em_PpmNoOverlap", 0.);

  Monitored::Scalar<float> eta_em_OverlapNoPpm = Monitored::Scalar<float>("eta_em_OverlapNoPpm", 0.);
  Monitored::Scalar<float> phi_em_OverlapNoPpm = Monitored::Scalar<float>("phi_em_OverlapNoPpm", 0.);

  //
  Monitored::Scalar<int> loc_PpmEqCpmFpga = Monitored::Scalar<int>("loc_PpmEqCpmFpga", 0.);
  Monitored::Scalar<int> loc_fpga_PpmEqCpmFpga = Monitored::Scalar<int>("loc_fpga_PpmEqCpmFpga", 0.);

  Monitored::Scalar<int> loc_PpmNeCpmFpga = Monitored::Scalar<int>("loc_PpmNeCpmFpga", 0.);
  Monitored::Scalar<int> loc_fpga_PpmNeCpmFpga = Monitored::Scalar<int>("loc_fpga_PpmNeCpmFpga", 0.);

  Monitored::Scalar<int> loc_PpmNoCpmFpga = Monitored::Scalar<int>("loc_PpmNoCpmFpga", 0.);
  Monitored::Scalar<int> loc_fpga_PpmNoCpmFpga = Monitored::Scalar<int>("loc_fpga_PpmNoCpmFpga", 0.);

  Monitored::Scalar<int> loc_CpmNoPpmFpga = Monitored::Scalar<int>("loc_CpmNoPpmFpga", 0.);
  Monitored::Scalar<int> loc_fpga_CpmNoPpmFpga = Monitored::Scalar<int>("loc_fpga_CpmNoPpmFpga", 0.);

  bool mismatch = false;
  LVL1::CoordToHardware converter;
  TriggerTowerMapEm::const_iterator ttMapIter = ttMap.begin();
  TriggerTowerMapEm::const_iterator ttMapIterEnd = ttMap.end();
  CpmTowerMap::const_iterator cpMapIter = cpMap.begin();
  CpmTowerMap::const_iterator cpMapIterEnd = cpMap.end();

  while (ttMapIter != ttMapIterEnd || cpMapIter != cpMapIterEnd) {

    int ttKey = 0;
    int cpKey = 0;
    int ttEm = 0;
    int cpEm = 0;
    double eta = 0.;
    double phi = 0.;
    int key = 0;

    if (ttMapIter != ttMapIterEnd)
      ttKey = ttMapIter->first;
    if (cpMapIter != cpMapIterEnd)
      cpKey = cpMapIter->first;

    if ((cpMapIter == cpMapIterEnd) ||
        ((ttMapIter != ttMapIterEnd) && (cpKey > ttKey))) {

      // TriggerTower but no CPMTower

      const xAOD::TriggerTower *tt = ttMapIter->second;
      ++ttMapIter;
      const int layer = tt->layer();
      eta = tt->eta();
      phi = tt->phi();
      if (overlap) { // skip non-overlap TTs
        const LVL1::Coordinate coord(phi, eta);
        const int crate = converter.cpCrateOverlap(coord);
        if (crate >= m_crates)
          continue;
      }
      // check if the TriggerTower is in EM or HAD layer
      if (layer == 0) { // EM
        ttEm = tt->cpET();
      }
      key = ttKey;

    } else if ((ttMapIter == ttMapIterEnd) ||
               ((cpMapIter != cpMapIterEnd) && (ttKey > cpKey))) {

      // CPMTower but no TriggerTower

      const xAOD::CPMTower *cp = cpMapIter->second;
      ++cpMapIter;
      eta = cp->eta();
      phi = cp->phi();
      cpEm = cp->emEnergy();
      key = cpKey;

    } else {

      // Have both

      const xAOD::TriggerTower *tt = ttMapIter->second;
      const xAOD::CPMTower *cp = cpMapIter->second;
      ++ttMapIter;
      ++cpMapIter;
      const int layer = tt->layer();
      eta = tt->eta();
      phi = tt->phi();
      // check if the TriggerTower is in EM or HAD layer
      if (layer == 0) { // EM
        ttEm = tt->cpET();
        ;
      }
      cpEm = cp->emEnergy();
      key = ttKey;
    }

    if (!ttEm && !cpEm)
      continue;

    //  Fill in error plots
    const LVL1::Coordinate coord(phi, eta);
    const int crate =
        (overlap) ? converter.cpCrateOverlap(coord) : converter.cpCrate(coord);
    const int cpm = (overlap) ? converter.cpModuleOverlap(coord)
                              : converter.cpModule(coord);
    if (crate >= m_crates || cpm > m_modules)
      continue;
    const int loc = crate * m_modules + cpm - 1;
    const int cpmBins = m_crates * m_modules;
    const int bitEm = (1 << EMTowerMismatch);
    double phiFPGA = phi;
    if (overlap) {
      const double twoPi = 2. * M_PI;
      const double piByFour = M_PI / 4.;
      if (phi > 7. * piByFour)
        phiFPGA -= twoPi;
      else if (phi < piByFour)
        phiFPGA += twoPi;
    }
    const int loc2 = fpga(crate, phiFPGA);

    const double phiMod = phi * (32./M_PI);

    if (ttEm && ttEm == cpEm) { // non-zero match
      errors[loc] |= bitEm;
      if (overlap) {
	eta_em_PpmEqOverlap=eta;
	phi_em_PpmEqOverlap=phiMod;
	fill(m_packageName,eta_em_PpmEqOverlap,phi_em_PpmEqOverlap);
      } else {
	eta_em_PpmEqCor=eta;
	phi_em_PpmEqCor=phiMod;
	fill(m_packageName,eta_em_PpmEqCor,phi_em_PpmEqCor);
      }
      loc_PpmEqCpmFpga=loc;
      loc_fpga_PpmEqCpmFpga=loc2;
      fill(m_packageName,loc_PpmEqCpmFpga,loc_fpga_PpmEqCpmFpga);

    } else if (ttEm != cpEm) { // mis-match
      mismatch = true;
      errors[loc + cpmBins] |= bitEm;
      if (ttEm && cpEm) { // non-zero mis-match
	if (overlap) {
	  eta_em_PpmNeOverlap=eta;
	  phi_em_PpmNeOverlap=phiMod;
	  fill(m_packageName,eta_em_PpmNeOverlap,phi_em_PpmNeOverlap);
	} else {
	  eta_em_PpmNeCor=eta;
	  phi_em_PpmNeCor=phiMod;
	  fill(m_packageName,eta_em_PpmNeCor,phi_em_PpmNeCor);
	}
	loc_PpmNeCpmFpga=loc;
	loc_fpga_PpmNeCpmFpga=loc2;
	fill(m_packageName,loc_PpmNeCpmFpga,loc_fpga_PpmNeCpmFpga);

      } else if (!cpEm) { // no cp
	if (overlap) {
	  eta_em_PpmNoOverlap=eta;
	  phi_em_PpmNoOverlap=phiMod;
	  fill(m_packageName,eta_em_PpmNoOverlap,phi_em_PpmNoOverlap);
	} else {
	  eta_em_PpmNoCor=eta;
	  phi_em_PpmNoCor=phiMod;
	  fill(m_packageName,eta_em_PpmNoCor,phi_em_PpmNoCor);
	}
	loc_PpmNoCpmFpga=loc;
	loc_fpga_PpmNoCpmFpga=loc2;
	fill(m_packageName,loc_PpmNoCpmFpga,loc_fpga_PpmNoCpmFpga);

      } else { // no tt
	if (overlap) {
	  //
	  eta_em_OverlapNoPpm=eta;
	  phi_em_OverlapNoPpm=phiMod;
	  fill(m_packageName,eta_em_OverlapNoPpm,phi_em_OverlapNoPpm);
	} else {
	  eta_em_OverlapNoPpm=eta;
	  phi_em_OverlapNoPpm=phiMod;
	  fill(m_packageName,eta_em_OverlapNoPpm,phi_em_OverlapNoPpm);
	}
	loc_CpmNoPpmFpga=loc;
	loc_fpga_CpmNoPpmFpga=loc2;
	fill(m_packageName,loc_CpmNoPpmFpga,loc_fpga_CpmNoPpmFpga);

      }
      ATH_MSG_DEBUG(" EMTowerMismatch key/eta/phi/crate/cpm/tt/cp: "
		    << key << "/" << eta << "/" << phi << "/" << crate
		    << "/" << cpm << "/" << ttEm << "/" << cpEm);
    }
  }

  return mismatch;
}

bool CpmSimMonitorAlgorithm::compareHad(const TriggerTowerMapHad &ttMap,
                          const CpmTowerMap &cpMap, ErrorVector &errors,
                          bool overlap) const {
 
  ATH_MSG_DEBUG("Compare Trigger Towers and CPM Towers");


  //
  Monitored::Scalar<float> eta_had_PpmEqCor = Monitored::Scalar<float>("eta_had_PpmEqCor", 0.);
  Monitored::Scalar<float> phi_had_PpmEqCor = Monitored::Scalar<float>("phi_had_PpmEqCor", 0.);

  Monitored::Scalar<float> eta_had_PpmNeCor = Monitored::Scalar<float>("eta_had_PpmNeCor", 0.);
  Monitored::Scalar<float> phi_had_PpmNeCor = Monitored::Scalar<float>("phi_had_PpmNeCor", 0.);

  Monitored::Scalar<float> eta_had_PpmNoCor = Monitored::Scalar<float>("eta_had_PpmNoCor", 0.);
  Monitored::Scalar<float> phi_had_PpmNoCor = Monitored::Scalar<float>("phi_had_PpmNoCor", 0.);

  Monitored::Scalar<float> eta_had_CoreNoPpm = Monitored::Scalar<float>("eta_had_CoreNoPpm", 0.);
  Monitored::Scalar<float> phi_had_CoreNoPpm = Monitored::Scalar<float>("phi_had_CoreNoPpm", 0.);

  //
  Monitored::Scalar<float> eta_had_PpmEqOverlap = Monitored::Scalar<float>("eta_had_PpmEqOverlap", 0.);
  Monitored::Scalar<float> phi_had_PpmEqOverlap = Monitored::Scalar<float>("phi_had_PpmEqOverlap", 0.);

  Monitored::Scalar<float> eta_had_PpmNeOverlap = Monitored::Scalar<float>("eta_had_PpmNeOverlap", 0.);
  Monitored::Scalar<float> phi_had_PpmNeOverlap = Monitored::Scalar<float>("phi_had_PpmNeOverlap", 0.);

  Monitored::Scalar<float> eta_had_PpmNoOverlap = Monitored::Scalar<float>("eta_had_PpmNoOverlap", 0.);
  Monitored::Scalar<float> phi_had_PpmNoOverlap = Monitored::Scalar<float>("phi_had_PpmNoOverlap", 0.);

  Monitored::Scalar<float> eta_had_OverlapNoPpm = Monitored::Scalar<float>("eta_had_OverlapNoPpm", 0.);
  Monitored::Scalar<float> phi_had_OverlapNoPpm = Monitored::Scalar<float>("phi_had_OverlapNoPpm", 0.);

  //
  Monitored::Scalar<int> loc_PpmEqCpmFpga = Monitored::Scalar<int>("loc_PpmEqCpmFpga", 0.);
  Monitored::Scalar<int> loc_fpga_PpmEqCpmFpga = Monitored::Scalar<int>("loc_fpga_PpmEqCpmFpga", 0.);

  Monitored::Scalar<int> loc_PpmNeCpmFpga = Monitored::Scalar<int>("loc_PpmNeCpmFpga", 0.);
  Monitored::Scalar<int> loc_fpga_PpmNeCpmFpga = Monitored::Scalar<int>("loc_fpga_PpmNeCpmFpga", 0.);

  Monitored::Scalar<int> loc_PpmNoCpmFpga = Monitored::Scalar<int>("loc_PpmNoCpmFpga", 0.);
  Monitored::Scalar<int> loc_fpga_PpmNoCpmFpga = Monitored::Scalar<int>("loc_fpga_PpmNoCpmFpga", 0.);

  Monitored::Scalar<int> loc_CpmNoPpmFpga = Monitored::Scalar<int>("loc_CpmNoPpmFpga", 0.);
  Monitored::Scalar<int> loc_fpga_CpmNoPpmFpga = Monitored::Scalar<int>("loc_fpga_CpmNoPpmFpga", 0.);

  bool mismatch = false;
  LVL1::CoordToHardware converter;
  TriggerTowerMapHad::const_iterator ttMapIter = ttMap.begin();
  TriggerTowerMapHad::const_iterator ttMapIterEnd = ttMap.end();
  CpmTowerMap::const_iterator cpMapIter = cpMap.begin();
  CpmTowerMap::const_iterator cpMapIterEnd = cpMap.end();

  while (ttMapIter != ttMapIterEnd || cpMapIter != cpMapIterEnd) {

    int ttKey = 0;
    int cpKey = 0;
    int ttHad = 0;
    int cpHad = 0;
    double eta = 0.;
    double phi = 0.;
    int key = 0;

    if (ttMapIter != ttMapIterEnd)
      ttKey = ttMapIter->first;
    if (cpMapIter != cpMapIterEnd)
      cpKey = cpMapIter->first;

    if ((cpMapIter == cpMapIterEnd) ||
        ((ttMapIter != ttMapIterEnd) && (cpKey > ttKey))) {

      // TriggerTower but no CPMTower

      const xAOD::TriggerTower *tt = ttMapIter->second;
      ++ttMapIter;
      const int layer = tt->layer();
      eta = tt->eta();
      phi = tt->phi();
      if (overlap) { // skip non-overlap TTs
        const LVL1::Coordinate coord(phi, eta);
        const int crate = converter.cpCrateOverlap(coord);
        if (crate >= m_crates)
          continue;
      }
      // check if the TriggerTower is in EM or HAD layer
      if (layer == 1) { // HAD
        ttHad = tt->cpET();
      }
      key = ttKey;

    } else if ((ttMapIter == ttMapIterEnd) ||
               ((cpMapIter != cpMapIterEnd) && (ttKey > cpKey))) {

      // CPMTower but no TriggerTower

      const xAOD::CPMTower *cp = cpMapIter->second;
      ++cpMapIter;
      eta = cp->eta();
      phi = cp->phi();
      cpHad = cp->hadEnergy();
      key = cpKey;

    } else {

      // Have both

      const xAOD::TriggerTower *tt = ttMapIter->second;
      const xAOD::CPMTower *cp = cpMapIter->second;
      ++ttMapIter;
      ++cpMapIter;
      const int layer = tt->layer();
      eta = tt->eta();
      phi = tt->phi();
      // check if the TriggerTower is in EM or HAD layer
      if (layer == 1) { // HAD
        ttHad = tt->cpET();
        ;
      }
      cpHad = cp->hadEnergy();
      key = ttKey;
    }

    if (!ttHad && !cpHad)
      continue;

    //  Fill in error plots
    const LVL1::Coordinate coord(phi, eta);
    const int crate =
        (overlap) ? converter.cpCrateOverlap(coord) : converter.cpCrate(coord);
    const int cpm = (overlap) ? converter.cpModuleOverlap(coord)
                              : converter.cpModule(coord);
    if (crate >= m_crates || cpm > m_modules)
      continue;
    const int loc = crate * m_modules + cpm - 1;
    const int cpmBins = m_crates * m_modules;
    const int bitHad = (1 << HadTowerMismatch);
    double phiFPGA = phi;
    if (overlap) {
      const double twoPi = 2. * M_PI;
      const double piByFour = M_PI / 4.;
      if (phi > 7. * piByFour)
        phiFPGA -= twoPi;
      else if (phi < piByFour)
        phiFPGA += twoPi;
    }
    const int loc2 = fpga(crate, phiFPGA);
    const int loc2Mod = loc2+1;

    const double phiMod = phi * (32./M_PI);
    if (ttHad && ttHad == cpHad) { // non-zero match
      errors[loc] |= bitHad;
      if (overlap) {
	eta_had_PpmEqOverlap=eta;
	phi_had_PpmEqOverlap=phiMod;
	fill(m_packageName,eta_had_PpmEqOverlap,phi_had_PpmEqOverlap);
      } else {
	eta_had_PpmEqCor=eta;
	phi_had_PpmEqCor=phiMod;
	fill(m_packageName,eta_had_PpmEqCor,phi_had_PpmEqCor);
      }
      loc_PpmEqCpmFpga=loc;
      loc_fpga_PpmEqCpmFpga=loc2Mod;
      fill(m_packageName,loc_PpmEqCpmFpga,loc_fpga_PpmEqCpmFpga);


    } else if (ttHad != cpHad) { // mis-match
      mismatch = true;
      errors[loc + cpmBins] |= bitHad;
      if (ttHad && cpHad) { // non-zero mis-match
	if (overlap) {
	  eta_had_PpmNeOverlap=eta;
	  phi_had_PpmNeOverlap=phiMod;
	  fill(m_packageName,eta_had_PpmNeOverlap,phi_had_PpmNeOverlap);
	} else {
	  eta_had_PpmNeCor=eta;
	  phi_had_PpmNeCor=phiMod;
	  fill(m_packageName,eta_had_PpmNeCor,phi_had_PpmNeCor);
	}
	loc_PpmNeCpmFpga=loc;
	loc_fpga_PpmNeCpmFpga=loc2Mod;
	fill(m_packageName,loc_PpmNeCpmFpga,loc_fpga_PpmNeCpmFpga);
      } else if (!cpHad) { // no cp
	if (overlap) {
	  eta_had_PpmNoOverlap=eta;
	  phi_had_PpmNoOverlap=phiMod;
	  fill(m_packageName,eta_had_PpmNoOverlap,phi_had_PpmNoOverlap);
	} else {
	  eta_had_PpmNoCor=eta;
	  phi_had_PpmNoCor=phiMod;
	  fill(m_packageName,eta_had_PpmNoCor,phi_had_PpmNoCor);
	}
	loc_PpmNoCpmFpga=loc;
	loc_fpga_PpmNoCpmFpga=loc2Mod;
	fill(m_packageName,loc_PpmNoCpmFpga,loc_fpga_PpmNoCpmFpga);	

      } else { // no tt
	if (overlap) {
	  eta_had_OverlapNoPpm=eta;
	  phi_had_OverlapNoPpm=phiMod;
	  fill(m_packageName,eta_had_OverlapNoPpm,phi_had_OverlapNoPpm);
	} else {
	  eta_had_OverlapNoPpm=eta;
	  phi_had_OverlapNoPpm=phiMod;
	  fill(m_packageName,eta_had_OverlapNoPpm,phi_had_OverlapNoPpm);
	}
	loc_CpmNoPpmFpga=loc;
	loc_fpga_CpmNoPpmFpga=loc2Mod;
	fill(m_packageName,loc_CpmNoPpmFpga,loc_fpga_CpmNoPpmFpga);
      }

      ATH_MSG_DEBUG(" HadTowerMismatch key/eta/phi/crate/cpm/tt/cp: "
		    << key << "/" << eta << "/" << phi << "/" << crate
		    << "/" << cpm << "/" << ttHad << "/" << cpHad);
      
    }
  }
  
  return mismatch;
}

//  Compare Simulated RoIs with data

void CpmSimMonitorAlgorithm::compare(const CpmTobRoiMap &roiSimMap,
				     const CpmTobRoiMap &roiMap, ErrorVector &errors, 
				     const xAOD::RODHeaderContainer *rodTES) const {
  
  ATH_MSG_DEBUG("Compare Simulated RoIs with data");

  LVL1::CPRoIDecoder decoder;
  CpmTobRoiMap::const_iterator simMapIter = roiSimMap.begin();
  CpmTobRoiMap::const_iterator simMapIterEnd = roiSimMap.end();
  CpmTobRoiMap::const_iterator datMapIter = roiMap.begin();
  CpmTobRoiMap::const_iterator datMapIterEnd = roiMap.end();

  // scalars to fill bitwise histograms directly
  Monitored::Scalar<int> emEnerSimEqDataLocX = Monitored::Scalar<int>("emEnerSimEqDataLocX", 0);
  Monitored::Scalar<int> emEnerSimEqDataLocY = Monitored::Scalar<int>("emEnerSimEqDataLocY", 0);
  Monitored::Scalar<int> tauEnerSimEqDataLocX = Monitored::Scalar<int>("tauEnerSimEqDataLocX", 0);
  Monitored::Scalar<int> tauEnerSimEqDataLocY = Monitored::Scalar<int>("tauEnerSimEqDataLocY", 0);

  Monitored::Scalar<int> emEnerSimNeDataLocX = Monitored::Scalar<int>("emEnerSimNeDataLocX", 0);
  Monitored::Scalar<int> emEnerSimNeDataLocY = Monitored::Scalar<int>("emEnerSimNeDataLocY", 0);
  Monitored::Scalar<int> tauEnerSimNeDataLocX = Monitored::Scalar<int>("tauEnerSimNeDataLocX", 0);
  Monitored::Scalar<int> tauEnerSimNeDataLocY = Monitored::Scalar<int>("tauEnerSimNeDataLocY", 0);

  Monitored::Scalar<int> emEnerDataNoSimLocX = Monitored::Scalar<int>("emEnerDataNoSimLocX", 0);
  Monitored::Scalar<int> emEnerDataNoSimLocY = Monitored::Scalar<int>("emEnerDataNoSimLocY", 0);
  Monitored::Scalar<int> tauEnerDataNoSimLocX = Monitored::Scalar<int>("tauEnerDataNoSimLocX", 0);
  Monitored::Scalar<int> tauEnerDataNoSimLocY = Monitored::Scalar<int>("tauEnerDataNoSimLocY", 0);

  Monitored::Scalar<int> emEnerSimNoDataLocX = Monitored::Scalar<int>("emEnerSimNoDataLocX", 0);
  Monitored::Scalar<int> emEnerSimNoDataLocY = Monitored::Scalar<int>("emEnerSimNoDataLocY", 0);
  Monitored::Scalar<int> tauEnerSimNoDataLocX = Monitored::Scalar<int>("tauEnerSimNoDataLocX", 0);
  Monitored::Scalar<int> tauEnerSimNoDataLocY = Monitored::Scalar<int>("tauEnerSimNoDataLocY", 0);

  Monitored::Scalar<int> emIsolSimEqDataLocX = Monitored::Scalar<int>("emIsolSimEqDataLocX", 0);
  Monitored::Scalar<int> emIsolSimEqDataLocY = Monitored::Scalar<int>("emIsolSimEqDataLocY", 0);
  Monitored::Scalar<int> tauIsolSimEqDataLocX = Monitored::Scalar<int>("tauIsolSimEqDataLocX", 0);
  Monitored::Scalar<int> tauIsolSimEqDataLocY = Monitored::Scalar<int>("tauIsolSimEqDataLocY", 0);

  Monitored::Scalar<int> emIsolSimNeDataLocX = Monitored::Scalar<int>("emIsolSimNeDataLocX", 0);
  Monitored::Scalar<int> emIsolSimNeDataLocY = Monitored::Scalar<int>("emIsolSimNeDataLocY", 0);
  Monitored::Scalar<int> tauIsolSimNeDataLocX = Monitored::Scalar<int>("tauIsolSimNeDataLocX", 0);
  Monitored::Scalar<int> tauIsolSimNeDataLocY = Monitored::Scalar<int>("tauIsolSimNeDataLocY", 0);

  Monitored::Scalar<int> emIsolDataNoSimLocX = Monitored::Scalar<int>("emIsolDataNoSimLocX", 0);
  Monitored::Scalar<int> emIsolDataNoSimLocY = Monitored::Scalar<int>("emIsolDataNoSimLocY", 0);
  Monitored::Scalar<int> tauIsolDataNoSimLocX = Monitored::Scalar<int>("tauIsolDataNoSimLocX", 0);
  Monitored::Scalar<int> tauIsolDataNoSimLocY = Monitored::Scalar<int>("tauIsolDataNoSimLocY", 0);

  Monitored::Scalar<int> emIsolSimNoDataLocX = Monitored::Scalar<int>("emIsolSimNoDataLocX", 0);
  Monitored::Scalar<int> emIsolSimNoDataLocY = Monitored::Scalar<int>("emIsolSimNoDataLocY", 0);
  Monitored::Scalar<int> tauIsolSimNoDataLocX = Monitored::Scalar<int>("tauIsolSimNoDataLocX", 0);
  Monitored::Scalar<int> tauIsolSimNoDataLocY = Monitored::Scalar<int>("tauIsolSimNoDataLocY", 0);

  Monitored::Scalar<float> roiEtaSimEqData = Monitored::Scalar<float>("roiEtaSimEqData", 0.);
  Monitored::Scalar<float> roiPhiSimEqData = Monitored::Scalar<float>("roiPhiSimEqData", 0.);

  Monitored::Scalar<float> roiEtaSimNeData = Monitored::Scalar<float>("roiEtaSimNeData", 0.);
  Monitored::Scalar<float> roiPhiSimNeData = Monitored::Scalar<float>("roiPhiSimNeData", 0.);

  Monitored::Scalar<float> roiEtaDataNoSim = Monitored::Scalar<float>("roiEtaDataNoSim", 0.);
  Monitored::Scalar<float> roiPhiDataNoSim = Monitored::Scalar<float>("roiPhiDataNoSim", 0.);

  Monitored::Scalar<float> roiEtaSimNoData = Monitored::Scalar<float>("roiEtaSimNoData", 0.);
  Monitored::Scalar<float> roiPhiSimNoData = Monitored::Scalar<float>("roiPhiSimNoData", 0.);

  while (simMapIter != simMapIterEnd || datMapIter != datMapIterEnd) {

    int simKey = 0;
    int datKey = 0;
    unsigned int simEnergy = 0;
    unsigned int datEnergy = 0;
    unsigned int simIsol = 0;
    unsigned int datIsol = 0;
    const xAOD::CPMTobRoI *roi = 0;

    if (simMapIter != simMapIterEnd)
      simKey = simMapIter->first;
    if (datMapIter != datMapIterEnd)
      datKey = datMapIter->first;

    if ((datMapIter == datMapIterEnd) ||
        ((simMapIter != simMapIterEnd) && (datKey > simKey))) {

      // Simulated RoI but no data RoI

      roi = simMapIter->second;
      simEnergy = roi->energy();
      simIsol = roi->isolation();
      ++simMapIter;

    } else if ((simMapIter == simMapIterEnd) ||
               ((datMapIter != datMapIterEnd) && (simKey > datKey))) {

      // Data RoI but no simulated RoI

      roi = datMapIter->second;
      datEnergy = roi->energy();
      datIsol = roi->isolation();
      ++datMapIter;

    } else {

      // Have both

      const xAOD::CPMTobRoI *roiS = simMapIter->second;
      roi = datMapIter->second;
      simEnergy = roiS->energy();
      simIsol = roiS->isolation();
      datEnergy = roi->energy();
      datIsol = roi->isolation();
      ++simMapIter;
      ++datMapIter;
    }

    if (!simEnergy && !simIsol && !datEnergy && !datIsol)
      continue;

    //  Check LimitedRoISet bit

    const int crate = roi->crate();
    if (!datEnergy && !datIsol && limitedRoiSet(crate,rodTES))
      continue;

    //  Fill in error plots

    const int cpm = roi->cpm();
    const int chip = roi->chip();
    const int local = roi->location();
    const int type = roi->type();
    const int locX = crate * m_modules + cpm - 1;
    const int locY = chip * 8 + local;
    const int cpmBins = m_crates * m_modules;
    const int bit = (1 << ((type) ? TauRoIMismatch : EMRoIMismatch));
    const LVL1::CoordinateRange coord(
				      decoder.coordinate((roi->roiWord()))
				      ); // hack till updated
    const double eta = coord.eta();
    const double phi = coord.phi();

    if (simEnergy || datEnergy) {
      if (simEnergy == datEnergy) {
        errors[locX] |= bit;
	if (type) {
	  tauEnerSimEqDataLocX=locX;
	  tauEnerSimEqDataLocY=locY;
	  fill(m_packageName,tauEnerSimEqDataLocX,tauEnerSimEqDataLocY);
	} else {
	  emEnerSimEqDataLocX=locX;
	  emEnerSimEqDataLocY=locY;
	  fill(m_packageName,emEnerSimEqDataLocX,emEnerSimEqDataLocY);
	}
      } else {
        errors[locX + cpmBins] |= bit;
        if (simEnergy && datEnergy) {
 	  if (type) {
	    tauEnerSimNeDataLocX=locX;
	    tauEnerSimNeDataLocY=locY;
	    fill(m_packageName,tauEnerSimNeDataLocX,tauEnerSimNeDataLocY);	  
	  } else {
	    emEnerSimNeDataLocX=locX;
	    emEnerSimNeDataLocY=locY;
	    fill(m_packageName,emEnerSimNeDataLocX,emEnerSimNeDataLocY);
	  }
        } else if (simEnergy && !datEnergy) {
	  if (type) {
	    tauEnerSimNoDataLocX=locX;
	    tauEnerSimNoDataLocY=locY;
	    fill(m_packageName,tauEnerSimNoDataLocX,tauEnerSimNoDataLocY);	  
	  } else {
	    emEnerSimNoDataLocX=locX;
	    emEnerSimNoDataLocY=locY;
	    fill(m_packageName,emEnerSimNoDataLocX,emEnerSimNoDataLocY);
	  }
        } else {
	  if (type) {
	    tauEnerDataNoSimLocX=locX;
	    tauEnerDataNoSimLocY=locY;
	    fill(m_packageName,tauEnerDataNoSimLocX,tauEnerDataNoSimLocY);	  
	  } else {
	    emEnerDataNoSimLocX=locX;
	    emEnerDataNoSimLocY=locY;
	    fill(m_packageName,emEnerDataNoSimLocX,emEnerDataNoSimLocY);
	  }
        }
      }
    }
    if (simIsol || datIsol) {
      if (simIsol == datIsol) {
        errors[locX] |= bit;
	if (type) {
	  tauIsolSimEqDataLocX=locX;
	  tauIsolSimEqDataLocY=locY;
	  fill(m_packageName,tauIsolSimEqDataLocX,tauIsolSimEqDataLocY);
	} else {
	  emIsolSimEqDataLocX=locX;
	  emIsolSimEqDataLocY=locY;
	  fill(m_packageName,emIsolSimEqDataLocX,emIsolSimEqDataLocY);
	}

      } else {
        errors[locX + cpmBins] |= bit;
        if (simIsol && datIsol) {
 	  if (type) {
	    tauIsolSimNeDataLocX=locX;
	    tauIsolSimNeDataLocY=locY;
	    fill(m_packageName,tauIsolSimNeDataLocX,tauIsolSimNeDataLocY);	  
	  } else {
	    emIsolSimNeDataLocX=locX;
	    emIsolSimNeDataLocY=locY;
	    fill(m_packageName,emIsolSimNeDataLocX,emIsolSimNeDataLocY);
	  }
        } else if (simIsol && !datIsol) {
	  if (type) {
	    tauIsolSimNoDataLocX=locX;
	    tauIsolSimNoDataLocY=locY;
	    fill(m_packageName,tauIsolSimNoDataLocX,tauIsolSimNoDataLocY);	  
	  } else {
	    emIsolSimNoDataLocX=locX;
	    emIsolSimNoDataLocY=locY;
	    fill(m_packageName,emIsolSimNoDataLocX,emIsolSimNoDataLocY);
	  }
        } else {
	  if (type) {
	    tauIsolDataNoSimLocX=locX;
	    tauIsolDataNoSimLocY=locY;
	    fill(m_packageName,tauIsolDataNoSimLocX,tauIsolDataNoSimLocY);	  
	  } else {
	    emIsolDataNoSimLocX=locX;
	    emIsolDataNoSimLocY=locY;
	    fill(m_packageName,emIsolDataNoSimLocX,emIsolDataNoSimLocY);
	  }
        }
      }
    }
    const double phiMod = (phi * (32./M_PI)) - 0.5;
    double etaMod = eta - 0.05;
    if (simEnergy == datEnergy && simIsol == datIsol) {
      roiEtaSimEqData=etaMod;
      roiPhiSimEqData=phiMod;
      fill(m_packageName,roiEtaSimEqData,roiPhiSimEqData);	  
    } else {
      if ((simEnergy || simIsol) && (datEnergy || datIsol)) {
	roiEtaSimNeData=etaMod;
	roiPhiSimNeData=phiMod;
	fill(m_packageName,roiEtaSimNeData,roiPhiSimNeData);	  
      } else if (!datEnergy && !datIsol) {
	roiEtaSimNoData=etaMod;
	roiPhiSimNoData=phiMod;
	fill(m_packageName,roiEtaSimNoData,roiPhiSimNoData);	  
      } else {
	roiEtaDataNoSim=etaMod;
	roiPhiDataNoSim=phiMod;
	fill(m_packageName,roiEtaDataNoSim,roiPhiDataNoSim);	  
      }
    }
  } // end of iteration
}

//  Compare simulated CMX TOBs with data

void CpmSimMonitorAlgorithm::compare(const CmxCpTobMap &simMap, const CmxCpTobMap &datMap,
				     const std::vector<int> & parityMap, ErrorVector &errorsCPM,
				     ErrorVector &errorsCMX,
				     const xAOD::RODHeaderContainer *rodTES) const {

  ATH_MSG_DEBUG("Compare simulated CMX TOBs with data");

  //
  Monitored::Scalar<int> cmxLeftEnerSimEqDataLocX = Monitored::Scalar<int>("cmxLeftEnerSimEqDataLocX", 0);
  Monitored::Scalar<int> cmxLeftEnerSimEqDataLocY = Monitored::Scalar<int>("cmxLeftEnerSimEqDataLocY", 0);
  Monitored::Scalar<int> cmxRightEnerSimEqDataLocX = Monitored::Scalar<int>("cmxRightEnerSimEqDataLocX", 0);
  Monitored::Scalar<int> cmxRightEnerSimEqDataLocY = Monitored::Scalar<int>("cmxRightEnerSimEqDataLocY", 0);

  Monitored::Scalar<int> cmxLeftEnerSimNeDataLocX = Monitored::Scalar<int>("cmxLeftEnerSimNeDataLocX", 0);
  Monitored::Scalar<int> cmxLeftEnerSimNeDataLocY = Monitored::Scalar<int>("cmxLeftEnerSimNeDataLocY", 0);
  Monitored::Scalar<int> cmxRightEnerSimNeDataLocX = Monitored::Scalar<int>("cmxRightEnerSimNeDataLocX", 0);
  Monitored::Scalar<int> cmxRightEnerSimNeDataLocY = Monitored::Scalar<int>("cmxRightEnerSimNeDataLocY", 0);

  Monitored::Scalar<int> cmxLeftEnerSimNoDataLocX = Monitored::Scalar<int>("cmxLeftEnerSimNoDataLocX", 0);
  Monitored::Scalar<int> cmxLeftEnerSimNoDataLocY = Monitored::Scalar<int>("cmxLeftEnerSimNoDataLocY", 0);
  Monitored::Scalar<int> cmxRightEnerSimNoDataLocX = Monitored::Scalar<int>("cmxRightEnerSimNoDataLocX", 0);
  Monitored::Scalar<int> cmxRightEnerSimNoDataLocY = Monitored::Scalar<int>("cmxRightEnerSimNoDataLocY", 0);

  Monitored::Scalar<int> cmxLeftEnerDataNoSimLocX = Monitored::Scalar<int>("cmxLeftEnerDataNoSimLocX", 0);
  Monitored::Scalar<int> cmxLeftEnerDataNoSimLocY = Monitored::Scalar<int>("cmxLeftEnerDataNoSimLocY", 0);
  Monitored::Scalar<int> cmxRightEnerDataNoSimLocX = Monitored::Scalar<int>("cmxRightEnerDataNoSimLocX", 0);
  Monitored::Scalar<int> cmxRightEnerDataNoSimLocY = Monitored::Scalar<int>("cmxRightEnerDataNoSimLocY", 0);

  Monitored::Scalar<int> cmxLeftIsolSimEqDataLocX = Monitored::Scalar<int>("cmxLeftIsolSimEqDataLocX", 0);
  Monitored::Scalar<int> cmxLeftIsolSimEqDataLocY = Monitored::Scalar<int>("cmxLeftIsolSimEqDataLocY", 0);
  Monitored::Scalar<int> cmxRightIsolSimEqDataLocX = Monitored::Scalar<int>("cmxRightIsolSimEqDataLocX", 0);
  Monitored::Scalar<int> cmxRightIsolSimEqDataLocY = Monitored::Scalar<int>("cmxRightIsolSimEqDataLocY", 0);

  Monitored::Scalar<int> cmxLeftIsolSimNeDataLocX = Monitored::Scalar<int>("cmxLeftIsolSimNeDataLocX", 0);
  Monitored::Scalar<int> cmxLeftIsolSimNeDataLocY = Monitored::Scalar<int>("cmxLeftIsolSimNeDataLocY", 0);
  Monitored::Scalar<int> cmxRightIsolSimNeDataLocX = Monitored::Scalar<int>("cmxRightIsolSimNeDataLocX", 0);
  Monitored::Scalar<int> cmxRightIsolSimNeDataLocY = Monitored::Scalar<int>("cmxRightIsolSimNeDataLocY", 0);

  Monitored::Scalar<int> cmxLeftIsolSimNoDataLocX = Monitored::Scalar<int>("cmxLeftIsolSimNoDataLocX", 0);
  Monitored::Scalar<int> cmxLeftIsolSimNoDataLocY = Monitored::Scalar<int>("cmxLeftIsolSimNoDataLocY", 0);
  Monitored::Scalar<int> cmxRightIsolSimNoDataLocX = Monitored::Scalar<int>("cmxRightIsolSimNoDataLocX", 0);
  Monitored::Scalar<int> cmxRightIsolSimNoDataLocY = Monitored::Scalar<int>("cmxRightIsolSimNoDataLocY", 0);

  Monitored::Scalar<int> cmxLeftIsolDataNoSimLocX = Monitored::Scalar<int>("cmxLeftIsolDataNoSimLocX", 0);
  Monitored::Scalar<int> cmxLeftIsolDataNoSimLocY = Monitored::Scalar<int>("cmxLeftIsolDataNoSimLocY", 0);
  Monitored::Scalar<int> cmxRightIsolDataNoSimLocX = Monitored::Scalar<int>("cmxRightIsolDataNoSimLocX", 0);
  Monitored::Scalar<int> cmxRightIsolDataNoSimLocY = Monitored::Scalar<int>("cmxRightIsolDataNoSimLocY", 0);

  Monitored::Scalar<float> cmxEtaSimEqData = Monitored::Scalar<float>("cmxEtaSimEqData", 0.);
  Monitored::Scalar<float> cmxPhiSimEqData = Monitored::Scalar<float>("cmxPhiSimEqData", 0.);

  Monitored::Scalar<float> cmxEtaSimNeData = Monitored::Scalar<float>("cmxEtaSimNeData", 0.);
  Monitored::Scalar<float> cmxPhiSimNeData = Monitored::Scalar<float>("cmxPhiSimNeData", 0.);

  Monitored::Scalar<float> cmxEtaDataNoSim = Monitored::Scalar<float>("cmxEtaDataNoSim", 0.);
  Monitored::Scalar<float> cmxPhiDataNoSim = Monitored::Scalar<float>("cmxPhiDataNoSim", 0.);

  Monitored::Scalar<float> cmxEtaSimNoData = Monitored::Scalar<float>("cmxEtaSimNoData", 0.);
  Monitored::Scalar<float> cmxPhiSimNoData = Monitored::Scalar<float>("cmxPhiSimNoData", 0.);

  Monitored::Scalar<int> cmxOverLocXSimEqData = Monitored::Scalar<int>("cmxOverLocXSimEqData", 0);
  Monitored::Scalar<int> cmxOverCmxSimEqData = Monitored::Scalar<int>("cmxOverCmxSimEqData", 0);

  Monitored::Scalar<int> cmxOverLocXSimNeData = Monitored::Scalar<int>("cmxOverLocXSimNeData", 0);
  Monitored::Scalar<int> cmxOverCmxSimNeData = Monitored::Scalar<int>("cmxOverCmxSimNeData", 0);


  LVL1::CPRoIDecoder decoder;
  CmxCpTobMap::const_iterator simMapIter = simMap.begin();
  CmxCpTobMap::const_iterator simMapIterEnd = simMap.end();
  CmxCpTobMap::const_iterator datMapIter = datMap.begin();
  CmxCpTobMap::const_iterator datMapIterEnd = datMap.end();

  while (simMapIter != simMapIterEnd || datMapIter != datMapIterEnd) {
    
    int simKey = 0;
    int datKey = 0;
    int simEnergy = 0;
    int simIsol = 0;
    int simOvf = 0;
    int datEnergy = 0;
    int datIsol = 0;
    int datOvf = 0;
    
    const xAOD::CMXCPTob *tob = 0;
    if (simMapIter != simMapIterEnd)
      simKey = simMapIter->first;
    if (datMapIter != datMapIterEnd)
      datKey = datMapIter->first;
    
    if ((datMapIter == datMapIterEnd) ||
        ((simMapIter != simMapIterEnd) && (datKey > simKey))) {
      
      // Simulated TOB but no data TOB

      tob = simMapIter->second;
      simEnergy = tob->energy();
      simIsol = tob->isolation();
      simOvf = (LVL1::DataError(tob->error())).get(LVL1::DataError::Overflow);
      ++simMapIter;

    } else if ((simMapIter == simMapIterEnd) ||
               ((datMapIter != datMapIterEnd) && (simKey > datKey))) {
      
      // Data TOB but no simulated TOB

      tob = datMapIter->second;
      datEnergy = tob->energy();
      datIsol = tob->isolation();
      datOvf = (LVL1::DataError(tob->error())).get(LVL1::DataError::Overflow);
      ++datMapIter;
      
    } else {
      
      // Have both
      
      const xAOD::CMXCPTob *tobS = simMapIter->second;
      tob = datMapIter->second;
      simEnergy = tobS->energy();
      simIsol = tobS->isolation();
      simOvf = (LVL1::DataError(tobS->error())).get(LVL1::DataError::Overflow);
      datEnergy = tob->energy();
      datIsol = tob->isolation();
      datOvf = (LVL1::DataError(tob->error())).get(LVL1::DataError::Overflow);
      ++simMapIter;
      ++datMapIter;
    }

    if (!simEnergy && !simIsol && !datEnergy && !datIsol)
      continue;

    //  Check LimitedRoISet bit and ParityMerge bit

    const int crate = tob->crate();
    if (!simEnergy && !simIsol && limitedRoiSet(crate, rodTES))
      continue;
    const int cpm = tob->cpm();
    const int cmx = tob->cmx();
    if (!datEnergy && !datIsol) {
      const int index = (crate * m_cmxs + cmx) * m_modules + cpm - 1;
      if (parityMap[index])
        continue;
    }

    //  Fill in error plots

    const int chip = tob->chip();
    const int loc = tob->location();
    const int locX = crate * m_modules + cpm - 1;
    const int locY = chip * 4 + loc;
    const int loc2 = crate * m_cmxs + cmx;
    const int cpmBins = m_crates * m_modules;
    const int cmxBins = m_crates * m_cmxs;
    const int bit = (1 << ((cmx) ? RightCMXTobMismatch : LeftCMXTobMismatch));
    const uint32_t roiWord = ((((((crate << 4) + cpm) << 4) + chip) << 2) + loc)
                             << 18;
    const LVL1::CoordinateRange coord(
        decoder.coordinate(roiWord)); // hack till updated
    const double eta = coord.eta();
    const double phi = coord.phi();

    if (simEnergy || datEnergy) {
      if (simEnergy == datEnergy) {
        errorsCPM[locX] |= bit;
        errorsCMX[loc2] |= bit;
	if (cmx) {
	  cmxRightEnerSimEqDataLocX=locX;
	  cmxRightEnerSimEqDataLocY=locY;
	  fill(m_packageName,cmxRightEnerSimEqDataLocX,cmxRightEnerSimEqDataLocY);
	} else {
	  cmxLeftEnerSimEqDataLocX=locX;
	  cmxLeftEnerSimEqDataLocY=locY;
	  fill(m_packageName,cmxLeftEnerSimEqDataLocX,cmxLeftEnerSimEqDataLocY);
	}
      } else {
        errorsCPM[locX + cpmBins] |= bit;
        errorsCMX[loc2 + cmxBins] |= bit;
        if (simEnergy && datEnergy) {
	  if (cmx) {
	    cmxRightEnerSimNeDataLocX=locX;
	    cmxRightEnerSimNeDataLocY=locY;
	    fill(m_packageName,cmxRightEnerSimNeDataLocX,cmxRightEnerSimNeDataLocY);
	  } else {
	    cmxLeftEnerSimNeDataLocX=locX;
	    cmxLeftEnerSimNeDataLocY=locY;
	    fill(m_packageName,cmxLeftEnerSimNeDataLocX,cmxLeftEnerSimNeDataLocY);
	  }
        } else if (simEnergy && !datEnergy) {
	  if (cmx) {
	    cmxRightEnerSimNoDataLocX=locX;
	    cmxRightEnerSimNoDataLocY=locY;
	    fill(m_packageName,cmxRightEnerSimNoDataLocX,cmxRightEnerSimNoDataLocY);
	  } else {
	    cmxLeftEnerSimNoDataLocX=locX;
	    cmxLeftEnerSimNoDataLocY=locY;
	    fill(m_packageName,cmxLeftEnerSimNoDataLocX,cmxLeftEnerSimNoDataLocY);
	  }
        } else {
	  if (cmx) {
	    cmxRightEnerDataNoSimLocX=locX;
	    cmxRightEnerDataNoSimLocY=locY;
	    fill(m_packageName,cmxRightEnerDataNoSimLocX,cmxRightEnerDataNoSimLocY);
	  } else {
	    cmxLeftEnerDataNoSimLocX=locX;
	    cmxLeftEnerDataNoSimLocY=locY;
	    fill(m_packageName,cmxLeftEnerDataNoSimLocX,cmxLeftEnerDataNoSimLocY);
	  }
        }
      }
    }
    if (simIsol || datIsol) {
      if (simIsol == datIsol) {
        errorsCPM[locX] |= bit;
        errorsCMX[loc2] |= bit;
	if (cmx) {
	  cmxRightIsolSimEqDataLocX=locX;
	  cmxRightIsolSimEqDataLocY=locY;
	  fill(m_packageName,cmxRightIsolSimEqDataLocX,cmxRightIsolSimEqDataLocY);
	} else {
	  cmxLeftIsolSimEqDataLocX=locX;
	  cmxLeftIsolSimEqDataLocY=locY;
	  fill(m_packageName,cmxLeftIsolSimEqDataLocX,cmxLeftIsolSimEqDataLocY);
	}
      } else {
        errorsCPM[locX + cpmBins] |= bit;
        errorsCMX[loc2 + cmxBins] |= bit;
        if (simIsol && datIsol) {
	  if (cmx) {
	    cmxRightIsolSimNeDataLocX=locX;
	    cmxRightIsolSimNeDataLocY=locY;
	    fill(m_packageName,cmxRightIsolSimNeDataLocX,cmxRightIsolSimNeDataLocY);
	  } else {
	    cmxLeftIsolSimNeDataLocX=locX;
	    cmxLeftIsolSimNeDataLocY=locY;
	    fill(m_packageName,cmxLeftIsolSimNeDataLocX,cmxLeftIsolSimNeDataLocY);
	  }
        } else if (simIsol && !datIsol) {
	  if (cmx) {
	    cmxRightIsolSimNoDataLocX=locX;
	    cmxRightIsolSimNoDataLocY=locY;
	    fill(m_packageName,cmxRightIsolSimNoDataLocX,cmxRightIsolSimNoDataLocY);
	  } else {
	    cmxLeftIsolSimNoDataLocX=locX;
	    cmxLeftIsolSimNoDataLocY=locY;
	    fill(m_packageName,cmxLeftIsolSimNoDataLocX,cmxLeftIsolSimNoDataLocY);
	  }
        } else {
	  if (cmx) {
	    cmxRightIsolDataNoSimLocX=locX;
	    cmxRightIsolDataNoSimLocY=locY;
	    fill(m_packageName,cmxRightIsolDataNoSimLocX,cmxRightIsolDataNoSimLocY);
	  } else {
	    cmxLeftIsolDataNoSimLocX=locX;
	    cmxLeftIsolDataNoSimLocY=locY;
	    fill(m_packageName,cmxLeftIsolDataNoSimLocX,cmxLeftIsolDataNoSimLocY);
	  }
        }
      }
    }
    if (simOvf || datOvf) {
      if (simOvf == datOvf) {
        errorsCPM[locX] |= bit;
        errorsCMX[loc2] |= bit;
	cmxOverLocXSimEqData = locX;
	cmxOverCmxSimEqData = cmx;
	fill(m_packageName,cmxOverLocXSimEqData,cmxOverCmxSimEqData);
      } else {
        errorsCPM[locX + cpmBins] |= bit;
        errorsCMX[loc2 + cmxBins] |= bit;
	cmxOverLocXSimNeData = locX;
	cmxOverCmxSimNeData = cmx;
	fill(m_packageName,cmxOverLocXSimNeData,cmxOverCmxSimNeData);
      }
    }
    const double phiMod = phi * (32./M_PI) - 0.5;
    double etaMod = eta - 0.05;
    if (simEnergy == datEnergy && simIsol == datIsol) {
      cmxEtaSimEqData=etaMod;
      cmxPhiSimEqData=phiMod;
      fill(m_packageName,cmxEtaSimEqData,cmxPhiSimEqData);	  
    } else {
      if ((simEnergy || simIsol) && (datEnergy || datIsol)) {
	cmxEtaSimNeData=etaMod;
	cmxPhiSimNeData=phiMod;
	fill(m_packageName,cmxEtaSimNeData,cmxPhiSimNeData);	  
      } else if (!datEnergy && !datIsol) {
	cmxEtaSimNoData=etaMod;
	cmxPhiSimNoData=phiMod;
	fill(m_packageName,cmxEtaSimNoData,cmxPhiSimNoData);	  
      } else {
	cmxEtaDataNoSim=etaMod;
	cmxPhiDataNoSim=phiMod;
	fill(m_packageName,cmxEtaDataNoSim,cmxPhiDataNoSim);	  
      }
    }
    //  
  }
}

//  Compare Simulated CMX Hit Sums and Data CMX Hit Sums

void CpmSimMonitorAlgorithm::compare(const CmxCpHitsMap &cmxSimMap,
                       const CmxCpHitsMap &cmxMap, ErrorVector &errors,
                       int selection) const {

  ATH_MSG_DEBUG("Compare Simulated CMX Hit Sums and Data CMX Hit Sums");

  const bool local = (selection == xAOD::CMXCPHits::LOCAL);
  const bool remote = (selection == xAOD::CMXCPHits::REMOTE_0);
  const bool total = (selection == xAOD::CMXCPHits::TOTAL);
  const bool topo = (selection == xAOD::CMXCPHits::TOPO_CHECKSUM);

  if (!local && !remote && !total /* && !topo*/)
    return;

  Monitored::Scalar<int> cmx_sum_loc_SimEqData = Monitored::Scalar<int>("cmx_sum_loc_SimEqData", 0);
  Monitored::Scalar<int> cmx_sum_loc_SimNeData = Monitored::Scalar<int>("cmx_sum_loc_SimNeData", 0);
  Monitored::Scalar<int> cmx_sum_loc_SimNoData = Monitored::Scalar<int>("cmx_sum_loc_SimNoData", 0);
  Monitored::Scalar<int> cmx_sum_loc_DataNoSim = Monitored::Scalar<int>("cmx_sum_loc_DataNoSim", 0);

  //
  Monitored::Scalar<int> cmxTopoLocXSimEqData = Monitored::Scalar<int>("cmxTopoLocXSimEqData", 0);
  Monitored::Scalar<int> cmxTopoLocYSimEqData = Monitored::Scalar<int>("cmxTopoLocYSimEqData", 0);
  Monitored::Scalar<int> cmxTopoLocXSimNeData = Monitored::Scalar<int>("cmxTopoLocXSimNeData", 0);
  Monitored::Scalar<int> cmxTopoLocYSimNeData = Monitored::Scalar<int>("cmxTopoLocYSimNeData", 0);
  Monitored::Scalar<int> cmxTopoLocXSimNoData = Monitored::Scalar<int>("cmxTopoLocXSimNoData", 0);
  Monitored::Scalar<int> cmxTopoLocYSimNoData = Monitored::Scalar<int>("cmxTopoLocYSimNoData", 0);
  Monitored::Scalar<int> cmxTopoLocXDataNoSim = Monitored::Scalar<int>("cmxTopoLocXDataNoSim", 0);
  Monitored::Scalar<int> cmxTopoLocYDataNoSim = Monitored::Scalar<int>("cmxTopoLocYDataNoSim", 0);

  // 
  Monitored::Scalar<int> cmx_x_leftsums_SimEqData = Monitored::Scalar<int>("cmx_x_leftsums_SimEqData", 0);
  Monitored::Scalar<int> cmx_y_leftsums_SimEqData = Monitored::Scalar<int>("cmx_y_leftsums_SimEqData", 0);
  Monitored::Scalar<int> cmx_w_leftsums_SimEqData = Monitored::Scalar<int>("cmx_w_leftsums_SimEqData", 0);
  Monitored::Scalar<int> cmx_x_rightsums_SimEqData = Monitored::Scalar<int>("cmx_x_rightsums_SimEqData", 0);
  Monitored::Scalar<int> cmx_y_rightsums_SimEqData = Monitored::Scalar<int>("cmx_y_rightsums_SimEqData", 0);
  Monitored::Scalar<int> cmx_w_rightsums_SimEqData = Monitored::Scalar<int>("cmx_w_rightsums_SimEqData", 0);

  Monitored::Scalar<int> cmx_x_leftsums_SimNeData = Monitored::Scalar<int>("cmx_x_leftsums_SimNeData", 0);
  Monitored::Scalar<int> cmx_y_leftsums_SimNeData = Monitored::Scalar<int>("cmx_y_leftsums_SimNeData", 0);
  Monitored::Scalar<int> cmx_w_leftsums_SimNeData = Monitored::Scalar<int>("cmx_w_leftsums_SimNeData", 0);
  Monitored::Scalar<int> cmx_x_rightsums_SimNeData = Monitored::Scalar<int>("cmx_x_rightsums_SimNeData", 0);
  Monitored::Scalar<int> cmx_y_rightsums_SimNeData = Monitored::Scalar<int>("cmx_y_rightsums_SimNeData", 0);
  Monitored::Scalar<int> cmx_w_rightsums_SimNeData = Monitored::Scalar<int>("cmx_w_rightsums_SimNeData", 0);


  std::vector<unsigned int> hits0Sim(m_crates * m_cmxs);
  std::vector<unsigned int> hits1Sim(m_crates * m_cmxs);
  std::vector<unsigned int> hits0(m_crates * m_cmxs);
  std::vector<unsigned int> hits1(m_crates * m_cmxs);
  CmxCpHitsMap::const_iterator cmxSimMapIter = cmxSimMap.begin();
  CmxCpHitsMap::const_iterator cmxSimMapIterEnd = cmxSimMap.end();
  CmxCpHitsMap::const_iterator cmxMapIter = cmxMap.begin();
  CmxCpHitsMap::const_iterator cmxMapIterEnd = cmxMap.end();

  while (cmxSimMapIter != cmxSimMapIterEnd || cmxMapIter != cmxMapIterEnd) {

    int cmxSimKey = 0;
    int cmxKey = 0;
    unsigned int cmxSimHits0 = 0;
    unsigned int cmxSimHits1 = 0;
    unsigned int cmxHits0 = 0;
    unsigned int cmxHits1 = 0;
    int crate = 0;
    int cmx = 0;
    int source = 0;

    if (cmxSimMapIter != cmxSimMapIterEnd)
      cmxSimKey = cmxSimMapIter->first;
    if (cmxMapIter != cmxMapIterEnd)
      cmxKey = cmxMapIter->first;

    if ((cmxMapIter == cmxMapIterEnd) ||
        ((cmxSimMapIter != cmxSimMapIterEnd) && (cmxKey > cmxSimKey))) {

      // Sim CMX Hits but no Data CMX Hits

      const xAOD::CMXCPHits *cmxS = cmxSimMapIter->second;
      ++cmxSimMapIter;
      source = cmxS->sourceComponent();
      if (local && source != xAOD::CMXCPHits::LOCAL)
        continue;
      if (remote && source != xAOD::CMXCPHits::LOCAL)
        continue;
      if (total && source != xAOD::CMXCPHits::TOTAL)
        continue;
      if (topo && source != xAOD::CMXCPHits::TOPO_CHECKSUM &&
          source != xAOD::CMXCPHits::TOPO_OCCUPANCY_MAP &&
          source != xAOD::CMXCPHits::TOPO_OCCUPANCY_COUNTS)
        continue;
      cmxSimHits0 = cmxS->hits0();
      cmxSimHits1 = cmxS->hits1();
      crate = cmxS->crate();
      cmx = cmxS->cmx();

    } else if ((cmxSimMapIter == cmxSimMapIterEnd) ||
               ((cmxMapIter != cmxMapIterEnd) && (cmxSimKey > cmxKey))) {

      // Data CMX Hits but no Sim CMX Hits

      const xAOD::CMXCPHits *cmxD = cmxMapIter->second;
      ++cmxMapIter;
      source = cmxD->sourceComponent();
      if (local && source != xAOD::CMXCPHits::LOCAL)
        continue;
      if (remote && source != xAOD::CMXCPHits::REMOTE_0 &&
          source != xAOD::CMXCPHits::REMOTE_1 &&
          source != xAOD::CMXCPHits::REMOTE_2)
        continue;
      if (total && source != xAOD::CMXCPHits::TOTAL)
        continue;
      if (topo && source != xAOD::CMXCPHits::TOPO_CHECKSUM &&
          source != xAOD::CMXCPHits::TOPO_OCCUPANCY_MAP &&
          source != xAOD::CMXCPHits::TOPO_OCCUPANCY_COUNTS)
        continue;
      cmxHits0 = cmxD->hits0();
      cmxHits1 = cmxD->hits1();
      crate = cmxD->crate();
      cmx = cmxD->cmx();

    } else {

      // Have both

      const xAOD::CMXCPHits *cmxS = cmxSimMapIter->second;
      const xAOD::CMXCPHits *cmxD = cmxMapIter->second;
      ++cmxSimMapIter;
      ++cmxMapIter;
      source = cmxS->sourceComponent();
      if (local && source != xAOD::CMXCPHits::LOCAL)
        continue;
      if (remote && source != xAOD::CMXCPHits::LOCAL &&
          source != xAOD::CMXCPHits::REMOTE_0 &&
          source != xAOD::CMXCPHits::REMOTE_1 &&
          source != xAOD::CMXCPHits::REMOTE_2)
        continue;
      if (total && source != xAOD::CMXCPHits::TOTAL)
        continue;
      if (topo && source != xAOD::CMXCPHits::TOPO_CHECKSUM &&
          source != xAOD::CMXCPHits::TOPO_OCCUPANCY_MAP &&
          source != xAOD::CMXCPHits::TOPO_OCCUPANCY_COUNTS)
        continue;
      cmxSimHits0 = cmxS->hits0();
      cmxSimHits1 = cmxS->hits1();
      cmxHits0 = cmxD->hits0();
      cmxHits1 = cmxD->hits1();
      crate = cmxS->crate();
      cmx = cmxS->cmx();
    }

    if (!cmxSimHits0 && !cmxSimHits1 && !cmxHits0 && !cmxHits1)
      continue;

    //  Fill in error plots

    if (local || total) {
      int loc = crate * m_cmxs + cmx;
      const int cmxBins = m_crates * m_cmxs;
      const int bit =
          (local) ? (1 << LocalSumMismatch) : (1 << TotalSumMismatch);
      // loc for the histogram filling
      int loc_fill = (local) ? loc : 14 + cmx;
      if (cmxSimHits0 == cmxHits0 && cmxSimHits1 == cmxHits1) {
        errors[loc] |= bit;
	cmx_sum_loc_SimEqData=loc_fill;
	fill(m_packageName,cmx_sum_loc_SimEqData);	  
      } else {
        errors[loc + cmxBins] |= bit;	
        if ((cmxSimHits0 || cmxSimHits1) && (cmxHits0 || cmxHits1)) {
	  cmx_sum_loc_SimNeData=loc_fill;
	  fill(m_packageName,cmx_sum_loc_SimNeData);	  
        } else if (!cmxHits0 && !cmxHits1) {
	  cmx_sum_loc_SimNoData=loc_fill;
	  fill(m_packageName,cmx_sum_loc_SimNoData);	  
        } else {
	  cmx_sum_loc_DataNoSim=loc_fill;
	  fill(m_packageName,cmx_sum_loc_SimEqData);	  	  
	}
      }
      // the loc used for filling histograms
      loc = (local) ? loc : 14 + cmx;
      // now divide it by 2
      loc /= 2;
      const int nThresh = 8;
      const int thrLen = 3;
      int same0 = thresholdsSame(cmxHits0, cmxSimHits0, nThresh, thrLen);
      int diff0 = thresholdsDiff(cmxHits0, cmxSimHits0, nThresh, thrLen);
      int same1 = thresholdsSame(cmxHits1, cmxSimHits1, nThresh, thrLen);
      int diff1 = thresholdsDiff(cmxHits1, cmxSimHits1, nThresh, thrLen);
      if (cmx) {
	  fillXVsThresholds(cmx_x_rightsums_SimEqData,cmx_y_rightsums_SimEqData,cmx_w_rightsums_SimEqData,
			    loc, same0, nThresh, 1);
	  fillXVsThresholds(cmx_x_rightsums_SimNeData,cmx_y_rightsums_SimNeData,cmx_w_rightsums_SimNeData,
			    loc, diff0, nThresh, 1);
	  const int offset = nThresh;
	  fillXVsThresholds(cmx_x_rightsums_SimEqData,cmx_y_rightsums_SimEqData,cmx_w_rightsums_SimEqData,
			    loc, same1, nThresh, 1, offset);
	  fillXVsThresholds(cmx_x_rightsums_SimNeData,cmx_y_rightsums_SimNeData,cmx_w_rightsums_SimNeData,
			    loc, diff1, nThresh, 1, offset);
      } else {
	  fillXVsThresholds(cmx_x_leftsums_SimEqData,cmx_y_leftsums_SimEqData,cmx_w_leftsums_SimEqData,
			    loc, same0, nThresh, 1);
	  fillXVsThresholds(cmx_x_leftsums_SimNeData,cmx_y_leftsums_SimNeData,cmx_w_leftsums_SimNeData,
			    loc, diff0, nThresh, 1);
	  const int offset = nThresh;
	  fillXVsThresholds(cmx_x_leftsums_SimEqData,cmx_y_leftsums_SimEqData,cmx_w_leftsums_SimEqData,
			    loc, same1, nThresh, 1, offset);
	  fillXVsThresholds(cmx_x_leftsums_SimNeData,cmx_y_leftsums_SimNeData,cmx_w_leftsums_SimNeData,
			    loc, diff1, nThresh, 1, offset);
      }
    } else if (remote) {
      if (source == xAOD::CMXCPHits::LOCAL) {
        if (crate != m_crates - 1) {
          hits0Sim[crate * m_cmxs + cmx] = cmxSimHits0;
          hits1Sim[crate * m_cmxs + cmx] = cmxSimHits1;
        }
      } else {
        const int remCrate = source - xAOD::CMXCPHits::REMOTE_0;
        hits0[remCrate * m_cmxs + cmx] = cmxHits0;
        hits1[remCrate * m_cmxs + cmx] = cmxHits1;
      }
    } else {
      const int locX = crate * m_cmxs + cmx;
      const int locY = source - xAOD::CMXCPHits::TOPO_CHECKSUM;
      const int cmxBins = m_crates * m_cmxs;
      const int bit = (1 << TopoMismatch);
      if (cmxSimHits0 == cmxHits0 && cmxSimHits1 == cmxHits1) {
        errors[locX] |= bit;
	cmxTopoLocXSimEqData=locX;
	cmxTopoLocYSimEqData=locY;
	fill(m_packageName,cmxTopoLocXSimEqData,cmxTopoLocYSimEqData);
      } else {
        errors[locX + cmxBins] |= bit;
        if ((cmxSimHits0 || cmxSimHits1) && (cmxHits0 || cmxHits1)) {
	  cmxTopoLocXSimNeData=locX;
	  cmxTopoLocYSimNeData=locY;
	  fill(m_packageName,cmxTopoLocXSimNeData,cmxTopoLocYSimNeData);
        } else if (!cmxHits0 && !cmxHits1) {
	  cmxTopoLocXSimNoData=locX;
	  cmxTopoLocYSimNoData=locY;
	  fill(m_packageName,cmxTopoLocXSimNoData,cmxTopoLocYSimNoData);
        } else {
	  cmxTopoLocXDataNoSim=locX;
	  cmxTopoLocYDataNoSim=locY;
	  fill(m_packageName,cmxTopoLocXDataNoSim,cmxTopoLocYDataNoSim);
	}
      }
    }
  }
  if (remote) {
    for (int crate = 0; crate < m_crates - 1; ++crate) {
      for (int cmx = 0; cmx < m_cmxs; ++cmx) {
        int loc = crate * m_cmxs + cmx;
        const int cmxBins = m_crates * m_cmxs;
        const int bit = (1 << RemoteSumMismatch);
        const unsigned int hd0 = hits0[loc];
        const unsigned int hd1 = hits1[loc];
        const unsigned int hs0 = hits0Sim[loc];
        const unsigned int hs1 = hits1Sim[loc];

        if (!hd0 && !hd1 && !hs0 && !hs1)
          continue;

        //TH1F_LW *hist = 0;
	int loc_fill = loc +=8;
        if (hs0 == hd0 && hs1 == hd1) {
          errors[loc] |= bit;
	  cmx_sum_loc_SimEqData=loc_fill;
	  fill(m_packageName,cmx_sum_loc_SimEqData);	  
        } else {
          errors[loc + cmxBins] |= bit;	  
          if ((hs0 || hs1) && (hd0 || hd1)) {
	    cmx_sum_loc_SimNeData=loc_fill;
	    fill(m_packageName,cmx_sum_loc_SimNeData);	  
          } else if (!hd0 && !hd1) {
	    cmx_sum_loc_SimNoData=loc_fill;
	    fill(m_packageName,cmx_sum_loc_SimNoData);	  
          } else {
	    cmx_sum_loc_DataNoSim=loc_fill;
	    fill(m_packageName,cmx_sum_loc_DataNoSim);	  
	  }
        }
	// this is the loc used for filling
        loc += 8;
	// and for the rest
        loc /= 2;
        const int nThresh = 8;
        const int thrLen = 3;
        int same0 = thresholdsSame(hd0, hs0, nThresh, thrLen);
        int diff0 =  thresholdsDiff(hd0, hs0, nThresh, thrLen);
        int same1 = thresholdsSame(hd1, hs1, nThresh, thrLen);
        int diff1 = thresholdsDiff(hd1, hs1, nThresh, thrLen);
	if (cmx) {
	  fillXVsThresholds(cmx_x_rightsums_SimEqData,cmx_y_rightsums_SimEqData,cmx_w_rightsums_SimEqData,
			    loc, same0, nThresh, 1);
	  fillXVsThresholds(cmx_x_rightsums_SimNeData,cmx_y_rightsums_SimNeData,cmx_w_rightsums_SimNeData,
			    loc, diff0, nThresh, 1);
	  const int offset = nThresh;
	  fillXVsThresholds(cmx_x_rightsums_SimEqData,cmx_y_rightsums_SimEqData,cmx_w_rightsums_SimEqData,
			    loc, same1, nThresh, 1, offset);
	  fillXVsThresholds(cmx_x_rightsums_SimNeData,cmx_y_rightsums_SimNeData,cmx_w_rightsums_SimNeData,
			    loc, diff1, nThresh, 1, offset);
	} else {
	  fillXVsThresholds(cmx_x_leftsums_SimEqData,cmx_y_leftsums_SimEqData,cmx_w_leftsums_SimEqData,
			    loc, same0, nThresh, 1);
	  fillXVsThresholds(cmx_x_leftsums_SimNeData,cmx_y_leftsums_SimNeData,cmx_w_leftsums_SimNeData,
			    loc, diff0, nThresh, 1);
	  const int offset = nThresh;
	  fillXVsThresholds(cmx_x_leftsums_SimEqData,cmx_y_leftsums_SimEqData,cmx_w_leftsums_SimEqData,
			    loc, same1, nThresh, 1, offset);
	  fillXVsThresholds(cmx_x_leftsums_SimNeData,cmx_y_leftsums_SimNeData,cmx_w_leftsums_SimNeData,
			    loc, diff1, nThresh, 1, offset);
	}
      }
    }
  }
}


// Fill weighted thresholds 2D, Y axis

void CpmSimMonitorAlgorithm::fillXVsThresholds(Monitored::Scalar<int> &xitem, 
					       Monitored::Scalar<int> &yitem,
					       Monitored::Scalar<int> &witem,
					       int x,int val, int nThresh, int nBits, int offset) const
{
  if (val) {
    const int mask = (1 << nBits) - 1;
    for (int thr = 0; thr < nThresh; ++thr) {
      const int hit = (val >> (nBits*thr)) & mask;
      if (hit) {
	xitem=x;
	yitem=thr+offset;
	witem=hit;
	fill(m_packageName, xitem, yitem, witem);
      }
    }
  }
}

// Flag which threshold hit values are non-zero and the same

int CpmSimMonitorAlgorithm::thresholdsSame(int val1, int val2,
					   int nThresh, int nBits) const
{
  int result = 0;
  const int mask = (1 << nBits) - 1;
  for (int thr = 0; thr < nThresh; ++thr) {
    const int hit1 = (val1 >> (nBits*thr)) & mask;
    const int hit2 = (val2 >> (nBits*thr)) & mask;
    if (hit1 && (hit1 == hit2)) result |= (1 << thr);
  }
  return result;
}

// Flag which threshold hit values are different

int CpmSimMonitorAlgorithm::thresholdsDiff(int val1, int val2,
					   int nThresh, int nBits) const
{
  int result = 0;
  const int mask = (1 << nBits) - 1;
  for (int thr = 0; thr < nThresh; ++thr) {
    const int hit1 = (val1 >> (nBits*thr)) & mask;
    const int hit2 = (val2 >> (nBits*thr)) & mask;
    if (hit1 != hit2) result |= (1 << thr);
  }
  return result;
}



// Set up TriggerTower map

StatusCode CpmSimMonitorAlgorithm::setupMap(const xAOD::TriggerTowerContainer *coll,
					    TriggerTowerMapEm &mapEm, 
					    TriggerTowerMapHad &mapHad) const {

  LVL1::TriggerTowerKey towerKey;
  xAOD::TriggerTowerContainer::const_iterator pos = coll->begin();
  xAOD::TriggerTowerContainer::const_iterator posE = coll->end();
  int emE = 0;
  int hadE = 0;
  for (; pos != posE; ++pos) {
    const xAOD::TriggerTower *tt = (*pos);
    const int layer = (*pos)->layer();
    if (layer == 0)
      emE = (*pos)->cpET();
    ;
    if (layer == 1)
      hadE = (*pos)->cpET();
    ;
    const double eta = (*pos)->eta();
    if (eta > -2.5 && eta < 2.5 && (emE > 0 || hadE > 0)) {
      const double phi = (*pos)->phi();
      const int key = towerKey.ttKey(phi, eta);
      if (emE > 0)
	mapEm.insert(std::make_pair(key, tt));
      if (hadE > 0)
	mapHad.insert(std::make_pair(key, tt));
    }
  }

  return StatusCode::SUCCESS;
}


// Set up CpmTower map

StatusCode CpmSimMonitorAlgorithm::setupMap(const xAOD::CPMTowerContainer* coll, CpmTowerMap &map) const {

  LVL1::TriggerTowerKey towerKey;
  xAOD::CPMTowerContainer::const_iterator pos = coll->begin();
  xAOD::CPMTowerContainer::const_iterator posE = coll->end();
  for (; pos != posE; ++pos) {
    CpmTowerMap::mapped_type cp = (*pos);
    const double eta = (*pos)->eta();
    const double phi = (*pos)->phi();
    const int key = towerKey.ttKey(phi, eta);
    map.insert(std::make_pair(key, cp));
  }
  return StatusCode::SUCCESS;
}

// Set up CpmTobRoi map

StatusCode CpmSimMonitorAlgorithm::setupMap(const xAOD::CPMTobRoIContainer *coll,
                        CpmTobRoiMap &map) const {
  if (coll) {
    xAOD::CPMTobRoIContainer::const_iterator pos = coll->begin();
    xAOD::CPMTobRoIContainer::const_iterator posE = coll->end();
    for (; pos != posE; ++pos) {
      const int crate = (*pos)->crate();
      const int cpm = (*pos)->cpm();
      const int chip = (*pos)->chip();
      const int loc = (*pos)->location();
      const int type = (*pos)->type();
      const int key =
          (((((((type << 2) | crate) << 4) | cpm) << 3) | chip) << 3) | loc;
      map.insert(std::make_pair(key, *pos));
    }
  }
  return StatusCode::SUCCESS;
}

// Set up CmxCpTob map

StatusCode CpmSimMonitorAlgorithm::setupMap(const xAOD::CMXCPTobContainer* coll, CmxCpTobMap &map,
                        std::vector<int> *parityMap) const {

  xAOD::CMXCPTobContainer::const_iterator pos = coll->begin();
  xAOD::CMXCPTobContainer::const_iterator posE = coll->end();
  for (; pos != posE; ++pos) {
    const int crate = (*pos)->crate();
    const int cpm = (*pos)->cpm();
    const int cmx = (*pos)->cmx();
    const int chip = (*pos)->chip();
    const int loc = (*pos)->location();
    const int key =
      (((((((crate << 1) | cmx) << 4) | cpm) << 4) | chip) << 2) | loc;
    map.insert(std::make_pair(key, *pos));
    if (parityMap) {
      LVL1::DataError err((*pos)->error());
      const int parity = err.get(LVL1::DataError::ParityMerge);
      if (parity) {
	const int index = (crate * m_cmxs + cmx) * m_modules + cpm - 1;
	(*parityMap)[index] = parity;
      }
    }
  }
  return StatusCode::SUCCESS;
}

// Set up CmxCpHits map

StatusCode CpmSimMonitorAlgorithm::setupMap(const xAOD::CMXCPHitsContainer *coll,
					    CmxCpHitsMap &map) const {
  xAOD::CMXCPHitsContainer::const_iterator pos = coll->begin();
  xAOD::CMXCPHitsContainer::const_iterator posE = coll->end();
  for (; pos != posE; ++pos) {
    const int crate = (*pos)->crate();
    const int cmx = (*pos)->cmx();
    const int source = (*pos)->sourceComponent();
    const int key = (crate * 2 + cmx) * 8 + source;
    map.insert(std::make_pair(key, *pos));
  }
  return StatusCode::SUCCESS;
}

// Simulate CPM RoIs from CPM Towers

void CpmSimMonitorAlgorithm::simulate(const CpmTowerMap *towers, const CpmTowerMap *towersOv,
				      xAOD::CPMTobRoIContainer *rois) const {

  ATH_MSG_DEBUG("Simulate CPM TOB RoIs from CPM Towers");

  // Process a crate at a time to use overlap data
  std::vector<CpmTowerMap> crateMaps(m_crates);
  LVL1::CoordToHardware converter;
  std::unique_ptr<xAOD::CPMTowerContainer> tempColl = std::make_unique<xAOD::CPMTowerContainer>();
  std::unique_ptr<xAOD::CPMTowerAuxContainer> tempCollAux = std::make_unique<xAOD::CPMTowerAuxContainer>();

  tempColl.get()->setStore(tempCollAux.get());
  for (const auto& iter : *towers) {
    CpmTowerMap::mapped_type tt = ttCheck(iter.second, tempColl.get());
    const LVL1::Coordinate coord(tt->phi(), tt->eta());
    const int crate = converter.cpCrate(coord);
    if (crate >= m_crates)
      continue;
    crateMaps[crate].insert(std::make_pair(iter.first, tt));
  }
  // If overlap data not present take from core data
  for (const auto& iter : ((m_overlapPresent) ? *towersOv : *towers)) {
    CpmTowerMap::mapped_type tt = ttCheck(iter.second, tempColl.get());
    const LVL1::Coordinate coord(tt->phi(), tt->eta());
    const int crate = converter.cpCrateOverlap(coord);
    if (crate >= m_crates)
      continue;
    crateMaps[crate].insert(std::make_pair(iter.first, tt));
  }
  for (int crate = 0; crate < m_crates; ++crate) {
    xAOD::CPMTobRoIContainer *roiTemp =
        new xAOD::CPMTobRoIContainer(SG::VIEW_ELEMENTS);

    m_cpmTool->findCPMTobRoIs(towers, roiTemp, 1);

    xAOD::CPMTobRoIContainer::iterator roiIter = roiTemp->begin();
    xAOD::CPMTobRoIContainer::iterator roiIterE = roiTemp->end();
    for (; roiIter != roiIterE; ++roiIter) {
      if ((*roiIter)->crate() == crate) {
        rois->push_back(*roiIter);
      }
    }
    delete roiTemp;
  }
}

// Simulate CPM RoIs from CPM Towers quick version

void CpmSimMonitorAlgorithm::simulate(const CpmTowerMap *towers,
				      xAOD::CPMTobRoIContainer *rois) const {

  ATH_MSG_DEBUG("Simulate CPM TOB RoIs from CPM Towers");
  
  m_cpmTool->findCPMTobRoIs(towers, rois, 1);
}

// Simulate CMX-CP TOBs from CPM RoIs

void CpmSimMonitorAlgorithm::simulate(const xAOD::CPMTobRoIContainer *rois,
				      xAOD::CMXCPTobContainer *tobs) const {

  ATH_MSG_DEBUG("Simulate CMX TOBs from CPM TOB RoIs");
  
  m_cpCmxTool->formCMXCPTob(rois, tobs);
}

// Simulate CMX Hit sums from CMX TOBs

void CpmSimMonitorAlgorithm::simulate(const xAOD::CMXCPTobContainer *tobs,
				      xAOD::CMXCPHitsContainer *hits, 
				      int selection) const {

  ATH_MSG_DEBUG("Simulate CMX Hit sums from CMX TOBs");

  if (selection == xAOD::CMXCPHits::LOCAL) {
    m_cpCmxTool->formCMXCPHitsCrate(tobs, hits);
  } else if (selection == xAOD::CMXCPHits::TOPO_CHECKSUM) {
    m_cpCmxTool->formCMXCPHitsTopo(tobs, hits);
  }
}

// Simulate CMX Total Hit sums from Remote/Local

void CpmSimMonitorAlgorithm::simulate(const xAOD::CMXCPHitsContainer *hitsIn,
				      xAOD::CMXCPHitsContainer *hitsOut) const {
  
  ATH_MSG_DEBUG("Simulate CMX Total Hit sums from Remote/Local");

  m_cpCmxTool->formCMXCPHitsSystem(hitsIn, hitsOut);
}


int CpmSimMonitorAlgorithm::fpga(int crate, double phi) const {
  const double phiGran = M_PI / 32.;
  const double phiBase = M_PI / 2. * double(crate);
  const int phiBin = int(floor((phi - phiBase) / phiGran)) + 2;
  return 2 * (phiBin / 2);
}


// Return a tower with zero energy if parity bit is set

CpmSimMonitorAlgorithm::CpmTowerMap::mapped_type
CpmSimMonitorAlgorithm::ttCheck( CpmTowerMap::mapped_type tt, xAOD::CPMTowerContainer* coll) const
{
  const LVL1::DataError emError(tt->emError());
  const LVL1::DataError hadError(tt->hadError());
  const int emParity = emError.get(LVL1::DataError::Parity);
  const int hadParity = hadError.get(LVL1::DataError::Parity);
  if ((emParity && tt->emEnergy()) || (hadParity && tt->hadEnergy())) {
    const int peak = tt->peak();
    std::vector<uint8_t> emEnergyVec(tt->emEnergyVec());
    std::vector<uint8_t> hadEnergyVec(tt->hadEnergyVec());
    std::vector<uint32_t> emErrorVec(tt->emErrorVec());
    std::vector<uint32_t> hadErrorVec(tt->hadErrorVec());
    if (emParity)
      emEnergyVec[peak] = 0;
    if (hadParity)
      hadEnergyVec[peak] = 0;
    xAOD::CPMTower *ct = new xAOD::CPMTower();
    ct->makePrivateStore();
    ct->initialize(tt->eta(), tt->phi(), emEnergyVec, hadEnergyVec, emErrorVec,
                   hadErrorVec, peak);
    coll->push_back(ct);
    return ct;
  }
  return tt;
}

bool CpmSimMonitorAlgorithm::limitedRoiSet(int crate, const xAOD::RODHeaderContainer *rodTES) const {

  int limitedRoi = 0;
  xAOD::RODHeaderContainer::const_iterator rodIter = rodTES->begin();
  xAOD::RODHeaderContainer::const_iterator rodIterE = rodTES->end();
  for (; rodIter != rodIterE; ++rodIter) {
    const xAOD::RODHeader *rod = *rodIter;
    const int rodCrate = rod->crate() - 8;
    if (rodCrate >= 0 && rodCrate < m_crates && rod->dataType() == 1 &&
	rod->limitedRoISet()) {
      limitedRoi |= (1 << rodCrate);
    }
  }
  return (((limitedRoi >> crate) & 0x1) == 1);
}

bool CpmSimMonitorAlgorithm::limitedRoiSet(int crate, SG::ReadHandle<xAOD::RODHeaderContainer> &rodTES) const {

  int limitedRoi = 0;
  xAOD::RODHeaderContainer::const_iterator rodIter = (*rodTES).begin();
  xAOD::RODHeaderContainer::const_iterator rodIterE = (*rodTES).end();
  for (; rodIter != rodIterE; ++rodIter) {
    const xAOD::RODHeader *rod = *rodIter;
    const int rodCrate = rod->crate() - 8;
    if (rodCrate >= 0 && rodCrate < m_crates && rod->dataType() == 1 &&
	rod->limitedRoISet()) {
      limitedRoi |= (1 << rodCrate);
    }
  }
  return (((limitedRoi >> crate) & 0x1) == 1);
}
