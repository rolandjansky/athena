/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ModuleVetoSvc.cxx
 * implementation file for service allowing one to declare modules as bad
 * @author shaun.roe@cern.ch
 **/

#include "SCT_ModuleVetoSvc.h"
//STL includes
#include <vector>
#include <list>
#include <algorithm>
#include <sstream>
#include <iterator>

//Gaudi includes
#include "GaudiKernel/StatusCode.h"

#include "Identifier/IdentifierHash.h"

#include "InDetIdentifier/SCT_ID.h"

static const std::string databaseSignature("database");
static const std::string coolFolderName("/SCT/Manual/BadModules"); //a single-channel folder in the DB

template <class T> 
static std::vector<T> 
string2Vector(const std::string & s){
  std::vector<T> v;
  std::istringstream inputStream(s);
  std::istream_iterator<T> vecRead(inputStream);
  std::istream_iterator<T> endOfString; //relies on default constructor to produce eof
  std::copy(vecRead,endOfString,std::back_inserter(v));// DOESN'T ALLOW NON-WHITESPACE DELIMITER !
  return v;
}

// Constructor
SCT_ModuleVetoSvc::SCT_ModuleVetoSvc( const std::string& name, ISvcLocator* pSvcLocator ) :
  AthService(name, pSvcLocator),
  m_filled{false},
  m_pHelper{nullptr},
  m_useDatabase{false},
  m_maskLayers{false},
  m_maskSide{-1},
  m_detStore("DetectorStore",name)
  {
    declareProperty("BadModuleIdentifiers",m_badElements);
    declareProperty("MaskLayers",  m_maskLayers, "Mask full layers/disks in overlay" ); 
    declareProperty("MaskSide",  m_maskSide, "Mask full modules (-1), innwe (0) or outer (1) sides" );  
    declareProperty("LayersToMask", m_layersToMask, "Which barrel layers to mask out, goes from 0 to N-1"); 
    declareProperty("DisksToMask", m_disksToMask, "Which endcap disks to mask out, goes from -N+1 to N+1 , skipping zero");
  }

//Initialize
StatusCode 
SCT_ModuleVetoSvc::initialize(){
  StatusCode sc{StatusCode::SUCCESS};

  if(m_maskLayers &&  !m_layersToMask.size() && !m_disksToMask.size()) {
    ATH_MSG_DEBUG( "Layer/Disk masking enabled, but no layer/disk specified!" );
    m_maskLayers = false;
  }
  
  if(!m_maskLayers &&  (m_layersToMask.size() || m_disksToMask.size())) {
    ATH_MSG_DEBUG( "Layer/Disk to mask specified, but masking is disabled!" );
  } 

  if(!m_maskLayers &&  m_maskSide!=-1) {
    ATH_MSG_DEBUG( "Layer/Disk side to mask specified, but masking is disabled!" );
  } 
  
  if(m_maskLayers &&  m_disksToMask.size() && (std::find(m_disksToMask.begin(), m_disksToMask.end(),0)!=m_disksToMask.end())) {
    ATH_MSG_WARNING( "0th Disk not defined (-N to N) - check your setup!" );
  }   

  if(m_detStore->retrieve(m_pHelper,"SCT_ID").isFailure()) {
    ATH_MSG_ERROR("SCT helper failed to retrieve");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Successfully retrieved SCT_ID helper");
  }

  m_useDatabase=(std::find(m_badElements.value().begin(),m_badElements.value().end(),databaseSignature) != m_badElements.value().end());
  if(not m_useDatabase) {
    if(fillData().isFailure()) {
      ATH_MSG_ERROR("Failed to fill data");
      return StatusCode::FAILURE;
    }
  } else {
    if(m_detStore->regFcn(&SCT_ModuleVetoSvc::fillData,this,m_dbList,coolFolderName).isFailure()) {
      ATH_MSG_ERROR("Failed to register callback");
      return StatusCode::FAILURE;
    }
  }
  //
  const std::string databaseUseString{m_useDatabase?"":"not "};
  ATH_MSG_INFO("Initialized veto service with data, "
               <<(m_badElements.value().size() - int(m_useDatabase))
               <<" elements declared bad. Database will "<<databaseUseString<<"be used.");
 
  return sc;
}

//Finalize
StatusCode
SCT_ModuleVetoSvc::finalize() {
  StatusCode sc{StatusCode::SUCCESS};
  return sc;
}

