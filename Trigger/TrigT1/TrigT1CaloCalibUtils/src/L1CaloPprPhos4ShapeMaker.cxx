/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibUtils/L1CaloPprPhos4ShapeMaker.h"

#include "TH2F.h"
#include "TStyle.h"
#include "TFile.h"

L1CaloPprPhos4ShapeMaker::L1CaloPprPhos4ShapeMaker(const std::string& name, ISvcLocator* pSvcLocator) : AthAlgorithm(name, pSvcLocator),
   m_signalShapes(0),
   m_storeGate(0),
   m_detStore(0),
   m_histTool("TrigT1CaloLWHistogramTool"),
   m_currentDatabaseMapsFilled(false),
   m_l1aFadcSlice(0),
   m_triggerTowers(0),
   m_rodHeader(0),
   m_evtInfo(0),
   m_runNumber(0),
   m_attrList(0),
   m_l1CaloTTIdTools("LVL1::L1CaloTTIdTools/L1CaloTTIdTools"),
   m_caloMgr(0),
   m_ttSvc(0),
   m_ttOnlineIdHelper(0),
   m_lvl1Helper(0),
   m_signalProfiles(0),
   m_firstEvent(true),
   m_minSignalHeight(0),
   m_isTile(false),
   m_isLAr(false)
{
   
//  declareProperty("UseTile",m_useTile = false);
   declareProperty("outputDirectory",m_outputDirectory = ".");
   declareProperty("minimumSignalSizeInAdcCounts",m_minSignalHeight = 30);
   declareProperty("selectedCoolIds",m_selectedCoolIds);
   
   SetAtlasStyle();
}

StatusCode L1CaloPprPhos4ShapeMaker::initialize(){
   StatusCode sc;
   
   // The MsgStream is the athena replacement for std::cout and std::endl
   // You can specify the level of output in your python jobOptions.py file
   // Options are DEBUG, INFO, WARNING, ERROR, FATAL
   // You can still use std::cout and std::endl, but I guess it's considered bad form.
   ATH_MSG_INFO("In Initialize()");
   
   
   // create selected histograms
   for(unsigned int i=0;i<m_selectedCoolIds.size()&&i<m_maxSelectedCoolIds;++i){
      char name[500];
      sprintf(name,"selected_%08x",m_selectedCoolIds[i]);
      TH2F *histo = new TH2F(name,name,15*25,0,15*25,1024,0,1024);
      m_histosForSelectedCoolIds[m_selectedCoolIds[i]] = histo;
      ATH_MSG_INFO("creating select histogram for: "<< std::hex << m_selectedCoolIds[i] << std::dec );
   }
   
   
   m_storeGate = 0;
   m_detStore = 0;
   
//    m_l1CaloTTIdTools = 0;
   m_caloMgr = 0;
   m_ttSvc = 0;
//    m_ttOnlineIdHelper = 0; 
   m_lvl1Helper = 0;
//    m_cells2tt = 0;

   m_evtInfo = 0;
   m_runNumber = 0;
   m_attrList = 0;
   m_pprChanCalibFolderName = "/TRIGGER/L1Calo/V1/Calibration/Calib1/PprChanCalib";
   m_readoutConfigFolderName = "/TRIGGER/L1Calo/V1/Configuration/ReadoutConfig";
   m_ppmDeadChannelsFolderName = "/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels";
   m_l1caloDerivedRunParsFolderName = "/TRIGGER/L1Calo/V1/Conditions/DerivedRunPars";
   m_l1caloRunParametersFolderName = "/TRIGGER/L1Calo/V1/Conditions/RunParameters";
   
   m_currentDatabaseMapsFilled = false;
   m_fullDataDelayName = "FullDelayData";
   m_currentFullDelayDataMap.clear();
   m_pedValueName = "PedValue";
   m_currentPedValueMap.clear();
   m_l1aFadcSliceName = "l1aFadcSlice";
   m_l1aFadcSlice = 0;
   m_errorCodeName = "ErrorCode";
   m_timingRegimeName = "timingRegime";
   m_timingRegime.clear();
   m_ttcConfigurationName = "ttcConfiguration";
   m_ttcConfiguration.clear();
   
   m_signalShapes = new L1CaloPprPhos4ShapeCollection;
   
   m_signalShapes->SetOutputDirectoryName(m_outputDirectory);
   ATH_MSG_INFO( "outputdir: " << m_outputDirectory );
   m_signalShapes->SetMinimumSignalHeight(m_minSignalHeight);
   ATH_MSG_INFO( "min signal height: " << m_minSignalHeight );
   
   // Setup all the Athena Services
   sc = this->loadAthenaObjects();if(sc.isFailure()){return sc;}
   
   m_signalShapes->SetHistogramTool(m_histTool);
   
   m_firstEvent = true;
   
   return StatusCode::SUCCESS;
}

