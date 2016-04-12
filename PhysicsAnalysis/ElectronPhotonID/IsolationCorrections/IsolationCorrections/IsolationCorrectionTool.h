/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONCORRECTION_ISOLATIONCORRECTIONTOOL_H
#define ISOLATIONCORRECTION_ISOLATIONCORRECTIONTOOL_H

#include "AsgTools/AsgTool.h"
#include "IsolationCorrections/IIsolationCorrectionTool.h"
#include "IsolationCorrections/IsolationCorrection.h"

namespace CP {

class IsolationCorrectionTool  : virtual public IIsolationCorrectionTool,
                                 virtual public CP::ISystematicsTool,
                                 public asg::AsgTool {
    // Create a proper constructor for Athena
    ASG_TOOL_CLASS2( IsolationCorrectionTool, IIsolationCorrectionTool, CP::ISystematicsTool)

      public:
	IsolationCorrectionTool( const std::string& name );
        ~IsolationCorrectionTool();

        StatusCode initialize();
        StatusCode finalize();

        // Apply correction to a modifyable Egamma object
        virtual CP::CorrectionCode applyCorrection(xAOD::Egamma&);

      	// This helps to correct for the (wrong) leakage at the analysis level
      	virtual CP::CorrectionCode CorrectLeakage(xAOD::Egamma&);

        // Create a corrected copy from a const Egamma object
        virtual CP::CorrectionCode correctedCopy( const xAOD::Egamma&, xAOD::Egamma*&);

        //systematics
        // Which systematics have an effect on the tool's behaviour?
        virtual CP::SystematicSet affectingSystematics() const;
        // Is the tool affected by a specific systematic?
        virtual bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const;
        //Systematics to be used for physics analysis
        virtual CP::SystematicSet recommendedSystematics() const;
        //Use specific systematic
        virtual CP::SystematicCode applySystematicVariation ( const CP::SystematicSet& systConfig );

        float GetEtaPointing(const xAOD::Egamma* input) {return m_isol_corr->GetEtaPointing(input);};

        float GetPtCorrectedIsolation(const xAOD::Egamma&, xAOD::Iso::IsolationType);
	float GetPtCorrection(const xAOD::Egamma&, xAOD::Iso::IsolationType);
        float GetDDCorrection(const xAOD::Egamma&, xAOD::Iso::IsolationType);

    private:

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
        bool m_correct_etcone;
        bool m_trouble_categories;

	// For systematcis
	CP::SystematicVariation m_systDDonoff;
};

}

#endif
