/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TileTriggerDefaultCalibTool.h"

#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloTTIdTools.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"

#include "Identifier/HWIdentifier.h"
#include "StoreGate/ReadHandle.h"

#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloIdentifier/TileID.h"

#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileIdentifier/TileHWID.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileConditions/TileCablingService.h"

#include "TFile.h"
#include "TTree.h"
#include "TString.h"

#include <vector>
#include <cmath>


namespace {
template <class T>
inline
T square(T x) { return x*x; }
}


TileTriggerDefaultCalibTool::TileTriggerDefaultCalibTool(const std::string& type, const std::string& name,const IInterface* pParent):
  AthAlgTool(type, name, pParent)
  , m_TT_ID(nullptr)
  , m_tileHWID(nullptr)
  , m_tileID(nullptr)
  , m_tileCablingService(nullptr)
  , m_charge(0)
  , m_ipmt(0)
  , m_ipmtCount(0)
  , m_ipmtOld(0)
  , m_DACvalue(0)
  , m_nEvtGlobal(0)
{
  declareInterface<ITileCalibTool>( this );
  declareProperty("MaxNTriggerTowers", m_maxNTT=7200);
  declareProperty("NtupleID", m_ntupleID="h3000");
  declareProperty("NumEventPerPMT", m_nevpmt=195); // changed from 200 to 195
  
  m_meanTile = new float[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN]();
  m_rmsTile = new float[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN]();
  m_meanTileDAC = new float[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN]();
  m_rmsTileDAC = new float[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN]();
  m_ietaTile = new int[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN]();
  m_iphiTile = new int[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN]();
  m_ipmtTile = new int[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN]();
  m_nEvtTile = new int[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN]();

  m_meanL1Calo = new float[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN]();
  m_rmsL1Calo = new float[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN]();
  m_meanL1CaloDAC = new float[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN]();
  m_rmsL1CaloDAC = new float[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN]();
  m_ietaL1Calo = new int[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN]();
  m_iphiL1Calo = new int[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN]();
  m_ipmtL1Calo = new int[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN]();
  m_nEvtL1Calo = new int[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN]();

  m_meanTileL1Calo = new float[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN]();
  m_rmsTileL1Calo = new float[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN]();
}

TileTriggerDefaultCalibTool::~TileTriggerDefaultCalibTool()
{
  delete[] m_meanTile;
  delete[] m_rmsTile;
  delete[] m_meanTileDAC;
  delete[] m_rmsTileDAC;
  delete[] m_ietaTile;
  delete[] m_iphiTile;
  delete[] m_ipmtTile;
  delete[] m_nEvtTile;
  delete[] m_meanL1Calo;
  delete[] m_rmsL1Calo;
  delete[] m_meanL1CaloDAC;
  delete[] m_rmsL1CaloDAC;
  delete[] m_ietaL1Calo;
  delete[] m_iphiL1Calo;
  delete[] m_ipmtL1Calo;
  delete[] m_nEvtL1Calo;
  delete[] m_meanTileL1Calo;
  delete[] m_rmsTileL1Calo;
}      

StatusCode TileTriggerDefaultCalibTool::initialize()
{
  ATH_MSG_INFO ( "initialize()" );

  // Initialize arrays for results
  m_charge = 0;
  m_ipmt = 0;
  m_ipmtOld = 0;
  m_ipmtCount = 0;

  ATH_CHECK( m_rawChannelContainerKey.initialize() );
  ATH_CHECK( m_triggerTowerContainerKey.initialize() );

  ATH_CHECK( m_l1CaloTTIdTools.retrieve() );
  ATH_MSG_DEBUG("L1CaloTTIdTools retrieved");

  ATH_CHECK( detStore()->retrieve(m_TT_ID) );
  ATH_CHECK( detStore()->retrieve(m_tileHWID) );
  ATH_CHECK( detStore()->retrieve(m_tileID) );

  m_tileCablingService = TileCablingService::getInstance();

  //=== get TileCondToolEmscale
  CHECK( m_tileToolEmscale.retrieve() );
  
  CHECK( m_dqStatusKey.initialize() );

  return StatusCode::SUCCESS;  
}