StatusCode L1CaloPprPhos4ShapeMaker::execute(){
   // This section is for code that is run for each event.
   
   ATH_MSG_INFO("In execute()");
   
   StatusCode sc;
   sc = GetEventInfo();
   if(sc.isFailure()){
      ATH_MSG_WARNING( "Failed to get Event Info, but continuing." );
   }
   
   if(m_runNumber == 0){
      m_runNumber = m_evtInfo->event_ID()->run_number();
      
      ATH_MSG_INFO("Run Number: " << m_runNumber );
      m_signalShapes->SetRunNumber(m_runNumber);
   }
   
   unsigned int eventNum = m_evtInfo->event_ID()->event_number();
   
   // Retrieve the Database parameters.
   // Only need to do this once because they do not change from event to event.
   if(!m_currentDatabaseMapsFilled){
      
      ATH_MSG_INFO( "Getting database handle for folder " << m_readoutConfigFolderName );
      
      
      
      // first  open the ReadoutConfig folder so I can see in which bunch
      // the peak should be located.
      
      sc = GetDatabaseHandle(m_readoutConfigFolderName);
      if(sc.isFailure()){
         ATH_MSG_INFO( "Failed to retrieve database handle: " << m_readoutConfigFolderName );
         return sc;
      }
      
      // should really only be one entry, but still will catch everything
      CondAttrListCollection::const_iterator attrListCollItr = m_attrList->begin();
      
      for(;attrListCollItr != m_attrList->end();attrListCollItr++){
         const coral::AttributeList* attrList = &(attrListCollItr->second);
         
         m_l1aFadcSlice = (*attrList)[m_l1aFadcSliceName].data<unsigned int>();
         // look at the next loop to find where I fill this value into the
         // collection
         
      } // end for(attrList)
      
      // now retreive the timingRegime from the DerivedRunPars folder
      ATH_MSG_INFO( "Getting database handle for folder " << m_l1caloDerivedRunParsFolderName );
      
      sc = GetDatabaseHandle(m_l1caloDerivedRunParsFolderName);
      if(sc.isFailure()){
         ATH_MSG_INFO( "Failed to retrieve database handle: " << m_l1caloDerivedRunParsFolderName  );
         return sc;
      }
      
      // should really only be one entry, but still will catch everything
      attrListCollItr = m_attrList->begin();
      
      for(;attrListCollItr != m_attrList->end();attrListCollItr++){
         const coral::AttributeList* attrList = &(attrListCollItr->second);
         
         m_timingRegime = (*attrList)[m_timingRegimeName].data<std::string>();
         
      } // end for(attrList)
      // set timingRegime for SignalShapeCollection and databases
      SetTimingRegime(m_timingRegime);
      
      // now retreive the ttcConfiguration from the RunParameters folder
      // this allows to check if LAr or Tile is being used.
      ATH_MSG_INFO( "Getting database handle for folder " << m_l1caloRunParametersFolderName );
      
      sc = GetDatabaseHandle(m_l1caloRunParametersFolderName);
      if(sc.isFailure()){
         ATH_MSG_INFO( "Failed to retrieve database handle: " << m_l1caloRunParametersFolderName  );
         return sc;
      }
      
      // should really only be one entry, but still will catch everything
      attrListCollItr = m_attrList->begin();
      
      for(;attrListCollItr != m_attrList->end();attrListCollItr++){
         const coral::AttributeList* attrList = &(attrListCollItr->second);
            
         m_ttcConfiguration = (*attrList)[m_ttcConfigurationName].data<std::string>();
         
         if(m_ttcConfiguration.find("LAr") != std::string::npos){
            ATH_MSG_INFO( " This is a LAr run." );
            m_isLAr  = true;
            m_isTile = false;
            m_signalShapes->SetLArRun(true);
         }
         else if(m_ttcConfiguration.find("Tile") != std::string::npos){
            ATH_MSG_INFO( " This is a Tile run." );
            m_isLAr  = false;
            m_isTile = true;
            m_signalShapes->SetTileRun(true);
         }
         else{
            ATH_MSG_INFO( " Could not determine Tile or LAr so all channels will be processed." );
            m_isLAr  = false;
            m_isTile = false;
         }
         
      } // end for(attrList)
      
      
      
      ATH_MSG_INFO( "Getting database handle for folder " << m_pprChanCalibFolderName );
      // now retrieve the PprChanCalib folder so I can
      // get the FullDelayData and the pedestal value
      
      sc = GetDatabaseHandle(m_pprChanCalibFolderName);
      if(sc.isFailure()){
         ATH_MSG_INFO( "Failed to retrieve database handle." );
         return sc;
      }
      
      attrListCollItr = m_attrList->begin();
      
      for(;attrListCollItr != m_attrList->end();attrListCollItr++){
         
         const unsigned int coolId = attrListCollItr->first;
         
         const coral::AttributeList* attrList = &(attrListCollItr->second);
         
         unsigned int tempFullDelayData = (*attrList)[m_fullDataDelayName].data<unsigned int>();
         m_currentFullDelayDataMap[coolId] = tempFullDelayData;
         
         sc = m_signalShapes->SetCurrentFullDelayData(coolId,tempFullDelayData);
         if(sc.isFailure()){
            ATH_MSG_INFO( "Failed to set FullDelayData from database to channel 0x" << std::hex << coolId << std::dec );
         }
         
         unsigned int tempPedValue = (*attrList)[m_pedValueName].data<unsigned int>();
         m_currentPedValueMap[coolId] = tempPedValue;
         
         sc = m_signalShapes->SetPedValue(coolId,tempPedValue);
         if(sc.isFailure()){
            ATH_MSG_INFO( "Failed to set PedValue from database to channel 0x" << std::hex << coolId << std::dec );
         }
         
         // utilize this loop over the cool channel id's in order to set the l1aFadcSlice
         // which is the same for all channels.
         sc = m_signalShapes->SetL1aFadcSlice(coolId,m_l1aFadcSlice);
         if(sc.isFailure()){
            ATH_MSG_INFO( "Failed to set L1aFadcSlice from database to channel 0x" << std::hex << coolId << std::dec );
         }
         
         // want to check that this channel is a valid channel
         bool isValid = false;
         L1CaloCoolChannelId id(coolId);
         isValid = IsCoolIdValid(id);
         sc = m_signalShapes->SetValidChannel(coolId,isValid);
         if(sc.isFailure()){
            ATH_MSG_INFO( "Failed to set ValidChannel from database to channel 0x" << std::hex << coolId << std::dec );
         }
         
      } // end for(attrList)
      
      // now retrieve the PprDeadChannels folder
      // want to book the dead channels so I can add this info to the tree
      sc = GetDatabaseHandle(m_ppmDeadChannelsFolderName);
      if(sc.isFailure()){
         ATH_MSG_INFO( "Failed to retrieve database handle." );
         return sc;
      }
      
      attrListCollItr = m_attrList->begin();
      
      for(;attrListCollItr != m_attrList->end();attrListCollItr++){
         const unsigned int coolId = attrListCollItr->first;
         
         const coral::AttributeList* attrList = &(attrListCollItr->second);
         
         unsigned int tempErrorCode = (*attrList)[m_errorCodeName].data<unsigned int>();
         bool enabled = true;
         if(tempErrorCode & 0xf) enabled = false;
         
         sc = m_signalShapes->SetChannelEnabled(coolId,enabled);
         if(sc.isFailure()){
            ATH_MSG_INFO( "Failed to set ChannelEnabled from database to channel 0x" << std::hex << coolId << std::dec );
         }
         
      } // end for(attrList)
      
      
      ATH_MSG_INFO( "Finished database initialization." );
      m_currentDatabaseMapsFilled = true;
   }
   
   
   ATH_MSG_INFO("Event Number: " << eventNum );
   
   if (eventNum == 0 && m_isTile){
      return sc;
   }
   
   
   sc = GetTriggerTowers();
   if(sc.isFailure()){
      ATH_MSG_FATAL( "Failed to get the trigger tower collection so stopping." );
      return StatusCode::FAILURE;
   }
   
   sc = GetRODHeader();
   if(sc.isFailure()){
      ATH_MSG_FATAL( "Failed to get the ROD Header so stopping." );
      return StatusCode::FAILURE;
   }
   
   ATH_MSG_INFO( "Filling trigger tower pusles." );
   
   Itr_tt towerItr = m_triggerTowers->begin();
   Itr_tt towerEnd = m_triggerTowers->end();
   
   ATH_MSG_INFO( "Trigger Tower Collection Size: " << m_triggerTowers->size() );
   
   // Loop over the trigger tower collection
   for(;towerItr != towerEnd;++towerItr){
      
      // need to read the scan step counter which counts the steps taken during
      // the run and can be used to determine the phos4 setting
      DataVector<LVL1::RODHeader>::const_iterator rodItr = m_rodHeader->begin();
      const unsigned int scanStepNumber = (unsigned int)(*rodItr)->stepNumber();
      // FYI: As of 27/10/2009 for historic reasons this value runs from 0 to 49
      // but we are only interested in the modulo of this step with respect to 
      // 25
      const unsigned int phos4step = scanStepNumber % 25; // 0 to 24
      
      
      // get the eta/phi
      double eta = (*towerItr)->eta();
      double phi = (*towerItr)->phi();
      
      // add the EM trigger tower to the collection
      {
         // get cool id
         unsigned int layer = 0; // 0 == EM Layer; 1 == HAD Layer
         L1CaloCoolChannelId coolId = GetCoolId(eta,phi,layer);
         
         const std::vector<int> adcs = (*towerItr)->emADC();
         
         sc = m_signalShapes->Fill(coolId.id(),phos4step,adcs);
         if(sc.isFailure()){
            ATH_MSG_WARNING( " [EM] Failed to fill signal shape collection." );
         }
         
         // fill selected channel histograms
         for(unsigned int i=0;i<m_selectedCoolIds.size();++i){
            if(m_selectedCoolIds[i] == coolId.id()){
               std::map<unsigned int,TH2F*>::iterator itr =  m_histosForSelectedCoolIds.find(m_selectedCoolIds[i]);
               if(itr == m_histosForSelectedCoolIds.end()) continue;
               
               for(unsigned int j=0;j<adcs.size();++j)
                  itr->second->Fill(phos4step+j*25,adcs[j]);
               break;
            }
         }
         
         
         // need to set the triggered time slice in the signal shapes:
         if(m_firstEvent){
            sc = m_signalShapes->SetL1aFadcSlice(coolId.id(),(*towerItr)->emADCPeak());
            if(sc.isFailure()){
               ATH_MSG_WARNING( "Failed to set L1aFadcSlice from Em data to channel 0x" << std::hex << coolId << std::dec );
            }
            if(!m_signalShapes->SetChannelEtaPhiLayer(coolId.id(),eta,phi,L1CaloPprPhos4Shape::EM)) {
               ATH_MSG_WARNING( "Failed to set ChannelEtaPhiLayer from Em data to channel 0x" << std::hex << coolId << std::dec );
            }
         }
         
      }
      
      // add the HAD trigger tower to the collection
      {
         // get cool id
         unsigned int layer = 1; // 0 == EM Layer; 1 == HAD Layer
         L1CaloCoolChannelId coolId = GetCoolId(eta,phi,layer);
         
         const std::vector<int> adcs = (*towerItr)->hadADC();
         
         sc = m_signalShapes->Fill(coolId.id(),phos4step,adcs);
         if(sc.isFailure()){
            ATH_MSG_WARNING( " [HAD] Failed to fill signal shape collection." );
         }
         
         
         // fill selected channel histograms
         for(unsigned int i=0;i<m_selectedCoolIds.size();++i){
            if(m_selectedCoolIds[i] == coolId.id()){
               std::map<unsigned int,TH2F*>::iterator itr =  m_histosForSelectedCoolIds.find(m_selectedCoolIds[i]);
               if(itr == m_histosForSelectedCoolIds.end()) continue;
               
               for(unsigned int j=0;j<adcs.size();++j){
                  itr->second->Fill(phos4step+j*25,adcs[j]);
               }
               
               break;
            }
         }
         
         
         // need to set the triggered time slice in the signal shapes:
         if(m_firstEvent){
            sc = m_signalShapes->SetL1aFadcSlice(coolId.id(),(*towerItr)->hadADCPeak());
            if(sc.isFailure()){
               ATH_MSG_WARNING( "Failed to set L1aFadcSlice from HAD data to channel 0x" << std::hex << coolId << std::dec );
            }
            if(!m_signalShapes->SetChannelEtaPhiLayer(coolId.id(),eta,phi,L1CaloPprPhos4Shape::HAD)) {
               ATH_MSG_WARNING( "Failed to set ChannelEtaPhiLayer from HAD data to channel 0x" << std::hex << coolId << std::dec );
            }
         }
      }
      
      if(m_firstEvent && m_l1aFadcSlice == 0)
         m_l1aFadcSlice = (*towerItr)->emADCPeak();
      
   }// end for(towerItr)
   
   m_firstEvent = false;
   
   return StatusCode::SUCCESS;
}

