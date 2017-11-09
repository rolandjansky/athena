// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef NSW_TDR_COMPAT_ENUM_H
#define NSW_TDR_COMPAT_ENUM_H

namespace nsw {
/**
   @defgroup tdr_compat_enum "enum values needed for compatibility with TDR code"

   In the TDR study we had several enum and constants (from
   sTGCenumeration.h and sTGCgeometry.h). We got rid of most of
   them, but we still need the ones below.

   Note that the enum values might have changed with the new AGDD geometry.

   \todo check enum values DG-2015-10-07 (in particular 0->1 shifts)

*/
/* @{ */
const int STGC_LAYER_1=1, STGC_LAYER_2=2, STGC_LAYER_3=3, STGC_LAYER_4=4;
const int SMALL_PIVOT=0, SMALL_CONFIRM=1, LARGE_PIVOT=2, LARGE_CONFIRM=3;
/* @} */

} // end namespace nsw

#endif

