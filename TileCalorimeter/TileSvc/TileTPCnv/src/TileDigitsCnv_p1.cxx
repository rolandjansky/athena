///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileDigitsCnv_p1.cxx 
// Implementation file for class TileDigitsCnv_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 

#include "TileEvent/TileDigits.h"
#include "TileTPCnv/TileDigitsCnv_p1.h"


void
TileDigitsCnv_p1::persToTrans(const TileDigits_p1* persObj, TileDigits* transObj, MsgStream &/*log*/)
{
  std::vector<float> digits;
  digits.reserve(persObj->m_digits.size());
  for (short d : persObj->m_digits) {
    digits.push_back( d / 16. );
  }

  *transObj = TileDigits (HWIdentifier(Identifier32(persObj->m_channelID)),
                          std::move(digits));
}


void
TileDigitsCnv_p1::transToPers(const TileDigits* transObj, TileDigits_p1* persObj, MsgStream &/*log*/)
{
  persObj->m_channelID = transObj->adc_HWID().get_identifier32().get_compact();

  persObj->m_digits.reserve(transObj->nsamples());
  for (float d : transObj->samples()) {
    short val = (short)(std::min(d,2047.F)*16.);
    persObj->m_digits.push_back( val );
  }
}
