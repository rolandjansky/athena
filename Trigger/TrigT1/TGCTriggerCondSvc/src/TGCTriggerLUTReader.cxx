/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCTriggerCondSvc/TGCTriggerLUTReader.h"

#include "TGCTriggerCondSvc/TGCTriggerLUTs.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaKernel/errorcheck.h"

TGCTriggerLUTReader::TGCTriggerLUTReader(int lutType):
  m_lutType(lutType),
  m_msgSvc(nullptr)
{
  m_svcLocator = Gaudi::svcLocator();
}



bool TGCTriggerLUTReader::readLUT(TGCTriggerLUTs* ){

  MsgStream log(m_msgSvc, "TGCTriggerLUTReader::readLUT");
  if(m_svcLocator->service("MessageSvc", m_msgSvc) == StatusCode::FAILURE){
    return false;
  }

  log<<MSG::ERROR<<" Please implement readLUT"<<endmsg;

  return true;
}

bool TGCTriggerLUTReader::loadParameters(TGCTriggerLUTs* ,
                                         const CondAttrListCollection* 
                                         ){

  MsgStream log(m_msgSvc, "TGCTriggerLUTReader::loadParameters");
  if(m_svcLocator->service("MessageSvc", m_msgSvc) == StatusCode::FAILURE){
    return false;
  }
  log<<MSG::ERROR<<" Please implement loadParameters"<<endmsg;

  return true;

}
