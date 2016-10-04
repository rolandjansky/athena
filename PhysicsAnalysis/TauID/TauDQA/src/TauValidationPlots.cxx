/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "../share/TauValidationPlots.h"

TauValidationPlots::TauValidationPlots(PlotBase* pParent, std::string sDir, std::string sTauJetContainerName):
  PlotBase(pParent, sDir),
  m_oGeneralTauAllProngsPlots(this, "Matched/", sTauJetContainerName),
  m_oHad1ProngPlots(this, "Matched/Tau1P/", sTauJetContainerName),             // tau1P plots : variables for tau ID
  m_oHad3ProngPlots(this, "Matched/Tau3P/", sTauJetContainerName),             // tau3P plots : variables for tau ID
  m_oElMatchedParamPlots(this, "Electron/", sTauJetContainerName),     // electron veto variables for electrons matching tau candidates	  
  m_oElMatchedEVetoPlots(this, "Electron/", sTauJetContainerName),     // electron veto variables for electrons matching tau candidates
  m_oFakeGeneralTauAllProngsPlots(this,"Fake/", sTauJetContainerName),  // general tau all fake prongs plots
  m_oFakeHad1ProngPlots(this,"Fake/Jet1P/", sTauJetContainerName), 		     // tau1P fake plots : variables for tau ID
  m_oFakeHad3ProngPlots(this,"Fake/Jet3P/", sTauJetContainerName),  	       // tau3P fake plots : variables for tau ID
  m_oRecoGeneralTauAllProngsPlots(this,"RecTau/All/",sTauJetContainerName),// "recTau_General"),  // general tau all fake prongs plots
  m_oRecoHad1ProngPlots(this,"RecTau/1P/", sTauJetContainerName),//"recTau_1P"), 		     // tau1P fake plots : variables for tau ID
  m_oRecoHad3ProngPlots(this,"RecTau/3P/", sTauJetContainerName),// "recTau_3P"),  	       // tau3P fake plots : variables for tau ID
  m_oRecoTauAllProngsPlots(this,"RecTau/PFOs/", sTauJetContainerName),             // all tau reco, no match to truth
  m_oMatchedTauAllProngsPlots(this,"Matched/PFOs/", sTauJetContainerName),
  m_oFakeTauAllProngsPlots(this,"Fake/PFOs/", sTauJetContainerName),             // all tau reco, no match to truth
  m_oMatchedTauEffPlots  (this,"Matched/Eff/All/", sTauJetContainerName),
  m_oMatchedTauEff1PPlots(this,"Matched/Eff/Tau1P/", sTauJetContainerName),
  m_oMatchedTauEff3PPlots(this,"Matched/Eff/Tau3P/", sTauJetContainerName),
  m_oRecTauEffPlots  (this,"RecTau/Eff/All/", sTauJetContainerName),
  m_oRecTauEff1PPlots(this,"RecTau/Eff/Tau1P/", sTauJetContainerName),
  m_oRecTauEff3PPlots(this,"RecTau/Eff/Tau3P/", sTauJetContainerName),
  m_oFakeTauEffPlots  (this,"Fake/Eff/All/", sTauJetContainerName),
  m_oFakeTauEff1PPlots(this,"Fake/Eff/Tau1P/", sTauJetContainerName),
  m_oFakeTauEff3PPlots(this,"Fake/Eff/Tau3P/", sTauJetContainerName),
  m_oNewCorePlots(this,"RecTau/All/", sTauJetContainerName),             // all tau reco, newCore variables
  m_oNewCoreMatchedPlots(this,"Matched/All/", sTauJetContainerName),             // all tau reco, newCore variables
  m_oNewCoreFakePlots(this,"Fake/All/", sTauJetContainerName),             // all tau reco, newCore variables
  m_oMigrationPlots(this,"Matched/Migration/", sTauJetContainerName)             // Migration Matrix
{}	

// no fill method implement in order to let filling logic stay in the ManagedMonitoringTool
