/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCALIBTOOLS_MC2MCCORRECTION_H
#define JETCALIBTOOLS_MC2MCCORRECTION_H


#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCalibTools/JetCalibrationToolBase.h"

#include <memory>
#include <vector>

class MC2MCCorrection
    : virtual public IJetCalibrationTool, virtual public JetCalibrationToolBase
{

    ASG_TOOL_CLASS(MC2MCCorrection,IJetCalibrationTool)

    public:
        // Constructor/destructor/init
        MC2MCCorrection(const std::string name = "MC2MCtool");
        MC2MCCorrection(const std::string& name, TEnv* config, TString jetAlgo, TString calibAreaTag, TString shower, bool dev);
        virtual ~MC2MCCorrection();
        virtual StatusCode initializeTool(const std::string& name);

    protected:
        virtual StatusCode calibrateImpl(xAOD::Jet& jet, JetEventInfo& jetEventInfo) const;

    private:
        // Helper methods
        StatusCode readHisto(double& returnValue, TH2* histo, double x, double y) const;

        // Class variables from constructor
        TEnv* m_config;
        const TString m_jetAlgo;
        const TString m_calibAreaTag;
        const TString m_shower;
        const bool m_dev;
        
        // Class variables read in from the config file
        TString m_jetStartScale;
        TString m_jetOutScale;
        TString m_mapTag;
        bool m_doCjetCorrection;
        bool m_doBjetCorrection;

        TH2 *m_scalingMC_g;
        TH2 *m_scalingMC_q;
        TH2 *m_scalingMC_c;
        TH2 *m_scalingMC_b;

};


#endif

