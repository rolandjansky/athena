// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuctpiBitMasks.h 700318 2015-10-13 14:13:15Z wengler $
#ifndef TRIGT1MUCTPI_MUCTPISECTORBITMASKS_H
#define TRIGT1MUCTPI_MUCTPISECTORBITMASKS_H

namespace LVL1MUCTPI {

  enum BitMask {
    // masks equal for all input data words
    SectorOverflowMask       = 0x00000001,
    Pt1Mask                  = 0x00380000, 
    Pt2Mask                  = 0x01c00000, 
    Pt1and2Mask              = 0x01f80000, 
    ROI1OverflowMask         = 0x02000000, 
    ROI2OverflowMask         = 0x04000000, 
    BCIDMask                 = 0x38000000,
    Candidate1Sign           = 0x40000000,
    Candidate2Sign           = 0x80000000,
    // Barrel specific constants
    BarrelROI1Mask           = 0x0000003e,
    BarrelBarrelOverlap1Mask = 0x00000100, 
    BarrelEndcapOverlap1Mask = 0x00000200, 
    BarrelROI2Mask           = 0x00007c00, 
    BarrelBarrelOverlap2Mask = 0x00020000, 
    BarrelEndcapOverlap2Mask = 0x00040000,
    BarrelReservedMask       = 0xC00180C0,
    // Endcap specific masks
    EndcapROI1Mask           = 0x000001fe,
    EndcapBarrelOverlap1Mask = 0x00000200,
    EndcapROI2Mask           = 0x0003fc00,
    EndcapBarrelOverlap2Mask = 0x00040000,
    EndcapReservedMask       = 0x06000000,
    // Forward specific masks
    ForwardROI1Mask          = 0x0000007e,
    ForwardROI2Mask          = 0x0000fc00,
    ForwardReservedMask      = 0x06070380,
    // MIOCT ---------------------------------------------------------
    // Data flow marker on the MIBAK
    MioctStartMask           = 0x0000000e,
    MioctEndMask             = 0x0000000f,
    // MICTP ---------------------------------------------------------
    // The ID of the MICTP
    MictpIDMask              = 0x0000000f,
    MictpIDPositionMask      = 0xf0000000,
    // The position of the event ID in the MICTP start word
    MictpEvIDMask            = 0x00ffffff,
    // The position of the bunch ID, trigger type, monitr bit 
    // in the MICTP second word
    MictpECRCMask            = 0x000ff000,
    MictpBcIDMask            = 0x00000fff,
    MictpECRCBcIDMask        = 0x000fffff,
    MictpTrTypeMask          = 0x0ff00000,
    MictpMonitorMask         = 0x10000000,
    // position of values in the MICTP data word
    MictpSum1Mask            = 0x00000007,
    MictpSum2Mask            = 0x00000038,
    MictpSum3Mask            = 0x000001c0,
    MictpSum4Mask            = 0x00000e00,
    MictpSum5Mask            = 0x00007000,
    MictpSum6Mask            = 0x00038000,
    MictpTrailerSum1Mask     = 0x00001c00,
    MictpTrailerSum2Mask     = 0x0000e000,
    MictpTrailerSum3Mask     = 0x00070000,
    MictpTrailerSum4Mask     = 0x00380000,
    MictpTrailerSum5Mask     = 0x01c00000,
    MictpTrailerSum6Mask     = 0x0e000000,
    MictpTrailerFlagsMask    = 0x00000380,
    MictpTrailerWordsMask    = 0x0000007f,
    // The position of module number, eventID, BCID in MIOCT start word
    MioctMNBRMask            = 0xf0000000,
    MioctEvIDMask            = 0x0ffff000,
    MioctBcIDMask            = 0x00000fff,
    // The Event Reader -----------------------------------------------
    // position of values in EventFifo
    EvReEvIDMask             = 0x00ffffff,
    EvReBcIDMask             = 0x00000fff,
    EvReNoWordsMask          = 0x00ffe000,
    EvReMonitorMask          = 0x00001000,
    EvReTrTypeMask           = 0x00000ff0,
    EvReStatusMask           = 0x0000000f,
    // Sector Address coding in EventReader Format
    EvReSecAddressSystemMask = 0x000000c0,
    EvReSecAddressSubSysMask = 0x00000001,
    EvReSecNumberBarrelMask  = 0x0000003e,
    EvReSecNumberEndcapMask  = 0x0000007e,
    EvReSecNumberForwardMask = 0x0000003e,
    // RoI and Overlap coding in Barrel, Endcap and Forward
    EvReBarrelRoIMask        = 0x0000001f,
    EvReBarrelOvlMask        = 0x00000180,
    EvReEndcapRoIMask        = 0x000000ff,
    EvReEndcapOvlMask        = 0x00000100,
    EvReForwardRoIMask       = 0x0000003f,
    // The Extractor --------------------------------------------------
    // position of values in the Extractor header
    ExtrHeaderEvID           = 0x00ffffff,
    ExtrHeaderBcID           = 0x00000fff,
    ExtrHeaderECRC           = 0x000ff000,
    ExtrMonitorMask          = 0x00001000,
    ExtrTrTypeMask           = 0x00000ff0,
    ExtrStatusMask           = 0x0000000f,
    // position of values in MICTP word in Extractor format
    ExtrSum1Mask             = 0x00000007,
    ExtrSum2Mask             = 0x00000038,
    ExtrSum3Mask             = 0x000001c0,
    ExtrSum4Mask             = 0x00000e00,
    ExtrSum5Mask             = 0x00007000,
    ExtrSum6Mask             = 0x00038000,
    ExtrMictpBCIDMask        = 0x001c0000,
    // position of values in Extrotor Trailer word
    ExtrCandCountMask        = 0x000ffe00,
    ExtrThresh1Mask          = 0x000001c0,
    ExtrThresh2Mask          = 0x00000038,
    Extr1stCandSupprMask     = 0x00000004,
    Extr2ndCandSupprMask     = 0x00000002,
    ExtrEitherCandSupprMask  = 0x00000001,
    // position of all the single values in the Extractor data format
    ExtrPassedMask           = 0x04000000,
    ExtrFirstCandMask        = 0x02000000,
    ExtrSectorAddressMask    = 0x01fe0000,
    ExtrBCIDMask             = 0x0001c000,
    ExtrPtValueMask          = 0x00003800,
    ExtrRoIOvlMask           = 0x000007fc,
    ExtrPadOverflowMask      = 0x00000002,
    ExtrSectorOverflowMask   = 0x00000001,
    // position of RoI + Overlap candidate 1 and 2
    RoIOvl1Mask              = 0x000003fe,
    RoIOvl2Mask              = 0x0007fc00,
    // position of thresholds, reject bits in trailer of Extractor
    ExtrThresholdOneMask     = 0x000001c0,
    ExtrThresholdSecondMask  = 0x00000038,
    ExtrSuppressedFirstMask  = 0x00000004,
    ExtrSuppressedSecondMask = 0x00000002,
    ExtrSuppressedEitherMask = 0x00000001,
    // Charge:
    ExtrCandidateSign        = 0x08000000,
    // MirodDaqProcessor ---------------------------------------------
    DaqHeaderStartMask       = 0xee1234ee,
    DaqHeaderSizeMask        = 0x00000009,
    DaqHeaderFormatMask      = 0x03000000,
    DaqHeaderDetIDMask       = 0x00760000,
    DaqHeaderLvl1IDMask      = 0x00ffffff,
    DaqHeaderBcIDMask        = 0x00000fff,
    DaqHeaderTrTypeMask      = 0x000000ff,
    DaqTrailerCandMask       = 0x0000000f,
    // MirodLvl2Processor --------------------------------------------
    Lvl2HeaderStartMask      = 0xee1234ee,
    Lvl2HeaderSizeMask       = 0x00000009,
    Lvl2HeaderFormatMask     = 0x03000000,
    Lvl2HeaderDetIDMask      = 0x00760001,
    Lvl2FirstCandMask        = 0x00400000,
    Lvl2SectorAddressMask    = 0x003fc000,
    Lvl2PtValueMask          = 0x00003800,
    Lvl2RoIOvlMask           = 0x000007fc, 
    Lvl2PadOverflowMask      = 0x00000002,
    Lvl2SectorOverflowMask   = 0x00000001,
    Lvl2SecAddressStartBit   = 14,
    Lvl2BarrelRoIMask        = 0x0000001f,
    Lvl2BarrelOvlMask        = 0x00000180,
    Lvl2EndcapRoIMask        = 0x000000ff,
    Lvl2EndcapOvlMask        = 0x00000100,
    Lvl2ForwardRoIMask       = 0x0000003f,
    Lvl2CandidateSign        = 0x00800000

  }; // enum BitMask

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_MUCTPISECTORBITMASKS_H