StatusCode L1CaloPprPhos4ShapeMaker::finalize(){
   // Place your post-event-loop code here
   ATH_MSG_INFO("In finalize()");
   
   StatusCode sc = m_signalShapes->Finalize();
   if(sc.isFailure()){
      ATH_MSG_WARNING(" PHOS4 Signal Shapes did not Finalize correctly." );
      return sc;
   }
   
   
   // Write selected Cool ID plots to disk
   
   if(m_selectedCoolIds.size() > 0){
      char temp[50];
      std::string filename = "selectedCoodIds";
      if(m_runNumber != 0){
         sprintf(temp,"%08i",m_runNumber);
         filename += "_";
         filename += temp;
      }
      filename += ".root";
      
      std::string filePath = m_outputDirectory + filename;
      TFile outputFile(filePath.c_str(),"RECREATE");
      
      for(unsigned int i=0;i<m_selectedCoolIds.size();++i){
         std::map<unsigned int,TH2F*>::iterator itr =  m_histosForSelectedCoolIds.find(m_selectedCoolIds[i]);
         if(itr == m_histosForSelectedCoolIds.end()) continue;
         
         itr->second->Write();
         
      }
      
      outputFile.Close();
   }
   
   
   return StatusCode::SUCCESS;
}


StatusCode L1CaloPprPhos4ShapeMaker::loadAthenaObjects(){

   StatusCode sc;
   
   // Load StoreGate Tool 
   // StoreGate is your access to the data.
   sc = service("StoreGateSvc",m_storeGate);
   if(sc.isFailure()){
      ATH_MSG_ERROR("Unable to retrieve pointer to StoreGateSvc");
      return sc;
   }

   // Load Detector Store - Needed for access to L1Calo COOL ID for
   // the trigger towers.
   sc = service("DetectorStore", m_detStore);
   if (sc.isFailure()) {
      ATH_MSG_ERROR("Cannot access DetectorStore");
      return StatusCode::FAILURE;
   }
   else
   	ATH_MSG_INFO("Found detector store");
   
   sc = m_detStore->retrieve(m_caloMgr);
   if (sc.isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve CaloIdManager from DetectorStore");
      return StatusCode::FAILURE;
   }
   else
   	ATH_MSG_INFO( "CaloIdManager retrieved successfully." );
   
   m_ttOnlineIdHelper = m_caloMgr->getTTOnlineID();
   if (!m_ttOnlineIdHelper) {
      ATH_MSG_ERROR( "Could not access TTOnlineId helper" );
      return StatusCode::FAILURE;
   }
   else
   	ATH_MSG_INFO( "TTOnlineId retrieved successfully." );
	
   m_lvl1Helper = m_caloMgr->getLVL1_ID();
   if (!m_lvl1Helper) {
      ATH_MSG_ERROR( "Could not access CaloLVL1_ID helper" );
      return StatusCode::FAILURE;
   }
   else
   	ATH_MSG_INFO( "CaloLVL1_ID retrieved successfully." );
   
   sc = m_histTool.retrieve();
   if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not retrieve TrigT1CaloLWHistogramTool" );
      return StatusCode::FAILURE;
   }
   else
   	ATH_MSG_INFO( "TrigT1CaloLWHistogramTool retrieved successfully." );
   

   sc = m_l1CaloTTIdTools.retrieve();
   if(sc.isFailure()){
         ATH_MSG_WARNING( "Cannot get L1CaloTTIdTools !" );
      }
   
   IToolSvc* toolSvc;
   sc = service( "ToolSvc",toolSvc  );
   if(sc.isSuccess()){
//      IAlgTool *algtool;
      
//       sc = toolSvc->retrieveTool("L1CaloTTIdTools", algtool);
//       ATH_MSG_DEBUG("L1CaloTTIdTools retrieved");
//       if (sc.isFailure()) {
//          ATH_MSG_WARNING( " Cannot get L1CaloTTIdTools !" );
//       }
//       m_l1CaloTTIdTools = dynamic_cast<L1CaloTTIdTools*> (algtool);
      
      sc = toolSvc->retrieveTool("CaloTriggerTowerService",m_ttSvc);
      if(sc.isFailure()){
         ATH_MSG_WARNING( "Could not retrieve CaloTriggerTowerService Tool" );
      }
      
   } // End of IToolSvc initialization.
   
   
   
   return StatusCode::SUCCESS;
}

