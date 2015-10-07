/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaPoolCnvSvc.cxx
 *  @brief This file contains the implementation for the AthenaPoolCnvSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaPoolCnvSvc.h"

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/AttribStringParser.h"

#include "AthenaKernel/IAthenaIPCTool.h"
#include "AthenaKernel/IAthenaSerializeSvc.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "PersistentDataModel/Token.h"
#include "PersistentDataModel/TokenAddress.h"
#include "PoolSvc/IPoolSvc.h"
#include "StoreGate/StoreGate.h"

#include <set>

//______________________________________________________________________________
// Initialize the service.
StatusCode AthenaPoolCnvSvc::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   if (!::AthCnvSvc::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize ConversionSvc base class.");
      return(StatusCode::FAILURE);
   }

   // Initialize DataModelCompatSvc
   ServiceHandle<IService> dmcsvc("DataModelCompatSvc", this->name());
   if (!dmcsvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get DataModelCompatSvc.");
      return(StatusCode::FAILURE);
   }
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
   // Initialize AthenaRootStreamerSvc
   ServiceHandle<IService> arssvc("AthenaRootStreamerSvc", this->name());
   if (!arssvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get AthenaRootStreamerSvc.");
      return(StatusCode::FAILURE);
   }
#endif
   // Retrieve PoolSvc
   if (!m_poolSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get PoolSvc.");
      return(StatusCode::FAILURE);
   }
   // Retrieve ChronoStatSvc
   if (!m_chronoStatSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get ChronoStatSvc.");
      return(StatusCode::FAILURE);
   }
   // Retrieve ClassIDSvc
   if (!m_clidSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get ClassIDSvc.");
      return(StatusCode::FAILURE);
   }
   // Retrieve DataStreamingTool (if configured)
   if (!m_dataStreamingTool.empty()) {
      if (!m_dataStreamingTool.retrieve().isSuccess()) {
         ATH_MSG_FATAL("Cannot get AthenaIPCTool");
         return(StatusCode::FAILURE);
      }
      // Retrieve AthenaSerializeSvc
      if (!m_dataSerializeSvc.retrieve().isSuccess()) {
         ATH_MSG_FATAL("Cannot get AthenaSerializeSvc.");
         return(StatusCode::FAILURE);
      }
   }
   // Extracting MaxFileSizes for global default and map by Database name.
   for (std::vector<std::string>::const_iterator iter = m_maxFileSizes.value().begin(),
	   last = m_maxFileSizes.value().end(); iter != last; iter++) {
      if (iter->find('=') != std::string::npos) {
         long long maxFileSize = atoll(iter->substr(iter->find('=') + 1).c_str());
         if (maxFileSize > 10000000000LL) {
            ATH_MSG_WARNING("Files larger than 10GB are disallowed by ATLAS policy.");
            ATH_MSG_WARNING("They should only be produced for private use or in special cases.");
         }
         std::string databaseName = iter->substr(0, iter->find_first_of(" 	="));
         std::pair<std::string, long long> entry(databaseName, maxFileSize);
         m_databaseMaxFileSize.insert(entry);
      } else {
         m_domainMaxFileSize = atoll(iter->c_str());
         if (m_domainMaxFileSize > 10000000000LL) {
            ATH_MSG_WARNING("Files larger than 10GB are disallowed by ATLAS policy.");
            ATH_MSG_WARNING("They should only be produced for private use or in special cases.");
         }
      }
   }
   // Setting default 'TREE_MAX_SIZE' for ROOT to 1024 GB to avoid file chains.
   std::vector<std::string> maxFileSize;
   maxFileSize.push_back("TREE_MAX_SIZE");
   maxFileSize.push_back("1099511627776L");
   m_domainAttr.push_back(maxFileSize);
   // Extracting OUTPUT POOL ItechnologySpecificAttributes for Domain, Database and Container.
   extractPoolAttributes(m_poolAttr, &m_containerAttr, &m_databaseAttr, &m_domainAttr);
   // Extracting INPUT POOL ItechnologySpecificAttributes for Domain, Database and Container.
   extractPoolAttributes(m_inputPoolAttr, &m_inputAttr, &m_inputAttr);
   // Extracting the INPUT POOL ItechnologySpecificAttributes which are to be printed for each event
   extractPoolAttributes(m_inputPoolAttrPerEvent, &m_inputAttrPerEvent, &m_inputAttrPerEvent);
   if (!m_inputPoolAttrPerEvent.value().empty()) {
      // Setup incident for EndEvent to print out attributes each event
      ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", name());
      long int pri = 1000;
      // Set to be listener for EndEvent
      incSvc->addListener(this, "EndEvent", pri);
      ATH_MSG_DEBUG("Subscribed to EndEvent for printing out input file attributes.");
   }
   m_doChronoStat = m_skipFirstChronoCommit.value() ? false : true;
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::finalize() {
   // Release DataStreamingTool (if configured)
   if (!m_dataStreamingTool.empty()) {
      // Release AthenaSerializeSvc
      if (!m_dataSerializeSvc.release().isSuccess()) {
         ATH_MSG_WARNING("Cannot release AthenaSerializeSvc.");
      }
      if (!m_dataStreamingTool.release().isSuccess()) {
         ATH_MSG_WARNING("Cannot release AthenaIPCTool.");
      }
   }
   // Release ClassIDSvc
   if (!m_clidSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release ClassIDSvc.");
   }
   // Release ChronoStatSvc
   if (!m_chronoStatSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release ChronoStatSvc.");
   }
   // Release PoolSvc
   if (!m_poolSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release PoolSvc.");
   }

   m_cnvs.clear();
   m_cnvs.shrink_to_fit();
   return(::AthCnvSvc::finalize());
}
//_______________________________________________________________________
StatusCode AthenaPoolCnvSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if (IAthenaPoolCnvSvc::interfaceID().versionMatch(riid)) {
      *ppvInterface = dynamic_cast<IAthenaPoolCnvSvc*>(this);
   } else {
      // Interface is not directly available: try out a base class
      return(::AthCnvSvc::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::createObj(IOpaqueAddress* pAddress, DataObject*& refpObject) {
#ifdef ATHENAHIVE  
  std::lock_guard<CallMutex> lock(m_i_mut);
#endif

   assert(pAddress);
   if (m_useDetailChronoStat.value() && m_doChronoStat) {
      std::string objName, keyName = "#" + *(pAddress->par() + 1);
      if (m_clidSvc->getTypeNameOfID(pAddress->clID(), objName).isFailure()) {
         std::ostringstream oss;
         oss << std::dec << pAddress->clID();
         objName = oss.str();
      }
      objName += keyName;
      m_chronoStatSvc->chronoStart("cObj_" + objName);
      m_className.push_back(objName);
   }
   // Save pool input context to be used in setObjPtr for "this" converter
   m_contextIds.push_back(*(pAddress->ipar()));

   // Forward to base class createObj
   StatusCode status = ::AthCnvSvc::createObj(pAddress, refpObject);
   if (m_useDetailChronoStat.value() && m_doChronoStat) {
      std::string objName = m_className.back();
      m_className.pop_back();
      m_chronoStatSvc->chronoStop("cObj_" + objName);
   }
   // Remove pool input context for "this" converter
   m_contextIds.pop_back();
   return(status);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::createRep(DataObject* pObject, IOpaqueAddress*& refpAddress) {
#ifdef ATHENAHIVE  
  std::lock_guard<CallMutex> lock(m_o_mut);
#endif
   assert(pObject);
   if (m_useDetailChronoStat.value() && m_doChronoStat) {
      std::string objName, keyName = "#" + pObject->registry()->name();
      if (m_clidSvc->getTypeNameOfID(pObject->clID(), objName).isFailure()) {
         std::ostringstream oss;
         oss << std::dec << pObject->clID();
         objName = oss.str();
      }
      objName += keyName;
      m_chronoStatSvc->chronoStart("cRep_" + objName);
      m_className.push_back(objName);
   }
   StatusCode status = StatusCode::FAILURE;
   if (pObject->clID() == 1) {
      // No transient object was found use cnv to write default persistent object
      SG::DataProxy* proxy = dynamic_cast<SG::DataProxy*>(pObject->registry());
      if (proxy != 0) {
         IConverter* cnv = converter(proxy->clID());
         status = cnv->createRep(pObject, refpAddress);
      }
   } else {
      // Forward to base class createRep
      try { 
         status = ::AthCnvSvc::createRep(pObject, refpAddress);
      } catch(std::runtime_error& e) {
         ATH_MSG_FATAL(e.what());
      }
   }
   if (m_useDetailChronoStat.value() && m_doChronoStat) {
      std::string objName = m_className.back();
      m_className.pop_back();
      m_chronoStatSvc->chronoStop("cRep_" + objName);
   }
   return(status);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::fillRepRefs(IOpaqueAddress* pAddress, DataObject* pObject) {
   assert(pObject);
   if (m_useDetailChronoStat.value() && m_doChronoStat) {
      std::string objName, keyName = "#" + pObject->registry()->name();
      if (m_clidSvc->getTypeNameOfID(pObject->clID(), objName).isFailure()) {
         std::ostringstream oss;
         oss << std::dec << pObject->clID();
         objName = oss.str();
      }
      objName += keyName;
      m_chronoStatSvc->chronoStart("fRep_" + objName);
      m_className.push_back(objName);
   }
   StatusCode status = StatusCode::FAILURE;
   if (pObject->clID() == 1) {
      // No transient object was found use cnv to write default persistent object
      SG::DataProxy* proxy = dynamic_cast<SG::DataProxy*>(pObject->registry());
      if (proxy != 0) {
         IConverter* cnv = converter(proxy->clID());
         status = cnv->fillRepRefs(pAddress, pObject);
      }
   } else {
      try {
         // Forward to base class fillRepRefs
         status = ::AthCnvSvc::fillRepRefs(pAddress, pObject);
      } catch(std::runtime_error& e) {
         ATH_MSG_FATAL(e.what());
      } 
   }
   if (m_useDetailChronoStat.value() && m_doChronoStat) {
      std::string objName = m_className.back();
      m_className.pop_back();
      m_chronoStatSvc->chronoStop("fRep_" + objName);
   }
   return(status);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::connectOutput(const std::string& outputConnectionSpec,
		const std::string& /*openMode*/) {
   return(connectOutput(outputConnectionSpec));
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::connectOutput(const std::string& outputConnectionSpec) {
// This is called before DataObjects are being converted.
   try {
      if (!m_poolSvc->connect(pool::ITransaction::UPDATE).isSuccess()) {
         ATH_MSG_ERROR("connectOutput FAILED to open an UPDATE transaction.");
         return(StatusCode::FAILURE);
      }
   } catch (std::exception& e) {
      ATH_MSG_ERROR("connectOutput - caught exception: " << e.what());
      return(StatusCode::FAILURE);
   }
   // Reset streaming parameters to CnvSvc properties.
   m_dhContainerPrefix = "POOLContainer";
   m_containerPrefix = m_containerPrefixProp.value();
   m_containerNameHint = m_containerNameHintProp.value();
   m_branchNameHint = m_branchNameHintProp.value();

   // Override streaming parameters from StreamTool if requested.
   std::string::size_type pos1 = outputConnectionSpec.find("[");
   m_outputConnectionSpec = outputConnectionSpec.substr(0, pos1);
   while (pos1 != std::string::npos) {
      const std::string::size_type pos2 = outputConnectionSpec.find("=", pos1);
      const std::string key = outputConnectionSpec.substr(pos1 + 1, pos2 - pos1 - 1);
      const std::string::size_type pos3 = outputConnectionSpec.find("]", pos2);
      const std::string value = outputConnectionSpec.substr(pos2 + 1, pos3 - pos2 - 1);
      if (key == "OutputCollection") {
         m_dhContainerPrefix = value;
      } else if (key == "PoolContainerPrefix") {
         m_containerPrefix = value;
      } else if (key == "TopLevelContainerName") {
         m_containerNameHint = value;
      } else if (key == "SubLevelBranchName") {
         m_branchNameHint = value;
      }
      pos1 = outputConnectionSpec.find("[", pos3);
   }

   // Extract the technology
   StatusCode status = decodeOutputSpec(m_outputConnectionSpec, m_dbType);
   if (!status.isSuccess()) {
      ATH_MSG_ERROR("connectOutput FAILED extract file name and technology.");
      return(StatusCode::FAILURE);
   }
   status = processPoolAttributes(m_domainAttr, m_outputConnectionSpec, IPoolSvc::kOutputStream);
   if (!status.isSuccess()) {
      ATH_MSG_DEBUG("connectOutput failed process POOL domain attributes.");
   }
   status = processPoolAttributes(m_databaseAttr, m_outputConnectionSpec, IPoolSvc::kOutputStream);
   if (!status.isSuccess()) {
      ATH_MSG_DEBUG("connectOutput failed process POOL database attributes.");
   }
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::commitOutput(const std::string& /*outputConnectionSpec*/, bool doCommit) {
// This is called after all DataObjects are converted.
   if (m_useDetailChronoStat.value()) {
      m_chronoStatSvc->chronoStart("commitOutput");
   }
   StatusCode status = processPoolAttributes(m_domainAttr, m_outputConnectionSpec, IPoolSvc::kOutputStream);
   if (!status.isSuccess()) {
      ATH_MSG_DEBUG("commitOutput failed process POOL domain attributes.");
   }
   status = processPoolAttributes(m_databaseAttr, m_outputConnectionSpec, IPoolSvc::kOutputStream);
   if (!status.isSuccess()) {
      ATH_MSG_DEBUG("commitOutput failed process POOL database attributes.");
   }
   status = processPoolAttributes(m_containerAttr, m_outputConnectionSpec, IPoolSvc::kOutputStream);
   if (!status.isSuccess()) {
      ATH_MSG_DEBUG("commitOutput failed process POOL container attributes.");
   }
   static int commitCounter = 1;
   try {
      if ((commitCounter > m_commitInterval && m_commitInterval > 0) || doCommit) {
         commitCounter = 1;
         if (!m_poolSvc->commit(IPoolSvc::kOutputStream).isSuccess()) {
            ATH_MSG_ERROR("commitOutput FAILED to commit OutputStream.");
            return(StatusCode::FAILURE);
         }
      } else {
         commitCounter++;
         if (!m_poolSvc->commitAndHold(IPoolSvc::kOutputStream).isSuccess()) {
            ATH_MSG_ERROR("commitOutput FAILED to commitAndHold OutputStream.");
            return(StatusCode::FAILURE);
         }
      }
   } catch (std::exception& e) {
      ATH_MSG_ERROR("commitOutput - caught exception: " << e.what());
      return(StatusCode::FAILURE);
   }
   if (!this->cleanUp().isSuccess()) {
      ATH_MSG_ERROR("commitOutput FAILED to cleanup converters.");
      return(StatusCode::FAILURE);
   }
   // Check FileSize
   long long int currentFileSize = m_poolSvc->getFileSize(m_outputConnectionSpec, m_dbType.type(), IPoolSvc::kOutputStream);
   if (m_databaseMaxFileSize.find(m_outputConnectionSpec) != m_databaseMaxFileSize.end()) {
      if (currentFileSize > m_databaseMaxFileSize[m_outputConnectionSpec]) {
         ATH_MSG_WARNING("FileSize > MaxFileSize for " << m_outputConnectionSpec);
         return(StatusCode::RECOVERABLE);
      }
   } else if (currentFileSize > m_domainMaxFileSize) {
      ATH_MSG_WARNING("FileSize > domMaxFileSize for " << m_outputConnectionSpec);
      return(StatusCode::RECOVERABLE);
   }
   // For "safety" we reset the output file and the technology type
   m_outputConnectionSpec = "";
   m_dbType = pool::DbType();
   if (m_useDetailChronoStat.value()) {
      m_chronoStatSvc->chronoStop("commitOutput");
   }
   // prepare chrono for next commit
   m_doChronoStat = true;
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::disconnectOutput() {
   // Setting default 'TREE_MAX_SIZE' for ROOT to 1024 GB to avoid file chains.
   std::vector<std::string> maxFileSize;
   maxFileSize.push_back("TREE_MAX_SIZE");
   maxFileSize.push_back("1099511627776L");
   m_domainAttr.push_back(maxFileSize);
   // Extracting OUTPUT POOL ItechnologySpecificAttributes for Domain, Database and Container.
   extractPoolAttributes(m_poolAttr, &m_containerAttr, &m_databaseAttr, &m_domainAttr);
   return(m_poolSvc->disconnect(IPoolSvc::kOutputStream));
}
//______________________________________________________________________________
const std::string& AthenaPoolCnvSvc::getOutputConnectionSpec() const {
   return(m_outputConnectionSpec);
}
//______________________________________________________________________________
std::string AthenaPoolCnvSvc::getOutputContainer(const std::string& typeName,
		const std::string& key) const {
   if (typeName.substr(0, 14) == "DataHeaderForm") {
      return(m_dhContainerPrefix + "Form" + "(" + typeName + ")");
   }
   if (typeName.substr(0, 10) == "DataHeader") {
      if (key.substr(key.size() - 1) == "/") {
         return(m_dhContainerPrefix + "(" + key + typeName + ")");
      }
      return(m_dhContainerPrefix + "(" + typeName + ")");
   }
   if (typeName.substr(0, 13) == "AttributeList") {
      return(m_collContainerPrefix + "(" + key + ")");
   }
   if (key.empty()) {  
      return(m_containerPrefix + typeName);  
   }  
   const std::string typeTok = "<type>", keyTok = "<key>";
   std::string ret = m_containerPrefix + m_containerNameHint;
   if (!m_branchNameHint.empty()) {
      ret += "(" + m_branchNameHint + ")";
   }
   const std::size_t pos1 = ret.find(typeTok);
   if (pos1 != std::string::npos) {
      ret.replace(pos1, typeTok.size(), typeName);
   }
   const std::size_t pos2 = ret.find(keyTok);
   if (pos2 != std::string::npos) {
      if (key.empty()) {
         ret.replace(pos2, keyTok.size(), typeName);
      } else {
         ret.replace(pos2, keyTok.size(), key);
      }
   }
   return(ret);
}
//______________________________________________________________________________
pool::DbType AthenaPoolCnvSvc::technologyType() const {
   return(m_dbType);
}
//__________________________________________________________________________
IPoolSvc* AthenaPoolCnvSvc::getPoolSvc() {
   return(&*m_poolSvc);
}
//______________________________________________________________________________
const Token* AthenaPoolCnvSvc::registerForWrite(const pool::Placement* placement,
	const void* obj,
	const RootType& classDesc) const {
   if (m_useDetailChronoStat.value() && m_doChronoStat) {
      m_chronoStatSvc->chronoStart("cRepR_" + m_className.back());
   }
   const Token* token = m_poolSvc->registerForWrite(placement, obj, classDesc);
   if (m_useDetailChronoStat.value() && m_doChronoStat) {
      m_chronoStatSvc->chronoStop("cRepR_" + m_className.back());
   }
   return(token);
}
//______________________________________________________________________________
void AthenaPoolCnvSvc::setObjPtr(void*& obj, const Token* token) const {
   if (m_useDetailChronoStat.value() && m_doChronoStat) {
      m_chronoStatSvc->chronoStart("cObjR_" + m_className.back());
   }
   if (!m_dataStreamingTool.empty() && m_dataStreamingTool->isClient()) {
      void* buffer = 0;
      size_t nbytes = 0;
      if (!m_dataStreamingTool->getObject(token->toString().c_str(), &buffer, nbytes).isSuccess()) {
         ATH_MSG_WARNING("Failed to get Data for " << token->toString());
         obj = 0;
      } else {
         obj = m_dataSerializeSvc->deserialize(buffer, nbytes, token->classID());
         if (!this->releaseData().isSuccess()) {
            ATH_MSG_WARNING("Failed to release Data for " << token->toString());
         }
      }
   } else if (!m_dataStreamingTool.empty() && m_dataStreamingTool->isServer()) {
      // Reading in Server
      m_poolSvc->setObjPtr(obj, token);
   } else {
      m_poolSvc->setObjPtr(obj, token, m_contextIds.back());
   }
   if (m_useDetailChronoStat.value() && m_doChronoStat) {
      m_chronoStatSvc->chronoStop("cObjR_" + m_className.back());
   }
}
//______________________________________________________________________________
bool AthenaPoolCnvSvc::testDictionary(const std::string& className) const {
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,0)
   return(m_poolSvc->testDictionary(className));
#else
   ATH_MSG_DEBUG("Skipping Dictionary check for: " << className);
   return(true);
#endif
}
//______________________________________________________________________________
bool AthenaPoolCnvSvc::useDetailChronoStat() const {
   return(m_useDetailChronoStat.value());
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::createAddress(long svcType,
		const CLID& clid,
		const std::string* par,
		const unsigned long* ip,
		IOpaqueAddress*& refpAddress) {
   if (svcType != POOL_StorageType) {
      ATH_MSG_ERROR("createAddress: svcType != POOL_StorageType " << svcType << " " << POOL_StorageType);
      return(StatusCode::FAILURE);
   }
   Token* token = 0;
   if (!m_dataStreamingTool.empty() && m_dataStreamingTool->isClient()) {
      void* buffer = 0;
      size_t nbytes = 0;
      Token addressToken;
      addressToken.setDb(par[0].substr(4));
      addressToken.setCont(par[1]);
      addressToken.setOid(Token::OID_t(ip[0], ip[1]));
      if (!m_dataStreamingTool->getObject(addressToken.toString().c_str(), &buffer, nbytes).isSuccess()) {
         ATH_MSG_WARNING("Failed to get Address Token");
         return(StatusCode::FAILURE);
      } else {
         token = new Token();
         token->fromString((char*)buffer);
         if (token->classID() == Guid::null()) {
            return(StatusCode::RECOVERABLE);
         }
      }
      if (!this->releaseData().isSuccess()) {
         ATH_MSG_WARNING("Failed to release Address Token for " << token->toString());
      }
   } else {
      token = m_poolSvc->getToken(par[0], par[1], ip[0]);
   }
   if (token == 0) {
      return(StatusCode::RECOVERABLE);
   }
   refpAddress = new TokenAddress(POOL_StorageType, clid, "", "", 0, token);
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::createAddress(long svcType,
		const CLID& clid,
		const std::string& refAddress,
		IOpaqueAddress*& refpAddress) {
   if (svcType != POOL_StorageType) {
      ATH_MSG_ERROR("createAddress: svcType != POOL_StorageType " << svcType << " " << POOL_StorageType);
      return(StatusCode::FAILURE);
   }
   refpAddress = new GenericAddress(POOL_StorageType, clid, refAddress);
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::convertAddress(const IOpaqueAddress* pAddress,
		std::string& refAddress) {
   assert(pAddress);
   refAddress = *pAddress->par();
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::registerCleanUp(IAthenaPoolCleanUp* cnv) {
   m_cnvs.push_back(cnv);
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::cleanUp() {
   bool retError = false;
   for (std::vector<IAthenaPoolCleanUp*>::iterator iter = m_cnvs.begin(), last = m_cnvs.end();
		   iter != last; iter++) {
      if (!(*iter)->cleanUp().isSuccess()) {
         ATH_MSG_WARNING("AthenaPoolConverter cleanUp failed.");
         retError = true;
      }
   }
   return(retError ? StatusCode::FAILURE : StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::setInputAttributes(const std::string& fileName) {
   // Set attributes for input file
   m_lastFileName = fileName; // Save file name for printing attributes per event
   StatusCode status = processPoolAttributes(m_inputAttr, m_lastFileName, IPoolSvc::kInputStream, false, true, false);
   if (!status.isSuccess()) {
      ATH_MSG_DEBUG("setInputAttribute failed setting POOL database/container attributes.");
   }
   status = processPoolAttributes(m_inputAttr, m_lastFileName, IPoolSvc::kInputStream, true, false);
   if (!status.isSuccess()) {
      ATH_MSG_DEBUG("setInputAttribute failed getting POOL database/container attributes.");
   }
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::makeServer(int num) {
   ATH_MSG_DEBUG("makeServer: " << m_dataStreamingTool << " = " << num);
   if (m_dataStreamingTool.empty()) {
      return(StatusCode::RECOVERABLE);
   }
   m_doChronoStat = false;
   return(m_dataStreamingTool->makeServer(num));
}
//________________________________________________________________________________
StatusCode AthenaPoolCnvSvc::makeClient(int num) {
   ATH_MSG_DEBUG("makeClient: " << m_dataStreamingTool << " = " << num);
   if (m_dataStreamingTool.empty()) {
      return(StatusCode::RECOVERABLE);
   }
   return(m_dataStreamingTool->makeClient(num));
}
//________________________________________________________________________________
StatusCode AthenaPoolCnvSvc::readData() const {
   if (m_dataStreamingTool.empty()) {
      return(StatusCode::FAILURE);
   }
   char* tokenStr = 0;
   int num = 0;
   if (m_dataStreamingTool->lockObject(&tokenStr, num).isSuccess() && tokenStr != 0 && strlen(tokenStr) > 0) {
      ATH_MSG_DEBUG("readData: " << tokenStr << ", for client: " << num);
   } else {
      return(StatusCode::RECOVERABLE);
   }
   // Read object instance via POOL/ROOT
   void* instance = 0;
   Token token;
   token.fromString(tokenStr);
   if (token.classID() != Guid::null()) {
      this->setObjPtr(instance, &token);
      // Serialize object via ROOT
      void* buffer = 0;
      size_t nbytes = 0;
      buffer = m_dataSerializeSvc->serialize(instance, token.classID(), nbytes);
      // Share object
      while (m_dataStreamingTool->putObject(buffer, nbytes, num).isRecoverable()) {
         usleep(10000);
      }
      delete [] (char*)buffer; buffer = 0;
   } else {
      std::string returnToken = "";
      const Token* metadataToken = m_poolSvc->getToken("FID:" + token.dbID().toString(), token.contID(), token.oid().first);
      if (metadataToken != 0) {
         returnToken = metadataToken->toString();
      }
      delete metadataToken; metadataToken = 0;
      // Share object
      while (m_dataStreamingTool->putObject(returnToken.c_str(), returnToken.size() + 1, num).isRecoverable()) {
         usleep(10000);
      }
   }
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
bool AthenaPoolCnvSvc::ownsData() const {
   if (!m_dataStreamingTool.empty() && m_dataStreamingTool->isClient()) {
      return(true);
   }
   return(false);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::releaseData() const {
   if (m_dataStreamingTool.empty()) {
      return(StatusCode::FAILURE);
   }
   return(m_dataStreamingTool->unlockObject());
}
//______________________________________________________________________________
void AthenaPoolCnvSvc::handle(const Incident& incident) {
   if (incident.type() == "EndEvent") {
      StatusCode status = processPoolAttributes(m_inputAttrPerEvent, m_lastFileName, IPoolSvc::kInputStream);
      if (!status.isSuccess()) {
         ATH_MSG_DEBUG("handle EndEvent failed process POOL database attributes.");
      }
   }
}
//______________________________________________________________________________
AthenaPoolCnvSvc::AthenaPoolCnvSvc(const std::string& name, ISvcLocator* pSvcLocator) :
	::AthCnvSvc(name, pSvcLocator, POOL_StorageType),
	m_dbType(),
	m_outputConnectionSpec(),
	m_dhContainerPrefix("POOLContainer"),
	m_collContainerPrefix("POOLCollectionTree"),
	m_lastFileName(),
	m_className(),
	m_poolSvc("PoolSvc", name),
	m_chronoStatSvc("ChronoStatSvc", name),
	m_clidSvc("ClassIDSvc", name),
	m_dataStreamingTool("", this),
	m_dataSerializeSvc("AthenaRootSerializeSvc", name),
	m_containerPrefix(),
	m_containerNameHint(),
	m_branchNameHint(),
	m_domainMaxFileSize(10000000000LL),
	m_doChronoStat(true) {
   declareProperty("UseDetailChronoStat", m_useDetailChronoStat = false);
   declareProperty("PoolContainerPrefix", m_containerPrefixProp = "CollectionTree");
   declareProperty("TopLevelContainerName", m_containerNameHintProp = "");
   declareProperty("SubLevelBranchName", m_branchNameHintProp = "<type>/<key>");
   declareProperty("PoolAttributes", m_poolAttr);
   declareProperty("InputPoolAttributes", m_inputPoolAttr);
   declareProperty("PrintInputAttrPerEvt", m_inputPoolAttrPerEvent);
   declareProperty("MaxFileSizes", m_maxFileSizes);
   declareProperty("CommitInterval", m_commitInterval = 0);
   declareProperty("SkipFirstChronoCommit", m_skipFirstChronoCommit = false);
   declareProperty("DataStreamingTool", m_dataStreamingTool);
}
//______________________________________________________________________________
AthenaPoolCnvSvc::~AthenaPoolCnvSvc() {
}
//__________________________________________________________________________
StatusCode AthenaPoolCnvSvc::decodeOutputSpec(std::string& fileSpec,
	pool::DbType& outputTech) const {
   outputTech = pool::ROOTTREE_StorageType;
   if (fileSpec.find("oracle") == 0 || fileSpec.find("mysql") == 0) {
      outputTech = pool::POOL_RDBMS_StorageType;
   } else if (fileSpec.find("ROOTKEY:") == 0) {
      outputTech = pool::ROOTKEY_StorageType;
      fileSpec.erase(0, 8);
   } else if (fileSpec.find("ROOTTREE:") == 0) {
      outputTech = pool::ROOTTREE_StorageType;
      fileSpec.erase(0, 9);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
void AthenaPoolCnvSvc::extractPoolAttributes(const StringArrayProperty& property,
	std::vector<std::vector<std::string> >* contAttr,
	std::vector<std::vector<std::string> >* dbAttr,
	std::vector<std::vector<std::string> >* domAttr) const {
   std::vector<std::string> opt;
   std::string attributeName, containerName, databaseName, valueString;
   for (std::vector<std::string>::const_iterator iter = property.value().begin(),
           last = property.value().end(); iter != last; iter++) {
      opt.clear();
      attributeName.clear();
      containerName.clear();
      databaseName.clear();
      valueString.clear();
      using Gaudi::Utils::AttribStringParser;
      for (const AttribStringParser::Attrib& attrib : AttribStringParser (*iter)) {
         const std::string tag = attrib.tag;
         const std::string val = attrib.value;
         if (tag == "DatabaseName") {
            databaseName = val;
         } else if (tag == "ContainerName") {
            if (databaseName.empty()) {
               databaseName = "*";
            }
            containerName = val;
         } else {
            attributeName = tag;
            valueString = val;
         }
      }
      if (!attributeName.empty() && !valueString.empty()) {
         opt.push_back(attributeName);
         opt.push_back(valueString);
         if (!databaseName.empty()) {
            opt.push_back(databaseName);
            if (!containerName.empty()) {
               opt.push_back(containerName);
               if (containerName.substr(0, 6) == "TTree=") {
                  dbAttr->push_back(opt);
               } else {
                  contAttr->push_back(opt);
               }
            } else {
               opt.push_back("");
               dbAttr->push_back(opt);
            }
         } else if (domAttr != 0) {
            domAttr->push_back(opt);
         } else {
            opt.push_back("*");
            opt.push_back("");
            dbAttr->push_back(opt);
         }
      }
   }
}
//__________________________________________________________________________
StatusCode AthenaPoolCnvSvc::processPoolAttributes(std::vector<std::vector<std::string> >& attr,
	const std::string& fileName,
	unsigned long contextId,
	bool doGet,
	bool doSet,
	bool doClear) const {
   bool retError = false;
   if (!m_dataStreamingTool.empty() && m_dataStreamingTool->isClient()) doGet = false;
   for (std::vector<std::vector<std::string> >::iterator iter = attr.begin(), last = attr.end();
		   iter != last; ++iter) {
      if (iter->size() == 2) {
         const std::string& opt = (*iter)[0];
         std::string data = (*iter)[1];
         if (data == "int" || data == "DbLonglong" || data == "double" || data == "string") {
            if (doGet) {
               StatusCode status = m_poolSvc->getAttribute(opt, data, m_dbType.type(), contextId);
               if (!status.isSuccess()) {
                  ATH_MSG_DEBUG("getAttribute failed for domain attr " << opt);
                  retError = true;
               }
            }
         } else if (doSet) {
            StatusCode status = m_poolSvc->setAttribute(opt, data, m_dbType.type(), contextId);
            if (status.isSuccess()) {
               ATH_MSG_DEBUG("setAttribute " << opt << " to " << data);
               if (doClear) {
                  iter->clear();
               }
            } else {
               ATH_MSG_DEBUG("setAttribute failed for domain attr " << opt << " to " << data);
               retError = true;
            }
         }
      }
      if (iter->size() == 4) {
         const std::string& opt = (*iter)[0];
         std::string data = (*iter)[1];
         const std::string& file = (*iter)[2];
         const std::string& cont = (*iter)[3];
         if ((file == fileName || (file.substr(0, 1) == "*"
		         && file.find("," + fileName + ",") == std::string::npos))) {
            if (data == "int" || data == "DbLonglong" || data == "double" || data == "string") {
               if (doGet) {
                  StatusCode status = m_poolSvc->getAttribute(opt, data, m_dbType.type(), fileName, cont, contextId);
                  if (!status.isSuccess()) {
                     ATH_MSG_DEBUG("getAttribute failed for database/container attr " << opt);
                     retError = true;
                  }
               }
            } else if (doSet) {
               StatusCode status = m_poolSvc->setAttribute(opt, data, m_dbType.type(), fileName, cont, contextId);
               if (status.isSuccess()) {
                  ATH_MSG_DEBUG("setAttribute " << opt << " to " << data << " for db: " << fileName << " and cont: " << cont);
                  if (doClear) {
                     if (file.substr(0, 1) == "*") {
                        (*iter)[2] += "," + fileName + ",";
                     } else {
                        iter->clear();
                     }
                  }
               } else {
                  ATH_MSG_DEBUG("setAttribute failed for " << opt << " to " << data << " for db: " << fileName << " and cont: " << cont);
                  retError = true;
               }
            }
         }
      }
   }
   for (std::vector<std::vector<std::string> >::iterator iter = attr.begin(); iter != attr.end(); ) {
      if (iter->empty()) {
         iter = attr.erase(iter);
      } else {
         iter++;
      }
   }
   return(retError ? StatusCode::FAILURE : StatusCode::SUCCESS);
}
