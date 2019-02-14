/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2CaloMissingET.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigL2MissingET
//
// AUTHOR:   Denis Oliveira Damazio - Brookhaven National Laboratory
//           Chris Young - Oxford University
//           Sven Kreiss - New York University
//           
// Description: Level2 MissingET AllTEAlgo (FEX) algorithm. 
//              
//
// ********************************************************************

#include "TrigL2MissingET/T2CaloMissingET.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"
//#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "xAODTrigMissingET/TrigMissingET.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"

#include "TrigT1Interfaces/RecEnergyRoI.h"

#include <cmath>
#include <cstdio>

#define USECABLINGSERVICE
#ifndef USECABLINGSERVICE
#include "TrigL2MissingET/febmapL2.h"
#endif

// number of components for the MET feature
#define NCOM 16

T2CaloMissingET::T2CaloMissingET(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator),
    m_useCachedResult(false),
    m_data("TrigDataAccess/TrigDataAccess"),
    m_log(NULL),
    m_timersvc("TrigTimerSvc/TrigTimerSvc","T2CaloMissingET"),
    m_met_feature(NULL),
    m_cachedTE(NULL),
    m_LArOnlineID(NULL),
    m_CaloCell_ID(NULL)
{
  declareProperty("METLabel", m_featureLabel = "L2MissingET_FEB", "label for the MET feature in the HLT Navigation"); //change this to another name later
  declareProperty("METL2L1Label", m_L2L1featureLabel = "T2MissingET", "label for the MET feature from the L2=L1 algorithm");
  declareProperty("TrigDataAccess",m_data,"Data Access for LVL2 Calo Algorithms");
  declareProperty("TrigTimerSvc", m_timersvc,"TrigTimerSvc Handle.");
  declareProperty("EtaWidth", m_etaWidth = 0.1,"Eta Width of the Region of Interest");
  declareProperty("PhiWidth", m_phiWidth = 0.1,"Phi Width of the Region of Interest");  
  declareProperty("OneByOne", m_one_by_one=false, "get data from each ROB (opposite to get data from each ROS)");
  declareProperty("L1MetDiffCut",m_l1metDiffCut=100., "fail event if large difference from L1 values");  
  declareProperty("ReadL2L1", m_ReadL2L1=false, "Read the L2=L1 result");
  declareMonitoredVariable("E_x",m_Ex);
  declareMonitoredVariable("E_y",m_Ey);
  declareMonitoredVariable("E_z",m_Ez);
  declareMonitoredVariable("Met",m_met);
  declareMonitoredVariable("E",m_E);
  declareMonitoredVariable("Et",m_Et);
  //     declareMonitoredVariable("E_x_Tile",m_ExT);
  //     declareMonitoredVariable("E_y_Tile",m_EyT);
  //     declareMonitoredVariable("E_z_Tile",m_EzT);
  declareMonitoredStdContainer("Tile_LBA", m_tile_lba ); 
  declareMonitoredStdContainer("Tile_LBC", m_tile_lbc ); 
  declareMonitoredStdContainer("Tile_EBA", m_tile_eba ); 
  declareMonitoredStdContainer("Tile_EBC", m_tile_ebc ); 
  declareMonitoredStdContainer("Tile_nLBA", m_tile_nlba ); 
  declareMonitoredStdContainer("Tile_nLBC", m_tile_nlbc ); 
  declareMonitoredStdContainer("Tile_nEBA", m_tile_neba ); 
  declareMonitoredStdContainer("Tile_nEBC", m_tile_nebc ); 
  m_vec_robs.reserve(1000);

  m_timer[0] = 0;
  m_timer[1] = 0;

}

