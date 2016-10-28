/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonEfficiencyScaleFactors.h
 *
 *  Created on: Apr 9, 2014
 *      Author: goblirsc
 */

#ifndef MUONEFFICIENCYSCALEFACTORS_H_
#define MUONEFFICIENCYSCALEFACTORS_H_

#include "MuonEfficiencyCorrections/IMuonEfficiencyScaleFactors.h"
#include "MuonEfficiencyCorrections/EfficiencyScaleFactor.h"
#include "MuonEfficiencyCorrections/EffiCollection.h"

#include <AsgTools/ToolHandle.h>
#include <AsgTools/AsgTool.h>

#include <string>

#include <boost/unordered_map.hpp>

namespace CP {
    class MuonEfficiencyScaleFactors: public CP::IMuonEfficiencyScaleFactors, public asg::AsgTool {
            ASG_TOOL_CLASS2( MuonEfficiencyScaleFactors, CP::IMuonEfficiencyScaleFactors, CP::ISystematicsTool )

            public:
            MuonEfficiencyScaleFactors(const std::string& name);

            virtual ~MuonEfficiencyScaleFactors();

            /// initialize the tool once all settings are in place!
            virtual StatusCode initialize();

            /// Retrieve the Scale factor and decorate the muon
            virtual CorrectionCode getEfficiencyScaleFactor(const xAOD::Muon& mu, float& sf, const xAOD::EventInfo* info = 0);
            virtual CorrectionCode applyEfficiencyScaleFactor(const xAOD::Muon& mu, const xAOD::EventInfo* info = 0);
            /// replica generation
            virtual CorrectionCode getEfficiencyScaleFactorReplicas(const xAOD::Muon& mu, std::vector<float> & sf_err, const xAOD::EventInfo* info = 0);
            virtual CorrectionCode applyEfficiencyScaleFactorReplicas(const xAOD::Muon& mu, int nreplicas = 50, const xAOD::EventInfo* info = 0);

            /// returns: whether this tool is affected by the given systematis
            virtual bool isAffectedBySystematic(const SystematicVariation& systematic) const;

            /// returns: the list of all systematics this tool can be affected by
            virtual SystematicSet affectingSystematics() const;

            /// returns: the list of al l systematics this tool recommends to use
            virtual SystematicSet recommendedSystematics() const;

            virtual SystematicCode applySystematicVariation(const SystematicSet& systConfig);

            /// Obtain the muon efficiency measured using the data
            virtual CorrectionCode getDataEfficiency(const xAOD::Muon& mu, float& eff, const xAOD::EventInfo* info = 0);
            virtual CorrectionCode applyDataEfficiency(const xAOD::Muon& mu, const xAOD::EventInfo* info = 0);

            virtual CorrectionCode getDataEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & sf_err, const xAOD::EventInfo* info = 0);
            virtual CorrectionCode applyDataEfficiencyReplicas(const xAOD::Muon& mu, int nreplicas = 50, const xAOD::EventInfo* info = 0);

            /// Obtain the muon efficiency measured using the MC
            virtual CorrectionCode getMCEfficiency(const xAOD::Muon& mu, float& eff, const xAOD::EventInfo* info = 0);
            virtual CorrectionCode applyMCEfficiency(const xAOD::Muon& mu, const xAOD::EventInfo* info = 0);

            virtual CorrectionCode getMCEfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & sf_err, const xAOD::EventInfo* info = 0);
            virtual CorrectionCode applyMCEfficiencyReplicas(const xAOD::Muon& mu, int nreplicas = 50, const xAOD::EventInfo* info = 0);

            // audit trail functionality
            bool AlreadyApplied(const xAOD::Muon & mu);
            bool AlreadyApplied(xAOD::Muon & mu);

            // copy constructor, to make reflex happy...
            MuonEfficiencyScaleFactors(const MuonEfficiencyScaleFactors& tocopy);

            // to make coverity happy
            MuonEfficiencyScaleFactors & operator =(const MuonEfficiencyScaleFactors & tocopy);

        private:

            unsigned int getRandomRunNumber(const xAOD::EventInfo* info);
            /// load the SF histos
            bool LoadEffiSet(SystematicSet sys);
            bool LoadInputs();

            /// a tool to help handle the user lumi inputs

            /// construct the name of the input files from the configuration
            std::string filename_Central();
            std::string filename_Calo();
            std::string filename_HighEta();
            std::string filename_LowPt();
            std::string filename_LowPtCalo();

            // apply info to muons and log that we processed them
            void ApplyAuditInfo(const xAOD::Muon& mu);

            // reset the audit log at every event
            void CheckAuditEvent();

            // utility method to 'dress' a filename using the path resolver
            std::string resolve_file_location(std::string filename);

            //Some util functions
            void CopyInformation(const MuonEfficiencyScaleFactors & tocopy);
            StatusCode CreateDecorator(SG::AuxElement::Decorator<float>* &Dec, std::string &DecName, const std::string& defaultName);
            StatusCode CreateVecDecorator(SG::AuxElement::Decorator<std::vector<float>>* &Dec, std::string &DecName, const std::string& defaultName);

            StatusCode IsDecoratorNameUnique(std::string &name);

            /// the working point to operate on
            std::string m_wp;
            /// the data taking period to work with

            boost::unordered_map<SystematicSet, EffiCollection*> m_sf_sets;
            EffiCollection *m_current_sf;

            std::string m_custom_dir;
            std::string m_custom_file_Combined;
            std::string m_custom_file_Calo;
            std::string m_custom_file_HighEta;
            std::string m_custom_file_LowPt;
            std::string m_custom_file_LowPtCalo;

            // audit mode
            bool m_doAudit;

            // remember what we already looked at
            std::set<std::pair<EffiCollection*, xAOD::Muon*> > m_audit_processed;

            // info to apply to the muon when in audit mode
            std::string m_version_string;
            std::string m_sys_string;
            // the current event
            unsigned long long m_audit_last_evt;

            std::map<CP::SystematicSet, CP::SystematicSet> m_filtered_sys_sets;

            std::string m_efficiency_decoration_name_data;
            std::string m_efficiency_decoration_name_mc;
            std::string m_sf_decoration_name;
            std::string m_sf_replica_decoration_name;
            std::string m_eff_replica_decoration_name;
            std::string m_mc_eff_replica_decoration_name;

            // subfolder to load from the calibration db
            std::string m_calibration_version;

            std::string m_effType;

            // threshold below which low-pt SF (i.e. from JPsi) should be used
            double m_lowpt_threshold;
            // decorators to quickly apply the eff and SF
            SG::AuxElement::Decorator<float>* m_effDec;
            SG::AuxElement::Decorator<float>* m_MCeffDec;
            SG::AuxElement::Decorator<float>* m_sfDec;

            SG::AuxElement::Decorator<std::vector<float> >* m_sfrDec;
            SG::AuxElement::Decorator<std::vector<float> >* m_effrDec;
            SG::AuxElement::Decorator<std::vector<float> >* m_MCeffrDec;

            CP::SystematicSet m_affectingSys;

            bool m_init;
    };

} /* namespace CP */

#endif /* MUONEFFICIENCYSCALEFACTORS_H_ */
