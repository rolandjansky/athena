/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRIGT1MUONRECROITOOL_H
#define ITRIGT1MUONRECROITOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigT1MuonRecRoiData.h"

namespace LVL1 {

  class ITrigT1MuonRecRoiTool: public virtual IAlgTool {
  public:
    
    ITrigT1MuonRecRoiTool();

    virtual ~ITrigT1MuonRecRoiTool()=default;

    DeclareInterfaceID( ITrigT1MuonRecRoiTool, 1, 0);

    /// pure virtual function to return eta and phi coord of RoI
    virtual TrigT1MuonRecRoiData roiData(const unsigned int& roiWord ) const = 0;

    /// pure virtual function returns roi size
    virtual void RoIsize(const unsigned int& roiWord,
			 double& etaMin, double& etaMax, 
			 double& phiMin, double& phiMax) const = 0;

    /// pure virtual function for dumping roi mapping
    virtual bool dumpRoiMap(const std::string& filename) const = 0;
    
    unsigned int getBitMaskValue( const unsigned int * uintValue,
				  const unsigned int mask ) const;
    
    enum RoiWordFormat{
      Run2, Run3
    };
    void updateBitMask( const RoiWordFormat );
    
    enum MuonTriggerSystem{
      Barrel, Endcap, Forward, Undef
    };
    MuonTriggerSystem getSystem( const unsigned int& roiWord ) const;
    
    unsigned int IsRun3Mask() const {return m_IsRun3Mask;}
    unsigned int IsVetoedMask() const {return m_IsVetoedMask;}
    unsigned int ChargeMask() const {return m_ChargeMask;}
    unsigned int IsFirstCandMask() const {return m_IsFirstCandMask;}
    unsigned int SectorAddressMask() const {return m_SectorAddressMask;}
    unsigned int BarrelSectorIDMask() const {return m_BarrelSectorIDMask;}
    unsigned int EndcapSectorIDMask() const {return m_EndcapSectorIDMask;}
    unsigned int ForwardSectorIDMask() const {return m_ForwardSectorIDMask;}
    unsigned int SysIDMask() const {return m_SysIDMask;}
    unsigned int SubSysIDMask() const {return m_SubSysIDMask;}
    unsigned int ThresholdMask() const {return m_ThresholdMask;}
    unsigned int BarrelRoIMask() const {return m_BarrelRoIMask;}
    unsigned int EndcapRoIMask() const {return m_EndcapRoIMask;}
    unsigned int ForwardRoIMask() const {return m_ForwardRoIMask;}
    unsigned int EndcapRMask() const {return m_EndcapRMask;}
    unsigned int EndcapPhiMask() const {return m_EndcapPhiMask;}
    unsigned int ForwardRMask() const {return m_ForwardRMask;}
    unsigned int ForwardPhiMask() const {return m_ForwardPhiMask;}
    unsigned int OverflowPerRoIMask() const {return m_OverflowPerRoIMask;}
    unsigned int OverflowPerSectorMask() const {return m_OverflowPerSectorMask;}
    unsigned int BW2Or3Mask() const {return m_BW2Or3Mask;}
    unsigned int InnerCoinMask() const {return m_InnerCoinMask;}
    unsigned int GoodMFMask() const {return m_GoodMFMask;}

    // etaDimLow and etaDimHigh only exist for RPCs, but without this, one cannot use the interface in MuFastDataPreparator
    virtual bool etaDimLow (const TrigT1MuonRecRoiData& data, double& etaMin, double& etaMax) const = 0;
    virtual bool etaDimHigh(const TrigT1MuonRecRoiData& data, double& etaMin, double& etaMax) const = 0;

  private:

    unsigned int m_IsRun3Mask{0};
    unsigned int m_IsVetoedMask{0};
    unsigned int m_ChargeMask{0};
    unsigned int m_IsFirstCandMask{0};
    unsigned int m_SectorAddressMask{0};
    unsigned int m_BarrelSectorIDMask{0};
    unsigned int m_EndcapSectorIDMask{0};
    unsigned int m_ForwardSectorIDMask{0};
    unsigned int m_SysIDMask{0};
    unsigned int m_SubSysIDMask{0};
    unsigned int m_ThresholdMask{0};
    unsigned int m_BarrelRoIMask{0};
    unsigned int m_EndcapRoIMask{0};
    unsigned int m_ForwardRoIMask{0};
    unsigned int m_EndcapRMask{0};
    unsigned int m_EndcapPhiMask{0};
    unsigned int m_ForwardRMask{0};
    unsigned int m_ForwardPhiMask{0};
    unsigned int m_OverflowPerRoIMask{0};
    unsigned int m_OverflowPerSectorMask{0};
    unsigned int m_BW2Or3Mask{0};
    unsigned int m_InnerCoinMask{0};
    unsigned int m_GoodMFMask{0};

  }; // end of ITrigT1MuonRecRoiTool

} // namespace LVL1

#endif
