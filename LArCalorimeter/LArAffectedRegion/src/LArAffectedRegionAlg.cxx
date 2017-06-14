/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define HV_NON_NOMINAL_TOLERANCE 10 // tolerance : 1V for HV
#define DEAD_HV_THRESHOLD 10 // HV <10 V="dead"

//==============
#include "EventInfo/EventIncident.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
//===================
//For HV : 
#include "GaudiKernel/IIncidentSvc.h"

#include "LArAffectedRegion/LArAffectedRegionAlg.h"
#include "CaloConditions/CaloAffectedRegionInfo.h"
#include "CaloConditions/CaloAffectedRegionInfoVec.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "LArRecConditions/ILArBadChanTool.h"
#include "LArRecConditions/LArBadChanBitPacking.h"
#include "StoreGate/StoreGateSvc.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "LArCabling/LArCablingService.h"
#include "LArIdentifier/LArOnlineID.h"
//===========for HV
#include "LArHV/LArHVManager.h"
#include "LArHV/EMBHVManager.h"
#include "LArHV/EMBHVModule.h"
#include "LArHV/EMBHVModuleConstLink.h"
#include "LArHV/EMBHVElectrode.h"
#include "LArHV/EMBHVElectrodeConstLink.h"
#include "LArHV/EMBPresamplerHVManager.h"
#include "LArHV/EMBPresamplerHVModule.h"
#include "LArHV/EMBPresamplerHVModuleConstLink.h"


#include "LArHV/EMECHVManager.h"
#include "LArHV/EMECHVModule.h"
#include "LArHV/EMECHVModuleConstLink.h"
#include "LArHV/EMECHVElectrode.h"
#include "LArHV/EMECHVElectrodeConstLink.h"
#include "LArHV/EMECPresamplerHVManager.h"
#include "LArHV/EMECPresamplerHVModule.h"
#include "LArHV/EMECPresamplerHVModuleConstLink.h"

#include "LArHV/HECHVManager.h"
#include "LArHV/HECHVModule.h"
#include "LArHV/HECHVModuleConstLink.h"
#include "LArHV/HECHVSubgap.h"

#include "LArHV/FCALHVManager.h"
#include "LArHV/FCALHVModule.h"
#include "LArHV/FCALHVModuleConstLink.h"
#include "LArHV/FCALHVLine.h"
#include "LArHV/FCALHVLineConstLink.h"
//================
// AttributeList
#include "CoralBase/Attribute.h"
#include "CoralBase/Blob.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "IOVDbMetaDataTools/IIOVDbMetaDataTool.h"

#include "LArElecCalib/ILArHVTool.h"

