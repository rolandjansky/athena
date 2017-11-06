/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// New SCT_DCSConditions Service, based on existing tool in SCT_ConditionsAlgs
// A. R-Veronneau 26/02/08


#include "Identifier/Identifier.h"
#include "GaudiKernel/Property.h"

#include "SCT_DCSConditionsSvc.h"
#include "SCT_DcsAlerts.h"
#include "SCT_SlhcIdConverter.h"
#include "SCT_ConditionsData/SCT_DCSConditionsData.h"

// AttributeList
#include "CoolKernel/StorageType.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include <iostream>
#include <algorithm>

using namespace std;
using SCT_ConditionsServices::DcsValueStatus;
using SCT_ConditionsServices::castId;
static const Identifier InvalidId;

SCT_DCSConditionsSvc::SCT_DCSConditionsSvc( const std::string& name,
                                           ISvcLocator* pSvcLocator ) :
AthService( name, pSvcLocator ),
m_detStore("DetectorStore",name),
m_IOVDbSvc("IOVDbSvc", name),
m_dataFilled(false),
m_status(0),
m_readAllDBFolders(true),
m_returnHVTemp(true),
m_dropFolder(true),
m_barrel_correction(-3.7),
m_ecInner_correction(-13.1),
m_ecOuter_correction(-15.5),
m_hvLowLimit(0.0),
m_hvUpLimit(1000000.0),
m_pBadModules(0),
m_pModulesHV(0),
m_pModulesTemp0(0),
m_pModulesTemp1(0),
m_pHelper(0), 
m_folderPrefix("/SCT/DCS"),
m_chanstatCut("NORM"),
m_useHV(false),
m_useHVLowLimit(19.),
m_useHVUpLimit(1000000.0),
m_useHVChanCut("LOOSE"),
m_cacheHV{} {
  //declare variables which will be filled by jobOptions
  declareProperty("DetectorStore", m_detStore);
  declareProperty("AttrListCollFolders",m_par_atrcollist);
  declareProperty("ReadAllDBFolders", m_readAllDBFolders);
  declareProperty("ReturnHVTemp", m_returnHVTemp);
  declareProperty("DropDCSFolders", m_dropFolder);
  declareProperty("TempBarrelCorrection", m_barrel_correction);
  declareProperty("TempEcInnerCorrection", m_ecInner_correction);
  declareProperty("HVCutLow", m_hvLowLimit);
  declareProperty("HVCutUp", m_hvUpLimit);
  declareProperty("TempEcOuterCorrection", m_ecOuter_correction);
  declareProperty("FolderLocation",m_folderPrefix);
  declareProperty("StateCut",m_chanstatCut);
  declareProperty("UseDefaultHV",m_useHV);
  declareProperty("useHVLow",m_useHVLowLimit);
  declareProperty("useHVUp",m_useHVUpLimit);
  declareProperty("useHVChan",m_useHVChanCut);
}

