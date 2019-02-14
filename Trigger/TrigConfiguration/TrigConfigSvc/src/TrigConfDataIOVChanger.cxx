/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         TrigConfDataIOVChanger.cxx
 **
 **   Description: 
 **
 **                - algorithm to trigger the copy of trigger
 **                configuration meta-data from COOL to ESD/AOD. This
 **                should only run when data is transformed from
 **                BS/RDO into ESD/AOD.
 **
 **                - Additionally the algorithm changes the IOVrange
 **                in the ESD/AOD trigger configuration meta-data,
 **                which should ONLY be done on simulated data
 **
 * @author Joerg Stelzer  <stelzer@cern.ch>  - CERN
 **
 **   Created:      Tue May  09 14:55:56 GMT 2006
 **   Modified:
 **
 **************************************************************************/

#include "TrigConfigSvc/TrigConfDataIOVChanger.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"

#include "IOVDbDataModel/IOVMetaDataContainer.h"

#include "AthenaKernel/IOVTime.h"
#include "AthenaKernel/IOVRange.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "GaudiKernel/ThreadLocalContext.h"

#define TRIGGER_CONF_HLTMENU  "/TRIGGER/HLT/Menu"
#define TRIGGER_CONF_HLTKEYS  "/TRIGGER/HLT/HltConfigKeys"
#define TRIGGER_CONF_LVL1KEY  "/TRIGGER/LVL1/Lvl1ConfigKey"
#define TRIGGER_CONF_LVL1MENU "/TRIGGER/LVL1/Menu"
#define TRIGGER_CONF_LVL1PS   "/TRIGGER/LVL1/Prescales"
#define TRIGGER_CONF_LVL1ITD  "/TRIGGER/LVL1/ItemDef"
#define TRIGGER_CONF_HLTPS    "/TRIGGER/HLT/Prescales"
#define TRIGGER_CONF_HLTPSK   "/TRIGGER/HLT/PrescaleKey"

using namespace std;
using namespace TrigConf;

