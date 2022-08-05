/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Framework include(s):
#include "AsgDataHandles/ReadHandle.h"
#include "PathResolver/PathResolver.h"

// Local include(s):
#include <cmath>
#include "TRandom3.h"

#include "MuonMomentumCorrections/MuonCalibTool.h"

namespace CP
{

    MuonCalibTool::MuonCalibTool(const std::string &name) : asg::AsgTool(name),
                                                            m_MuonSelectionTool("")
    {
    }

    StatusCode MuonCalibTool::initialize()
    {
        // Greet the user:
        ATH_MSG_INFO("Initialising...");

        // Get the m_eventinfo container
        ATH_CHECK(m_eventInfo.initialize());

        // Set the options
        if (m_calibMode == MuonCalibTool::correctData_CB) {
            ATH_MSG_INFO("Data will be corrected for sagitta bias with CB calibration");
            m_doDirectCBCalib = true;
            m_applyCorrectionOnData = true;

        } else if (m_calibMode == MuonCalibTool::correctData_IDMS) {
            ATH_MSG_INFO("Data will be corrected for sagitta bias with ID+MS calibration");
            m_doDirectCBCalib = false;
            m_applyCorrectionOnData = true;

        } else if (m_calibMode == MuonCalibTool::notCorrectData_IDMS) {
            ATH_MSG_INFO("Data will be untouched. Instead an additional systematic will be added with ID+MS calibration");
            m_doDirectCBCalib = false;
            m_applyCorrectionOnData = false;

        } 
        else if (m_calibMode == MuonCalibTool::userDefined) {
            ATH_MSG_INFO("Using options as provided by the user");
        } 
        else {
            ATH_MSG_FATAL("Invalid  calibration mode: " << m_calibMode << " Allowed modes are correctData_CB("
                                                        << MuonCalibTool::correctData_CB << ") correctData_IDMS ("
                                                        << MuonCalibTool::correctData_IDMS << ") or notCorrectData_IDMS ("
                                                        << MuonCalibTool::notCorrectData_IDMS << ")");
            return StatusCode::FAILURE;
        }

        // Get the muon selection tool
        m_MuonSelectionTool.setTypeAndName("CP::MuonSelectionTool/MCaST_Own_MST");
        ATH_CHECK(m_MuonSelectionTool.setProperty("MaxEta", 2.7));
        ATH_CHECK(m_MuonSelectionTool.setProperty("MuQuality", 1));
        ATH_CHECK(m_MuonSelectionTool.setProperty("TurnOffMomCorr", true));
        ATH_CHECK(m_MuonSelectionTool.setProperty("IsRun3Geo", m_isRun3.value()));
        // ATH_CHECK(m_MuonSelectionTool.setProperty("OutputLevel", msg().level()));
        ATH_CHECK(m_MuonSelectionTool.retrieve());


        // Create the Sagitta tool
        m_MuonIntSagittaTool.setTypeAndName("CP::MuonCalibIntSagittaTool/MCaST_Sagitta");
        ATH_CHECK(m_MuonIntSagittaTool.setProperty("release", m_release.value()));
        ATH_CHECK(m_MuonIntSagittaTool.setProperty("systematicScheme", m_sysScheme.value()));
        ATH_CHECK(m_MuonIntSagittaTool.setProperty("applyCorrectionOnData", m_applyCorrectionOnData.value()));
        ATH_CHECK(m_MuonIntSagittaTool.setProperty("doEtaSagittaSys", m_doEtaSagittaSys.value()));
        ATH_CHECK(m_MuonIntSagittaTool.setProperty("OutputLevel", msg().level()));
        ATH_CHECK(m_MuonIntSagittaTool.retrieve());

        // Create the scale smear tool
        m_MuonIntScaleSmearTool.setTypeAndName("CP::MuonCalibIntScaleSmearTool/MCaST_ScaleSmear");
        ATH_CHECK(m_MuonIntScaleSmearTool.setProperty("release", m_release.value()));
        ATH_CHECK(m_MuonIntScaleSmearTool.setProperty("systematicScheme", m_sysScheme.value()));
        ATH_CHECK(m_MuonIntScaleSmearTool.setProperty("doDirectCBCalib", m_doDirectCBCalib.value()));
        ATH_CHECK(m_MuonIntScaleSmearTool.setProperty("OutputLevel", msg().level()));
        ATH_CHECK(m_MuonIntScaleSmearTool.retrieve());

        // Create the high pT tool
        m_MuonIntHighTSmearTool.setTypeAndName("CP::MuonCalibIntHighpTSmearTool/MCaST_highPtScaleSmear");
        ATH_CHECK(m_MuonIntHighTSmearTool.setProperty("release", m_release.value()));
        ATH_CHECK(m_MuonIntHighTSmearTool.setProperty("OutputLevel", msg().level()));
        ATH_CHECK(m_MuonIntHighTSmearTool.retrieve());

        if (!applySystematicVariation(SystematicSet()))
        {
            ATH_MSG_ERROR("Unable to run with no systematic");
            return StatusCode::FAILURE;
        }
        SystematicRegistry &registry = SystematicRegistry::getInstance();
        if (registry.registerSystematics(*this) != StatusCode::SUCCESS)
        {
            ATH_MSG_ERROR("Unkown systematic list");
            return StatusCode::FAILURE;
        }
        // Return gracefully:
        return StatusCode::SUCCESS;
    }