LArAffectedRegionAlg::LArAffectedRegionAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_BadChanTool("LArBadChanTool"),
  m_hvtool("LArHVToolMC"),
  m_onlineID(NULL),
  m_metaDataTool("IOVDbMetaDataTool"),
  m_doHV(true)
{
  declareProperty("BadChannelTool", m_BadChanTool, "public, shared BadChannelTool");
  declareProperty("ReadingFromBytestream", m_readingFromBytestream = true);
  declareProperty("doHV",m_doHV,"include HV non nominal regions info");
  m_attrListColl =0;
}
//=========================================================================================
StatusCode LArAffectedRegionAlg::initialize() {
  ATH_MSG_INFO ( "initialize()" );

  //-------------- deals with LAr Febs  
  if (m_larCablingSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR ( "Could not retrieve LArCablingService Tool" );
    return StatusCode::FAILURE;
  }
  
  ATH_CHECK( detStore()->retrieve(m_onlineID, "LArOnlineID") );
  ATH_CHECK( detStore()->retrieve(m_caloIdMgr) );
  ATH_CHECK( detStore()->retrieve(m_calodetdescrmgr) );

  //-------------
  // Do only when reading bytestream

  if (m_readingFromBytestream) {
    
    ATH_CHECK( m_BadChanTool.retrieve() );
    ATH_MSG_INFO ( m_BadChanTool.propertyName() << ": Retrieved tool " 
                   << m_BadChanTool.type() );
    
    StatusCode sc = detStore()->regFcn( &ILArBadChanTool::updateBadFebsFromDB,dynamic_cast<ILArBadChanTool*>(&(*m_BadChanTool)),&LArAffectedRegionAlg::updateMethod,this,true);
    
    if (sc.isSuccess())
      ATH_MSG_INFO ( "Registered callback for LArAffectedRegionAlg/LArAffectedRegionAlg" );
    else
      ATH_MSG_WARNING ( "Cannot register Callback function for LArAffectedRegionAlg/LArAffectedRegionAlg" );
  }
  else {     // Do only when reading ESD/AOD

    if (detStore()->contains<CondAttrListCollection>("/LAR/LArAffectedRegionInfo")) {
        const DataHandle<CondAttrListCollection> affectedRegionH;
        if (detStore()->regFcn(&LArAffectedRegionAlg::updateAffectedRegionsFromDB,
                               this,
                               affectedRegionH,
                               "/LAR/LArAffectedRegionInfo").isSuccess()) {
          ATH_MSG_DEBUG ( "Registered callback for  LArAffectedRegion " );
        }
        else {
          ATH_MSG_WARNING ( "Cannot register callback for LArAffectedRegion " );
        }
    }
    else {
      ATH_MSG_WARNING ( " no LArAffectedRegion information available from metadata " );
    }
  }

  /* for the moment, we don't do the call back
  //example : 
   std::string folderName = "/LAR/DCS/HV/BARREl/I16";
   if ((m_detStore->regFcn(&ILArHVTool::LoadCalibration,dynamic_cast<ILArHVTool*>(this), MyMethod,folderName)).isFailure()) {
      log << MSG::ERROR << "cannot register callback to " << folderName << std::endl;  
   }
   folderName = "/LAR/DCS/HV/BARREL/I8";
   if ((m_detStore->regFcn(&ILArHVTool::LoadCalibration,dynamic_cast<ILArHVTool*>(this), MyMethod2,folderName)).isFailure()) {
      log << MSG::ERROR << "cannot register callback to " << folderName << std::endl;
   }
   log << MSG::INFO << " registered callback to /LAR/DCS/HV folders " << endmsg;
  */

  // register incident handler for begin run
  IIncidentSvc* incSvc = nullptr;
  ATH_CHECK( service( "IncidentSvc", incSvc ) );
  long int priority=100;
  /*  in case one want to launch the search only one time
      bool rethrow = 0;
      bool singleShot = 1; 
      incSvc->addListener(this,"BeginRun",priority,rethrow,singleShot);
  */
  incSvc->addListener(this,"BeginRun",priority);

//--------------------------

  return StatusCode::SUCCESS;
}
//=========================================================================================
void LArAffectedRegionAlg::handle(const Incident& inc) { //for non nominal HV

  if (inc.type()!="BeginRun")
    return;
  if (!m_readingFromBytestream) // Only process if reading from bytestream
    return;

  
  ATH_MSG_INFO ( " in LArAffectedRegionAlg  beginning of run handle " );
  
  long int runNumber=-1;  
  const EventIncident* eventInc=dynamic_cast<const EventIncident*>(&inc);
  if (eventInc) { // was a bug here in previous version : !eventInc was written
    const EventInfo* evt = &eventInc->eventInfo();
    runNumber=evt->event_ID()->run_number();
  }
   
  //next 5 methods fill the current run info : m_ArrayLArAffectedRegionInfo_current_run
  if (m_doHV) {
   searchNonNominalHV_EMB();
   searchNonNominalHV_EMEC_OUTER();
   searchNonNominalHV_EMEC_INNER();
   searchNonNominalHV_HEC();
   searchNonNominalHV_FCAL();
  }

  int flag_At_Least_New_Problem_In_Current_Run=0; //flag to compare with global(=all runs) AffectedProblems. Specific case of first run (for which global run will be yet empty) is taken into consideration in the algorithm
  
  //next algorithm is : search for a new problem in the global (=all runs) list of AffectedRegion and update the MetaData afterwards if at least one new problem has been found 

  for (unsigned int i=0;i<m_ArrayLArAffectedRegionInfo_current_run.size();++i) {     // loop over m_ArrayLArAffectedRegionInfo_current_run
    if (getOccurenceProblem(m_ArrayLArAffectedRegionInfo_global,m_ArrayLArAffectedRegionInfo_current_run[i].get_eta_min(),m_ArrayLArAffectedRegionInfo_current_run[i].get_eta_max(),m_ArrayLArAffectedRegionInfo_current_run[i].get_phi_min(),m_ArrayLArAffectedRegionInfo_current_run[i].get_phi_max(),m_ArrayLArAffectedRegionInfo_current_run[i].get_layer_min(),m_ArrayLArAffectedRegionInfo_current_run[i].get_layer_max(),m_ArrayLArAffectedRegionInfo_current_run[i].get_problem(),m_ArrayLArAffectedRegionInfo_current_run[i].get_rate())) {
      
            //std::cout << "found the asked AffectedRegion in the global list of AffectedRegion" << std::endl;
            //std::cout << "The AffectedRegion found is : " << std::endl;
            //m_ArrayLArAffectedRegionInfo_current_run[i].PrintInfo();
    }
    else { // found a new problem that was no in the global list : add it and update the MetaData
      //std::cout << "FOUND A NEW PROBLEM : " << std::endl;
      //m_ArrayLArAffectedRegionInfo_current_run[i].PrintInfo();
      flag_At_Least_New_Problem_In_Current_Run=1;
      m_ArrayLArAffectedRegionInfo_global.push_back(m_ArrayLArAffectedRegionInfo_current_run[i]); //add the new problem of current run to the global list
    }
  } //end loop on AffectedRegion


  // Fill AttributeListCollection and add to MetaData store
  coral::AttributeListSpecification* attrSpec = new coral::AttributeListSpecification();
  attrSpec->extend("eta_min", "float");
  attrSpec->extend("eta_max", "float");
  attrSpec->extend("phi_min", "float");
  attrSpec->extend("phi_max", "float");
  attrSpec->extend("layer_min", "int");
  attrSpec->extend("layer_max", "int");
  attrSpec->extend("problem", "int");
  
  if (!m_attrListColl || (m_attrListColl && flag_At_Least_New_Problem_In_Current_Run) ) { // first run or new run with new problems

    m_attrListColl = new CondAttrListCollection(true);   // information that will be sent to the MetaData
    
    for (unsigned int i=0;i<m_ArrayLArAffectedRegionInfo_global.size();++i) {
      CaloAffectedRegionInfo& info = m_ArrayLArAffectedRegionInfo_global[i];
      coral::AttributeList attrList(*attrSpec,true);
      attrList["eta_min"].setValue(info.get_eta_min());
      attrList["eta_max"].setValue(info.get_eta_max());
      attrList["phi_min"].setValue(info.get_phi_min());
      attrList["phi_max"].setValue(info.get_phi_max());
      attrList["layer_min"].setValue(info.get_layer_min());
      attrList["layer_max"].setValue(info.get_layer_max());
      attrList["problem"].setValue(info.get_problem());
      m_attrListColl->add(i, attrList);  // channel number, payload
    }
    
    m_attrListColl->addNewStart(IOVTime(runNumber, 0));       // start run, LB

    attrSpec->release(); //Decrease ref-count, we don't need that guy any more

    // Register folder in the IOV Db MetaData
    if (StatusCode::SUCCESS != m_metaDataTool->registerFolder("/LAR/LArAffectedRegionInfo")) {      
      ATH_MSG_ERROR ( "fillMetaData: Unable to register folder for /LAR/LArAffectedRegionInfo with meta data tool " );
      //return StatusCode::FAILURE;
      throw GaudiException( "Unable to get EventInfo from BeginRun incident", "TagInfoMgr::handle", StatusCode::FAILURE );
    }
    // Add payload
    if (StatusCode::SUCCESS != m_metaDataTool->addPayload("/LAR/LArAffectedRegionInfo", m_attrListColl)) {      
      ATH_MSG_ERROR ( "fillMetaData: Unable to register folder for /LAR/LArAffectedRegionInfo with meta data tool " );
      throw GaudiException( "fillMetaData: Unable to register folder for /LAR/LArAffectedRegionInfo with meta data tool ", "TagInfoMgr::handle", StatusCode::FAILURE );
    }

    // record also in the detector store
    CaloAffectedRegionInfoVec* affRegVec=0;
    if (detStore()->contains<CaloAffectedRegionInfoVec>("LArAffectedRegion")) {
      if (StatusCode::SUCCESS != detStore()->retrieve(affRegVec,"LArAffectedRegion")) {
        ATH_MSG_ERROR ( " Cannot retrieve LArAffectedRegion " );
        return;
      }
    }
    else {
         affRegVec = new CaloAffectedRegionInfoVec();
         if (StatusCode::SUCCESS != detStore()->record(affRegVec,"LArAffectedRegion")) {
           ATH_MSG_ERROR ( " Cannot record LArAffectedRegion " );
           return;
         }
    }
    affRegVec->clear();
    affRegVec->reserve(m_ArrayLArAffectedRegionInfo_global.size());
    for (unsigned int i=0;i<m_ArrayLArAffectedRegionInfo_global.size();++i) {
        CaloAffectedRegionInfo& info = m_ArrayLArAffectedRegionInfo_global[i];
        CaloAffectedRegionInfo region(info.get_eta_min(),info.get_eta_max(),info.get_phi_min(),info.get_phi_max(),info.get_layer_min(),info.get_layer_max(),(CaloAffectedRegionInfo::type_problem)(info.get_problem()));
        affRegVec->push_back(region);
    }

  } //end first run or new run with new problems

  m_attrListColl->addNewStop(IOVTime(runNumber + 1, 0));  // create (or extend) IOV to the current run
}
//=========================================================================================
void LArAffectedRegionAlg::searchNonNominalHV_EMB() {  // deals with LAr HV, EMBarrel
  const LArHVManager *manager = NULL;

  //std::cout << " in HV_EMB " << std::endl;
  if (detStore()->retrieve(manager)==StatusCode::SUCCESS) {
    
// accordion calorimeter
    float HVnominal = HV_nominal("EMB",0.);
    const EMBHVManager* hvManager_EMB=manager->getEMBHVManager();
    
    for (unsigned int iSide=hvManager_EMB->beginSideIndex();iSide<hvManager_EMB->endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMB->beginPhiIndex();iPhi<hvManager_EMB->endPhiIndex();iPhi++) {
	for (unsigned int iSector=hvManager_EMB->beginSectorIndex();iSector<hvManager_EMB->endSectorIndex();iSector++) {
	  for (unsigned int iEta=hvManager_EMB->beginEtaIndex();iEta<hvManager_EMB->endEtaIndex();iEta++) { //0 to 7
	    EMBHVModuleConstLink hvMod = hvManager_EMB->getHVModule(iSide,iEta,iPhi,iSector);

            float eta_min=hvMod->getEtaMin();
            float eta_max=hvMod->getEtaMax();

            //std::cout << "iSide,iPhi,iSector,iEta " << iSide << " " << iPhi << " " << iSector << " " << iEta << std::endl;
            //std::cout << " eta_min , eta_max " << eta_min << " " << eta_max << std::endl;

	    float phi_min=+30.,phi_max=-30.;
	    
	    bool are_previous_HV_affected=false;
            bool are_previous_HV_dead=false;
	    for (unsigned int ielec=0;ielec<32;ielec++) { //use hvMod->getNumElectrodes when bug is corrected
	      EMBHVElectrodeConstLink electrode = hvMod->getElectrode(ielec);

	      double hv[2];
	      for (unsigned int iGap=0;iGap<2;iGap++) { // EMB : 2, TRY TO FIND AUTOMATICALLY NB OF GAPS
		hv[iGap]=electrode->voltage(iGap);
	      } //end for iGap

              //std::cout << " electrode HV " << ielec << " " << electrode->getPhi() << " "<< hv[0] << " " << hv[1] << " ";

	      //take decisions according to all the gaps HV :
              bool isDead=false;
              if (fabs(hv[0])<DEAD_HV_THRESHOLD && fabs(hv[1])<DEAD_HV_THRESHOLD) isDead=true;
              bool isAffected=false;
              if ( !isDead && ((fabs(hv[0]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || (fabs(hv[1]-HVnominal)>HV_NON_NOMINAL_TOLERANCE))) isAffected=true;
              //std::cout << " dead/affected " << isDead << " " << isAffected << std::endl;
          
              // end previous dead region
              if (are_previous_HV_dead && !isDead) {
                 are_previous_HV_dead=false;
                 //std::cout << " -- end dead region " << eta_min << " " << eta_max << " " <<phi_min << " " << phi_max << std::endl;
                 CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,1,3,CaloAffectedRegionInfo::HVdead);
                 m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);
              }

              // end previous affected region
              if (are_previous_HV_affected && !isAffected) {
                 are_previous_HV_affected=false;
                 //std::cout << " -- end affected region " << eta_min << " " << eta_max << " " <<phi_min << " " << phi_max << std::endl;
                 CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,1,3,CaloAffectedRegionInfo::HVaffected);
                 m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);
              }

              if (isDead) {
                 if (!are_previous_HV_dead) {
                  phi_min=CaloPhiRange::fix(electrode->getPhi()-1e-4);
                  phi_max=CaloPhiRange::fix(electrode->getPhi()+1e-4);
                  //std::cout << " -- start dead region " << eta_min << " " << eta_max << " " << phi_min << " " <<phi_max << std::endl;
                  are_previous_HV_dead = true;
                 }
                 else {
                  extendPhiRegion(electrode->getPhi(),phi_min,phi_max);
                  //std::cout << " extend dead region " << phi_min << " " << phi_max << std::endl;
                 }
              }

              if (isAffected) {
                 if (!are_previous_HV_affected) {
                  phi_min=CaloPhiRange::fix(electrode->getPhi()-1e-4);
                  phi_max=CaloPhiRange::fix(electrode->getPhi()+1e-4);
                  //std::cout << " -- start affected region " << eta_min << " " << eta_max << " " << phi_min << " " <<phi_max << std::endl;
                  are_previous_HV_affected = true;
                 }
                 else {
                  extendPhiRegion(electrode->getPhi(),phi_min,phi_max);
                  //std::cout << " extend affected region " << phi_min << " " << phi_max << std::endl;
                 }
              }

	    } // end for ielec
	    
	    if (are_previous_HV_affected) { //in case a non nominal exists, stores it if we finish the 32 electrodes (because else the are_previous_HV_affected will be reinitialized for the next 32 electrodes serie )
              //std::cout << "  -- finish affected region after electrode loop " << eta_min << " " << eta_max << " " << phi_min << " " <<phi_max << std::endl;
	      CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,1,3,CaloAffectedRegionInfo::HVaffected);
	      m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);	      
	    }
            if (are_previous_HV_dead) {
              //std::cout << "  -- finish dead region after electrode loop " << eta_min << " " << eta_max << " " << phi_min << " " <<phi_max << std::endl;
              CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,1,3,CaloAffectedRegionInfo::HVdead);
              m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);
            }
	  } // end for iEta
	} // end for iSector
      } // end for iPhi
    } //end for iSide

