///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileDigitsCnv_p1.cxx 
// Implementation file for class TileDigitsCnv_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 

#define private public
#define protected public
#include "TileEvent/TileDigits.h"
#undef private
#undef protected

#include "TileTPCnv/TileDigitsCnv_p1.h"


void
TileDigitsCnv_p1::persToTrans(const TileDigits_p1* persObj, TileDigits* transObj, MsgStream &/*log*/)
{
  transObj->m_adc_hwid = HWIdentifier(Identifier32(persObj->m_channelID));
   
  transObj->m_digits.reserve(persObj->m_digits.size());
  for (std::vector<short>::const_iterator it = persObj->m_digits.begin(); it != persObj->m_digits.end(); ++it) {
    float val = (*it)/16.;
    transObj->m_digits.push_back( val );
  }
}


void
TileDigitsCnv_p1::transToPers(const TileDigits* transObj, TileDigits_p1* persObj, MsgStream &/*log*/)
{
  persObj->m_channelID = transObj->m_adc_hwid.get_identifier32().get_compact();

  persObj->m_digits.reserve(transObj->m_digits.size());
  for (std::vector<float>::const_iterator it = transObj->m_digits.begin(); it != transObj->m_digits.end(); ++it) {
    short val = (short)(std::min((*it),float(2047.))*16.);
    persObj->m_digits.push_back( val );
  }
}
