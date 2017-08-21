/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

/*
 * IMuonEfficiencyScaleFactors.h
 *
 *  Created on: Apr 8, 2014
 *      Author: goblirsc
 */

#ifndef IMUONEFFICIENCYSCALEFACTORS_H_
#define IMUONEFFICIENCYSCALEFACTORS_H_

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODMuon/Muon.h"
#include <xAODEventInfo/EventInfo.h>

// Local include(s):
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"

// STL include(s):
#include <string>
#include <vector>
#include <map>

/**
 * @mainpage MuonEfficiencyCorrections
 *
 * This package provides information about the muon efficiency and the corresponding scale factors.
 *
 * supported <b> user configuration via Properties</b>:
 *
 * set using the setProperty("NameOfProperty",YourInput) method <br>
 * <ul>
 * <li><b>"WorkingPoint"</b> -> Takes a string</li> <br>
 *  supported working points:
 * <li> "Loose","Medium" or "Tight" - see https://twiki.cern.ch/twiki/bin/view/Atlas/MuonSelectionTool
 * </ul>
 * <li><b>"DataPeriod"</b> -> Takes a string </li><br>
 *  supported data period:/li>
 * <li> "2015": 13 TeV pp data - default, so no need to configure this atm! 
 * </ul>
 * <li><b>Optional: "doAudit"</b> -> Activates audit mode  (for debugging) </li><br>
 *  supported choices:
 * <ul>
 * <li> true  </li>
 * <li> false (default)  </li>
 * </ul>
 * <li><i><b>OPTIONAL: "LumiWeights"</b></i></li><br>
 * -> Assign custom lumi weights to replace the default (AllGood GRL) ones
 *
 *    Argument should be an std::map <std::string, double>, where the strings are run or period names
 *    and the doubles are the lumi values
 *
 *    Example:  std::map<std::string, double> my_lumis; <br>
 *              my_lumis ["A"] = 201.1; <br>
 *              my_lumis ["B"] = 600.4; <br>
 *              (...) <br>
 *              m_effi_corr.setProperty("LumiWeights",my_lumis); <br>
 *
 *              or <br>
 *              my_lumis ["205123"] = 23.5; <br>
 *              my_lumis ["205126"] = 42.1; <br>
 *              (...) <br>
 *              m_effi_corr.setProperty("LumiWeights",my_lumis); <br>
 * </ul>
 *
 *  <b>example use</b>, for 8 TeV data and Medium+ Muons:
 *
 *  (when initializing)
 *
 *  CP::MuonEfficiencyCorrections m_effi_corr ("EfficiencyCorrections"); <br>
 *
 *  m_effi_corr.setProperty("WorkingPoint",CP::MuonEfficiencyWorkingPoint::CBandST); <br>
 *  m_effi_corr.setProperty("DataPeriod","2012"); <br>
 *  m_effi_corr.initialize();
 *
 *
 *  (when calibrating your muons)
 *
 *  // either (decorate the muon with the information)
 *
 *  CHECK(m_effi_corr.applyEfficiencyScaleFactor(*mu)); <br>
 *  float my_SF = mu->auxdata< float >( "EfficiencyScaleFactor" ); <br>
 *
 *  // or (only obtain the SF, without decorating the muon):
 *
 *  float my_scale_factor = 0; <br>
 *  CHECK(m_effi_corr.getEfficiencyScaleFactor(*mu),my_scale_factor); <br>
 *
 *
 *
 * see the README for more details
 * @author goblirsc@SPAMNOT_CERN.ch
 *
 *
 *
 */

namespace CP {
    class SystematicSet;

    class IMuonEfficiencyScaleFactors: public virtual CP::ISystematicsTool {
            ///
            /// @class IMuonEfficiencyScaleFactors
            /// @brief Interface class.
            /// Abstract interface class. Provides the user interface for the
            /// MuonEfficiencyScaleFactors class.

            ASG_TOOL_INTERFACE (CP::IMuonEfficiencyScaleFactors)

        public:

            /// Retrieve the Scale factor
            virtual CorrectionCode getEfficiencyScaleFactor(const xAOD::Muon& mu, float& sf, const xAOD::EventInfo* info = 0) const = 0;
            /// decorate the muon with scale factor information
            virtual CorrectionCode applyEfficiencyScaleFactor(const xAOD::Muon& mu, const xAOD::EventInfo* info = 0) const = 0;

            /// BONUS functionality

            /// An optional, less conservative way of treating the SF stat error.
            /// Here, we generate a set of SF replicas,
            /// each by smearing the SF bin by bin with a gaussian with the width of the SF error in that bin
            /// the SF stat systematic is then the variation of the final observable within the replicas

            virtual CorrectionCode getEfficiencyScaleFactorReplicas(const xAOD::Muon& mu, std::vector<float> & sf_err, const xAOD::EventInfo* info = 0) const = 0;
            /// decorate the muon with a set of SF replica weights.
            virtual CorrectionCode applyEfficiencyScaleFactorReplicas(const xAOD::Muon& mu, int nreplicas = 50, const xAOD::EventInfo* info = 0) const = 0;

            /// Obtain the muon efficiency measured using the data
            //  Can for example be used to scale truth-level MC
            virtual CorrectionCode getDataEfficiency(const xAOD::Muon& mu, float& eff, const xAOD::EventInfo* info = 0) const = 0;
            /// decorate a muon with the efficiency information
            virtual CorrectionCode applyDataEfficiency(const xAOD::Muon& mu, const xAOD::EventInfo* info = 0) const= 0;

            ///Obtain the muon efficiency measured using the MonteCarlo
            virtual CorrectionCode getMCEfficiency(const xAOD::Muon &mu, float& eff, const xAOD::EventInfo* info = 0) const=0;
            // decorate a muon with the efficiency information
            virtual CorrectionCode applyMCEfficiency(const xAOD::Muon &mu, const xAOD::EventInfo* info = 0) const =0;

            //Functions to get the names of the unfolded systematic bins
            virtual int getUnCorrelatedSystBin(const xAOD::Muon& mu) const=0;
            virtual std::string getUncorrelatedSysBinName(unsigned int Bin) const = 0;
            virtual std::string getUncorrelatedSysBinName(const SystematicSet& systConfig) const = 0;

            virtual ~IMuonEfficiencyScaleFactors() {
            }

    };
// class IMuonEfficiencyTool

}// namespace CP

#endif /* IMUONEFFICIENCYSCALEFACTORS_H_ */
