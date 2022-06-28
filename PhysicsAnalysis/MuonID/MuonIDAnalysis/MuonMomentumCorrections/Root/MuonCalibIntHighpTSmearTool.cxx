/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Framework include(s):
#include "AsgDataHandles/ReadHandle.h"
#include "PathResolver/PathResolver.h"

// Local include(s):
#include <cmath>
#include "TRandom3.h"

#include "MuonMomentumCorrections/MuonCalibIntHighpTSmearTool.h"
#include "MuonMomentumCorrections/EnumDef.h"
#include <MuonMomentumCorrections/CalibInitializer.h>
#include "MuonAnalysisInterfaces/IMuonSelectionTool.h"


namespace CP
{

    MuonCalibIntHighpTSmearTool::MuonCalibIntHighpTSmearTool(const std::string &name) : asg::AsgTool(name)
    {
    }

    StatusCode MuonCalibIntHighpTSmearTool::initialize()
    {
        // Greet the user:
        ATH_MSG_INFO("Initializing MuonCalibIntHighpTSmearTool");

        // Get the m_eventinfo container
        ATH_CHECK(m_eventInfo.initialize());

        
        // Load the constants
        for(const auto& year: MCP::dataYearList)
        {
            m_MS_Misaligned_p1[year]          = MCP::CalibInitializer::createHighpTCorrMap(year, "Misaligned", m_release, "p1");
            m_MS_Misaligned_p2[year]          = MCP::CalibInitializer::createHighpTCorrMap(year, "Misaligned", m_release, "p2");
            m_MS_AlignedAndCorrected_p1[year] = MCP::CalibInitializer::createHighpTCorrMap(year, "AlignedAndCorrected", m_release, "p1");
            m_MS_AlignedAndCorrected_p2[year] = MCP::CalibInitializer::createHighpTCorrMap(year, "AlignedAndCorrected", m_release, "p2");

        }


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

    CorrectionCode MuonCalibIntHighpTSmearTool::applyCorrection(MCP::MuonObj& mu) const
    {
        // Do nothing, if it is data
        if(mu.CB.isData) return CorrectionCode::Ok;


        auto cat = ConvertToMacroCategory(mu.raw_mst_category);

        double MS_Misaligned_p1 = m_MS_Misaligned_p1.at(mu.CB.year).at(cat)->getCalibConstant(mu.CB);
        double MS_Misaligned_p2 = m_MS_Misaligned_p2.at(mu.CB.year).at(cat)->getCalibConstant(mu.CB);
        double reso_Misaligned = std::hypot(MS_Misaligned_p1, MS_Misaligned_p2 * mu.CB.calib_pt);

        double MS_AlignedAndCorrected_p1 = m_MS_AlignedAndCorrected_p1.at(mu.CB.year).at(cat)->getCalibConstant(mu.CB);
        double MS_AlignedAndCorrected_p2 = m_MS_AlignedAndCorrected_p2.at(mu.CB.year).at(cat)->getCalibConstant(mu.CB);

        double reso_AlignedAndCorrected = std::sqrt(std::pow(MS_AlignedAndCorrected_p1, 2) + std::pow(MS_AlignedAndCorrected_p2 * mu.CB.calib_pt, 2));

        double smearDeltaCBOnly = 0;
        if (reso_Misaligned > reso_AlignedAndCorrected) smearDeltaCBOnly = std::sqrt(std::pow(reso_Misaligned, 2) - std::pow(reso_AlignedAndCorrected, 2));
        
        
        double smeared_combined_pt = mu.CB.calib_pt * (1. + mu.rnd_g_highPt * smearDeltaCBOnly);

        mu.CB.calib_pt = smeared_combined_pt;

        // Return gracefully:
        return CorrectionCode::Ok;
    }

    bool MuonCalibIntHighpTSmearTool::isAffectedBySystematic(const SystematicVariation &systematic) const
    {
        SystematicSet sys = affectingSystematics();
        return sys.find(systematic) != sys.end();
    }

    SystematicSet MuonCalibIntHighpTSmearTool::affectingSystematics() const
    {
        SystematicSet result;
        // No systematics for now
        return result;
    }

    SystematicSet MuonCalibIntHighpTSmearTool::recommendedSystematics() const { return affectingSystematics(); }

    StatusCode MuonCalibIntHighpTSmearTool::applySystematicVariation(const SystematicSet&)
    {
        // No systematics current supported for this tool
        return StatusCode::SUCCESS;
    }

    MCP::MST_Categories MuonCalibIntHighpTSmearTool::ConvertToMacroCategory(const int raw_mst_category) const {
        if (raw_mst_category < 0) {
            return MCP::MST_Categories::Undefined;
        } else if (raw_mst_category & IMuonSelectionTool::CategoryZero)
            return MCP::MST_Categories::Zero;
        else if (raw_mst_category & IMuonSelectionTool::CategoryOne)
            return MCP::MST_Categories::One;
        else if (raw_mst_category & IMuonSelectionTool::CategoryTwo)
            return MCP::MST_Categories::Two;
        else if (raw_mst_category & IMuonSelectionTool::CategoryThree)
            return MCP::MST_Categories::Three;
        else if (raw_mst_category & IMuonSelectionTool::CategoryFour)
            return MCP::MST_Categories::Four;
        else
            return MCP::MST_Categories::Undefined;
    }



} // namespace CP
