/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONCORRECTION_ISOLATIONCORRECTIONTOOL_H
#define ISOLATIONCORRECTION_ISOLATIONCORRECTIONTOOL_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/AsgMetadataTool.h"
#include "AsgTools/AsgMessaging.h"
#include "IsolationCorrections/IIsolationCorrectionTool.h"
#include "IsolationCorrections/IsolationCorrection.h"
#include "TGraph.h"

namespace CP {

class IsolationCorrectionTool  : virtual public IIsolationCorrectionTool,
                                 public asg::AsgMetadataTool {
    // Create a proper constructor for Athena
    ASG_TOOL_CLASS3( IsolationCorrectionTool, IIsolationCorrectionTool, CP::ISystematicsTool, CP::IReentrantSystematicsTool)

      public:
	IsolationCorrectionTool( const std::string& name );
        virtual ~IsolationCorrectionTool();

        virtual StatusCode initialize() override;
        virtual StatusCode finalize()
#ifndef XAOD_STANDALONE
          override
#endif
          ;

        virtual StatusCode beginInputFile() override;
	virtual StatusCode beginEvent() override;
	virtual StatusCode endInputFile() override;

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
	void setDDVer(std::string a) { m_ddVersion = a; }; //temporary for test

  void Print() { m_isol_corr->Print(); };

    private:
	StatusCode get_simflavour_from_metadata(PATCore::ParticleDataType::DataType& result) ;
        float GetEtaPointing(const xAOD::Egamma* input) {return m_isol_corr->GetEtaPointing(input);};

        std::string m_corr_file;
        std::string m_corr_ddshift_file;
        std::string m_corr_ddshift_2015_2016_file;
        std::string m_corr_ddshift_2017_file;
        std::string m_corr_ddsmearing_file;
        IsolationCorrection* m_isol_corr;
        std::string m_tool_ver_str;

        bool m_apply_SC_leak_corr;
        bool m_apply_etaEDParPU_corr;
        bool m_apply_etaEDParPU_mc_corr;
        std::string m_corr_etaEDParPU_file;
        std::string m_corr_etaEDPar_mc_corr_file;
        std::map<xAOD::Iso::IsolationType,std::unique_ptr<TGraph>> m_map_isotype_zetaPU;
        std::map<xAOD::Iso::IsolationType,std::unique_ptr<TGraph>> m_map_isotype_zeta_mc_corr;

        bool m_usemetadata;
	bool m_is_mc;
        bool m_AFII_corr;
	std::string m_ddVersion;
        bool m_apply_dd, m_apply_ddDefault;
        bool m_correct_etcone;
        bool m_trouble_categories;
        bool m_useLogLogFit;
        bool m_forcePartType;
        bool m_metadata_retrieved;

	// For systematcis
	CP::SystematicVariation m_systDDonoff;
};

}

#endif
