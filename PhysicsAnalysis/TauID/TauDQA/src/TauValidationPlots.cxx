/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TauValidationPlots.h"

TauValidationPlots::TauValidationPlots(PlotBase* pParent, const std::string& sDir, const std::string& sTauJetContainerName):
  PlotBase(pParent, sDir),
  // Plots with the "primitive" tau selection
  m_oGeneralTauAllProngsPlots(this, "NoCuts/Matched/", sTauJetContainerName),
  m_oHad1ProngPlots(this, "NoCuts/Matched/Tau1P/", sTauJetContainerName),             // tau1P plots : variables for tau ID
  m_oHad3ProngPlots(this, "NoCuts/Matched/Tau3P/", sTauJetContainerName),             // tau3P plots : variables for tau ID
  m_oElMatchedParamPlots(this, "Electron/", sTauJetContainerName),     // electron veto variables for electrons matching tau candidates	  
  m_oElMatchedEVetoPlots(this, "Electron/", sTauJetContainerName),     // electron veto variables for electrons matching tau candidates
  m_oFakeGeneralTauAllProngsPlots(this,"NoCuts/Fake/", sTauJetContainerName),  // general tau all fake prongs plots
  m_oFakeHad1ProngPlots(this,"NoCuts/Fake/Jet1P/", sTauJetContainerName), 		     // tau1P fake plots : variables for tau ID
  m_oFakeHad3ProngPlots(this,"NoCuts/Fake/Jet3P/", sTauJetContainerName),  	       // tau3P fake plots : variables for tau ID
  m_oRecoGeneralTauAllProngsPlots(this,"NoCuts/RecTau/",sTauJetContainerName),// "recTau_General"),  // general tau all fake prongs plots
  m_oRecoHad1ProngPlots(this,"NoCuts/RecTau/1P/", sTauJetContainerName),//"recTau_1P"), 		     // tau1P fake plots : variables for tau ID
  m_oRecoHad3ProngPlots(this,"NoCuts/RecTau/3P/", sTauJetContainerName),// "recTau_3P"),  	       // tau3P fake plots : variables for tau ID
  m_oRecoTauAllProngsPlots(this,"NoCuts/RecTau/PFOs/", sTauJetContainerName),             // all tau reco, no match to truth
  m_oMatchedTauAllProngsPlots(this,"NoCuts/Matched/PFOs/", sTauJetContainerName),
  m_oFakeTauAllProngsPlots(this,"NoCuts/Fake/PFOs/", sTauJetContainerName),             // all tau reco, no match to truth
  m_oMatchedTauEffPlots  (this,"NoCuts/Matched/Eff/All/", sTauJetContainerName),
  m_oMatchedTauEff1PPlots(this,"NoCuts/Matched/Eff/Tau1P/", sTauJetContainerName),
  m_oMatchedTauEff3PPlots(this,"NoCuts/Matched/Eff/Tau3P/", sTauJetContainerName),
  m_oRecTauEffPlots  (this,"NoCuts/RecTau/Eff/All/", sTauJetContainerName),
  m_oRecTauEff1PPlots(this,"NoCuts/RecTau/Eff/Tau1P/", sTauJetContainerName),
  m_oRecTauEff3PPlots(this,"NoCuts/RecTau/Eff/Tau3P/", sTauJetContainerName),
  m_oFakeTauEffPlots  (this,"NoCuts/Fake/Eff/All/", sTauJetContainerName),
  m_oFakeTauEff1PPlots(this,"NoCuts/Fake/Eff/Tau1P/", sTauJetContainerName),
  m_oFakeTauEff3PPlots(this,"NoCuts/Fake/Eff/Tau3P/", sTauJetContainerName),
  m_oNewCorePlots(this,"NoCuts/RecTau/All/", sTauJetContainerName),             // all tau reco, newCore variables
  m_oNewCoreMatchedPlots(this,"NoCuts/Matched/All/", sTauJetContainerName),             // all tau reco, newCore variables
  m_oNewCoreFakePlots(this,"NoCuts/Fake/All/", sTauJetContainerName),             // all tau reco, newCore variables
  m_oMigrationPlots(this,"NoCuts/Matched/Migration/", sTauJetContainerName),             // Migration Matrix
  
  // Plots with the "nominal" tau selection
  m_oFakeGeneralNom(this,"Nominal/Fake/", sTauJetContainerName),
  m_oFakeHad1ProngNom(this,"Nominal/Fake/Jet1P/", sTauJetContainerName),
  m_oFakeHad3ProngNom(this,"Nominal/Fake/Jet3P/", sTauJetContainerName),
  m_oFakeTauEffPlotsNom(this, "Nominal/Fake/Eff/All/", sTauJetContainerName),
  m_oFakeTauEff1PPlotsNom(this, "Nominal/Fake/Eff/Jet1P/", sTauJetContainerName),
  m_oFakeTauEff3PPlotsNom(this, "Nominal/Fake/Eff/Jet3P/", sTauJetContainerName),
  m_oFakeTauRecoTauPlotsNom(this, "Nominal/Fake/PFOs/", sTauJetContainerName),
  m_oNewCoreFakePlotsNom(this, "Nominal/Fake/All/", sTauJetContainerName),

  m_oRecoGeneralNom(this,"Nominal/RecTau/", sTauJetContainerName),
  m_oRecoHad1ProngNom(this,"Nominal/RecTau/1P/", sTauJetContainerName),
  m_oRecoHad3ProngNom(this,"Nominal/RecTau/3P/", sTauJetContainerName),
  m_oRecTauEffPlotsNom(this, "Nominal/RecTau/Eff/All/", sTauJetContainerName),
  m_oRecTauEff1PPlotsNom(this, "Nominal/RecTau/Eff/1P/", sTauJetContainerName),
  m_oRecTauEff3PPlotsNom(this, "Nominal/RecTau/Eff/3P/", sTauJetContainerName),
  m_oRecTauRecoTauPlotsNom(this, "Nominal/RecTau/PFOs/", sTauJetContainerName),
  m_oNewCoreRecTauPlotsNom(this, "Nominal/RecTau/All/", sTauJetContainerName),

  m_oMatchedGeneralNom(this,"Nominal/Matched/", sTauJetContainerName),
  m_oMatchedHad1ProngNom(this,"Nominal/Matched/Tau1P/", sTauJetContainerName),
  m_oMatchedHad3ProngNom(this,"Nominal/Matched/Tau3P/", sTauJetContainerName),
  m_oMatchedTauEffPlotsNom(this, "Nominal/Matched/Eff/All/", sTauJetContainerName),
  m_oMatchedTauEff1PPlotsNom(this, "Nominal/Matched/Eff/1P/", sTauJetContainerName),
  m_oMatchedTauEff3PPlotsNom(this, "Nominal/Matched/Eff/3P/", sTauJetContainerName),
  m_oMatchedTauRecoTauPlotsNom(this, "Nominal/Matched/PFOs/", sTauJetContainerName),
  m_oMigrationPlotsNom(this, "Nominal/Matched/Migration/", sTauJetContainerName),
  m_oNewCoreMatchedPlotsNom(this, "Nominal/Matched/All/", sTauJetContainerName)
{}	

// no fill method implement in order to let filling logic stay in the ManagedMonitoringTool
