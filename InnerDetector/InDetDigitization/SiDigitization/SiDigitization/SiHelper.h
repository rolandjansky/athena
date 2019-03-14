/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*************************************************************************
    SiHelper for Silicons
    This is used by the SiDigitization
    ---------------------------------
    ATLAS Collaboration


    This is a short copy of the PixelSimHelper used by the SDO.
    It is needed to set the word of the Charged Diodes in the same 
    way as it will be registered in the SDO.
  
    D. Costanzo 4-3-03
***************************************************************************/


#ifndef SIDIGITIZATION_SIHELPER_H
#define SIDIGITIZATION_SIHELPER_H

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include "SiDigitization/SiChargedDiode.h"

class SiHelper
{
 public:
  // methods to set characteristics of a new object

  static void noise(SiChargedDiode& chDiode, bool flag, bool mask=false);
  static void belowThreshold(SiChargedDiode& chDiode, bool flag, bool mask=false);
  static void disabled(SiChargedDiode& chDiode, bool flag, bool mask=false);
  static void badToT(SiChargedDiode& chDiode, bool flag, bool mask=false);
  static void disconnected(SiChargedDiode& chDiode, bool flag, bool mask=false);
  static void maskOut(SiChargedDiode& chDiode, bool flag);
  static void ClusterUsed(SiChargedDiode& chDiode, bool flag);
  static void SetBunch(SiChargedDiode& chDiode, int bunch, MsgStream* log=nullptr);
  static void SetStripNum(SiChargedDiode& chDiode, int nstrip, MsgStream* log=nullptr);
  static void SetTimeBin(SiChargedDiode& chDiode, int time, MsgStream* log=nullptr);

  static bool isUsable(SiChargedDiode& chDiode);
  static bool isNoise(SiChargedDiode& chDiode);
  static bool isBelowThreshold(SiChargedDiode& chDiode);
  static bool isDisabled(SiChargedDiode& chDiode);
  static bool isBadToT(SiChargedDiode& chDiode);
  static bool isDisconnected(SiChargedDiode& chDiode);
  static bool isMaskOut(SiChargedDiode& chDiode);
  static bool isClusterUsed(SiChargedDiode &chDiode);
  static int  GetBunch(SiChargedDiode& chDiode);
  static int  GetStripNum(SiChargedDiode& chDiode);
  static int  GetTimeBin(SiChargedDiode& chDiode);

 private:
  enum {
    NOISE_SET        = 0x1,  // main charge is noise
    BT_SET           = 0x2,  // below threshold
    DISABLED_SET     = 0x4,  // disabled
    BADTOT_SET       = 0x8,  // bad TOT
    DISCONNECTED_SET = 0x10, // disconnected
    CLUSTERUSED_SET  = 0x20, // used in cluster - used by anyone???
    MASKOUT_SET      = 0x40  // charge diode is masked out, not to be used for RDO creation
  };
};

inline void SiHelper::maskOut(SiChargedDiode& chDiode, bool flag) {
  if (flag) {
    chDiode.m_word |=  MASKOUT_SET;
  } else {
    chDiode.m_word &= ~MASKOUT_SET;
  }
}
    
inline void SiHelper::noise(SiChargedDiode& chDiode, bool flag, bool mask) {
  if (flag) {
    chDiode.m_word |=  NOISE_SET;
  } else {
    chDiode.m_word &= ~NOISE_SET;
  }
  if (mask) SiHelper::maskOut(chDiode,true);
}
    
inline void SiHelper::belowThreshold(SiChargedDiode& chDiode, bool flag, bool mask) {
  if (flag) {
    chDiode.m_word |=  BT_SET;
  } else {
    chDiode.m_word &= ~BT_SET;
  }
  if (mask) SiHelper::maskOut(chDiode,true);
}
    
inline void SiHelper::disabled(SiChargedDiode& chDiode, bool flag, bool mask) {
  if (flag) {
    chDiode.m_word |=  DISABLED_SET;
  } else {
    chDiode.m_word &= ~DISABLED_SET;
  }
  if (mask) SiHelper::maskOut(chDiode,true);
}
    
