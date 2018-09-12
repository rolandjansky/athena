/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"

// Local include(s):
#include "MuonMomentumCorrections/MuonCalibrationPeriodTool.h"
#include <cmath>

namespace CP {
    
    
    StatusCode MuonCalibrationPeriodTool::beginEvent() {
        static const unsigned int mc16a_period_number = 284500;
        static const unsigned int mc16d_period_number = 300000;
        static const unsigned int mc16e_period_number = 320000;
        // I've copied the run number ranges from SUSYTools
        // https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/SUSYPhys/SUSYTools/Root/SUSYObjDef_xAOD.cxx#L2438
        static const unsigned int last_run_16  = 320000;
        static const unsigned int last_run_17  = 350000;
        // Not the appropiate value but okay we can fix it afterwards
        static const unsigned int last_run_18  = 500000;
              
        const xAOD::EventInfo* info = nullptr;
        ATH_CHECK(evtStore()->retrieve(info, "EventInfo"));
     
        unsigned int run = info->runNumber();
        bool isData = !info->eventType(xAOD::EventInfo::IS_SIMULATION);        
        if ( (isData  && run <= last_run_16) || (!isData && run == mc16a_period_number) ) m_activeTool = m_calibTool_1516.operator->();
        else if ( (isData  && run <= last_run_17) || (!isData && run == mc16d_period_number) ) m_activeTool = m_calibTool_17.operator->();
        else if ( (isData  && run <= last_run_18) || (!isData && run == mc16e_period_number) ) m_activeTool = m_calibTool_18.operator->();
        else {
            ATH_MSG_FATAL("Could not determine on which year we are running");
            return StatusCode::FAILURE;
        }
         
        return StatusCode::SUCCESS;
    }

    StatusCode MuonCalibrationPeriodTool::initialize() {
        if (!m_calibTool_1516.isUserConfigured()){
            ATH_MSG_INFO("Setup the MuonMomentum calibration tool for 2015-2016 & mc16a");
            m_calibTool_1516.setTypeAndName("CP::MuonCalibrationAndSmearingTool/"+name()+"_1516");
            ATH_CHECK(m_calibTool_1516.setProperty("Year", "Data16"));
            ATH_CHECK(m_calibTool_1516.setProperty("Release", m_release));
            ATH_CHECK(m_calibTool_1516.setProperty("SagittaRelease", m_sagittaRelease1516));
            ATH_CHECK(m_calibTool_1516.setProperty("StatComb", false));
            ATH_CHECK(m_calibTool_1516.setProperty("SagittaCorr", true));
            ATH_CHECK(m_calibTool_1516.setProperty("doSagittaMCDistortion", false));
            ATH_CHECK(m_calibTool_1516.setProperty("SagittaCorrPhaseSpace", true));
        }
        if (!m_calibTool_17.isUserConfigured()){
            ATH_MSG_INFO("Setup the MuonMomentum calibration tool for 2017 & mc16c/d");
            m_calibTool_17.setTypeAndName("CP::MuonCalibrationAndSmearingTool/"+name()+"_17");
            ATH_CHECK(m_calibTool_17.setProperty("Year", "Data17"));
            ATH_CHECK(m_calibTool_17.setProperty("Release", m_release));
            ATH_CHECK(m_calibTool_17.setProperty("SagittaRelease", m_sagittaRelease17));
            ATH_CHECK(m_calibTool_17.setProperty("StatComb", false));
            ATH_CHECK(m_calibTool_17.setProperty("SagittaCorr", true));
            ATH_CHECK(m_calibTool_17.setProperty("doSagittaMCDistortion", false));
            ATH_CHECK(m_calibTool_17.setProperty("SagittaCorrPhaseSpace", true));
        }
        if (!m_calibTool_18.isUserConfigured()){
            ATH_MSG_INFO("Setup the MuonMomentum calibration tool for 2018 & mc16e");
            m_calibTool_18.setTypeAndName("CP::MuonCalibrationAndSmearingTool/"+name()+"_18");
            ATH_CHECK(m_calibTool_18.setProperty("Year", "Data17"));
            ATH_CHECK(m_calibTool_18.setProperty("Release", m_release));
            ATH_CHECK(m_calibTool_18.setProperty("SagittaRelease", m_sagittaRelease18));
            ATH_CHECK(m_calibTool_18.setProperty("StatComb", false));
            ATH_CHECK(m_calibTool_18.setProperty("SagittaCorr", false));
            ATH_CHECK(m_calibTool_18.setProperty("doSagittaMCDistortion", true));
            ATH_CHECK(m_calibTool_18.setProperty("SagittaCorrPhaseSpace", true));
        }        
        ATH_CHECK(m_calibTool_1516.retrieve());
        ATH_CHECK(m_calibTool_17.retrieve());
        ATH_CHECK(m_calibTool_18.retrieve());
        
        return StatusCode::SUCCESS;
    }
    