StatusCode TileTriggerDefaultCalibTool::initNtuple(int runNumber, int runType, TFile * rootFile )
{
  ATH_MSG_INFO ( "initialize(" << runNumber << "," << runType << "," << rootFile << ")" );
  return StatusCode::SUCCESS;  
}

StatusCode TileTriggerDefaultCalibTool::execute()
{
  ATH_MSG_DEBUG ( "executeTrigger()" );

  const EventContext& ctx = Gaudi::Hive::currentContext();
  const TileDQstatus* dqStatus = SG::makeHandle (m_dqStatusKey, ctx).get();

  // Get TileRawChannelContainer
  SG::ReadHandle<TileRawChannelContainer> container(m_rawChannelContainerKey);
         ATH_CHECK( container.isValid() );

  ATH_MSG_DEBUG ( "second executeTrigger()" );
  // declare an array of pmt id for the trigger tower loop
  HWIdentifier chanIds[2][16][64][6];
  memset(chanIds, 0, sizeof(chanIds));

  // Get event's ext CIS parameters
  const uint32_t *cispar = dqStatus->cispar();

  // Mapping pmt2chan
  int chan_bar[54] = {4 ,1 ,2 ,3 ,0 ,-1 ,
		      8 ,5 ,6 ,7 ,13,-1 ,
		      10,9 ,12,11,14,-1 ,
		      18,15,16,17,25,-1 ,
		      20,19,22,21,24,-1 ,
		      26,23,28,27,41,-1 ,
		      32,29,34,33,44,-1 ,
		      38,35,40,39,-1,-1 ,
		      36,37,42,47,46,45 };

  int chan_ext[36] = {4, 5 ,2 ,3 ,16,-1 ,
		      8, 9 ,17,-1,-1,-1 ,
		      6, 7 ,14,15,38,-1 ,
		      10,11,22,23,37,-1 ,
		      20,21,30,35,-1,-1 ,
		      31,32,39,36,41,40 };

  ATH_MSG_DEBUG ( "cispar[16] " << cispar[16] << ", cispas[17] " << cispar[17] << ", cispar[18] " << cispar[18] );
  if (cispar[16] == 0x07){
      if (cispar[18]>5) {
          ATH_MSG_WARNING ( "Bad CISpar detected, using pmt index and charge from previous event: " << m_ipmt << " " << m_charge );
      } else {
          m_charge = cispar[17];
          m_ipmt = cispar[18];
          //m_itower = cispar[19];
          //m_idrawer = cispar[20];
      }
  }
  else if (cispar[16] == 0x107){ // bad CISpar
      ATH_MSG_WARNING ( "Bad CISpar detected, using pmt index and charge from previous event: " << m_ipmt << " " << m_charge );
  }
  else
      return StatusCode::SUCCESS;
  
  ATH_MSG_DEBUG ( "Pattern/ipmt/charge: " << cispar[16] << " " << m_ipmt << " " << m_charge );
  if (m_charge<1.0e-20) {
      ATH_MSG_WARNING ( "Bad charge " << m_charge << " - skipping event" );
      return StatusCode::SUCCESS;
  }
  
  if (m_ipmtOld != m_ipmt){
	if (m_ipmt < m_ipmtOld) return StatusCode::SUCCESS;
	m_ipmtCount = 1;
	m_ipmtOld = m_ipmt;
  }
  else
	m_ipmtCount += 1;
		   
  if (m_ipmtCount > m_nevpmt) return StatusCode::SUCCESS;  //takes into account only the first m_nevpmt events for each pmt to avoid problem with wrong CISpar

  m_DACvalue = m_charge;
  
  // Create iterator over RawChannelContainer for Tile
  TileRawChannelContainer::const_iterator itColl = (*container).begin();
  TileRawChannelContainer::const_iterator itCollEnd = (*container).end();
  TileRawChannelCollection::const_iterator it, itEnd;
  TileRawChannelUnit::UNIT RChUnit = container->get_unit();

  //     Go through all TileRawChannelCollections
  for(; itColl != itCollEnd; ++itColl) {
    
    //go through all TileRawChannels in collection (loop over all channels)
    it = (*itColl)->begin();
    itEnd = (*itColl)->end();
       
    for(; it != itEnd; ++it) {
  
      // get hardware id to identify adc channel
      HWIdentifier hwid=(*it)->adc_HWID();
      //ATH_MSG_DEBUG("HWID1: "<< m_tileHWID->to_string(hwid));
      int ros    = m_tileHWID->ros(hwid);    // LBA=1  LBC=2  EBA=3  EBC=4 
      int drawer = m_tileHWID->drawer(hwid); // 0 to 63
      int chan = m_tileHWID->channel(hwid);  // 0 to 47 channel not PMT
      int gain = m_tileHWID->adc(hwid); // low=0 high=1
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);

      // Get the PMT Id, create on the fly a mapping to the trigger tower id
      // this mapping is to be used below in the loop over all trigger towers
      Identifier tt_id = (*it)->tt_ID();
      if (!tt_id.is_valid()) continue;
      //Identifier pmt_id = (*it)->pmt_ID();

      int pos_neg_z = m_TT_ID->pos_neg_z(tt_id);
      int ieta = m_TT_ID->eta(tt_id);
      int iphi = m_TT_ID->phi(tt_id);

      if (ros < 3) {
        if (ieta < 0 || ieta > 8) continue;
      } else {
        if (ieta < 9 || ieta > 14) continue;
      }

      if((ros==1) || (ros==2)){
        // (Manually fold constants to avoid cppcheck warning.)
	if (chan != chan_bar[(ieta*6)/*-1*/ + m_ipmt/*+1*/]) continue;   	// check if the chan is firing
      }
      else if ((ros==3) || (ros==4)){
	if ((chan != chan_ext[((ieta-9)*6)/*-1*/ + m_ipmt/*+1*/]) || (chan == 0)) continue;	// check if the chan is firing
      }
      else
	continue;

      float amp = (*it)->amplitude();
      float amp_pC = m_tileToolEmscale->channelCalib(drawerIdx, chan, gain, amp, RChUnit, TileRawChannelUnit::PicoCoulombs);
      //float amp_GeV = m_tileToolEmscale->channelCalib(drawerIdx, chan, gain, amp, RChUnit, TileRawChannelUnit::MegaElectronVolts) * 0.001;
      //float pC2GeV = m_tileToolEmscale->channelCalib(drawerIdx, chan, 0, 1.0, TileRawChannelUnit::PicoCoulombs, TileRawChannelUnit::MegaElectronVolts) * 0.001;
      //log << MSG::DEBUG << "ros " << ros << ", pos_neg_z " << pos_neg_z << ", drawer " << drawer <<" ieta " << ieta <<  ", chan " << chan << ", amplitude " << amp << endreq;
      
      m_meanTile[ros][drawer][chan] += amp_pC;
      m_rmsTile[ros][drawer][chan]  += square(amp_pC);
      m_meanTileDAC[ros][drawer][chan] += (amp_pC-m_charge);
      m_rmsTileDAC[ros][drawer][chan]  += square(amp_pC-m_charge);
      m_ietaTile[ros][drawer][chan]  = ieta;
      m_iphiTile[ros][drawer][chan]  = iphi;
      m_ipmtTile[ros][drawer][chan]  = m_ipmt;
      ++m_nEvtTile[ros][drawer][chan];

      if (pos_neg_z < 0) pos_neg_z = 0;
      chanIds[pos_neg_z][ieta][iphi][m_ipmt] = hwid;
    }

  } // end of loop over raw channels for Tile
  
  // loop over all L1Calo trigger channels, calculate the average and RMS
  SG::ReadHandle<xAOD::TriggerTowerContainer> triggerTowers(m_triggerTowerContainerKey);
  ATH_CHECK( triggerTowers.isValid() );

  int ntt = 0;
 
  for (const xAOD::TriggerTower* triggerTower : *triggerTowers) {

    if(ntt>m_maxNTT) break;

    if (!triggerTower->sampling()) continue; // is not hadronic

    double eta = triggerTower->eta();
    double phi = triggerTower->phi();
    double tt_ene = 0.;  //triggerTower->e();

    // temporary, compute id out of the tower position
    int pos_neg_z = m_l1CaloTTIdTools->pos_neg_z(eta);
    int region = m_l1CaloTTIdTools->regionIndex(eta);
    int ieta   = m_l1CaloTTIdTools->etaIndex(eta);
    int iphi   = m_l1CaloTTIdTools->phiIndex(eta, phi);
    Identifier tt_id = m_TT_ID->tower_id(pos_neg_z, 1, region, ieta, iphi);
    //Not used, but could be useful, eg. coolId is used to get proper pedestal value
    //Identifier id = m_ttTool->identifier(triggerTower->eta(), triggerTower->phi(), 1); //alternative way to get tt_id
    //unsigned int coolId = m_ttTool->channelID(tt_id).id();
    
    // check if it is a Tile tower
    if (tt_id.is_valid() && m_TT_ID->is_tile(tt_id))	
    	++ntt;
    else continue;

    //compute L1Calo energy from ADC:
    // - take max ADC value
    // - subtract pedestal (about 32 ADC counts), apply 0.25 conversion factor to GeV 
    const std::vector<uint16_t>& adc = triggerTower->adc();
    uint16_t max = *(std::max_element(adc.begin(), adc.end()));
    uint16_t ped = adc.front(); 
    tt_ene = (max - ped) * 0.25; 
    if (tt_ene < 0.) tt_ene = 0.;

    // check boundaries
    if ((ieta < 0) || (ieta > 14)) continue;

    if (pos_neg_z < 0) pos_neg_z = 0;
    HWIdentifier hwid = chanIds[pos_neg_z][ieta][iphi][m_ipmt];
    int ros    = m_tileHWID->ros(hwid);    // LBA=1  LBC=2  EBA=3  EBC=4 
    if(ros >= 5 || ros <=0){continue;} // ignoring special channels, like those connected to inner MBTS in EBA/EBC 39-42,55-58 
    int drawer = m_tileHWID->drawer(hwid); // 0 to 63
    int chan = m_tileHWID->channel(hwid);  // 0 to 47 channel not PMT
    unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
    float pC2GeV = m_tileToolEmscale->channelCalib(drawerIdx, chan, 0, 1.0, TileRawChannelUnit::PicoCoulombs, TileRawChannelUnit::MegaElectronVolts) * 0.001;
    double tt_ene_pC = tt_ene / pC2GeV;

    //comparing results from triggerTower->e() and energy computed from ADC (default method)
    if (triggerTower->e() == 0) 
      ATH_MSG_DEBUG ( "Tower partition " << ros 
                      << ", drawer "<< drawer 
                      << ", chan: " << chan 
                      << ", eta "<< eta 
                      << ", phi: " << phi 
                      << ", amplitude : "  << tt_ene 
                      << ", old " << triggerTower->e() 
                      << ", ratio old/new " << triggerTower->e() / tt_ene );

    if ((ros==1) || (ros==2)){ 
      if (chan != chan_bar[(ieta*6)-1 + m_ipmt+1]) continue;   		// check if the chan is firing    
    }
    else if ((ros==3) || (ros==4)) {
      if ((chan != chan_ext[((ieta-9)*6)-1 + m_ipmt+1]) || (chan == 0)) continue;	// check if the chan is firing
    }
    else continue;

    m_meanL1Calo[ros][drawer][chan] += tt_ene_pC;
    m_rmsL1Calo[ros][drawer][chan]  += square(tt_ene_pC);
    m_meanL1CaloDAC[ros][drawer][chan] += tt_ene_pC - m_charge;
    m_rmsL1CaloDAC[ros][drawer][chan]  += square(tt_ene_pC - m_charge);
    m_ietaL1Calo[ros][drawer][chan]  = ieta;
    m_iphiL1Calo[ros][drawer][chan]  = iphi;
    m_ipmtL1Calo[ros][drawer][chan]  = m_ipmt;
    ++m_nEvtL1Calo[ros][drawer][chan];
    
  }// end of trigger tower loop for L1Calo

  ++m_nEvtGlobal; // Number of MaxEvt from Athena

  for (int ros=0;ros<5;ros++) {
    for (int drawer=0;drawer<64;drawer++) {
      for (int chan=0;chan<48;chan++) {
        // ???
	m_rmsTileL1Calo[ros][drawer][chan] += m_meanTile[ros][drawer][chan]*m_meanL1Calo[ros][drawer][chan];
      }
    }
  }

  return StatusCode::SUCCESS;

}

