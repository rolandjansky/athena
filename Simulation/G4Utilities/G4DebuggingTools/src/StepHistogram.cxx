/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "StepHistogram.h"

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"

#include "SimHelpers/ServiceAccessor.h"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VProcess.hh"

#include <iostream>

namespace G4UA{
  
  StepHistogram::StepHistogram(const Config& config):
    AthMessaging(Gaudi::svcLocator()->service<IMessageSvc>("MessageSvc"),"StepHistogram"),
    m_config(config),
    m_initialKineticEnergyOfStep()
  {}
  
  void StepHistogram::UserSteppingAction(const G4Step * aStep){

    // track
    G4Track *tr = aStep->GetTrack();

    // pre-step point
    G4StepPoint *PreStepPoint = aStep->GetPreStepPoint();

    // post-step point
    G4StepPoint *PostStepPoint = aStep->GetPostStepPoint();

    // pre-step kinetic energy
    double stepKinetic = PreStepPoint->GetKineticEnergy();

    // post-step kinetic energy
    double postStepKinetic = PostStepPoint->GetKineticEnergy();

    // pre-step position
    G4ThreeVector myPos = PreStepPoint->GetPosition();

    // particle name
    G4String particleName = "nucleus";
    if (!(tr->GetDefinition()->GetParticleType() == "nucleus"))
      particleName = G4DebuggingHelpers::ClassifyParticle(tr->GetParticleDefinition());

    // pre-step volume
    G4String volumeName = PreStepPoint->GetPhysicalVolume()->GetName();
    volumeName = G4DebuggingHelpers::ClassifyVolume(volumeName);

    // pre-step material
    G4String materialName = PreStepPoint->GetMaterial()->GetName();
    materialName = G4DebuggingHelpers::ClassifyMaterial(materialName);

    // process name (uses post-step point)
    G4String processName = PostStepPoint->GetProcessDefinedStep() ?
                           PostStepPoint->GetProcessDefinedStep()->GetProcessName() : "Unknown";

    // secondaries
    const std::vector<const G4Track*>* secondaries = aStep->GetSecondaryInCurrentStep();

    // 2D map
    if (m_config.do2DHistograms) {
      InitializeFillHistogram2D(m_report.histoMapMap2D_vol_RZ, "vol_RZ", particleName, volumeName,
                              1000, -20000, 20000, 1000, 0, 5000, myPos.getZ(), myPos.perp(), 1.);
      InitializeFillHistogram2D(m_report.histoMapMap2D_mat_RZ, "mat_RZ", particleName, materialName,
                              1000, -20000, 20000, 1000, 0, 5000, myPos.getZ(), myPos.perp(), 1.);
      InitializeFillHistogram2D(m_report.histoMapMap2D_prc_RZ, "prc_RZ", particleName, processName,
                              1000, -20000, 20000, 1000, 0, 5000, myPos.getZ(), myPos.perp(), 1.);
    }

    // step length
    InitializeFillHistogram(m_report.histoMapMap_vol_stepSize, "vol_stepLength", particleName, volumeName,
                            1000, -12, 4, log10(aStep->GetStepLength()), 1.);
    InitializeFillHistogram(m_report.histoMapMap_mat_stepSize, "mat_stepLength", particleName, materialName,
                            1000, -12, 4, log10(aStep->GetStepLength()), 1.);
    InitializeFillHistogram(m_report.histoMapMap_prc_stepSize, "prc_stepLength", particleName, processName,
                            1000, -12, 4, log10(aStep->GetStepLength()), 1.);

    // step pseudorapidity    
    InitializeFillHistogram(m_report.histoMapMap_vol_stepPseudorapidity, "vol_stepPseudorapidity", particleName, volumeName,
                            200, -10, 10, myPos.eta(), 1.);
    InitializeFillHistogram(m_report.histoMapMap_mat_stepPseudorapidity, "mat_stepPseudorapidity", particleName, materialName,
                            200, -10, 10, myPos.eta(), 1.);
    InitializeFillHistogram(m_report.histoMapMap_prc_stepPseudorapidity, "prc_stepPseudorapidity", particleName, processName,
                            200, -10, 10, myPos.eta(), 1.);

    // step kinetic energy
    InitializeFillHistogram(m_report.histoMapMap_vol_stepKineticEnergy, "vol_stepKineticEnergy", particleName, volumeName,
                            1000, -9, 7, log10(stepKinetic), 1.);
    InitializeFillHistogram(m_report.histoMapMap_mat_stepKineticEnergy, "mat_stepKineticEnergy", particleName, materialName,
                            1000, -9, 7, log10(stepKinetic), 1.);
    InitializeFillHistogram(m_report.histoMapMap_prc_stepKineticEnergy, "prc_stepKineticEnergy", particleName, processName,
                            1000, -9, 7, log10(stepKinetic), 1.);
    InitializeFillHistogram(m_report.histoMapMap_stepKinetic, "stepKineticEnergy", particleName, "AllATLAS",
                            1000, -9, 7, log10(stepKinetic), 1.);

    // post step kinetic energy
    InitializeFillHistogram(m_report.histoMapMap_vol_postStepKineticEnergy, "vol_postStepKineticEnergy", particleName, volumeName,
                            1000, -9, 7, log10(postStepKinetic), 1.);
    InitializeFillHistogram(m_report.histoMapMap_mat_postStepKineticEnergy, "mat_postStepKineticEnergy", particleName, materialName,
                            1000, -9, 7, log10(postStepKinetic), 1.);
    InitializeFillHistogram(m_report.histoMapMap_prc_postStepKineticEnergy, "prc_postStepKineticEnergy", particleName, processName,
                            1000, -9, 7, log10(postStepKinetic), 1.);
    InitializeFillHistogram(m_report.histoMapMap_postStepKinetic, "postStepKineticEnergy", particleName, "AllATLAS",
                            1000, -9, 7, log10(postStepKinetic), 1.);

    // step energy deposit
    InitializeFillHistogram(m_report.histoMapMap_vol_stepEnergyDeposit, "vol_stepEnergyDeposit", particleName, volumeName,
                            1000, -11, 3, log10(aStep->GetTotalEnergyDeposit()), 1.);
    InitializeFillHistogram(m_report.histoMapMap_mat_stepEnergyDeposit, "mat_stepEnergyDeposit", particleName, materialName,
                            1000, -11, 3, log10(aStep->GetTotalEnergyDeposit()), 1.);
    InitializeFillHistogram(m_report.histoMapMap_prc_stepEnergyDeposit, "prc_stepEnergyDeposit", particleName, processName,
                            1000, -11, 3, log10(aStep->GetTotalEnergyDeposit()), 1.);

    // step non-ionizing energy deposit
    InitializeFillHistogram(m_report.histoMapMap_vol_stepEnergyNonIonDeposit, "vol_stepEnergyNonIonDeposit", particleName, volumeName,
                            1000, -11, 1, log10(aStep->GetNonIonizingEnergyDeposit()), 1.);
    InitializeFillHistogram(m_report.histoMapMap_mat_stepEnergyNonIonDeposit, "mat_stepEnergyNonIonDeposit", particleName, materialName,
                            1000, -11, 1, log10(aStep->GetNonIonizingEnergyDeposit()), 1.);
    InitializeFillHistogram(m_report.histoMapMap_prc_stepEnergyNonIonDeposit, "prc_stepEnergyNonIonDeposit", particleName, processName,
                            1000, -11, 1, log10(aStep->GetNonIonizingEnergyDeposit()), 1.);

    // secondary kinetic energy
    for (const auto &track : *secondaries) {
      G4String secondary_particleName = G4DebuggingHelpers::ClassifyParticle(track->GetParticleDefinition());
      InitializeFillHistogram(m_report.histoMapMap_vol_stepSecondaryKinetic, "vol_stepSecondaryKinetic", secondary_particleName, volumeName,
                              1000, -7, 5, log10(track->GetKineticEnergy()), 1.);
      InitializeFillHistogram(m_report.histoMapMap_mat_stepSecondaryKinetic, "mat_stepSecondaryKinetic", secondary_particleName, materialName,
                              1000, -7, 5, log10(track->GetKineticEnergy()), 1.);
      InitializeFillHistogram(m_report.histoMapMap_prc_stepSecondaryKinetic, "prc_stepSecondaryKinetic", secondary_particleName, processName,
                              1000, -7, 5, log10(track->GetKineticEnergy()), 1.);
    }

    // stop here if 'general' histograms not activated
    // _______________________________________________
    if (!m_config.doGeneralHistograms)
      return;

    // first step (after initial step)
    if (tr->GetCurrentStepNumber()==1) {
      // initial kinetic energy
      m_initialKineticEnergyOfStep = stepKinetic;

      // save track ID for checking if we later have the same track
      m_trackID = tr->GetTrackID();

      // initial energy
      InitializeFillHistogram(m_report.histoMapMap_InitialE, "InitialE", particleName, "AllATLAS",
                              1000, -9, 7, log10(m_initialKineticEnergyOfStep), 1.0);
    }

    // last step
    if ( tr->GetTrackStatus() == 2 ) {
      // assert to check if we have the correct track
      if (not (tr->GetTrackID() == m_trackID)) {
        ATH_MSG_ERROR("Track ID changed between the assumed first step and the last.");
        throw std::exception();
      }
      // number of steps
      int nSteps = tr->GetCurrentStepNumber() + 1;
      InitializeFillHistogram(m_report.histoMapMap_numberOfSteps, "numberOfSteps", particleName, "AllATLAS",
                              10000, 0.5, 10000.5, nSteps, 1.);
      // number of steps vs initial energy
      InitializeFillHistogram(m_report.histoMapMap_numberOfStepsPerInitialE, "numberOfStepsPerInitialE", particleName, "AllATLAS",
                              1000, -9, 7, log10(m_initialKineticEnergyOfStep), nSteps);
    }
  }

