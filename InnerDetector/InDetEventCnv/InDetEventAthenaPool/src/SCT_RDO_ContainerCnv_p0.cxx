/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_RDO_ContainerCnv_p0.h"

#include "CreateTransientTemplate.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/SCT_RDO_Collection.h"

//================================================================
SCT_RDO_Container* SCT_RDO_ContainerCnv_p0::createTransient(const SCT_RDO_Container_p0* persObj, MsgStream& log) {
  std::unique_ptr<SCT_RDO_Container> trans(std::make_unique<SCT_RDO_Container>(m_sctId->wafer_hash_max()));

  SCT_RDO_Container_p0::const_iterator it   = persObj->begin();
  SCT_RDO_Container_p0::const_iterator last = persObj->end();
  for (; it != last; ++it) {
    
    // Old persistent format used collection templated on the specific raw data type
    const InDetRawDataCollection<SCT1_RawData>* rdoCollOld = *it;
    
    if (rdoCollOld==nullptr) {
      throw std::runtime_error("Got NULL collection reading old format SCT_RDO_Container");
    }

    // Ugly cast...  The new format does not need it in its converters.
    const InDetRawDataCollection<SCT_RDORawData>* rdoColl = reinterpret_cast<const InDetRawDataCollection<SCT_RDORawData> *>(rdoCollOld);
    
    // Add to the container
    if (trans->addCollection( rdoColl, rdoColl->identifyHash() ).isFailure()) {
      log << MSG::FATAL << "[p0] SCT RDOs could not be added to the container!" << endmsg;
      throw std::runtime_error("SCT_RDO_ContainerCnv_p0::createTransient(): SCT RDOs could not be added to the container!");
    }
  }

  return trans.release();
}

//================================================================
