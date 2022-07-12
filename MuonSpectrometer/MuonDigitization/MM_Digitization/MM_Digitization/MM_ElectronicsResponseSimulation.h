/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_MM_ELECTRONICSRESPONSESIMULATION_H
#define MM_DIGITIZATION_MM_ELECTRONICSRESPONSESIMULATION_H
/** @class MM_ElectronicsResponseSimulation

// ------------
// Authors:
//   Iakovidis George        <george.iakovidis@cern.ch>
//   Karakostas Konstantinos <konstantinos.karakostas@cern.ch>
//   Leontsinis Stefanos     <stefanos.leontsinis@cern.ch>
//   Nektarios Chr. Benekos  <nbenekos@cern.ch>
//   Jessica Metcalfe        <jessica.metcalfe@gmail.com>
//
// Major Contributions From: Verena Martinez
//                           Tomoyuki Saito
//
// Major Restructuring for r21+ From: Lawrence Lee <lawrence.lee.jr@cern.ch>
//
//////////////////////////////////////////////////////////////////////////////

*/

#include <TF1.h>

/// STD'S
#include <sys/stat.h>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

/// Projects
#include "MM_Digitization/MM_DigitToolOutput.h"
#include "MM_Digitization/MM_ElectronicsToolInput.h"
#include "MM_Digitization/MM_ElectronicsToolTriggerOutput.h"

// VMM Mapping
#include "MM_Digitization/MM_StripVmmMappingTool.h"
#include "MM_Digitization/VMM_Shaper.h"
namespace CLHEP{
  class HepRandomEngine;
}
class MM_ElectronicsResponseSimulation {
public:
    
    struct ConfigModule {
      /** power of responce function */
      float peakTime{0.};
      float timeWindowLowerOffset{0.};
      float timeWindowUpperOffset{0.};
      float stripDeadTime{0.};
      float artDeadTime{0.};
      float vmmDeadtime{0.};
      float vmmUpperGrazeWindow{0.};
      bool useNeighborLogic{false};
    };

    MM_ElectronicsResponseSimulation(ConfigModule&& module);
    ~MM_ElectronicsResponseSimulation() = default;   
   

    MM_DigitToolOutput getPeakResponseFrom(const MM_ElectronicsToolInput& digiInput) const;
    MM_DigitToolOutput getThresholdResponseFrom(const MM_ElectronicsToolInput& digiInput) const;

    MM_DigitToolOutput applyDeadTimeStrip(const MM_DigitToolOutput& ElectronicsTriggerOutput) const;
   
    MM_ElectronicsToolTriggerOutput getTheFastestSignalInVMM(const MM_DigitToolOutput& ElectronicThresholdOutput, const int chMax,
                                                             const int stationEta);
    
    MM_ElectronicsToolTriggerOutput applyDeadTimeART(const MM_ElectronicsToolTriggerOutput& ElectronicsTriggerOutput);
    MM_ElectronicsToolTriggerOutput applyARTTiming(const MM_ElectronicsToolTriggerOutput& ElectronicsTriggerOutput, CLHEP::HepRandomEngine* random_engine, float jitter,
                                                   float offset) const ;
    
    

    
    float getPeakTime() const { return m_cfg.peakTime; };
    float getTimeWindowLowerOffset() const { return m_cfg.timeWindowLowerOffset; };
    float getTimeWindowUpperOffset() const { return m_cfg.timeWindowUpperOffset; };
    float getVmmDeadTime() { return m_cfg.vmmDeadtime; }
    float getVmmUpperGrazeWindow() { return m_cfg.vmmUpperGrazeWindow; }
    float getStripdeadtime() const { return m_cfg.stripDeadTime; };
    float getARTdeadtime() const { return m_cfg.artDeadTime; };

  

private:
    const ConfigModule m_cfg{};
    
    struct DataCache {
        std::vector<float> tStripElectronicsAbThr{};
        std::vector<float> qStripElectronics{};
        std::vector<int> nStripElectronics{};
        std::unique_ptr<VMM_Shaper> vmmShaper{};
    };
    
    std::unique_ptr<VMM_Shaper> m_vmmShaper{};
      
    void vmmPeakResponseFunction(DataCache& cache, const MM_ElectronicsToolInput& digiInput) const;
    void vmmThresholdResponseFunction(DataCache& cache, const MM_ElectronicsToolInput& digiInput) const;

    void getVMMId(const std::vector<int>& ElectronicsThreshold_stripPos, const int chMax, const int stationEta,
                  std::vector<int>& trigger_VMM_id, std::vector<int>& trigger_MMFE_VMM_id) const;

    bool deadChannel(int id, float time, const std::vector<int>& v_id, const std::vector<float>& v_time, float deadtime) const;
 
    int getIdTheFastestSignalInVMM(float time, int VMM_id, std::vector<int> trigger_VMM_id,
                                   const std::vector<float>& ElectronicsThreshold_stripTime, float timeWindowLower, float timeWindowUpper) const;
 
    
};

#endif