  void StepHistogram::InitializeFillHistogram2D(HistoMapMap_t &hMapMap, const char* suffix,
                                                G4String particleName, G4String vol,
                                                int nbinsx, double xmin, double xmax, 
                                                int nbinsy, double ymin, double ymax, 
                                                double valuex, double valuey, double weight)
  {
    if ( hMapMap.find(vol) == hMapMap.end() ) {
      // initialize HistoMap_t if not yet exist
      hMapMap.emplace(vol,HistoMap_t());
    }
    HistoMap_t &hMap = hMapMap[vol];
    if ( hMap.find(particleName) == hMap.end() ) {
      // initialize histogram if not yet exist
      std::ostringstream stringStream;
      stringStream << vol << "_" << particleName << "_" << suffix;
      hMap[particleName] = new TH2F(stringStream.str().c_str(), stringStream.str().c_str(), nbinsx, xmin, xmax, nbinsy, ymin, ymax);
    }
    ((TH2*)hMap[particleName])->Fill(valuex, valuey, weight);
  }

  void StepHistogram::InitializeFillHistogram(HistoMapMap_t &hMapMap, const char* suffix,
                                              G4String particleName, G4String vol,
                                              int nbins, double xmin, double xmax, double value, double weight)
  {
    if ( hMapMap.find(vol) == hMapMap.end() ) {
      // initialize HistoMap_t if not yet exist
      hMapMap.emplace(vol,HistoMap_t());
    }
    HistoMap_t &hMap = hMapMap[vol];
    if ( hMap.find(particleName) == hMap.end() ) {
      // initialize histogram if not yet exist
      std::ostringstream stringStream;
      stringStream << vol << "_" << particleName << "_" << suffix;
      hMap[particleName] = new TH1F(stringStream.str().c_str(), stringStream.str().c_str(), nbins, xmin, xmax);
    }
    hMap[particleName]->Fill(value, weight);
  }