    CorrectionCode MuonCalibTool::applyCorrection(xAOD::Muon &mu) const
    {
        ATH_MSG_VERBOSE("Muon Type = " << mu.muonType() << " ( 0: Combined, 1: StandAlone, 2: SegmentTagged, 3: CaloTagged, 4: SiliconAssociatedForwardMuon)");
        ATH_MSG_VERBOSE("Muon Author = " << mu.author());

        // Convert to the internal object
        MCP::MuonObj muonObj = convertToMuonObj(mu);
        static const SG::AuxElement::Decorator<float> dec_idPt("InnerDetectorPt");
        static const SG::AuxElement::Decorator<float> dec_mePt("MuonSpectrometerPt");
        static const SG::AuxElement::Accessor<float> acc_id_pt("InnerDetectorPt");
        static const SG::AuxElement::Accessor<float> acc_me_pt("MuonSpectrometerPt");
        
        ATH_MSG_VERBOSE("input ID pT "<<muonObj.ID.calib_pt);
        ATH_MSG_VERBOSE("input ME pT "<<muonObj.ME.calib_pt);
        ATH_MSG_VERBOSE("input CB pT "<<muonObj.CB.calib_pt);
        ATH_MSG_VERBOSE("input eta "<<muonObj.CB.eta);
        ATH_MSG_VERBOSE("input phi "<<muonObj.CB.phi);


        if (muonObj.CB.isData) 
        {
            ATH_MSG_VERBOSE("Doing data corrections");

            // Sagitta Correction  specifics
            if (m_applyCorrectionOnData) 
            {
                CorrectionCode sgCode = m_MuonIntSagittaTool->applyCorrection(muonObj);
                if (sgCode != CorrectionCode::Ok) return sgCode;
            }

            // SAF, and Calo specifics, reset to original pT
            if (!(mu.muonType() == xAOD::Muon::SiliconAssociatedForwardMuon) && !(mu.muonType() == xAOD::Muon::CaloTagged) && !(mu.muonType() == xAOD::Muon::SegmentTagged)) 
            {
                mu.setP4(muonObj.CB.calib_pt * GeVtoMeV, muonObj.CB.eta, muonObj.CB.phi);
            }

            dec_idPt(mu) = muonObj.ID.calib_pt * GeVtoMeV;
            dec_mePt(mu) = muonObj.ME.calib_pt * GeVtoMeV;

            return CorrectionCode::Ok;
        }

        // Do Scale and Smearing corrections
        CorrectionCode sgCode = m_MuonIntScaleSmearTool->applyCorrection(muonObj);
        if (sgCode != CorrectionCode::Ok) return sgCode;

        // Systematics for sagitta correction
        if ((mu.muonType() != xAOD::Muon::SiliconAssociatedForwardMuon)) 
        {
            ATH_MSG_VERBOSE("Systematic uncertainties for sagitta bias ");
            // TODO:: something specific for calo tags
            CorrectionCode sgCode = m_MuonIntSagittaTool->applyCorrection(muonObj);
            if (sgCode != CorrectionCode::Ok) return sgCode;
        }

        // Override combined momentum for special cases
        if (std::abs(muonObj.ME.calib_pt) == 0) muonObj.CB.calib_pt = muonObj.ID.calib_pt;
        if (std::abs(muonObj.ID.calib_pt) == 0) muonObj.CB.calib_pt = muonObj.ME.calib_pt;


        // Setting the output object properties right now, so the resolution category get the corrected info
        mu.setP4(muonObj.CB.calib_pt * GeVtoMeV, muonObj.CB.eta, muonObj.CB.phi);
        dec_idPt(mu) = muonObj.ID.calib_pt * GeVtoMeV;
        dec_mePt(mu) = muonObj.ME.calib_pt * GeVtoMeV;
        muonObj.raw_mst_category = (CP::IMuonSelectionTool::ResolutionCategory) m_MuonSelectionTool->getResolutionCategory(mu);

        // Special case: if the proper flags are selected (m_extra_highpt_smearing or m_2stations_highpt_smearing)
        // an ad-hoc smearing of the combined momentum has to be applied
        bool extra_smearing   = (m_extra_highpt_smearing && (muonObj.raw_mst_category >= 0) && !( muonObj.raw_mst_category & IMuonSelectionTool::CategoryFour));  // Extra smearing, if selected, gets anyway only applied to non-3-station muons!
        bool highpt_smearing = (m_2stations_highpt_smearing && ( muonObj.raw_mst_category & IMuonSelectionTool::CategoryThree));  // Special highpt smearing, if selected, gets anyway only applied to missing-inner, 2-station muons only!

        if (((extra_smearing || highpt_smearing)) && (mu.pt() > m_HighPtSystThreshold * GeVtoMeV))
        {
            sgCode = m_MuonIntHighTSmearTool->applyCorrection(muonObj);
            if (sgCode != CorrectionCode::Ok) return sgCode;
        }

        // Final info to be written to the muon object
        mu.setP4(muonObj.CB.calib_pt * GeVtoMeV, muonObj.CB.eta, muonObj.CB.phi);
        dec_idPt(mu) = muonObj.ID.calib_pt * GeVtoMeV;
        dec_mePt(mu) = muonObj.ME.calib_pt * GeVtoMeV;


        ATH_MSG_DEBUG("Checking Output Muon Info - Pt_ID: " << acc_id_pt(mu));
        ATH_MSG_DEBUG("Checking Output Muon Info - Pt_MS: " << acc_me_pt(mu));
        ATH_MSG_DEBUG("Checking Output Muon Info - Pt_CB: " << mu.pt());

        // If saggita was out of validity, return it here
        if (sgCode == CorrectionCode::OutOfValidityRange) return sgCode;
        // Return gracefully:


        // Return gracefully:
        return CorrectionCode::Ok;
    }