TrigConfDataIOVChanger::TrigConfDataIOVChanger(const std::string &name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_AdjustIOV(false),
    m_LastRun(0),
    m_storeGate("StoreGateSvc", name),
    m_metaDataStore("StoreGateSvc/MetaDataStore",name),
    m_detStore("StoreGateSvc/DetectorStore",name)
{
  declareProperty("AdjustIOV", m_AdjustIOV, "Set to true if the IOV needs adjustment (MC production), false (default) for data-reco");
}


TrigConf::TrigConfDataIOVChanger::~TrigConfDataIOVChanger() {}


StatusCode
TrigConf::TrigConfDataIOVChanger::initialize() {

   ATH_MSG_DEBUG("Initializing TrigConfDataIOVChanger..." << endl << "Properties:");

   CHECK(m_storeGate.retrieve());

   CHECK(m_metaDataStore.retrieve());

   return StatusCode::SUCCESS;
}



StatusCode
TrigConf::TrigConfDataIOVChanger::finalize() {
  return StatusCode::SUCCESS;
}


StatusCode
TrigConf::TrigConfDataIOVChanger::execute() {

   // First get the runnumber:
   const EventContext& ctx = Gaudi::Hive::currentContext();
   unsigned int runNumber = ctx.eventID().run_number();
  
   if(m_LastRun == runNumber)       // already executed in this run?
      return StatusCode::SUCCESS;

   m_LastRun = runNumber;

   // execute only once
   //   if(m_AlreadyExecuted)
   //     return StatusCode::SUCCESS;
  
   //   m_AlreadyExecuted = true;
  

   // ----------------------------------------
   // the copying of the cool folders into the
   // metadatastore (which is written out to 
   // ESD/AOD) is triggered by accessing them
   // in the detectorstore
   // (perhaps at some point a forced copy flag
   // will be implemented, but for now...)
   // ----------------------------------------
   const DataHandle<CondAttrListCollection> listColl;
   const DataHandle<AthenaAttributeList> attList;

   if( m_detStore->retrieve(listColl, TRIGGER_CONF_HLTMENU  ).isSuccess() && listColl) {
      ATH_MSG_INFO("Triggered reading of  " << TRIGGER_CONF_HLTMENU);    
   } else {
      ATH_MSG_INFO("Failed to read " << TRIGGER_CONF_HLTMENU);    
   }
   if( m_detStore->retrieve( attList, TRIGGER_CONF_HLTKEYS  ).isSuccess() && listColl) {
      ATH_MSG_INFO("Triggered reading of  " << TRIGGER_CONF_HLTKEYS);    
   } else {
      ATH_MSG_INFO("Failed to read " << TRIGGER_CONF_HLTKEYS);    
   }
   if( m_detStore->retrieve( attList, TRIGGER_CONF_LVL1KEY  ).isSuccess() && listColl) {
      ATH_MSG_INFO("Triggered reading of  " << TRIGGER_CONF_LVL1KEY);    
   } else {
      ATH_MSG_INFO("Failed to read " << TRIGGER_CONF_LVL1KEY);    
   }
   if( m_detStore->retrieve(listColl, TRIGGER_CONF_LVL1MENU ).isSuccess() && listColl) {
      ATH_MSG_INFO("Triggered reading of  " << TRIGGER_CONF_LVL1MENU);    
   } else {
      ATH_MSG_INFO("Failed to read " << TRIGGER_CONF_LVL1MENU);    
   }
   if( m_detStore->retrieve(listColl, TRIGGER_CONF_LVL1PS   ).isSuccess() && listColl) {
      ATH_MSG_INFO("Triggered reading of  " << TRIGGER_CONF_LVL1PS);    
   } else {
      ATH_MSG_INFO("Failed to read " << TRIGGER_CONF_LVL1PS);    
   } 
   if( m_detStore->retrieve(listColl, TRIGGER_CONF_LVL1ITD   ).isSuccess() && listColl) {
      ATH_MSG_INFO("Triggered reading of  " << TRIGGER_CONF_LVL1ITD);
   } else {
      ATH_MSG_INFO("Failed to read " << TRIGGER_CONF_LVL1ITD);
   }
   if( m_detStore->retrieve(listColl, TRIGGER_CONF_HLTPS   ).isSuccess() && listColl) {
      ATH_MSG_INFO("Triggered reading of  " << TRIGGER_CONF_HLTPS);
   } else {
      ATH_MSG_INFO("Failed to read " << TRIGGER_CONF_HLTPS);
   }
   if( m_detStore->retrieve(listColl, TRIGGER_CONF_HLTPSK   ).isSuccess() && listColl) {
      ATH_MSG_INFO("Triggered reading of  " << TRIGGER_CONF_HLTPSK);
   } else {
      ATH_MSG_INFO("Failed to read " << TRIGGER_CONF_HLTPSK);
   }


   if(!m_AdjustIOV) return StatusCode::SUCCESS;

   // ----------------------------------------
   // in the following section the IOV is 
   // adjusted to cover the range of the run
   // ----------------------------------------

   // Second retrieve all meta data containers from the MetaDataStore
   // (the one that goes out into the ESD/AOD)
   const DataHandle<IOVMetaDataContainer> cont;
   const DataHandle<IOVMetaDataContainer> contEnd;

   CHECK(m_metaDataStore->retrieve(cont, contEnd));

   // For each container, print out its folder name, get the payload
   // (containing data + IOVs) and adjust the IOV for each
   // attributeList (we have only one) and each channel therein
   for (; cont != contEnd; ++cont) {
      if(cont->folderName() != TRIGGER_CONF_HLTMENU  &&
         cont->folderName() != TRIGGER_CONF_HLTKEYS  &&
         cont->folderName() != TRIGGER_CONF_LVL1KEY  &&
         cont->folderName() != TRIGGER_CONF_LVL1MENU &&
         cont->folderName() != TRIGGER_CONF_LVL1PS   &&
         cont->folderName() != TRIGGER_CONF_LVL1ITD  &&
         cont->folderName() != TRIGGER_CONF_HLTPS    &&
         cont->folderName() != TRIGGER_CONF_HLTPSK) continue;
    

      IOVPayloadContainer*  payload = const_cast<IOVPayloadContainer*>(cont->payloadContainer());

      ATH_MSG_DEBUG("Size of folder payload is " << payload->size());

      if(payload->size()==0) {
         ATH_MSG_ERROR("Size of folder payload is 0, no configuration meta data, please check setup");
         return StatusCode::FAILURE;
      }

      IOVPayloadContainer::const_iterator itAttList    = payload->begin();
      IOVPayloadContainer::const_iterator itAttListEnd = payload->end();

      bool needToChangeIOV = false; // if there is already multiple payloads (= multiple IOV's then a new one needs to be added
      if(payload->size()==1) // if there is one payload, a new one needs to be added if the existing one is a valid run
         needToChangeIOV = CheckIOV(*itAttList);

      itAttList    = payload->begin();
      if(needToChangeIOV) {
         ATH_MSG_INFO("Adjusting IOV of folder " << cont->folderName() << " to run " << runNumber);
         for (; itAttList != itAttListEnd; ++itAttList)
            ChangeIOV((*itAttList), runNumber);
      } else {
         ATH_MSG_INFO("Adding new IOV for folder " << cont->folderName() << " for run " << runNumber);
         for (; itAttList != itAttListEnd; ++itAttList) {
            CondAttrListCollection * pl = CreatePayload((*itAttList), runNumber);
            if(!payload->merge(pl))
               delete pl; 
         }
      }

      if( msgLevel()<=MSG::VERBOSE) {
         itAttList    = payload->begin();
         for (; itAttList != itAttListEnd; ++itAttList)
            printOut((*itAttList));
      }
   }

   return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------
bool
TrigConf::TrigConfDataIOVChanger::CheckIOV(const CondAttrListCollection* coll) const {

  // check for the IOVRange of the first channel
  CondAttrListCollection::iov_const_iterator  itIOV    = coll->iov_begin();

  // if it starts at run=0, lb=0, then the range needs to be changed
  if((*itIOV).second.start() == IOVTime(0,0)) return true;

  // otherwise a new payload needs to be added
  return false;
}


//--------------------------------------------------------------------------
void
TrigConf::TrigConfDataIOVChanger::ChangeIOV(const CondAttrListCollection* coll, unsigned int runNumber) const {
  CondAttrListCollection* cColl = const_cast<CondAttrListCollection*>(coll);

  // loop over the internal map<ChanNum, IOVRange>
  CondAttrListCollection::iov_const_iterator  itIOV    = cColl->iov_begin();
  CondAttrListCollection::iov_const_iterator  itIOVEnd = cColl->iov_end  ();
  std::vector<CondAttrListCollection::ChanNum> channels; channels.reserve(cColl->iov_size());
  for (; itIOV != itIOVEnd; ++itIOV) {
    CondAttrListCollection::ChanNum channel = (*itIOV).first;
    //    std::cout << channel << ": name = " << coll->chanName(channel) << ", start time = " << (*itIOV).second.start() << std::endl;
     //if((*itIOV).second.start()!=IOVTime(0,0)) continue;
    channels.push_back(channel);
  }

  std::vector<CondAttrListCollection::ChanNum>::const_iterator itCh = channels.begin();
  std::vector<CondAttrListCollection::ChanNum>::const_iterator itChEnd = channels.end();
  for(; itCh != itChEnd; ++itCh) {
    cColl->add( (*itCh), IOVRange(IOVTime(runNumber,0), IOVTime(runNumber+1,0)) );
  }
  
  return;
}


//--------------------------------------------------------------------------
CondAttrListCollection*
TrigConf::TrigConfDataIOVChanger::CreatePayload(const CondAttrListCollection* coll, unsigned int runNumber) const {
  CondAttrListCollection * pl = new CondAttrListCollection(true);

  // loop over the internal map<ChanNum, IOVRange>
  CondAttrListCollection::iov_const_iterator  itIOV    = coll->iov_begin();
  CondAttrListCollection::iov_const_iterator  itIOVEnd = coll->iov_end  ();
  CondAttrListCollection::const_iterator      itAttr   = coll->begin();
  for (; itIOV != itIOVEnd; ++itIOV, ++itAttr) {
    CondAttrListCollection::ChanNum channel = (*itIOV).first;
    pl->add( channel, (*itAttr).second ); // add the attribute list to the new collection
    pl->add( channel, IOVRange(IOVTime(runNumber,0), IOVTime(runNumber+1,0)) ); // add the IOV to the new collection
  }
  return pl;
}



//--------------------------------------------------------------------------
void
TrigConf::TrigConfDataIOVChanger::printOut(const CondAttrListCollection* coll) const
{
    ATH_MSG_VERBOSE(coll->minRange() << " iov size " 
        <<     coll->iov_size());
    CondAttrListCollection::iov_const_iterator  itIOV    = coll->iov_begin();
    CondAttrListCollection::iov_const_iterator  itIOVEnd = coll->iov_end  ();
    for (; itIOV != itIOVEnd; ++itIOV) {
        ATH_MSG_VERBOSE((*itIOV).first << " " << (*itIOV).second);
    }

    CondAttrListCollection::const_iterator  itAtt    = coll->begin();
    CondAttrListCollection::const_iterator  itAttEnd = coll->end  ();
    for (; itAtt != itAttEnd; ++itAtt) {
        std::ostringstream attrStr;
        attrStr << "{";
        for (coral::AttributeList::const_iterator itr= (*itAtt).second.begin();
             itr !=  (*itAtt).second.end();++itr) {
            if (itr !=  (*itAtt).second.begin()) attrStr << ",";
            itr->toOutputStream(attrStr);
        }
        attrStr << "}";
        ATH_MSG_VERBOSE((*itAtt).first << " " << attrStr.str());
    }
}