StatusCode SCT_DCSConditionsSvc::initialize() {
  static const StatusCode fail(StatusCode::FAILURE);
  if (AthService::initialize().isFailure()) return fail;
  if (m_detStore.retrieve().isFailure()) {
    msg(MSG::ERROR) << " Cannot retrieve detector store " << endmsg;
    return fail;
  }
  if (m_readAllDBFolders && m_returnHVTemp){
    std::vector<std::string> names(3);
    names[0] = m_folderPrefix+std::string("/HV");
    names[1] = m_folderPrefix+std::string("/MODTEMP");
    names[2] = m_folderPrefix+std::string("/CHANSTAT");
    m_par_atrcollist.setValue(names);
  } else if (m_returnHVTemp) { 
    std::vector<std::string> names(2);
    names[0] = m_folderPrefix+std::string("/HV");
    names[1] = m_folderPrefix+std::string("/MODTEMP");
    m_par_atrcollist.setValue(names);
  } else if (!m_readAllDBFolders && !m_returnHVTemp) {
    std::vector<std::string> names(1);
    names[0] = m_folderPrefix+std::string("/CHANSTAT");
    m_par_atrcollist.setValue(names);
  } else {
    msg(MSG::INFO) << "Please check jobOptions, SCT_DCSConditionsSvs does nothing as set up." << endmsg;
  }
  msg(MSG::INFO) << "Test: How many folders in Coll: " << m_par_atrcollist.value().size()  << endmsg;
  if (m_detStore->retrieve(m_pHelper,"SCT_ID").isFailure()) return  msg(MSG::ERROR) << "SCT helper failed to retrieve " << endmsg, fail;
  
  if (m_useHV){
    m_hvLowLimit = m_useHVLowLimit;
    m_hvUpLimit = m_useHVUpLimit;
    m_chanstatCut = m_useHVChanCut;
    msg(MSG::INFO) << "Using HV and Chanstat"<< m_chanstatCut << " for marking modules bad. >=Hvlow: "<<  m_hvLowLimit<< " and <=Hv Up: " <<  m_hvUpLimit << m_par_atrcollist.value().size()  << ". Note: UseHV Overrides hv limit and chanstat values in joboptions!!"<< endmsg;
  }

  //Register callbacks for folders in CondAttrListCollection using vector of keys (not hard-coded)
  std::vector<std::string>::const_iterator itr(m_par_atrcollist.value().begin());
  std::vector<std::string>::const_iterator end(m_par_atrcollist.value().end());
  for (;itr!=end;++itr) {
    std::string key = *itr ;
    msg(MSG::INFO) << key << endmsg;
    if (key==m_folderPrefix+std::string("/HV")){
      if (StatusCode::SUCCESS==m_detStore->regFcn(&SCT_DCSConditionsSvc::fillData,this,m_DCSData_HV,key)) {
	msg(MSG::INFO) << "Registered callback for key: " << *itr << endmsg;
      } else {
	msg(MSG::ERROR) << "Cannot register callback function for key " <<  *itr  << endmsg;
      }
    } else if (key==m_folderPrefix+std::string("/MODTEMP")){
      if (StatusCode::SUCCESS==m_detStore->regFcn(&SCT_DCSConditionsSvc::fillData,this,m_DCSData_MT,key)) {
	msg(MSG::INFO) << "Registered callback for key: " << *itr << endmsg;
      } else {
	msg(MSG::ERROR) << "Cannot register callback function for key " <<  *itr  << endmsg;
      }
    } else if (key==m_folderPrefix+std::string("/CHANSTAT")){
      if (StatusCode::SUCCESS==m_detStore->regFcn(&SCT_DCSConditionsSvc::fillData,this,m_DCSData_CS,key)) {
	msg(MSG::INFO) << "Registered callback for key: " << *itr << endmsg;
      } else {
	msg(MSG::ERROR) << "Cannot register callback function for key " <<  *itr  << endmsg;
      }
    } else { 
	  msg(MSG::INFO) << "Cannot registered callback for key: " << *itr <<" Missing data handle."<<endmsg;
    }
  }
  m_pBadModules = new SCT_DCSConditionsData;
  m_pModulesHV = new map<CondAttrListCollection::ChanNum, float >;
  m_pModulesTemp0 = new map<CondAttrListCollection::ChanNum, float >;
  m_pModulesTemp1 = new map<CondAttrListCollection::ChanNum, float >;

  // Initialize HV cache
  m_cacheHV.resize(m_pHelper->wafer_hash_max(), std::pair<bool, float>{false, -30.});

  return StatusCode::SUCCESS;
}

StatusCode SCT_DCSConditionsSvc::finalize(){ 
  if (m_pBadModules){
    delete m_pBadModules;   
  }
  if (m_pModulesHV){
    m_pModulesHV->clear();
    delete m_pModulesHV;    
  }
  if (m_pModulesTemp0){
    m_pModulesTemp0->clear();
    delete m_pModulesTemp0; 
  }
  if (m_pModulesTemp1){
    m_pModulesTemp1->clear();
    delete m_pModulesTemp1; 
  }
  return StatusCode::SUCCESS;
}