    CorrectionCode MuonCalibTool::applyCorrectionTrkOnly(xAOD::TrackParticle &inTrk, const int DetType) const
    {
                // Convert to the internal object
        MCP::MuonObj muonObj = convertToMuonObj(inTrk, DetType);

        // Do Scale and Smearing corrections
        CorrectionCode sgCode = m_MuonIntScaleSmearTool->applyCorrection(muonObj);
        if (sgCode != CorrectionCode::Ok) return sgCode;

        double res_pt = muonObj.ID.calib_pt;
        if(DetType == MCP::DetectorType::MS) res_pt = muonObj.ME.calib_pt;

        inTrk.setDefiningParameters(inTrk.d0(), inTrk.z0(), inTrk.phi0(), inTrk.theta(),
                            inTrk.charge() / (res_pt * std::cosh(inTrk.eta())));

        // Return gracefully:
        return CorrectionCode::Ok;
    }

    CorrectionCode MuonCalibTool::correctedCopy(const xAOD::Muon &input, xAOD::Muon *&output) const
    {
        // A sanity check:
        if (output)
            ATH_MSG_WARNING(
                "Non-null pointer received. "
                "There's a possible memory leak!");

        // Create a new object:
        ATH_MSG_VERBOSE("Going to create new xAOD::Muon...");
        output = new xAOD::Muon();
        ATH_MSG_VERBOSE("Calling makePrivateStore...");
        output->makePrivateStore(input);

        // Use the other function to modify this object:
        ATH_MSG_VERBOSE("Calling applyCorrection...");

        CP::CorrectionCode retCode = applyCorrection(*output);

        return retCode;
    }

