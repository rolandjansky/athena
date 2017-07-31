/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006 

#include "TRT_RDO_ContainerCnv_p0.h"

#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetIdentifier/TRT_ID.h"
#include "MsgUtil.h"

#include <string>
#include <iostream>
#include <sstream>
#include "CreateTransientTemplate.h"

//================================================================
TRT_RDO_Container* TRT_RDO_ContainerCnv_p0::createTransient(const TRT_RDO_Container_p0* persObj, MsgStream& log) {
  std::unique_ptr<TRT_RDO_Container> trans(new TRT_RDO_Container(m_trtId->straw_layer_hash_max()));

  MSG_DEBUG(log,"Read RDO vector, size " << persObj->size());
  
  TRT_RDO_Container_p0::const_iterator it   = persObj->begin();
  TRT_RDO_Container_p0::const_iterator last = persObj->end();
  for (; it != last; ++it) {
    
    // Old persistent format used collection templated on the specific raw data type
    const InDetRawDataCollection<TRT_LoLumRawData>* rdoCollOld = *it;
    
    if(!rdoCollOld) {
      throw std::runtime_error("Got NULL collection reading old format TRT_RDO_Container");
    }

    // Ugly cast...  The new format does not need it in its converters.
    const InDetRawDataCollection<TRT_RDORawData>* rdoColl = reinterpret_cast<const InDetRawDataCollection<TRT_RDORawData> *>(rdoCollOld);
    
    // Add to the container
    if (trans->addCollection( rdoColl, rdoColl->identifyHash() ).isFailure()) {
      log << MSG::FATAL << "[p0] TRT RDOs could not be added to the container!" << endmsg;
      throw std::runtime_error("TRT_RDO_ContainerCnv_p0::createTransient(): TRT RDOs could not be added to the container!");
    }
  }

  return(trans.release());
}

//================================================================

