/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONCORRECTION_ISOLATIONCORRECTIONTOOL_H
#define ISOLATIONCORRECTION_ISOLATIONCORRECTIONTOOL_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/AsgMetadataTool.h"
#include "AsgMessaging/AsgMessaging.h"
#include "IsolationCorrections/IIsolationCorrectionTool.h"
#include "IsolationCorrections/IsolationCorrection.h"

namespace CP {

class IsolationCorrectionTool  : virtual public IIsolationCorrectionTool,
                                 virtual public CP::ISystematicsTool,
                                 public asg::AsgMetadataTool {
    // Create a proper constructor for Athena
    ASG_TOOL_CLASS2( IsolationCorrectionTool, IIsolationCorrectionTool, CP::ISystematicsTool)

      public:
	IsolationCorrectionTool( const std::string& name );
        virtual ~IsolationCorrectionTool();

        virtual StatusCode initialize() override;
        virtual StatusCode finalize()
#ifndef XAOD_STANDALONE
          override
#endif
          ;

        // Apply correction to a modifyable Egamma object
        virtual CP::CorrectionCode applyCorrection(xAOD::Egamma&) override;

      	// This helps to correct for the (wrong) leakage at the analysis level
      	virtual CP::CorrectionCode CorrectLeakage(xAOD::Egamma&) override;

        // Create a corrected copy from a const Egamma object
        virtual CP::CorrectionCode correctedCopy( const xAOD::Egamma&, xAOD::Egamma*&) override;

        //systematics
        // Which systematics have an effect on the tool's behaviour?
        virtual CP::SystematicSet affectingSystematics() const override;
        // Is the tool affected by a specific systematic?
        virtual bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const override;
        //Systematics to be used for physics analysis
        virtual CP::SystematicSet recommendedSystematics() const override;
        //Use specific systematic
        virtual CP::SystematicCode applySystematicVariation ( const CP::SystematicSet& systConfig ) override;

        virtual float GetPtCorrectedIsolation(const xAOD::Egamma&, xAOD::Iso::IsolationType) override;
	virtual float GetPtCorrection(const xAOD::Egamma&, xAOD::Iso::IsolationType) const override;
        virtual float GetDDCorrection(const xAOD::Egamma&, xAOD::Iso::IsolationType) override;

    private:
	StatusCode get_simflavour_from_metadata(PATCore::ParticleDataType::DataType& result) ;
        float GetEtaPointing(const xAOD::Egamma* input) {return m_isol_corr->GetEtaPointing(input);};

        std::string m_corr_file;
        std::string m_corr_ddshift_2015_file;
        std::string m_corr_ddshift_file;
        std::string m_corr_ddsmearing_file;
        IsolationCorrection* m_isol_corr;
        std::string m_tool_ver_str;
	bool m_is_mc;
        bool m_AFII_corr;
	std::string m_ddVersion;
        bool m_apply_dd;
        bool m_apply_ddDefault;
        bool m_correct_etcone;
        bool m_trouble_categories;

	// For systematcis
	CP::SystematicVariation m_systDDonoff;
};

}

#endif
