/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"

namespace LVL1{

  unsigned int ITrigT1MuonRecRoiTool::getBitMaskValue( const unsigned int * uintValue,
						       const unsigned int mask ) const {
    unsigned int maskcopy = mask;
    unsigned int result = *uintValue & mask;
    if ( mask != 0 ) {
      while ( ( maskcopy & 0x00000001 ) == 0 ) {
	maskcopy = maskcopy >> 1;
	result = result >> 1;
      }
    }
    return result;
  }

  ITrigT1MuonRecRoiTool::MuonTriggerSystem ITrigT1MuonRecRoiTool::getSystem( const unsigned int& roiWord ) const
  {
    unsigned int result = getBitMaskValue(&roiWord,SysIDMask());
    if( result == 0x0 ) return Barrel;
    else if( result == 0x1 ) return Forward;
    else if( result>>1 ) return Endcap;
    else return Undef;
  }
  
  ITrigT1MuonRecRoiTool::ITrigT1MuonRecRoiTool()
  {
    updateBitMask( Run2 );
  }
  void ITrigT1MuonRecRoiTool::updateBitMask( const ITrigT1MuonRecRoiTool::RoiWordFormat format ){
    if( format == Run2 ){
      m_IsRun3Mask            = 0x80000000;
      m_IsVetoedMask          = 0x10000000;
      m_ChargeMask            = 0x08000000;
      m_IsFirstCandMask       = 0x00400000;
      m_SectorAddressMask     = 0x003fc000;
      m_BarrelSectorIDMask    = 0x000f8000;
      m_EndcapSectorIDMask    = 0x001f8000;
      m_ForwardSectorIDMask   = 0x000f8000;
      m_SysIDMask             = 0x00300000;
      m_SubSysIDMask          = 0x00004000;
      m_ThresholdMask         = 0x00003800;
      m_BarrelRoIMask         = 0x0000007c;
      m_EndcapRoIMask         = 0x000003fc;
      m_ForwardRoIMask        = 0x000000fc;
      m_EndcapRMask           = 0x000003f0;
      m_EndcapPhiMask         = 0x0000000c;
      m_ForwardRMask          = 0x000000f0;
      m_ForwardPhiMask        = 0x0000000c;
      m_OverflowPerRoIMask    = 0x00000002;
      m_OverflowPerSectorMask = 0x00000001;
      m_BW2Or3Mask            = 0x00000000; // undef
      m_InnerCoinMask         = 0x00000000; // undef
      m_GoodMFMask            = 0x00000000; // undef
    }else if( format == Run3 ){
      m_IsRun3Mask            = 0x80000000;
      m_IsVetoedMask          = 0x00010000;
      m_ChargeMask            = 0x00001000;
      m_IsFirstCandMask       = 0x00000000; // undef
      m_SectorAddressMask     = 0x1fe00000;
      m_BarrelSectorIDMask    = 0x07c00000;
      m_EndcapSectorIDMask    = 0x0fc00000;
      m_ForwardSectorIDMask   = 0x07c00000;
      m_SysIDMask             = 0x18000000;
      m_SubSysIDMask          = 0x00200000;
      m_ThresholdMask         = 0x00000f00;
      m_BarrelRoIMask         = 0x0000001f;
      m_EndcapRoIMask         = 0x000000ff;
      m_ForwardRoIMask        = 0x0000003f;
      m_EndcapRMask           = 0x000000fc;
      m_EndcapPhiMask         = 0x00000003;
      m_ForwardRMask          = 0x0000003c;
      m_ForwardPhiMask        = 0x00000003;
      m_OverflowPerRoIMask    = 0x00001000;
      m_OverflowPerSectorMask = 0x00020000;
      m_BW2Or3Mask            = 0x00002000;
      m_InnerCoinMask         = 0x00004000;
      m_GoodMFMask            = 0x00008000;
    }else{
      // no update
    }
  }
  
}
