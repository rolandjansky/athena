/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/** header file */
#include "SCTRawContByteStreamTool.h"

#include "ByteStreamData/RawEvent.h" 
#include "ByteStreamCnvSvcBase/SrcIdMap.h" 
#include "eformat/SourceIdentifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "SCT_RawDataByteStreamCnv/ISCT_RodEncoder.h"
#include "StoreGate/ReadCondHandle.h"

/// ------------------------------------------------------------------------
/// contructor 

SCTRawContByteStreamTool::SCTRawContByteStreamTool
(const std::string& type, const std::string& name,const IInterface* parent):
  base_class(type, name, parent),
  m_cabling{"SCT_CablingSvc", name},
  m_sct_idHelper{nullptr}
{
  declareProperty("RodBlockVersion", m_RodBlockVersion=0);
  declareProperty("CablingSvc", m_cabling);
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
  ATH_CHECK(detStore()->retrieve(m_sct_idHelper, "SCT_ID"));

  // Initialize ReadCondHandleKey
  ATH_CHECK(m_SCTDetEleCollKey.initialize());
  
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
SCTRawContByteStreamTool::convert(SCT_RDO_Container* cont, RawEventWrite* re, MsgStream& log) {
  // Get SCT_DetectorElementCollection
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey);
  const InDetDD::SiDetectorElementCollection* elements(sctDetEle.retrieve());
  if (elements==nullptr) {
    ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " could not be retrieved");
    return StatusCode::FAILURE;
  }

  m_fea.clear();   
  FullEventAssembler<SrcIdMap>::RODDATA* theROD; 
  
  /** set ROD Minor version */
  m_fea.setRodMinorVersion(m_RodBlockVersion);
  ATH_MSG_DEBUG(" Setting Minor Version Number to " << m_RodBlockVersion);
  
  /** mapping between ROD IDs and the hits in that ROD */
  std::map<uint32_t, std::vector<const RDO*> > rdoMap;

  /** The following few lines are to make sure there is an entry in the rdoMap for 
   *  every ROD, even if there are no hits in it for a particular event 
   *  (as there might be ByteStream errors e.g. TimeOut errors). */

  std::vector<boost::uint32_t> listOfAllRODs;
  m_cabling->getAllRods(listOfAllRODs);
  std::vector<boost::uint32_t>::iterator rodIter{listOfAllRODs.begin()};
  std::vector<boost::uint32_t>::iterator rodEnd{listOfAllRODs.end()};
  for (; rodIter != rodEnd; ++rodIter) {
    rdoMap[*rodIter].clear();
  }

  /**loop over the collections in the SCT RDO container */
  SCTRawContainer::const_iterator it_coll{cont->begin()}; 
  SCTRawContainer::const_iterator it_coll_end{cont->end()};
  
  for(; it_coll!=it_coll_end;++it_coll) {
    const SCTRawCollection* coll{*it_coll};
    if (coll == 0) {
      ATH_MSG_WARNING("Null pointer to SCT RDO collection.");
      continue;
    } else {
      /** Collection Id */
      Identifier idColl{coll->identify()};
      IdentifierHash idCollHash{m_sct_idHelper->wafer_hash(idColl)};
      uint32_t robid{m_cabling->getRobIdFromHash(idCollHash)};
      if (robid == 0) continue;
      
      /** Building the rod ID */
      eformat::helper::SourceIdentifier sid_rob{robid};
      eformat::helper::SourceIdentifier sid_rod{sid_rob.subdetector_id(), sid_rob.module_id()};
      uint32_t rodid{sid_rod.code()};
      /** see if strip numbers go from 0 to 767 or vice versa for this wafer */
      const InDetDD::SiDetectorElement* siDetElement{elements->getDetectorElement(idCollHash)};
      bool swapPhiReadoutDirection{siDetElement->swapPhiReadoutDirection()};
      
      /** loop over RDOs in the collection;  */
      SCTRawCollection::const_iterator it_b{coll->begin()};
      SCTRawCollection::const_iterator it_e{coll->end()}; 
      for(; it_b!=it_e; ++it_b) {
        const RDO* theRdo{*it_b};          
        /** fill ROD/ RDO map */
        rdoMap[rodid].push_back(theRdo);
        if (swapPhiReadoutDirection) m_encoder->addSwapModuleId(idColl);
      }
    }
  }  /** End loop over collections */

  /** now encode data for each ROD in turn */
  
  std::map<uint32_t,std::vector<const RDO*> >::iterator it_map{rdoMap.begin()};
  std::map<uint32_t,std::vector<const RDO*> >::iterator it_map_end{rdoMap.end()};
  
  for (; it_map != it_map_end; ++it_map) { 
    theROD = m_fea.getRodData(it_map->first); /** get ROD data address */
    m_encoder->fillROD(*theROD,it_map->first, it_map->second);  /** encode ROD data */
  }
  m_fea.fill(re, log); 
  
  return StatusCode::SUCCESS; 
}
