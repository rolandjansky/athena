/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTRawContByteStreamTool.h"

#include "ByteStreamData/RawEvent.h" 
#include "ByteStreamCnvSvcBase/SrcIdMap.h" 
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "SCT_RawDataByteStreamCnv/ISCT_RodEncoder.h"
#include "SCT_Cabling/ISCT_CablingTool.h"

#include "StoreGate/ReadCondHandle.h"
#include "eformat/SourceIdentifier.h"

#include <cstdint>

/// ------------------------------------------------------------------------
/// contructor 

SCTRawContByteStreamTool::SCTRawContByteStreamTool
(const std::string& type, const std::string& name,const IInterface* parent):
  base_class(type, name, parent),
  m_sctIDHelper{nullptr},
  m_mutex{}
{
  declareProperty("RodBlockVersion", m_rodBlockVersion=0);
  return;
}

/// ------------------------------------------------------------------------
/// initialize the tool

StatusCode
SCTRawContByteStreamTool::initialize() {
  /** Retrieve id mapping  */
  ATH_CHECK(m_cabling.retrieve());
  ATH_MSG_INFO("Retrieved service " << m_cabling);

  /** Get the SCT Helper */
  ATH_CHECK(detStore()->retrieve(m_sctIDHelper, "SCT_ID"));

  return StatusCode::SUCCESS;
}

/// ------------------------------------------------------------------------
/// finalize the tool

StatusCode
SCTRawContByteStreamTool::finalize() {
  return StatusCode::SUCCESS;
}

/// ------------------------------------------------------------------------
/// convert() method which maps rodid's to vectors of RDOs in those RODs,
/// then loops through the map, using the RodEncoder to fill data for each
/// ROD in turn.

StatusCode
SCTRawContByteStreamTool::convert(SCT_RDO_Container* sctRDOCont, RawEventWrite* rawEvtWrite, MsgStream& log) const {
  std::lock_guard<std::mutex> lock(m_mutex);

  m_fullEventAssembler.clear();
  FullEventAssembler<SrcIdMap>::RODDATA* rod;
  
  /** set ROD Minor version */
  m_fullEventAssembler.setRodMinorVersion(m_rodBlockVersion);
  ATH_MSG_DEBUG(" Setting Minor Version Number to " << m_rodBlockVersion);
  
  /** mapping between ROD IDs and the hits in that ROD */
  std::map<uint32_t, std::vector<const SCT_RDORawData*> > rdoMap;

  /** The following few lines are to make sure there is an entry in the rdoMap for 
   *  every ROD, even if there are no hits in it for a particular event 
   *  (as there might be ByteStream errors e.g. TimeOut errors). */

  std::vector<std::uint32_t> listOfAllRODs;
  m_cabling->getAllRods(listOfAllRODs);
  for (std::uint32_t rod : listOfAllRODs) {
    rdoMap[rod].clear();
  }

  /**loop over the collections in the SCT RDO container */
  for (const InDetRawDataCollection<SCT_RDORawData>* sctRawColl : *sctRDOCont) {
    if (sctRawColl == nullptr) {
      ATH_MSG_WARNING("Null pointer to SCT RDO collection.");
      continue;
    } 
    else {
      /** Collection Id */
      Identifier idColl{sctRawColl->identify()};
      IdentifierHash idCollHash{m_sctIDHelper->wafer_hash(idColl)};
      uint32_t robid{m_cabling->getRobIdFromHash(idCollHash)};
      
      if (robid == 0) continue;
      
      /** Building the rod ID */
      eformat::helper::SourceIdentifier srcIDROB{robid};
      eformat::helper::SourceIdentifier srcIDROD{srcIDROB.subdetector_id(), srcIDROB.module_id()};
      uint32_t rodid{srcIDROD.code()};
      
      /** loop over RDOs in the collection;  */
      for (const SCT_RDORawData* rdo : *sctRawColl) {
        /** fill ROD/ RDO map */
        rdoMap[rodid].push_back(rdo);
      }
    }
  }  /** End loop over collections */

  /** now encode data for each ROD in turn */
  for (std::pair<uint32_t, std::vector<const SCT_RDORawData*>> rodToRDOs : rdoMap) {
    rod = m_fullEventAssembler.getRodData(rodToRDOs.first); /** get ROD data address */
    m_encoder->fillROD(*rod, rodToRDOs.first, rodToRDOs.second); /** encode ROD data */
  }
  m_fullEventAssembler.fill(rawEvtWrite, log);
  
  return StatusCode::SUCCESS;
}
