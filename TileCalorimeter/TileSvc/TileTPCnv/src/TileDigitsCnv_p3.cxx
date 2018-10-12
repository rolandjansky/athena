///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileDigitsCnv_p3.cxx 
// Implementation file for class TileDigitsCnv_p3
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 

#include "TileEvent/TileDigits.h"
#include "TileTPCnv/TileDigitsCnv_p3.h"


void
TileDigitsCnv_p3::persToTrans(const TileDigits_p3* persObj, TileDigits* transObj, MsgStream &/*log*/)
{
  *transObj = TileDigits (HWIdentifier(Identifier32(persObj->m_channelID)),
                          persObj->m_digits);
}


void
TileDigitsCnv_p3::transToPers(const TileDigits* transObj, TileDigits_p3* persObj, MsgStream &/*log*/)
{
  persObj->m_channelID = transObj->adc_HWID().get_identifier32().get_compact();
  persObj->m_digits = transObj->samples();
}
