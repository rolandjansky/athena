/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include <MuonMomentumCorrections/MuonCalibrationPeriodTool.h>

namespace CP {
    
    
    StatusCode MuonCalibrationPeriodTool::beginEvent() {
        ATH_CHECK(evtStore()->retrieve(m_evInfo, "EventInfo"));
        // reset the active tool
        m_activeTool = nullptr;
        return StatusCode::SUCCESS;
    }
   unsigned int MuonCalibrationPeriodTool::getPeriod() const {
        static SG::AuxElement::ConstAccessor<unsigned int> acc_rnd("RandomRunNumber");
        // I've copied the run number ranges from SUSYTools
        // https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/SUSYPhys/SUSYTools/Root/SUSYObjDef_xAOD.cxx#L2438
        static const unsigned int last_run_16  = 320000;
        static const unsigned int last_run_17  = 350000;
        // Not the appropiate value but okay we can fix it afterwards
        static const unsigned int last_run_18  = 500000;
      
        bool isData = !m_evInfo->eventType(xAOD::EventInfo::IS_SIMULATION);        
        unsigned int run = m_evInfo->runNumber();
        // retrieve the random run number
        if (!isData && m_useRndRun){
             if (acc_rnd.isAvailable(*m_evInfo)) run = acc_rnd(*m_evInfo);
             else {
                 ATH_MSG_ERROR("No random runnumber could be found allthough the tool is configured to assign the years based on it. Please make sure to apply the prwTool before-hand or consider to set the property 'useRandomRunNumber' to false.");
                 return -1;
             }
        }
        // Check the Monte carlo
        if (!isData &&  (!m_useRndRun || !acc_rnd.isAvailable(*m_evInfo)) ){
            if (std::find(m_MCperiods1516.begin(), m_MCperiods1516.end(),run) != m_MCperiods1516.end()) {
                ATH_MSG_DEBUG("The current run "<<run<<" corresponds to data mc16a / data15-16");                
                return 1516;
            } else if (std::find(m_MCperiods17.begin(), m_MCperiods17.end(),run) != m_MCperiods17.end()) {
                ATH_MSG_DEBUG("The current run "<<run<<" corresponds to data mc16d / data17");                
                return 17;
            } else if (std::find(m_MCperiods18.begin(), m_MCperiods18.end(),run) != m_MCperiods18.end()){ 
                ATH_MSG_DEBUG("The current run "<<run<<" corresponds to data mc16e / data18");                
                return 18;
            }
        } 
        // Check data itself or the random run number is used
        else if (isData || m_useRndRun) {
            if (run < last_run_16) {
                ATH_MSG_DEBUG("The current run "<<run<<" is taken in data 15-16");
                return 1516;
            } else if (run < last_run_17) {
                 ATH_MSG_DEBUG("The current run "<<run<<" is taken in data 17");
                return 17;
            } else if (run < last_run_18) {
                ATH_MSG_DEBUG("The current run "<<run<<" is taken in data 18");
                return 18;
            }
        }
        ATH_MSG_FATAL("Could not assign run-number "<<run<<" to a specific year of data-taking");
        return -1;
    }
   const CP::IMuonCalibrationAndSmearingTool* MuonCalibrationPeriodTool::getTool() const {
        if (m_activeTool != nullptr) return m_activeTool;
        unsigned int period = getPeriod();
        if (period ==  1516) m_activeTool = m_calibTool_1516.get();
        else if (period ==  17) m_activeTool = m_calibTool_17.get();
        else if (period ==  18) m_activeTool = m_calibTool_18.get();
        return m_activeTool;
    }
    StatusCode MuonCalibrationPeriodTool::initialize() {
        if(m_MCperiods1516.empty()) {
            ATH_MSG_FATAL("No period numbers have been provided for mc16a");
            return StatusCode::FAILURE;
        } else if (m_MCperiods17.empty()){
            ATH_MSG_FATAL("No period numbers have been provided for mc16d");
            return StatusCode::FAILURE;
        } else if (m_MCperiods18.empty()){
            ATH_MSG_FATAL("No period numbers have been provided for mc16e");
            return StatusCode::FAILURE;
        }
        if (m_useRndRun) ATH_MSG_INFO("The assignment of the calibration tools will be based on the random run number. Please make sure to call the pileup-tool before applying this tool");
        bool statComb1516(false), sagittaCorr1516(false),sagittaMCDistortion1516(false), sagittaCorrPhaseSpace1516(false);
        bool statComb17(false), sagittaCorr17(false), sagittaMCDistortion17(false), sagittaCorrPhaseSpace17(false);
        bool statComb18(false), sagittaCorr18(false), sagittaMCDistortion18(false), sagittaCorrPhaseSpace18(false);
        
        if (m_calib_mode == MuonCalibrationPeriodTool::correctData){
            ATH_MSG_INFO("Data will be corrected for sagitta bias which (recommended setup 1)");
            statComb1516 = false;
            sagittaCorr1516 = true;
            sagittaMCDistortion1516 = false;
            sagittaCorrPhaseSpace1516 = true;
            
            statComb17 = false;
            sagittaCorr17 = true;
            sagittaMCDistortion17 = false;
            sagittaCorrPhaseSpace17 = true;
            
            statComb18 = false;
            sagittaCorr18 = true;
            sagittaMCDistortion18 = false;
            sagittaCorrPhaseSpace18 = true;
            
        }  else if (m_calib_mode == MuonCalibrationPeriodTool::additionalMCsys){
            ATH_MSG_INFO("Data will be untouched. Instead an additional systematic will be added (recommended setup 2)");
            statComb1516 = false;
            sagittaCorr1516 = true;
            sagittaMCDistortion1516 = false;
            sagittaCorrPhaseSpace1516 = false;
            
            statComb17 = false;
            sagittaCorr17 = false;
            sagittaMCDistortion17 = true;
            sagittaCorrPhaseSpace17 = false;
            
            statComb18 = false;
            sagittaCorr18 = false;
            sagittaMCDistortion18 = true;
            sagittaCorrPhaseSpace18 = false; 
        } else if (m_calib_mode == MuonCalibrationPeriodTool::expert){
          ATH_MSG_INFO("You've choosen the expert mode and overwrite each property for yourself.");
            statComb1516 = m_StatComb1516;
            sagittaCorr1516 = m_SagittaCorr1516;
            sagittaMCDistortion1516 = m_SagittaMCDistortion1516;
            sagittaCorrPhaseSpace1516 = m_SagittaCorrPhaseSpace1516;
            
            statComb17 = m_StatComb17;
            sagittaCorr17 = m_SagittaCorr17;
            sagittaMCDistortion17 = m_SagittaMCDistortion17;
            sagittaCorrPhaseSpace17 = m_SagittaCorrPhaseSpace17;
            
            statComb18 = m_StatComb18;
            sagittaCorr18 = m_SagittaCorr18;
            sagittaMCDistortion18 = m_SagittaMCDistortion18;
            sagittaCorrPhaseSpace18 = m_SagittaCorrPhaseSpace18;
        
        } else{
            ATH_MSG_FATAL("Invalid  calibration mode: "<<m_calib_mode<<" Allowed modes are correctData("<<MuonCalibrationPeriodTool::correctData
                            <<") additionalMCsys ("<<MuonCalibrationPeriodTool::additionalMCsys<<") or expert ("<<MuonCalibrationPeriodTool::expert<<")");
            return StatusCode::FAILURE;
        }
        
        if (!m_calibTool_1516.isUserConfigured()){
            ATH_MSG_INFO("Setup the MuonMomentum calibration tool for 2015-2016 & mc16a");
            m_calibTool_1516.setTypeAndName("CP::MuonCalibrationAndSmearingTool/"+name()+"_1516");
            ATH_CHECK(m_calibTool_1516.setProperty("Year", "Data16"));
            ATH_CHECK(m_calibTool_1516.setProperty("Release", m_release));
            ATH_CHECK(m_calibTool_1516.setProperty("SagittaRelease", m_sagittaRelease1516));
            ATH_CHECK(m_calibTool_1516.setProperty("StatComb", statComb1516));
            ATH_CHECK(m_calibTool_1516.setProperty("SagittaCorr", sagittaCorr1516));
            ATH_CHECK(m_calibTool_1516.setProperty("doSagittaMCDistortion", sagittaMCDistortion1516));
            ATH_CHECK(m_calibTool_1516.setProperty("SagittaCorrPhaseSpace", sagittaCorrPhaseSpace1516));
        }
        if (!m_calibTool_17.isUserConfigured()){
            ATH_MSG_INFO("Setup the MuonMomentum calibration tool for 2017 & mc16c/d");
            m_calibTool_17.setTypeAndName("CP::MuonCalibrationAndSmearingTool/"+name()+"_17");
            ATH_CHECK(m_calibTool_17.setProperty("Year", "Data17"));
            ATH_CHECK(m_calibTool_17.setProperty("Release", m_release));
            ATH_CHECK(m_calibTool_17.setProperty("SagittaRelease", m_sagittaRelease17));
            ATH_CHECK(m_calibTool_17.setProperty("StatComb", statComb17));
            ATH_CHECK(m_calibTool_17.setProperty("SagittaCorr", sagittaCorr17));
            ATH_CHECK(m_calibTool_17.setProperty("doSagittaMCDistortion", sagittaMCDistortion17));
            ATH_CHECK(m_calibTool_17.setProperty("SagittaCorrPhaseSpace", sagittaCorrPhaseSpace17));
        }
        if (!m_calibTool_18.isUserConfigured()){
            ATH_MSG_INFO("Setup the MuonMomentum calibration tool for 2018 & mc16e");
            m_calibTool_18.setTypeAndName("CP::MuonCalibrationAndSmearingTool/"+name()+"_18");
            ATH_CHECK(m_calibTool_18.setProperty("Year", "Data18"));
            ATH_CHECK(m_calibTool_18.setProperty("Release", m_release));
            ATH_CHECK(m_calibTool_18.setProperty("SagittaRelease", m_sagittaRelease18));
            ATH_CHECK(m_calibTool_18.setProperty("StatComb", statComb18));
            ATH_CHECK(m_calibTool_18.setProperty("SagittaCorr", sagittaCorr18));
            ATH_CHECK(m_calibTool_18.setProperty("doSagittaMCDistortion", sagittaMCDistortion18));
            ATH_CHECK(m_calibTool_18.setProperty("SagittaCorrPhaseSpace", sagittaCorrPhaseSpace18));
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
        m_evInfo(nullptr),
        m_sagittaRelease1516("sagittaBiasDataAll_03_02_19_Data16"),
        m_sagittaRelease17("sagittaBiasDataAll_03_02_19_Data17"),
        m_sagittaRelease18("sagittaBiasDataAll_03_02_19_Data18"),
        m_release("Recs2018_05_20"),
        m_calib_mode(CalibMode::additionalMCsys),
        m_StatComb1516(false),
        m_SagittaCorr1516(true),
        m_SagittaMCDistortion1516(false),
        m_SagittaCorrPhaseSpace1516(true),
        
        m_StatComb17(false),
        m_SagittaCorr17(true),
        m_SagittaMCDistortion17(false),
        m_SagittaCorrPhaseSpace17(true),        
        
        m_StatComb18(false),
        m_SagittaCorr18(false),
        m_SagittaMCDistortion18(true),
        m_SagittaCorrPhaseSpace18(true),
        
        m_MCperiods1516(),
        m_MCperiods17(),
        m_MCperiods18(),
        m_useRndRun(true) {

        m_calibTool_1516.declarePropertyFor(this, "CalibrationTool1516", "Parse the configured instance of the calibration tool for 15&16 data or mc16a");
        m_calibTool_17.declarePropertyFor(this, "CalibrationTool17", "Parse the configured instance of the calibration tool for 17 data or mc16d");
        m_calibTool_18.declarePropertyFor(this, "CalibrationTool18", "Parse the configured instance of the calibration tool for 18 data or mc16e");
        declareProperty("calibrationMode", m_calib_mode);
        
        // Properties to configure the subtools -- expert only
        declareProperty("SagittaRelease1516", m_sagittaRelease1516);
        declareProperty("SagittaRelease17", m_sagittaRelease17);
        declareProperty("SagittaRelease18", m_sagittaRelease18);
        declareProperty("Release", m_release);
        
        declareProperty("StatComb1516", m_StatComb1516);
        declareProperty("SagittaCorr1516", m_SagittaCorr1516);
        declareProperty("doSagittaMCDistortion1516", m_SagittaMCDistortion1516);
        declareProperty("SagittaCorrPhaseSpace1516", m_SagittaCorrPhaseSpace1516);
        
        declareProperty("StatComb17", m_StatComb17);
        declareProperty("SagittaCorr17", m_SagittaCorr17);
        declareProperty("doSagittaMCDistortion17", m_SagittaMCDistortion17);
        declareProperty("SagittaCorrPhaseSpace17", m_SagittaCorrPhaseSpace17);
        
        declareProperty("StatComb18", m_StatComb18);
        declareProperty("SagittaCorr18", m_SagittaCorr18);
        declareProperty("doSagittaMCDistortion18", m_SagittaMCDistortion18);
        declareProperty("SagittaCorrPhaseSpace18", m_SagittaCorrPhaseSpace18);
        
        declareProperty("MCperiods1516", m_MCperiods1516 = {284500});
        declareProperty("MCperiods17", m_MCperiods17 = {300000, 304000, 305000});
        declareProperty("MCperiods18", m_MCperiods18 = {310000});
        
        declareProperty("useRandomRunNumber", m_useRndRun);
        // run 304000/305000 correspond to 13/7 TeV low-mu data in 2017
    }
    MuonCalibrationPeriodTool::~MuonCalibrationPeriodTool(){}

    CorrectionCode MuonCalibrationPeriodTool::applyCorrection( xAOD::Muon& mu ) const {
        const CP::IMuonCalibrationAndSmearingTool* tool = getTool();
        if (tool == nullptr){
            ATH_MSG_FATAL("No valid tool has been found to apply the correction of muon with pt: "<<mu.pt()/1.e3<<"  eta: "<<mu.eta()<<" phi: "<<mu.phi());
            return CorrectionCode::Error;
        }
        return tool->applyCorrection(mu);
    }
    CorrectionCode MuonCalibrationPeriodTool::correctedCopy( const xAOD::Muon& input, xAOD::Muon*& output ) const {
        const CP::IMuonCalibrationAndSmearingTool* tool = getTool();
        if (tool == nullptr){
            ATH_MSG_FATAL("No valid tool has been found to create the corrected copy of muon with pt: "<<input.pt()/1.e3<<"  eta: "<<input.eta()<<" phi: "<<input.phi());
            return CorrectionCode::Error;
        }
        return tool->correctedCopy(input, output);
    
    }
    CorrectionCode MuonCalibrationPeriodTool::applyCorrectionTrkOnly( xAOD::TrackParticle& inTrk, const int DetType) const {
        const CP::IMuonCalibrationAndSmearingTool* tool = getTool();
        if (tool == nullptr){
            ATH_MSG_FATAL("No valid tool has been found to apply the correction of the track with pt: "<<inTrk.pt()/1.e3<<"  eta: "<<inTrk.eta()<<" phi: "<<inTrk.phi());
            return CorrectionCode::Error;
        }
        return tool->applyCorrectionTrkOnly(inTrk, DetType);        
    }    
    double MuonCalibrationPeriodTool::expectedResolution( const std::string& DetType, const xAOD::Muon& mu, const bool mc ) const {
        const CP::IMuonCalibrationAndSmearingTool* tool = getTool();
        if (tool == nullptr){
            ATH_MSG_FATAL("No valid tool has been found to predict the resolution of muon with pt: "<<mu.pt()/1.e3<<"  eta: "<<mu.eta()<<" phi: "<<mu.phi());
            throw std::runtime_error(Form("No valid tool has been found to predict the resolution of muon with pt: %.3f eta: %.2f phi: %.2f",mu.pt()/1.e3,mu.eta(), mu.phi()));
            return std::nan("float");
        }
        return tool->expectedResolution(DetType,mu,mc);
    }
    double MuonCalibrationPeriodTool::expectedResolution( const int& DetType, const xAOD::Muon& mu, const bool mc ) const {
        const CP::IMuonCalibrationAndSmearingTool* tool = getTool();
        if (tool == nullptr){
            ATH_MSG_FATAL("No valid tool has been found to predict the resolution of muon with pt: "<<mu.pt()/1.e3<<"  eta: "<<mu.eta()<<" phi: "<<mu.phi());
            throw std::runtime_error(Form("No valid tool has been found to predict the resolution of muon with pt: %.3f eta: %.2f phi: %.2f",mu.pt()/1.e3,mu.eta(), mu.phi()));
            return std::nan("float");
        }
        return tool->expectedResolution(DetType,mu,mc);
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
