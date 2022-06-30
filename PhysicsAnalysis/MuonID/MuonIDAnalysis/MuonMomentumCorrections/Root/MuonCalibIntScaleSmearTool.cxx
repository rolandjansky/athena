/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Framework include(s):
#include "AsgDataHandles/ReadHandle.h"
#include "PathResolver/PathResolver.h"

// Local include(s):
#include <cmath>
#include "TRandom3.h"

#include "MuonMomentumCorrections/MuonCalibIntScaleSmearTool.h"
#include "MuonMomentumCorrections/EnumDef.h"
#include <MuonMomentumCorrections/CalibInitializer.h>

namespace CP
{

    MuonCalibIntScaleSmearTool::MuonCalibIntScaleSmearTool(const std::string &name) : asg::AsgTool(name)
    {}

    StatusCode MuonCalibIntScaleSmearTool::initialize()
    {
        // Greet the user:
        ATH_MSG_INFO("Initializing MuonCalibIntScaleSmearTool");

        // Get the m_eventinfo container
        ATH_CHECK(m_eventInfo.initialize());

        
        // Load the constants
        for(const auto & year: MCP::dataYearList)
        {
            for(auto param: m_paramList)
            {
                m_IDparams[year][param] = MCP::CalibInitializer::createScaleResCorrMap(year, MCP::TrackType::ID, m_release, param);
                m_MEparams[year][param] = MCP::CalibInitializer::createScaleResCorrMap(year, MCP::TrackType::ME, m_release, param);
                m_CBparams[year][param] = MCP::CalibInitializer::createScaleResCorrMap(year, MCP::TrackType::CB, m_release, param);
            }

            m_IDExpectedResparams[year] = MCP::CalibInitializer::createExpectedResMap(year, MCP::TrackType::ID, m_release);
            m_MEExpectedResparams[year] = MCP::CalibInitializer::createExpectedResMap(year, MCP::TrackType::ME, m_release);
        }


        m_currentParameters = nullptr;
        // Init the systematics
        m_Parameters.initialize(affectingSystematics(), [this](const SystematicSet &systConfig, ParameterSetScaleSmear &param)
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

    CorrectionCode MuonCalibIntScaleSmearTool::applyCorrection(MCP::MuonObj& mu) const
    {
        // Do nothing, if it is data
        if(mu.CB.isData) return CorrectionCode::Ok;

        // Get the correction constants for each year
        auto IDcorrConstants = getConstants(mu.ID, m_IDparams.at(mu.ID.year), m_currentParameters->m_IDparams);
        auto MEcorrConstants = getConstants(mu.ME, m_MEparams.at(mu.ME.year), m_currentParameters->m_MEparams);
        auto CBcorrConstants = getConstants(mu.CB, m_CBparams.at(mu.CB.year), m_currentParameters->m_CBparams);
      
        double corrIDpT = getCorrectedPt(mu, mu.ID, IDcorrConstants);
        double corrMEpT = getCorrectedPt(mu, mu.ME, MEcorrConstants);
        double corrCBpT = getCorrectedPt(mu, mu.CB, CBcorrConstants);

        double corrCBpTWithIDME =  getCorrectedCBPtWithIDMSComb(mu, IDcorrConstants, MEcorrConstants);


        // Write the pT into the object
        mu.ID.calib_pt = corrIDpT;
        mu.ME.calib_pt = corrMEpT;
        if(m_doDirectCBCalib) mu.CB.calib_pt = corrCBpT;
        else mu.CB.calib_pt = corrCBpTWithIDME;

        // Return gracefully:
        return CorrectionCode::Ok;
    }

    std::map<MCP::ScaleSmearParam, double> MuonCalibIntScaleSmearTool::getConstants(const MCP::TrackCalibObj& trk, const std::map<MCP::ScaleSmearParam, ScaleSmearCorrConstMap>& constants, const std::map<MCP::ScaleSmearParam, double>& direction) const
    {

        std::map<MCP::ScaleSmearParam, double> calibConstants;

        // Extra the constants from container into a simple map
        for(const auto& param: m_paramList)
        {
            auto contantList = constants.at(param);

            double val = contantList.at(MCP::ScaleResCorrection::Nominal)->getCalibConstant(trk);
            if(direction.at(param) == MCP::SystVariation::Up)
            {
                val += contantList.at(MCP::ScaleResCorrection::SystErr__1up)->getCalibConstant(trk);
            }
            else if (direction.at(param) == MCP::SystVariation::Down)
            {
                val -= contantList.at(MCP::ScaleResCorrection::SystErr__1down)->getCalibConstant(trk);
            }

            // Remove unphysical smearing
            if(param == MCP::ScaleSmearParam::r0 || param == MCP::ScaleSmearParam::r1 || param == MCP::ScaleSmearParam::r2)
            {
                val = std::max(0.0, val);
            }

            calibConstants[param] = val;
        }
        
        return calibConstants;
    }


    double MuonCalibIntScaleSmearTool::getCorrectedPt(const MCP::MuonObj& mu, const MCP::TrackCalibObj& trk, const std::map<MCP::ScaleSmearParam, double>& calibConstant) const
    {
        // For debugging:: Todo add checking if in verbose mode
        ATH_MSG_VERBOSE("MuonType:  "<<MCP::toString(trk.type));
        for(const auto& var: calibConstant) ATH_MSG_VERBOSE("var:  "<<MCP::toString(var.first)<<" = "<<var.second);

        double pT = trk.calib_pt;

        // Calculate the denominator for the scale correction
        double smearCorr = getSmearCorr(mu, trk, calibConstant.at(MCP::ScaleSmearParam::r0), calibConstant.at(MCP::ScaleSmearParam::r1), calibConstant.at(MCP::ScaleSmearParam::r2));
        ATH_MSG_VERBOSE("smearCorr:  "<<smearCorr);

        // apply the full correction
        double corrpT = (pT + pT * calibConstant.at(MCP::ScaleSmearParam::s1) + calibConstant.at(MCP::ScaleSmearParam::s0))/(1 + smearCorr);

        return corrpT;
    }

    double MuonCalibIntScaleSmearTool::getSmearCorr(const MCP::MuonObj& mu, const MCP::TrackCalibObj& trk, double r0, double r1, double r2) const
    {
        double pT = trk.calib_pt;
        if(trk.type == MCP::TrackType::ID)
        {
            // ID applies a tan(theta) correction for r2 for high eta muons
            double additional_weight = 1.;
            if (std::abs(trk.eta) > 2) additional_weight = sinh(trk.eta);
            
            return r1 * mu.rnd_g3 + r2 * mu.rnd_g4 * pT * additional_weight;
        }
        return r0 * mu.rnd_g0 / pT + r1 * mu.rnd_g1 + r2 * mu.rnd_g2 * pT;
    }


    double MuonCalibIntScaleSmearTool::getCorrectedCBPtWithIDMSComb(const MCP::MuonObj& mu, const std::map<MCP::ScaleSmearParam, double>& calibIDConstant, const std::map<MCP::ScaleSmearParam, double>& calibMEConstant) const
    {
        // calculate the relative of ID and ME for the corrections
        double weightID = 0.5;
        double weightME = 0.5;

        double deltaCBME = mu.CB.calib_pt - mu.ME.calib_pt;
        double deltaCBID = mu.CB.calib_pt - mu.ID.calib_pt;
       
        if (mu.CB.calib_pt != 0) 
        {        
            if (std::abs(deltaCBME) > 0 || std::abs(deltaCBID) > 0) 
            {
                double R = 1, Rplus = 1;
                if (std::abs(deltaCBME) == std::abs(deltaCBID))
                {
                    // do nothing
                }
                else if (std::abs(deltaCBME) != 0 &&
                    std::abs(deltaCBME) > std::abs(deltaCBID)) {
                    R = (-deltaCBID) / deltaCBME; /* R~wMS/wID */
                    Rplus = 1 + R;
                    if (Rplus != 0 && R > 0) {
                        weightID = 1 / Rplus;
                        weightME = R / Rplus;
                    }
                }
                else if (std::abs(deltaCBID) != 0 &&
                    std::abs(deltaCBME) < std::abs(deltaCBID)) {
                    R = (-deltaCBME) / (deltaCBID); /* R~wID/wMS */
                    Rplus = 1 + R;
                    if (Rplus != 0 && R > 0) {
                        weightID = R / Rplus;
                        weightME = 1 / Rplus;
                    }
                }
            }
        }
        // If no correction was found
        if(weightID == 0.5 && weightME == 0.5)
        {
            double wME = mu.ME.calib_pt / mu.CB.calib_pt / std::pow(mu.expectedPercentResME, 2);
            double wID = mu.ID.calib_pt / mu.CB.calib_pt / std::pow(mu.expectedPercentResID, 2);
            weightID = wID / (wME + wID);
            weightME = wME / (wME + wID);
        }


        // Calculate the denominator for the scale correction
        double smearIDCorr = getSmearCorr(mu, mu.ID, calibIDConstant.at(MCP::ScaleSmearParam::r0), calibIDConstant.at(MCP::ScaleSmearParam::r1), calibIDConstant.at(MCP::ScaleSmearParam::r2));
        double smearMECorr = getSmearCorr(mu, mu.ME, calibMEConstant.at(MCP::ScaleSmearParam::r0), calibMEConstant.at(MCP::ScaleSmearParam::r1), calibMEConstant.at(MCP::ScaleSmearParam::r2));
        double smearCorr = weightID * smearIDCorr + weightME * smearMECorr;
        // apply the full correction
        double scaleCB = (calibIDConstant.at(MCP::ScaleSmearParam::s1) * weightID + (calibMEConstant.at(MCP::ScaleSmearParam::s1) + calibMEConstant.at(MCP::ScaleSmearParam::s0) / mu.ME.calib_pt) * weightME);

        ATH_MSG_VERBOSE("mu.ID.calib_pt: "<<mu.ID.calib_pt);
        ATH_MSG_VERBOSE("mu.ME.calib_pt: "<<mu.ME.calib_pt);
        ATH_MSG_VERBOSE("mu.CB.calib_pt: "<<mu.CB.calib_pt);
        ATH_MSG_VERBOSE("mu.expectedPercentResME: "<<mu.expectedPercentResME);
        ATH_MSG_VERBOSE("mu.expectedPercentResID: "<<mu.expectedPercentResID);

        ATH_MSG_VERBOSE("weightID: "<<weightID);
        ATH_MSG_VERBOSE("weightME: "<<weightME);
        ATH_MSG_VERBOSE("smearIDCorr: "<<smearIDCorr);
        ATH_MSG_VERBOSE("smearMECorr: "<<smearMECorr);
        ATH_MSG_VERBOSE("smearCorr: "<<smearCorr);
        ATH_MSG_VERBOSE("scaleCB: "<<scaleCB);

        double pT = mu.CB.calib_pt;
        double corrpT = (pT + pT * scaleCB)/(1 + smearCorr);

        return corrpT;
    }


    bool MuonCalibIntScaleSmearTool::isAffectedBySystematic(const SystematicVariation &systematic) const
    {
        SystematicSet sys = affectingSystematics();
        return sys.find(systematic) != sys.end();
    }

    SystematicSet MuonCalibIntScaleSmearTool::affectingSystematics() const
    {
        SystematicSet result;
        /////////////////////////
        // Resolution systematics
        /////////////////////////
        if (m_doDirectCBCalib || m_sysScheme == "AllSys") {
            // CB systematics
            result.insert(SystematicVariation("MUON_CB", 1));
            result.insert(SystematicVariation("MUON_CB", -1));
        }
        if (!m_doDirectCBCalib || m_sysScheme == "AllSys") {
            // ID systematics
            result.insert(SystematicVariation("MUON_ID", 1));
            result.insert(SystematicVariation("MUON_ID", -1));

            // MS systematics
            result.insert(SystematicVariation("MUON_MS", 1));
            result.insert(SystematicVariation("MUON_MS", -1));
        }

        /////////////////////
        /// Scale Systematics
        /////////////////////
        if (m_sysScheme == "Corr_Scale") {
            result.insert(SystematicVariation("MUON_SCALE", 1));
            result.insert(SystematicVariation("MUON_SCALE", -1));
        } 
        else if (m_sysScheme == "Decorr_Scale" || m_sysScheme == "AllSys") {
            // Either doing direct calib of CB or asking for all the sys
            if (m_doDirectCBCalib || m_sysScheme == "AllSys") {
                result.insert(SystematicVariation("MUON_SCALE_CB", 1));
                result.insert(SystematicVariation("MUON_SCALE_CB", -1));

                result.insert(SystematicVariation("MUON_SCALE_CB_ELOSS", 1));
                result.insert(SystematicVariation("MUON_SCALE_CB_ELOSS", -1));
            }

            // Either not doing direct calib of CB or asking for all the sys
            if (!m_doDirectCBCalib || m_sysScheme == "AllSys") {
                result.insert(SystematicVariation("MUON_SCALE_ID", 1));
                result.insert(SystematicVariation("MUON_SCALE_ID", -1));

                result.insert(SystematicVariation("MUON_SCALE_MS", 1));
                result.insert(SystematicVariation("MUON_SCALE_MS", -1));

                result.insert(SystematicVariation("MUON_SCALE_MS_ELOSS", 1));
                result.insert(SystematicVariation("MUON_SCALE_MS_ELOSS", -1));
            }
        }


        

        return result;
    }

    SystematicSet MuonCalibIntScaleSmearTool::recommendedSystematics() const { return affectingSystematics(); }

    StatusCode MuonCalibIntScaleSmearTool::calcSystematicVariation(const SystematicSet &systConfig, ParameterSetScaleSmear &param) const
    {
        param.m_IDparams[MCP::ScaleSmearParam::s0] = MCP::SystVariation::Default;
        param.m_IDparams[MCP::ScaleSmearParam::s1] = MCP::SystVariation::Default;
        param.m_IDparams[MCP::ScaleSmearParam::r0] = MCP::SystVariation::Default;
        param.m_IDparams[MCP::ScaleSmearParam::r1] = MCP::SystVariation::Default;
        param.m_IDparams[MCP::ScaleSmearParam::r2] = MCP::SystVariation::Default;

        param.m_MEparams[MCP::ScaleSmearParam::s0] = MCP::SystVariation::Default;
        param.m_MEparams[MCP::ScaleSmearParam::s1] = MCP::SystVariation::Default;
        param.m_MEparams[MCP::ScaleSmearParam::r0] = MCP::SystVariation::Default;
        param.m_MEparams[MCP::ScaleSmearParam::r1] = MCP::SystVariation::Default;
        param.m_MEparams[MCP::ScaleSmearParam::r2] = MCP::SystVariation::Default;

        param.m_CBparams[MCP::ScaleSmearParam::s0] = MCP::SystVariation::Default;
        param.m_CBparams[MCP::ScaleSmearParam::s1] = MCP::SystVariation::Default;
        param.m_CBparams[MCP::ScaleSmearParam::r0] = MCP::SystVariation::Default;
        param.m_CBparams[MCP::ScaleSmearParam::r1] = MCP::SystVariation::Default;
        param.m_CBparams[MCP::ScaleSmearParam::r2] = MCP::SystVariation::Default;


        // ID systematics
        SystematicVariation syst = systConfig.getSystematicByBaseName("MUON_ID");

        if (syst == SystematicVariation("MUON_ID", 1)) {
            param.m_IDparams[MCP::ScaleSmearParam::r0] = MCP::SystVariation::Up;
            param.m_IDparams[MCP::ScaleSmearParam::r1] = MCP::SystVariation::Up;
            param.m_IDparams[MCP::ScaleSmearParam::r2] = MCP::SystVariation::Up;
        } else if (syst == SystematicVariation("MUON_ID", -1)) {
            param.m_IDparams[MCP::ScaleSmearParam::r0] = MCP::SystVariation::Down;
            param.m_IDparams[MCP::ScaleSmearParam::r1] = MCP::SystVariation::Down;
            param.m_IDparams[MCP::ScaleSmearParam::r2] = MCP::SystVariation::Down;
        } else if (!syst.empty()) return StatusCode::FAILURE;

        // MS systematics
        syst = systConfig.getSystematicByBaseName("MUON_MS");

        if (syst == SystematicVariation("MUON_MS", 1)) {
            param.m_MEparams[MCP::ScaleSmearParam::r0] = MCP::SystVariation::Up;
            param.m_MEparams[MCP::ScaleSmearParam::r1] = MCP::SystVariation::Up;
            param.m_MEparams[MCP::ScaleSmearParam::r2] = MCP::SystVariation::Up;
        } else if (syst == SystematicVariation("MUON_MS", -1)) {
            param.m_MEparams[MCP::ScaleSmearParam::r0] = MCP::SystVariation::Down;
            param.m_MEparams[MCP::ScaleSmearParam::r1] = MCP::SystVariation::Down;
            param.m_MEparams[MCP::ScaleSmearParam::r2] = MCP::SystVariation::Down;
        } else if (!syst.empty()) return StatusCode::FAILURE;

        // CB systematics
        syst = systConfig.getSystematicByBaseName("MUON_CB");

        if (syst == SystematicVariation("MUON_CB", 1)) {
            param.m_CBparams[MCP::ScaleSmearParam::r0] = MCP::SystVariation::Up;
            param.m_CBparams[MCP::ScaleSmearParam::r1] = MCP::SystVariation::Up;
            param.m_CBparams[MCP::ScaleSmearParam::r2] = MCP::SystVariation::Up;
        } else if (syst == SystematicVariation("MUON_CB", -1)) {
            param.m_CBparams[MCP::ScaleSmearParam::r0] = MCP::SystVariation::Down;
            param.m_CBparams[MCP::ScaleSmearParam::r1] = MCP::SystVariation::Down;
            param.m_CBparams[MCP::ScaleSmearParam::r2] = MCP::SystVariation::Down;
        } else if (!syst.empty()) return StatusCode::FAILURE;

        // Scale systematics
        syst = systConfig.getSystematicByBaseName("MUON_SCALE");

        if (syst == SystematicVariation("MUON_SCALE", 1)) {
            param.m_IDparams[MCP::ScaleSmearParam::s0] = MCP::SystVariation::Down;
            param.m_IDparams[MCP::ScaleSmearParam::s1] = MCP::SystVariation::Down;

            param.m_MEparams[MCP::ScaleSmearParam::s0] = MCP::SystVariation::Down;
            param.m_MEparams[MCP::ScaleSmearParam::s1] = MCP::SystVariation::Down;

            param.m_CBparams[MCP::ScaleSmearParam::s0] = MCP::SystVariation::Down;
            param.m_CBparams[MCP::ScaleSmearParam::s1] = MCP::SystVariation::Down;

        } else if (syst == SystematicVariation("MUON_SCALE", -1)) {
            param.m_IDparams[MCP::ScaleSmearParam::s0] = MCP::SystVariation::Up;
            param.m_IDparams[MCP::ScaleSmearParam::s1] = MCP::SystVariation::Up;

            param.m_MEparams[MCP::ScaleSmearParam::s0] = MCP::SystVariation::Up;
            param.m_MEparams[MCP::ScaleSmearParam::s1] = MCP::SystVariation::Up;

            param.m_CBparams[MCP::ScaleSmearParam::s0] = MCP::SystVariation::Up;
            param.m_CBparams[MCP::ScaleSmearParam::s1] = MCP::SystVariation::Up;
        } else if (!syst.empty()) return StatusCode::FAILURE;

        // Split scale ID/MS/EGloss
        syst = systConfig.getSystematicByBaseName("MUON_SCALE_ID");

        if (syst == SystematicVariation("MUON_SCALE_ID", 1)) {
            param.m_IDparams[MCP::ScaleSmearParam::s0] = MCP::SystVariation::Down;
            param.m_IDparams[MCP::ScaleSmearParam::s1] = MCP::SystVariation::Down;
        } else if (syst == SystematicVariation("MUON_SCALE_ID", -1)) {
            param.m_IDparams[MCP::ScaleSmearParam::s0] = MCP::SystVariation::Up;
            param.m_IDparams[MCP::ScaleSmearParam::s1] = MCP::SystVariation::Up;
        } else if (!syst.empty()) return StatusCode::FAILURE;

        syst = systConfig.getSystematicByBaseName("MUON_SCALE_MS");

        if (syst == SystematicVariation("MUON_SCALE_MS", 1)) {
            param.m_MEparams[MCP::ScaleSmearParam::s1] = MCP::SystVariation::Down;
        } else if (syst == SystematicVariation("MUON_SCALE_MS", -1)) {
            param.m_MEparams[MCP::ScaleSmearParam::s1] = MCP::SystVariation::Up;
        } else if (!syst.empty()) return StatusCode::FAILURE;

        syst = systConfig.getSystematicByBaseName("MUON_SCALE_MS_ELOSS");

        if (syst == SystematicVariation("MUON_SCALE_MS_ELOSS", 1)) {
            param.m_MEparams[MCP::ScaleSmearParam::s0] = MCP::SystVariation::Down;
        } else if (syst == SystematicVariation("MUON_SCALE_MS_ELOSS", -1)) {
            param.m_MEparams[MCP::ScaleSmearParam::s0] = MCP::SystVariation::Up;
        } else if (!syst.empty()) return StatusCode::FAILURE;

        syst = systConfig.getSystematicByBaseName("MUON_SCALE_CB");

        if (syst == SystematicVariation("MUON_SCALE_CB", 1)) {
            param.m_CBparams[MCP::ScaleSmearParam::s1] = MCP::SystVariation::Down;
        } else if (syst == SystematicVariation("MUON_SCALE_CB", -1)) {
            param.m_CBparams[MCP::ScaleSmearParam::s1] = MCP::SystVariation::Up;
        } else if (!syst.empty()) return StatusCode::FAILURE;

        syst = systConfig.getSystematicByBaseName("MUON_SCALE_CB_ELOSS");

        if (syst == SystematicVariation("MUON_SCALE_CB_ELOSS", 1)) {
            param.m_CBparams[MCP::ScaleSmearParam::s0] = MCP::SystVariation::Down;
        } else if (syst == SystematicVariation("MUON_SCALE_CB_ELOSS", -1)) {
            param.m_CBparams[MCP::ScaleSmearParam::s0] = MCP::SystVariation::Up;
        } else if (!syst.empty()) return StatusCode::FAILURE;

        return StatusCode::SUCCESS;
    }

    StatusCode MuonCalibIntScaleSmearTool::applySystematicVariation(const SystematicSet &systConfig)
    {
        return m_Parameters.get(systConfig, m_currentParameters);
    }


    double MuonCalibIntScaleSmearTool::getExpectedResolution(const int &DetType, double pT, double eta, double phi, MCP::DataYear year, bool addMCCorrectionSmearing) const 
    {

        auto IDcorrConstants = m_IDparams.at(year);
        auto MEcorrConstants = m_MEparams.at(year);
        auto IDExpectedResConstants = m_IDExpectedResparams.at(year);
        auto MEExpectedResConstants = m_MEExpectedResparams.at(year);

        MCP::TrackType type = MCP::TrackType::ID;
        if(DetType == MCP::DetectorType::MS) type = MCP::TrackType::ME;

        auto trk = MCP::TrackCalibObj(type, pT * GeVtoMeV, eta, phi, year, false);


        double expRes = 0.;
        if (DetType == MCP::DetectorType::MS) 
        {
            if (pT == 0) return 1e12;

            double p0 = 0;
            double p1 = 0;
            double p2 = 0;

            if(!addMCCorrectionSmearing)
            {
                p0 = MEExpectedResConstants.at(MCP::ExpectedResParam::r0)->getCalibConstant(trk);
                p1 = MEExpectedResConstants.at(MCP::ExpectedResParam::r1)->getCalibConstant(trk);
                p2 = MEExpectedResConstants.at(MCP::ExpectedResParam::r2)->getCalibConstant(trk);
            }
            else
            {
                double expectedP0 = MEExpectedResConstants.at(MCP::ExpectedResParam::r0)->getCalibConstant(trk);
                double expectedP1 = MEExpectedResConstants.at(MCP::ExpectedResParam::r1)->getCalibConstant(trk);
                double expectedP2 = MEExpectedResConstants.at(MCP::ExpectedResParam::r2)->getCalibConstant(trk);

                double r0 = MEcorrConstants.at(MCP::ScaleSmearParam::r0).at(MCP::ScaleResCorrection::Nominal)->getCalibConstant(trk);
                double r1 = MEcorrConstants.at(MCP::ScaleSmearParam::r1).at(MCP::ScaleResCorrection::Nominal)->getCalibConstant(trk);
                double r2 = MEcorrConstants.at(MCP::ScaleSmearParam::r2).at(MCP::ScaleResCorrection::Nominal)->getCalibConstant(trk);

                p0 = std::sqrt(std::pow(expectedP0, 2) + std::pow(r0, 2));
                p1 = std::sqrt(std::pow(expectedP1, 2) + std::pow(r1, 2));
                p2 = std::sqrt(std::pow(expectedP2, 2) + std::pow(r2, 2));
            }

            expRes = std::sqrt(std::pow(p0 / pT, 2) + std::pow(p1, 2) + std::pow(p2 * pT, 2));
            
        } 
        else if (DetType == MCP::DetectorType::ID) 
        {
            if (pT == 0) return 1e12;

            double p1 = 0;
            double p2 = 0;

            if(!addMCCorrectionSmearing)
            {
                p1 = IDExpectedResConstants.at(MCP::ExpectedResParam::r1)->getCalibConstant(trk);
                p2 = IDExpectedResConstants.at(MCP::ExpectedResParam::r2)->getCalibConstant(trk);
                double p2Tan = IDExpectedResConstants.at(MCP::ExpectedResParam::r2tan2)->getCalibConstant(trk);
                if(p2Tan) p2 = p2Tan;
            }
            else
            {
                double expectedP1 = IDExpectedResConstants.at(MCP::ExpectedResParam::r1)->getCalibConstant(trk);
                double expectedP2 = IDExpectedResConstants.at(MCP::ExpectedResParam::r2)->getCalibConstant(trk);
                double p2Tan = IDExpectedResConstants.at(MCP::ExpectedResParam::r2tan2)->getCalibConstant(trk);
                if(p2Tan) expectedP2 = p2Tan;

                double r1 = IDcorrConstants.at(MCP::ScaleSmearParam::r1).at(MCP::ScaleResCorrection::Nominal)->getCalibConstant(trk);
                double r2 = IDcorrConstants.at(MCP::ScaleSmearParam::r2).at(MCP::ScaleResCorrection::Nominal)->getCalibConstant(trk);

                p1 = std::sqrt(std::pow(expectedP1, 2) + std::pow(r1, 2));
                p2 = std::sqrt(std::pow(expectedP2, 2) + std::pow(r2, 2));

                if(p2Tan) p2 = p2 * std::sinh(eta) * std::sinh(eta);
            }

            expRes = std::sqrt(std::pow(p1, 2) + std::pow(p2 * pT, 2));
            return expRes;  
        } 
        else 
        {
            ATH_MSG_ERROR("wrong DetType in input " << DetType);
            return 0.;
        }

        return expRes; 
    }


} // namespace CP