// barrel presampler
    //std::cout << " start loop over EMBPS " << std::endl;
    const EMBPresamplerHVManager* hvManager_EMBPS=manager->getEMBPresamplerHVManager();
    HVnominal = HV_nominal("EMBPS",0.);
    for (unsigned int iSide=hvManager_EMBPS->beginSideIndex();iSide<hvManager_EMBPS->endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMBPS->beginPhiIndex();iPhi<hvManager_EMBPS->endPhiIndex();iPhi++) {
          for (unsigned int iEta=hvManager_EMBPS->beginEtaIndex();iEta<hvManager_EMBPS->endEtaIndex();iEta++) { //0 to 7
            EMBPresamplerHVModuleConstLink hvMod = hvManager_EMBPS->getHVModule(iSide,iEta,iPhi);
            //std::cout << "iSide,iPhi,iEta " << iSide << " " << iPhi << " " << iEta << std::endl;
            double hv[2];
            for (int iGap=0;iGap<2;iGap++) {
             hv[iGap] =  hvMod->voltage(iGap);
            }
            //std::cout << "  HV " <<  hv[0] << " " << hv[1] << " ";
            float eta_min=hvMod->getEtaMin();
            float eta_max=hvMod->getEtaMax();
            float phi_min=CaloPhiRange::fix(hvMod->getPhiMin());
            float phi_max=CaloPhiRange::fix(hvMod->getPhiMax());
            //std::cout << "  etamin,etamax,phimin,phimax " << eta_min << " " << eta_max << " " << phi_min << " " << phi_max ;

            //take decisions according to all the gaps HV :
            bool isDead=false;
            if (fabs(hv[0])<DEAD_HV_THRESHOLD && fabs(hv[1])<DEAD_HV_THRESHOLD) isDead=true;
            bool isAffected=false;
            if ( !isDead && ((fabs(hv[0]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || (fabs(hv[1]-HVnominal)>HV_NON_NOMINAL_TOLERANCE))) isAffected=true;
            //std::cout << " dead/affected " << isDead << " " << isAffected << std::endl;

            if (isDead) {
              CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,0,0,CaloAffectedRegionInfo::HVdead);
              m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);             
            }
            if (isAffected) {
              CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,0,0,CaloAffectedRegionInfo::HVaffected);
              m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);
            }
          }       // loop over iEta EMBPS
      }        // loop over iphi EMBPS
    }    // lop over EMBPS side




  }
}
//=========================================================================================
void LArAffectedRegionAlg::searchNonNominalHV_EMEC_OUTER() { // deals with LAr HV, EM EndCap OUTER
  const LArHVManager *manager = NULL;

  //std::cout << " start HV_EMEC_OUTER " << std::endl;
  if (detStore()->retrieve(manager)==StatusCode::SUCCESS) {
      
    const EMECHVManager* hvManager_EMEC=manager->getEMECHVManager(EMECHVModule::OUTER);

    for (unsigned int iSide=hvManager_EMEC->beginSideIndex();iSide<hvManager_EMEC->endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMEC->beginPhiIndex();iPhi<hvManager_EMEC->endPhiIndex();iPhi++) {
	for (unsigned int iSector=hvManager_EMEC->beginSectorIndex();iSector<hvManager_EMEC->endSectorIndex();iSector++) {
	  for (unsigned int iEta=hvManager_EMEC->beginEtaIndex();iEta<hvManager_EMEC->endEtaIndex();iEta++) {
	    EMECHVModuleConstLink hvMod=hvManager_EMEC->getHVModule(iSide,iEta,iPhi,iSector);

            float etaMod = 0.5*(fabs(hvMod->getEtaMin())+fabs(hvMod->getEtaMax()));
            float HVnominal = HV_nominal("EMEC",etaMod);

            float eta_min=hvMod->getEtaMin();
            float eta_max=hvMod->getEtaMax();

            //std::cout << "iSide,iPhi,iSector,iEta " << iSide << " " << iPhi << " " << iSector << " " << iEta << std::endl;
            //std::cout << " eta_min , eta_max " << eta_min << " " << eta_max << std::endl;


	    float phi_min=+30.,phi_max=-30.;

	    bool are_previous_HV_affected=false;
            bool are_previous_HV_dead=false;
            //std::cout << " number of electrodes " << hvMod->getNumElectrodes() << std::endl;
	    for (unsigned int ielec=0;ielec<hvMod->getNumElectrodes();ielec++) { //use hvMod->getNumElectrodes when bug is corrected
	      EMECHVElectrodeConstLink electrode = hvMod->getElectrode(ielec);

	      double hv[2];
	      for (unsigned int iGap=0;iGap<2;iGap++) { //EMEC : 2 gaps, TRY TO FIND AUTOMATICALLY NB OF GAPS
		hv[iGap]=electrode->voltage(iGap);
	      } //end for iGap

	      //------------------
	      //take decisions according to all the gaps HV :
              //std::cout << " electrode HV " << ielec << " " << electrode->getPhi() << " "<< hv[0] << " " << hv[1] << " " ;

	      //take decisions according to all the gaps HV :
              bool isDead=false;
              if (fabs(hv[0])<DEAD_HV_THRESHOLD && fabs(hv[1])<DEAD_HV_THRESHOLD) isDead=true;
              bool isAffected=false;
              if ( !isDead && ((fabs(hv[0]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || (fabs(hv[1]-HVnominal)>HV_NON_NOMINAL_TOLERANCE))) isAffected=true;
              //std::cout << " isDead/isAffected " << isDead << " " << isAffected << std::endl;
          
              // end previous dead region
              if (are_previous_HV_dead && !isDead) {
                 are_previous_HV_dead=false;
                 //std::cout << " -- end dead region " << eta_min << " " << eta_max << " " <<phi_min << " " << phi_max << std::endl;
                 CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,5,7,CaloAffectedRegionInfo::HVdead);
                 m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);
              }

              // end previous affected region
              if (are_previous_HV_affected && !isAffected) {
                 are_previous_HV_affected=false;
                 //std::cout << " -- end affected region " << eta_min << " " << eta_max << " " <<phi_min << " " << phi_max << std::endl;
                 CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,5,7,CaloAffectedRegionInfo::HVaffected);
                 m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);
              }

              if (isDead) {
                 if (!are_previous_HV_dead) {
                  phi_min=CaloPhiRange::fix(electrode->getPhi()-1e-4);
                  phi_max=CaloPhiRange::fix(electrode->getPhi()+1e-4);
                  are_previous_HV_dead = true;
                  //std::cout << " -- start dead region " << eta_min << " " << eta_max << " " << phi_min << " " <<phi_max << std::endl;
                 }
                 else {
                  extendPhiRegion(electrode->getPhi(),phi_min,phi_max);
                  //std::cout << "  extend affected region " << phi_min << " " << phi_max << std::endl;
                 }
              }

              if (isAffected) {
                 if (!are_previous_HV_affected) {
                  phi_min=CaloPhiRange::fix(electrode->getPhi()-1e-4);
                  phi_max=CaloPhiRange::fix(electrode->getPhi()+1e-4);
                  are_previous_HV_affected = true;
                  //std::cout << " -- start affected region " << eta_min << " " << eta_max << " " << phi_min << " " <<phi_max << std::endl;
                 }
                 else {
                  extendPhiRegion(electrode->getPhi(),phi_min,phi_max);
                  //std::cout << " extend affected region " << phi_min << " " << phi_max << std::endl;
                 }
              }

	    } // end for ielec
	    
	    if (are_previous_HV_affected) { //in case a non nominal exists, stores it if we finish the 32 electrodes (because else the are_previous_HV_affected will be reinitialized for the next 32 electrodes serie )
              //std::cout << "   - finih affected region after electrode loop " << eta_min << " " << eta_max << " " << phi_max << " " <<phi_max << std::endl;
	      CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,5,7,CaloAffectedRegionInfo::HVaffected);
	      m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);	      
	    }
            if (are_previous_HV_dead) {
              //std::cout << " -- finish dead region after electrode loop " << eta_min << " " << eta_max << " " << phi_max << " " <<phi_max << std::endl;
              CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,5,7,CaloAffectedRegionInfo::HVdead);
              m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);
            }

	  } // end for iEta
	} // end for iSector
      } // end for iPhi
    } //end for iSide

