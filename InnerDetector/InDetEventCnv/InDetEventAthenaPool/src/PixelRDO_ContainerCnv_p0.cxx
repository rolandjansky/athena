/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelRDO_ContainerCnv_p0.h"

#include "InDetRawData/PixelRDO_Container.h"
#include "InDetRawData/PixelRDO_Collection.h"
#include "InDetIdentifier/PixelID.h"

#include "MsgUtil.h"

#include <string>
#include <iostream>
#include <sstream>
#include "CreateTransientTemplate.h"

//================================================================
PixelRDO_Container* PixelRDO_ContainerCnv_p0::createTransient(const PixelRDO_Container_p0* persObj, MsgStream& log) {
  std::unique_ptr<PixelRDO_Container> trans(new PixelRDO_Container(m_pixId->wafer_hash_max()));

  MSG_DEBUG(log,"Read RDO vector, size " << persObj->size());
  
  PixelRDO_Container_p0::const_iterator it   = persObj->begin();
  PixelRDO_Container_p0::const_iterator last = persObj->end();
  for (; it != last; ++it) {
    
    // Old persistent format used collection templated on the specific raw data type
    const InDetRawDataCollection<Pixel1RawData>* rdoCollOld = *it;
    
    if(!rdoCollOld) {
      throw std::runtime_error("Got NULL collection reading old format PixelRDO_Container");
    }

    // Ugly cast...  The new format does not need it in its converters.
    const InDetRawDataCollection<PixelRDORawData>* rdoColl = reinterpret_cast<const InDetRawDataCollection<PixelRDORawData> *>(rdoCollOld);
    
    // Add to the container
    if (trans->addCollection( rdoColl, rdoColl->identifyHash() ).isFailure()) {
      log << MSG::FATAL << "[p0] Pixel RDOs could not be added to the container!" << endmsg;
      throw std::runtime_error("PixelRDO_ContainerCnv_p0::createTransient(): Pixel RDOs could not be added to the container!");
    }
  }

  return(trans.release());
}

//================================================================


