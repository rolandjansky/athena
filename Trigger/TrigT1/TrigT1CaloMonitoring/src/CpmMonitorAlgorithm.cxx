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
  bool core=true;
  ATH_CHECK(fillCpmTowerVectors(cpmTowerTES, monCpmTTs_em, monCpmTTs_had, errorsCPM, core));
  std::vector<MonitorCpmTT> monCpmOverlapTTs_em;
  std::vector<MonitorCpmTT> monCpmOverlapTTs_had;
  core=false;
  ATH_CHECK(fillCpmTowerVectors(cpmTowerOverlapTES, monCpmOverlapTTs_em, monCpmOverlapTTs_had, errorsCPM, core));

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
      // have a go at isolation bits
      int * isolbits=getIsolationBits(isolation, m_isolBits, 1);
      bool isolationBits[4]={false}; //cannot use Gaudi::Property<int> so set size

      bool isolationBitSet=false;
      for (int thr = 0; thr < m_isolBits; ++thr) {
	isolationBits[thr]=isolbits[thr];
	if (isolationBits[thr])
	  isolationBitSet=true;
      }
      //
      MonitorTobRoI monTobRoI;
      monTobRoI.tobroi=(*crIterator);
      monTobRoI.etaMod=etaMod;
      monTobRoI.phiMod=phiMod;
      memcpy(monTobRoI.isolationBits, isolationBits , sizeof(isolationBits));
      monTobRoI.isolationBitSet=isolationBitSet;
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

  std::vector<MonitorCmxCpTob> monCmxCpTobEner;
  std::vector<MonitorCmxCpTob> monCmxCpTobIsol;
  std::vector<MonitorCmxCpTob> monCmxCpTobError;

  tobCount.assign(vecSize, 0);
  std::vector<int> cmxCount(m_crates * 2);
  xAOD::CMXCPTobContainer::const_iterator cmxCpTobIter    = (*cmxCpTobTES).begin();
  xAOD::CMXCPTobContainer::const_iterator cmxCpTobIterEnd = (*cmxCpTobTES).end();
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
      monCmxCpTobEner.push_back(monCmxCpTob);

    }
    if (isolation) {
      MonitorCmxCpTob monCmxCpTob;
      monCmxCpTob.tob=(*cmxCpTobIter);
      monCmxCpTob.x=x;
      monCmxCpTob.y=y;
      monCmxCpTobIsol.push_back(monCmxCpTob);
    }
    if (error) {
      const LVL1::DataError err(error);
      if (err.get(LVL1::DataError::Overflow)) {
	tobCount[x * 2 + cmx] = m_tobsPerCPM + 1;
      }
      const int ybase = cmx * 5;
      bool parity = false;
      if (err.get(LVL1::DataError::ParityMerge)) {
	parity = true;
      }
      if (err.get(LVL1::DataError::ParityPhase0)) {
	parity = true;
      }
      if (err.get(LVL1::DataError::ParityPhase1)) {
	parity = true;
      }
      if (err.get(LVL1::DataError::ParityPhase2)) {
	parity = true;
      }
      if (err.get(LVL1::DataError::ParityPhase3)) {
	parity = true;
      }
      if (parity) errorsCMX[crate * 2 + cmx] |= (1 << TOBParity);

      MonitorCmxCpTob monCmxCpTob;
      monCmxCpTob.tob=(*cmxCpTobIter);
      monCmxCpTob.x=x;
      monCmxCpTob.ybase=ybase;
      monCmxCpTobEner.push_back(monCmxCpTob);
    }
    if (energy || isolation || error) {
      ++tobCount[x * 2 + cmx];
      ++cmxCount[crate * 2 + cmx];
    }
  }
  

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
						    bool core
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

    // Errors    
    bool emParityError=true;
    bool emLinkDownError=true;
    bool emGLinkParityError[8]={false};
    uint32_t error = ct->emError(); // uint8_t
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
      const int status = (error >> LVL1::DataError::GLinkParity) & 0xff;
      if (status) {
        for (int bit = 0; bit < 8; ++bit) {
          if ((status >> bit) & 0x1) emGLinkParityError[bit]=true; 
        }
        errorsCPM[loc] |= (1 << CPMStatus);
      }
    }

    bool hadParityError=true;
    bool hadLinkDownError=true;
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