  void StepHistogram::InitializeFillHistogram(HistoMapMap_t &hMapMap, const char* suffix,
                                              G4String particleName, G4String vol,
                                              int nbins, double *edges, double value, double weight)
  {
    if ( hMapMap.find(vol) == hMapMap.end() ) {
      // initialize HistoMap_t if not yet exist
      hMapMap.emplace(vol,HistoMap_t());
    }
    HistoMap_t &hMap = hMapMap[vol];
    if ( hMap.find(particleName) == hMap.end() ) {
      // initialize histogram if not yet exist
      std::ostringstream stringStream;
      stringStream << vol << "_" << particleName << "_" << suffix;
      hMap[particleName] = new TH1F(stringStream.str().c_str(), stringStream.str().c_str(), nbins, edges);
    }
    hMap[particleName]->Fill(value, weight);
  }

  void StepHistogram::Report::mergeMaps(HistoMapMap_t &selfMap, HistoMapMap_t refMap) {
    for (auto const& ref : refMap)
    {
      if ( selfMap.find(ref.first) == selfMap.end() ) {
        // HistoMap_t does not yet exist
        selfMap.emplace(ref.first, ref.second);
      }
      else {
        HistoMap_t &target = selfMap[ref.first];
        for (auto const& hm : ref.second)
        {
          if ( target.find(hm.first) == target.end() ) {
            // histogram does not yet exist
            target.emplace(hm.first, hm.second);
          }
          else {
            // add histograms
            target[hm.first]->Add(hm.second);
          }
        }
      }
    }
  }

