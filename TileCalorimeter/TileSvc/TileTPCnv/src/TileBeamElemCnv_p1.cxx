///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileBeamElemCnv_p1.cxx 
// Implementation file for class TileBeamElemCnv_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 

#include "TileEvent/TileBeamElem.h"
#include "TileTPCnv/TileBeamElemCnv_p1.h"


void
TileBeamElemCnv_p1::persToTrans(const TileBeamElem_p1* persObj, TileBeamElem* transObj, MsgStream &/*log*/)
{
  std::vector<uint32_t> digits (persObj->m_digits.begin(),
                                persObj->m_digits.end());

  *transObj = TileBeamElem (HWIdentifier(Identifier32(persObj->m_channelID)),
                            std::move(digits));
}


void
TileBeamElemCnv_p1::transToPers(const TileBeamElem* transObj, TileBeamElem_p1* persObj, MsgStream &/*log*/)
{
  persObj->m_channelID = transObj->adc_HWID().get_identifier32().get_compact();
  persObj->m_digits.assign (transObj->get_digits().begin(),
                            transObj->get_digits().end());
}
