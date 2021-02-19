/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CpmMonitorAlgorithm.h"
#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"

CpmMonitorAlgorithm::CpmMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator),
    m_phiScaleTT(32./M_PI)
{
}

StatusCode CpmMonitorAlgorithm::initialize() {

  ATH_MSG_DEBUG("CpmMonitorAlgorith::initialize");
  ATH_MSG_DEBUG("Package Name "<< m_packageName);
  // container names
  ATH_MSG_DEBUG("m_xAODTriggerTowerContainerName"<< m_xAODTriggerTowerContainerName); 
  ATH_MSG_DEBUG("m_cpmTowerLocation"<< m_cpmTowerLocation); 
  ATH_MSG_DEBUG("m_cpmTowerLocationOverlap"<< m_cpmTowerLocationOverlap); 

  // steering parameters
  ATH_MSG_DEBUG("m_crates"<<m_crates ); 
  ATH_MSG_DEBUG("m_modules"<<m_modules ); 
  ATH_MSG_DEBUG("m_maxSlices"<<m_maxSlices );
  ATH_MSG_DEBUG("m_tobsPerCPM"<<m_tobsPerCPM );  
  ATH_MSG_DEBUG("m_isolBits"<<m_isolBits ); 
  ATH_MSG_DEBUG("m_maxTobsPerCmx"<<m_maxTobsPerCmx ); 

  // initialise all the containers that we need
  ATH_CHECK(m_xAODTriggerTowerContainerName.initialize());
  ATH_CHECK(m_cpmTowerLocation.initialize());
  ATH_CHECK(m_cpmTowerLocationOverlap.initialize());
  ATH_CHECK( m_cpmTobRoiLocation.initialize());
  ATH_CHECK( m_cmxCpTobLocation.initialize());
  ATH_CHECK( m_cmxCpHitsLocation.initialize());

  return AthMonitorAlgorithm::initialize();
}

