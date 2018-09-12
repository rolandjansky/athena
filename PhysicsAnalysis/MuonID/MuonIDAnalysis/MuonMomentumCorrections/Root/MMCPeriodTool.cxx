/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Framework include(s):
#include "PathResolver/PathResolver.h"

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"

// Local include(s):
#include "MuonMomentumCorrections/MMCPeriodTool.h"
#include <cmath>

namespace CP {

  MMCPeriodTool::MMCPeriodTool( const std::string& name ) :
        asg::AsgMetadataTool( name ),
        m_calibTool_1516(),
        m_calibTool_17(),
        m_calibTool_18(),
        m_activeTool(nullptr),
        m_sagittaRelease1516("sagittaBiasDataAll_25_07_17"),
        m_sagittaRelease17("sagittaBiasDataAll_30_07_18"),
        m_sagittaRelease18("sagittaBiasDataAll_30_07_18"),
        m_release("Recs2017_08_02"){

        m_calibTool_1516.declarePropertyFor(this, "CalibrationTool1516", "Parse the configured instance of the calibration tool for 15&16 data or mc16a");
        m_calibTool_17.declarePropertyFor(this, "CalibrationTool17", "Parse the configured instance of the calibration tool for 17 data or mc16d");
        m_calibTool_17.declarePropertyFor(this, "CalibrationTool18", "Parse the configured instance of the calibration tool for 18 data or mc16e");
        declareProperty("SagittaRelease1516", m_sagittaRelease1516);
        declareProperty("SagittaRelease17", m_sagittaRelease17);
        declareProperty("SagittaRelease18", m_sagittaRelease18);
        declareProperty("Release", m_release);
    }



    CorrectionCode MMCPeriodTool::applyCorrection( xAOD::Muon& mu ) const {
        if (m_activeTool == nullptr){
            ATH_MSG_FATAL("No valid tool has been found to apply the correction of muon with pt: "<<mu.pt()/1.e3<<"  eta: "<<mu.eta()<<" phi: "<<mu.phi());
            return CorrectionCode::Error;
        }
        return m_activeTool->applyCorrection(mu);
    }
    CorrectionCode MMCPeriodTool::correctedCopy( const xAOD::Muon& input, xAOD::Muon*& output ) const {
        if (m_activeTool == nullptr){
            ATH_MSG_FATAL("No valid tool has been found to apply the correction of muon with pt: "<<input.pt()/1.e3<<"  eta: "<<input.eta()<<" phi: "<<input.phi());
            return CorrectionCode::Error;
        }
        return m_activeTool->correctedCopy(input, output);
    
    }
    CorrectionCode MMCPeriodTool::applyCorrectionTrkOnly( xAOD::TrackParticle& inTrk, const int DetType) const {
        if (m_activeTool == nullptr){
            ATH_MSG_FATAL("No valid tool has been found to apply the correction of the track with pt: "<<inTrk.pt()/1.e3<<"  eta: "<<inTrk.eta()<<" phi: "<<inTrk.phi());
            return CorrectionCode::Error;
        }
        return m_activeTool->applyCorrectionTrkOnly(inTrk, DetType);        
    }    
    double MMCPeriodTool::expectedResolution( const std::string& DetType, const xAOD::Muon& mu, const bool mc ) const {
        if (m_activeTool == nullptr){
            ATH_MSG_FATAL("No valid tool has been found to predict the resolution of muon with pt: "<<mu.pt()/1.e3<<"  eta: "<<mu.eta()<<" phi: "<<mu.phi());
            return std::nan("float");
        }
        return m_activeTool->expectedResolution(DetType,mu,mc);
    }
    double MMCPeriodTool::expectedResolution( const int& DetType, const xAOD::Muon& mu, const bool mc ) const {
        if (m_activeTool == nullptr){
            ATH_MSG_FATAL("No valid tool has been found to predict the resolution of muon with pt: "<<mu.pt()/1.e3<<"  eta: "<<mu.eta()<<" phi: "<<mu.phi());
            return std::nan("float");
        }
        return m_activeTool->expectedResolution(DetType,mu,mc);
    }
    // Systematics do not change throughout the year. So let's take the ones given by 1516 tool
    bool MMCPeriodTool::isAffectedBySystematic( const SystematicVariation& systematic ) const {
        return m_calibTool_1516->isAffectedBySystematic(systematic);
    }
    SystematicSet MMCPeriodTool::affectingSystematics() const {
        return m_calibTool_1516->affectingSystematics();
    }
    SystematicSet MMCPeriodTool::recommendedSystematics() const {
        return m_calibTool_1516->recommendedSystematics();
    }
    SystematicCode MMCPeriodTool::applySystematicVariation ( const SystematicSet& systConfig ) {
        return 
    }


    

 

} // namespace CP
