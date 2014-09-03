/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////
//  Constants
//=====================================================================
//  Description: A file defining some constants needed by the 
//               SiTracker Digit byte stream converter package.
//---------------------------------------------------------------------
//  K. Karr, 03.02.2002
//---------------------------------------------------------------------
//  Changes: J. Schieck, 27.05.2002
//           -Added SCT information   
///////////////////////////////////////////////////////////////////////

#ifndef SITRACKER_DIGIT_BYTESTREAMCONVERTER_CONSTANTS_H
#define SITRACKER_DIGIT_BYTESTREAMCONVERTER_CONSTANTS_H

// SiTracker detector subsystem identifier numbers used by ATLAS 
// raw event format.
const int PIXEL_SDET_ID = 0x10;
const int SCT_SDET_BARREL_ID = 0x20;
const int SCT_SDET_ECL_ID = 0x21;
const int SCT_SDET_ECR_ID = 0x22;

// PixelMUR2TEMapper detector description constants.
const int PIXEL_BARREL_ID = 0;
const int PIXEL_BARREL_HALF_ID_MIN = 0;   
const int PIXEL_BARREL_HALF_ID_MAX = 1;
const int PIXEL_BARREL_NUM_PHI_PER_MUR = 1;
const int PIXEL_ENDCAP_ID = 2;
const int PIXEL_ENDCAP_NUM_PHI_PER_MUR = 6;

// Pixel wafer TEID overflow check.
const unsigned int PIXEL_BARREL_TEID_MAX = 0x1000;
const unsigned int PIXEL_ENDCAP_TEID_MAX = 0x4000;

// Pixel wafer REID overflow check.
const unsigned int PIXEL_BARREL_REID_MAX = 0x1000;
const unsigned int PIXEL_ENDCAP_REID_MAX = 0x1000;

// Pixel ROBID overflow check.
const int PIXEL_ROBID_MAX = 0x1000;

// Pixel wafer StoreGate key overflow check.
const int PIXEL_WAFER_KEY_OFFSET = 0;
const int PIXEL_WAFER_KEY_MAX = 3218 + PIXEL_WAFER_KEY_OFFSET;

// SCTMUR2TEMapper detector description constants.
const int SCT_BARREL_ID = 0;
const int SCT_BARREL_HALF_ID_MIN = 0;   
const int SCT_BARREL_HALF_ID_MAX = 1;
const int SCT_BARREL_NUM_PHI_PER_MUR = 1;
const int SCT_ENDCAP_ID = 2;
const int SCT_ENDCAP_MUR_OFFSET_OUTRING = 13;
const int SCT_ENDCAP_NUM_PHI_PER_MUR_OUTRING = 6; 
const int SCT_ENDCAP_NUM_PHI_PER_MUR_INRINGS = 5;

// SCT wafer TEID overflow check.
const unsigned int SCT_BARREL_TEID_MAX = 0x2000;
const unsigned int SCT_ENDCAP_TEID_MAX = 0x8000;

// SCT wafer REID overflow check.
const unsigned int SCT_BARREL_REID_MAX = 0x4000;
const unsigned int SCT_ENDCAP_REID_MAX = 0x400000;

// SCT ROBID overflow check.
const int SCT_ROBID_MAX = 0x1000;

// SCT wafer StoreGate key overflow check.
const int SCT_WAFER_KEY_OFFSET = 5000;
const int SCT_WAFER_KEY_MAX = 9328 + SCT_WAFER_KEY_OFFSET;

// Maximum REID value.
const unsigned int REID_MAX = 0x400000;

#endif  // SITRACKER_DIGIT_BYTESTREAMCONVERTER_CONSTANTS_H
