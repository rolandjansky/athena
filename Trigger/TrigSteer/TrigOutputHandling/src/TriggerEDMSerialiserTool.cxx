/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <cstring>
#include <boost/core/demangle.hpp>
#include <boost/algorithm/string.hpp>
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/System.h"
#include "AthenaKernel/StorableConversions.h"
#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/debug.h"
#include "xAODCore/AuxContainerBase.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "TrigSerializeResult/StringSerializer.h"

#include "TriggerEDMSerialiserTool.h"
#include "TriggerEDMCLIDs.h"

#include <numeric>

namespace {
  // Special module ID used internally to store total result size limit in the truncation threshold map
  constexpr uint16_t fullResultTruncationID = std::numeric_limits<uint16_t>::max();
}

TriggerEDMSerialiserTool::TriggerEDMSerialiserTool( const std::string& type,
						    const std::string& name,
						    const IInterface* parent )
  : base_class( type, name, parent ) {}

TriggerEDMSerialiserTool::~TriggerEDMSerialiserTool() {}

StatusCode TriggerEDMSerialiserTool::initialize() {
  // Initialise tools and services
  ATH_CHECK( m_serializerSvc.retrieve() );
  ATH_CHECK( m_clidSvc.retrieve() );
  ATH_CHECK( m_debugInfoWHKey.initialize() );
  if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());
  // Parse the list of collections to serialise
  for ( const std::string& typeKeyAuxIDs : m_collectionsToSerialize.value() ) {
    ATH_CHECK(addCollectionToSerialise(typeKeyAuxIDs, m_toSerialise));
  }

  // Retrieve the total result size limit from DataFlowConfig which is a special object
  // used online to hold DF properties passed from TDAQ to HLT as run parameters
  SmartIF<IJobOptionsSvc> jobOptionsSvc = service<IJobOptionsSvc>("JobOptionsSvc", /*createIf=*/ false);
  if (!jobOptionsSvc.isValid()) {
    ATH_MSG_WARNING("Could not retrieve JobOptionsSvc, will not update the EventSizeHardLimitMB property");
  }
  else {
    const Gaudi::Details::PropertyBase* prop = jobOptionsSvc->getClientProperty("DataFlowConfig", "DF_MaxEventSizeMB");
    if (prop && m_eventSizeHardLimitMB.assign(*prop)) {
      ATH_MSG_DEBUG("Updated EventSizeHardLimitMB to " << m_eventSizeHardLimitMB.value()
                    << " from DataFlowConfig.DF_MaxEventSizeMB");
    }
    else {
      ATH_MSG_DEBUG("Could not retrieve DataFlowConfig.DF_MaxEventSizeMB from JobOptionsSvc. This is fine if running "
                    << "offline, but should not happen online. Leaving EventSizeHardLimitMB="
                    << m_eventSizeHardLimitMB.value());
    }
  }

  // Add the total result size limit to truncation threshold map
  if (m_eventSizeHardLimitMB >= 0) {
    if (m_fullResultTruncationFrac > 1.0) {
      ATH_MSG_ERROR("Fraction cannot be > 1.0, but FullResultTruncationFrac is set to " << m_fullResultTruncationFrac);
      return StatusCode::FAILURE;
    }
    float totalResultSizeLimitBytes = m_fullResultTruncationFrac * m_eventSizeHardLimitMB * 1024. * 1024.;
    m_truncationThresholds[fullResultTruncationID] = static_cast<uint32_t>(totalResultSizeLimitBytes);
  }
  else {
    m_truncationThresholds[fullResultTruncationID] = std::numeric_limits<uint32_t>::max();
  }

  return StatusCode::SUCCESS;
}

