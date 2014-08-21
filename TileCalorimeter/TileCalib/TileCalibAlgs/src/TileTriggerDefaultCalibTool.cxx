/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TileCalibAlgs/TileTriggerDefaultCalibTool.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"

#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloTTIdTools.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"

#include "Identifier/HWIdentifier.h"

#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloIdentifier/TileID.h"

#include "TileIdentifier/TileHWID.h"
#include "TileRecUtils/TileBeamInfoProvider.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileBeamElemContainer.h"
#include "TileConditions/TileCablingService.h"
//for the extended CISpar
#include "TileIdentifier/TileTBFrag.h"

#include "TFile.h"
#include "TTree.h"
#include "TObjString.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include <cmath>

#include <iostream>


TileTriggerDefaultCalibTool::TileTriggerDefaultCalibTool(const std::string& type, const std::string& name,const IInterface* pParent):
  AlgTool(type, name, pParent)
  , m_TileTriggerContainerID("")
  , m_storeGate("StoreGateSvc",name)
  , m_l1CaloTTIdTools("LVL1::L1CaloTTIdTools/L1CaloTTIdTools")
  //, m_ttTool("LVL1::L1TriggerTowerTool/LVL1::L1TriggerTowerTool")
{
  declareInterface<ITileCalibTool>( this );
  declareProperty("TriggerTowerLocation", m_triggerTowerLocation="TriggerTowers");
  declareProperty("MaxNTriggerTowers", m_maxNTT=7200);
  declareProperty("rawChannelContainer", m_rawChannelContainerName="TileRawChannelFit");
  declareProperty("NtupleID", m_ntupleID="h3000");
  declareProperty("NumEventPerPMT", m_nevpmt=200);
  declareProperty("EventStore",m_storeGate,"StoreGate Service"); // for extended CISpar
  declareProperty("TileBeamElemContainer",m_TileBeamContainerID);
  //  declareProperty("L1TriggerTowerTool", m_ttTool);
}

TileTriggerDefaultCalibTool::~TileTriggerDefaultCalibTool()
{}      

StatusCode TileTriggerDefaultCalibTool::initialize()
{
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "initialize()" << endreq;

  // Initialize arrays for results
  memset(meanTile, 0, sizeof(meanTile));
  memset(rmsTile, 0, sizeof(rmsTile));
  memset(meanTileDAC, 0, sizeof(meanTileDAC));
  memset(rmsTileDAC, 0, sizeof(rmsTileDAC));
  memset(ietaTile, 0, sizeof(ietaTile));
  memset(iphiTile, 0, sizeof(iphiTile));
  memset(ipmtTile, 0, sizeof(ipmtTile));
  memset(nEvtTile, 0, sizeof(nEvtTile));
  memset(meanL1Calo, 0, sizeof(meanL1Calo));
  memset(rmsL1Calo, 0, sizeof(rmsL1Calo));
  memset(meanL1CaloDAC, 0, sizeof(meanL1CaloDAC));
  memset(rmsL1CaloDAC, 0, sizeof(rmsL1CaloDAC));
  memset(ietaL1Calo, 0, sizeof(ietaL1Calo));
  memset(iphiL1Calo, 0, sizeof(iphiL1Calo));
  memset(ipmtL1Calo, 0, sizeof(ipmtL1Calo));
  memset(nEvtL1Calo, 0, sizeof(nEvtL1Calo));
  memset(nEvtGlobal, 0, sizeof(nEvtGlobal));
  memset(meanTileL1Calo, 0, sizeof(meanTileL1Calo));
  memset(rmsTileL1Calo, 0, sizeof(rmsTileL1Calo));
  charge = 0;
  ipmt = 0;
  itower = 0;
  idrawer = 0;
  ipmtOld = 0;
  ipmtCount = 0;
  DACvalue = 0;

  // Find necessary services
  if(service("StoreGateSvc", m_evtStore).isFailure()){
    log<<MSG::ERROR<<"Unable to get pointer to eventStore Service"<<endreq;
    return StatusCode::FAILURE;
  }

  if(service("DetectorStore", m_detStore).isFailure()){
    log << MSG::ERROR
        << "Unable to get pointer to DetectorStore Service" << endreq;
    return StatusCode::FAILURE;
  }

  IToolSvc* toolSvc;
  if (service("ToolSvc",toolSvc).isFailure()) {
    log << MSG::ERROR <<" Can't get ToolSvc " << endreq;
    return StatusCode::FAILURE;
  }

  StatusCode sc = m_l1CaloTTIdTools.retrieve();
  log<<MSG::DEBUG<<"L1CaloTTIdTools retrieved"<<endreq;
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::WARNING << " Cannot get L1CaloTTIdTools !" << endreq;
    return StatusCode::FAILURE;
  }

  // get beam info tool
  if(toolSvc->retrieveTool("TileBeamInfoProvider",m_beamPrv).isFailure()) {
    log << MSG::ERROR << "Unable to get tool 'TileBeamInfoProvider'" << endreq;
    return StatusCode::FAILURE;
  }

  if( m_beamPrv->setProperty("TileRawChannelContainer","TileRawChannelCnt").isFailure())
    {
      log << MSG::ERROR <<" Can't set property 'TileRawChannelContainer' for TileBeamInfoProvider " << endreq;
      return StatusCode::FAILURE;
    }

  // retrieve CaloLVL1_ID from det store
  sc = m_detStore->retrieve(m_TT_ID);
  if (sc.isFailure()) {
    log << MSG::ERROR
	<< "Unable to retrieve CaloLVL1_ID helper from DetectorStore" << endreq;
    return sc;
  }

  // get TileHWID helper
  if(m_detStore->retrieve(m_tileHWID).isFailure()){
    log << MSG::ERROR << "Unable to retrieve TileHWID helper from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }

  // get TileID helper
  if (m_detStore->retrieve(m_tileID).isFailure()) {
    log << MSG::ERROR
	<< "Unable to retrieve TileID helper from DetectorStore" << endreq;
    return sc;
  }

  // get TileCabling Service
  m_tileCablingService = TileCablingService::getInstance();

  return StatusCode::SUCCESS;  
}

