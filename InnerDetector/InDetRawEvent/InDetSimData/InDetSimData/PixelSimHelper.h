/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDetSimData helper for pixels
 This is based on the G3/RD-event. The digitization packages may permit or
 require more possibilities
 ------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: PixelSimHelper.h,v 1.2 2003-02-21 09:59:12 schaffer Exp $

#ifndef INDETRAWUTILS_PIXELSIMHELPER_H
# define INDETRAWUTILS_PIXELSIMHELPER_H

//<<<<<< INCLUDES                                                       >>>>>>
#include "InDetSimData/InDetSimData.h"
//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class PixelSimHelper
{
public:
    // methods to set characteristics of a new object
    static void noise(InDetSimData& sdo, bool flag);
    static void belowThreshold(InDetSimData& sdo, bool flag);
    static void disabled(InDetSimData& sdo, bool flag);
    static void badToT(InDetSimData& sdo, bool flag);

    // methods for clients
    static bool isNoise(const InDetSimData& sdo);
    static bool isBelowThreshold(const InDetSimData& sdo);
    static bool isDisabled(const InDetSimData& sdo);
    static bool hasBadTOT(const InDetSimData& sdo);
    static bool isRejected(const InDetSimData& sdo,
			   bool rejectOutOfBunch,
			   bool rejectUnconnected,
			   bool rejectBelowThreshold,
			   bool rejectDisabled,
			   bool rejectLostInReadout,
			   bool rejectNotTrack,
			   bool rejectExtraNoise,
			   bool rejectBadTOT);
private:
    enum 
    {
	NOISE_SET      = 0x1, NOISE_CLEAR    = 0xfffffffe,
	BT_SET         = 0x2, BT_CLEAR       = 0xfffffffd,
	DISABLED_SET   = 0x4, DISABLED_CLEAR = 0xfffffffb,
	BADTOT_SET     = 0x8, BADTOT_CLEAR   = 0xfffffff7
    };
};
    
inline void PixelSimHelper::noise(InDetSimData& sdo, bool flag)
{
    if (flag)
    {
	sdo.m_word |= NOISE_SET;
    }
    else 
    {
	sdo.m_word &= NOISE_CLEAR;
    };
}
    
inline void PixelSimHelper::belowThreshold(InDetSimData& sdo, bool flag)
{
    if (flag)
    {
	sdo.m_word |= BT_SET;
    }
    else 
    {
	sdo.m_word &= BT_CLEAR;
    };
}
    
inline void PixelSimHelper::disabled(InDetSimData& sdo, bool flag)
{
    if (flag)
    {
	sdo.m_word |=  DISABLED_SET;
    }
    else 
    {
	sdo.m_word &= DISABLED_CLEAR;
    };
}
    
inline void PixelSimHelper::badToT(InDetSimData& sdo, bool flag)
{
    
    if (flag) 
    {
	sdo.m_word |=  BADTOT_SET;
    }
    else 
    {
	sdo.m_word &= BADTOT_CLEAR;
    };
}

inline bool  PixelSimHelper::isNoise(const InDetSimData& sdo)
{
    return (sdo.m_word&NOISE_SET) != 0;
}

inline bool  PixelSimHelper::isBelowThreshold(const InDetSimData& sdo)
{
    return (sdo.m_word&BT_SET) != 0;
}

inline bool  PixelSimHelper::isDisabled(const InDetSimData& sdo)
{
    return (sdo.m_word&DISABLED_SET) != 0;
}

inline bool  PixelSimHelper::hasBadTOT(const InDetSimData& sdo)
{
    return (sdo.m_word&BADTOT_SET) != 0;
}

inline bool PixelSimHelper::isRejected(const InDetSimData& sdo,
				       bool /*rejectOutOfBunch*/,
				       bool /*rejectUnconnected*/,
				       bool rejectBelowThreshold,
				       bool rejectDisabled,
				       bool /*rejectLostInReadout*/,
				       bool /*rejectNotTrack*/,
				       bool rejectExtraNoise,
				       bool rejectBadTOT)
{
    // rejectOutOfBunch not implemented
    // rejectUnconnected not implemented
    if (rejectBelowThreshold && isBelowThreshold(sdo) && !isNoise(sdo)) 
	return true; // as in previous PixelDigitTDR_Cnv
    if (rejectDisabled && isDisabled(sdo)) return true;
    // rejectLostInReadout not implemented
    if (rejectBadTOT && hasBadTOT(sdo)) return true;
    // rejectNotTrack not implemented
    if (rejectExtraNoise && isNoise(sdo) && isBelowThreshold(sdo)) 
	return true;
    return false;
}

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // INDETRAWUTILS_PIXELSIMHELPER_H
