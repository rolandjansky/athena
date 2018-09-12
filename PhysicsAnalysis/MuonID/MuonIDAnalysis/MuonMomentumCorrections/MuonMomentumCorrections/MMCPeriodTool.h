/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCAST_MMCPeriodTool_H
#define MCAST_MMCPeriodTool_H

// Framework include(s):
#include <AsgTools/AsgMetadataTool.h>
#include <AsgTools/AnaToolHandle.h>

// Local include(s):
#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"


namespace CP {


class MMCPeriodTool : public virtual IMuonCalibrationAndSmearingTool, public virtual ISystematicsTool, public asg::AsgMetadataTool {

  // Create a proper constructor for Athena
  ASG_TOOL_CLASS2( MMCPeriodTool, CP::IMuonCalibrationAndSmearingTool, CP::ISystematicsTool )

  public:
     /// Apply the correction on a modifyable object
    virtual CorrectionCode applyCorrection( xAOD::Muon& mu ) const;
    /// Create a corrected copy from a constant muon
    virtual CorrectionCode correctedCopy( const xAOD::Muon& input, xAOD::Muon*& output ) const;
    /// Get the expected pT resolution
    virtual double expectedResolution( const std::string& DetType, const xAOD::Muon& mu, const bool mc ) const;
    /// Get the expected pT resolution - int argument is more efficient
    virtual double expectedResolution( const int& DetType, const xAOD::Muon& mu, const bool mc ) const;
    /// Expert method to apply the MC correction on a modifyable trackParticle for ID- or MS-only corrections
    virtual CorrectionCode applyCorrectionTrkOnly( xAOD::TrackParticle& inTrk, const int DetType) const;
    
    
    // Interface - Is the tool affected by a specific systematic?
    virtual bool isAffectedBySystematic( const SystematicVariation& systematic ) const;
    // Interface - Which systematics have an effect on the tool's behaviour?
    virtual SystematicSet affectingSystematics() const;
    // Interface - Systematics to be used for physics analysis
    virtual SystematicSet recommendedSystematics() const;
    // Interface - Use specific systematic
    virtual SystematicCode applySystematicVariation ( const SystematicSet& systConfig );
    
    // Constructor
    MMCPeriodTool( const std::string& name );

    // Destructor
    virtual ~MMCPeriodTool();

    virtual StatusCode initialize();

    

    virtual StatusCode beginInputFile();
    
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