    bool MuonCalibTool::isAffectedBySystematic(const SystematicVariation &systematic) const
    {
        SystematicSet sys = affectingSystematics();
        return sys.find(systematic) != sys.end();
    }

    SystematicSet MuonCalibTool::affectingSystematics() const
    {
        SystematicSet result = m_MuonIntSagittaTool->affectingSystematics();
        result.insert(m_MuonIntScaleSmearTool->affectingSystematics());
        result.insert(m_MuonIntHighTSmearTool->affectingSystematics());
     
        return result;
    }

    SystematicSet MuonCalibTool::recommendedSystematics() const { return affectingSystematics(); }

    StatusCode MuonCalibTool::applySystematicVariation(const SystematicSet &systConfig)
    {
        // Apply to the underlying tool
        StatusCode code = m_MuonIntSagittaTool->applySystematicVariation(systConfig);
        if(code != StatusCode::SUCCESS) return code;
        
        code = m_MuonIntScaleSmearTool->applySystematicVariation(systConfig);          
        if(code != StatusCode::SUCCESS) return code;

        code = m_MuonIntHighTSmearTool->applySystematicVariation(systConfig);          
        if(code != StatusCode::SUCCESS) return code;

        return code;
    }

    double MuonCalibTool::expectedResolution(const std::string &DetType, const xAOD::Muon &mu, const bool addMCCorrectionSmearing) const
    {
        // Expected resolution in data (or unsmeared MC if second argument is true)
        if (DetType == "MS") {
            return expectedResolution(MCP::DetectorType::MS, mu, addMCCorrectionSmearing);
        } else if (DetType == "ID") {
            return expectedResolution(MCP::DetectorType::ID, mu, addMCCorrectionSmearing);
        } else if (DetType == "CB") {
            return expectedResolution(MCP::DetectorType::CB, mu, addMCCorrectionSmearing);
        } else {
            ATH_MSG_ERROR("The DetType that you entered is not allows - DetType = " << DetType);
            return 0.;
        }
    }

    double MuonCalibTool::expectedResolution(const int &DetType, const xAOD::Muon &mu, const bool addMCCorrectionSmearing) const
    {
        // Get information about data
        bool isData = false;
        if (m_validationMode) isData = m_expertMode_isData;
        else 
        {
            // Retrieve the event information:
            SG::ReadHandle<xAOD::EventInfo> evtInfo(m_eventInfo);
            isData = !evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
        }

        // Get information about which year it is here
        MCP::DataYear dataYear = MuonCalibTool::getPeriod(isData);

        // get the pt measurements from the xAOD::Muon object
        double loc_ptid = 0;
        double loc_ptms = 0;
        double loc_ptcb = 0;

        double Primary_eta = mu.eta();
        double Primary_phi = mu.phi();

        if (m_validationMode) 
        {
            static const SG::AuxElement::Accessor<float> id_pt("expert_ptid");
            static const SG::AuxElement::Accessor<float> ms_pt("expert_ptms");
            static const SG::AuxElement::Accessor<float> cb_pt("expert_ptcb");

            loc_ptid = id_pt(mu) * MeVtoGeV;
            loc_ptms = ms_pt(mu) * MeVtoGeV;
            loc_ptcb = cb_pt(mu) * MeVtoGeV;

        } else {

            // Retrieve all the trans
            const xAOD::TrackParticle* CB_track = mu.trackParticle(xAOD::Muon::CombinedTrackParticle);
            const xAOD::TrackParticle* ID_track = mu.trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
            const xAOD::TrackParticle* ME_track = mu.trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);

            if(CB_track) loc_ptcb = CB_track->pt() * MeVtoGeV;
            if(ID_track) loc_ptid = ID_track->pt() * MeVtoGeV;
            if(ME_track) loc_ptms = ME_track->pt() * MeVtoGeV;
        }

