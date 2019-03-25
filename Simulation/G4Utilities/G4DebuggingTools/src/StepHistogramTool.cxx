/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "StepHistogramTool.h"

namespace G4UA{ 

  StepHistogramTool::StepHistogramTool(const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent):
    UserActionToolBase<StepHistogram>(type, name, parent),
    m_config(),
    m_histSvc("THistSvc", name){

    declareProperty("do2DHistograms", m_config.do2DHistograms);
    declareProperty("doGeneralHistograms", m_config.doGeneralHistograms);
  }

  std::unique_ptr<StepHistogram>
  StepHistogramTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a TestActionTimer");
    auto action = std::make_unique<StepHistogram>(m_config);
    actionList.runActions.push_back( action.get() );
    actionList.eventActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

  StatusCode StepHistogramTool::initialize(){
    ATH_CHECK(m_histSvc.retrieve());
    return StatusCode::SUCCESS;
  }

  void StepHistogramTool::BookHistograms(StepHistogram::HistoMapMap_t &hMap, const char* suffix, const char* subfolder)
  {
    for (auto const& x : hMap)
    {
      ATH_MSG_INFO("Currently in volume:\t" << x.first << " got histoMap " << x.second);
      for (auto const& hm : x.second)
      {
        ATH_MSG_INFO("Registering histogram:\t" << hm.first);
        std::ostringstream stringStream;
        stringStream << "/stepHisto/" << subfolder << x.first << "/" << suffix << hm.first;
        if ( m_histSvc->regHist(stringStream.str().c_str(), hm.second).isFailure() ) {
          ATH_MSG_ERROR("Could not register histogram!");
        }
      }
    }    
  }

  StatusCode StepHistogramTool::finalize() {
    ATH_MSG_INFO("Preparing histograms...");

    // Accumulate results across threads
    StepHistogram::Report report;
    m_actions.accumulate(report, &StepHistogram::getReport,
                         &StepHistogram::Report::merge);

    if (m_histSvc) {
      BookHistograms(report.histoMapMap_vol_stepSize, "stepLength/", "volumes/");
      BookHistograms(report.histoMapMap_vol_stepKineticEnergy, "stepKineticEnergy/", "volumes/");
      BookHistograms(report.histoMapMap_vol_postStepKineticEnergy, "postStepKineticEnergy/", "volumes/");
      BookHistograms(report.histoMapMap_vol_stepPseudorapidity, "stepPseudorapidity/", "volumes/");
      BookHistograms(report.histoMapMap_vol_stepEnergyDeposit, "stepEnergyDeposit/", "volumes/");
      BookHistograms(report.histoMapMap_vol_stepEnergyNonIonDeposit, "stepEnergyNonIonDeposit/", "volumes/");
      BookHistograms(report.histoMapMap_vol_stepSecondaryKinetic, "stepSecondaryKinetic/", "volumes/");
      BookHistograms(report.histoMapMap_vol_trackLengthPerInitialE, "trackLengthPerInitialE/", "volumes/");
      BookHistograms(report.histoMapMap_vol_InitialE, "InitialE/", "volumes/");
      BookHistograms(report.histoMapMap_vol_numberOfSteps, "numberOfSteps/", "volumes/");
      BookHistograms(report.histoMapMap_vol_numberOfStepsPerInitialE, "numberOfStepsPerInitialE/", "volumes/");

      BookHistograms(report.histoMapMap_mat_stepSize, "stepLength/", "materials/");
      BookHistograms(report.histoMapMap_mat_stepKineticEnergy, "stepKineticEnergy/", "materials/");
      BookHistograms(report.histoMapMap_mat_stepPseudorapidity, "stepPseudorapidity/", "materials/");
      BookHistograms(report.histoMapMap_mat_postStepKineticEnergy, "postStepKineticEnergy/", "materials/");
      BookHistograms(report.histoMapMap_mat_stepEnergyDeposit, "stepEnergyDeposit/", "materials/");
      BookHistograms(report.histoMapMap_mat_stepEnergyNonIonDeposit, "stepEnergyNonIonDeposit/", "materials/");
      BookHistograms(report.histoMapMap_mat_stepSecondaryKinetic, "stepSecondaryKinetic/", "materials/");
      BookHistograms(report.histoMapMap_mat_trackLengthPerInitialE, "trackLengthPerInitialE/", "materials/");
      BookHistograms(report.histoMapMap_mat_InitialE, "InitialE/", "materials/");
      BookHistograms(report.histoMapMap_mat_numberOfSteps, "numberOfSteps/", "materials/");
      BookHistograms(report.histoMapMap_mat_numberOfStepsPerInitialE, "numberOfStepsPerInitialE/", "materials/");

      BookHistograms(report.histoMapMap_prc_stepSize, "stepLength/", "processes/");
      BookHistograms(report.histoMapMap_prc_stepKineticEnergy, "stepKineticEnergy/", "processes/");
      BookHistograms(report.histoMapMap_prc_stepPseudorapidity, "stepPseudorapidity/", "processes/");
      BookHistograms(report.histoMapMap_prc_postStepKineticEnergy, "postStepKineticEnergy/", "processes/");
      BookHistograms(report.histoMapMap_prc_stepEnergyDeposit, "stepEnergyDeposit/", "processes/");
      BookHistograms(report.histoMapMap_prc_stepEnergyNonIonDeposit, "stepEnergyNonIonDeposit/", "processes/");
      BookHistograms(report.histoMapMap_prc_stepSecondaryKinetic, "stepSecondaryKinetic/", "processes/");
      BookHistograms(report.histoMapMap_prc_trackLengthPerInitialE, "trackLengthPerInitialE/", "processes/");
      BookHistograms(report.histoMapMap_prc_InitialE, "InitialE/", "processes/");
      BookHistograms(report.histoMapMap_prc_numberOfSteps, "numberOfSteps/", "processes/");
      BookHistograms(report.histoMapMap_prc_numberOfStepsPerInitialE, "numberOfStepsPerInitialE/", "processes/");

      if (m_config.doGeneralHistograms) {
        BookHistograms(report.histoMapMap_numberOfSteps, "numberOfSteps/", "nSteps/");
        BookHistograms(report.histoMapMap_numberOfStepsPerInitialE, "numberOfStepsPerInitialE/", "nSteps/");
        BookHistograms(report.histoMapMap_trackLengthPerInitialE, "trackLengthPerInitialE/", "nSteps/");
        BookHistograms(report.histoMapMap_InitialE, "InitialE/", "nSteps/");
        BookHistograms(report.histoMapMap_stepKinetic, "stepKineticEnergy/", "nSteps/");
        BookHistograms(report.histoMapMap_postStepKinetic, "postStepKineticEnergy/", "nSteps/");
      }

      if (m_config.do2DHistograms) {
        BookHistograms(report.histoMapMap2D_vol_RZ, "2DMaps/", "volumes/");
        BookHistograms(report.histoMapMap2D_mat_RZ, "2DMaps/", "materials/");
        BookHistograms(report.histoMapMap2D_prc_RZ, "2DMaps/", "processes/");
      }
    }
    else {
      ATH_MSG_WARNING("HistSvc not initialized...");      
    }

    return StatusCode::SUCCESS;
  }

} // namespace G4UA 