// endcap presampler
    //std::cout << " start loop over EMECPS " << std::endl;
    const EMECPresamplerHVManager* hvManager_EMECPS=manager->getEMECPresamplerHVManager();
    float HVnominal = HV_nominal("EMECPS",0.);
    for (unsigned int iSide=hvManager_EMECPS->beginSideIndex();iSide<hvManager_EMECPS->endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMECPS->beginPhiIndex();iPhi<hvManager_EMECPS->endPhiIndex();iPhi++) {
            EMECPresamplerHVModuleConstLink hvMod = hvManager_EMECPS->getHVModule(iSide,iPhi);
            //std::cout << "iSide,iPhi" << iSide << " " << iPhi << std::endl;
            double hv[2];
            for (int iGap=0;iGap<2;iGap++) {
             hv[iGap] =  fabs(hvMod->voltage(iGap));
            }
            //std::cout << "  HV " <<  hv[0] << " " << hv[1] << " ";
            float eta_min=hvMod->getEtaMin(); 
            float eta_max=hvMod->getEtaMax();
            float phi_min=CaloPhiRange::fix(hvMod->getPhiMin());
            float phi_max=CaloPhiRange::fix(hvMod->getPhiMax());
            //std::cout << "  etamin,etamax,phimin,phimax " << eta_min << " " << eta_max << " " << phi_min << " " << phi_max ;

            //take decisions according to all the gaps HV :
            bool isDead=false;
            if (fabs(hv[0])<DEAD_HV_THRESHOLD && fabs(hv[1])<DEAD_HV_THRESHOLD) isDead=true;
            bool isAffected=false;
            if ( !isDead && ((fabs(hv[0]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || (fabs(hv[1]-HVnominal)>HV_NON_NOMINAL_TOLERANCE))) isAffected=true;
            //std::cout << " dead/affected " << isDead << " " << isAffected << std::endl;

            if (isDead) {
              CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,4,4,CaloAffectedRegionInfo::HVdead);
              m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);             
            }
            if (isAffected) {
              CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,4,4,CaloAffectedRegionInfo::HVaffected);
              m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);
            }
      }        // loop over iphi EMECPS
    }    // lop over EMECPS side
    

  }
}
//=========================================================================================
void LArAffectedRegionAlg::searchNonNominalHV_EMEC_INNER() { // deals with LAr HV, EM EndCap INNER
  const LArHVManager *manager = NULL;

  //std::cout << " start loop over EMEC_INNER " << std::endl;
  if (detStore()->retrieve(manager)==StatusCode::SUCCESS) {

    const EMECHVManager* hvManager_EMEC=manager->getEMECHVManager(EMECHVModule::INNER);

    for (unsigned int iSide=hvManager_EMEC->beginSideIndex();iSide<hvManager_EMEC->endSideIndex();iSide++) { // loop over HV modules
      for (unsigned int iPhi=hvManager_EMEC->beginPhiIndex();iPhi<hvManager_EMEC->endPhiIndex();iPhi++) {
	for (unsigned int iSector=hvManager_EMEC->beginSectorIndex();iSector<hvManager_EMEC->endSectorIndex();iSector++) {
	  for (unsigned int iEta=hvManager_EMEC->beginEtaIndex();iEta<hvManager_EMEC->endEtaIndex();iEta++) {
	    EMECHVModuleConstLink hvMod=hvManager_EMEC->getHVModule(iSide,iEta,iPhi,iSector);

            float etaMod = 0.5*(fabs(hvMod->getEtaMin())+fabs(hvMod->getEtaMax()));
            float HVnominal = HV_nominal("EMEC",etaMod);

            float eta_min = hvMod->getEtaMin();
            float eta_max = hvMod->getEtaMax();

            //std::cout << "iSide,iPhi,iSector,iEta " << iSide << " " << iPhi << " " << iSector << " " << iEta << std::endl;
            //std::cout << " eta_min , eta_max " << eta_min << " " << eta_max << std::endl;

	    float phi_min=+30.,phi_max=-30.;

	    bool are_previous_HV_affected=0;
	    bool are_previous_HV_dead=0;
	    //	    std::cout << "There are " << hvMod->getNumElectrodes() << " electrodes" << std::endl;
	    for (unsigned int ielec=0;ielec<hvMod->getNumElectrodes();ielec++) { //use hvMod->getNumElectrodes when bug is corrected
	      EMECHVElectrodeConstLink electrode = hvMod->getElectrode(ielec);

	      double hv[2];
	      for (unsigned int iGap=0;iGap<2;iGap++) { //EMEC : 2 gaps, TRY TO FIND AUTOMATICALLY NB OF GAPS
		hv[iGap]=electrode->voltage(iGap);
	      } //end for iGap

	      //------------------
	      //------------------
	      //take decisions according to all the gaps HV :
              //std::cout << " electrode HV " << ielec << " " << electrode->getPhi() << " "<< hv[0] << " " << hv[1] << " ";

	      //take decisions according to all the gaps HV :
              bool isDead=false;
              if (fabs(hv[0])<DEAD_HV_THRESHOLD && fabs(hv[1])<DEAD_HV_THRESHOLD) isDead=true;
              bool isAffected=false;
              if ( !isDead && ((fabs(hv[0]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || (fabs(hv[1]-HVnominal)>HV_NON_NOMINAL_TOLERANCE))) isAffected=true;
              //std::cout << " isDead, isAffected " << isDead << " " << isAffected << std::endl;
          
              // end previous dead region
              if (are_previous_HV_dead && !isDead) {
                 are_previous_HV_dead=false;
                 //std::cout << " -- end dead region " << eta_min << " " << eta_max << " " <<phi_min << " " << phi_max << std::endl;
                 CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,5,7,CaloAffectedRegionInfo::HVdead);
                 m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);
              }

              // end previous affected region
              if (are_previous_HV_affected && !isAffected) {
                 are_previous_HV_affected=false;
                 //std::cout << " -- end affected region " << eta_min << " " << eta_max << " " <<phi_min << " " << phi_max << std::endl;
                 CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,5,7,CaloAffectedRegionInfo::HVaffected);
                 m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);
              }

              if (isDead) {
                 if (!are_previous_HV_dead) {
                  phi_min=CaloPhiRange::fix(electrode->getPhi()-1e-4);
                  phi_max=CaloPhiRange::fix(electrode->getPhi()+1e-4);
                  //std::cout << " -- start dead region " << phi_min << " " << phi_max << std::endl;
                  are_previous_HV_dead = true;
                 }
                 else {
                  extendPhiRegion(electrode->getPhi(),phi_min,phi_max);
                  //std::cout << "  extend dead region " << phi_min << " " << phi_max << std::endl;
                 }
              }

              if (isAffected) {
                 if (!are_previous_HV_affected) {
                  phi_min=CaloPhiRange::fix(electrode->getPhi()-1e-4);
                  phi_max=CaloPhiRange::fix(electrode->getPhi()+1e-4);
                  are_previous_HV_affected = true;
                  //std::cout << " -- start affected region " << phi_min << " " << phi_max << std::endl;
                 }
                 else {
                   extendPhiRegion(electrode->getPhi(),phi_min,phi_max);
                   //std::cout << "  extend affected region " << phi_min << " " << phi_max << std::endl;
                 }
              }

	    } // end for ielec
	    
	    if (are_previous_HV_affected) { //in case a non nominal exists, stores it if we finish the 32 electrodes (because else the are_previous_HV_affected will be reinitialized for the next 32 electrodes serie )
              //std::cout << "   - finih affected region after electrode loop " << eta_min << " " << eta_max << " " << phi_max << " " <<phi_max << std::endl;
	      CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,5,7,CaloAffectedRegionInfo::HVaffected);
	      m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);	      
	    }
            if (are_previous_HV_dead) {
              //std::cout << "    - end dead region after electrode loop " << eta_min << " " << eta_max << " " << phi_max << " " <<phi_max << std::endl;
              CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,5,7,CaloAffectedRegionInfo::HVdead);
              m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);
            }
	  } // end for iEta
	} // end for iSector
      } // end for iPhi
    } //end for iSide
  }
}
//=========================================================================================
void LArAffectedRegionAlg::searchNonNominalHV_HEC() { // deals with LAr HV, HEC
  
 // std::cout << " in HEC " << std::endl;
  const LArHVManager *manager = NULL;
  float etamax_layer[4]={3.3,3.1,3.1,3.3};
  float etamin_layer[4]={1.5,1.5,1.6,1.7};
 
  float HVnominal = HV_nominal("HEC",0.);


  if (detStore()->retrieve(manager)==StatusCode::SUCCESS) {
    //no sector index, no eta index==>sampling index
  
    const HECHVManager* hvManager_HEC=manager->getHECHVManager();
    
    for (unsigned int iSide=hvManager_HEC->beginSideIndex();iSide<hvManager_HEC->endSideIndex();iSide++) { // loop over HV modules      
      for (unsigned int iPhi=hvManager_HEC->beginPhiIndex();iPhi<hvManager_HEC->endPhiIndex();iPhi++) {
	for (unsigned int iSampling=hvManager_HEC->beginSamplingIndex();iSampling<hvManager_HEC->endSamplingIndex();iSampling++) {
          float eta_min,eta_max;
          if (iSide==1) {
           eta_min = etamin_layer[iSampling];
           eta_max = etamax_layer[iSampling];
          } else {
           eta_min = -1.*etamax_layer[iSampling];
           eta_max = -1.*etamin_layer[iSampling];
         }

	  HECHVModuleConstLink hvMod = hvManager_HEC->getHVModule(iSide,iPhi,iSampling);
          //std::cout << " iSide,iPhi,iSampling " << iSide << " " << iPhi << " " << iSampling << std::endl;

	  double hv[4] = {0};
	  for (unsigned int iGap=0;iGap<hvMod->getNumSubgaps();iGap++) {//HEC : 4 gaps, TRY TO FIND AUTOMATICALLY NB OF GAPS
	    HECHVSubgapConstLink subgap=hvMod->getSubgap(iGap);
	    if(iGap<4) hv[iGap]=subgap->voltage();
	  }// end for iGap

	  //------------------
          //std::cout << " HV values " << hv[0] << " " << hv[1] << " " << hv[2] << " " << hv[3] << " ";

          //take decisions according to all the gaps HV :
          bool isDead=false;
          if (fabs(hv[0])<DEAD_HV_THRESHOLD && fabs(hv[1])<DEAD_HV_THRESHOLD  && fabs(hv[2])<DEAD_HV_THRESHOLD && fabs(hv[3])<DEAD_HV_THRESHOLD) isDead=true;
          bool isAffected=false;
          if ( !isDead && ((fabs(hv[0]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || (fabs(hv[1]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || 
                           (fabs(hv[2]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || (fabs(hv[3]-HVnominal)>HV_NON_NOMINAL_TOLERANCE)) ) isAffected=true;
          //std::cout << " isDead/isAffected " << isDead << " " << isAffected << std::endl;

          float phiMin = CaloPhiRange::fix(hvMod->getPhiMin());
          float phiMax = CaloPhiRange::fix(hvMod->getPhiMax());


	  if (isDead) { //stores it, DEAD means all hvs < threshold
            //std::cout << " new dead region " << eta_min << " " << eta_max << " " << phiMin << " " << phiMax << " layer " << 8+iSampling << std::endl;
	    CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phiMin,phiMax,8+iSampling,8+iSampling,CaloAffectedRegionInfo::HVdead);
	    m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);
	  }
          if (isAffected) {
            //std::cout << " new affected region " << eta_min << " " << eta_max << " " << phiMin << " " << phiMax << " layer " << 8+iSampling << std::endl;
            CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phiMin,phiMax,8+iSampling,8+iSampling,CaloAffectedRegionInfo::HVaffected);
            m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);
          }

	} //end for iSampling
      }//end for iPhi
    }//end for iSide
  }
}
//=========================================================================================
void LArAffectedRegionAlg::searchNonNominalHV_FCAL() { // deals with LAr HV, FCAL

  //std::cout << " inFCAL " << std::endl;
  const LArHVManager *manager = NULL;
  if (detStore()->retrieve(manager)==StatusCode::SUCCESS) {  
    
    const FCALHVManager *hvManager_FCAL=manager->getFCALHVManager();
    
    for (unsigned int iSide=hvManager_FCAL->beginSideIndex();iSide<hvManager_FCAL->endSideIndex();iSide++) { // loop over HV modules
      float eta_min=3.1,eta_max=4.9;
      if (iSide==0) {
         eta_min=-4.9;
         eta_max=-3.1;
      }
      for (unsigned int iSampling=hvManager_FCAL->beginSamplingIndex();iSampling<hvManager_FCAL->endSamplingIndex();iSampling++) {
        float HVnominal = HV_nominal("FCAL",(float)(iSampling));
	for (unsigned int iSector=hvManager_FCAL->beginSectorIndex(iSampling);iSector<hvManager_FCAL->endSectorIndex(iSampling);iSector++) {

	  FCALHVModuleConstLink hvMod = hvManager_FCAL->getHVModule(iSide,iSector,iSampling);
          //std::cout << " FCAL HVModule side,sampling,sector " << iSide << " " << iSampling << " " << iSector << std::endl;
          //std::cout << "   HV nominal " << HVnominal << std::endl;
 
          float dphi=CaloPhiRange::twopi()/16;
          if (iSampling==1) dphi=CaloPhiRange::twopi()/8.;
          if (iSampling==2) dphi=CaloPhiRange::twopi()/4.;
          float phi_min = ((float)(iSector))*dphi;
          phi_min =   CaloPhiRange::fix(phi_min);
          float phi_max = CaloPhiRange::fix(dphi+phi_min);
        
          //std::cout << " eta_min,eta_max,phi_min,phi_max " << eta_min << " " << eta_max << " " << phi_min << " " << phi_max << std::endl;
	  
          //std::cout << "   number of lines " << hvMod->getNumHVLines() << std::endl;
          float hv[4] = {0};
	  for (unsigned int iLine=0;iLine<hvMod->getNumHVLines();iLine++) {
	    FCALHVLineConstLink hvline = hvMod->getHVLine(iLine);
	    if (iLine<4) hv[iLine] = hvline->voltage();
          }
          //std::cout << " HV values " << hv[0] << " " << hv[1] << " " << hv[2] << " " << hv[3] <<  " ";
	  //------------------
	  //take decisions according to all the gaps HV :
          bool isDead=false;
          if (fabs(hv[0]) < DEAD_HV_THRESHOLD && fabs(hv[1]) < DEAD_HV_THRESHOLD && fabs(hv[2]) < DEAD_HV_THRESHOLD && fabs(hv[3]) < DEAD_HV_THRESHOLD) isDead=true;
          bool isAffected=false;
          if ( !isDead && ((fabs(hv[0]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || (fabs(hv[1]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) ||
                           (fabs(hv[2]-HVnominal)>HV_NON_NOMINAL_TOLERANCE) || (fabs(hv[3]-HVnominal)>HV_NON_NOMINAL_TOLERANCE)) ) isAffected=true;
          //std::cout << " isDead/isAffected " << isDead << " " << isAffected << std::endl;

	    
	  if (isAffected) { 
            //std::cout << " -- store affected region " << std::endl;
	    CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,21+iSampling,21+iSampling,CaloAffectedRegionInfo::HVaffected);
	    m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);	      
	  }
          if (isDead) { 
            //std::cout << " -- store dead region " << std::endl;
            CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,21+iSampling,21+iSampling,CaloAffectedRegionInfo::HVdead);
            m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);
          }
	} //end iSector
      } //end iSampling
    }// end iSide
  } //end status success
}
//=========================================================================================
StatusCode LArAffectedRegionAlg::updateMethod(IOVSVC_CALLBACK_ARGS) { //store informations on the missing Febs w/ range of eta, phi, layer
  ATH_MSG_INFO ( "updateMethod()" );
  
  std::vector<HWIdentifier>::const_iterator febid_it=m_onlineID->feb_begin();
  std::vector<HWIdentifier>::const_iterator febid_end_it=m_onlineID->feb_end();

  for (;febid_it!=febid_end_it;++febid_it) {
      
    bool IsMissingFeb=m_BadChanTool->febMissing(*febid_it);
    
    if (IsMissingFeb) {       //flag for special treatment for FEB that has non contiguous eta regions, so we have to separate them
      bool is_normal=0; //FEB without discontinuity
      bool is_additive1=0; //FEB with a discontinuity
      
      int layer_min=+30,layer_max=-30;
      float eta_min=+30,eta_max=-30;
      float phi_min=+30,phi_max=-30;
      
      int layer_min_additive1=+30,layer_max_additive1=-30;
      float eta_min_additive1=+30,eta_max_additive1=-30;
      float phi_min_additive1=+30,phi_max_additive1=-30;
      
      int chans_per_feb = m_onlineID->channelInSlotMax(*febid_it);
      
      for (int icha=0;icha<chans_per_feb;icha++) {   //loop on each channel of the relevant FEB
	HWIdentifier channelId=m_onlineID->channel_Id(*febid_it,icha);

	if (m_larCablingSvc->isOnlineConnected(channelId)) {
	  Identifier offlineId=m_larCablingSvc->cnvToIdentifier(channelId);
	  const CaloDetDescrElement* caloddElement=m_calodetdescrmgr->get_element(offlineId);
	  
	  CaloCell_ID::CaloSample current_layer=caloddElement->getSampling(); // calo layer
	  float current_eta=caloddElement->eta();
	  float current_eta_low=caloddElement->eta()-0.5*caloddElement->deta();
	  float current_eta_high=caloddElement->eta()+0.5*caloddElement->deta();
	  float current_phi_low=caloddElement->phi()-0.5*caloddElement->dphi();
	  float current_phi_high=caloddElement->phi()+0.5*caloddElement->dphi();
	  
	  if (caloddElement->is_lar_em_barrel() && caloddElement->getLayer()==3 && fabs(current_eta)>0.79 && fabs(current_eta)<1.33) { //EMB, back sampling, slot 12 : special treatment : this FEB has non contiguous eta regions, so we have to separate them
	    is_additive1=1; //in order to add it to the summary only if it happens
	    
	    if (current_layer<layer_min_additive1)
	      layer_min_additive1=current_layer;
	    if (current_layer>layer_max_additive1)
	      layer_max_additive1=current_layer;
	    
	    if (current_eta_low<eta_min_additive1)
	      eta_min_additive1=current_eta_low;
	    if (current_eta_high>eta_max_additive1)
	      eta_max_additive1=current_eta_high;
	    
            extendPhiRegion(current_phi_low,phi_min_additive1,phi_max_additive1);
            extendPhiRegion(current_phi_high,phi_min_additive1,phi_max_additive1);
	    
	    //	    std::cout << "found the special case, icha=" << icha << ", etamin=" << eta_min_additive1 << ", current layer=" << current_layer << ", current eta=" << current_eta << std::endl;
	  }
	  else { //normal case
	    is_normal=1; // normal case
	    
	    if (current_layer<layer_min)
	      layer_min=current_layer;
	    if (current_layer>layer_max)
	      layer_max=current_layer;
	    
	    if (current_eta_low<eta_min)
	      eta_min=current_eta_low;
	    if (current_eta_high>eta_max)
	      eta_max=current_eta_high;
	    
            extendPhiRegion(current_phi_low,phi_min,phi_max);
            extendPhiRegion(current_phi_high,phi_min,phi_max);

	  }
	} //end of isOnlineConnected()
      } // end of loop on channels
      
      if (is_normal) {
	CaloAffectedRegionInfo current_CaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,layer_min,layer_max,CaloAffectedRegionInfo::missingReadout);
	m_ArrayLArAffectedRegionInfo_current_run.push_back(current_CaloAffectedRegionInfo);
      }
      
      if (is_additive1) {
	CaloAffectedRegionInfo current_additive1_CaloAffectedRegionInfo(eta_min_additive1,eta_max_additive1,phi_min_additive1,phi_max_additive1,layer_min_additive1,layer_max_additive1,CaloAffectedRegionInfo::missingReadout);
	m_ArrayLArAffectedRegionInfo_current_run.push_back(current_additive1_CaloAffectedRegionInfo);
      }
    } // end of isMissingFeb  
  } // end of loop on Febs  
  return StatusCode::SUCCESS;
}
//================================================================
StatusCode LArAffectedRegionAlg::updateAffectedRegionsFromDB(IOVSVC_CALLBACK_ARGS)
{
  // Converter attribute list collection into vector of AffectedRegionInfos

  ATH_MSG_INFO ( "updateAffectedRegionsFromDB()" );

  // retrieve from detStore 
  const CondAttrListCollection* attrListColl = 0;
  ATH_CHECK( detStore()->retrieve(attrListColl, "/LAR/LArAffectedRegionInfo") );

  CaloAffectedRegionInfoVec* affRegVec = 0;

  if (detStore()->contains<CaloAffectedRegionInfoVec>("LArAffectedRegion")) {
    ATH_CHECK( detStore()->retrieve(affRegVec,"LArAffectedRegion") );
    affRegVec->clear();
  }
  else {
      affRegVec = new CaloAffectedRegionInfoVec();
      ATH_CHECK( detStore()->record(affRegVec, "LArAffectedRegion") );
  }

  
  // Loop over collection
  CondAttrListCollection::const_iterator first = attrListColl->begin();
  CondAttrListCollection::const_iterator last  = attrListColl->end();
  for (; first != last; ++first) {
      std::ostringstream attrStr1;
      (*first).second.toOutputStream( attrStr1 );
      ATH_MSG_DEBUG ( "ChanNum " << (*first).first << 
                      " Attribute list " << attrStr1.str() );
      //      const AttributeList& attrList = (*first).second;
      const coral::AttributeList& attrList = (*first).second;
      CaloAffectedRegionInfo info;
      float eta_min = attrList["eta_min"].data<float>();
      float eta_max = attrList["eta_max"].data<float>();
      float phi_min = attrList["phi_min"].data<float>();
      float phi_max = attrList["phi_max"].data<float>();
      int layer_min = attrList["layer_min"].data<int>();
      int layer_max = attrList["layer_max"].data<int>();
      CaloAffectedRegionInfo::type_problem problem = (CaloAffectedRegionInfo::type_problem)(attrList["problem"].data<int>());

      info.FillCaloAffectedRegionInfo(eta_min,eta_max,phi_min,phi_max,layer_min,layer_max,problem);
      affRegVec->push_back(info);
  }

  return StatusCode::SUCCESS;
}
//================================================================
StatusCode LArAffectedRegionAlg::execute() {
  ATH_MSG_DEBUG ( "execute()" );
  
  if (!m_readingFromBytestream) {  //put the print to check that it is ok
    
    CaloAffectedRegionInfoVec* affRegVec;
    
    if (detStore()->contains<CaloAffectedRegionInfoVec>("LArAffectedRegion")) {
     ATH_CHECK( detStore()->retrieve(affRegVec, "LArAffectedRegion") );

     if (msgLvl(MSG::DEBUG)) {
       ATH_MSG_DEBUG  ( " Dump of affected regions read : " );
      for (unsigned int i=0;i<affRegVec->size();i++) {
        ((*affRegVec)[i]).PrintInfo();
      }
     }
    }
   
  }

  return StatusCode::SUCCESS;
}