StatusCode L1CaloPprPhos4ShapeMaker::GetTriggerTowers(void){
   
   // Load in Trigger Towers. I'm not going to comment the rest, should be obvious.
   StatusCode sc = m_storeGate->retrieve(m_triggerTowers, "TriggerTowers");
   if(sc.isFailure()){
      ATH_MSG_INFO("Failed to load TriggerTowers");
      return StatusCode::FAILURE;
   }
   
   return StatusCode::SUCCESS;
}

StatusCode L1CaloPprPhos4ShapeMaker::GetRODHeader(){
   
   StatusCode sc = m_storeGate->retrieve(m_rodHeader, "RODHeaders");
   if(sc.isFailure()){
      ATH_MSG_INFO("Failed to load ROD Headers");
      return StatusCode::FAILURE;
   }
   return StatusCode::SUCCESS;
}

StatusCode L1CaloPprPhos4ShapeMaker::GetEventInfo(){
   
   StatusCode sc = m_storeGate->retrieve(m_evtInfo);
   if(sc.isFailure()){
      ATH_MSG_INFO("Failed to load Event Information");
      return StatusCode::FAILURE;
   }
   return StatusCode::SUCCESS;
}

StatusCode L1CaloPprPhos4ShapeMaker::GetDatabaseHandle(std::string folderName){
   
   StatusCode sc = m_storeGate->retrieve(m_attrList,folderName.c_str());
   //ATH_MSG_INFO( m_detStore->dump() );
   if(sc.isFailure()){
      ATH_MSG_INFO("Failed to retrieve database handle");
      return StatusCode::FAILURE;
   }
   return StatusCode::SUCCESS;
}