HLT::ErrorCode T2CaloMissingET::hltInitialize(){

  m_log = new MsgStream(msgSvc(), name());
  (*m_log) << MSG::INFO << "on hltInitialize()" << endmsg;

  if ((m_data.retrieve()).isFailure()) {
    (*m_log) << MSG::ERROR << "Could not get m_data" << endmsg;
    return HLT::OK;
  }

  if( (m_timersvc.retrieve()).isFailure() ) {
    (*m_log) << MSG::WARNING << name() << ": Unable to locate TrigTimer Service" << endmsg;
  } 
  
  if ( !m_timersvc.empty()) {
    m_timer[0] = m_timersvc->addItem("totaltime");
    m_timer[1] = m_timersvc->addItem("onlyExecs");
  }

  // Monitor the variables Ex, Ey, Ez per event
  m_detid.push_back(TTEM);
  if ( m_one_by_one ){
    m_detid.push_back(TTHEC);
    m_detid.push_back(FCALEM);
    m_detid.push_back(FCALHAD);
  }

#ifdef USECABLINGSERVICE
  if(m_cablingKey.initialize().isFailure()) {
    *m_log << MSG::FATAL << "Could not get LAr Cabling map with key " << m_cablingKey.key() << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

  if(m_RodKey.initialize().isFailure()) {
    *m_log << MSG::FATAL << "Could not get LAr FebRod map with key " << m_RodKey.key() << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
#endif

  StoreGateSvc* detStore = 0;
  if (service( "DetectorStore", detStore ).isFailure()) {
    *m_log << MSG::FATAL << "Unable to locate DetectorStore" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }	

  if (detStore->retrieve(m_LArOnlineID, "LArOnlineID").isFailure()) {
    *m_log << MSG::FATAL << "Could not get LArOnlineID helper!" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

  if (detStore->retrieve(m_CaloCell_ID, "CaloCell_ID").isFailure()) {
    *m_log << MSG::FATAL << "Could not get CaloCell_ID helper!" << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

  return HLT::OK;
}

HLT::ErrorCode T2CaloMissingET::hltFinalize(){
  (*m_log) << MSG::INFO << "on hltFinalize()" << endmsg;
  delete m_log;
  return HLT::OK;
}


HLT::TriggerElement* T2CaloMissingET::makeSeeding(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int type_out ) {

  HLT::TEVec allTEs;
  for ( unsigned type = 0; type < tes_in.size(); ++ type ) {
    std::vector<HLT::TriggerElement*>& tes = tes_in.at(type);

    for ( unsigned teIdx = 0; teIdx < tes.size(); ++teIdx ) {

      const LVL1::RecEnergyRoI* lvl1_energyRoi(0);
      if ( getFeature(tes.at(teIdx), lvl1_energyRoi) == HLT::OK) { // for the seeding use only XE/TE/XS trigger elements (i.e. those having the RecEnergy obj)
        if ( lvl1_energyRoi ) {
          allTEs.push_back(tes.at(teIdx));
          /*
            if (msgLvl() <= MSG::INFO) {
	    msg() << MSG::INFO << "using TE as a seed " << tes.at(teIdx)->getId() << endmsg;
            }
          */
        } else {
          /*
	    if (msgLvl() <= MSG::INFO) {
            msg() << MSG::INFO << "skipping TE  " << tes.at(teIdx)->getId() << endmsg;
            }
          */
        }
      }     
    }
  }
  return  config()->getNavigation()->addNode(allTEs, type_out);
}
//////////////////////////////////////////////////////////

HLT::ErrorCode T2CaloMissingET::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
					   unsigned int type_out)
{


  // CACHING
  // first check whether we executed this instance before:
  if (m_useCachedResult) {
    if (msgLvl() <= MSG::DEBUG) {
      (*m_log) << MSG::DEBUG << "Executing this T2iCaloMissingET " << name() << " in cached mode" << endmsg;
    }

    HLT::TriggerElement* outputTE = makeSeeding(tes_in, type_out);
    m_config->getNavigation()->copyAllFeatures( m_cachedTE, outputTE );

    return HLT::OK;
  }

  // start monitoring
  beforeExecMonitors().ignore();

  bool debug = (m_log->level() <= MSG::DEBUG);

  m_timer[0]->start();

  m_met_feature = new xAOD::TrigMissingET(); m_met_feature->makePrivateStore();

  std::vector <std::string> vs_aux;
  for(int i = 0; i < NCOM; i++)
   vs_aux.push_back("");    
  m_met_feature->defineComponents(vs_aux);   
  
  // assign names and usedchannels
  init(m_met_feature);
  m_timer[1]->start();
  m_timer[1]->pause();

  //If flag is set, read in results of L2=L1 algorithm and put the results on the zero component
  if(m_ReadL2L1) {    

    if(msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG<< " Reading in L2=L1 MET container" << endmsg;
    }

    bool storeL2L1 = true;    

    unsigned int tes_in_size=tes_in.size(); // = 2 (seeded) or 1 (unseeded)
    std::vector<const xAOD::TrigMissingET*> vectorL2L1MissingET;

    if (tes_in_size != 1) {
      msg() << MSG::WARNING << " Failed to get L2=L1 vectorMissingETs. No storage of L1 will be performed " << endmsg;
      storeL2L1 = false;
    }
    else {
      HLT::ErrorCode status = getFeatures(tes_in[0][0], vectorL2L1MissingET);
      if(status != HLT::OK) {
        msg() << MSG::WARNING << "Cannot find L2=L1 MET result!" << endmsg;
        storeL2L1 = false;
      }
      else {
        if ( vectorL2L1MissingET.size() < 1 ) {
          msg() << MSG::WARNING << "Cannot find L2=L1 MET result!" << endmsg;
          storeL2L1 = false;
        }
      }
    }

    if(storeL2L1) {
      if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << " Got " << vectorL2L1MissingET.size()
              << " vectorL2L1MissingET size associated to the TE" << endmsg;
      }
    }

   const xAOD::TrigMissingET* metL2L1;
   if(storeL2L1) {
     metL2L1 = vectorL2L1MissingET.front();
     if (metL2L1==0){
       if(msgLvl() <= MSG::DEBUG) {
         msg() << MSG::DEBUG<< " L2=L1 MissingET pointer is null! No storage of L1 will be performed" << endmsg;
       }
       storeL2L1 = false;
     }
   }
   if(storeL2L1) {
     //float L2L1MET=metL2L1->et();    // No component level MET so don't get this
     float L2L1SET=metL2L1->sumEt(); // MeV
     float L2L1Ex=metL2L1->ex();     // MeV
     float L2L1Ey=metL2L1->ey();     // MeV
     float L2L1Ez=metL2L1->ez();     // MeV

     m_met_feature->setExComponent(0,L2L1Ex);
     m_met_feature->setEyComponent(0,L2L1Ey);
     m_met_feature->setEzComponent(0,L2L1Ez);
     m_met_feature->setSumEtComponent(0,L2L1SET);
     float tempE = sqrt(L2L1Ex*L2L1Ex+L2L1Ey*L2L1Ey+L2L1Ez*L2L1Ez);
     m_met_feature->setSumEComponent(0,tempE);
   }
  }
  LArFebEnergyCollection::const_iterator feb_it, feb_it_beg, feb_it_end;
  TileL2Container::const_iterator draw_it, draw_it_beg, draw_it_end;
  if ( debug ) (*m_log) << MSG::DEBUG << "Executing" << endmsg;

  int k_n[30];
  for (int p=0; p<30; p++) k_n[p]=0;

  if ( debug ) (*m_log) << MSG::DEBUG << "Size of detid:" << m_detid.size() << endmsg;

#ifdef USECABLINGSERVICE
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling = *cablingHdl;
  if(!cabling) {
     (*m_log) << MSG::ERROR << "Do not have cabling map !" << endmsg;
     return HLT::ERROR;
  }
  SG::ReadCondHandle<LArFebRodMapping> rodHdl{m_RodKey};
  const LArFebRodMapping* rodmap = *rodHdl;
  if(!rodmap) {
     (*m_log) << MSG::ERROR << "Do not have LAr FEB-ROD map !" << endmsg;
     return HLT::ERROR;
  }
#endif
  //bool BSerrors = false;
  //if (m_data->report_error()) BSerrors = true;

  // Get the LAr part of the Calo info
  for(size_t detid = 0; detid<m_detid.size();detid++){
    m_timer[1]->resume();
    if ( (m_data->LoadFullCollections(feb_it_beg,feb_it_end, m_detid[detid])).isFailure() ){
      (*m_log) << MSG::INFO << "problems to run load collection" << endmsg;
      return HLT::OK;
    }

    m_timer[1]->pause();
    for(feb_it = feb_it_beg; feb_it != feb_it_end; ++feb_it){
      // start timer

#ifdef USECABLINGSERVICE
      if( (*feb_it)->getFebId()  ==  0) {
	if(msgLvl() <= MSG::DEBUG) {
	  (*m_log) << "component unknown for this FEB id: " << std::hex << (*feb_it)->getFebId() << std::dec << endmsg;
	  (*m_log) << "this feb has: Ex " << (*feb_it)->getFebEx() << "  Ey: " << (*feb_it)->getFebEy() << "  Ez: " << (*feb_it)->getFebEz() << endmsg;
	  (*m_log) << "Skipping this FEB" << endmsg;
	}
	continue;
      }
      HWIdentifier febid2((*feb_it)->getFebId());

      Identifier offChId;
      int ichannel=0;
      do {
        HWIdentifier onlChId = m_LArOnlineID->channel_Id(febid2,ichannel);
        offChId = cabling->cnvToIdentifier(onlChId);
        ichannel++;
        if (ichannel>127) {
          (*m_log) << MSG::ERROR 
		   << "not connected channel found for this FEB: " << (*feb_it)->getFebId() << endmsg;
          return HLT::ERROR;
        }
      } while(!offChId.is_valid());

      int caloSamp = m_CaloCell_ID->sampling(offChId); 
      HWIdentifier modId = rodmap->getReadoutModuleID(febid2); //ReadOutModuleId
      int subdet = m_larROModSvc.em_hec_fcal(modId);  // em=0, hec=1, fcal=2
      int caloId = m_larROModSvc.barrel_ec(modId);    // barrel:0 or EndCAp:1

      // stop timer

      // compute the component index
      unsigned int k=0;
      //bool doHackForHEC=false;
      switch (subdet) {
      case 0: // EM
	k = 1 + 4*caloId + caloSamp;
	break;
      case 1: // HEC
	k = 9 + caloSamp;
	//doHackForHEC=true;
	break;
      case 2: // FCAL
	k = 11 + caloSamp;
	break;
      default:
	(*m_log) << MSG::FATAL << "Unknown subdetector!" << endmsg;
	return HLT::ERROR;
      }

      // check that the sample value is sensible
      if (k<30){
	k_n[k]=k_n[k]+1;
      }else{
	(*m_log) << MSG::ERROR << "Unknown subdetector index!" << endmsg;
      }
#else
      //if(msgLvl() <= MSG::DEBUG) (*m_log) << "USING FEB MAP" << endmsg;

      unsigned char k = 0;
      std::map<int,unsigned char>::const_iterator it = febIdToComponentMap.find( (*feb_it)->getFebId() );
      if( it != febIdToComponentMap.end() ) {
	k = it->second +1;
      } else {
	if(msgLvl() <= MSG::DEBUG) {
	  (*m_log) << "component unknown for this FEB id: " << std::hex << (*feb_it)->getFebId() << std::dec << endmsg;
	  (*m_log) << "this feb has: Ex " << (*feb_it)->getFebEx() << "  Ey: " << (*feb_it)->getFebEy() << "  Ez: " << (*feb_it)->getFebEz() << endmsg;
	}
	k = 1; // TODO to which component do we add unknown FEB energies; or should we have an "unknown" component
      }
      
#endif

      // save into TrigMissingET: fetch MEx,...SumE, add the current values, save them back
      float tempEx = m_met_feature->exComponent(k);
      float compEx = (*feb_it)->getFebEx();
      m_met_feature->setExComponent(k,tempEx-compEx);
      float tempEy = m_met_feature->eyComponent(k);
      float compEy = (*feb_it)->getFebEy();
      m_met_feature->setEyComponent(k,tempEy-compEy);
      float tempEz = m_met_feature->ezComponent(k);
      float compEz = (*feb_it)->getFebEz();
      m_met_feature->setEzComponent(k,tempEz-compEz);
      float tempE = m_met_feature->sumEComponent(k);
      tempE += sqrt(compEx*compEx+compEy*compEy+compEz*compEz);
      m_met_feature->setSumEComponent(k,tempE);
      float tempEt = m_met_feature->sumEtComponent(k);
      tempEt += sqrt(compEx*compEx+compEy*compEy);
      m_met_feature->setSumEtComponent(k,tempEt);
      //Sum of Signs?

      if ( debug ){
	(*m_log) << MSG::DEBUG;
	(*m_log) << std::hex << "febID=0x" << (*feb_it)->getFebId() << std::dec;
	(*m_log) << "; Ex=" << (*feb_it)->getFebEx();
	(*m_log) << "; Ey=" << (*feb_it)->getFebEy();
	(*m_log) << "; Ez=" << (*feb_it)->getFebEz();
	(*m_log) << endmsg;
	} // end of debug
    } // end of feb_it for
  } // end of detid for

  // Get the Tile part of the Calo info
  if( (m_data->LoadFullCollections(draw_it_beg, draw_it_end )).isFailure() ){
    (*m_log) << MSG::INFO << "Call to LoadFullCollections for tile drawers failed." << endmsg;
    return HLT::OK;
  }
  draw_it = draw_it_beg;
  m_tile_lba.clear(); 
  m_tile_lbc.clear(); 
  m_tile_eba.clear(); 
  m_tile_ebc.clear(); 
  m_tile_nlba.clear(); 
  m_tile_nlbc.clear(); 
  m_tile_neba.clear(); 
  m_tile_nebc.clear(); 
  for(;draw_it!=draw_it_end;++draw_it){
    int targetComponent = 10;
    int id = (*draw_it)->identify();
    int idd = (id & 0xFF); 
    float Etd = (*draw_it)->Et()/1e3; 
    if ( Etd > 490. ) Etd=490.0; 
    if ( (id & 0x700) == 0x100 ){ 
        m_tile_nlba.push_back( idd ); 
        m_tile_lba.push_back( Etd ); 
    } 
    if ( (id & 0x700) == 0x200 ){ 
        m_tile_nlbc.push_back( idd ); 
        m_tile_lbc.push_back( Etd ); 
    } 
    if ( (id & 0x700) == 0x300 ){ 
        m_tile_neba.push_back( idd ); 
        m_tile_eba.push_back( Etd ); 
    } 
    if ( (id & 0x700) == 0x400 ){ 
        m_tile_nebc.push_back( idd ); 
        m_tile_ebc.push_back( Etd ); 
    } 
    //if(m_outputLevel <= MSG::DEBUG) (*m_log) << MSG::DEBUG << std::hex << "drawerID=0x" << id << std::dec << endmsg;
    if( (id & 0x700) == 0x100  ||  (id & 0x700) == 0x200 ) {
      //if(m_outputLevel <= MSG::DEBUG) (*m_log) << MSG::DEBUG << "barrel" << endmsg;
      targetComponent = 10;
    }else if( (id & 0x700) == 0x300  ||  (id & 0x700) == 0x400 ) {
      //if(m_outputLevel <= MSG::DEBUG) (*m_log) << MSG::DEBUG << "extended barrel" << endmsg;
      targetComponent = 11;
    }else{
      (*m_log) << MSG::ERROR << "dont know which part this tile drawer belongs to." << endmsg;
    }

    float tempEx = m_met_feature->exComponent(targetComponent);
    float compEx = (*draw_it)->Ex();
    m_met_feature->setExComponent(targetComponent,tempEx-compEx);
    float tempEy = m_met_feature->eyComponent(targetComponent);
    float compEy = (*draw_it)->Ey();
    m_met_feature->setEyComponent(targetComponent,tempEy-compEy);
    float tempEz = m_met_feature->ezComponent(targetComponent);
    float compEz = (*draw_it)->Ez();
    m_met_feature->setEzComponent(targetComponent,tempEz-compEz);
    float tempE = m_met_feature->sumEComponent(targetComponent);
    tempE += sqrt(compEx*compEx+compEy*compEy+compEz*compEz);
    m_met_feature->setSumEComponent(targetComponent,tempE);
    float tempEt = m_met_feature->sumEtComponent(targetComponent);
    tempEt += sqrt(compEx*compEx+compEy*compEy);
    m_met_feature->setSumEtComponent(targetComponent,tempEt);
    if ( debug ){
      (*m_log) << MSG::DEBUG;
      (*m_log) << std::hex << "draweID=0x" << (*draw_it)->identify() << std::dec;
      (*m_log) << "; Ex=" << (*draw_it)->Ex();
      (*m_log) << "; Ey=" << (*draw_it)->Ey();
      (*m_log) << "; Ez=" << (*draw_it)->Ez();
      (*m_log) << endmsg;
    }

  }
  if ( debug )  (*m_log) << MSG::DEBUG << "Setting energies" << endmsg;

  //Fill the total Ex,Ey,Ez,E,Et
  float tempEx=0.;
  float tempEy=0.;
  float tempEz=0.;
  float tempE=0.;
  float tempEt=0.;
  for (int p=1; p<NCOM-1; p++) {
    tempEx += m_met_feature->exComponent(p);
    tempEy += m_met_feature->eyComponent(p);
    tempEz += m_met_feature->ezComponent(p);
    tempE += m_met_feature->sumEComponent(p);
    tempEt += m_met_feature->sumEtComponent(p);
  }
  m_met_feature->setEx(tempEx);
  m_met_feature->setEy(tempEy);
  m_met_feature->setEz(tempEz);
  m_met_feature->setSumE(tempE);
  m_met_feature->setSumEt(tempEt);
  

  if ( debug ) {
    (*m_log) << MSG::DEBUG << "m_met_feature" << std::endl << m_met_feature << endmsg;
    for(int i=0; i < NCOM; i++) { 
      (*m_log) << MSG::DEBUG << "m_met_feature component: " << i;
      (*m_log) << "\t Ex   : " << m_met_feature->exComponent(i);
      (*m_log) << "\t Ey   : " << m_met_feature->eyComponent(i);
      (*m_log) << "\t Ez   : " << m_met_feature->ezComponent(i);
      (*m_log) << "\t SumE : " << m_met_feature->sumEComponent(i);
      (*m_log) << "\t SumEt: " << m_met_feature->sumEtComponent(i);
      (*m_log) << endmsg;
    }
  }
  // convert back to GeV for output log
  m_Ex=tempEx/1000.;
  m_Ey=tempEy/1000.;
  m_Ez=tempEz/1000.;
  m_E=std::fabs(tempE/1000.);
  m_Et=std::fabs(tempEt/1000.);
  m_met=sqrt(tempEx*tempEx+tempEy*tempEy)/1000.;

  if ( debug ) {
    msg() << MSG::DEBUG << "REGTEST: (LVL2 FEB) MET = " <<  m_met << " GeV" << endmsg;
    msg() << MSG::DEBUG << "REGTEST: (LVL2 FEB) METx = " <<  m_Ex << " GeV" << endmsg;
    msg() << MSG::DEBUG << "REGTEST: (LVL2 FEB) METy = " <<  m_Ey << " GeV" << endmsg;
    msg() << MSG::DEBUG << "REGTEST: (LVL2 FEB) SumEt = " <<  m_Et << " GeV" << endmsg;
  }

  m_timer[1]->stop();
  m_timer[0]->stop();

  HLT::TriggerElement* outputTE = makeSeeding(tes_in, type_out);

  // save feature to output TE:
  HLT::ErrorCode hltStatus = attachFeature(outputTE, m_met_feature, m_featureLabel);
 
  if ( hltStatus != HLT::OK ) {
    msg() << MSG::WARNING // ERROR
	  << "Write of TrigMissingET feature into outputTE failed"
	  << endmsg;
    return hltStatus;
  }

  // CACHING
  // if we got here, everything was okay. so, we cache the feature for further execution of this instance in e.g. other MET Sequences:
  m_useCachedResult = true;
  m_cachedTE = outputTE;

  // stop monitoring
  afterExecMonitors().ignore();

  return HLT::OK;

}

//////////////////////////////////////////////////////////

HLT::ErrorCode T2CaloMissingET::init(xAOD::TrigMissingET *met){
  int ncom=met->getNumberOfComponents();
  if(ncom!=NCOM){
    msg() << MSG::ERROR << "Wrong number of TrigMissingET dimension." << endmsg;
    return HLT::NO_HLT_RESULT;
  }
  met->setNameOfComponent(0, "L1Calo   ");
  met->setNameOfComponent(1, "PreSamplB");
  met->setNameOfComponent(2, "EMB1     ");
  met->setNameOfComponent(3, "EMB2     ");
  met->setNameOfComponent(4, "EMB3     ");
  met->setNameOfComponent(5, "PreSamplE");
  met->setNameOfComponent(6, "EME1     ");
  met->setNameOfComponent(7, "EME2     ");
  met->setNameOfComponent(8, "EME3     ");
  met->setNameOfComponent(9, "HEC      ");
  met->setNameOfComponent(10,"TileBar  ");
  met->setNameOfComponent(11,"TileExt  ");
  met->setNameOfComponent(12,"FCalEM   ");
  met->setNameOfComponent(13,"FCalHad1 ");
  met->setNameOfComponent(14,"FCalHad2 ");
  met->setNameOfComponent(15,"Muons    ");
  // Already set to zero by constructor of EDM
  for(int index=0;index<ncom-1;index++){
    met->setUsedChannelsComponent(index,1); // Suggestion by Diego
    met->setExComponent(index,0.);
    met->setEyComponent(index,0.);
    met->setEzComponent(index,0.);
    met->setSumEComponent(index,0.);
    met->setSumEtComponent(index,0.);   
  }
  return HLT::OK;
}

HLT::ErrorCode T2CaloMissingET::prepareRobRequests( const std::vector<HLT::TEConstVec>& /*inputs*/ ){

   // Calculate ROBs needed
   const RoiDescriptor roi (true) ;

   m_data->ROBList( roi, m_vec_robs);

   config()->robRequestInfo()->addRequestScheduledRobIDs(m_vec_robs);

   return HLT::OK;

}