StatusCode TriggerEDMSerialiserTool::addCollectionToSerialise(const std::string& typeKeyAuxIDs, std::vector<Address>& addressVec) const {
  ATH_MSG_DEBUG("Parsing " << typeKeyAuxIDs);
  const std::string typeKeyAux = typeKeyAuxIDs.substr( 0, typeKeyAuxIDs.find(';') );
  const std::string configuredType = typeKeyAux.substr( 0, typeKeyAux.find('#') );
  const std::string key = typeKeyAux.substr( typeKeyAux.find('#')+1, typeKeyAux.find('.')-typeKeyAux.find('#') );

  std::string transientType;
  std::string persistentType;

  if ( configuredType.find('_') == std::string::npos ) {
    transientType = configuredType;
  } else {
    transientType  = configuredType.substr( 0, configuredType.find('_') );
  }
  CLID clid;
  if ( m_clidSvc->getIDOfTypeName(transientType, clid).isFailure() )  {
    ATH_MSG_ERROR( "Can not find CLID for " << transientType << " that is needed for serialisation " << key );
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("Decoded transient type: " << transientType << " with the CLID " << clid );
  if ( transientType == configuredType ) {
    std::string realTypeName;
    if( m_clidSvc->getTypeInfoNameOfID( clid, realTypeName ).isFailure() ) {
      ATH_MSG_ERROR( "Can not find real type name for " << transientType << " that is needed for serialisation " << key );
      return StatusCode::FAILURE;
    }
    persistentType = transientType + version( realTypeName );
    ATH_MSG_VERBOSE(transientType << " = "<< configuredType << " thus obtained real type name from clid svc " << realTypeName << " forming persistent type name "<< persistentType );
  } else {
    persistentType = configuredType;
  }

  ATH_MSG_DEBUG( "Persistent type: " << persistentType );

  RootType classDesc = RootType::ByNameNoQuiet( persistentType );
  if ( ! classDesc.IsComplete() ) {
    ATH_MSG_ERROR( "The type " << persistentType <<  " is not known to ROOT serialiser" );
    return StatusCode::FAILURE;
  }

  const std::string moduleIDs  = typeKeyAuxIDs.substr( typeKeyAuxIDs.find(';')+1 );
  std::vector<std::string> splitModuleIDs;
  boost::split( splitModuleIDs, moduleIDs, [](const char c){ return c == ','; } );
  std::vector<uint16_t> moduleIdVec;
  for ( const auto& module: splitModuleIDs ) moduleIdVec.push_back( std::stoi( module ) );

  if (moduleIdVec.empty()) {
    ATH_MSG_ERROR( "No HLT result module IDs given for " << typeKeyAux );
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "Transient type " << transientType << " persistent type " << persistentType << " will be written to " << moduleIdVec.size() << " result ROBFragments with IDs: "
      << moduleIdVec << "" );

  if ( persistentType.find("xAOD") != std::string::npos ) { // xAOD - either interface of Aux
    xAOD::AuxSelection sel;
    if ( typeKeyAux.find('.') != std::string::npos ) { // Aux, possibly with selection of variables
      ATH_MSG_DEBUG( "with aux content: "  );
      const std::string allVars = typeKeyAux.substr( typeKeyAux.find('.')+1 );
      if (!allVars.empty()) {
        std::set<std::string> variableNames;
        boost::split( variableNames, allVars, [](const char c){ return c == '.'; } );
        for ( auto el: variableNames ) {
          ATH_MSG_DEBUG( " \"" << el << "\""  );
        }
        sel.selectAux( variableNames );
      }
      addressVec.emplace_back( transientType, persistentType, clid, key, moduleIdVec, Address::xAODAux, sel );
    } else {
    addressVec.emplace_back( transientType, persistentType, clid, key, moduleIdVec, Address::xAODInterface, xAOD::AuxSelection() );
    }
  } else { // an old T/P type
    addressVec.emplace_back( transientType, persistentType, clid, key, moduleIdVec, Address::OldTP, xAOD::AuxSelection() );
  }
  return StatusCode::SUCCESS;
}

StatusCode TriggerEDMSerialiserTool::makeHeader(const Address& address, std::vector<uint32_t>& buffer  ) const {
  buffer.push_back(0); // fragment size placeholder
  buffer.push_back( address.clid ); // type info via CLID

  std::vector<uint32_t> serializedLabel;
  StringSerializer ss;
  std::vector<std::string> descr({address.persType, address.key});
  ss.serialize( descr, serializedLabel );
  buffer.push_back( serializedLabel.size() );
  buffer.insert( buffer.end(), serializedLabel.begin(), serializedLabel.end() ); // plain SG key
  return StatusCode::SUCCESS;
}

StatusCode TriggerEDMSerialiserTool::fillPayload( const void* data, size_t sz, std::vector<uint32_t>& buffer ) const {
  ATH_CHECK( sz != 0 );
  ATH_CHECK( data != nullptr );

  buffer.push_back( sz ); // size in bytes
  const size_t neededSize = std::ceil( double(sz)/sizeof(uint32_t) );
  const size_t existingSize = buffer.size();
  buffer.resize(existingSize + neededSize);
  std::memcpy(buffer.data() + existingSize, data, sz);
  return StatusCode::SUCCESS;
}



StatusCode TriggerEDMSerialiserTool::serialiseDynAux( DataObject* dObj, const Address& address, std::vector<uint32_t>& buffer, size_t& nDynWritten ) const {
  ATH_MSG_DEBUG( "" );
  ATH_MSG_DEBUG( "About to start streaming aux data of " << address.key );
  DataBucketBase* dObjAux = dynamic_cast<DataBucketBase*>(dObj);
  ATH_CHECK( dObjAux != nullptr );

  const SG::IAuxStoreIO* auxStoreIO = dObjAux->template cast<SG::IAuxStoreIO> (nullptr, true);
  if ( auxStoreIO == nullptr ) {
    ATH_MSG_DEBUG( "Can't obtain AuxContainerBase of " << address.key <<  " no dynamic variables presumably" );
    return StatusCode::SUCCESS;
  }

  const SG::auxid_set_t& selected = address.sel.getSelectedAuxIDs( auxStoreIO->getDynamicAuxIDs() );

  if ( selected.empty() ) {
    ATH_MSG_VERBOSE( "Empty set of dynamic variables to store, do nothing" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Ready for serialisation of " << selected.size() << " dynamic variables");

  for (SG::auxid_t auxVarID : selected ) {

    const std::string decorationName = SG::AuxTypeRegistry::instance().getName(auxVarID);
    const std::type_info* tinfo = auxStoreIO->getIOType (auxVarID);
    const std::string typeName = SG::AuxTypeRegistry::instance().getVecTypeName(auxVarID);
    const std::string fullTypeName = System::typeinfoName( *tinfo );

    ATH_CHECK( tinfo != nullptr );
    TClass* cls = TClass::GetClass (*tinfo);
    ATH_CHECK( cls != nullptr );
    ATH_MSG_DEBUG( "" );
    ATH_MSG_DEBUG( "Streaming '" << decorationName << "' of type '" << typeName 
      << "' fulltype '" << fullTypeName << "' aux ID '" << auxVarID << "' class '" << cls->GetName() );

    CLID clid;
    if ( m_clidSvc->getIDOfTypeName(typeName, clid).isFailure() ) { // First try
      if ( m_clidSvc->getIDOfTypeInfoName(fullTypeName, clid).isFailure() ) { // Second try
        ATH_MSG_ERROR("Unable to obtain CLID for either typeName:" << typeName << " or fullTypeName:" << fullTypeName);
        ATH_MSG_ERROR("Please check if this is something which should obtain a CLID via TriggerEDMCLIDs.h");
        return StatusCode::FAILURE;
      }
    }
    ATH_MSG_DEBUG( "CLID " << clid );

    RootType classDesc = RootType::ByNameNoQuiet( cls->GetName() );

    const void* rawptr = auxStoreIO->getIOData( auxVarID );
    ATH_CHECK( rawptr != nullptr );

    size_t sz=0;
    void* mem = m_serializerSvc->serialize( rawptr, classDesc, sz );

    if ( mem == nullptr or sz == 0 ) {
      ATH_MSG_ERROR( "Serialisation of " << address.persType <<"#" << address.key << "."<< decorationName << " unsuccessful" );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG( "Serialised " << address.persType <<"#" << address.key << "."<< decorationName  << " memory size " << sz );
    }

    std::vector<uint32_t> fragment;

    Address auxAddress = { typeName, cls->GetName(), clid, decorationName, address.moduleIdVec, Address::xAODDecoration };

    ATH_CHECK( makeHeader( auxAddress, fragment ) );
    ATH_CHECK( fillPayload( mem, sz, fragment ) );
    fragment[0] = fragment.size();

    if ( mem ) delete [] static_cast<const char*>( mem );

    buffer.insert( buffer.end(), fragment.begin(), fragment.end() );
    ++nDynWritten;
  }
  return StatusCode::SUCCESS;
}


StatusCode TriggerEDMSerialiserTool::serialiseContainer( void* data, const Address& address, std::vector<uint32_t>& buffer ) const {

  RootType classDesc = RootType::ByNameNoQuiet( address.persType );
  size_t sz=0;
  void* mem = m_serializerSvc->serialize( data, classDesc, sz );

  ATH_MSG_DEBUG( "Streamed to buffer at address " << mem << " of " << sz << " bytes" );

  if ( mem == nullptr or sz == 0 ) {
    ATH_MSG_ERROR( "Serialisation of " << address.persType << " " << address.key << " unsuccessful" );
    return StatusCode::FAILURE;
  }

  // prepare fragment
  std::vector<uint32_t> fragment;
  ATH_CHECK( makeHeader( address, fragment ) );
  ATH_CHECK( fillPayload( mem, sz, fragment ) );
  if ( mem ) delete [] static_cast<const char*>( mem );


  ATH_MSG_DEBUG( address.transType << "#" << address.key << " Fragment size :" << fragment.size()*sizeof(uint32_t) << " bytes");
  fragment[0] = fragment.size();
  buffer.insert( buffer.end(), fragment.begin(), fragment.end() );

  return StatusCode::SUCCESS;
}

StatusCode TriggerEDMSerialiserTool::serialisexAODAuxContainer( void* data,
  const Address& address,
  std::vector<uint32_t>& buffer,
  SGImplSvc* evtStore) const
{
  ATH_MSG_DEBUG("xAOD Aux Container");
  ATH_CHECK( serialiseContainer( data, address, buffer ) );
  size_t baseSize = buffer.size();
  if ( not m_saveDynamic )
    return StatusCode::SUCCESS;

  DataObject* dObj = evtStore->accessData( address.clid, address.key );
  ATH_CHECK( dObj != nullptr );
  size_t nDynWritten = 0;
  ATH_CHECK( serialiseDynAux( dObj, address, buffer, nDynWritten ) );
  if ( nDynWritten > 0 ) {
    ATH_MSG_DEBUG( "   Fragment size including " <<  (buffer.size() - baseSize)*sizeof(uint32_t)  << " bytes from "
		   << nDynWritten << "x DynAux : " << buffer.size()*sizeof(uint32_t) );
  }
  return StatusCode::SUCCESS;
}

StatusCode TriggerEDMSerialiserTool::serialiseTPContainer( void* data, const Address& address, std::vector<uint32_t>& buffer ) const {

  ATH_MSG_DEBUG("TP Container, converting from: " <<  address.transType << " to " << address.persType  );
  std::string converterPersistentType;
  void * persistent = m_tpTool->convertTP( address.transType,  data, converterPersistentType );
  ATH_CHECK( persistent != 0 );
  ATH_CHECK ( converterPersistentType == address.persType );
  ATH_CHECK( serialiseContainer( persistent, address, buffer ) );

  return StatusCode::SUCCESS;
}

StatusCode TriggerEDMSerialiserTool::serialise( const Address& address, std::vector<uint32_t>& buffer, SGImplSvc* evtStore ) const {
  DataObject* dObj = evtStore->accessData( address.clid, address.key );
  if ( dObj == nullptr ) {
    ATH_MSG_DEBUG("Data Object with the CLID " << address.clid << " and the key " << address.key << " is missing");
    return StatusCode::SUCCESS;
  }

  void* rawptr = SG::fromStorable( dObj, address.clid, nullptr, msgLvl(MSG::DEBUG) );
  if ( rawptr == nullptr ) {
    ATH_MSG_DEBUG( "Data Object with key " << address.key << " can not be converted to void* for streaming" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG("Obtained raw pointer " << rawptr );

  if ( address.category == Address::xAODInterface ) {
    return serialiseContainer( rawptr, address, buffer );
  } else if ( address.category == Address::xAODAux ) {
    return serialisexAODAuxContainer( rawptr, address, buffer, evtStore );
  } else if ( address.category == Address::OldTP ) {
    return serialiseTPContainer( rawptr, address, buffer );
  } else {
    ATH_MSG_ERROR("Unknown Address category - neither of xAODInterface, xAODAux, OldTP");
    return StatusCode::FAILURE;
  }
  return StatusCode::FAILURE;
}

StatusCode TriggerEDMSerialiserTool::fill( HLT::HLTResultMT& resultToFill, const EventContext& ctx ) const {

  // Leave this check until there is a justified case for appending data to an existing result
  if (not resultToFill.getSerialisedData().empty()) {
    ATH_MSG_ERROR("Trying to fill a result which is not empty! Likely misconfiguration, returning a FAILURE");
    return StatusCode::FAILURE;
  }

  SGImplSvc* evtStore = static_cast<SGImplSvc*>(Atlas::getExtendedEventContext(ctx).proxy());
  ATH_CHECK( evtStore != nullptr );

  // Map storing information to be written out in case of truncation for each module
  TruncationInfoMap truncationInfoMap;

  // Record debug info container to be filled in case of truncation
  auto debugInfo = SG::makeHandle(m_debugInfoWHKey, ctx);
  auto debugInfoData = std::make_unique<xAOD::TrigCompositeContainer>();
  auto debugInfoAux  = std::make_unique<xAOD::TrigCompositeAuxContainer>();
  debugInfoData->setStore(debugInfoAux.get());
  ATH_CHECK(debugInfo.record(std::move(debugInfoData), std::move(debugInfoAux)));

  // Create buffer for serialised data
  std::vector<uint32_t> buffer;
  buffer.reserve(1000);

  for ( const Address& address: m_toSerialise ) {
    buffer.clear();
    ATH_MSG_DEBUG( "Streaming " << address.persTypeName() );
    ATH_CHECK( serialise(address, buffer, evtStore) );
    if (buffer.empty()) {
      ATH_MSG_DEBUG("Streaming of " << address.persTypeName() << " is skipped");
      continue;
    }

    const size_t thisFragmentSize = buffer.size()*sizeof(uint32_t);
    ATH_MSG_DEBUG( "Serialised size of " << address.persTypeName() << " is " << thisFragmentSize << " bytes" );

    for (const uint16_t id : address.moduleIdVec) {
      // If result not yet truncated, try adding the serialised data
      if (resultToFill.getTruncatedModuleIds().count(id)==0) {
        ATH_CHECK(tryAddData(resultToFill,id,buffer));
      }
      // Check for truncation after adding data
      if (resultToFill.getTruncatedModuleIds().count(id)==0) {
        ATH_MSG_DEBUG("Module " << id << " payload after inserting " << address.persTypeName() << " has "
                      << resultToFill.getSerialisedData().at(id).size()*sizeof(uint32_t) << " bytes");
        truncationInfoMap[id].emplace_back(&address, thisFragmentSize, true);
      }
      else {
        ATH_MSG_WARNING("HLTResult with module ID " << id << " truncated - could not add " << address.persTypeName());
        truncationInfoMap[id].emplace_back(&address, thisFragmentSize, false);
      }
    }
  }

  // Handle truncation
  ATH_CHECK(fillDebugInfo(truncationInfoMap, *debugInfo, resultToFill, evtStore));

  return StatusCode::SUCCESS;
}

StatusCode TriggerEDMSerialiserTool::tryAddData(HLT::HLTResultMT& hltResult,
                                                const uint16_t id,
                                                const std::vector<uint32_t>& data) const {
  if (m_truncationThresholds.value().count(id)==0) {
    ATH_MSG_ERROR("Module ID " << id << " missing from TruncationThresholds map. Cannot determine if result needs truncation");
    return StatusCode::FAILURE;
  }

  // Size in this module
  const uint32_t currentSizeBytes = hltResult.getSerialisedData().count(id)==0
                                    ? 0 : hltResult.getSerialisedData().at(id).size()*sizeof(uint32_t);
  // Total size
  size_t currentTotalSizeWords = 0;
  for (const auto& [id, data] : hltResult.getSerialisedData()) currentTotalSizeWords += data.size();
  const uint32_t currentTotalSizeBytes = currentTotalSizeWords*sizeof(uint32_t);
  // Size to be added
  const uint32_t extraSizeBytes = data.size()*sizeof(uint32_t);

  if (currentTotalSizeBytes+extraSizeBytes > m_truncationThresholds.value().at(fullResultTruncationID)) {
    // The data doesn't fit, flag the full result as truncated
    ATH_MSG_DEBUG("Skipping adding data to result with module ID " << id << " because of full-result truncation");
    hltResult.addTruncatedModuleId(fullResultTruncationID);
    hltResult.addTruncatedModuleId(id);
  }
  else if (currentSizeBytes+extraSizeBytes > m_truncationThresholds.value().at(id)) {
    // The data doesn't fit, flag this module's result as truncated
    ATH_MSG_DEBUG("Skipping adding data to truncated result with module ID " << id);
    hltResult.addTruncatedModuleId(id);
  }
  else {
    // The data fits, so add it to the result
    ATH_MSG_DEBUG("Adding data to result with module ID " << id);
    hltResult.addSerialisedData(id, data);
  }
  return StatusCode::SUCCESS;
}

StatusCode TriggerEDMSerialiserTool::fillDebugInfo(const TruncationInfoMap& truncationInfoMap,
                                                   xAOD::TrigCompositeContainer& debugInfoCont,
                                                   HLT::HLTResultMT& resultToFill,
                                                   SGImplSvc* evtStore) const {
  // If full result truncation happened, flag all results as truncated to produce debug info for all
  if (resultToFill.getTruncatedModuleIds().count(fullResultTruncationID)>0) {
    ATH_MSG_ERROR("HLT result truncation on total size! Limit of "
                  << m_truncationThresholds.value().at(fullResultTruncationID)/1024./1024.
                  << " MB exceeded. Flagging all module IDs as truncated.");
    for (const auto& [id, data] : resultToFill.getSerialisedData()) {
      resultToFill.addTruncatedModuleId(id);
    }
  }
  // Loop over truncation info and fill histograms and debug info objects
  for (const auto& [id, truncationInfoVec] : truncationInfoMap) {
    if (resultToFill.getTruncatedModuleIds().count(id)>0) {
      // Create and fill a debug info object
      xAOD::TrigComposite* debugInfoThisModule = new xAOD::TrigComposite;
      debugInfoCont.push_back(debugInfoThisModule); // the container in event store takes ownership of debugInfoThisModule
      xAOD::TrigComposite::Accessor<uint16_t> moduleId("moduleId");
      xAOD::TrigComposite::Accessor<uint32_t> totalSize("totalSize");
      xAOD::TrigComposite::Accessor<std::vector<std::string>> typeNameVec("typeName");
      xAOD::TrigComposite::Accessor<std::vector<uint32_t>> sizeVec("size");
      xAOD::TrigComposite::Accessor<std::vector<char>> isRecordedVec("isRecorded");
      std::pair<std::string, size_t> largestRecorded{"None", 0};
      std::pair<std::string, size_t> largestDropped{"None", 0};
      moduleId(*debugInfoThisModule) = id;
      uint32_t sizeSum = 0;
      for (const TruncationInfo& truncationInfo : truncationInfoVec) {
        // Store typeName and size information
        sizeSum += truncationInfo.size;
        typeNameVec(*debugInfoThisModule).push_back(truncationInfo.addrPtr->persTypeName());
        sizeVec(*debugInfoThisModule).push_back(truncationInfo.size);
        isRecordedVec(*debugInfoThisModule).push_back(truncationInfo.recorded);
        if (truncationInfo.recorded && truncationInfo.size > largestRecorded.second) {
          largestRecorded = {truncationInfo.addrPtr->persTypeName(), truncationInfo.size};
        }
        if (!truncationInfo.recorded && truncationInfo.size > largestDropped.second) {
          largestDropped = {truncationInfo.addrPtr->persTypeName(), truncationInfo.size};
        }
      }
      totalSize(*debugInfoThisModule) = sizeSum;
      ATH_MSG_ERROR("HLT result truncation. Module ID: " << id << ", limit: "
                    << m_truncationThresholds.value().at(id)/1024. << " kB, total size: "
                    << sizeSum/1024. << " kB, largest recorded collection: " << largestRecorded.first
                    << " (" << largestRecorded.second/1024. << " kB), largest dropped collection: "
                    << largestDropped.first << " (" << largestDropped.second/1024. << " kB).");
      // Monitoring
      auto monModuleId = Monitored::Scalar<int>("Truncation_ModuleId", id);
      auto monTotalSize = Monitored::Scalar<float>("Truncation_TotalSize", sizeSum/1024.);
      auto monLargestName = Monitored::Scalar<std::string>(
        "Truncation_LargestName",
        largestRecorded.second > largestDropped.second ? largestRecorded.first : largestDropped.first);
      auto monLargestSize = Monitored::Scalar<float>(
        "Truncation_LargestSize",
        largestRecorded.second > largestDropped.second ? largestRecorded.second/1024. : largestDropped.second/1024.);
      auto mon = Monitored::Group(m_monTool, monModuleId, monTotalSize, monLargestName, monLargestSize);
    }
  }
  // Serialise and write the debug info only in case of truncation
  if (!debugInfoCont.empty()) {
    std::vector<Address> debugInfoAddressVec;
    const std::string debugInfoID = std::string("xAOD::TrigCompositeContainer#")+m_debugInfoWHKey.key()+";0";
    const std::string debugInfoAuxID = std::string("xAOD::TrigCompositeAuxContainer#")+m_debugInfoWHKey.key()+"Aux.;0";
    ATH_CHECK(addCollectionToSerialise(debugInfoID, debugInfoAddressVec));
    ATH_CHECK(addCollectionToSerialise(debugInfoAuxID, debugInfoAddressVec));
    std::vector<uint32_t> buffer;
    for (const Address& address : debugInfoAddressVec) {
      buffer.clear();
      ATH_CHECK( serialise(address, buffer, evtStore) );
      resultToFill.addSerialisedData(0, buffer); // 0 is the main result ID
      // It would be better to avoid hard-coding main result ID but it should not be configurable either
    }
  }
  return StatusCode::SUCCESS;
}

std::string TriggerEDMSerialiserTool::version( const std::string& name ) const {
  if ( name.find("DataVector") != std::string::npos ) {
    size_t start = name.find("_");
    return name.substr( start, name.find(">") - start );
  }
  if ( name.find("_") != std::string::npos )
    return name.substr( name.find("_") );
  return "";
}
