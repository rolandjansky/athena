///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileBeamElemCnv_p1.cxx 
// Implementation file for class TileBeamElemCnv_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 

#define private public
#define protected public
#include "TileEvent/TileBeamElem.h"
#undef private
#undef protected

#include "TileTPCnv/TileBeamElemCnv_p1.h"


void
TileBeamElemCnv_p1::persToTrans(const TileBeamElem_p1* persObj, TileBeamElem* transObj, MsgStream &/*log*/)
{
  transObj->m_adc_hwid = HWIdentifier(Identifier32(persObj->m_channelID));
   
  transObj->m_digits.reserve(persObj->m_digits.size());
  for (std::vector<uint16_t>::const_iterator it = persObj->m_digits.begin(); it != persObj->m_digits.end(); ++it) {
    transObj->m_digits.push_back( (uint32_t)(*it) );
  }
}


void
TileBeamElemCnv_p1::transToPers(const TileBeamElem* transObj, TileBeamElem_p1* persObj, MsgStream &/*log*/)
{
  persObj->m_channelID = transObj->m_adc_hwid.get_identifier32().get_compact();

  persObj->m_digits.reserve(transObj->m_digits.size());
  for (std::vector<uint32_t>::const_iterator it = transObj->m_digits.begin(); it != transObj->m_digits.end(); ++it) {
    persObj->m_digits.push_back( (uint16_t)(*it) );
  }
}
