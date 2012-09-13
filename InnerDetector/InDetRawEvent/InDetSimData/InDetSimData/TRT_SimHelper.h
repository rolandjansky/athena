/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDetSimData helper for TRT
 This is based on the G3/RD-event. The digitization packages may permit or
 require more possibilities
 ------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: TRT_SimHelper.h,v 1.2 2003-02-21 13:39:01 candlin Exp $

#ifndef INDETRAWUTILS_TRT_SIMHELPER_H
# define INDETRAWUTILS_TRT_SIMHELPER_H

//<<<<<< INCLUDES                                                       >>>>>>
#include "InDetSimData/InDetSimData.h"
//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class TRT_SimHelper
{
public:
    // methods to set characteristics of a new object
    static void aboveTR_Thresh(InDetSimData& sdo, bool flag);
    static void deadChannel(InDetSimData& sdo, bool flag);
    static void rodData(InDetSimData& sdo, bool flag);
    // check to know if time is ok for rod data
    static void validStraw(InDetSimData& sdo, bool flag);
    // An invalid straw may have a valid hit - time is zero, but 
    // is_above_TR_thresh may be set
    static void hasHit(InDetSimData& sdo, bool flag);

    // methods for clients
    static bool isAboveTR_Thresh(const InDetSimData& sdo);
    static bool isDeadChannel(const InDetSimData& sdo);
    static bool isRodData(const InDetSimData& sdo);
    static bool isValidStraw(const InDetSimData& sdo);
    static bool HasHit(const InDetSimData& sdo);
    // Probably useful at some point:
    //    static bool isRejected(const InDetSimData& sdo,
    //			   bool rejectOutOfBunch,
    //			   bool rejectUnconnected,
    //			   bool rejectBelowThreshold,
    //			   bool rejectDisabled,
    //			   bool rejectLostInReadout,
    //			   bool rejectNotTrack,
    //			   bool rejectExtraNoise,
    //			   bool rejectBadTOT);
private:
    enum 
    {
        ABTR_SET  = 0x1,  ABTR_CLEAR  = 0xfffffffe,
	DEAD_SET  = 0x2,  DEAD_CLEAR  = 0xfffffffd,
	ROD_SET   = 0x4,  ROD_CLEAR   = 0xfffffffb,
	VALID_SET = 0x8,  VALID_CLEAR = 0xfffffff7,
        HIT_SET   = 0x10, HIT_CLEAR   = 0xffffffef
    };
};



    
inline void TRT_SimHelper::aboveTR_Thresh(InDetSimData& sdo, bool flag)
{
    if (flag)
    {
	sdo.m_word |= ABTR_SET;
    }
    else 
    {
	sdo.m_word &= ABTR_CLEAR;
    };
}
    
inline void TRT_SimHelper::deadChannel(InDetSimData& sdo, bool flag)
{
    if (flag)
    {
	sdo.m_word |= DEAD_SET;
    }
    else 
    {
	sdo.m_word &= DEAD_CLEAR;
    };
}
    
inline void TRT_SimHelper::rodData(InDetSimData& sdo, bool flag)
{
    if (flag)
    {
	sdo.m_word |=  ROD_SET;
    }
    else 
    {
	sdo.m_word &= ROD_CLEAR;
    };
}
    
inline void TRT_SimHelper::validStraw(InDetSimData& sdo, bool flag)
{
    
    if (flag) 
    {
	sdo.m_word |=  VALID_SET;
    }
    else 
    {
	sdo.m_word &= VALID_CLEAR;
    };
}
inline void TRT_SimHelper::hasHit(InDetSimData& sdo, bool flag)
{
    
    if (flag) 
    {
	sdo.m_word |=  HIT_SET;
    }
    else 
    {
	sdo.m_word &= HIT_CLEAR;
    };
}



inline bool  TRT_SimHelper::isAboveTR_Thresh(const InDetSimData& sdo)
{
    return (sdo.m_word&ABTR_SET) != 0;
}

inline bool  TRT_SimHelper::isDeadChannel(const InDetSimData& sdo)
{
    return (sdo.m_word&DEAD_SET) != 0;
}

inline bool  TRT_SimHelper::isRodData(const InDetSimData& sdo)
{
    return (sdo.m_word&ROD_SET) != 0;
}

inline bool  TRT_SimHelper::isValidStraw(const InDetSimData& sdo)
{
    return (sdo.m_word&VALID_SET) != 0;
}
inline bool  TRT_SimHelper::HasHit(const InDetSimData& sdo)
{
    return (sdo.m_word&HIT_SET) != 0;
}
//inline bool TRT_SimHelper::isRejected(const InDetSimData& sdo,
//				       bool rejectOutOfBunch,
//				       bool rejectUnconnected,
//				       bool rejectBelowThreshold,
//				       bool rejectDisabled,
//				       bool rejectLostInReadout,
//				       bool rejectNotTrack,
//				       bool rejectExtraNoise,
//				       bool rejectBadTOT)
//{
    // rejectOutOfBunch not implemented
    // rejectUnconnected not implemented
//    if (rejectBelowThreshold && isBelowThreshold(sdo) && !isNoise(sdo)) 
//	return true; // as in previous PixelDigitTDR_Cnv
//    if (rejectDisabled && isDisabled(sdo)) return true;
    // rejectLostInReadout not implemented
//    if (rejectBadTOT && hasBadTOT(sdo)) return true;
    // rejectNotTrack not implemented
//    if (rejectExtraNoise && isNoise(sdo) && isBelowThreshold(sdo)) 
//	return true;
//    return false;
//}

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // INDETRAWUTILS_TRT_SIMHELPER_H