  void StepHistogram::Report::merge(const Report & rep) {
    mergeMaps(histoMapMap_vol_stepSize, rep.histoMapMap_vol_stepSize);
    mergeMaps(histoMapMap_vol_stepKineticEnergy, rep.histoMapMap_vol_stepKineticEnergy);
    mergeMaps(histoMapMap_vol_postStepKineticEnergy, rep.histoMapMap_vol_postStepKineticEnergy);
    mergeMaps(histoMapMap_vol_stepPseudorapidity, rep.histoMapMap_vol_stepPseudorapidity);
    mergeMaps(histoMapMap_vol_stepEnergyDeposit, rep.histoMapMap_vol_stepEnergyDeposit);
    mergeMaps(histoMapMap_vol_stepEnergyNonIonDeposit, rep.histoMapMap_vol_stepEnergyNonIonDeposit);
    mergeMaps(histoMapMap_vol_stepSecondaryKinetic, rep.histoMapMap_vol_stepSecondaryKinetic);

    mergeMaps(histoMapMap_mat_stepSize, rep.histoMapMap_mat_stepSize);
    mergeMaps(histoMapMap_mat_stepKineticEnergy, rep.histoMapMap_mat_stepKineticEnergy);
    mergeMaps(histoMapMap_mat_postStepKineticEnergy, rep.histoMapMap_mat_postStepKineticEnergy);
    mergeMaps(histoMapMap_mat_stepPseudorapidity, rep.histoMapMap_mat_stepPseudorapidity);
    mergeMaps(histoMapMap_mat_stepEnergyDeposit, rep.histoMapMap_mat_stepEnergyDeposit);
    mergeMaps(histoMapMap_mat_stepEnergyNonIonDeposit, rep.histoMapMap_mat_stepEnergyNonIonDeposit);
    mergeMaps(histoMapMap_mat_stepSecondaryKinetic, rep.histoMapMap_mat_stepSecondaryKinetic);

    mergeMaps(histoMapMap_prc_stepSize, rep.histoMapMap_prc_stepSize);
    mergeMaps(histoMapMap_prc_stepKineticEnergy, rep.histoMapMap_prc_stepKineticEnergy);
    mergeMaps(histoMapMap_prc_postStepKineticEnergy, rep.histoMapMap_prc_postStepKineticEnergy);
    mergeMaps(histoMapMap_prc_stepPseudorapidity, rep.histoMapMap_prc_stepPseudorapidity);
    mergeMaps(histoMapMap_prc_stepEnergyDeposit, rep.histoMapMap_prc_stepEnergyDeposit);
    mergeMaps(histoMapMap_prc_stepEnergyNonIonDeposit, rep.histoMapMap_prc_stepEnergyNonIonDeposit);
    mergeMaps(histoMapMap_prc_stepSecondaryKinetic, rep.histoMapMap_prc_stepSecondaryKinetic);

    mergeMaps(histoMapMap_numberOfSteps, rep.histoMapMap_numberOfSteps);
    mergeMaps(histoMapMap_numberOfStepsPerInitialE, rep.histoMapMap_numberOfStepsPerInitialE);
    mergeMaps(histoMapMap_InitialE, rep.histoMapMap_InitialE);
    mergeMaps(histoMapMap_stepKinetic, rep.histoMapMap_stepKinetic);
    mergeMaps(histoMapMap_postStepKinetic, rep.histoMapMap_postStepKinetic);

    mergeMaps(histoMapMap2D_vol_RZ, rep.histoMapMap2D_vol_RZ);
    mergeMaps(histoMapMap2D_mat_RZ, rep.histoMapMap2D_mat_RZ);
    mergeMaps(histoMapMap2D_prc_RZ, rep.histoMapMap2D_prc_RZ);
  }

} // namespace G4UA 