//////////////////////////////
// From s.binet
// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode SCT_DCSConditionsSvc::queryInterface(const InterfaceID& riid, void** ppvInterface){
  if ( ISCT_DCSConditionsSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = this;
  } else if ( ISCT_ConditionsSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ISCT_ConditionsSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

//returns if fillData worked correctly
bool SCT_DCSConditionsSvc::filled() const{
  return m_dataFilled;
}

//Can report about the module as a whole or the wafer
bool SCT_DCSConditionsSvc::canReportAbout(InDetConditions::Hierarchy h) {
  return (h == InDetConditions::SCT_MODULE or h == InDetConditions::SCT_SIDE or h == InDetConditions::SCT_STRIP);
}   

//returns the module ID (int), or returns -1 if not able to report
Identifier SCT_DCSConditionsSvc::getModuleID(const Identifier & elementId, InDetConditions::Hierarchy h) {
  if (not canReportAbout(h)) return InvalidId;  
  else {
    if ( h == InDetConditions::SCT_MODULE ) m_moduleId = elementId;
    else if (h == InDetConditions::SCT_SIDE) m_moduleId = m_pHelper->module_id(elementId); 
    else if (h == InDetConditions::SCT_STRIP) {
      m_waferId = m_pHelper->wafer_id(elementId);
      m_moduleId = m_pHelper->module_id(m_waferId); 
    }
    return m_moduleId;
  }
}

//Returns if element Id is good or bad
bool SCT_DCSConditionsSvc::isGood(const Identifier & elementId, InDetConditions::Hierarchy h) {
  m_moduleId=getModuleID(elementId,h);
  if (m_moduleId==InvalidId) return true; // not canreportabout
  else if (m_pBadModules->output(castId(m_moduleId)) == 0) return true; //No params are listed as bad
  else return false;
}

//Does the same for hashIds
bool SCT_DCSConditionsSvc::isGood(const IdentifierHash & hashId){
  m_waferId = m_pHelper->wafer_id(hashId);
  m_moduleId = m_pHelper->module_id(m_waferId);
  return isGood(m_moduleId,InDetConditions::SCT_MODULE);
}

/////////////////////////////////// 

// some lame helper methods: 
// returns HV (-30 if there is no information)
float SCT_DCSConditionsSvc::modHV(const Identifier & elementId, InDetConditions::Hierarchy h) {
  m_moduleId = getModuleID(elementId,h);
  if (m_moduleId==InvalidId) return -30.; // not canreportabout, return -30. 
  std::map<CondAttrListCollection::ChanNum, float >::const_iterator pPair(m_pModulesHV->find(castId(m_moduleId)) ); // find the module 
  if (pPair != m_pModulesHV->end() && isGood(elementId, h) ) return pPair->second;  //return the hv 
  return -30.; //didn't find the module, return -30. 
}

//Does the same for hashIds
float SCT_DCSConditionsSvc::modHV(const IdentifierHash & hashId){
  // Check hashId is OK.
  if(m_cacheHV.size()<=hashId.value()) {
    ATH_MSG_ERROR("hashId " << hashId << " exceeds HV cache size " << m_cacheHV.size());
    return -30.;
  }
  // Check cache is available.
  if(m_cacheHV[hashId].first) {
    return m_cacheHV[hashId].second;
  }
  // Retreive HV value
  m_waferId = m_pHelper->wafer_id(hashId);
  m_moduleId = m_pHelper->module_id(m_waferId);
  float hv{modHV(m_moduleId,InDetConditions::SCT_MODULE)};
  // Set the HV value in the cache
  m_cacheHV[hashId].first = true;
  m_cacheHV[hashId].second = hv;
  // Return HV value
  return hv;
} 

//Returns temp0 (-40 if there is no information)
float SCT_DCSConditionsSvc::hybridTemperature(const Identifier & elementId, InDetConditions::Hierarchy h) {
  m_moduleId = getModuleID(elementId,h);
  if (m_moduleId==InvalidId) return -50.; // not canreportabout
  std::map<CondAttrListCollection::ChanNum, float >::const_iterator pPair(m_pModulesTemp0->find(castId(m_moduleId)) );  // find the module 
  if (pPair != m_pModulesTemp0->end() && isGood(elementId, h) ){
    return pPair->second;  //return the temp
  }
  return -40.;//didn't find the module, return -40. 
} 

//Does the same for hashIds
float SCT_DCSConditionsSvc::hybridTemperature(const IdentifierHash & hashId){
  m_waferId = m_pHelper->wafer_id(hashId);
  m_moduleId = m_pHelper->module_id(m_waferId);
  return hybridTemperature(m_moduleId,InDetConditions::SCT_MODULE);
}

//Returns temp0 + correction for Lorentz angle calculation (-40 if there is no information)
float SCT_DCSConditionsSvc::sensorTemperature(const Identifier & elementId, InDetConditions::Hierarchy h) {
  m_moduleId = getModuleID(elementId,h);
  if (m_moduleId==InvalidId) return -50.; // not canreportabout
  std::map<CondAttrListCollection::ChanNum, float >::const_iterator pPair(m_pModulesTemp0->find(castId(m_moduleId)) );  // find the module 
  if ( pPair != m_pModulesTemp0->end() && isGood(elementId, h)){
    int bec = m_pHelper->barrel_ec(m_moduleId);
    if (bec == 0){
      return ( pPair->second + m_barrel_correction);  //return the temp+correction    
    } else {
      int modeta =  m_pHelper->eta_module(m_moduleId);
      if (modeta==2){
      	return ( pPair->second + m_ecInner_correction);  //return the temp+correction
      } else {
	      return ( pPair->second + m_ecOuter_correction);  //return the temp+correction	
      }
    }
  }
  return -40.;  //didn't find the module, return -40. 
} 

//Does the same for hashIds
float SCT_DCSConditionsSvc::sensorTemperature(const IdentifierHash & hashId){
  m_waferId = m_pHelper->wafer_id(hashId);
  m_moduleId = m_pHelper->module_id(m_waferId);
  return sensorTemperature(m_moduleId,InDetConditions::SCT_MODULE);
}

///////////////////////////////////

//The actual function which puts a error code in a map for each DCS parameter/folder
StatusCode SCT_DCSConditionsSvc::fillData(int &/* i */, std::list<std::string>& keys) {
 
  m_dataFilled=false;
  if (not m_pBadModules) {
    msg(MSG::ERROR) << "Bad modules data object has NULL pointer" << endmsg;
    return StatusCode::FAILURE;
  }

  std::list<std::string>::const_iterator itr_key(keys.begin());
  // Retrieve CondAttrListCollection
  const CondAttrListCollection* attrListCollection;
  for (; itr_key!=keys.end(); ++itr_key) {
    const std::string & thisFolder=*itr_key;
    if (m_detStore->retrieve(attrListCollection,thisFolder).isSuccess()) {
      // loop over collection
      CondAttrListCollection::const_iterator attrList( attrListCollection->begin());
      CondAttrListCollection::const_iterator end( attrListCollection->end());
      //std::ostringstream attrStr1;
      for (;attrList!=end;++attrList) {
        //A CondAttrListCollection is a map of ChanNum and AttributeList
        CondAttrListCollection::ChanNum  channelNumber=(*attrList).first;
        const CondAttrListCollection::AttributeList   & payload=attrList->second;
        //loop over AttributeListSpecification
        coral::AttributeList::const_iterator attrspecb = payload.begin();
        coral::AttributeList::const_iterator attrspece = payload.end();
        for (;attrspecb!=attrspece;++attrspecb) {
          //0 is OK, 1 is Alarm, 2 is Unknown : see SCT_DcsAlerts.h 'DcsStatus' enum.
          std::string param=(*attrspecb).specification().name();
	        //Check if modules was not ok (1 or 16 for HV and LV) and not set manually (3 or 48). Both LV and HV must be either for module to be good.  
          if (param == "STATE") {
	    try{
	      int hvstate = (*attrList).second[param].data<unsigned int>() bitand 240;
	      int lvstate = (*attrList).second[param].data<unsigned int>() bitand 15;
	      if ((    (m_chanstatCut == "NORM")  && !( ( hvstate==16 || hvstate==48 ) && ( lvstate==1 || lvstate==3 ) )  )
		  || ( (m_chanstatCut == "NSTBY") && !( ( hvstate==16 || hvstate==48 || hvstate == 32) && ( lvstate==1 || lvstate==3 || lvstate == 2) ) )
		  || ( (m_chanstatCut == "LOOSE") && !( ( hvstate==16 || hvstate==48 || hvstate == 32 || hvstate == 128) && ( lvstate==1 || lvstate==3 || lvstate == 2 || lvstate == 8) ) )
		  ){  //Option for no cut is m_chanstatCut == 'NONE'
		m_pBadModules->fill(channelNumber, param);
	      } 
	      else m_pBadModules->remove(channelNumber, param);
	    }catch(...){
        ATH_MSG_DEBUG( "Exception caught while trying to access STATE");
	    }
	  } 
	  //Set to ignore all parameters, except HV and LV
	  else if (param == "CCtagON_Name" || param == "CCtagON_Send" || param == "CCtagON_Recv" || param == "LVCHSTAT_RECV" || param == "HVCHCURR_RECV") {
	    continue;
	  } 
	  // if we are asked to return the HV and temp, then we need to fill these strucures. 
	  if (param == "HVCHVOLT_RECV") {
	    try{
	      float hvval = (*attrList).second[param].data<float>();
	      if( (hvval < m_hvLowLimit) || (hvval > m_hvUpLimit) ) {
		m_pBadModules->fill(channelNumber, param);
	      }
	      else m_pBadModules->remove(channelNumber, param);
	      if (m_returnHVTemp){
		(*m_pModulesHV)[channelNumber]=hvval;
	      }
	    }catch(...){
        ATH_MSG_DEBUG("Exception caught while trying to access HVCHVOLT_RECV");
	    }
	  } else if (m_returnHVTemp && param == "MOCH_TM0_RECV") {
	    try{
              (*m_pModulesTemp0)[channelNumber]=(*attrList).second[param].data<float>();
	    }catch(...){
	      ATH_MSG_DEBUG("Exception caught while trying to access MOCH_TM0_RECV");
	    }
	  } else if (m_returnHVTemp && param == "MOCH_TM1_RECV") { //2 temp sensors per module
	    try{
	      (*m_pModulesTemp1)[channelNumber]=(*attrList).second[param].data<float>();
	    }catch(...){
        ATH_MSG_DEBUG("Exception caught while trying to access MOCH_TM1_RECV");
	    }
	  }
	}

        }
      }
      m_dataFilled=true;
    }

  //we are done with the db folders and have the data locally. 
  if (!m_dropFolder){
    std::list<std::string>::const_iterator itr2_key(keys.begin());
    for (; itr2_key!=keys.end(); ++itr2_key) {
      const std::string & aFolder=*itr2_key;
      // loop over collection
      m_IOVDbSvc->dropObject(aFolder,false); //"false" here keeps the data in the cache, so SG doesn't have to go back to the DB each time  
    }
  }

  // Reset HV cache
  std::fill(m_cacheHV.begin(), m_cacheHV.end(), std::pair<bool, float>{false, -30.});

  return StatusCode::SUCCESS;
}

