/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_StepHistogram_H
#define G4DEBUGGINGTOOLS_StepHistogram_H

//C++
#include <map>
#include <string>

//ROOT
#include "TH1.h"
#include "TH2.h"

//G4
#include "G4UserEventAction.hh"
#include "G4UserSteppingAction.hh"
#include "G4UserRunAction.hh"
#include "G4String.hh"

//Athena
#include "AthenaBaseComps/AthMessaging.h"
#include "G4DebuggingHelper.h"

namespace G4UA{
  
  class StepHistogram : public AthMessaging,
                        public G4UserEventAction,
                        public G4UserRunAction,
                        public G4UserSteppingAction
  {
  public:
    /// the hooks for G4 UA handling
    virtual void UserSteppingAction(const G4Step*) override;

    // maps to hold info per volume/process/material per particle type
    typedef std::map<G4String, TH1*> HistoMap_t;
    typedef std::map<G4String, HistoMap_t> HistoMapMap_t;

    /// this holds all the data from individual threads that needs to be merged at EoR
    struct Report
    {
      // distributions per volume per particle type
      HistoMapMap_t histoMapMap_vol_stepSize;
      HistoMapMap_t histoMapMap_vol_stepKineticEnergy;
      HistoMapMap_t histoMapMap_vol_postStepKineticEnergy;
      HistoMapMap_t histoMapMap_vol_stepPseudorapidity;
      HistoMapMap_t histoMapMap_vol_stepEnergyDeposit;
      HistoMapMap_t histoMapMap_vol_stepEnergyNonIonDeposit;
      HistoMapMap_t histoMapMap_vol_stepSecondaryKinetic;
      HistoMapMap_t histoMapMap_vol_numberOfSteps;
      HistoMapMap_t histoMapMap_vol_numberOfStepsPerInitialE;
      HistoMapMap_t histoMapMap_vol_trackLengthPerInitialE;
      HistoMapMap_t histoMapMap_vol_InitialE;

      // distributions per material per particle type
      HistoMapMap_t histoMapMap_mat_stepSize;
      HistoMapMap_t histoMapMap_mat_stepKineticEnergy;
      HistoMapMap_t histoMapMap_mat_postStepKineticEnergy;
      HistoMapMap_t histoMapMap_mat_stepPseudorapidity;
      HistoMapMap_t histoMapMap_mat_stepEnergyDeposit;
      HistoMapMap_t histoMapMap_mat_stepEnergyNonIonDeposit;
      HistoMapMap_t histoMapMap_mat_stepSecondaryKinetic;
      HistoMapMap_t histoMapMap_mat_numberOfSteps;
      HistoMapMap_t histoMapMap_mat_numberOfStepsPerInitialE;
      HistoMapMap_t histoMapMap_mat_trackLengthPerInitialE;
      HistoMapMap_t histoMapMap_mat_InitialE;

      // distributions per process per particle type
      HistoMapMap_t histoMapMap_prc_stepSize;
      HistoMapMap_t histoMapMap_prc_stepKineticEnergy;
      HistoMapMap_t histoMapMap_prc_postStepKineticEnergy;
      HistoMapMap_t histoMapMap_prc_stepPseudorapidity;
      HistoMapMap_t histoMapMap_prc_stepEnergyDeposit;
      HistoMapMap_t histoMapMap_prc_stepEnergyNonIonDeposit;
      HistoMapMap_t histoMapMap_prc_stepSecondaryKinetic;
      HistoMapMap_t histoMapMap_prc_numberOfSteps;
      HistoMapMap_t histoMapMap_prc_numberOfStepsPerInitialE;
      HistoMapMap_t histoMapMap_prc_trackLengthPerInitialE;
      HistoMapMap_t histoMapMap_prc_InitialE;

      // all atlas
      HistoMapMap_t histoMapMap_numberOfSteps;
      HistoMapMap_t histoMapMap_numberOfStepsPerInitialE;
      HistoMapMap_t histoMapMap_trackLengthPerInitialE;
      HistoMapMap_t histoMapMap_InitialE;
      HistoMapMap_t histoMapMap_stepKinetic;
      HistoMapMap_t histoMapMap_postStepKinetic;

      // 2D maps
      HistoMapMap_t histoMapMap2D_vol_RZ;
      HistoMapMap_t histoMapMap2D_mat_RZ;
      HistoMapMap_t histoMapMap2D_prc_RZ;

      // rather complicated function that merges two maps
      void mergeMaps(HistoMapMap_t &selfMap, HistoMapMap_t refMap);

      // function needed by ActionToolBaseReport base class
      void merge(const Report & rep);
    };

    // configurable properties
    struct Config
    {
      bool do2DHistograms = false;
      bool doGeneralHistograms = false;
    };

    /// ctor
    StepHistogram(const Config&);

    const Report& getReport() const { return m_report; }

  private:
    // report
    Report m_report;

    /// configuration data
    Config m_config;

    // initialize and fill histogram in a map
    void InitializeFillHistogram2D(HistoMapMap_t &hMapMap, const char* suffix,
                                                G4String pdgId, G4String vol,
                                                int nbinsx, double xmin, double xmax, 
                                                int nbinsy, double ymin, double ymax, 
                                                double valuex, double valuey, double weight);

    void InitializeFillHistogram(HistoMapMap_t &hMapMap, const char* suffix,
                                 G4String pdgId, G4String vol,
                                 int nbins, double xmin, double xmax, double value, double weight);

    void InitializeFillHistogram(HistoMapMap_t &hMapMap, const char* suffix,
                                 G4String pdgId, G4String vol,
                                 int nbins, double *edges, double value, double weight);

    float m_initialKineticEnergyOfStep;
    G4String m_initialVolume;
    G4String m_initialMaterial;
    G4String m_initialProcess;
    int m_trackID;

  }; // class StepHistogram
  
} // namespace G4UA 

#endif // G4DEBUGGINGTOOLS_StepHistogram_H
