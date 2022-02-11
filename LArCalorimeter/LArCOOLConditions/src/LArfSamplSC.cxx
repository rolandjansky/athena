/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCOOLConditions/LArfSamplSC.h"


//const float LArfSamplSC::errorcode=ILArfSampl::ERRORCODE;

LArfSamplSC::LArfSamplSC()
  : LArCondSuperCellBase ("LArfSamplSC"),
    m_null(0.)
{}

LArfSamplSC::~LArfSamplSC() {}


LArfSamplSC::LArfSamplSC(const CondAttrListCollection* attrList)
  : LArCondSuperCellBase ("LArfSamplSC"),
    m_null(0.)
{
  if (initializeBase().isFailure()) return;
 
  readBlob(attrList,"fSampl",msg());

  if (m_pValues.size()!=1) {
    ATH_MSG_ERROR( "Found unexpected number of gains (" << m_pValues.size() <<"). Expected exactly one gain." );
  }

  return;
}


const float& LArfSamplSC::FSAMPL(const HWIdentifier& hwid) const {
  const IdentifierHash hash=m_scOnlineID->channel_Hash(hwid);
  return this->getDataByHash(hash,0);
}

// retrieving LArfSampl using offline ID  
const float& LArfSamplSC::FSAMPL(const Identifier& /*id*/) const  {
  ATH_MSG_WARNING( "LArfSamplSC::FSAMPL not implemented for CellId !!!" );
  return m_null; 
}
