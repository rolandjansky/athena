/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCAST_MuonCalibrationPeriodTool_H
#define MCAST_MuonCalibrationPeriodTool_H

// Framework include(s):
#include <AsgTools/AsgMetadataTool.h>
#include <AsgTools/AnaToolHandle.h>

// Local include(s):
#include "MuonMomentumCorrections/IMuonCalibrationAndSmearingTool.h"
// EDM include(s):

#include "xAODEventInfo/EventInfo.h"

namespace CP {


class MuonCalibrationPeriodTool : public virtual IMuonCalibrationAndSmearingTool, public virtual ISystematicsTool, public asg::AsgMetadataTool {
  
  // Create a proper constructor for Athena
  ASG_TOOL_CLASS2( MuonCalibrationPeriodTool, CP::IMuonCalibrationAndSmearingTool, CP::ISystematicsTool )
  
  public:
     // There are two recommended options by MCP to setup the tool (https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/MCPAnalysisConsolidationMC16)
     //  1) Correct data for the sagitta bias -- if no statistical limits
     //  2) Add extra systematic for MC and keep data untouched
     //  The calibration mode can be set via the property 'calibrationMode'
     enum CalibMode{
        correctData =0,
        additionalMCsys = 1, // <--- default
        expert =2
     };
     /// Apply the correction on a modifyable object
    virtual CorrectionCode applyCorrection( xAOD::Muon& mu ) const override;
    /// Create a corrected copy from a constant muon
    virtual CorrectionCode correctedCopy( const xAOD::Muon& input, xAOD::Muon*& output ) const override;
    /// Get the expected pT resolution
    virtual double expectedResolution( const std::string& DetType, const xAOD::Muon& mu, const bool mc ) const override;
    /// Get the expected pT resolution - int argument is more efficient
    virtual double expectedResolution( const int& DetType, const xAOD::Muon& mu, const bool mc ) const override;
    /// Expert method to apply the MC correction on a modifyable trackParticle for ID- or MS-only corrections
    virtual CorrectionCode applyCorrectionTrkOnly( xAOD::TrackParticle& inTrk, const int DetType) const override;
    
    
    // Interface - Is the tool affected by a specific systematic?
    virtual bool isAffectedBySystematic( const SystematicVariation& systematic ) const override;
    // Interface - Which systematics have an effect on the tool's behaviour?
    virtual SystematicSet affectingSystematics() const override;
    // Interface - Systematics to be used for physics analysis
    virtual SystematicSet recommendedSystematics() const override;
    // Interface - Use specific systematic
    virtual SystematicCode applySystematicVariation ( const SystematicSet& systConfig ) override;
    
    // Constructor
    MuonCalibrationPeriodTool( const std::string& name );

    // Destructor
    virtual ~MuonCalibrationPeriodTool();

    // Initializing the subtools
    virtual StatusCode initialize() override;
    // Retrieve the runNumber of the event and assign the correct tool 
    virtual StatusCode beginEvent() override;
    
    private:
        
        unsigned int getPeriod() const;
        const CP::IMuonCalibrationAndSmearingTool* getTool() const;
        
        // Have three tool instances for each Monte Carlo campaign
        asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> m_calibTool_1516;
        asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> m_calibTool_17;
        asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> m_calibTool_18;
        
        
        mutable const CP::IMuonCalibrationAndSmearingTool* m_activeTool;
        const xAOD::EventInfo* m_evInfo;
        
        // Sagitta and calibration releases which are parsed to
        // the three instances of the tool
        std::string m_sagittaRelease1516;
        std::string m_sagittaRelease17;
        std::string m_sagittaRelease18;
        std::string m_release;
        
        int m_calib_mode;
        
        bool m_StatComb1516;
        bool m_SagittaCorr1516;
        bool m_SagittaMCDistortion1516;
        bool m_SagittaCorrPhaseSpace1516;
        
        bool m_StatComb17;
        bool m_SagittaCorr17;
        bool m_SagittaMCDistortion17;
        bool m_SagittaCorrPhaseSpace17;
        
        bool m_StatComb18;
        bool m_SagittaCorr18;
        bool m_SagittaMCDistortion18;
        bool m_SagittaCorrPhaseSpace18;
        
        
        
        // Monte Carlo runNumbers correspond to different production campaigns
        std::vector<unsigned int> m_MCperiods1516;
        std::vector<unsigned int> m_MCperiods17;
        std::vector<unsigned int> m_MCperiods18;
        // Optionally one can use the random run number to assign the right tool in MC
        bool m_useRndRun;
 
  }; 

} // namespace CP

#endif