inline void SiHelper::badToT(SiChargedDiode& chDiode, bool flag, bool mask) {
  if (flag) {
    chDiode.m_word |=  BADTOT_SET;
  } else {
    chDiode.m_word &= ~BADTOT_SET;
  }
  if (mask) SiHelper::maskOut(chDiode,true);
}

inline void SiHelper::disconnected(SiChargedDiode& chDiode, bool flag, bool mask) {
  if (flag) {
    chDiode.m_word |=  DISCONNECTED_SET;
  } else {
    chDiode.m_word &= ~DISCONNECTED_SET;
  }
  if (mask) SiHelper::maskOut(chDiode,true);
}


inline void SiHelper::ClusterUsed(SiChargedDiode& chDiode, bool flag) {
  if (flag) {
    chDiode.m_word |=  CLUSTERUSED_SET;
  } else {
    chDiode.m_word &= ~CLUSTERUSED_SET;
  }
}

inline void SiHelper::SetBunch(SiChargedDiode& chDiode, int bunch, MsgStream* log) {
  //
  // Code the bunch number in the 8 bits set corresponding to xx in xx00 
  //
  if (bunch > 0xff) {
    if (log) (*log) << MSG::ERROR << "Bunch Number not allowed" << endmsg;
  }
  chDiode.m_word = chDiode.m_word | ( (bunch&0xff) <<8 )  ;
}

inline void SiHelper::SetStripNum(SiChargedDiode& chDiode, int nstrip, MsgStream* log) {
  //
  // Code the number of strips in the 12 bits set corresponding to xxx in 0xxx0000 
  //
  if (nstrip > 0xfff) {
    if (log) (*log) << MSG::ERROR << "Number of strips not allowed" << endmsg;
  }
  chDiode.m_word = chDiode.m_word | ((nstrip&0xfff) << 16 ) ;
}

inline void SiHelper::SetTimeBin(SiChargedDiode& chDiode, int time, MsgStream* log) {
  //
  // Code the SCT Timebin number in the 3 bits set corresponding to x in x0000000
  //
  if (time > 0xf) {
    if (log) (*log) << MSG::ERROR << "TimeBin not allowed" << endmsg;
  }
  chDiode.m_word = chDiode.m_word | ( (time&0xf) <<28 )  ;
}

////////////////////////////////////////////////////////////

inline bool SiHelper::isUsable(SiChargedDiode& chDiode) {
  return !SiHelper::isMaskOut(chDiode);
  //  return (((chDiode.m_word & 0xff) == 0) ||     // no special status bits sets, either track, xtalk or random noise
  //          (SiHelper::isClusterUsed(chDiode)) ); // cluster used - whatever that means - not set in digitization
}

inline bool SiHelper::isNoise(SiChargedDiode& chDiode) {
  return (chDiode.m_word & NOISE_SET);
}

inline bool SiHelper::isMaskOut(SiChargedDiode& chDiode) {
  return (chDiode.m_word & MASKOUT_SET);
}

inline bool SiHelper::isBelowThreshold(SiChargedDiode& chDiode) {
  return (chDiode.m_word & BT_SET);
}
    
inline bool SiHelper::isDisabled(SiChargedDiode& chDiode) {
  return (chDiode.m_word & DISABLED_SET);
}
    
inline bool SiHelper::isBadToT(SiChargedDiode& chDiode) {
  return (chDiode.m_word & BADTOT_SET);
}

inline bool SiHelper::isDisconnected(SiChargedDiode& chDiode) {
  return (chDiode.m_word & DISCONNECTED_SET);
}

inline bool SiHelper::isClusterUsed(SiChargedDiode& chDiode) {
  return (chDiode.m_word & CLUSTERUSED_SET);
}

inline int SiHelper::GetBunch(SiChargedDiode& chDiode) {
return ( (chDiode.m_word >> 8) & 0xff );
}

inline int SiHelper::GetStripNum(SiChargedDiode& chDiode) {
  return ( (chDiode.m_word >> 16) & 0xfff );
}

inline int SiHelper::GetTimeBin(SiChargedDiode& chDiode) {
return ( (chDiode.m_word >> 28) & 0xf );
}    


#endif