StatusCode CpmMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {

  ATH_MSG_DEBUG("CpmMonitorAlgorithm::fillHistograms");

  // Retrieve Trigger Towers from SG
  SG::ReadHandle<xAOD::TriggerTowerContainer> triggerTowerTES(m_xAODTriggerTowerContainerName, ctx);

  ATH_CHECK(triggerTowerTES.isValid());

  // Retrieve Core CPM Towers from SG
  SG::ReadHandle<xAOD::CPMTowerContainer> cpmTowerTES(m_cpmTowerLocation, ctx);
  ATH_CHECK(cpmTowerTES.isValid());
  
  // Retrieve Overlap CPM Towers from SG
  SG::ReadHandle<xAOD::CPMTowerContainer> cpmTowerOverlapTES(m_cpmTowerLocationOverlap, ctx);
  ATH_CHECK(cpmTowerOverlapTES.isValid());

  //Retrieve CPM TOB RoIs from SG
  SG::ReadHandle<xAOD::CPMTobRoIContainer> cpmTobRoiTES(m_cpmTobRoiLocation, ctx);
  ATH_CHECK(cpmTobRoiTES.isValid());

  //Retrieve CMX-CP TOBs from SG
  SG::ReadHandle<xAOD::CMXCPTobContainer> cmxCpTobTES(m_cmxCpTobLocation, ctx);
  ATH_CHECK(cmxCpTobTES.isValid());

  //Retrieve CMX-CP Hits from SG
  SG::ReadHandle<xAOD::CMXCPHitsContainer> cmxCpHitsTES(m_cmxCpHitsLocation, ctx);
  ATH_CHECK(cmxCpHitsTES.isValid());

  //
  // array of monitored items
  std::vector<std::reference_wrapper<Monitored::IMonitoredVariable>> variables;

  // Global plots
  // Create a vector of towers we want to monitor 
  std::vector<MonitorTT> monTTs;
  xAOD::TriggerTowerContainer::const_iterator ttIterator = (*triggerTowerTES).begin();
  xAOD::TriggerTowerContainer::const_iterator ttIteratorEnd = (*triggerTowerTES).end();
  for (; ttIterator != ttIteratorEnd; ++ttIterator) {
    const int layer = (*ttIterator)->layer();
    const xAOD::TriggerTower_v2* tt = *ttIterator; 
    const double eta = (*ttIterator)->eta();
    if ( std::abs(eta) > 2.5) continue;
      
    if (!tt->cpET()) continue; 
    //check if the TriggerTower is in EM or HAD layer
    if (layer == 0) { //EM
      const int em = int(tt->cpET()); 
      if (em) { 
	MonitorTT monTT; 
	monTT.ttower=(*ttIterator);
	monTT.phi_scaled=(*ttIterator)->phi()*m_phiScaleTT;
	monTTs.push_back(monTT);
      }
    }
    if (layer == 1) { //HAD
      const int had = int(tt->cpET()); 
      if (had)  {
	MonitorTT monTT; 
	monTT.ttower=(*ttIterator);
	monTT.phi_scaled=(*ttIterator)->phi()*m_phiScaleTT;
	monTTs.push_back(monTT);
      }
    }      
  } //ttIterator

  // setup cutmasks to select the em and had TTs
  auto mask_em_TT = Monitored::Collection("mask_em_TT", monTTs, []( const auto &tower ) {return ( tower.ttower->layer()==0); } );  variables.push_back( mask_em_TT );
  auto mask_had_TT = Monitored::Collection("mask_had_TT", monTTs, []( const auto &tower ){return ( tower.ttower->layer()==1); } ); variables.push_back( mask_had_TT );

  // the variables to monitor
  auto etaTT = Monitored::Collection("etaTT", monTTs, []( const auto &tower ){return tower.ttower->eta();} );  variables.push_back( etaTT );
  auto phiTT = Monitored::Collection("phiTT", monTTs, []( const auto &tower ){return tower.phi_scaled;} );  variables.push_back( phiTT );

  // CPM Global maps

  // Vectors for error overview bits;
  std::vector<int> errorsCPM(m_crates * m_modules);
  std::vector<int> errorsCMX(m_crates * 2); // L/R
  //cpmTowerTES
  std::vector<MonitorCpmTT> monCpmTTs_em;
  std::vector<MonitorCpmTT> monCpmTTs_had;
  // scalars to fill bitwise histograms directly
  Monitored::Scalar<int> GLinkParityError = Monitored::Scalar<int>("GLinkParityError", 0);
  Monitored::Scalar<int> cpmLoc = Monitored::Scalar<int>("cpmLoc", 0);
  bool core=true;
  ATH_CHECK(fillCpmTowerVectors(cpmTowerTES, monCpmTTs_em, monCpmTTs_had, errorsCPM, core, cpmLoc, GLinkParityError));
  std::vector<MonitorCpmTT> monCpmOverlapTTs_em;
  std::vector<MonitorCpmTT> monCpmOverlapTTs_had;
  core=false;
  ATH_CHECK(fillCpmTowerVectors(cpmTowerOverlapTES, monCpmOverlapTTs_em, monCpmOverlapTTs_had, errorsCPM, core, cpmLoc, GLinkParityError));

  // add the CPM global variables to be monitored - exclude Overlap
  auto etCpmTT_em = Monitored::Collection("etCpmTT_em", monCpmTTs_em, []( const auto &tt ){return tt.ttower->emEnergy();} ); variables.push_back( etCpmTT_em );
  auto etaCpmTT_em = Monitored::Collection("etaCpmTT_em", monCpmTTs_em, []( const auto &tt ){return tt.ttower->eta();} ); variables.push_back( etaCpmTT_em );
  auto phiCpmTT_em = Monitored::Collection("phiCpmTT_em", monCpmTTs_em, []( const auto &tt ){return tt.ttower->phi();} ); variables.push_back( phiCpmTT_em );
  auto phiScaledCpmTT_em = Monitored::Collection("phiScaledCpmTT_em", monCpmTTs_em, []( const auto &tt ){return tt.phi_scaled;} ); 
  variables.push_back( phiScaledCpmTT_em );
  auto etCpmTT_had = Monitored::Collection("etCpmTT_had", monCpmTTs_had, []( const auto &tt ){return tt.ttower->hadEnergy();} ); variables.push_back( etCpmTT_had );
  auto etaCpmTT_had = Monitored::Collection("etaCpmTT_had", monCpmTTs_had, []( const auto &tt ){return tt.ttower->eta();} ); variables.push_back( etaCpmTT_had );
  auto phiCpmTT_had = Monitored::Collection("phiCpmTT_had", monCpmTTs_had, []( const auto &tt ){return tt.ttower->phi();} ); variables.push_back( phiCpmTT_had );
  auto phiScaledCpmTT_had = Monitored::Collection("phiScaledCpmTT_had", monCpmTTs_had, []( const auto &tt ){return tt.phi_scaled;} ); 
  variables.push_back( phiScaledCpmTT_had );

  // errors and slices are filled for sum of Core and Overlap
  std::vector<MonitorCpmTT> monCpmTTs_em_tot(monCpmTTs_em.begin(),monCpmTTs_em.end());
  monCpmTTs_em_tot.insert(monCpmTTs_em_tot.end(), monCpmOverlapTTs_em.begin(),monCpmOverlapTTs_em.end());
  std::vector<MonitorCpmTT> monCpmTTs_had_tot(monCpmTTs_had.begin(),monCpmTTs_had.end());
  monCpmTTs_had_tot.insert(monCpmTTs_had_tot.end(), monCpmOverlapTTs_had.begin(),monCpmOverlapTTs_had.end());
  // the variables
  auto etaCpmTT_em_tot = Monitored::Collection("etaCpmTT_em_tot", monCpmTTs_em_tot, []( const auto &tt ){return tt.ttower->eta();} ); 
  variables.push_back( etaCpmTT_em_tot) ;
  auto phiScaledCpmTT_em_tot = Monitored::Collection("phiScaledCpmTT_em_tot", monCpmTTs_em_tot, []( const auto &tt ){return tt.phi_scaled;} ); 
  variables.push_back( phiScaledCpmTT_em_tot );
  auto etaCpmTT_had_tot = Monitored::Collection("etaCpmTT_had_tot", monCpmTTs_had_tot, []( const auto &tt ){return tt.ttower->eta();} ); 
  variables.push_back( etaCpmTT_had_tot) ;
  auto phiScaledCpmTT_had_tot = Monitored::Collection("phiScaledCpmTT_had_tot", monCpmTTs_had_tot, []( const auto &tt ){return tt.phi_scaled;} ); 
  variables.push_back( phiScaledCpmTT_had_tot );


  // the masks
  auto parityErrorCpmTT_em = Monitored::Collection("parityErrorCpmTT_em", monCpmTTs_em_tot, []( const auto &tt ){return tt.emParityError;} ); 
  variables.push_back( parityErrorCpmTT_em );
  auto linkDownErrorCpmTT_em = Monitored::Collection("linkDownErrorCpmTT_em", monCpmTTs_em_tot, []( const auto &tt ){return tt.emLinkDownError;} ); 
  variables.push_back( linkDownErrorCpmTT_em );
  auto parityErrorCpmTT_had = Monitored::Collection("parityErrorCpmTT_had", monCpmTTs_had_tot, []( const auto &tt ){return tt.hadParityError;} ); 
  variables.push_back( parityErrorCpmTT_had );
  auto linkDownErrorCpmTT_had = Monitored::Collection("linkDownErrorCpmTT_had", monCpmTTs_had_tot, []( const auto &tt ){return tt.hadLinkDownError;} ); 
  variables.push_back( linkDownErrorCpmTT_had );

  // and the sum of the Core and Overlap for EM and HAD
  std::vector<MonitorCpmTT> monCpmTTs_tot(monCpmTTs_em_tot.begin(),monCpmTTs_em_tot.end());
  monCpmTTs_tot.insert(monCpmTTs_tot.end(), monCpmTTs_had_tot.begin(),monCpmTTs_had_tot.end());

  auto peakCpmTT_tot = Monitored::Collection("peakCpmTT_tot", monCpmTTs_tot, []( const auto &tt ){return tt.ttower->peak();} ); 
  variables.push_back( peakCpmTT_tot );
  auto sliceCpmTT_tot = Monitored::Collection("sliceCpmTT_tot", monCpmTTs_tot, []( const auto &tt ){return tt.slice;} ); 
  variables.push_back( sliceCpmTT_tot );

  //=============================================
  //  CPM TOB RoIs
  //=============================================

  std::vector<MonitorTobRoI> monTobRoIsEner;
  std::vector<MonitorTobRoI> monTobRoIsIsol;

  const int vecSize = m_crates * m_modules * 2;
  std::vector<int> tobCount(vecSize);
  LVL1::CPRoIDecoder decoder;
  xAOD::CPMTobRoIContainer::const_iterator crIterator    = (*cpmTobRoiTES).begin();
  xAOD::CPMTobRoIContainer::const_iterator crIteratorEnd = (*cpmTobRoiTES).end();
  // fill thresholds and bits as scalars in the loop
  auto thresholdsEm = Monitored::Scalar("bitsTobRoIsIsolEm", 0.);
  auto thresholdWeightsEm = Monitored::Scalar<float>("bitsTobRoIsIsolEmWeight", 1.);
  auto thresholdsTau = Monitored::Scalar("bitsTobRoIsIsolTau", 0.);
  auto thresholdWeightsTau = Monitored::Scalar("bitsTobRoIsIsolTauWeight", 0.);
  for (; crIterator != crIteratorEnd; ++crIterator) {
    const int type      = (*crIterator)->type();  // 0=EM, 1=Tau
    const int energy    = (*crIterator)->energy();
    const int isolation = (*crIterator)->isolation();
    const LVL1::CoordinateRange coord(decoder.coordinate((*crIterator)->roiWord())); 
    const double eta = coord.eta();
    const double phi = coord.phi();
    const double phiMod = phi * m_phiScaleTT - 0.5;
    const double etaMod = eta - 0.05;
    if (energy) {
      MonitorTobRoI monTobRoI;
      monTobRoI.tobroi=(*crIterator);
      monTobRoI.etaMod=etaMod;
      monTobRoI.phiMod=phiMod;
      monTobRoIsEner.push_back(monTobRoI);
    }
    if (isolation) {
      // fill isolation bits
      int * isolbits=getIsolationBits(isolation, m_isolBits, 1);
      for (int thr = 0; thr < m_isolBits; ++thr) {
	if (isolbits[thr]){
	  if(type==0) {
	    thresholdsEm = thr;
	    thresholdWeightsEm = isolbits[thr];
	    fill(m_packageName, thresholdsEm, thresholdWeightsEm);
	  } else {
	    thresholdsTau = thr;
	    thresholdWeightsTau = isolbits[thr];
	    fill(m_packageName, thresholdsTau, thresholdWeightsTau);
	  }
	}
      }
      //
      MonitorTobRoI monTobRoI;
      monTobRoI.tobroi=(*crIterator);
      monTobRoI.etaMod=etaMod;
      monTobRoI.phiMod=phiMod;
      monTobRoIsIsol.push_back(monTobRoI);
    }
    const int crate = (*crIterator)->crate();
    const int cpm   = (*crIterator)->cpm();
    ++tobCount[(crate * m_modules + cpm - 1) * 2 + type];
  }
  // Energy
  auto etaTobRoIsEner = Monitored::Collection("etaTobRoIsEner", monTobRoIsEner, []( const auto &roi ){return roi.etaMod;} ); 
  variables.push_back(etaTobRoIsEner);
  auto phiTobRoIsEner = Monitored::Collection("phiTobRoIsEner", monTobRoIsEner, []( const auto &roi ){return roi.phiMod;} );
  variables.push_back(phiTobRoIsEner);
  auto energyTobRoIsEner = Monitored::Collection("energyTobRoIsEner", monTobRoIsEner, []( const auto &roi ){return roi.tobroi->energy();} ); 
  variables.push_back(energyTobRoIsEner);
  auto typeTobRoIsEner = Monitored::Collection("typeTobRoIsEner", monTobRoIsEner, []( const auto &roi ){return roi.tobroi->type();} ); 
  variables.push_back(typeTobRoIsEner);

  // setup cutmasks to select EM or Tau
  auto mask_tobroi_ener_em = Monitored::Collection("mask_tobroi_ener_em", monTobRoIsEner, []( const auto &roi ) {return ( roi.tobroi->type()==0); } );  
  variables.push_back( mask_tobroi_ener_em );
  auto mask_tobroi_ener_tau = Monitored::Collection("mask_tobroi_ener_tau", monTobRoIsEner, []( const auto &roi ){return ( roi.tobroi->type()==1); } ); 
  variables.push_back( mask_tobroi_ener_tau );

  // Isol
  auto etaTobRoIsIsol = Monitored::Collection("etaTobRoIsIsol", monTobRoIsIsol, []( const auto &roi ){return roi.etaMod;} ); 
  variables.push_back(etaTobRoIsIsol);
  auto phiTobRoIsIsol = Monitored::Collection("phiTobRoIsIsol", monTobRoIsIsol, []( const auto &roi ){return roi.phiMod;} );
  variables.push_back(phiTobRoIsIsol);
  auto energyTobRoIsIsol = Monitored::Collection("energyTobRoIsIsol", monTobRoIsIsol, []( const auto &roi ){return roi.tobroi->isolation();} ); 
  variables.push_back(energyTobRoIsIsol);

  auto mask_tobroi_isol_em = Monitored::Collection("mask_tobroi_isol_em", monTobRoIsIsol, []( const auto &roi ) {return ( roi.tobroi->type()==0); } );  
  variables.push_back( mask_tobroi_isol_em );
  auto mask_tobroi_isol_tau = Monitored::Collection("mask_tobroi_isol_tau", monTobRoIsIsol, []( const auto &roi ){return ( roi.tobroi->type()==1); } ); 
  variables.push_back( mask_tobroi_isol_tau );


  // count ToBs
  std::vector<int> tobCountEm;
  std::vector<int> tobCountTau;
  for (int crate = 0; crate < m_crates; ++crate) {
    for (int cpm = 1; cpm <= m_modules; ++cpm) {
      for (int type = 0; type < 2; ++type) {
	int val = tobCount[(crate * m_modules + cpm - 1) * 2 + type];
	if (val) {
	  if (val > m_tobsPerCPM) val = m_tobsPerCPM + 1;
	  if (type == 0) {
	    tobCountEm.push_back(val);
	  } else {
	    tobCountTau.push_back(val);
	  }
	}
      }
    }
  }
  auto tobEm = Monitored::Collection("tobPerCPMEm", tobCountEm, []( const auto &tob ){return tob;} );  variables.push_back( tobEm );
  auto tobTau = Monitored::Collection("tobPerCPMTau", tobCountTau, []( const auto &tob ){return tob;} );  variables.push_back( tobTau );


  //=============================================
  //  CMX-CP TOBs
  //=============================================

  std::vector<MonitorCmxCpTob> monCmxCpTobEnerLeft;
  std::vector<MonitorCmxCpTob> monCmxCpTobEnerRight;
  std::vector<MonitorCmxCpTob> monCmxCpTobError;
  std::vector<MonitorCmxCpTob> monCmxCpTobIsolLeft;
  std::vector<MonitorCmxCpTob> monCmxCpTobIsolRight;

  tobCount.assign(vecSize, 0);
  std::vector<int> cmxCount(m_crates * 2);
  xAOD::CMXCPTobContainer::const_iterator cmxCpTobIter    = (*cmxCpTobTES).begin();
  xAOD::CMXCPTobContainer::const_iterator cmxCpTobIterEnd = (*cmxCpTobTES).end();
  //
  auto cmxCpmTobsIsolBitsLeft = Monitored::Scalar("cmxCpmTobsIsolBitsLeft", 0.);
  auto cmxCpmTobsIsolBitsLeftW = Monitored::Scalar<float>("cmxCpmTobsIsolBitsLeftWeight", 1.);
  auto cmxCpmTobsIsolBitsRight= Monitored::Scalar("cmxCpmTobsIsolBitsRight", 0.);
  auto cmxCpmTobsIsolBitsRightW = Monitored::Scalar<float>("cmxCpmTobsIsolBitsRightWeight", 1.);
  for (; cmxCpTobIter != cmxCpTobIterEnd; ++cmxCpTobIter) {
    const uint8_t crate     = (*cmxCpTobIter)->crate();
    const uint8_t cpm       = (*cmxCpTobIter)->cpm();     // 1-14
    const uint8_t cmx       = (*cmxCpTobIter)->cmx();     // 0=Left, 1=Right  (Assumed in Sim to be Left Tau, Right EM)
    const uint8_t chip      = (*cmxCpTobIter)->chip();    // 4 bits
    const uint8_t location  = (*cmxCpTobIter)->location();// 2 bits
    const uint8_t energy    = (*cmxCpTobIter)->energy();
    const uint8_t isolation = (*cmxCpTobIter)->isolation();
    const uint32_t error     = (*cmxCpTobIter)->error();
    const uint8_t x = crate * m_modules + cpm - 1;
    const uint8_t y = chip * 4 + location;
    if (energy) {
      MonitorCmxCpTob monCmxCpTob;
      monCmxCpTob.tob=(*cmxCpTobIter);
      monCmxCpTob.x=x;
      monCmxCpTob.y=y;
      if (cmx)
	monCmxCpTobEnerRight.push_back(monCmxCpTob);
      else
	monCmxCpTobEnerLeft.push_back(monCmxCpTob);
    }
    if (isolation) {
      MonitorCmxCpTob monCmxCpTob;
      monCmxCpTob.tob=(*cmxCpTobIter);
      monCmxCpTob.x=x;
      monCmxCpTob.y=y;
      if (cmx)
	monCmxCpTobIsolRight.push_back(monCmxCpTob);
      else
	monCmxCpTobIsolLeft.push_back(monCmxCpTob);

      int nBits=1; int offset = 0;
      const int mask = (1 << nBits) - 1;
      for (int thr = 0; thr < m_isolBits; ++thr) {
	const int hit = (isolation >> (nBits*thr)) & mask;
	if (hit) {
	  if(cmx) {
	    cmxCpmTobsIsolBitsRight=thr+offset;	  
	    cmxCpmTobsIsolBitsRightW=hit;	  
	    fill(m_packageName, cmxCpmTobsIsolBitsRight, cmxCpmTobsIsolBitsRightW);
	  } else {
	    cmxCpmTobsIsolBitsLeft=thr+offset;	  
	    cmxCpmTobsIsolBitsLeftW=hit;	  
	    fill(m_packageName, cmxCpmTobsIsolBitsLeft, cmxCpmTobsIsolBitsLeftW);
	  }
	}
      } // isol bits
    } // isolation
    if (error) {
      MonitorCmxCpTob monCmxCpTob;
      monCmxCpTob.tob=(*cmxCpTobIter);
      monCmxCpTob.x=x;
      monCmxCpTob.parityError=false;

      const LVL1::DataError err(error);
      if (err.get(LVL1::DataError::Overflow)) {
	tobCount[x * 2 + cmx] = m_tobsPerCPM + 1;
      }
      const int ybase = cmx * 5;
      monCmxCpTob.ybase=ybase;
      bool parity = false;
      if (err.get(LVL1::DataError::ParityMerge)) {
	parity = true;
	monCmxCpTob.parityError=true;
	monCmxCpTob.ybaseError=ybase;
      }
      if (err.get(LVL1::DataError::ParityPhase0)) {
	parity = true;
	monCmxCpTob.parityError=true;
	monCmxCpTob.ybaseError=ybase+1;
      }
      if (err.get(LVL1::DataError::ParityPhase1)) {
	parity = true;
	monCmxCpTob.parityError=true;
	monCmxCpTob.ybaseError=ybase+2;
      }
      if (err.get(LVL1::DataError::ParityPhase2)) {
	parity = true;
	monCmxCpTob.parityError=true;
	monCmxCpTob.ybaseError=ybase+3;
      }
      if (err.get(LVL1::DataError::ParityPhase3)) {
	parity = true;
	monCmxCpTob.parityError=true;
	monCmxCpTob.ybaseError=ybase+4;
      }
      if (parity) errorsCMX[crate * 2 + cmx] |= (1 << TOBParity);

      // and now push error structs
      monCmxCpTobError.push_back(monCmxCpTob);
    } // end of error 
    //
    if (energy || isolation || error) {
      ++tobCount[x * 2 + cmx];
      ++cmxCount[crate * 2 + cmx];
    }

    //
    auto cmxCpmTobsLeft = Monitored::Scalar("cmxCpmTobsLeft", 0.);
    auto cmxCpmTobsRight = Monitored::Scalar("cmxCpmTobsRight", 0.);
    auto cmxTobsCmxLeft = Monitored::Scalar("cmxTobsCmxLeft", 0.);
    auto cmxTobsCmxRight = Monitored::Scalar("cmxTobsCmxRight", 0.);
    for (int crate = 0; crate < m_crates; ++crate) {
      for (int cpm = 1; cpm <= m_modules; ++cpm) {
        for (int cmx = 0; cmx < 2; ++cmx) {
          int val = tobCount[(crate * m_modules + cpm - 1) * 2 + cmx];
          if (val) {
            if (val > m_tobsPerCPM) val = m_tobsPerCPM + 1;
            if (cmx == 0) {
	      cmxCpmTobsLeft=val;
	      fill(m_packageName, cmxCpmTobsLeft);
            } else {
	      cmxCpmTobsRight=val;
	      fill(m_packageName, cmxCpmTobsRight);
	    }
          }
        }
      }
      for (int cmx = 0; cmx < 2; ++cmx) {
        int val = cmxCount[crate * 2 + cmx];
        if (val) {
          if (val >= m_maxTobsPerCmx) val = m_maxTobsPerCmx - 1;
          if (cmx == 0) {
	    cmxTobsCmxLeft=val;
	    fill(m_packageName, cmxTobsCmxLeft);
          } else { 
	    cmxTobsCmxRight=val;
	    fill(m_packageName, cmxTobsCmxRight);
	  }
        }
      }
    }
  } // CMX loop end

  // now fill CMX-CP TOB monitor items
  // Energy
  auto cmxCpmTobsEnerLeft = Monitored::Collection("cmxCpmTobsEnerLeft", monCmxCpTobEnerLeft, []( const auto &tob ){return tob.tob->energy();} ); 
  variables.push_back(cmxCpmTobsEnerLeft);
  auto cmxCpmTobsEnerRight = Monitored::Collection("cmxCpmTobsEnerRight", monCmxCpTobEnerRight, []( const auto &tob ){return tob.tob->energy();} ); 
  variables.push_back(cmxCpmTobsEnerRight);
  
  auto cmxCpmTobsEnerXLeft = Monitored::Collection("cmxCpmTobsEnerXLeft", monCmxCpTobEnerLeft, []( const auto &tob ){return tob.x;} ); 
  variables.push_back(cmxCpmTobsEnerXLeft);
  auto cmxCpmTobsEnerYLeft = Monitored::Collection("cmxCpmTobsEnerYLeft", monCmxCpTobEnerLeft, []( const auto &tob ){return tob.y;} ); 
  variables.push_back(cmxCpmTobsEnerYLeft);
  auto cmxCpmTobsEnerXRight = Monitored::Collection("cmxCpmTobsEnerXRight", monCmxCpTobEnerRight, []( const auto &tob ){return tob.x;} ); 
  variables.push_back(cmxCpmTobsEnerXRight);
  auto cmxCpmTobsEnerYRight = Monitored::Collection("cmxCpmTobsEnerYRight", monCmxCpTobEnerRight, []( const auto &tob ){return tob.y;} ); 
  variables.push_back(cmxCpmTobsEnerYRight);

  // isolation
  auto cmxCpmTobsIsolLeft = Monitored::Collection("cmxCpmTobsIsolLeft", monCmxCpTobIsolLeft, []( const auto &tob ){return tob.tob->isolation();} ); 
  variables.push_back(cmxCpmTobsIsolLeft);
  auto cmxCpmTobsIsolRight = Monitored::Collection("cmxCpmTobsIsolRight", monCmxCpTobIsolRight, []( const auto &tob ){return tob.tob->isolation();} ); 
  variables.push_back(cmxCpmTobsIsolRight);

  auto cmxCpmTobsIsolXLeft = Monitored::Collection("cmxCpmTobsIsolXLeft", monCmxCpTobIsolLeft, []( const auto &tob ){return tob.x;} ); 
  variables.push_back(cmxCpmTobsIsolXLeft);
  auto cmxCpmTobsIsolYLeft = Monitored::Collection("cmxCpmTobsIsolYLeft", monCmxCpTobIsolLeft, []( const auto &tob ){return tob.y;} ); 
  variables.push_back(cmxCpmTobsIsolYLeft);
  auto cmxCpmTobsIsolXRight = Monitored::Collection("cmxCpmTobsIsolXRight", monCmxCpTobIsolRight, []( const auto &tob ){return tob.x;} ); 
  variables.push_back(cmxCpmTobsIsolXRight);
  auto cmxCpmTobsIsolYRight = Monitored::Collection("cmxCpmTobsIsolYRight", monCmxCpTobIsolRight, []( const auto &tob ){return tob.y;} ); 
  variables.push_back(cmxCpmTobsIsolYRight);

  // errors
  auto cmxCpmTobsErrorX = Monitored::Collection("cmxCpmTobsErrorX", monCmxCpTobError, []( const auto &tob ){return tob.x;} ); 
  variables.push_back(cmxCpmTobsErrorX);
  auto cmxCpmTobsErrorCmx = Monitored::Collection("cmxCpmTobsErrorCmx", monCmxCpTobError, []( const auto &tob ){return tob.tob->cmx();} ); 
  variables.push_back(cmxCpmTobsErrorCmx);
  auto cmxCpmTobsErrorYbase = Monitored::Collection("cmxCpmTobsErrorYbase", monCmxCpTobError, []( const auto &tob ){return tob.ybaseError;} ); 
  variables.push_back(cmxCpmTobsErrorYbase);
  // parity error mask
  auto cmxCpmTobsErrorParity = Monitored::Collection("cmxCpmTobsErrorParity", monCmxCpTobError, []( const auto &tob ){return tob.parityError;} ); 
  variables.push_back(cmxCpmTobsErrorParity);

  //
  //=============================================
  //  CMX-CP Hits
  //=============================================

  std::vector<MonitorCmxCpHits> monCmxCpHits;
  std::vector<MonitorCmxCpHits> monCmxCpHitsLeft;
  std::vector<MonitorCmxCpHits> monCmxCpHitsRight;

  // scalars to fill bitwise histograms directly
  Monitored::Scalar<int> cmxCpMapX = Monitored::Scalar<int>("cmxCpMapX", 0);
  Monitored::Scalar<int> cmxCpMapY = Monitored::Scalar<int>("cmxCpMapY", 0);
  Monitored::Scalar<int> cmxCpMapHit = Monitored::Scalar<int>("cmxCpMapHit", 0);
  Monitored::Scalar<int> cmxCpCountsX = Monitored::Scalar<int>("cmxCpCountsX", 0);
  Monitored::Scalar<int> cmxCpCountsY = Monitored::Scalar<int>("cmxCpCountsY", 0);
  Monitored::Scalar<int> cmxCpCountsHit = Monitored::Scalar<int>("cmxCpCountsHit", 0);
  //
  Monitored::Scalar<int> cmxTopoTobsCpmRight = Monitored::Scalar<int>("cmxTopoTobsCpmRight", 0);
  Monitored::Scalar<int> cmxTopoTobsCpmLeft = Monitored::Scalar<int>("cmxTopoTobsCpmLeft", 0);
  //
  Monitored::Scalar<int> cmxCpThresBinLeftX = Monitored::Scalar<int>("cmxCpThresBinLeftX", 0);
  Monitored::Scalar<int> cmxCpThresBinLeftY = Monitored::Scalar<int>("cmxCpThresBinLeftY", 0);
  Monitored::Scalar<int> cmxCpThresBinLeftHit = Monitored::Scalar<int>("cmxCpThresBinLeftHit", 0);
  Monitored::Scalar<int> cmxCpThresBinRightX = Monitored::Scalar<int>("cmxCpThresBinRightX", 0);
  Monitored::Scalar<int> cmxCpThresBinRightY = Monitored::Scalar<int>("cmxCpThresBinRightY", 0);
  Monitored::Scalar<int> cmxCpThresBinRightHit = Monitored::Scalar<int>("cmxCpThresBinRightHit", 0);

  cmxCount.assign(m_crates * 2, 0);
  xAOD::CMXCPHitsContainer::const_iterator cmIterator    = (*cmxCpHitsTES).begin();
  xAOD::CMXCPHitsContainer::const_iterator cmIteratorEnd = (*cmxCpHitsTES).end();
  for (; cmIterator != cmIteratorEnd; ++cmIterator) {
    const uint32_t hits0 = (*cmIterator)->hits0();
    const uint32_t hits1 = (*cmIterator)->hits1();
    const uint8_t crate  = (*cmIterator)->crate();
    const uint8_t cmx    = (*cmIterator)->cmx();
    const uint8_t source = (*cmIterator)->sourceComponent();
    const uint8_t slices = ((*cmIterator)->hitsVec0()).size();
    const uint8_t crateCmx = crate * 2 + cmx;

    //
    MonitorCmxCpHits monCmxCpHit;
    monCmxCpHit.hit=(*cmIterator);
    monCmxCpHit.crateCmx=crateCmx;
    monCmxCpHit.srcTopoCheckSum=false;
    monCmxCpHit.crateSlices=crate * m_maxSlices + slices - 1;
    if (source == xAOD::CMXCPHits::TOPO_CHECKSUM) {
      monCmxCpHit.srcTopoCheckSum=true;
    } else if (source == xAOD::CMXCPHits::TOPO_OCCUPANCY_MAP) {
      if (hits0) {
	const int nBits = 1;
	const int offset = 1;
	const int mask = (1 << nBits) - 1;
	for (int thr = 0; thr < m_modules; ++thr) {
	  const int hit = (hits0 >> (nBits*thr)) & mask;
	  if (hit) {
	    cmxCpMapX=thr+offset;
	    cmxCpMapY=crateCmx;
	    cmxCpMapHit=hit;
	    fill(m_packageName,cmxCpMapX,cmxCpMapY,cmxCpMapHit);
	  }
	}
      }   
    } else if (source == xAOD::CMXCPHits::TOPO_OCCUPANCY_COUNTS) {

      if (hits0) {
	const int nBits = 3;
	const int offset = 1;
	const int mask = (1 << nBits) - 1;
	for (int thr = 0; thr < (m_modules/2); ++thr) {
	  const int hit = (hits0 >> (nBits*thr)) & mask;
	  if (hit) {
	    cmxCpCountsX=thr+offset;
	    cmxCpCountsY=crateCmx;
	    cmxCpCountsHit=hit;
	    fill(m_packageName,cmxCpCountsX,cmxCpCountsY,cmxCpCountsHit);
	  }
	}
      }
      for (int mod = 0; mod < m_modules / 2; ++mod) {
	const int val = (hits0 >> (mod * 3)) & 0x7;
	if (val) {
	  if (cmx) {	    
	    cmxTopoTobsCpmRight=val;
	    fill(m_packageName,cmxTopoTobsCpmRight);
	  } else {
	    cmxTopoTobsCpmLeft=val;
	    fill(m_packageName,cmxTopoTobsCpmLeft);
	  }
	}
	cmxCount[crate * 2 + cmx] += val;
      } // hits0

      if (hits1) {
	const int nBits = 3;
	const int offset = m_modules / 2 + 1;
	const int mask = (1 << nBits) - 1;
	for (int thr = 0; thr < (m_modules/2); ++thr) {
	  const int hit = (hits1 >> (nBits*thr)) & mask;
	  if (hit) {
	    cmxCpCountsX=thr+offset;
	    cmxCpCountsY=crateCmx;
	    cmxCpCountsHit=hit;
	    fill(m_packageName,cmxCpCountsX,cmxCpCountsY,cmxCpCountsHit);
	  }
	}
	for (int mod = 0; mod < m_modules / 2; ++mod) {
	  const int val = (hits1 >> (mod * 3)) & 0x7;
	  if (val) {
	    if (cmx) {	    
	      cmxTopoTobsCpmRight=val;
	      fill(m_packageName,cmxTopoTobsCpmRight);
	    } else {
	      cmxTopoTobsCpmLeft=val;
	      fill(m_packageName,cmxTopoTobsCpmLeft);
	    }
	  }
	  cmxCount[crate * 2 + cmx] += val;
	}
      } // hits1
    } else {
      int bin = 0;
      if      (source == xAOD::CMXCPHits::LOCAL)    bin = crate;
      else if (source == xAOD::CMXCPHits::REMOTE_0) bin = m_crates;
      else if (source == xAOD::CMXCPHits::REMOTE_1) bin = m_crates + 1;
      else if (source == xAOD::CMXCPHits::REMOTE_2) bin = m_crates + 2;
      else if (source == xAOD::CMXCPHits::TOTAL)    bin = m_crates + 3;
      const int nThresh = m_thresholds / 2;
      if (hits0) {
	const int nBits = m_threshBits;
	const int offset = 0;
	const int mask = (1 << nBits) - 1;

	for (int thr = 0; thr < nThresh; ++thr) {
	  const int hit = (hits0 >> (nBits*thr)) & mask;
	  if (hit) {
	    if (cmx) {
	      cmxCpThresBinRightX=bin;
	      cmxCpThresBinRightY=thr+offset;
	      cmxCpThresBinRightHit=hit;
	      fill(m_packageName,cmxCpThresBinRightX,cmxCpThresBinRightY,cmxCpThresBinRightHit);
	    } else {
	      cmxCpThresBinLeftX=bin;
	      cmxCpThresBinLeftY=thr+offset;
	      cmxCpThresBinLeftHit=hit;
	      fill(m_packageName,cmxCpThresBinLeftX,cmxCpThresBinLeftY,cmxCpThresBinLeftHit);
	    }
	  }
	}
      } // hits0

      if (hits1) {
	// 
	const int nBits = 3;
	const int offset = nThresh;
	const int mask = (1 << nBits) - 1;
	for (int thr = 0; thr < nThresh; ++thr) {
	  const int hit = (hits0 >> (nBits*thr)) & mask;
	  if (hit) {
	    if (cmx) {
	      cmxCpThresBinRightX=bin;
	      cmxCpThresBinRightY=thr+offset;
	      cmxCpThresBinRightHit=hit;
	      fill(m_packageName,cmxCpThresBinRightX,cmxCpThresBinRightY,cmxCpThresBinRightHit);
	    } else {
	      cmxCpThresBinLeftX=bin;
	      cmxCpThresBinLeftY=thr+offset;
	      cmxCpThresBinLeftHit=hit;
	      fill(m_packageName,cmxCpThresBinLeftX,cmxCpThresBinLeftY,cmxCpThresBinLeftHit);
	    }
	  }
	}
      } // hits1

    }

    monCmxCpHits.push_back(monCmxCpHit);
    if (cmx)
      monCmxCpHitsRight.push_back(monCmxCpHit);
    else
      monCmxCpHitsLeft.push_back(monCmxCpHit);

  } //CmxCpHitsCollection iterator
  //  
  auto cmxCpmHitsPeak = Monitored::Collection("cmxCpHitsPeak", monCmxCpHits, []( const auto &hit ){return hit.hit->peak();} ); 
  variables.push_back(cmxCpmHitsPeak);
  auto cmxCpmHitsCrateSlices = Monitored::Collection("cmxCpHitsCrateSlices", monCmxCpHits, []( const auto &hit ){return hit.crateSlices;} ); 
  variables.push_back(cmxCpmHitsCrateSlices);
  auto cmxCpmHitsCrateCmx = Monitored::Collection("cmxCpHitsCrateCmx", monCmxCpHits, []( const auto &hit ){return hit.crateCmx;} ); 
  variables.push_back(cmxCpmHitsCrateCmx);
  // for masks
  auto cmxCpmHitsHits0 = Monitored::Collection("cmxCpHitsHits0", monCmxCpHits, []( const auto &hit ){return hit.hit->hits0();} ); 
  variables.push_back(cmxCpmHitsHits0);
  auto cmxCpmHitsHits1 = Monitored::Collection("cmxCpHitsHits1", monCmxCpHits, []( const auto &hit ){return hit.hit->hits1();} ); 
  variables.push_back(cmxCpmHitsHits1);
  // mask for checksum plus hits0
  auto cmxCpmHits0TopoCheckSum = Monitored::Collection("cmxCpHits0TopoCheckSum", monCmxCpHits, []( const auto &hit ){return (hit.hit->hits0() && hit.srcTopoCheckSum);} ); 
  variables.push_back(cmxCpmHits0TopoCheckSum);

  //  CMX-CP Topo Tobs
  Monitored::Scalar<int> cmxCpTopoTobsCmxLeft = Monitored::Scalar<int>("cmxCpTopoTobsCmxLeft", 0);
  Monitored::Scalar<int> cmxCpTopoTobsCmxRight = Monitored::Scalar<int>("cmxCpTopoTobsCmxRight", 0);
  for (int crate = 0; crate < m_crates; ++crate) {
    for (int cmx = 0; cmx < 2; ++cmx) {
      int val = cmxCount[crate * 2 + cmx];
      if (val) {
        if (val >= m_maxTobsPerCmx) val = m_maxTobsPerCmx - 1;
        if (cmx == 0) {
	  cmxCpTopoTobsCmxLeft=val;
	  fill(m_packageName,cmxCpTopoTobsCmxLeft);
        } else {          
	  cmxCpTopoTobsCmxRight=val;
	  fill(m_packageName,cmxCpTopoTobsCmxRight);
	}
      }
    }
  }
  
  // Update error summary plot
  Monitored::Scalar<int> cpmErrorX = Monitored::Scalar<int>("cpmErrorX", 0);
  Monitored::Scalar<int> cpmErrorY = Monitored::Scalar<int>("cpmErrorY", 0);
  Monitored::Scalar<int> cpmErrorSummary = Monitored::Scalar<int>("cpmErrorSummary", 0);
  std::vector<int> crateErr(4);
  for (int err = 0; err < NumberOfSummaryBins; ++err) {
    int error = 0;
    for (int loc = 0; loc < m_crates * m_modules; ++loc) {
      if ((errorsCPM[loc] >> err) & 0x1) {
	cpmErrorX=loc;
	cpmErrorY=err;
	fill(m_packageName,cpmErrorX,cpmErrorY);
        error = 1;
        crateErr[loc / m_modules] |= (1 << err);
      }
      if (loc < m_crates * 2) {
        if ((errorsCMX[loc] >> err) & 0x1) {
	  cpmErrorX=loc+(m_crates*m_modules);
	  cpmErrorY=err;
	  fill(m_packageName,cpmErrorX,cpmErrorY);
          error = 1;
          crateErr[loc / 2] |= (1 << err);
        }
      }
    }
    if (error) {
      cpmErrorSummary=err;
      fill(m_packageName,cpmErrorSummary);
    }
  } // NSummaryBins

  fill(m_packageName,variables);
  variables.clear();
  return StatusCode::SUCCESS;
}


