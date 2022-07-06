/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Framework include(s):
#include "AsgDataHandles/ReadHandle.h"
#include "PathResolver/PathResolver.h"
#include "FourMomUtils/xAODP4Helpers.h"
// Local include(s):
#include <cmath>
#include "TRandom3.h"

#include "MuonMomentumCorrections/MuonCalibIntSagittaTool.h"
#include "MuonMomentumCorrections/EnumDef.h"
#include <MuonMomentumCorrections/CalibInitializer.h>

namespace CP
{

    MuonCalibIntSagittaTool::MuonCalibIntSagittaTool(const std::string &name) : asg::AsgTool(name)
    {    }

    StatusCode MuonCalibIntSagittaTool::initialize()
    {
        // Greet the user:
        ATH_MSG_INFO("Initializing MuonCalibIntSagittaTool");

        // Get the m_eventinfo container
        ATH_CHECK(m_eventInfo.initialize());

        // Read the recommendations
        for(const auto& year: MCP::dataYearList)
        {
            m_sagittaCorrConst_ID[year] = MCP::CalibInitializer::createSagittaCorrMap(year, MCP::TrackType::ID, m_release, "data");
            m_sagittaCorrConst_ME[year] = MCP::CalibInitializer::createSagittaCorrMap(year, MCP::TrackType::ME, m_release, "data");
            m_sagittaCorrConst_CB[year] = MCP::CalibInitializer::createSagittaCorrMap(year, MCP::TrackType::CB, m_release, "data");

            // Corrections from the MC - what is typically referred to as the kinematic term
            m_sagittaCorrConst_mcID[year] = MCP::CalibInitializer::createSagittaCorrMap(year, MCP::TrackType::ID, m_release, "mc");
            m_sagittaCorrConst_mcME[year] = MCP::CalibInitializer::createSagittaCorrMap(year, MCP::TrackType::ME, m_release, "mc");
            m_sagittaCorrConst_mcCB[year] = MCP::CalibInitializer::createSagittaCorrMap(year, MCP::TrackType::CB, m_release, "mc");
        }

        m_currentParameters = nullptr;
        // Init the systematics
        m_Parameters.initialize(affectingSystematics(), [this](const SystematicSet &systConfig, ParameterSetSagitta &param)
                                { return calcSystematicVariation(systConfig, param); });
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

    CorrectionCode MuonCalibIntSagittaTool::applyCorrection(MCP::MuonObj& mu) const
    {
        auto IDCorrections = getCorrections(mu.ID);
        auto MECorrections = getCorrections(mu.ME);
        auto CBCorrections = getCorrections(mu.CB);


        // Directly correct all three tracks
        double corrIDpT = mu.ID.calib_pt;
        applySagittaCorrection(corrIDpT, IDCorrections, mu.ID.charge, mu.ID.isData);

        double corrMEpT = mu.ME.calib_pt;
        applySagittaCorrection(corrMEpT, MECorrections, mu.ME.charge, mu.ME.isData);

        double corrDirectCBpT = mu.CB.calib_pt;
        applySagittaCorrection(corrDirectCBpT, CBCorrections, mu.CB.charge, mu.CB.isData);


        // Perform the statistical combination of ID + ME, before and after correction. Apply that ratio as a correction to create the weighted rho
        double corrStatCombCBpT = statCombCorrection(mu, corrIDpT, corrMEpT, mu.CB.calib_pt);

        // Calculation of rho 
        // The central value is average value and width of muons coming Z
        // The idea of this rho calculationsi that we want the muons at 45 GeV to get the CB correction
        // And for it to slowly taper to ID+MS at the low and high pT specturm
        // All this if rho is dynamically set and not fixed
        double central(45.2), width(15.5);
        double rho = 0;

        // If we are doing the systematic, shift the central values by the expected resolution of the muon
        if (m_currentParameters->SagittaRho != MCP::SystVariation::Default) 
        {
            double sigmaID = mu.expectedResID;
            double sigmaME = mu.expectedResME;
            double denominator = mu.CB.calib_pt * std::sqrt(sigmaID * sigmaID + sigmaME * sigmaME);
            double res = denominator ? M_SQRT2 * sigmaID * sigmaME / denominator : 0.;

            if (m_currentParameters->SagittaRho == MCP::SystVariation::Up) 
            {
                central += std::abs(0.5 * res * central);
            } else if (m_currentParameters->SagittaRho == MCP::SystVariation::Down) 
            {
                central -= std::abs(0.5 * res * central);
            }
        }

        // Calculate the rho
        if (!m_useFixedRho)
        {
            double sigmas = std::max(1.,(std::abs(mu.CB.calib_pt - central) / width));
            rho = 1. / sigmas;
           
        }
        else rho = m_fixedRhoValue;

        // Caculated the corrected pT
        double corrCBpT = rho * corrDirectCBpT + (1 - rho) * corrStatCombCBpT;

        ATH_MSG_DEBUG("Saggita correction - corrDirectCBpT: " << corrDirectCBpT << " corrStatCombCBpT: " << corrStatCombCBpT << " rho " << rho<<" corrCBPt: "<<corrCBpT);


        // Write the pT into the object
        mu.ID.calib_pt = corrIDpT;
        mu.ME.calib_pt = corrMEpT;
        mu.CB.calib_pt = corrCBpT;



        // Return gracefully:
        return CorrectionCode::Ok;
    }

    // This function applies the vector of corrections iteratively to the pT
    void MuonCalibIntSagittaTool::applySagittaCorrection(double& pt, const std::vector<double>& correction, const int& charge, const bool& isData) const
    {
        for(const auto& corr: correction)
        {
            double originalPt = pt;
            if(isData) pt = pt / (1 + charge * corr * pt);
            else pt = pt / (1 - charge * corr * pt);
            ATH_MSG_DEBUG("CorrectForCharge - in pT: " << originalPt << " corrPt: " << pt << " applied saggita: " << corr);

        }
    }


    // Get the set of correction, based on data + systematic setting
    std::vector<double> MuonCalibIntSagittaTool::getCorrections(const MCP::TrackCalibObj& trk) const
    {        
        double eta = trk.eta;
        double pT = trk.calib_pt;
        SagittaCorrConstMap const *corrMap{nullptr};
        SagittaCorrConstMap const *kinematicTermMap{nullptr};
        if(trk.type == MCP::TrackType::CB)
        {
            corrMap          = &m_sagittaCorrConst_CB.at(trk.year);
            kinematicTermMap = &m_sagittaCorrConst_mcCB.at(trk.year);
        } 
        else if(trk.type == MCP::TrackType::ID)
        { 
            corrMap          = &m_sagittaCorrConst_ID.at(trk.year);
            kinematicTermMap = &m_sagittaCorrConst_mcID.at(trk.year);
        }
        else if(trk.type == MCP::TrackType::ME)
        { 
            corrMap          = &m_sagittaCorrConst_ME.at(trk.year);
            kinematicTermMap = &m_sagittaCorrConst_mcME.at(trk.year);
        }
        
        std::vector<double> corrections;
        if(trk.isData)
        {
            if(!m_applyCorrectionOnData) return corrections;
            else if (m_applyCorrectionOnData)
            {
                // Apply data - kinematic term for the nominal
                double corr = corrMap->at(MCP::SagittaCorrection::Nominal)->getCalibConstant(trk);
                corr -= kinematicTermMap->at(MCP::SagittaCorrection::Nominal)->getCalibConstant(trk);
                corrections.push_back(corr);
            }
        }
        else // if it is MC
        {
            // Otherwise do as needed
            if(m_currentParameters->SagittaBias != MCP::SystVariation::Default)
            {
                int scale = (m_currentParameters->SagittaBias ==  MCP::SystVariation::Up) ? 1: -1;
                // Sanity check
                if((m_currentParameters->SagittaEtaSlice != MCP::SystVariation::Default) && !m_applyCorrectionOnData)
                {
                    ATH_MSG_ERROR("Sagitta correction is not applied to data, yet Eta dependant systematics are requested. This configuration is not supported");
                    return corrections;
                }

                // if no apply correction on data, apply 100% of (Data - kinematic term)
                // TODO: Check if it 50% or 100%? 
                if(!m_applyCorrectionOnData) 
                {
                    // Apply data - kinematic term for the nominal
                    double corr =  corrMap->at(MCP::SagittaCorrection::Nominal)->getCalibConstant(trk);
                    ATH_MSG_VERBOSE("Raw corr: "<<corr);
                    // corr -= kinematicTermMap->at(MCP::SagittaCorrection::Nominal)->getCalibConstant(eta, phi);
                    // ATH_MSG_VERBOSE("Raw - Kinematic corr: "<<corr);
                    corr *= (scale);
                    ATH_MSG_VERBOSE("corr: "<<corr);
                    corrections.push_back(corr);
                }

                // if data/MC apply 50% of resbiasMap with the extra res bias added
                double corr =  corrMap->at(MCP::SagittaCorrection::Residual__1up)->getCalibConstant(trk);
                corr *= (0.5 * scale);
                ATH_MSG_VERBOSE("Residual corr: "<<corr);

                // Extra scaling to cover for non-closure in the forward and transition region
                // As seen in the Rel 21 validation of the sagitta correction
                // It is only concetrateed in a few high eta bin. Idea is to apply a linearly increasing sys
                // Till it reach 450 GeV and then to flatten it. 
                // The value is choosen in an arbitrary fashion. To be replaced and fixed, once we have a better idea of 
                double deltas = m_extraRebiasSys;
                if (deltas > 0) {
                    if (eta > 2 || (eta > -2 && eta < -1.05)) {
                        if (pT > 450.0)
                            deltas += std::abs(450.0 - 45) / 100 * deltas;  // Above 450 GeV flat
                        else
                            deltas += std::abs(pT - 45) / 100 * deltas;
                    }
                    if (eta < -2 || (eta < 2 && eta> 1.5)) {
                        if (pT > 450.0)
                            deltas += std::abs(450.0 - 45) / 200 * deltas;  // Above 450 GeV flat
                        else
                            deltas += std::abs(pT - 45) / 200 * deltas;
                    }
                }
                if (m_currentParameters->SagittaBias == MCP::SystVariation::Up) {
                    corr += deltas;
                } else if (m_currentParameters->SagittaBias == MCP::SystVariation::Down) {
                    corr -= deltas;
                }
                ATH_MSG_VERBOSE("Deltas corr: "<<deltas);

                // If eta dependant, set the p2 to 0, if it not in the given eta slices
                if ((m_currentParameters->SagittaEtaSlice == MCP::SystVariation::Up) && eta < 0) corr = 0;
                if ((m_currentParameters->SagittaEtaSlice == MCP::SystVariation::Down) && eta > 0) corr = 0;

                corrections.push_back(corr);
                ATH_MSG_VERBOSE("final corr: "<<corr);

            }
            else if(m_currentParameters->SagittaDataStat != MCP::SystVariation::Default)
            {
                int scale = (m_currentParameters->SagittaDataStat ==  MCP::SystVariation::Up) ? 1: -1;
                // Apply 50% of the datastat map, up and down as stat error
                // TODO change to 100%
                // -1 is to match the convention
                double corr = corrMap->at(MCP::SagittaCorrection::Datastat__1up)->getCalibConstant(trk);
                corr *= (0.5 * scale);
                ATH_MSG_VERBOSE("data stat final corr: "<<corr);
                corrections.push_back(corr);
            }
            else if(m_currentParameters->SagittaRho != MCP::SystVariation::Default)
            {
                // For rho systematic, nothing happens right now
            }
            else
            {
                // if it is MC, then for nominal corrections - we don't apply anything
            }
        }

        return corrections;
    }


    double MuonCalibIntSagittaTool::statCombCorrection(const MCP::MuonObj& mu, double corrIDpT, double corrMEpT, double CBpT) const
    {

        // Corner cases to prevent a NaN in the combination
        if(mu.ID.calib_pt == 0) return CBpT;
        if(mu.ME.calib_pt == 0) return CBpT;
        if(corrIDpT == 0) return CBpT;
        if(corrMEpT == 0) return CBpT;
        
        double chi2Nom = -999;
        AmgVector(5)        parsCBNom   = mu.CB.pars;
        AmgVector(5)        parsID      = mu.ID.pars;
        AmgVector(5)        parsMS      = mu.ME.pars;
        AmgSymMatrix(5)     covCBNom    = mu.CB.covariance;
        AmgSymMatrix(5)     covID       = mu.ID.covariance;
        AmgSymMatrix(5)     covMS       = mu.ME.covariance;
           
        AmgVector(5)        parsCBCorr;
        AmgSymMatrix(5)     covCBCorr;

        // Do the statistical combination, with the original pTs
        // create the TLV with original ID pT
        using TLV = ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double>>;
        TLV tlv{mu.ID.calib_pt, mu.ID.eta, mu.ID.phi, mu.ID.mass};
        // Now modify the ID covariance matrix, and convert it to MeV
        parsID[4] = 1.0 / (tlv.P() * 1e3);

        tlv.SetCoordinates(mu.ME.calib_pt, mu.ME.eta, mu.ME.phi, mu.ME.mass);   
        // Now modify the ME covariance matrix
        parsMS[4] = 1.0 / (tlv.P() * 1e3);

        CorrectionCode SysCorrCode = applyStatCombination(parsID, covID, parsMS, covMS, mu.CB.charge, parsCBNom, covCBNom, chi2Nom);
        if (SysCorrCode != CorrectionCode::Ok) return CBpT;


        // Do the statistical combination, with the original pTs
        // create the TLV with original ID pT
        tlv.SetCoordinates(corrIDpT,  mu.ID.eta, mu.ID.phi, mu.ID.mass);
        // Now modify the ID covariance matrix, and convert it to MeV
        parsID[4] = 1.0 / (tlv.P() * 1e3);

        tlv.SetCoordinates(corrMEpT, mu.ME.eta, mu.ME.phi, mu.ME.mass);   
        // Now modify the ME covariance matrix
        parsMS[4] = 1.0 / (tlv.P() * 1e3);

        SysCorrCode = applyStatCombination(parsID, covID, parsMS, covMS, mu.CB.charge, parsCBCorr, covCBCorr, chi2Nom);
        if (SysCorrCode != CorrectionCode::Ok) return CBpT;


        double statCombPtNom  = std::sin(parsCBNom[3]) / std::abs(parsCBNom[4]);
        double statCombPtCorr = std::sin(parsCBCorr[3]) / std::abs(parsCBCorr[4]);
        double corrCBpT = CBpT * (statCombPtCorr / statCombPtNom);

        return corrCBpT;

    }
    CorrectionCode MuonCalibIntSagittaTool::applyStatCombination(AmgVector(5) parsID, const AmgSymMatrix(5)& covID, AmgVector(5) parsMS,
                                                                        const AmgSymMatrix(5)& covMS, int charge, AmgVector(5) & parsCB,
                                                                        AmgSymMatrix(5) & covCB, double& chi2) const {
        chi2 = 1e20;
        parsID[4] = std::abs(parsID[4]);
        parsMS[4] = std::abs(parsMS[4]);

        const AmgSymMatrix(5) weightID = covID.inverse();
        if (weightID.determinant() == 0) {
            ATH_MSG_WARNING(" ID weight matrix computation failed     ");
            return CorrectionCode::Error;
        }

        const AmgSymMatrix(5) weightMS = covMS.inverse();
        if (weightMS.determinant() == 0) {
            ATH_MSG_WARNING("weightMS computation failed      ");
            return CorrectionCode::Error;
        }

        AmgSymMatrix(5) weightCB = weightID + weightMS;
        covCB = weightCB.inverse();
        if (covCB.determinant() == 0) {
            ATH_MSG_WARNING(" Inversion of weightCB failed ");
            return CorrectionCode::Error;
        }

        AmgSymMatrix(5) covSum = covID + covMS;
        AmgSymMatrix(5) invCovSum = covSum.inverse();

        if (invCovSum.determinant() == 0) {
            ATH_MSG_WARNING(" Inversion of covSum failed ");
            return CorrectionCode::Error;
        }

        AmgVector(5) diffPars = parsID - parsMS;
        chi2 = diffPars.transpose() * invCovSum * diffPars;
        chi2 = chi2 / 5.;

        parsCB = covCB * (weightID * parsID + weightMS * parsMS);
        parsCB[4] *= charge;

        if (parsCB[2] > M_PI)
            parsCB[2] -= 2. * M_PI;
        else if (parsCB[2] < -M_PI)
            parsCB[2] += 2. * M_PI;
        return CorrectionCode::Ok;
    }



    bool MuonCalibIntSagittaTool::isAffectedBySystematic(const SystematicVariation &systematic) const
    {
        SystematicSet sys = affectingSystematics();
        return sys.find(systematic) != sys.end();
    }

    SystematicSet MuonCalibIntSagittaTool::affectingSystematics() const
    {
        SystematicSet result;
        //////////////////////
        /// Sagitta Systematis
        //////////////////////
        if (!m_useFixedRho || m_sysScheme == "AllSys") {
            result.insert(SystematicVariation("MUON_SAGITTA_RHO", 1));
            result.insert(SystematicVariation("MUON_SAGITTA_RHO", -1));
        }

        if (m_doEtaSagittaSys) {
            // Sagitta correction residual bias
            result.insert(SystematicVariation("MUON_SAGITTA_RESBIAS_NEGETA", 1));
            result.insert(SystematicVariation("MUON_SAGITTA_RESBIAS_NEGETA", -1));

            result.insert(SystematicVariation("MUON_SAGITTA_RESBIAS_POSETA", 1));
            result.insert(SystematicVariation("MUON_SAGITTA_RESBIAS_POSETA", -1));

        } 
        else {
            // Sagitta correction residual bias
            result.insert(SystematicVariation("MUON_SAGITTA_RESBIAS", 1));
            result.insert(SystematicVariation("MUON_SAGITTA_RESBIAS", -1));
        }

        
        result.insert(SystematicVariation("MUON_SAGITTA_DATASTAT", 1));
        result.insert(SystematicVariation("MUON_SAGITTA_DATASTAT", -1));
        

        return result;
    }

    SystematicSet MuonCalibIntSagittaTool::recommendedSystematics() const { return affectingSystematics(); }

    StatusCode MuonCalibIntSagittaTool::calcSystematicVariation(const SystematicSet &systConfig, ParameterSetSagitta &param) const
    {
        param.SagittaRho        = MCP::SystVariation::Default;
        param.SagittaBias       = MCP::SystVariation::Default;
        param.SagittaDataStat   = MCP::SystVariation::Default;
        param.SagittaEtaSlice   = MCP::SystVariation::Default;

        // Sagitta Rho systematics
        SystematicVariation syst = systConfig.getSystematicByBaseName("MUON_SAGITTA_RHO");

        if      (syst == SystematicVariation("MUON_SAGITTA_RHO",  1)) param.SagittaRho = MCP::SystVariation::Down;
        else if (syst == SystematicVariation("MUON_SAGITTA_RHO", -1)) param.SagittaRho = MCP::SystVariation::Up;
        else if (!syst.empty()) return StatusCode::FAILURE;

        // Sagitta Residual Bias systematics
        syst = systConfig.getSystematicByBaseName("MUON_SAGITTA_RESBIAS");

        if      (syst == SystematicVariation("MUON_SAGITTA_RESBIAS",  1)) param.SagittaBias = MCP::SystVariation::Down;
        else if (syst == SystematicVariation("MUON_SAGITTA_RESBIAS", -1)) param.SagittaBias = MCP::SystVariation::Up;
        else if (!syst.empty()) return StatusCode::FAILURE;

        // Sagitta Residual Bias systematics
        syst = systConfig.getSystematicByBaseName("MUON_SAGITTA_RESBIAS_POSETA");

        if (syst == SystematicVariation("MUON_SAGITTA_RESBIAS_POSETA", 1)) 
        {
            param.SagittaBias = MCP::SystVariation::Down;
            param.SagittaEtaSlice = MCP::SystVariation::Up;
        } 
        else if (syst == SystematicVariation("MUON_SAGITTA_RESBIAS_POSETA", -1)) 
        {
            param.SagittaBias = MCP::SystVariation::Up;
            param.SagittaEtaSlice = MCP::SystVariation::Up;
        } 
        else if (!syst.empty()) return StatusCode::FAILURE;

        // Sagitta Residual Bias systematics
        syst = systConfig.getSystematicByBaseName("MUON_SAGITTA_RESBIAS_NEGETA");

        if (syst == SystematicVariation("MUON_SAGITTA_RESBIAS_NEGETA", 1)) 
        {
            param.SagittaBias = MCP::SystVariation::Down;
            param.SagittaEtaSlice = MCP::SystVariation::Down;
        }
        else if (syst == SystematicVariation("MUON_SAGITTA_RESBIAS_NEGETA", -1)) 
        {
            param.SagittaBias = MCP::SystVariation::Up;
            param.SagittaEtaSlice = MCP::SystVariation::Down;
        } 
        else if (!syst.empty()) return StatusCode::FAILURE;

        // Sagitta Residual Bias systematics
        syst = systConfig.getSystematicByBaseName("MUON_SAGITTA_DATASTAT");

        if      (syst == SystematicVariation("MUON_SAGITTA_DATASTAT",  1)) param.SagittaDataStat = MCP::SystVariation::Up;
        else if (syst == SystematicVariation("MUON_SAGITTA_DATASTAT", -1)) param.SagittaDataStat = MCP::SystVariation::Down;
        else if (!syst.empty()) return StatusCode::FAILURE;

        //
        ATH_MSG_DEBUG("Systematic variation's parameters, SagittaRho: " << param.SagittaRho);
        ATH_MSG_DEBUG("Systematic variation's parameters, SagittaBias: " << param.SagittaBias);
        ATH_MSG_DEBUG("Systematic variation's parameters, SagittaDataStat: " << param.SagittaDataStat);
        ATH_MSG_DEBUG("Systematic variation's parameters, SagittaEtaSlice: " << param.SagittaEtaSlice);

        return StatusCode::SUCCESS;
    }

    StatusCode MuonCalibIntSagittaTool::applySystematicVariation(const SystematicSet &systConfig)
    {
        return m_Parameters.get(systConfig, m_currentParameters);
    }



} // namespace CP