StatusCode TileTriggerDefaultCalibTool::finalizeCalculations()
{
  ATH_MSG_INFO ( "finalizeCalculations()" );
  
  for (int ros=0;ros<5;ros++) {
    for (int drawer=0;drawer<64;drawer++) {
      for (int chan=0;chan<48;chan++) {
	if (m_nEvtTile[ros][drawer][chan] > 0){
	  m_meanTile[ros][drawer][chan] /= m_nEvtTile[ros][drawer][chan];
	  m_rmsTile[ros][drawer][chan]  /= m_nEvtTile[ros][drawer][chan];
	  m_meanTileDAC[ros][drawer][chan] /= m_nEvtTile[ros][drawer][chan];
	  m_rmsTileDAC[ros][drawer][chan]  /= m_nEvtTile[ros][drawer][chan];
	}
	
	if(m_rmsTile[ros][drawer][chan] <= square(m_meanTile[ros][drawer][chan]))
	  m_rmsTile[ros][drawer][chan] = 0;
	else 
	  m_rmsTile[ros][drawer][chan] = sqrt(m_rmsTile[ros][drawer][chan] - square(m_meanTile[ros][drawer][chan]));
	
	if(m_rmsTileDAC[ros][drawer][chan] <= square(m_meanTileDAC[ros][drawer][chan]))
	  m_rmsTileDAC[ros][drawer][chan] = 0;
	else 
	  m_rmsTileDAC[ros][drawer][chan] = sqrt(m_rmsTileDAC[ros][drawer][chan] - square(m_meanTileDAC[ros][drawer][chan]));

	if (m_nEvtL1Calo[ros][drawer][chan] > 0){ 
	  m_meanL1Calo[ros][drawer][chan] /= m_nEvtL1Calo[ros][drawer][chan];
	  m_rmsL1Calo[ros][drawer][chan]  /= m_nEvtL1Calo[ros][drawer][chan];
 	  m_meanL1CaloDAC[ros][drawer][chan] /= m_nEvtL1Calo[ros][drawer][chan];
	  m_rmsL1CaloDAC[ros][drawer][chan]  /= m_nEvtL1Calo[ros][drawer][chan];
	}
	
	if(m_rmsL1Calo[ros][drawer][chan] <= square(m_meanL1Calo[ros][drawer][chan]))
	  m_rmsL1Calo[ros][drawer][chan] = 0;
	else 
	  m_rmsL1Calo[ros][drawer][chan] = sqrt(m_rmsL1Calo[ros][drawer][chan] - square(m_meanL1Calo[ros][drawer][chan]));

	if(m_rmsL1CaloDAC[ros][drawer][chan] <= square(m_meanL1CaloDAC[ros][drawer][chan]))
	  m_rmsL1CaloDAC[ros][drawer][chan] = 0;
	else 
	  m_rmsL1CaloDAC[ros][drawer][chan] = sqrt(m_rmsL1CaloDAC[ros][drawer][chan] - square(m_meanL1CaloDAC[ros][drawer][chan]));

	m_meanTileL1Calo[ros][drawer][chan] = m_meanTile[ros][drawer][chan] - m_meanL1Calo[ros][drawer][chan];
        if(m_rmsTileL1Calo[ros][drawer][chan] <= square(m_meanTileL1Calo[ros][drawer][chan]))
	  m_rmsTileL1Calo[ros][drawer][chan] = 0;
	else 
	  m_rmsTileL1Calo[ros][drawer][chan] = sqrt(m_rmsTileL1Calo[ros][drawer][chan] - square(m_meanTileL1Calo[ros][drawer][chan]));
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TileTriggerDefaultCalibTool::writeNtuple(int runNumber, int runType, TFile * rootFile)
{
  ATH_MSG_INFO ( "writeNtuple(" << runNumber << "," << runType << "," << rootFile << ")" );

  TTree *t = new TTree(m_ntupleID.c_str(), "TileCalib-Ntuple");
  TString ind = TString::Format("[%d][%d][%d]",Tile::MAX_ROS,Tile::MAX_DRAWER,Tile::MAX_CHAN);

  t->Branch("meanTile",m_meanTile,ind.Format("meanTile%s/F",(const char *)ind));
  t->Branch("rmsTile",m_rmsTile,ind.Format("rmsTile%s/F",(const char *)ind));
  t->Branch("meanTileDAC",m_meanTileDAC,ind.Format("meanTileDAC%s/F",(const char *)ind));
  t->Branch("rmsTileDAC",m_rmsTileDAC,ind.Format("rmsTileDAC%s/F",(const char *)ind));
  t->Branch("ietaTile",m_ietaTile,ind.Format("ietaTile%s/I",(const char *)ind));
  t->Branch("iphiTile",m_iphiTile,ind.Format("iphiTile%s/I",(const char *)ind));
  t->Branch("ipmtTile",m_ipmtTile,ind.Format("ipmtTile%s/I",(const char *)ind));
  t->Branch("nEvtTile",m_nEvtTile,ind.Format("nEvtTile%s/I",(const char *)ind));
  t->Branch("meanL1Calo",m_meanL1Calo,ind.Format("meanL1Calo%s/F",(const char *)ind));
  t->Branch("rmsL1Calo",m_rmsL1Calo,ind.Format("rmsL1Calo%s/F",(const char *)ind));
  t->Branch("meanL1CaloDAC",m_meanL1CaloDAC,ind.Format("meanL1CaloDAC%s/F",(const char *)ind));
  t->Branch("rmsL1CaloDAC",m_rmsL1CaloDAC,ind.Format("rmsL1CaloDAC%s/F",(const char *)ind));
  t->Branch("ietaL1Calo",m_ietaL1Calo,ind.Format("ietaL1Calo%s/I",(const char *)ind));
  t->Branch("iphiL1Calo",m_iphiL1Calo,ind.Format("iphiL1Calo%s/I",(const char *)ind));
  t->Branch("ipmtL1Calo",m_ipmtL1Calo,ind.Format("ipmtL1Calo%s/I",(const char *)ind));
  t->Branch("nEvtL1Calo",m_nEvtL1Calo,ind.Format("nEvtL1Calo%s/I",(const char *)ind));
  t->Branch("meanTileL1Calo",m_meanTileL1Calo,ind.Format("meanTileL1Calo%s/F",(const char *)ind));
  t->Branch("rmsTileL1Calo",m_rmsTileL1Calo,ind.Format("rmsTileL1Calo%s/F",(const char *)ind));
  t->Branch("nEvtGlobal",&m_nEvtGlobal,"nEvtGlobal/I");
  t->Branch("DACvalue",&m_DACvalue,"DACvalue/F");

  // Fill with current values (i.e. tree will have only one entry for this whole run)
  t->Fill();
  t->Write();

  return StatusCode::SUCCESS;
}

StatusCode TileTriggerDefaultCalibTool::finalize()
{
  ATH_MSG_DEBUG ( "finalizeTrigger()" );
  return StatusCode::SUCCESS;  
}