int* CpmMonitorAlgorithm::getIsolationBits(int val, int nThresh, int nBits, int offset) const
{
  // return array of threshold bits
  // 
  static int nthres[20]={0};
  for (int thr = 0; thr < 20; ++thr) {
    nthres[thr]=0;
  }
  if (val) {
    const int mask = (1 << nBits) - 1;
    for (int thr = 0; thr < nThresh; ++thr) {
      const int hit = (val >> (nBits*thr)) & mask;
      if (hit) {
	nthres[thr+offset]=hit;
      }  
    }
  } else {
    ATH_MSG_WARNING("getIsolationBits: no input word supplied" ); 
  }
  return nthres;

}

StatusCode CpmMonitorAlgorithm::fillCpmTowerVectors(SG::ReadHandle<xAOD::CPMTowerContainer> &cpmTower,
						    std::vector<MonitorCpmTT> &monCpmTTs_em, std::vector<MonitorCpmTT> &monCpmTTs_had,
						    std::vector<int> &errorsCPM,
						    bool core,
						    Monitored::Scalar<int> &cpmLoc,
						    Monitored::Scalar<int> &GLinkParityError
						    ) const
{
  //   
  xAOD::CPMTowerContainer::const_iterator ctIterator    = (*cpmTower).begin();
  xAOD::CPMTowerContainer::const_iterator ctIteratorEnd = (*cpmTower).end();      
  for (; ctIterator != ctIteratorEnd; ++ctIterator) {
    const xAOD::CPMTower* ct = *ctIterator;
    const uint8_t    em  = ct->emEnergy();
    const uint8_t    had = ct->hadEnergy();
    const double eta = ct->eta();
    const double phi = ct->phi();
    const LVL1::Coordinate coord(phi, eta);
    LVL1::CoordToHardware converter;
    const int crate  = (core) ? converter.cpCrate(coord)
      : converter.cpCrateOverlap(coord);
    const int cpm    = (core) ? converter.cpModule(coord)
      : converter.cpModuleOverlap(coord);
    const int loc    = crate * m_modules + cpm - 1;
    const int slices = (ct->emEnergyVec()).size();
    const int slice = crate * m_maxSlices + slices - 1;
    if (crate==999) {
      ATH_MSG_DEBUG("Crate number is 999, "<< crate << " eta " << eta << " phi " << phi <<       
 		    " cpm " << cpm << " slices " << slices <<
		    " max slices " << m_maxSlices << " m_modules " << m_modules <<
		    " slice " << slice);
    }
    GLinkParityError=1; 
    cpmLoc=1;
    fill(m_packageName, GLinkParityError, cpmLoc);


    // Errors    
    bool emParityError=false; 
    bool emLinkDownError=false; 
    bool emGLinkParityError[8]={false};
    uint32_t error = ct->emError(); 
    if (error) {
      const LVL1::DataError emError(error);
      if (emError.get(LVL1::DataError::Parity)) {
	emParityError=true;
        errorsCPM[loc] |= (1 << EMParity);
      }
      if (emError.get(LVL1::DataError::LinkDown)) {
	emLinkDownError=true;
        errorsCPM[loc] |= (1 << EMLink);
      }
      // fix me
      const int status = (error >> LVL1::DataError::GLinkParity) & 0xff; 
      if (status) {
	cpmLoc=loc;
        for (int bit = 0; bit < 8; ++bit) {
          if ((status >> bit) & 0x1) { 
	    GLinkParityError=bit; 
	    fill(m_packageName, GLinkParityError, cpmLoc);
	  }
        }
        errorsCPM[loc] |= (1 << CPMStatus);
      }
    }

    bool hadParityError=false;
    bool hadLinkDownError=false;
    error = ct->hadError();
    if (error) {
      const LVL1::DataError hadError(error);
      if (hadError.get(LVL1::DataError::Parity)) {
	hadParityError=true;
        errorsCPM[loc] |= (1 << HadParity);
      }
      if (hadError.get(LVL1::DataError::LinkDown)) {
	hadLinkDownError=true;
        errorsCPM[loc] |= (1 << HadLink);
      }
    }
    // fill tower vector for plots
    MonitorCpmTT monTT; 
    monTT.ttower=ct;
    monTT.phi_scaled=ct->phi()*m_phiScaleTT;
    monTT.slice=slice;
    monTT.emParityError=emParityError;
    monTT.emLinkDownError=emLinkDownError;
    memcpy(monTT.emGLinkParityError, emGLinkParityError, sizeof(emGLinkParityError));
    monTT.hadParityError=hadParityError;
    monTT.hadLinkDownError=hadLinkDownError;
    if (em) {
      monCpmTTs_em.push_back(monTT);
    }
    if (had) {
      monCpmTTs_had.push_back(monTT);
    }

  } // iterator


  return StatusCode::SUCCESS;
}


