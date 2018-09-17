/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCAST_MuonCalibrationPeriodTool_H
#define MCAST_MuonCalibrationPeriodTool_H

// Framework include(s):
#include <AsgTools/AsgMetadataTool.h>
#include <AsgTools/AnaToolHandle.h>

// Local include(s):
#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"


namespace CP {


class MuonCalibrationPeriodTool : public virtual IMuonCalibrationAndSmearingTool, public virtual ISystematicsTool, public asg::AsgMetadataTool {

  // Create a proper constructor for Athena
  ASG_TOOL_CLASS2( MuonCalibrationPeriodTool, CP::IMuonCalibrationAndSmearingTool, CP::ISystematicsTool )

  public:
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
    virtual SystematicCode applySystematicVariation ( const SystematicSet& systConfig );
    
    // Constructor
    MuonCalibrationPeriodTool( const std::string& name );

    // Destructor
    virtual ~MuonCalibrationPeriodTool();

    // Initializing the subtools
    virtual StatusCode initialize() override;
    // Retrieve the runNumber of the event and assign the correct tool 
    virtual StatusCode beginEvent() override;
    
    private:
        // Have three tool instances for each Monte Carlo campaign
        asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> m_calibTool_1516;
        asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> m_calibTool_17;
        asg::AnaToolHandle<CP::IMuonCalibrationAndSmearingTool> m_calibTool_18;
        
        // The runnumber of the file to be read once the file is opened and closed 
        const CP::IMuonCalibrationAndSmearingTool* m_activeTool;
        
        std::string m_sagittaRelease1516;
        std::string m_sagittaRelease17;
        std::string m_sagittaRelease18;
        std::string m_release;
        
        
        


  }; 

} // namespace CP

#endif