        if (DetType == MCP::DetectorType::MS) 
        {
            return m_MuonIntScaleSmearTool->getExpectedResolution(DetType, loc_ptms, Primary_eta, Primary_phi, dataYear, addMCCorrectionSmearing);
        } 
        else if (DetType == MCP::DetectorType::ID) 
        {
            return m_MuonIntScaleSmearTool->getExpectedResolution(DetType, loc_ptid, Primary_eta, Primary_phi, dataYear, addMCCorrectionSmearing);
        } 
        else if (DetType == MCP::DetectorType::CB) 
        {
            // Due to complicated maths, the expected combined resolution
            // is given by this equation (note: all sigmas are fractional uncertainties):
            // sigma_CB = std::sqrt(2) * sigma_ID * sigma_MS * pTMS * pTID / {pTCB * std::sqrt({sigma_ID*pTID}^2 + {sigma_MS*pTMS}^2)}
            // Do a little recursive calling to make things easier to read
            // Turn these into *absolute* uncertainties to make life easier
            double sigmaID = expectedResolution(MCP::DetectorType::ID, mu, addMCCorrectionSmearing) * loc_ptid;
            double sigmaMS = expectedResolution(MCP::DetectorType::MS, mu, addMCCorrectionSmearing) * loc_ptms;
            double denominator = (loc_ptcb)*std::sqrt(sigmaID * sigmaID + sigmaMS * sigmaMS);
            return denominator ? sigmaID * sigmaMS / denominator : 0.;
        } 
        else
         {
            ATH_MSG_ERROR("wrong DetType in input " << DetType);
        }
        return 0.;

    }

    // Internal tool function
    MCP::MuonObj MuonCalibTool::convertToMuonObj(const xAOD::Muon &mu) const
    {

        // Get information about data
        bool isData = false;
        if (m_validationMode) isData = m_expertMode_isData;
        else 
        {
            // Retrieve the event information:
            SG::ReadHandle<xAOD::EventInfo> evtInfo(m_eventInfo);
            isData = !evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
        }
        ATH_MSG_VERBOSE("Checking Simulation flag: " << !isData);

        // Get information about which year it is here
        auto year = MuonCalibTool::getPeriod(isData);

       
        double Primary_eta = mu.eta();
        double Primary_phi = mu.phi();
        int charge = mu.charge();
        double mass = mu.m() * MeVtoGeV;

        if (m_validationMode)
        {
            static const SG::AuxElement::Accessor<float> cb_pt("expert_ptcb");
            static const SG::AuxElement::Accessor<float> id_pt("expert_ptid");
            static const SG::AuxElement::Accessor<float> ms_pt("expert_ptms");
            static const SG::AuxElement::Accessor<AmgVector(5)> CBParam("CBParam");
            static const SG::AuxElement::Accessor<AmgSymMatrix(5)> CBCov("CBCov");
            static const SG::AuxElement::Accessor<AmgVector(5)>  IDParam("IDParam");
            static const SG::AuxElement::Accessor<AmgSymMatrix(5)>  IDCov("IDCov");
            static const SG::AuxElement::Accessor<AmgVector(5)> MEParam("MEParam");
            static const SG::AuxElement::Accessor<AmgSymMatrix(5)> MECov("MECov");

            // Use the constructor where the eta/phi are overwritten to keep it inma line with current recommendations. To be changed in the future
            auto CB = MCP::TrackCalibObj(MCP::TrackType::CB, charge, cb_pt(mu), Primary_eta, Primary_phi, mass, CBParam(mu), CBCov(mu), year, isData);
            auto ID = MCP::TrackCalibObj(MCP::TrackType::ID, charge, id_pt(mu), Primary_eta, Primary_phi, mass, IDParam(mu), IDCov(mu), year, isData);
            auto ME = MCP::TrackCalibObj(MCP::TrackType::ME, charge, ms_pt(mu), Primary_eta, Primary_phi, mass, MEParam(mu), MECov(mu), year, isData);

            MCP::MuonObj muonObj{CB,ID,ME};
            initializeRandNumbers(muonObj);

            muonObj.expectedPercentResID = expectedResolution(MCP::DetectorType::ID, mu, false);
            muonObj.expectedPercentResME = expectedResolution(MCP::DetectorType::MS, mu, false);
            muonObj.expectedResID = expectedResolution(MCP::DetectorType::ID, mu, true) * muonObj.CB.calib_pt;
            muonObj.expectedResME = expectedResolution(MCP::DetectorType::MS, mu, true) * muonObj.CB.calib_pt;

            return muonObj;
        }
        

            // Retrieve all the trans
            const xAOD::TrackParticle *CB_track = mu.trackParticle(xAOD::Muon::CombinedTrackParticle);
            const xAOD::TrackParticle *ID_track = mu.trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
            const xAOD::TrackParticle *ME_track = mu.trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);

            // For SI muons, overwrite the charge from the CB track
            if (mu.muonType() == xAOD::Muon::SiliconAssociatedForwardMuon)
            {
                if (CB_track) charge = CB_track->charge();
            }

            // Use the constructor where the eta/phi are overwritten to keep it inma line with current recommendations. To be changed in the future
            auto CB = MCP::TrackCalibObj(CB_track, MCP::TrackType::CB, charge, Primary_eta, Primary_phi, year, isData);
            auto ID = MCP::TrackCalibObj(ID_track, MCP::TrackType::ID, charge, Primary_eta, Primary_phi, year, isData);
            auto ME = MCP::TrackCalibObj(ME_track, MCP::TrackType::ME, charge, Primary_eta, Primary_phi, year, isData);

            MCP::MuonObj muonObj{CB,ID,ME};
            initializeRandNumbers(muonObj);
            muonObj.expectedPercentResID = expectedResolution(MCP::DetectorType::ID, mu, false);
            muonObj.expectedPercentResME = expectedResolution(MCP::DetectorType::MS, mu, false);

            muonObj.expectedResID = expectedResolution(MCP::DetectorType::ID, mu, true) * muonObj.CB.calib_pt;
            muonObj.expectedResME = expectedResolution(MCP::DetectorType::MS, mu, true) * muonObj.CB.calib_pt;
            return muonObj;
    }
    MCP::MuonObj MuonCalibTool::convertToMuonObj(const xAOD::TrackParticle& inTrk, const int DetType) const
    {
        // Get information about data
        bool isData = false;

        // Retrieve the event information:
        SG::ReadHandle<xAOD::EventInfo> evtInfo(m_eventInfo);
        isData = !evtInfo->eventType(xAOD::EventInfo::IS_SIMULATION);
        
        ATH_MSG_VERBOSE("Checking Simulation flag: " << !isData);

        // Get information about which year it is here
        auto year = MuonCalibTool::getPeriod(isData);
        int charge = inTrk.charge();
        if (DetType == MCP::DetectorType::ID) 
        {  
            auto CB = MCP::TrackCalibObj(&inTrk,   MCP::TrackType::CB, charge, year, isData);
            auto ID = MCP::TrackCalibObj(&inTrk,   MCP::TrackType::ID, charge, year, isData);
            auto ME = MCP::TrackCalibObj(nullptr,  MCP::TrackType::ME, charge, year, isData);

            MCP::MuonObj muonObj{CB,ID,ME};
            return muonObj;
        } 

        auto CB = MCP::TrackCalibObj(&inTrk,   MCP::TrackType::CB, charge, year, isData);
        auto ID = MCP::TrackCalibObj(nullptr,  MCP::TrackType::ID, charge, year, isData);
        auto ME = MCP::TrackCalibObj(&inTrk,   MCP::TrackType::ME, charge, year, isData);

        MCP::MuonObj muonObj{CB,ID,ME};
        return muonObj;
    }


    void MuonCalibTool::initializeRandNumbers(MCP::MuonObj& muonObj) const
    {
        // Random number generation for smearing
        TRandom3 loc_random3;
        // Get Event Number, Retrieve the event information:
        SG::ReadHandle<xAOD::EventInfo> evtInfo(m_eventInfo);
        const unsigned long long eventNumber = evtInfo->eventNumber();
        // Construct a seed for the random number generator:
        const UInt_t seed = 1 + std::abs(muonObj.CB.phi) * 1E6 + std::abs(muonObj.CB.eta) * 1E3 + eventNumber;
        loc_random3.SetSeed(seed);

        muonObj.rnd_g0 = loc_random3.Gaus(0, 1);
        muonObj.rnd_g1 = loc_random3.Gaus(0, 1);
        muonObj.rnd_g2 = loc_random3.Gaus(0, 1);
        muonObj.rnd_g3 = loc_random3.Gaus(0, 1);
        muonObj.rnd_g4 = loc_random3.Gaus(0, 1);
        muonObj.rnd_g_highPt = loc_random3.Gaus(0, 1);


    }

    MCP::DataYear MuonCalibTool::getPeriod(bool isData) const 
    {
        static const SG::AuxElement::Accessor<unsigned int> acc_rnd("RandomRunNumber");
        // I've copied the run number ranges from SUSYTools
        // https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/SUSYPhys/SUSYTools/Root/SUSYObjDef_xAOD.cxx#L2438
        static const unsigned int last_run_16 = 320000;
        static const unsigned int last_run_17 = 342000;
        static const unsigned int last_run_18 = 364485;
        static const unsigned int last_run_22 = 999999;

        static const std::vector<int> MCperiods1516{284500};
        static const std::vector<int> MCperiods17{300000, 304000, 305000};
        static const std::vector<int> MCperiods18{310000};
        static const std::vector<int> MCperiods22{330000, 410000};


        SG::ReadHandle<xAOD::EventInfo> evtInfo(m_eventInfo);
        unsigned int run = evtInfo->runNumber();
        // retrieve the random run number
        if (!isData && m_useRndRun) {
            if (acc_rnd.isAvailable(*evtInfo))
                run = acc_rnd(*evtInfo);
            else {
                ATH_MSG_ERROR(
                    "No random runnumber could be found although the tool is configured to assign the years based on it. Please make sure "
                    "to apply the prwTool before-hand or consider to set the property 'useRandomRunNumber' to false.");
                return MCP::DataYear::Data16;
            }
        }
        // Check the Monte carlo
        if (!isData && (!m_useRndRun || !acc_rnd.isAvailable(*evtInfo))) {
            if (std::find(MCperiods1516.begin(), MCperiods1516.end(), run) != MCperiods1516.end()) {
                ATH_MSG_DEBUG("The current run " << run << " corresponds to data mc20a / data15-16");
                return MCP::DataYear::Data16;
            } else if (std::find(MCperiods17.begin(), MCperiods17.end(), run) != MCperiods17.end()) {
                ATH_MSG_DEBUG("The current run " << run << " corresponds to data mc20d / data17");
                return MCP::DataYear::Data17;
            } else if (std::find(MCperiods18.begin(), MCperiods18.end(), run) != MCperiods18.end()) {
                ATH_MSG_DEBUG("The current run " << run << " corresponds to data mc20e / data18");
                return MCP::DataYear::Data18;
            } else if (std::find(MCperiods22.begin(), MCperiods22.end(), run) != MCperiods22.end()) {
                ATH_MSG_DEBUG("The current run " << run << " corresponds to data mc21a / data22");
                return MCP::DataYear::Data18;
            }

        }
        // Check data itself or the random run number is used
        else if (isData || m_useRndRun) {
            if (run < last_run_16) {
                ATH_MSG_DEBUG("The current run " << run << " is taken in data 15-16");
                return MCP::DataYear::Data16;
            } else if (run < last_run_17) {
                ATH_MSG_DEBUG("The current run " << run << " is taken in data 17");
                return MCP::DataYear::Data17;
            } else if (run < last_run_18) {
                ATH_MSG_DEBUG("The current run " << run << " is taken in data 18");
                return MCP::DataYear::Data18;
            }  else if (run < last_run_22) {
                ATH_MSG_DEBUG("The current run " << run << " is taken in data 22");
                return MCP::DataYear::Data22;
            }
        }
        ATH_MSG_FATAL("Could not assign run-number " << run << " to a specific year of data-taking");
        return MCP::DataYear::Data16;
    }


} // namespace CP