StatusCode TileTriggerDefaultCalibTool::initNtuple(int runNumber, int runType, TFile * rootFile )
{
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "initialize(" << runNumber << "," << runType << "," << rootFile << ")" << endreq;
  return StatusCode::SUCCESS;  
}

StatusCode TileTriggerDefaultCalibTool::execute()
{
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "executeTrigger()" << endreq;

  // Get TileRawChannelContainer
  const TileRawChannelContainer *container;
  if(m_evtStore->retrieve(container, m_rawChannelContainerName).isFailure()){
    log<<MSG::ERROR<<"Error getting TileRawChannelCnt '"<<m_rawChannelContainerName<<endreq;
    return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << "second executeTrigger()" << endreq;
  // declare an array of pmt id for the trigger tower loop
  HWIdentifier chanIds[2][16][64][6];
  memset(chanIds, 0, sizeof(chanIds));

  // Get event's ext CIS parameters
  const uint32_t *cispar = m_beamPrv->cispar();

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

  log << MSG::DEBUG << "cispar[16] " << cispar[16] << ", cispas[17] " << cispar[17] << ", cispar[18] " << cispar[18] << endreq;
  if (cispar[16] == 0x07){
  	charge = cispar[17];
  	ipmt = cispar[18];
  	itower = cispar[19];
	idrawer = cispar[20];
  }
  else
     return StatusCode::SUCCESS;
  
  log << MSG::DEBUG << "Pattern/ipmt/charge: " << cispar[16] << " " << ipmt << " " << charge << endreq;	
  if (ipmtOld != ipmt){
	if (ipmt < ipmtOld) return StatusCode::SUCCESS;
	ipmtCount = 1;
	ipmtOld = ipmt;
  }
  else
	ipmtCount += 1;
		   
  if (ipmtCount > 200) return StatusCode::SUCCESS;  //takes into account only the first 200 events for each pmt

  DACvalue = charge;
  
  // Create iterator over RawChannelContainer for Tile
  TileRawChannelContainer::const_iterator itColl = (*container).begin();
  TileRawChannelContainer::const_iterator itCollEnd = (*container).end();
  TileRawChannelCollection::const_iterator it, itEnd;

  //     Go through all TileRawChannelCollections
  for(; itColl != itCollEnd; ++itColl) {
    
    //go through all TileRawChannels in collection (loop over all channels)
    it = (*itColl)->begin();
    itEnd = (*itColl)->end();
       
    for(; it != itEnd; ++it) {
  
      // get hardware id to identify adc channel
      HWIdentifier hwid=(*it)->adc_HWID();
      int ros    = m_tileHWID->ros(hwid);    // LBA=1  LBC=2  EBA=3  EBC=4 
      int drawer = m_tileHWID->drawer(hwid); // 0 to 63
      int chan = m_tileHWID->channel(hwid);  // 0 to 47 channel not PMT
      //   int gain = m_tileHWID->adc(hwid); // low=0 high=1

      // Get the PMT Id, create on the fly a mapping to the trigger tower id
      // this mapping is to be used below in the loop over all trigger towers
      Identifier tt_id = (*it)->tt_ID();
      //Identifier pmt_id = (*it)->pmt_ID();

      int pos_neg_z = m_TT_ID->pos_neg_z(tt_id);
      int ieta = m_TT_ID->eta(tt_id);
      int iphi = m_TT_ID->phi(tt_id);

      if((ieta < 0) || (ieta > 14)) continue;

      if((ros==1) || (ros==2)){ 
	if (chan != chan_bar[(ieta*6)-1 + ipmt+1]) continue;   	// check if the chan is firing
      }
      else if ((ros==3) || (ros==4)){
	if ((chan != chan_ext[((ieta-9)*6)-1 + ipmt+1]) || (chan == 0)) continue;	// check if the chan is firing
      }
      else
	continue;

      float amp = (*it)->amplitude();
      //log << MSG::DEBUG << "ros " << ros << ", pos_neg_z " << pos_neg_z << ", drawer " << drawer <<" ieta " << ieta <<  ", chan " << chan << ", aplitude " << amp << endreq;

      meanTile[ros][drawer][chan] += amp;
      rmsTile[ros][drawer][chan]  += amp*amp;
      meanTileDAC[ros][drawer][chan] += amp-(charge);
      rmsTileDAC[ros][drawer][chan]  += (amp-(charge))*(amp-(charge));
      ietaTile[ros][drawer][chan]  = ieta;
      iphiTile[ros][drawer][chan]  = iphi;
      ipmtTile[ros][drawer][chan]  = ipmt;
      ++nEvtTile[ros][drawer][chan];

      if (pos_neg_z < 0) pos_neg_z = 0;
      chanIds[pos_neg_z][ieta][iphi][ipmt] = hwid;
    }

  } // end of loop over raw channels for Tile
  
  // loop over all L1Calo trigger channels, calculate the average and RMS
  const TriggerTowerCollection* ttCollection = 0;
  if (m_evtStore->retrieve(ttCollection, m_triggerTowerLocation).isFailure()) {
    log << MSG::WARNING << "No TriggerTowerCollectiton found with key: "<< m_triggerTowerLocation << endreq;
    return StatusCode::FAILURE;
  } else log << MSG::DEBUG <<"TriggerTowerCollection retrieved from StoreGate with size: "<< ttCollection->size()<<endreq;

  int ntt = 0;
 
  TriggerTowerCollection::const_iterator pos = ttCollection->begin();
  TriggerTowerCollection::const_iterator pos_end = ttCollection->end();

  for(;pos!=pos_end;++pos) {
  
    if(ntt>m_maxNTT) break;

    const LVL1::TriggerTower* ptt = (*pos);
    double eta = ptt->eta();
    double phi = ptt->phi();
    double tt_ene = 0.;  //ptt->hadEnergy();

    // temporary, compute id out of the tower position
    int pos_neg_z = m_l1CaloTTIdTools->pos_neg_z(eta);
    int region = m_l1CaloTTIdTools->regionIndex(eta);
    int ieta   = m_l1CaloTTIdTools->etaIndex(eta);
    int iphi   = m_l1CaloTTIdTools->phiIndex(eta, phi);
    Identifier tt_id = m_TT_ID->tower_id(pos_neg_z, 1, region, ieta, iphi);
    //Not used, but could be useful, eg. coolId is used to get proper pedestal value
    //Identifier id = m_ttTool->identifier(ptt->eta(), ptt->phi(), 1); //alternative way to get tt_id
    //unsigned int coolId = m_ttTool->channelID(tt_id).id();
    
    // check if it is a Tile tower
    if (m_TT_ID->is_tile(tt_id))	
    	++ntt;
    else continue;

    //compute L1Calo energy from ADC:
    // - take max ADC value
    // - subtract pedestal (about 32 ADC counts), apply 0.25 conversion factor to GeV 
    const std::vector <int>& adc = ptt->hadADC();
    std::vector<int>::const_iterator max = std::max_element(adc.begin(), adc.end()); 
    tt_ene = (*max-32)*0.25; 
    if (tt_ene < 0.) tt_ene = 0.;

    // check boundaries
    if ((ieta < 0) || (ieta > 14)) continue;

    if (pos_neg_z < 0) pos_neg_z = 0;
    HWIdentifier hwid = chanIds[pos_neg_z][ieta][iphi][ipmt];
    int ros    = m_tileHWID->ros(hwid);    // LBA=1  LBC=2  EBA=3  EBC=4 
    int drawer = m_tileHWID->drawer(hwid); // 0 to 63
    int chan = m_tileHWID->channel(hwid);  // 0 to 47 channel not PMT

    //comparing results from ptt->hadEnergy() and energy computed from ADC (default method)
    if (ptt->hadEnergy()==0) log << MSG::DEBUG << "Tower partition "<< ros << ", drawer "<< drawer << ", chan: " << chan << ", eta "<< eta << ", phi: " << phi << ", amplitude : "  << tt_ene << ", old " << ptt->hadEnergy() << ", ratio old/new " << ptt->hadEnergy()/tt_ene << endreq;

    if ((ros==1) || (ros==2)){ 
      if (chan != chan_bar[(ieta*6)-1 + ipmt+1]) continue;   		// check if the chan is firing    
    }
    else if ((ros==3) || (ros==4)) {
      if ((chan != chan_ext[((ieta-9)*6)-1 + ipmt+1]) || (chan == 0)) continue;	// check if the chan is firing
    }
    else continue;

    meanL1Calo[ros][drawer][chan] += tt_ene;
    rmsL1Calo[ros][drawer][chan]  += tt_ene*tt_ene;
    meanL1CaloDAC[ros][drawer][chan] += tt_ene-(charge);
    rmsL1CaloDAC[ros][drawer][chan]  += (tt_ene-(charge))*(tt_ene-(charge));
    ietaL1Calo[ros][drawer][chan]  = ieta;
    iphiL1Calo[ros][drawer][chan]  = iphi;
    ipmtL1Calo[ros][drawer][chan]  = ipmt;
    ++nEvtL1Calo[ros][drawer][chan];
    
  }// end of trigger tower loop for L1Calo

  ++nEvtGlobal[0]; // Number of MaxEvt from Athena

  for (int ros=0;ros<5;ros++) {
    for (int drawer=0;drawer<64;drawer++) {
      for (int chan=0;chan<48;chan++) {
	rmsTileL1Calo[ros][drawer][chan] += meanTile[ros][drawer][chan]*meanL1Calo[ros][drawer][chan];
      }
    }
  }

  return StatusCode::SUCCESS;

}

StatusCode TileTriggerDefaultCalibTool::finalizeCalculations()
{
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "finalizeCalculations()" << endreq;
  
  for (int ros=0;ros<5;ros++) {
    for (int drawer=0;drawer<64;drawer++) {
      for (int chan=0;chan<48;chan++) {
	if (nEvtTile[ros][drawer][chan] > 0){
	  meanTile[ros][drawer][chan] = (meanTile[ros][drawer][chan])/(nEvtTile[ros][drawer][chan]);
	  rmsTile[ros][drawer][chan]  = (rmsTile[ros][drawer][chan])/(nEvtTile[ros][drawer][chan]);
	  meanTileDAC[ros][drawer][chan] = (meanTileDAC[ros][drawer][chan])/(nEvtTile[ros][drawer][chan]);
	  rmsTileDAC[ros][drawer][chan]  = (rmsTileDAC[ros][drawer][chan])/(nEvtTile[ros][drawer][chan]);
	}
	
	if(rmsTile[ros][drawer][chan] <= (meanTile[ros][drawer][chan]*meanTile[ros][drawer][chan]))
	  rmsTile[ros][drawer][chan] = 0;
	else 
	  rmsTile[ros][drawer][chan] = sqrt(rmsTile[ros][drawer][chan] - (meanTile[ros][drawer][chan]*meanTile[ros][drawer][chan]));
	
	if(rmsTileDAC[ros][drawer][chan] <= (meanTileDAC[ros][drawer][chan]*meanTileDAC[ros][drawer][chan]))
	  rmsTileDAC[ros][drawer][chan] = 0;
	else 
	  rmsTileDAC[ros][drawer][chan] = sqrt(rmsTileDAC[ros][drawer][chan] - (meanTileDAC[ros][drawer][chan]*meanTileDAC[ros][drawer][chan]));

	if (nEvtL1Calo[ros][drawer][chan] > 0){ 
	  meanL1Calo[ros][drawer][chan] = (meanL1Calo[ros][drawer][chan])/(nEvtL1Calo[ros][drawer][chan]);
	  rmsL1Calo[ros][drawer][chan]  = (rmsL1Calo[ros][drawer][chan])/(nEvtL1Calo[ros][drawer][chan]);
 	  meanL1CaloDAC[ros][drawer][chan] = (meanL1CaloDAC[ros][drawer][chan])/(nEvtL1Calo[ros][drawer][chan]);
	  rmsL1CaloDAC[ros][drawer][chan]  = (rmsL1CaloDAC[ros][drawer][chan])/(nEvtL1Calo[ros][drawer][chan]);
	}
	
	if(rmsL1Calo[ros][drawer][chan] <= (meanL1Calo[ros][drawer][chan]*meanL1Calo[ros][drawer][chan]))
	  rmsL1Calo[ros][drawer][chan] = 0;
	else 
	  rmsL1Calo[ros][drawer][chan] = sqrt(rmsL1Calo[ros][drawer][chan] - (meanL1Calo[ros][drawer][chan]*meanL1Calo[ros][drawer][chan]));

	if(rmsL1CaloDAC[ros][drawer][chan] <= (meanL1CaloDAC[ros][drawer][chan]*meanL1CaloDAC[ros][drawer][chan]))
	  rmsL1CaloDAC[ros][drawer][chan] = 0;
	else 
	  rmsL1CaloDAC[ros][drawer][chan] = sqrt(rmsL1CaloDAC[ros][drawer][chan] - (meanL1CaloDAC[ros][drawer][chan]*meanL1CaloDAC[ros][drawer][chan]));

	meanTileL1Calo[ros][drawer][chan] = meanTile[ros][drawer][chan] - meanL1Calo[ros][drawer][chan];
        if(rmsTileL1Calo[ros][drawer][chan] <= (meanTileL1Calo[ros][drawer][chan]*meanTileL1Calo[ros][drawer][chan]))
	  rmsTileL1Calo[ros][drawer][chan] = 0;
	else 
	  rmsTileL1Calo[ros][drawer][chan] = sqrt(rmsTileL1Calo[ros][drawer][chan] - (meanTileL1Calo[ros][drawer][chan]*meanTileL1Calo[ros][drawer][chan]));
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TileTriggerDefaultCalibTool::writeNtuple(int runNumber, int runType, TFile * rootFile)
{
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "writeNtuple(" << runNumber << "," << runType << "," << rootFile << ")" << endreq;

  TTree *t = new TTree(m_ntupleID.c_str(), "TileCalib-Ntuple");
  t->Branch("meanTile",&meanTile,"meanTile[5][64][48]/F");
  t->Branch("rmsTile",&rmsTile,"rmsTile[5][64][48]/F");
  t->Branch("meanTileDAC",&meanTileDAC,"meanTileDAC[5][64][48]/F");
  t->Branch("rmsTileDAC",&rmsTileDAC,"rmsTileDAC[5][64][48]/F");
  t->Branch("ietaTile",&ietaTile,"ietaTile[5][64][48]/I");
  t->Branch("iphiTile",&iphiTile,"iphiTile[5][64][48]/I");
  t->Branch("ipmtTile",&ipmtTile,"ipmtTile[5][64][48]/I");
  t->Branch("nEvtTile",&nEvtTile,"nEvtTile[5][64][48]/I");
  t->Branch("meanL1Calo",&meanL1Calo,"meanL1Calo[5][64][48]/F");
  t->Branch("rmsL1Calo",&rmsL1Calo,"rmsL1Calo[5][64][48]/F");
  t->Branch("meanL1CaloDAC",&meanL1CaloDAC,"meanL1CaloDAC[5][64][48]/F");
  t->Branch("rmsL1CaloDAC",&rmsL1CaloDAC,"rmsL1CaloDAC[5][64][48]/F");
  t->Branch("ietaL1Calo",&ietaL1Calo,"ietaL1Calo[5][64][48]/I");
  t->Branch("iphiL1Calo",&iphiL1Calo,"iphiL1Calo[5][64][48]/I");
  t->Branch("ipmtL1Calo",&ipmtL1Calo,"ipmtL1Calo[5][64][48]/I");
  t->Branch("nEvtL1Calo",&nEvtL1Calo,"nEvtL1Calo[5][64][48]/I");
  t->Branch("nEvtGlobal",&nEvtGlobal,"nEvtGlobal[1]/I");
  t->Branch("meanTileL1Calo",&meanTileL1Calo,"meanTileL1Calo[5][64][48]/F");
  t->Branch("rmsTileL1Calo",&rmsTileL1Calo,"rmsTileL1Calo[5][64][48]/F");
  t->Branch("DACvalue",&DACvalue,"DACvalue/F");

  // Fill with current values (i.e. tree will have only one entry for this whole run)
  t->Fill();
  t->Write();

  return StatusCode::SUCCESS;
}

StatusCode TileTriggerDefaultCalibTool::finalize()
{
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "finalizeTrigger()" << endreq;
  return StatusCode::SUCCESS;  
}
