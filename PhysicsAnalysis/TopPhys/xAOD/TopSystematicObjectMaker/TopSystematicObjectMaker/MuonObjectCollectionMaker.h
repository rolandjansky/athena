/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

// $Id: MuonObjectCollectionMaker.h 799839 2017-03-08 11:07:28Z grancagn $
#ifndef ANALYSISTOP_TOPSYSTEMATICOBJECTMAKER_MUONOBJECTCOLLECTIONMAKER_H
#define ANALYSISTOP_TOPSYSTEMATICOBJECTMAKER_MUONOBJECTCOLLECTIONMAKER_H

/**
 * @author John Morris <john.morris@cern.ch>
 *
 * @brief MuonObjectCollectionMaker
 *   Makes all systematic variations of muons.
 *   Momentum Corrections are applied using the calibration tool.
 *   Efficiency Corrections are applied using the efficiency tool.
 *
 * $Revision: 799839 $
 * $Date: 2017-03-08 11:07:28 +0000 (Wed, 08 Mar 2017) $
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
#include "IsolationSelection/IIsolationSelectionTool.h"
#include "IsolationSelection/IIsolationLowPtPLVTool.h"
#include "AsgAnalysisInterfaces/IClassificationTool.h"
// the following is needed to make sure all muons for which d0sig is calculated are at least Loose
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"
#include "MuonMomentumCorrections/MuonCalibrationPeriodTool.h"
// Forward declaration(s):
namespace top {
  class TopConfig;
}

namespace top {
  class MuonObjectCollectionMaker final: public asg::AsgTool {
  public:
    explicit MuonObjectCollectionMaker(const std::string& name);
    virtual ~MuonObjectCollectionMaker() {}

    // Delete Standard constructors
    MuonObjectCollectionMaker(const MuonObjectCollectionMaker& rhs) = delete;
    MuonObjectCollectionMaker(MuonObjectCollectionMaker&& rhs) = delete;
    MuonObjectCollectionMaker& operator = (const MuonObjectCollectionMaker& rhs) = delete;

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

    ///-- Isolation --///
    //until release 21.2.110 these are the used WPs
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_FCTight;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_FCLoose;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_FCTightTrackOnly;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_FCTightTrackOnly_FixedRad;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_FCLoose_FixedRad;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_FCTight_FixedRad;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_FixedCutPflowTight;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_FixedCutPflowLoose;
    //since release 21.2.111 these are the used WPs
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_PflowTight_FixedRad;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_PflowLoose_FixedRad;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_PflowTight_VarRad;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_PflowLoose_VarRad;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_HighPtTrackOnly;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_TightTrackOnly_VarRad;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_TightTrackOnly_FixedRad;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_PLVTight;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_PLVLoose;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_Tight_VarRad;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_Tight_FixedRad;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_Loose_VarRad;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_Loose_FixedRad;
    ToolHandle<CP::IIsolationLowPtPLVTool> m_isolationTool_LowPtPLV;
    ToolHandle<CP::IClassificationTool> m_IFFTruthTool;
    //These WPs are defined with release 21.2.156 or later
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_PLImprovedTight;
    ToolHandle<CP::IIsolationSelectionTool> m_isolationTool_PLImprovedVeryTight;
    // the following is needed to make sure all muons for which d0sig is calculated are at least Loose
    ToolHandle<CP::IMuonSelectionTool> m_muonSelectionToolVeryLooseVeto;

    bool m_isFirstEvent;
  };
} // namespace
#endif