//====================================================================================
StatusCode LArAffectedRegionAlg::finalize() {

  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG ( "===============================" );
    ATH_MSG_DEBUG ( "Main summary of global problems (all runs) : " << m_ArrayLArAffectedRegionInfo_global.size() << " Affected regions" );
    for (int i=0;i<(int)m_ArrayLArAffectedRegionInfo_global.size();i++) {
      m_ArrayLArAffectedRegionInfo_global[i].PrintInfo();
    }
  }
    
  return StatusCode::SUCCESS;
}

//====================================================================================
float LArAffectedRegionAlg::HV_nominal(const char *identification,const float myparameter)
{
  if (strcmp(identification,"EMBPS")==0) 
    return 1200.;
  else if (strcmp(identification,"EMECPS")==0)
    return 1600.;
  else if (strcmp(identification,"EMB")==0)
    return 2000.;
  else if (strcmp(identification,"EMEC")==0) {
    if ( fabs(myparameter)<1.5 )
      return 2500.;
    else if (fabs(myparameter)<1.6)
      return 2300.;
    else if (fabs(myparameter)<1.8 )
      return 2100.;
    else if ( fabs(myparameter) < 2.0 )
      return 1700.;
    else if ( fabs(myparameter) < 2.1 )
      return 1500.;
    else if ( fabs(myparameter) < 2.3 )
      return 1250.;
    else if ( fabs(myparameter) < 2.5 )
      return 1000.;
    else if ( fabs(myparameter) < 2.8 )
      return 2300.;
    else return 1800.;
  }
  else if (strcmp(identification,"HEC")==0) {
    return 1800.;
  }
  else if (strcmp(identification,"FCAL")==0) {
    if (myparameter<0.5)
      return 250.;
    else if (myparameter<1.5)
      return 375.;
    else if (myparameter<2.5)
      return 500.;
  }

  //std::cout << "identification=" << identification << std::endl;
  //std::cout << "myparameter=" << myparameter << std::endl;
  //std::cout << "WARNING, HV_nominal used improperly !!!" << std::endl;
  return -1;
}
//====================================================================================
// Return the occurence of given AffectedRegion in the list of Affected Region (useful to extend the AffectedRegion in the case of a new run, and useful to update the MetaData)

