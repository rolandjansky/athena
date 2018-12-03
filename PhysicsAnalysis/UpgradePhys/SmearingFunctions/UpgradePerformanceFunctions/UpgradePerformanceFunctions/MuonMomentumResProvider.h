/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MOMENTUM_RES_PROVIDER_H
#define MUON_MOMENTUM_RES_PROVIDER_H

    // helper tool to access the muon pt resolutions via by the MCAST tool
    // can't use the nominal tool directly due to the "D3PD" nature of UpgradePerformanceFunctions :( 
    // March 2017, goblirsc<at> cern.ch
    

#include "AsgTools/ToolHandle.h"
#include "MuonMomentumCorrections/MuonCalibrationAndSmearingTool.h"

class MuonMomentumResProvider : public virtual CP::MuonCalibrationAndSmearingTool{

public:
    MuonMomentumResProvider(const std::string & name);
    double getExpMsResolution(double pt, double eta, double phi); 
    double getExpItkResolution(double pt, double eta, double phi); 
    // also overload this guy to get out custom resos!
    virtual StatusCode initialize();
private:
    // release to use for the upgrade-specific calibration files
    std::string m_upgradeResolutionRelease;

};
    



#endif // MUON_MOMENTUM_RES_PROVIDER_H