  MuonCalibrationPeriodTool::MuonCalibrationPeriodTool( const std::string& name ) :
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
    MuonCalibrationPeriodTool::~MuonCalibrationPeriodTool(){}

    CorrectionCode MuonCalibrationPeriodTool::applyCorrection( xAOD::Muon& mu ) const {
        if (m_activeTool == nullptr){
            ATH_MSG_FATAL("No valid tool has been found to apply the correction of muon with pt: "<<mu.pt()/1.e3<<"  eta: "<<mu.eta()<<" phi: "<<mu.phi());
            return CorrectionCode::Error;
        }
        return m_activeTool->applyCorrection(mu);
    }
    CorrectionCode MuonCalibrationPeriodTool::correctedCopy( const xAOD::Muon& input, xAOD::Muon*& output ) const {
        if (m_activeTool == nullptr){
            ATH_MSG_FATAL("No valid tool has been found to apply the correction of muon with pt: "<<input.pt()/1.e3<<"  eta: "<<input.eta()<<" phi: "<<input.phi());
            return CorrectionCode::Error;
        }
        return m_activeTool->correctedCopy(input, output);
    
    }
    CorrectionCode MuonCalibrationPeriodTool::applyCorrectionTrkOnly( xAOD::TrackParticle& inTrk, const int DetType) const {
        if (m_activeTool == nullptr){
            ATH_MSG_FATAL("No valid tool has been found to apply the correction of the track with pt: "<<inTrk.pt()/1.e3<<"  eta: "<<inTrk.eta()<<" phi: "<<inTrk.phi());
            return CorrectionCode::Error;
        }
        return m_activeTool->applyCorrectionTrkOnly(inTrk, DetType);        
    }    
    double MuonCalibrationPeriodTool::expectedResolution( const std::string& DetType, const xAOD::Muon& mu, const bool mc ) const {
        if (m_activeTool == nullptr){
            ATH_MSG_FATAL("No valid tool has been found to predict the resolution of muon with pt: "<<mu.pt()/1.e3<<"  eta: "<<mu.eta()<<" phi: "<<mu.phi());
            return std::nan("float");
        }
        return m_activeTool->expectedResolution(DetType,mu,mc);
    }
    double MuonCalibrationPeriodTool::expectedResolution( const int& DetType, const xAOD::Muon& mu, const bool mc ) const {
        if (m_activeTool == nullptr){
            ATH_MSG_FATAL("No valid tool has been found to predict the resolution of muon with pt: "<<mu.pt()/1.e3<<"  eta: "<<mu.eta()<<" phi: "<<mu.phi());
            return std::nan("float");
        }
        return m_activeTool->expectedResolution(DetType,mu,mc);
    }
    // Systematics do not change throughout the year. So let's take the ones given by 1516 tool
    bool MuonCalibrationPeriodTool::isAffectedBySystematic( const SystematicVariation& systematic ) const {
        return m_calibTool_1516->isAffectedBySystematic(systematic);
    }
    SystematicSet MuonCalibrationPeriodTool::affectingSystematics() const {
        return m_calibTool_1516->affectingSystematics();
    }
    SystematicSet MuonCalibrationPeriodTool::recommendedSystematics() const {
        return m_calibTool_1516->recommendedSystematics();
    }
    // We need to parse the variations to all three instances because we do not know whether the file changes in the backgronud
    // during the variation loop
    SystematicCode MuonCalibrationPeriodTool::applySystematicVariation ( const SystematicSet& systConfig ) {
        if (m_calibTool_1516->applySystematicVariation(systConfig) == SystematicCode::Unsupported) {}
        if (m_calibTool_17->applySystematicVariation(systConfig) == SystematicCode::Unsupported) {}
        return m_calibTool_18->applySystematicVariation(systConfig);
    }

} // namespace CP
