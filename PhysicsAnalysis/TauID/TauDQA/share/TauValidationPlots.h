#ifndef MUONPHYSVALMONITORING_MUONVALIDATIONPLOTS_H
#define MUONPHYSVALMONITORING_MUONVALIDATIONPLOTS_H

// PlotBase objects
#include "TauHistUtils/ParamPlots.h"
#include "TauHistUtils/GeneralTauPlots.h"
#include "TauHistUtils/Had1ProngPlots.h"
#include "TauHistUtils/Had3ProngPlots.h"
#include "TauHistUtils/EVetoPlots.h"
#include "TauHistUtils/RecoTauPlots.h"
#include "TauHistUtils/NewCorePlots.h"
#include "TauHistUtils/BDTinputPlots.h"
#include "TauHistUtils/Migration.h"
#include "TauHistUtils/EfficiencyPtPlots.h"

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

      // All tau BDT scrore and input variables
  //    Tau::BDTinputPlots m_oBDTinputPlotsRec;

   //   Tau::BDTinputPlots m_oBDTinputPlotsMatched;

    //  Tau::BDTinputPlots m_oBDTinputPlotsFakes;

     // Tau::BDTinputPlots m_oBDTinputPlotsElec;

      //Migration Matrix plots
      Tau::Migration m_oMigrationPlots;		


};

#endif
