/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCALIBTOOLS_JETSMEARINGCORRECTION_H
#define JETCALIBTOOLS_JETSMEARINGCORRECTION_H


#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetCalibTools/JetCalibrationToolBase.h"

#include "TRandom3.h"

class JetSmearingCorrection
    : virtual public IJetCalibrationTool, virtual public JetCalibrationToolBase
{

    ASG_TOOL_CLASS(JetSmearingCorrection,IJetCalibrationTool)

    public:
        // Constructor/destructor/init
        JetSmearingCorrection(const std::string name = "NominalJetSmear");
        JetSmearingCorrection(const std::string& name, TEnv* config, TString jetAlgo, TString calibAreaTag, bool dev);
        virtual ~JetSmearingCorrection();
        virtual StatusCode initializeTool(const std::string& name);

    protected:
        virtual StatusCode calibrateImpl(xAOD::Jet& jet, JetEventInfo&) const;

    private:
        // Helper methods
        StatusCode getSigmaSmear(xAOD::Jet& jet, double& sigmaSmear) const;
        StatusCode readHisto(double& returnValue, TH1* histo, double x) const;
        StatusCode readHisto(double& returnValue, TH1* histo, double x, double y) const;
        StatusCode readHisto(double& returnValue, TH1* histo, double x, double y, double z) const;

        // Private enums
        enum class SmearType
        {
            UNKNOWN=0,  // Unknown/unset/etc
            Pt,         // pT smearing (JpTR)
            Mass,       // Mass smearing (JMR)
            FourVec     // Four-vector smearing (JER)
        };
        enum class HistType
        {
            UNKNOWN=0,  // Unknown/unset/etc
            Pt,         // 1D histogram, binned in pT
            PtEta,      // 2D histogram, binned in x=pT y=eta
            PtAbsEta    // 2D histogram, binned in x=pT y=|eta|
        };
        enum class InterpType
        {
            UNKNOWN=0,  // Unknown/unset/etc
            Full,       // Full interpolation
            None,       // No interpolation
            OnlyX,      // Interpolate only in the x dimension
            OnlyY       // Interpolate only in the y dimension
        };

        // Class variables from constructor
        TEnv* m_config;
        const TString m_jetAlgo;
        const TString m_calibAreaTag;
        const bool m_dev;
        mutable TRandom3 m_rand; // mutable as this we want to call in a const function (everything else is fixed, the random generator is modifiable)
        
        // Class variables read in from the config file
        TString m_jetStartScale;
        TString m_jetOutScale;
        SmearType  m_smearType;
        HistType   m_histType;
        InterpType m_interpType;
        TH1* m_smearResolutionMC;
        TH1* m_smearResolutionData;
};


#endif