// From s.binet
// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode 
SCT_ModuleVetoSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if(ISCT_ConditionsSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<ISCT_ConditionsSvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

bool 
SCT_ModuleVetoSvc::canReportAbout(InDetConditions::Hierarchy h) {
  return ((h==InDetConditions::DEFAULT) or (h==InDetConditions::SCT_SIDE) );
}

bool 
SCT_ModuleVetoSvc::isGood(const Identifier & elementId, InDetConditions::Hierarchy h) {
  if(not canReportAbout(h)) return true;
  bool result{m_badIds.find(elementId) == m_badIds.end()};
  return result;
}

bool 
SCT_ModuleVetoSvc::isGood(const IdentifierHash & hashId) {
  Identifier elementId{m_pHelper->wafer_id(hashId)};
  return isGood(elementId);
}

StatusCode 
SCT_ModuleVetoSvc::fillData() {
  StatusCode sc{StatusCode::SUCCESS};
  if((m_badElements.value().size() - int(m_useDatabase)) == 0 && !m_maskLayers) {
    ATH_MSG_INFO("No bad modules in job options.");
    return sc;
  } 
  bool success{true};
  std::vector<std::string>::const_iterator pId{m_badElements.value().begin()};
  std::vector<std::string>::const_iterator last{m_badElements.value().end()};
  for(;pId not_eq last;++pId) {
    unsigned long long idToWrite{static_cast<unsigned long long>(atoll(pId->c_str()))};
    if(*pId != databaseSignature) success &= m_badIds.insert(Identifier(idToWrite)).second;
  }

  if(m_maskLayers) {
    ATH_MSG_INFO("Masking "<<m_layersToMask.size()<<" SCT Layers");
    ATH_MSG_INFO("Masking "<<m_disksToMask.size()<<" SCT Disks");
    for(unsigned int i{0}; i < m_pHelper->wafer_hash_max(); i++){
      Identifier mID{m_pHelper->wafer_id(i)};
      if(
         (m_pHelper->barrel_ec(mID) == 0 && (m_maskSide==-1 || m_pHelper->side(mID)==m_maskSide) && (std::find(m_layersToMask.begin(), m_layersToMask.end(), m_pHelper->layer_disk(mID)) != m_layersToMask.end())) ||
         (m_pHelper->barrel_ec(mID) == 2  && (m_maskSide==-1 || m_pHelper->side(mID)==m_maskSide) && (std::find(m_disksToMask.begin(), m_disksToMask.end(), (m_pHelper->layer_disk(mID) + 1)) != m_disksToMask.end())) ||
         (m_pHelper->barrel_ec(mID) == -2 && (m_maskSide==-1 || m_pHelper->side(mID)==m_maskSide) && (std::find(m_disksToMask.begin(), m_disksToMask.end(), -1*(m_pHelper->layer_disk(mID) + 1)) != m_disksToMask.end()))
         ) {
        ATH_MSG_DEBUG("Masking ID Hash"<<i);
        m_badIds.insert(mID);
      }
    }
  }
  
  m_filled=true;
  ATH_MSG_DEBUG("Successfully filled bad SCT identifiers list");
  return success?sc:(StatusCode::FAILURE);
}

StatusCode 
SCT_ModuleVetoSvc::fillData(int& /*i*/ , std::list<std::string>& /*folderList*/) {
  StatusCode sc{fillData()};
  if(sc.isFailure()) return StatusCode::FAILURE;
  if(m_detStore->retrieve(m_dbList,coolFolderName).isFailure()) return StatusCode::FAILURE;
  std::string badModuleString{m_dbList[0]["ModuleList"].data<std::string>()};
  std::vector<int> v{string2Vector<int>(badModuleString)};
  int numberInDb{static_cast<int>(v.size())};
  ATH_MSG_INFO(numberInDb<<" elements were declared bad in the database.");
  for(std::vector<int>::const_iterator i(v.begin());i!=v.end();++i) {
    m_badIds.insert(Identifier(*i));
  }
  return sc;
}

bool 
SCT_ModuleVetoSvc::canFillDuringInitialize() {
  return (not m_useDatabase);// can only fill during intialize if we don't use the database
}

bool
SCT_ModuleVetoSvc::filled() const {
  //code
  return m_filled;
}
