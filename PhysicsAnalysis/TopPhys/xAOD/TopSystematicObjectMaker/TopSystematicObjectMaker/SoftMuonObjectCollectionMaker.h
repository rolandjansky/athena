/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ANALYSISTOP_TOPSYSTEMATICOBJECTMAKER_SOFTMUONOBJECTCOLLECTIONMAKER_H
#define ANALYSISTOP_TOPSYSTEMATICOBJECTMAKER_SOFTMUONOBJECTCOLLECTIONMAKER_H

/**
 * @author Marco Vanadia <marco.vanadia@cern.ch>
 *
 * @brief SoftMuonObjectCollectionMaker
 *   Makes all systematic variations of soft muons.
 *   Momentum Corrections are applied using the calibration tool.
 *   Efficiency Corrections are applied using the efficiency tool.
 *
 **/

// system include(s):
#include <memory>
#include <set>
#include <list>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

// CP Tool include(s):
#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
//#include "IsolationSelection/IIsolationSelectionTool.h"
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "MuonMomentumCorrections/MuonCalibrationPeriodTool.h"
// Forward declaration(s):
namespace top {
  class TopConfig;
}

namespace top {
  class SoftMuonObjectCollectionMaker final: public asg::AsgTool {
  public:
    explicit SoftMuonObjectCollectionMaker(const std::string& name);
    virtual ~SoftMuonObjectCollectionMaker() {}

    // Delete Standard constructors
    SoftMuonObjectCollectionMaker(const SoftMuonObjectCollectionMaker& rhs) = delete;
    SoftMuonObjectCollectionMaker(SoftMuonObjectCollectionMaker&& rhs) = delete;
    SoftMuonObjectCollectionMaker& operator = (const SoftMuonObjectCollectionMaker& rhs) = delete;

    StatusCode initialize();
    StatusCode execute(bool);

    StatusCode printout();

    // return specific Systematic
    inline virtual const std::list<CP::SystematicSet>& specifiedSystematics() const {return m_specifiedSystematics;}

    // return all recommendedSystematics
    inline const std::list<CP::SystematicSet>& recommendedSystematics() const {return m_recommendedSystematics;}
  protected:
    // specify Systematic
    virtual void specifiedSystematics(const std::set<std::string>& specifiedSystematics);
  private:
    
    std::shared_ptr<top::TopConfig> m_config;
    std::list<CP::SystematicSet> m_specifiedSystematics;
    std::list<CP::SystematicSet> m_recommendedSystematics;

    ToolHandle<CP::IMuonCalibrationAndSmearingTool> m_calibrationPeriodTool;
    ToolHandle<CP::IMuonCalibrationAndSmearingTool> m_calibrationTool2017;
    
    // the following is needed to make sure all muons for which d0sig is calculated are at least Loose
    
    ToolHandle<CP::IMuonSelectionTool> m_muonSelectionToolVeryLooseVeto,m_softmuonSelectionTool;
    
  };
} // namespace
#endif