// Get Cool ID from Athena stuff
L1CaloCoolChannelId L1CaloPprPhos4ShapeMaker::GetCoolId(const double eta,const double phi,const unsigned int layer){
   Identifier id = m_lvl1Helper->tower_id(m_l1CaloTTIdTools->pos_neg_z(eta),
                                          layer,
                                          m_l1CaloTTIdTools->regionIndex(eta),
                                          m_l1CaloTTIdTools->etaIndex(eta),
                                          m_l1CaloTTIdTools->phiIndex(eta,phi));
   HWIdentifier hwId = m_ttSvc->createTTChannelID(id);
   L1CaloCoolChannelId coolId = m_ttSvc->createL1CoolChannelId(hwId);
   return coolId;
}

bool L1CaloPprPhos4ShapeMaker::IsCoolIdValid(const L1CaloCoolChannelId coolId){
   
   try{
      const unsigned int crate = coolId.crate();
      const unsigned int moduleSlot = coolId.module() + 5;
      const unsigned int subModule = coolId.subModule();
      const unsigned int channel = coolId.channel();
      m_ttOnlineIdHelper->channelId(crate,moduleSlot,subModule,channel);
   }
   catch(const CaloID_Exception&) {
      return false;
   }
   
   return true;
}

void L1CaloPprPhos4ShapeMaker::SetTimingRegime(std::string regime){
   
   // Regime should be "Physics", "Calib1", or "Calib2"
   if(regime != "Physics" && regime != "Calib1" && regime != "Calib2"){
      ATH_MSG_WARNING( "Timing Regime is expected to be Physics, Calib1, or Calib2, but is actually: " << regime << ", using Calib1 as default." );
      m_timingRegime = "Calib1";
   }
   
   // set local variable
   m_timingRegime = regime;
   
   // set signal shapes collection
   m_signalShapes->SetTimingRegime(m_timingRegime);
   
   // build database name for PprChanCalib folder
   
   m_pprChanCalibFolderName = "/TRIGGER/L1Calo/V1/Calibration/";
   m_pprChanCalibFolderName += m_timingRegime;
   m_pprChanCalibFolderName += "/PprChanCalib";
   
}

