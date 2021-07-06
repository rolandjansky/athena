/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _MUON_NSW_DECODE_BITMAPS_H_
#define _MUON_NSW_DECODE_BITMAPS_H_

namespace Muon
{
  namespace nsw
  {
    enum EMaskRawData
    {
      // Felix Header (2 words)
      eMaskFlxLENGTH      = 0x0000ffff,
      eMaskFlxSTATUS      = 0xffff0000,
      eMaskFlxELINKID     = 0xffffffff,

      // sROC Header (1 word)   
      eMaskSRocNullL1ID   = 0x00ff0000,
      eMaskSRocNullID     = 0x3f000000,
      eMaskSRocL1ID       = 0x0000ffff,
      eMaskSRocBCID       = 0x0fff0000,
      eMaskSRocORBIT      = 0x30000000,
      eMaskSRocNULL       = 0x40000000,
      eMaskSRocNOTDC      = 0x80000000,

      // VMM Hit Data
      eMaskVmmHitTDC      = 0x000000ff,
      eMaskVmmHitADC      = 0x0003ff00,
      eMaskVmmHitCHANNEL  = 0x00fc0000,
      eMaskVmmHitVMMID    = 0x07000000,
      eMaskVmmHitRELBCID  = 0x38000000,
      eMaskVmmHitN        = 0x40000000, // neighbor
      eMaskVmmHitP        = 0x80000000, // parity bit

      // ROC Trailer (1 word)
      eMaskTrailCHECKSUM  = 0x000000ff,
      eMaskTrailLENGTH    = 0x0003ff00,
      eMaskTrailL0ID      = 0x003c0000,
      eMaskTrailFLAGMISS  = 0x3fc00000,
      eMaskTrailTO        = 0x40000000, // true if VMMs ignored due to timeouts
      eMaskTrailEXTENDED  = 0x80000000  // "extended-trailer" bit, not used 
    };

    enum BitMaskRawData
    {
      // Felix Header (2 words)
      bitMaskFlxLENGTH      = 0xffff,
      bitMaskFlxSTATUS      = 0xffff,
      bitMaskFlxELINKID     = 0xffffffff,

      // sROC Header (1 word)   
      bitMaskSRocNullL1ID   = 0xff,
      bitMaskSRocNullID     = 0x3f,
      bitMaskSRocL1ID       = 0xffff,
      bitMaskSRocBCID       = 0xfff,
      bitMaskSRocORBIT      = 0x3,
      bitMaskSRocNULL       = 0x1,
      bitMaskSRocNOTDC      = 0x1,

      // VMM Hit Data
      bitMaskVmmHitTDC      = 0xff,
      bitMaskVmmHitADC      = 0x3ff,
      bitMaskVmmHitCHANNEL  = 0x3f,
      bitMaskVmmHitVMMID    = 0x7,
      bitMaskVmmHitRELBCID  = 0x7,
      bitMaskVmmHitN        = 0x1, // neighbor
      bitMaskVmmHitP        = 0x1, // parity bit

      // ROC Trailer (1 word)
      bitMaskTrailCHECKSUM  = 0xff,
      bitMaskTrailLENGTH    = 0x3ff,
      bitMaskTrailL0ID      = 0xf,
      bitMaskTrailFLAGMISS  = 0xff,
      bitMaskTrailTO        = 0x1, // true if VMMs ignored due to timeouts
      bitMaskTrailEXTENDED  = 0x1  // "extended-trailer" bit, not used 
    };

    enum BitPosRawData
    {
      // Felix Header (2 words)
      bitPosFlxLENGTH      = 0,
      bitPosFlxSTATUS      = 16,
      bitPosFlxELINKID     = 0,

      // sROC Header (1 word)   
      bitPosSRocNullL1ID   = 16,
      bitPosSRocNullID     = 24,
      bitPosSRocL1ID       = 0,
      bitPosSRocBCID       = 16,
      bitPosSRocORBIT      = 28,
      bitPosSRocNULL       = 30,
      bitPosSRocNOTDC      = 31,

      // VMM Hit Data
      bitPosVmmHitTDC      = 0,
      bitPosVmmHitADC      = 8,
      bitPosVmmHitCHANNEL  = 18,
      bitPosVmmHitVMMID    = 24,
      bitPosVmmHitRELBCID  = 27,
      bitPosVmmHitN        = 30, // neighbor
      bitPosVmmHitP        = 31, // parity bit

      // ROC Trailer (1 word)
      bitPosTrailCHECKSUM  = 0,
      bitPosTrailLENGTH    = 8,
      bitPosTrailL0ID      = 18,
      bitPosTrailFLAGMISS  = 22,
      bitPosTrailTO        = 30, // true if VMMs ignored due to timeouts
      bitPosTrailEXTENDED  = 31  // "extended-trailer" bit, not used 
    };

    enum EMaskDetectorResource
    {
      eMaskElink     = 0x00000007,
      eMaskRadius    = 0x00000078,
      eMaskLayer     = 0x00000380,
      eMaskSector    = 0x00003c00,
      eMaskResType   = 0x0001c000,
      eMaskDataType  = 0x000e0000,
      eMaskRESERVED  = 0x00100000,
      eMaskVersion   = 0x00e00000,
      eMaskDetId     = 0xff000000,

      // For pre-versioned format used during commissioning

      eMaskObsEta      = 0x00004000,
      eMaskObsResType  = 0x00078000,
      eMaskObsTech     = 0x00080000,
      eMaskObsDataType = 0x00700000
    };

    enum BitMaskDetectorResource
    {
      bitMaskElink     = 0x7,
      bitMaskRadius    = 0xf,
      bitMaskLayer     = 0x7,
      bitMaskSector    = 0xf,
      bitMaskResType   = 0x7,
      bitMaskDataType  = 0x7,
      bitMaskRESERVED  = 0x1,
      bitMaskVersion   = 0x7,
      bitMaskDetId     = 0xff,

      // For pre-versioned format used during commissioning

      bitMaskObsEta      = 0x1,
      bitMaskObsResType  = 0xf,
      bitMaskObsTech     = 0x1,
      bitMaskObsDataType = 0x7
    };

    enum BitPosDetectorResource
    {
      bitPosElink     = 0,
      bitPosRadius    = 3,
      bitPosLayer     = 7,
      bitPosSector    = 10,
      bitPosResType   = 14,
      bitPosDataType  = 17,
      bitPosRESERVED  = 20,
      bitPosVersion   = 21,
      bitPosDetId     = 24,

      // For pre-versioned format used during commissioning

      bitPosObsEta      = 14,
      bitPosObsResType  = 15,
      bitPosObsTech     = 19,
      bitPosObsDataType = 20
    };
  }
}

#endif // _MUON_NSW_DECODE_BITMAPS_H_


