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
        
        float GetPtCorrectedIsolation(const xAOD::Egamma&, xAOD::Iso::IsolationType);
	float GetPtCorrection(const xAOD::Egamma&, xAOD::Iso::IsolationType);

    private:

        std::string m_corr_file;
        IsolationCorrection* m_isol_corr;
        std::string m_tool_ver_str;
        bool m_is_mc;
	
};

}

#endif