void L1CaloPprPhos4ShapeMaker::SetAtlasStyle(void){
   TStyle* atlasStyle = new  TStyle("atlasStyle", "ATLAS Style");

// Colors

//set the background color to white
   atlasStyle->SetFillColor(10);
   atlasStyle->SetFrameFillColor(10);
   atlasStyle->SetFrameFillStyle(0);
   atlasStyle->SetFillStyle(0);
   atlasStyle->SetCanvasColor(10);
   atlasStyle->SetPadColor(10);
   atlasStyle->SetTitleFillColor(0);
   atlasStyle->SetStatColor(10);

//dont put a colored frame around the plots
   atlasStyle->SetFrameBorderMode(0);
   atlasStyle->SetCanvasBorderMode(0);
   atlasStyle->SetPadBorderMode(0);

//use the primary color palette
   atlasStyle->SetPalette(1,0);

//set the default line color for a histogram to be black
   atlasStyle->SetHistLineColor(kBlack);

//set the default line color for a fit function to be red
   atlasStyle->SetFuncColor(kRed);

//make the axis labels black
   atlasStyle->SetLabelColor(kBlack,"xyz");

//set the default title color to be black
   atlasStyle->SetTitleColor(kBlack);
 
// Sizes

//set the margins
//atlasStyle->SetPadBottomMargin(0.2);
//atlasStyle->SetPadTopMargin(0.075);
//atlasStyle->SetPadLeftMargin(0.15);

//set axis label and title text sizes
   atlasStyle->SetLabelSize(0.03,"xyz");
   atlasStyle->SetTitleSize(0.05,"xyz");
//atlasStyle->SetTitleOffset(0.9,"x");
   atlasStyle->SetTitleOffset(1.0,"xyz");
//atlasStyle->SetStatFontSize(0.07);
//atlasStyle->SetTextSize(0.08);
   atlasStyle->SetTitleBorderSize(0);
 
   // Histogram title position
   atlasStyle->SetTitleX(0.1);
   atlasStyle->SetTitleFontSize(0.11);
   
//set line widths
   atlasStyle->SetHistLineWidth(2);
   atlasStyle->SetFrameLineWidth(2);
   atlasStyle->SetFuncWidth(2);

// Misc

//align the titles to be centered
   atlasStyle->SetTextAlign(22);

//set the number of divisions to show
//atlasStyle->SetNdivisions(506, "xy");

//turn off xy grids
//atlasStyle->SetPadGridX(0);
//atlasStyle->SetPadGridY(0);

//set the tick mark style
//atlasStyle->SetPadTickX(1);
//atlasStyle->SetPadTickY(1);
   
   // Set Pad Margins
   //atlasStyle->SetPadBottomMargin(0.);
   //atlasStyle->SetPadTopMargin   (0.);
   //atlasStyle->SetPadLeftMargin  (0.);
   //atlasStyle->SetPadRightMargin (0.);

//show the fit parameters in a box
//atlasStyle->SetOptFit(1111);

//turn off all other stats
   atlasStyle->SetOptStat(0);

//marker settings
   atlasStyle->SetMarkerStyle(8);
   atlasStyle->SetMarkerSize(0.25);
 
// Fonts
//atlasStyle->SetStatFont(42);
//atlasStyle->SetLabelFont(42,"xyz");
//atlasStyle->SetTitleFont(42,"xyz");
//atlasStyle->SetTextFont(42);

//done
   atlasStyle->cd();

//gROOT->ForceStyle();
//    gStyle->ls();
}