int LArAffectedRegionAlg::getOccurenceProblem(const std::vector<CaloAffectedRegionInfo>& ArrayLArAffectedRegionInfo_global,float eta_min,float eta_max,float phi_min,float phi_max,int layer_min,int layer_max,int problem,float rate) {
  
  float epsilon=1e-3;
  int occurences=0;

  for (unsigned int i = 0; i < ArrayLArAffectedRegionInfo_global.size(); ++i) {
    
    if (fabs(ArrayLArAffectedRegionInfo_global[i].get_eta_min()-eta_min)<epsilon &&
	fabs(ArrayLArAffectedRegionInfo_global[i].get_eta_max()-eta_max)<epsilon &&
	fabs(ArrayLArAffectedRegionInfo_global[i].get_phi_min()-phi_min)<epsilon &&
	fabs(ArrayLArAffectedRegionInfo_global[i].get_phi_max()-phi_max)<epsilon &&
	ArrayLArAffectedRegionInfo_global[i].get_layer_min()==layer_min &&
	ArrayLArAffectedRegionInfo_global[i].get_layer_max()==layer_max &&
	ArrayLArAffectedRegionInfo_global[i].get_problem()==problem &&
	fabs(ArrayLArAffectedRegionInfo_global[i].get_rate()-rate)<epsilon) {
      occurences++;
      //      std::cout << "found the asked AffectedRegion in the global list of AffectedRegion" << std::endl;
    }
  }
    
  return occurences;
}
//=========================================================================================
std::vector<int> LArAffectedRegionAlg::returnProblem(const float eta, const float phi, const float delta_eta, const float delta_phi)   // from implementation of list of Affected Region : transparent wrt discontinuity
{
  std::vector<int> list_problem;

  if (delta_eta<0 || delta_phi<0) {
    std::cout << "error in LArAffectedRegionAlg::returnProblem : please give positive value for delta_eta, delta_phi" << std::endl;
    return list_problem;
  }
    
  bool flag_eta; //flag if eta and delta_eta relevant for the current Affected Region
  bool flag_phi; //flag if phi and delta_phi relevant for the current Affected Region

  for (int i=0;i<(int)m_ArrayLArAffectedRegionInfo_current_run.size();i++) {
    flag_eta=0; // not yet found
    flag_phi=0; // not yet found
    
    if (eta<m_ArrayLArAffectedRegionInfo_current_run[i].get_eta_min() && eta+delta_eta>=m_ArrayLArAffectedRegionInfo_current_run[i].get_eta_min())
      flag_eta=1;
    else if (eta>=m_ArrayLArAffectedRegionInfo_current_run[i].get_eta_min() && eta<=m_ArrayLArAffectedRegionInfo_current_run[i].get_eta_max())
      flag_eta=1;
    else if (eta>=m_ArrayLArAffectedRegionInfo_current_run[i].get_eta_max() && eta-delta_eta<=m_ArrayLArAffectedRegionInfo_current_run[i].get_eta_max())
      flag_eta=1;
    
    if (phi<m_ArrayLArAffectedRegionInfo_current_run[i].get_phi_min() && phi+delta_phi>=m_ArrayLArAffectedRegionInfo_current_run[i].get_phi_min())
      flag_phi=1;
    else if (phi>=m_ArrayLArAffectedRegionInfo_current_run[i].get_phi_min() && phi<=m_ArrayLArAffectedRegionInfo_current_run[i].get_phi_max())
      flag_phi=1;
    else if (phi>=m_ArrayLArAffectedRegionInfo_current_run[i].get_phi_max() && phi-delta_phi<=m_ArrayLArAffectedRegionInfo_current_run[i].get_phi_max())
      flag_phi=1;
    
    if (flag_eta && flag_phi) {//found
      m_ArrayLArAffectedRegionInfo_current_run[i].PrintInfo();
      list_problem.push_back(m_ArrayLArAffectedRegionInfo_current_run[i].get_problem());
    }
  }
  
  return list_problem;
}
//=========================================================================================
void LArAffectedRegionAlg::debuggingSearchDoublons(const std::vector<CaloAffectedRegionInfo>& ArrayLArAffectedRegionInfo) { //debugging method to check if there are doublons in the current run : for example because of lack of method for some detectors parts
  
  for (unsigned int i=0;i<ArrayLArAffectedRegionInfo.size();++i) {
    int occurences=0;
    
    for (unsigned int j=i+1;j<ArrayLArAffectedRegionInfo.size();++j) {
      
      if (ArrayLArAffectedRegionInfo[i].get_eta_min()  ==ArrayLArAffectedRegionInfo[j].get_eta_min() &&
	  ArrayLArAffectedRegionInfo[i].get_eta_max()  ==ArrayLArAffectedRegionInfo[j].get_eta_max() &&
	  ArrayLArAffectedRegionInfo[i].get_phi_min()  ==ArrayLArAffectedRegionInfo[j].get_phi_min() &&
	  ArrayLArAffectedRegionInfo[i].get_phi_max()  ==ArrayLArAffectedRegionInfo[j].get_phi_max() &&
	  ArrayLArAffectedRegionInfo[i].get_layer_min()==ArrayLArAffectedRegionInfo[j].get_layer_min() &&
	  ArrayLArAffectedRegionInfo[i].get_layer_max()==ArrayLArAffectedRegionInfo[j].get_layer_max() &&
	  ArrayLArAffectedRegionInfo[i].get_problem()  ==ArrayLArAffectedRegionInfo[j].get_problem() &&
	  ArrayLArAffectedRegionInfo[i].get_rate()     ==ArrayLArAffectedRegionInfo[j].get_rate()) {
	//std::cout << "found a doublon for " << "index=" << i << ", with index=" << j << std::endl;
	occurences++;
      }
    }
    /*
    if (occurences) {
      std::cout << "found " << occurences << ", the asked AffectedRegion in given list of AffectedRegion : see next line" << std::endl;
      ArrayLArAffectedRegionInfo[i].PrintInfo();      
     }
     */
  }
}
//=========================================================================================

void LArAffectedRegionAlg::extendPhiRegion(float phi, float & phi_min, float & phi_max) {

  static float epsilon=1e-4;
  
  phi = CaloPhiRange::fix(phi);

  if (phi_min>10. || phi_max<-10.) {
     phi_min = CaloPhiRange::fix(phi-epsilon);
     phi_max = CaloPhiRange::fix(phi+epsilon);
     return;
  }

  bool isInRegion=false;
  if (phi_min<phi_max) {
     if (phi>phi_min && phi<phi_max) isInRegion=true;
  }
  else {
     if (phi>phi_min || phi<phi_max) isInRegion=true;
  }
  if (isInRegion) return;

  float dphi1 = CaloPhiRange::diff(phi,phi_min); 
  float dphi2 = CaloPhiRange::diff(phi,phi_max); 
  if (fabs(dphi1)<fabs(dphi2) )
     phi_min=phi;
  else
     phi_max=phi;
  
  return;

}
