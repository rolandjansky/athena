/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCALIBTOOLS_GENERICHISTSCALECORRECTION_H
#define JETCALIBTOOLS_GENERICHISTSCALECORRECTION_H

// This class provides support for an arbitrary histogram scale factor to be inserted in the calibration sequence
// Original intent relates to the trigger-level dijet analysis, which derived online/offline correction factor
// However, this class is intended to be more generic as requested

#include <TH1.h>

#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCalibTools/JetCalibrationToolBase.h"

class GenericHistScaleCorrection
    : virtual public ::IJetCalibrationTool
    , virtual public ::JetCalibrationToolBase
{
    ASG_TOOL_CLASS( GenericHistScaleCorrection, IJetCalibrationTool )

    public:
        // Constructor/destructor/init
        GenericHistScaleCorrection(const std::string name = "GenericHistScaleCorrection");
        GenericHistScaleCorrection(const std::string& name, TEnv* config, TString jetAlgo, TString calibAreaTag, bool dev);
        virtual ~GenericHistScaleCorrection();
        
        virtual StatusCode initializeTool(const std::string& name);

    protected:
        // Concrete implementation of abstract method
        virtual StatusCode calibrateImpl(xAOD::Jet& jet, JetEventInfo&) const;

    private:
        // Private members set in the constructor
        TEnv* m_config;
        TString m_jetAlgo, m_calibAreaTag;
        bool m_dev;

        // Private members set during initialization
        TString m_jetOutScale;
        std::unique_ptr<TH1> m_hist;

        // Private helper functions
        StatusCode getCorrectionFactor(const xAOD::Jet& jet, double& factor) const;
        double enforceAxisRange(const TAxis& axis, const double inputValue) const;

};

#endif

