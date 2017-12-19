/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file MetaDataSvc.cxx
 *  @brief This file contains the implementation for the MetaDataSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: MetaDataSvc.cxx,v 1.46 2008-11-19 23:21:10 gemmeren Exp $
 **/

#include "MetaDataSvc.h"

#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/FileIncident.h"

#include "AthenaBaseComps/AthCnvSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/SGVersionedKey.h"
#include "PersistentDataModel/DataHeader.h"

#include <vector>
#include <sstream>

//________________________________________________________________________________
MetaDataSvc::MetaDataSvc(const std::string& name, ISvcLocator* pSvcLocator) : ::AthService(name, pSvcLocator),
	m_inputDataStore("StoreGateSvc/InputMetaDataStore", name),
	m_outputDataStore("StoreGateSvc/MetaDataStore", name),
	m_addrCrtr("AthenaPoolCnvSvc", name),
	m_fileMgr("FileMgr", name),
	m_incSvc("IncidentSvc", name),
	m_storageType(0L),
	m_clearedInputDataStore(true),
	m_allowMetaDataStop(false),
	m_persToClid(),
	m_toolForClid(),
	m_streamForKey() {
   // declare properties
   declareProperty("MetaDataContainer", m_metaDataCont = "");
   declareProperty("MetaDataTools", m_metaDataTools);
   declareProperty("CnvSvc", m_addrCrtr = ServiceHandle<IAddressCreator>("AthenaPoolCnvSvc", name));
   // persistent class name to transient CLID map
   m_persToClid.insert(std::pair<std::string, CLID>("DataHeader_p5", 222376821));
   m_persToClid.insert(std::pair<std::string, CLID>("EventStreamInfo_p3", 167728019));
   m_persToClid.insert(std::pair<std::string, CLID>("ByteStreamMetadataContainer_p1", 1076128893));
   m_persToClid.insert(std::pair<std::string, CLID>("IOVMetaDataContainer_p1", 1316383046));
   m_persToClid.insert(std::pair<std::string, CLID>("xAOD::EventFormat_v1", 243004407));
   m_persToClid.insert(std::pair<std::string, CLID>("xAOD::CutBookkeeperContainer_v1", 1234982351));
   m_persToClid.insert(std::pair<std::string, CLID>("xAOD::CutBookkeeperAuxContainer_v1", 1147935274));
   m_persToClid.insert(std::pair<std::string, CLID>("xAOD::TriggerMenuContainer_v1", 1107011239));
   m_persToClid.insert(std::pair<std::string, CLID>("DataVector<xAOD::TriggerMenu_v1>", 1107011239));
   m_persToClid.insert(std::pair<std::string, CLID>("xAOD::TriggerMenuAuxContainer_v1", 1212409402));
   m_persToClid.insert(std::pair<std::string, CLID>("xAOD::LumiBlockRangeContainer_v1", 1115934851));
   m_persToClid.insert(std::pair<std::string, CLID>("DataVector<xAOD::LumiBlockRange_v1>", 1115934851));
   m_persToClid.insert(std::pair<std::string, CLID>("xAOD::LumiBlockRangeAuxContainer_v1", 1251061086));
   m_persToClid.insert(std::pair<std::string, CLID>("xAOD::FileMetaData_v1", 178309087));
   m_persToClid.insert(std::pair<std::string, CLID>("xAOD::FileMetaDataAuxInfo_v1", 73252552));
   m_persToClid.insert(std::pair<std::string, CLID>("xAOD::RingSetConfContainer_v1", 1157997427));
   m_persToClid.insert(std::pair<std::string, CLID>("DataVector<xAOD::RingSetConf_v1>", 1157997427));
   m_persToClid.insert(std::pair<std::string, CLID>("xAOD::RingSetConfAuxContainer_v1", 1307745126));
   m_persToClid.insert(std::pair<std::string, CLID>("xAOD::TruthMetaDataContainer_v1", 1188015687));
   m_persToClid.insert(std::pair<std::string, CLID>("DataVector<xAOD::TruthMetaData_v1>", 1188015687));
   m_persToClid.insert(std::pair<std::string, CLID>("xAOD::TruthMetaDataAuxContainer_v1", 1094306618));
   // some classes need to have new/different tools added for metadata propagation
   m_toolForClid.insert(std::pair<CLID, std::string>(167728019, "CopyEventStreamInfo"));
   m_toolForClid.insert(std::pair<CLID, std::string>(243004407, "xAODMaker::EventFormatMetaDataTool"));
   m_toolForClid.insert(std::pair<CLID, std::string>(1234982351, "BookkeeperTool"));
   m_toolForClid.insert(std::pair<CLID, std::string>(1107011239, "xAODMaker::TriggerMenuMetaDataTool"));
   m_toolForClid.insert(std::pair<CLID, std::string>(1115934851, "LumiBlockMetaDataTool"));
}
//__________________________________________________________________________
MetaDataSvc::~MetaDataSvc() {
}
//__________________________________________________________________________
StatusCode MetaDataSvc::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   if (!::AthService::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthService base class.");
      return(StatusCode::FAILURE);
   }

   // Retrieve InputMetaDataStore
   if (!m_inputDataStore.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get InputMetaDataStore.");
      return(StatusCode::FAILURE);
   }
   // Retrieve OutputMetaDataStore
   if (!m_outputDataStore.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get OutputMetaDataStore.");
      return(StatusCode::FAILURE);
   }
   // Retrieve AddressCreator
   if (!m_addrCrtr.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get AddressCreator.");
      return(StatusCode::FAILURE);
   }
   AthCnvSvc* cnvSvc = dynamic_cast<AthCnvSvc*>(m_addrCrtr.operator->());
   if (cnvSvc) {
      m_storageType = cnvSvc->repSvcType();
   } else {
      ATH_MSG_WARNING("Cannot get ConversionSvc Interface.");
   }
   // Get FileMgr
   if (!m_fileMgr.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get FileMgr.");
      return(StatusCode::FAILURE);
   }
   // Set to be listener for end of event
   if (!m_incSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get IncidentSvc.");
      return(StatusCode::FAILURE);
   }
   if (!m_metaDataTools.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get " << m_metaDataTools);
      return(StatusCode::FAILURE);
   }
   m_incSvc->addListener(this, "FirstInputFile", 90);
   m_incSvc->addListener(this, "BeginTagFile", 90);
   m_incSvc->addListener(this, "BeginInputFile", 90);
   m_incSvc->addListener(this, "EndInputFile", 10);
   m_incSvc->addListener(this, "EndTagFile", 10);
   m_incSvc->addListener(this, "LastInputFile", 10);
   m_incSvc->addListener(this, "ShmProxy", 90);
   // Register this service for 'I/O' events
   ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", this->name());
   if (!iomgr.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get IoComponentMgr.");
      return(StatusCode::FAILURE);
   }
   if (!iomgr->io_register(this).isSuccess()) {
      ATH_MSG_FATAL("Cannot register myself with the IoComponentMgr.");
      return(StatusCode::FAILURE);
   }
   ServiceHandle<IJobOptionsSvc> joSvc("JobOptionsSvc", name());
   if (!joSvc.retrieve().isSuccess()) {
      ATH_MSG_WARNING("Cannot get JobOptionsSvc.");
   } else {
      const std::vector<const Property*>* evtselProps = joSvc->getProperties("EventSelector");
      if (evtselProps != nullptr) {
         for (std::vector<const Property*>::const_iterator iter = evtselProps->begin(),
                         last = evtselProps->end(); iter != last; iter++) {
            if ((*iter)->name() == "InputCollections") {
               // Get EventSelector to force in-time initialization and FirstInputFile incident
               ServiceHandle<IEvtSelector> evtsel("EventSelector", this->name());
               if (!evtsel.retrieve().isSuccess()) {
                  ATH_MSG_WARNING("Cannot get EventSelector.");
               }
            }
         }
      }
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode MetaDataSvc::finalize() {
   // Release IncidentService
   if (!m_incSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release IncidentService.");
   }
   // Release FileMgr
   if (!m_fileMgr.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release FileMgr.");
   }
   // Release AddressCreator
   if (!m_addrCrtr.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release AddressCreator.");
   }
   // Release OutputMetaDataStore 
   if (!m_outputDataStore.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release OutputMetaDataStore.");
   }
   // Release InputMetaDataStore 
   if (!m_inputDataStore.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release InputMetaDataStore.");
   }
   return(::AthService::finalize());
}
//__________________________________________________________________________
StatusCode MetaDataSvc::stop() {
   ServiceHandle<IJobOptionsSvc> joSvc("JobOptionsSvc", name());
   if (!joSvc.retrieve().isSuccess()) {
      ATH_MSG_WARNING("Cannot get JobOptionsSvc.");
   } else {
      const std::vector<const Property*>* evtselProps = joSvc->getProperties("EventSelector");
      if (evtselProps != nullptr) {
         for (std::vector<const Property*>::const_iterator iter = evtselProps->begin(),
                         last = evtselProps->end(); iter != last; iter++) {
            if ((*iter)->name() == "InputCollections") {
               // Get EventSelector to fire End...File incidents.
               ServiceHandle<IEvtSelector> evtsel("EventSelector", this->name());
               IEvtSelector::Context* ctxt(nullptr);
               if (!evtsel->releaseContext(ctxt).isSuccess()) {
                  ATH_MSG_WARNING("Cannot release context on EventSelector.");
               }
            }
         }
      }
   }
   ATH_MSG_DEBUG("Releasing MetaDataTools");
   if (!m_metaDataTools.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release " << m_metaDataTools);
   }
   // Set to be listener for end of event
   Incident metaDataStopIncident(name(), "MetaDataStop");
   m_incSvc->fireIncident(metaDataStopIncident);
   return(StatusCode::SUCCESS);
}
//_______________________________________________________________________
StatusCode MetaDataSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if (riid == this->interfaceID()) {
      *ppvInterface = this;
   } else {
      // Interface is not directly available: try out a base class
      return(::AthService::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode MetaDataSvc::preLoadAddresses(StoreID::type /*storeID*/,
	IAddressProvider::tadList& /*tads*/) {
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode MetaDataSvc::loadAddresses(StoreID::type storeID, IAddressProvider::tadList& tads) {
   if (storeID != StoreID::METADATA_STORE) {
      return(StatusCode::SUCCESS);
   }
   // Put Additional MetaData objects into Input MetaData Store using VersionedKey
   std::list<SG::ObjectWithVersion<DataHeader> > allVersions;
   StatusCode sc = m_inputDataStore->retrieveAllVersions(allVersions, name());
   if (!sc.isSuccess()) {
      ATH_MSG_WARNING("Could not retrieve all versions for DataHeader, will not read Metadata");
   } else {
      int verNumber = 0;
      for (std::list<SG::ObjectWithVersion<DataHeader> >::const_iterator iter = allVersions.begin(),
	      last = allVersions.end(); iter != last; iter++, verNumber++) {
         const DataHeader* dataHeader = iter->dataObject;
         if (dataHeader == nullptr) {
            ATH_MSG_ERROR("Could not get DataHeader, will not read Metadata");
            return(StatusCode::FAILURE);
         }
         for (std::vector<DataHeaderElement>::const_iterator dhIter = dataHeader->begin(),
		         dhLast = dataHeader->end(); dhIter != dhLast; dhIter++) {
            const CLID clid = dhIter->getPrimaryClassID();
            if (clid != ClassID_traits<DataHeader>::ID()) {
               SG::VersionedKey myVersObjKey(dhIter->getKey(), verNumber);
               std::string key = dhIter->getKey();
               if (verNumber != 0) key = myVersObjKey;
               tads.push_back(dhIter->getAddress(key));
            }
         }
      }
   }
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode MetaDataSvc::updateAddress(StoreID::type, SG::TransientAddress*,
                                      const EventContext&) {
   return(StatusCode::FAILURE);
}
//__________________________________________________________________________
void MetaDataSvc::handle(const Incident& inc) {
   const FileIncident* fileInc  = dynamic_cast<const FileIncident*>(&inc);
   if (fileInc == nullptr) {
      ATH_MSG_ERROR("Unable to get FileName from EndInputFile incident");
      return;
   }
   const std::string fileName = fileInc->fileName();
   ATH_MSG_DEBUG("handle() " << inc.type() << " for " << fileName);

   if (inc.type() == "FirstInputFile") {
      // Register open/close callback actions
      Io::bfcn_action_t boa = boost::bind(&MetaDataSvc::rootOpenAction, this, _1,_2);
      if (m_fileMgr->regAction(boa, Io::OPEN).isFailure()) {
         ATH_MSG_FATAL("Cannot register ROOT file open action with FileMgr.");
      }
      if (!initInputMetaDataStore(fileName).isSuccess()) {
         ATH_MSG_WARNING("Unable to initialize InputMetaDataStore");
      }
   } else if (inc.type() == "BeginInputFile") {
      m_allowMetaDataStop = false;
      if (fileName.find("BSF:") != 0) {
         if (!m_clearedInputDataStore) {
            if (!m_inputDataStore->clearStore().isSuccess()) {
               ATH_MSG_WARNING("Unable to clear input MetaData Proxies");
            }
            m_clearedInputDataStore = true;
         }
         if (!initInputMetaDataStore(fileName).isSuccess()) {
            ATH_MSG_WARNING("Unable to initialize InputMetaDataStore");
         }
      }
   } else if (inc.type() == "EndInputFile") {
/*
      if (!m_inputDataStore->clearStore().isSuccess()) {
         ATH_MSG_WARNING("Unable to clear input MetaData Proxies");
      }
      m_clearedInputDataStore = true;
*/
      m_allowMetaDataStop = true;
   } else if (inc.type() == "LastInputFile") {
      if (!m_metaDataTools.release().isSuccess()) {
         ATH_MSG_WARNING("Cannot release " << m_metaDataTools);
      }
   } else if (inc.type() == "ShmProxy") {
      if (!m_clearedInputDataStore) {
         if (!m_inputDataStore->clearStore().isSuccess()) {
            ATH_MSG_WARNING("Unable to clear input MetaData Proxies");
         }
         m_clearedInputDataStore = true;
      }
      if (!addProxyToInputMetaDataStore(fileName).isSuccess()) {
         ATH_MSG_WARNING("Unable to add proxy to InputMetaDataStore");
      }
   }
}
//__________________________________________________________________________
StatusCode MetaDataSvc::transitionMetaDataFile(bool ignoreInputFile) {
   // Allow MetaDataStop only on Input file transitions
   if (!m_allowMetaDataStop && !ignoreInputFile) {
      return(StatusCode::FAILURE);
   }
   // Set to be listener for end of event
   Incident metaDataStopIncident(name(), "MetaDataStop");
   m_incSvc->fireIncident(metaDataStopIncident);
   if (!m_metaDataTools.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release " << m_metaDataTools);
   }
/*
   if (!m_outputDataStore->clearStore().isSuccess()) {
      ATH_MSG_WARNING("Unable to clear input MetaData Proxies");
   }
*/
   AthCnvSvc* cnvSvc = dynamic_cast<AthCnvSvc*>(m_addrCrtr.operator->());
   if (cnvSvc) {
      if (!cnvSvc->disconnectOutput().isSuccess()) {
         ATH_MSG_WARNING("Cannot get disconnect Output Files");
      }
   }
   if (!m_metaDataTools.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get " << m_metaDataTools);
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode MetaDataSvc::io_reinit() {
   ATH_MSG_INFO("I/O reinitialization...");
   ATH_MSG_INFO("Dumping InputMetaDataStore: " << m_inputDataStore->dump());
   ATH_MSG_INFO("Dumping OutputMetaDataStore: " << m_outputDataStore->dump());
   for (ToolHandleArray<IAlgTool>::iterator iter = m_metaDataTools.begin(),
		   last = m_metaDataTools.end(); iter != last; iter++) {
      ATH_MSG_INFO("Attached MetadDataTool: " << (*iter)->name());
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode MetaDataSvc::rootOpenAction(FILEMGR_CALLBACK_ARGS) {
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode MetaDataSvc::addProxyToInputMetaDataStore(const std::string& tokenStr) {
   std::string fileName = tokenStr.substr(tokenStr.find("[FILE=") + 6);
   fileName = fileName.substr(0, fileName.find(']'));
   std::string className = tokenStr.substr(tokenStr.find("[PNAME=") + 7);
   className = className.substr(0, className.find(']'));
   std::string contName = tokenStr.substr(tokenStr.find("[CONT=") + 6);
   contName = contName.substr(0, contName.find(']'));
   std::size_t pos1 = contName.find('(');
   std::string keyName = contName.substr(pos1 + 1, contName.size() - pos1 - 2);
   std::size_t pos2 = keyName.find('/');
   if (pos2 != std::string::npos) keyName = keyName.substr(pos2 + 1);
   std::string numName = tokenStr.substr(tokenStr.find("[NUM=") + 5);
   numName = numName.substr(0, numName.find(']'));
   unsigned long num = 0;
   std::istringstream iss(numName);
   iss >> num;
   CLID clid = m_persToClid[className];
   const std::string par[2] = { "SHM" , className };
   const unsigned long ipar[2] = { num , 0 };
   IOpaqueAddress* opqAddr = nullptr;
   if (clid == 167728019) { // EventStreamInfo, will change tool to combine input metadata, clearing things before...
      bool foundTool = false;
      for (ToolHandleArray<IAlgTool>::const_iterator iter = m_metaDataTools.begin(), iterEnd = m_metaDataTools.end(); iter != iterEnd; iter++) {
         if ((*iter)->name() == "ToolSvc.CopyEventStreamInfo") foundTool = true;
      }
      if (!foundTool) {
         ServiceHandle<IIncidentListener> cfSvc("CutFlowSvc", this->name()); // Disable CutFlowSvc by stopping its incidents.
         if (cfSvc.retrieve().isSuccess()) {
            m_incSvc->removeListener(cfSvc.get(), IncidentType::BeginInputFile);
            m_incSvc->removeListener(cfSvc.get(), IncidentType::EndInputFile);
            m_incSvc->removeListener(cfSvc.get(), IncidentType::EndRun);
            m_incSvc->removeListener(cfSvc.get(), "StoreCleared");
            cfSvc.release().ignore();
         }
         if (!m_outputDataStore->clearStore().isSuccess()) {
            ATH_MSG_WARNING("Unable to clear output MetaData Proxies");
         }
      }
   }
   const std::string toolName = m_toolForClid[clid];
   if (!toolName.empty()) {
      bool foundTool = false;
      for (ToolHandleArray<IAlgTool>::const_iterator iter = m_metaDataTools.begin(), iterEnd = m_metaDataTools.end(); iter != iterEnd; iter++) {
         if ((*iter)->name() == "ToolSvc." + toolName) foundTool = true;
      }
      if (!foundTool) {
         ToolHandle<IAlgTool> metadataTool(toolName);
         m_metaDataTools.push_back(metadataTool);
         if (!metadataTool.retrieve().isSuccess()) {
            ATH_MSG_FATAL("Cannot get " << toolName);
            return(StatusCode::FAILURE);
         }
      }
   }

   if (!m_addrCrtr->createAddress(m_storageType, clid, par, ipar, opqAddr).isSuccess()) {
      ATH_MSG_FATAL("addProxyToInputMetaDataStore: Cannot create address for " << tokenStr);
      return(StatusCode::FAILURE);
   }
   if (m_inputDataStore->recordAddress(keyName, opqAddr).isFailure()) {
      delete opqAddr; opqAddr = nullptr;
      ATH_MSG_FATAL("addProxyToInputMetaDataStore: Cannot create proxy for " << tokenStr);
      return(StatusCode::FAILURE);
   }
   if (m_inputDataStore->accessData(clid, keyName) == nullptr) {
      ATH_MSG_FATAL("addProxyToInputMetaDataStore: Cannot access data for " << tokenStr);
      return(StatusCode::FAILURE);
   }
   std::map<std::string, std::string>::const_iterator iter = m_streamForKey.find(keyName);
   if (iter == m_streamForKey.end()) {
      m_streamForKey.insert(std::pair<std::string, std::string>(keyName, fileName));
   } else if (fileName != iter->second) { // Remove duplicated objects
      ATH_MSG_DEBUG("Resetting duplicate proxy for: " << clid << "#" << keyName << " from file: " << fileName);
      m_inputDataStore->proxy(clid, keyName)->reset();
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode MetaDataSvc::initInputMetaDataStore(const std::string& fileName) {
   ATH_MSG_DEBUG("initInputMetaDataStore: file name " << fileName);
   m_clearedInputDataStore = false;
   // Load proxies for InputMetaDataStore
   if (m_metaDataCont.value().empty()) {
      ATH_MSG_DEBUG("MetaDataSvc called without MetaDataContainer set.");
      return(StatusCode::SUCCESS);
   }
   if (fileName.find("BSF:") == 0) {
      ATH_MSG_DEBUG("MetaDataSvc called for non ROOT file.");
   } else if (fileName == "SHM") {
      ATH_MSG_DEBUG("MetaDataSvc called for shared memory.");
   } else {
      const std::string par[2] = { fileName,  m_metaDataCont.value() + "(DataHeader)" };
      const std::string parOld[2] = { fileName,  m_metaDataCont.value() + "DataHeader" };
      for (int verNumber = 0; verNumber < 100; verNumber++) {
         SG::VersionedKey myVersKey(name(), verNumber);
         if (m_inputDataStore->contains<DataHeader>(myVersKey)) {
            ATH_MSG_DEBUG("initInputMetaDataStore: MetaData Store already contains DataHeader, key = " << myVersKey);
         } else {
            const unsigned long ipar[2] = { (unsigned long)verNumber , 0 };
            IOpaqueAddress* opqAddr = nullptr;
            if (!m_addrCrtr->createAddress(m_storageType, ClassID_traits<DataHeader>::ID(), par, ipar, opqAddr).isSuccess()) {
               if (!m_addrCrtr->createAddress(m_storageType, ClassID_traits<DataHeader>::ID(), parOld, ipar, opqAddr).isSuccess()) {
                  break;
               }
            }
            if (m_inputDataStore->recordAddress(myVersKey, opqAddr).isFailure()) {
               delete opqAddr; opqAddr = nullptr;
               ATH_MSG_WARNING("initInputMetaDataStore: Cannot create proxy for DataHeader, key = " << myVersKey);
            }
         }
      }
      std::list<SG::TransientAddress*> tList;
      if (!loadAddresses(StoreID::METADATA_STORE, tList).isSuccess()) {
         ATH_MSG_WARNING("Unable to load MetaData Proxies");
      }
      for (SG::TransientAddress* tad : tList) {
         CLID clid = tad->clID();
         if (m_inputDataStore->contains(tad->clID(), tad->name())) {
            ATH_MSG_DEBUG("initInputMetaDataStore: MetaData Store already contains clid = " << clid << ", key = " << tad->name());
         } else {
            if (!m_inputDataStore->recordAddress(tad->name(), tad->address())) {
               ATH_MSG_ERROR("initInputMetaDataStore: Cannot create proxy for clid = " << clid << ", key = " << tad->name());
            }
         }

         for (CLID tclid : tad->transientID()) {
           if (tclid != clid) {
             if (m_inputDataStore->symLink (clid, tad->name(), tclid).isFailure()) {
               ATH_MSG_WARNING("Cannot make autosymlink from " <<
                               clid << "/" << tad->name() << " to " << tclid);
             }
           }
         }
         delete tad;
      }
      tList.clear();
   }
   ATH_MSG_DEBUG("Loaded input meta data store proxies");
   return(StatusCode::SUCCESS);
}
