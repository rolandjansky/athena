// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef TAUDQA_TAUVALIDATIONPLOTS_H
#define TAUDQA_TAUVALIDATIONPLOTS_H

// PlotBase objects
#include "ParamPlots.h"
#include "GeneralTauPlots.h"
#include "Had1ProngPlots.h"
#include "Had3ProngPlots.h"
#include "EVetoPlots.h"
#include "RecoTauPlots.h"
#include "NewCorePlots.h"
#include "Migration.h"
#include "EfficiencyPtPlots.h"

#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODTau/TauJetContainer.h" 

class TauValidationPlots:public PlotBase {
    public:
      TauValidationPlots(PlotBase* pParent, std::string sDir, std::string sTauJetContainerName);
      // general tau all prongs plots
      Tau::GeneralTauPlots m_oGeneralTauAllProngsPlots;


    	// tau1P plots : variables for tau ID and EVeto
      Tau::Had1ProngPlots m_oHad1ProngPlots;

      // tau3P plots : variables for tau ID
      Tau::Had3ProngPlots m_oHad3ProngPlots;

      // electron veto variables for electrons matching tau candidates	
      Tau::ParamPlots      m_oElMatchedParamPlots;
      Tau::EVetoPlots      m_oElMatchedEVetoPlots;

      // general tau all fake prongs plots
      Tau::GeneralTauPlots m_oFakeGeneralTauAllProngsPlots;


    	// tau1P fake plots : variables for tau ID
      Tau::Had1ProngPlots  m_oFakeHad1ProngPlots;		      

      // tau3P fake plots : variables for tau ID
      Tau::Had3ProngPlots  m_oFakeHad3ProngPlots;	        

      // general tau all fake prongs plots
      Tau::GeneralTauPlots m_oRecoGeneralTauAllProngsPlots;
	
    	// tau1P fake plots : variables for tau ID
      Tau::Had1ProngPlots  m_oRecoHad1ProngPlots;		      
   //   Tau::EVetoPlots     m_oRecoHad1ProngEVetoPlots;

      // tau3P fake plots : variables for tau ID
      Tau::Had3ProngPlots  m_oRecoHad3ProngPlots;	        

      // All tau Reco with no match to truth
      Tau::RecoTauPlots m_oRecoTauAllProngsPlots;		

      Tau::RecoTauPlots m_oMatchedTauAllProngsPlots;
      Tau::RecoTauPlots m_oFakeTauAllProngsPlots;

      //Efficiency plots
      Tau::EfficiencyPtPlots m_oMatchedTauEffPlots;
      Tau::EfficiencyPtPlots m_oMatchedTauEff1PPlots;
      Tau::EfficiencyPtPlots m_oMatchedTauEff3PPlots;

      Tau::EfficiencyPtPlots m_oRecTauEffPlots;
      Tau::EfficiencyPtPlots m_oRecTauEff1PPlots;
      Tau::EfficiencyPtPlots m_oRecTauEff3PPlots;

      Tau::EfficiencyPtPlots m_oFakeTauEffPlots;
      Tau::EfficiencyPtPlots m_oFakeTauEff1PPlots;
      Tau::EfficiencyPtPlots m_oFakeTauEff3PPlots;

      // All tau Reco with Backwards compatability, for comparison with 17.X.Y
      Tau::NewCorePlots m_oNewCorePlots;		

      Tau::NewCorePlots m_oNewCoreMatchedPlots;
      Tau::NewCorePlots m_oNewCoreFakePlots;

      //Migration Matrix plots
      Tau::Migration m_oMigrationPlots;		

      // Plots with the "nominal" tau selection
      Tau::GeneralTauPlots m_oFakeGeneralNom;
      Tau::Had1ProngPlots m_oFakeHad1ProngNom;
      Tau::Had3ProngPlots m_oFakeHad3ProngNom;
      Tau::EfficiencyPtPlots m_oFakeTauEffPlotsNom;
      Tau::EfficiencyPtPlots m_oFakeTauEff1PPlotsNom;
      Tau::EfficiencyPtPlots m_oFakeTauEff3PPlotsNom;
      Tau::RecoTauPlots m_oFakeTauRecoTauPlotsNom;
      Tau::NewCorePlots m_oNewCoreFakePlotsNom;
      
      Tau::GeneralTauPlots m_oRecoGeneralNom;
      Tau::Had1ProngPlots m_oRecoHad1ProngNom;
      Tau::Had3ProngPlots m_oRecoHad3ProngNom;
      Tau::EfficiencyPtPlots m_oRecTauEffPlotsNom;
      Tau::EfficiencyPtPlots m_oRecTauEff1PPlotsNom;
      Tau::EfficiencyPtPlots m_oRecTauEff3PPlotsNom;
      Tau::RecoTauPlots m_oRecTauRecoTauPlotsNom;
      Tau::NewCorePlots m_oNewCoreRecTauPlotsNom;
      
      Tau::GeneralTauPlots m_oMatchedGeneralNom;
      Tau::Had1ProngPlots m_oMatchedHad1ProngNom;
      Tau::Had3ProngPlots m_oMatchedHad3ProngNom;
      Tau::EfficiencyPtPlots m_oMatchedTauEffPlotsNom;
      Tau::EfficiencyPtPlots m_oMatchedTauEff1PPlotsNom;
      Tau::EfficiencyPtPlots m_oMatchedTauEff3PPlotsNom;
      Tau::RecoTauPlots m_oMatchedTauRecoTauPlotsNom;
      Tau::Migration m_oMigrationPlotsNom;
      Tau::NewCorePlots m_oNewCoreMatchedPlotsNom;


};

#endif // not TAUDQA_TAUVALIDATIONPLOTS_H
