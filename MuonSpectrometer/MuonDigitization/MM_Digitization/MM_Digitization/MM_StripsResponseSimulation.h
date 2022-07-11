/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_STRIPRESPONSESIMULATION_H
#define MM_DIGITIZATION_STRIPRESPONSESIMULATION_H
/** @class StripsResponse

// ------------
// Authors:
//   Iakovidis George        <george.iakovidis@cern.ch>
//   Karakostas Konstantinos <konstantinos.karakostas@cern.ch>
//   Leontsinis Stefanos     <stefanos.leontsinis@cern.ch>
//   Nektarios Chr. Benekos  <nbenekos@cern.ch>
//
// Major Contributions From: Verena Martinez
//                           Tomoyuki Saito
//
// Major Restructuring for r21+ From: Lawrence Lee <lawrence.lee.jr@cern.ch>
//
//////////////////////////////////////////////////////////////////////////////

Comments to be added here...

*/

/// ROOT
#include <TF1.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TRandom3.h>

/// Projects
#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/StatusCode.h"
#include "MM_Digitization/MM_DigitToolInput.h"
#include "MM_Digitization/MM_IonizationCluster.h"
#include "MM_Digitization/MM_StripToolOutput.h"

/// STD'S
#include <sys/stat.h>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

class MM_DigitToolInput;
class MM_StripToolOutput;

namespace CLHEP {
    class HepRandomEngine;
    class HepRandom;
    class RandGeneral;
}  // namespace CLHEP

class MM_StripsResponseSimulation : public AthMessaging {
public:

    struct ConfigModule{
        /** qThreshold=2e, we accept a good strip if the charge is >=2e */
        float qThreshold{0.};

        /** // 0.350/10 diffusSigma=transverse diffusion (350 microm per 1cm ) for 93:7 @ 600 V/cm, according to garfield  */
        float transverseDiffusionSigma{0.};
        float longitudinalDiffusionSigma{0.};
        /** crosstalk of neighbor strips, it's 15%  */
        float crossTalk1{0.};  // 0.10; //
        /** // crosstalk of second neighbor strips, it's 6% */
        float crossTalk2{0.};  // 0.03;

        float driftGapWidth{0.};

        /** //0.050 drift velocity in [mm/ns], driftGap=5 mm +0.128 mm (the amplification gap) */
        float driftVelocity{0.};

        // Avalanche gain
        float avalancheGain{0.};
        int maxPrimaryIons{300};

        float interactionDensityMean{0.};
        float interactionDensitySigma{0.};

        bool writeOutputFile{false};
        bool writeEventDisplays{false};

        /// ToDo: random number from custom functions
        const TF1* lorentzAngleFunction{nullptr};
    };
    MM_StripsResponseSimulation(ConfigModule&& cfg);

    virtual ~MM_StripsResponseSimulation();
    MM_StripToolOutput GetResponseFrom(const MM_DigitToolInput& digiInput, double gainFraction, double stripPitch,
                                       CLHEP::HepRandomEngine* rndmEngine);   
  
   
    float getQThreshold() const { return m_cfg.qThreshold; };
    float getDriftGapWidth() const { return m_cfg.driftGapWidth; };
    float getDriftVelocity() const { return m_cfg.driftVelocity; };
    float getInteractionDensityMean() const { return m_cfg.interactionDensityMean; }
    float getInteractionDensitySigma() const { return m_cfg.interactionDensitySigma; }
    float getLongitudinalDiffusionSigma() const { return m_cfg.longitudinalDiffusionSigma; }
    float getTransversDiffusionSigma() const { return m_cfg.transverseDiffusionSigma; }
    const TF1* getLorentzAngleFunction() const { return m_cfg.lorentzAngleFunction; }

private:
    void initHistos();  
    void writeHistos(); 
    const ConfigModule m_cfg{};

    struct DataCache {
        std::vector<std::unique_ptr<MM_IonizationCluster>> IonizationClusters{};
        std::vector<int> finalNumberofStrip{};
        std::vector<std::vector<float>> finalqStrip{};
        std::vector<std::vector<float>> finaltStrip{};
        std::vector<float> tStripElectronicsAbThr{};
        std::vector<float> qStripElectronics{};
    };

    void whichStrips(DataCache& cache,
                     const MM_DigitToolInput& digiInput, double gainFraction,
                     double stripPitch, CLHEP::HepRandomEngine* rndmEngine);

  
    std::map<TString, TH1F*> m_mapOfHistograms{};
    std::map<TString, TH2F*> m_mapOf2DHistograms{};

    std::unique_ptr<CLHEP::RandGeneral> m_randNelectrons{};

    static constexpr unsigned int s_NelectronPropBins{300};


    std::unique_ptr<TFile> m_outputFile{nullptr};

protected:
    // seperate random number generation for performance monitoring
    float generateTransverseDiffusion(float posY, CLHEP::HepRandomEngine* rndmEngine) const;
    float getTransverseDiffusion(float posY, CLHEP::HepRandomEngine* rndmEngine);
    float getLongitudinalDiffusion(float posY, CLHEP::HepRandomEngine* rndmEngine) const;
    float getEffectiveCharge(CLHEP::HepRandomEngine* rndmEngine) const;
    float getPathLengthTraveled(CLHEP::HepRandomEngine* rndmEngine) const;
};
#endif
