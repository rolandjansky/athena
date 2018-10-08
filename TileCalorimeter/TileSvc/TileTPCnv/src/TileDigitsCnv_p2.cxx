///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileDigitsCnv_p2.cxx 
// Implementation file for class TileDigitsCnv_p2
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 

#include "TileEvent/TileDigits.h"
#include "TileTPCnv/TileDigitsCnv_p2.h"


void
TileDigitsCnv_p2::persToTrans(const TileDigits_p2* persObj, TileDigits* transObj, MsgStream &/*log*/)
{
  *transObj = TileDigits (HWIdentifier(Identifier32(persObj->m_channelID)),
                          persObj->m_digits);
}


void
TileDigitsCnv_p2::transToPers(const TileDigits* transObj, TileDigits_p2* persObj, MsgStream &/*log*/)
{
  persObj->m_channelID = transObj->adc_HWID().get_identifier32().get_compact();

  persObj->m_digits.reserve(transObj->nsamples());
  for (float d : transObj->samples()) {
    short val = (short)(std::min(d,32767.F));
    persObj->m_digits.push_back( val );
  }
}
