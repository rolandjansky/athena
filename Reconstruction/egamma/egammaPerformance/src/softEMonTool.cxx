/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"


#include "AthenaMonitoring/AthenaMonManager.h"
#include "egammaPerformance/softEMonTool.h"

#include "TrigSteeringEvent/Enums.h"


#include "egammaEvent/egamma.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronContainer.h"
/********************************************************************
NAME:     softEMonTool.cxx
PACKAGE:  offline/Reconstruction/egamma/egammaPerformance

AUTHORS:  Olivier Arnaez, Fred Derue
CREATED:  code adapted from ElectronMonTool developed by K. Benslama, Y. Ming and E. Ortega from Univ. of Regina 

PURPOSE: softe electron offline monitoring tool

UPDATED:
        Feb. 06, 2009 (FD) better calculation of E/p
        Aug. 17, 2009 (OA) added A0, Weta2, Weta1, Wtot, pos7, Number of TRT hits with Outliers, Number of high threshold TRT hits with Outliers,
         added f3core, f1core
        Nov. 09, 2009 (FD) add protections + comments
********************************************************************/
#include "egammaEvent/EMConvert.h"
#include "egammaEvent/EMShower.h"
#include "egammaEvent/EMTrackMatch.h"

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egDetailContainer.h"
#include "egammaEvent/egDetail.h"

#include "CaloEvent/CaloClusterContainer.h"
//-REMI #include "CaloEvent/CaloSampling.h"
//+REMI
#include "CaloGeoHelpers/CaloSampling.h"

#include <sstream>

#include "TH2F.h"
#include "TH1F.h"
#include "TProfile.h"


//constructor
softEMonTool::softEMonTool( const std::string & type, const std::string & name,
	       const IInterface* parent )
  :IEgammaMonTool( type, name, parent ),
   m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool") 
{
  // Name of the electron collection
  declareProperty("ElectronContainer", 
		  m_ElectronCnt = "ElectronCollection",
		  "Name of the electron collection");
  // Name of the egDetail for cluster-track matching
  declareProperty("EMTrackMatchContainer",
		  m_EMTrackMatchCnt = 	"egDetailContainer",
		  "Name of the egDetail for cluster-track matching");
  // Name of the egDetail for shower
  declareProperty("EMShowerContainer", 
		  m_EMShowerCnt = "egDetailContainer",
		  "Name of the egDetail for shower");
  // Name of the egDetail for conversion
  declareProperty("EMConvertContainer",
		  m_EMConvertCnt ="egDetailContainer",
		  "Name of the egDetail for conversion");
  // Boolean to use trigger information
  declareProperty("UseTrigger", 
		  m_UseTrigger=true,
		  "Boolean to use trigger information");
  // Items for the trigger selection
  declareProperty("Electron_Trigger_Items",
		  m_e_triggers,
		  "Items for the trigger selection");
  // Items for the electron selection
  declareProperty("Electron_Selection_Items", 
		  m_e_selections,
		  "Items for the electron selection");
  // Items for the electron division
  declareProperty("Electron_Divison_Items", 
		  m_e_decdivs,
		  "Items for the electron division");
  // Name of the tool for the trigger decision
  declareProperty("TrigDecisionTool", 
		  m_trigDec, 
		  "Tool to access the trigger decision");
 
}

// ===========================================================
//destructor
softEMonTool::~softEMonTool()
{
}

// ===========================================================
StatusCode softEMonTool::bookElectronHistosDec(const std::string & groupName, const std::string & decName) 
{
  //
  // Booking of histograms
  //

  MsgStream log( msgSvc(), name() );  
  ATH_MSG_DEBUG("softEMonTool::bookElectronHistos(" + groupName + ", " + decName + ")");

  ///Generic electron properties
    ATH_MSG_DEBUG("   addMonGroup :" + m_basegroupName + decName + "/"+groupName+"/Generic");
    //-REMI addMonGroup(new MonGroup(this,m_basegroupName + decName + "/"+groupName+"/Generic",shift,run));

    //
    addHistogram(new TH1F("hClusterEt","Electron cluster E_{T} ;E_{T} [MeV]; Nevents",100, -1000, 250000));
    addHistogram(new TH1F("hClusterEta","Electron cluster #eta ;#eta; Nevents",50, -3.2, 3.2));
    addHistogram(new TH1F("hClusterPhi","Electron cluster #phi ;#phi; Nevents",50, -3.2,3.2));
    addHistogram(new TH1F("hF1","Electron fractional energy in 1st sampling;F1; Nevents",50, -0.2,1.0));
    addHistogram(new TH1F("hF2","Electron fractional energy in 2nd sampling;F2; Nevents",50, -0.2,1.0));
    addHistogram(new TH1F("hF3","Electron fractional energy in 3rd sampling;F3; Nevents",50, -0.2,1.0));
    addHistogram(new TH1F("hF1core","Electron fractional energy in the core of the 1st sampling;F1core; Nevents",50, -0.2,1.0));
    addHistogram(new TH1F("hF3core","Electron fractional energy in the core of the 3rd sampling;F3core; Nevents",50, -0.5,0.5));
    addHistogram(new TH1F("hEhad1","Electron energy leakage in 1st sampling of hadronic cal. ;E had1; Nevents",50, -1000., 10000.));
    addHistogram(new TH1F("hRe233e237","Electron uncor. energy fraction in 3x3/3x7 cells in em sampling 2 ;R 3x3/3x7; Nevents",50, 0., 2.));
    addHistogram(new TH1F("hRe237e277","Electron uncor. energy fraction in 3x7/7x7 cells in em sampling 2 ;R 3x7/7x7; Nevents",50, 0., 2.));
    addHistogram(new TH1F("hCoreEM","Electron core energy in EM calorimeter ;E [MeV]; Nevents",50, -5000., 250000.));
    addHistogram(new TH2F("hEtVsEta","Electron Et vs. #eta ;#eta;E_{T} [MeV]",50, -3.2, 3.2,100, 0, 150000));
    addHistogram(new TH2F("hEtVsPhi","Electron Et vs. #phi ;#phi;E_{T} [MeV]",50, -3.2, 3.2,100, 0, 150000));
    addHistogram(new TH1F("hWeta1","Electron shower width in 1st sampling;Weta1; Nevents",50, -1,1));
    addHistogram(new TH1F("hWeta2","Electron shower width in 2nd sampling;Weta2; Nevents",50, -0.05,0.05));
    addHistogram(new TH1F("hWtot","Electron total shower width in 1st sampling;Wtot; Nevents",50, 0,10));
    addHistogram(new TH1F("hPos7","Electron shower position match;Pos7; Nevents",50, -5.0,5.0));
    ///EMTrackMatch histograms
    addHistogram(new TH1F("hEoverP","Electron match track E over P ;E/p;Nevents",50,0,5));
    addHistogram(new TH2F("hEoverPvsEta","Electron E/p vs #eta ;charge*E/p;#eta",50,-5,5,50,-2.5,2.5));
    addHistogram(new TH2F("hEoverPvsPhi","Electron E/p vs #phi ;charge*E/p;#phi",50,-5,5,50,-3.1,3.1));
    addHistogram(new TH1F("hEoverPvsPhiEle2","Electron E/p for E/p>2 ;E/p;Nevents",50,2,5));
    addHistogram(new TH1F("hEoverPvsPhiPos2","Positron E/p for E/p>2 ;E/p;Nevents",50,2,5));
    addHistogram(new TH1F("hDeltaEta1","Electron track match #Delta #eta (1st sampling) ;#Delta #eta;Nevents",50,-0.05,0.05));
    addHistogram(new TH1F("hDeltaPhi2","Electron track match #Delta #phi (2st sampling) ;#Delta #phi;Nevents",50,-0.15,0.1));
    ///Electron counters in high et bins
    addHistogram(new TH1F("hHighEtRanges","Electrons with E_{T} > 100, 500 and 1000 GeV ;El. with E_{T} over th.;Nevents",3,-0.5,2.5));  
    hist("hHighEtRanges")->GetXaxis()->SetBinLabel(1,"E_{T}>100");
    hist("hHighEtRanges")->GetXaxis()->SetBinLabel(2,"E_{T}>500");
    hist("hHighEtRanges")->GetXaxis()->SetBinLabel(3,"E_{T}>1000"); 
    ///ID hit counter histograms
    addHistogram(new TH1F("hNOfBLayerHits","Electron number of track B-Layer Hits ;N B layer hits;Nevents",6,-0.5,5.5));
    addHistogram(new TH1F("hNOfPixelHits","Electron number of track pixel Hits ;N pixel hits;Nevents",21,-0.5,20.5));
    addHistogram(new TH1F("hNOfTrackSiHits","Electron number of track precision Hits ;N Si hits;Nevents",26,-0.5,25.5));
    //addHistogram(new TH1F("hTrackA0","Electron track transverse impact parameter;A0;Nevents",26,-0.5,5));
    addHistogram(new TH1F("hNOfTRTHits","Electron number of TRT Hits ;N TRT hits;Nevents",46,-0.5,45.5));
    addHistogram(new TH1F("hNOfHighTRTHits","Electron number of high threshold TRT Hits ;N TRT hits;Nevents",46,-0.5,45.5));
    addHistogram(new TH1F("hNOfTRTOutliers","Electron number of TRT hits with Outliers;N TRT w/Outliers;Nevents",51,-0.5,50.5));
    addHistogram(new TH1F("hNOfHighTRTOutliers","Electron number of high threshold TRT Hits with Outliers;N High TRT hits w/Outliers;Nevents",51,-0.5,50.5));
    ///Identification histograms
    addHistogram(new TH1F("hIsEM","Electron IsEM ;IsEM flag;Nevents",4,-0.5,3.5));
    hist("hIsEM")->GetXaxis()->SetBinLabel(1,"None");
    hist("hIsEM")->GetXaxis()->SetBinLabel(2,"Loose");
    hist("hIsEM")->GetXaxis()->SetBinLabel(3,"Medium");
    hist("hIsEM")->GetXaxis()->SetBinLabel(4,"Tight");
    ////Electron E, eta and phi in EM calorimeter samplings in barrel
    ATH_MSG_DEBUG("   addMonGroup :" + m_basegroupName + decName + "/"+groupName+"/BarrelSamplings");
    //-REMI addMonGroup(new MonGroup(this,m_basegroupName + decName + "/"+groupName+"/BarrelSamplings",shift,run));
    //
    addHistogram(new TH1F("hE_PresamplerB","Electron cluster energy in CaloSampling::PreSamplerB sampling;E [MeV]; Nevents",50, -5000., 50000.));
    addHistogram(new TH1F("hE_EMB1","Electron cluster energy in CaloSampling::EMB1 sampling;E [MeV]; Nevents",50, -5000., 50000.));
    addHistogram(new TH1F("hE_EMB2","Electron cluster energy in CaloSampling::EMB2 sampling;E [MeV]; Nevents",50, -5000., 50000.));
    addHistogram(new TH1F("hE_EMB3","Electron cluster energy in CaloSampling::EMB3 sampling;E [MeV]; Nevents",50, -5000., 50000.));
    //
    addHistogram(new TH1F("hEta_PresamplerB","Electron cluster #eta in CaloSampling::PreSamplerB sampling;#eta; Nevents",50, -3.2, 3.2));
    addHistogram(new TH1F("hEta_EMB1","Electron cluster #eta in CaloSampling::EMB1 sampling;#eta; Nevents",50, -3.2, 3.2));
    addHistogram(new TH1F("hEta_EMB2","Electron cluster #eta in CaloSampling::EMB2 sampling;#eta; Nevents",50, -3.2, 3.2));
    addHistogram(new TH1F("hEta_EMB3","Electron cluster #eta in CaloSampling::EMB3 sampling;#eta; Nevents",50, -3.2, 3.2));
    //
    addHistogram(new TH1F("hPhi_PresamplerB","Electron cluster #phi in CaloSampling::PreSamplerB sampling;#phi; Nevents",50, -3.2, 3.2));
    addHistogram(new TH1F("hPhi_EMB1","Electron cluster #phi in CaloSampling::EMB1 sampling;#phi; Nevents",50, -3.2, 3.2));
    addHistogram(new TH1F("hPhi_EMB2","Electron cluster #phi in CaloSampling::EMB2 sampling;#phi; Nevents",50, -3.2, 3.2));
    addHistogram(new TH1F("hPhi_EMB3","Electron cluster #phi in CaloSampling::EMB3 sampling;#phi; Nevents",50, -3.2, 3.2));
    ////Electron E, eta and phi in EM calorimeter samplings in endcap
    ATH_MSG_DEBUG("   addMonGroup :" + m_basegroupName + decName + "/"+groupName+"/EndcapSamplings");
    //-REMI addMonGroup(new MonGroup(this,m_basegroupName + decName + "/"+groupName+"/EndcapSamplings",shift,run));
    //
    addHistogram(new TH1F("hE_PresamplerE","Electron cluster energy in CaloSampling::PreSamplerE sampling;E [MeV]; Nevents",50, -5000., 50000.));
    addHistogram(new TH1F("hE_EME1","Electron cluster energy in CaloSampling::EME1 sampling;E [MeV]; Nevents",50, -5000., 50000.));
    addHistogram(new TH1F("hE_EME2","Electron cluster energy in CaloSampling::EME2 sampling;E [MeV]; Nevents",50, -5000., 50000.));
    addHistogram(new TH1F("hE_EME3","Electron cluster energy in CaloSampling::EME3 sampling;E [MeV]; Nevents",50, -5000., 50000.));
    //
    addHistogram(new TH1F("hEta_PresamplerE","Electron cluster #eta in CaloSampling::PreSamplerE sampling;#eta; Nevents",50, -3.2, 3.2));
    addHistogram(new TH1F("hEta_EME1","Electron cluster #eta in CaloSampling::EME1 sampling;#eta; Nevents",50, -3.2, 3.2));
    addHistogram(new TH1F("hEta_EME2","Electron cluster #eta in CaloSampling::EME2 sampling;#eta; Nevents",50, -3.2, 3.2));
    addHistogram(new TH1F("hEta_EME3","Electron cluster #eta in CaloSampling::EME3 sampling;#eta; Nevents",50, -3.2, 3.2));
    //
    addHistogram(new TH1F("hPhi_PresamplerE","Electron cluster #phi in CaloSampling::PreSamplerE sampling;#phi; Nevents",50, -3.2, 3.2));
    addHistogram(new TH1F("hPhi_EME1","Electron cluster #phi in CaloSampling::EME1 sampling;#phi; Nevents",50, -3.2, 3.2));
    addHistogram(new TH1F("hPhi_EME2","Electron cluster #phi in CaloSampling::EME2 sampling;#phi; Nevents",50, -3.2, 3.2));
    addHistogram(new TH1F("hPhi_EME3","Electron cluster #phi in CaloSampling::EME3 sampling;#phi; Nevents",50, -3.2, 3.2));
    
  return StatusCode::SUCCESS;
 }

// ===========================================================
StatusCode softEMonTool::book( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun )
{
  //
  // Booking of histograms
  //


  // Default values
  m_basegroupName = "egamma/";
  
  if (m_e_decdivs.size()==0) {
    m_e_decdivs.push_back("softEIDB");
    m_e_decdivs.push_back("softEIDC");
    m_e_decdivs.push_back("softEIDE");
  }


  MsgStream log( msgSvc(), name() );
  ATH_MSG_DEBUG("softEMonTool::bookHistograms()");
  StatusCode sc = service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()){
      ATH_MSG_ERROR
	  ("Unable to retrieve pointer to StoreGateSvc");
      return sc;
    }
  if(m_UseTrigger) {
    ATH_MSG_DEBUG("Trigger is on, retrieving trigger decision tool.");
  }
  else {
    ATH_MSG_DEBUG("Trigger is off.");
  }
  if(m_UseTrigger) {
    sc = m_trigDec.retrieve();
    if( !sc.isSuccess() && m_e_triggers.size()) {
      ATH_MSG_ERROR("!! Unable to retrieve the TrigDecisionTool !!" );
      return sc;
    }
    ATH_MSG_DEBUG( "  --> Found AlgTool \"TrigDecisionTool\"" );
    
  }
  ATH_MSG_DEBUG("softEMonTool::bookHistograms()\tDefining Monitoring Groups");
  // Add monitoring groups for trigger signatures if trigger is being used.


  /*-REMI
if(isNewRun){
  if(m_UseTrigger) {
    for(unsigned int i=0;i<m_e_triggers.size(); ++i) addMonGroup(new MonGroup(this,m_basegroupName + "softEIDB/"+m_e_triggers[i],shift,run));
    for(unsigned int i=0;i<m_e_triggers.size(); ++i) addMonGroup(new MonGroup(this,m_basegroupName + "softEIDC/"+m_e_triggers[i],shift,run));
    for(unsigned int i=0;i<m_e_triggers.size(); ++i) addMonGroup(new MonGroup(this,m_basegroupName + "softEIDE/"+m_e_triggers[i],shift,run));
    }
  
  for(unsigned int i=0;i<m_e_selections.size(); ++i) addMonGroup(new MonGroup(this,m_basegroupName + "softEIDB/"+m_e_selections[i],shift,run));
  for(unsigned int i=0;i<m_e_selections.size(); ++i) addMonGroup(new MonGroup(this,m_basegroupName + "softEIDC/"+m_e_selections[i],shift,run));
  for(unsigned int i=0;i<m_e_selections.size(); ++i) addMonGroup(new MonGroup(this,m_basegroupName + "softEIDE/"+m_e_selections[i],shift,run));
}
  */

  ATH_MSG_DEBUG( "Size of Electron Trigger vector = "<< m_e_triggers.size() );

  if(isNewRun) {   
    // Book histos for trigger signature
    for(unsigned int i=0;i<m_e_triggers.size(); ++i){
      sc = bookElectronHistosDec(m_e_triggers[i], "softEIDB");
      sc = bookElectronHistosDec(m_e_triggers[i], "softEIDC");
      sc = bookElectronHistosDec(m_e_triggers[i], "softEIDE");
      if(sc != StatusCode::SUCCESS ) {
	ATH_MSG_ERROR("Error booking electron histos for trigger items.");
	return sc;
      }
    }
    
    for(unsigned int i=0;i<m_e_selections.size(); ++i){
      sc = bookElectronHistosDec(m_e_selections[i], "softEIDB");
      sc = bookElectronHistosDec(m_e_selections[i], "softEIDC");
      sc = bookElectronHistosDec(m_e_selections[i], "softEIDE");
      if(sc != StatusCode::SUCCESS ) {
	ATH_MSG_ERROR("Error booking electron histos for id selecions.");
	return sc;
      }
    }
  }
  if(isNewEventsBlock || isNewLumiBlock) {}
  return sc;
}

// ===========================================================
StatusCode softEMonTool::fill() 
{
  //
  // method to fill histograms
  //
  bool LarIsBad = this->hasBadLar();

  StatusCode sc(StatusCode::SUCCESS, true);
  MsgStream log( msgSvc(), name() );
  ATH_MSG_DEBUG(" fillHistograms() m_storeGate: "<<m_storeGate);

//check whether Lar signalled event bad
if (LarIsBad) return StatusCode::SUCCESS;



  // Do electron histos filling
  const egammaContainer* electron_container=0;
  sc = m_storeGate->retrieve(electron_container, m_ElectronCnt);
  if(sc.isFailure()  ||  !electron_container){
    ATH_MSG_VERBOSE("no electron container found in TDS");
    return StatusCode::SUCCESS;
  }


  //For barrel B, crack C and endcap E
  for (unsigned int idecdiv=0; idecdiv<m_e_decdivs.size(); idecdiv++) {

    // iterators of the electron container
    egammaContainer::const_iterator eg_itersoftEID    = electron_container->begin();
    egammaContainer::const_iterator eg_itersoftEIDEnd = electron_container->end();
    ATH_MSG_DEBUG("Number of electrons: "<<electron_container->size());

    // loop on electron container
    for (; eg_itersoftEID!=eg_itersoftEIDEnd; eg_itersoftEID++){
      // select electrons reconstructed by the track-based algorithm
      if ((*eg_itersoftEID)->author(egammaParameters::AuthorSofte)==0)
	continue;
      for(unsigned int i=0;i<m_e_selections.size(); ++i){

	// apply tight isEM selection
	if(m_e_selections[i]=="tight" && passesIsEMCut(egammaPID::ElectronTightPP,*eg_itersoftEID)){
	  setCurrentMonGroup(m_basegroupName + m_e_decdivs[idecdiv] + "/"+m_e_selections[i]);	
	  fillElectronHistos(*eg_itersoftEID,m_e_decdivs[idecdiv]);
	}

	// apply loose isEM selection
	if(m_e_selections[i]=="loose" && passesIsEMCut(egammaPID::ElectronLoosePP,*eg_itersoftEID)){
	  setCurrentMonGroup(m_basegroupName + m_e_decdivs[idecdiv] + "/"+m_e_selections[i]);
	  fillElectronHistos(*eg_itersoftEID,m_e_decdivs[idecdiv]);
	}

	// apply medium isEM selection
	if(m_e_selections[i]=="medium" && passesIsEMCut(egammaPID::ElectronMediumPP,*eg_itersoftEID)){
	  setCurrentMonGroup(m_basegroupName + m_e_decdivs[idecdiv] + "/"+m_e_selections[i]);	
	  fillElectronHistos(*eg_itersoftEID,m_e_decdivs[idecdiv]);
	}

	// apply
	if(m_e_selections[i]=="all"){
	  setCurrentMonGroup(m_basegroupName + m_e_decdivs[idecdiv] + "/"+m_e_selections[i]);
	  fillElectronHistos(*eg_itersoftEID,m_e_decdivs[idecdiv]);
	}
      }
    }

    // in case trigger information is used
    if(m_UseTrigger) {
      ATH_MSG_DEBUG("trigger size : "<<m_e_triggers.size());
      for(unsigned int i=0;i<m_e_triggers.size(); ++i){
	Trig::FeatureContainer ef = m_trigDec->features(m_e_triggers[i]);
	Trig::FeatureContainer::combination_const_iterator cIt;
        for ( cIt = ef.getCombinations().begin(); cIt != ef.getCombinations().end(); ++cIt ) {
	  std::vector< Trig::Feature<egammaContainer> > trigEgamma = cIt->get<egammaContainer>();
	  if( ! trigEgamma.empty()){
	    setCurrentMonGroup(m_basegroupName + m_e_decdivs[idecdiv] + "/"+m_e_triggers[i]);
	    for(unsigned int ti=0;ti<trigEgamma.size();ti++){
	      const egammaContainer* trigEgammaContainer = trigEgamma[ti].cptr();
	      if ( ((trigEgamma[ti]).te())->getActiveState() )
		{
		  if(! (trigEgammaContainer)){continue;}
		  egammaContainer::const_iterator itrEg;
		  egammaContainer::const_iterator itrEgE= trigEgammaContainer->end();
		  for (itrEg = trigEgammaContainer->begin(); itrEg != itrEgE; ++itrEg){
		    if(*itrEg) {
		      if ((*itrEg)->author(egammaParameters::AuthorElectron)==0)
			continue;
		      fillElectronHistos(*itrEg,m_e_decdivs[idecdiv]);
		    }
		  }//loop in trigegammacontainer
		}
	    }//loop in trigegamma vector
          }//if trigEgamma Vector not empty
	}//loop in  combinations
      }//loop through triggers
    }//if use trigger
  }//end loop on decdiv



  return sc;
}

// ===========================================================
void softEMonTool::fillElectronHistos(const egamma *eg, const std::string & sreg) 
{
  //
  // Filling of histograms for electrons
  //

  MsgStream log( msgSvc(), name() );
  std::string aMonGroup = m_currentMonGroup;
  ATH_MSG_DEBUG(" fillElectronHistos(" + sreg + ") for group: "<<aMonGroup);

  // retrieve the associated cluster
  const CaloCluster *aCluster = eg->cluster();
  if (!aCluster)
    ATH_MSG_WARNING( "Can't get CaloCluster" );
  // check if object is in barrel or end-caps
  else if ( (inBarrel(aCluster) && sreg=="softEIDB") || (inCrack(aCluster) && sreg=="softEIDC") || (inEndcap(aCluster) && sreg=="softEIDE") ){
    setCurrentMonGroup(aMonGroup+"/Generic");
    ATH_MSG_DEBUG(" fillElectronHistos() for subgroup: "<<m_currentMonGroup);

    // transverse energy from cluster
    float et = aCluster->et();
    // eta position from the cluster
    float eta = aCluster->eta();
    // phi position from the cluster
    float phi = aCluster->phi();
    hist("hClusterEt")->Fill(et);
    hist("hClusterEta")->Fill(eta);
    hist("hClusterPhi")->Fill(phi);

    // fraction of energy reconstructed in the 2nd sampling
    float f2 = aCluster->energyBE(2)/(et*cosh(eta)); 
    //float f2 = aCluster->e()!=0. ? aCluster->energyBE(2)/(aCluster->e()) : 0.; 
    hist("hF2")->Fill(f2);
    hist2("hEtVsEta")->Fill(eta,et);
    hist2("hEtVsPhi")->Fill(phi,et);
    if(et>100000) hist("hHighEtRanges")->Fill(( et > 500000 ) + ( et > 1000000 ));
    // E/p
    double ep =0;
    const Rec::TrackParticle *t = eg->trackParticle();
    if(t)
      {
        double trackp = t->p();
        if(trackp !=0){
	  ep = eg->cluster()->energy()/trackp;
        }
      }
    
    hist("hEoverP")->Fill(ep);
    hist2("hEoverPvsEta")->Fill(eg->charge()*ep,eta);
    hist2("hEoverPvsPhi")->Fill(eg->charge()*ep,phi);
    if(ep>2. && eg->charge()==-1) hist("hEoverPvsPhiEle2")->Fill(ep);
    if(ep>2. && eg->charge()==1) hist("hEoverPvsPhiPos2")->Fill(ep);
    
    // retrieve the cluster-track-matching information
    const EMTrackMatch* trackMatch = eg->detail<EMTrackMatch>(m_EMTrackMatchCnt);
    if(trackMatch) {
      // Difference in eta position between track extrapolated and shower 
      // in the 1st sampling
      hist("hDeltaEta1")->Fill(trackMatch->deltaEta(1));
      // Difference in phi position between track extrapolated and shower 
      // in the 2nd sampling
      hist("hDeltaPhi2")->Fill(trackMatch->deltaPhi(2));
    }
    else ATH_MSG_DEBUG( "Can't get EMTrackMatch  with the name:"<<m_EMTrackMatchCnt);
    
    // retrieve information of tracks in the inner detector
    const Rec::TrackParticle* aTrackParticle = eg->trackParticle();
    // if trackParticle information is available
    if(aTrackParticle) {
      // retrieve track summary information
      const Trk::TrackSummary * trksummary = aTrackParticle->trackSummary();
      // if track summary information is available
      if(trksummary){
	hist("hNOfBLayerHits")->Fill(trksummary->get(Trk::numberOfBLayerHits));
	hist("hNOfPixelHits")->Fill( trksummary->get(Trk::numberOfPixelHits));
	hist("hNOfTrackSiHits")->Fill(trksummary->get(Trk::numberOfPixelHits)+
				      trksummary->get(Trk::numberOfSCTHits));
	hist("hNOfTRTHits")->Fill(trksummary->get(Trk::numberOfTRTHits));
	hist("hNOfHighTRTHits")->Fill(trksummary->get(Trk::numberOfTRTHighThresholdHits));
	hist("hNOfTRTOutliers")->Fill(trksummary->get(Trk::numberOfTRTOutliers)+trksummary->get(Trk::numberOfTRTHits));
	hist("hNOfHighTRTOutliers")->Fill(trksummary->get(Trk::numberOfTRTHighThresholdOutliers)+trksummary->get(Trk::numberOfTRTHighThresholdHits));
	
	//double A0 = fabs(findImpact(*aTrackParticle,0));
	//hist("hTrackA0")->Fill(A0);
      }
      else   ATH_MSG_DEBUG( "Error attempting to retrieve trackSummary");
    }
    else ATH_MSG_DEBUG( "Error attempting to retrieve trackParticle");
    
    
    // Fill information of cut-based (IsEM) selection
    hist("hIsEM")->Fill( passesIsEMCut(egammaPID::ElectronLoosePP, eg) + 
			 passesIsEMCut(egammaPID::ElectronMediumPP, eg) + 
			 passesIsEMCut(egammaPID::ElectronTightPP, eg));

    // Fill E,eta,phi in each sampling
    if(inBarrel(aCluster)){
      setCurrentMonGroup(aMonGroup+"/BarrelSamplings");
      ATH_MSG_DEBUG(" fillElectronHistos() for subgroup: "<<m_currentMonGroup);
      hist("hE_PresamplerB")->Fill(aCluster->eSample(CaloSampling::PreSamplerB));
      hist("hE_EMB1")->Fill(aCluster->eSample(CaloSampling::EMB1));
      hist("hE_EMB2")->Fill(aCluster->eSample(CaloSampling::EMB2));
      hist("hE_EMB3")->Fill(aCluster->eSample(CaloSampling::EMB3));
      //
      hist("hEta_PresamplerB")->Fill(aCluster->etaSample(CaloSampling::PreSamplerB));
      hist("hEta_EMB1")->Fill(aCluster->etaSample(CaloSampling::EMB1));
      hist("hEta_EMB2")->Fill(aCluster->etaSample(CaloSampling::EMB2));
      hist("hEta_EMB3")->Fill(aCluster->etaSample(CaloSampling::EMB3));
      //
      hist("hPhi_PresamplerB")->Fill(aCluster->phiSample(CaloSampling::PreSamplerB));
      hist("hPhi_EMB1")->Fill(aCluster->phiSample(CaloSampling::EMB1));
      hist("hPhi_EMB2")->Fill(aCluster->phiSample(CaloSampling::EMB2));
      hist("hPhi_EMB3")->Fill(aCluster->phiSample(CaloSampling::EMB3));
    }
    //
    if(inEndcap(aCluster)){
      setCurrentMonGroup(aMonGroup+"/EndcapSamplings");
      ATH_MSG_DEBUG(" fillElectronHistos() for subgroup: "<<m_currentMonGroup);
      
      hist("hE_PresamplerE")->Fill(aCluster->eSample(CaloSampling::PreSamplerE));
      hist("hE_EME1")->Fill(aCluster->eSample(CaloSampling::EME1));
      hist("hE_EME2")->Fill(aCluster->eSample(CaloSampling::EME2));
      hist("hE_EME3")->Fill(aCluster->eSample(CaloSampling::EME3));
      //
      hist("hEta_PresamplerE")->Fill(aCluster->etaSample(CaloSampling::PreSamplerE));
      hist("hEta_EME1")->Fill(aCluster->etaSample(CaloSampling::EME1));
      hist("hEta_EME2")->Fill(aCluster->etaSample(CaloSampling::EME2));
      hist("hEta_EME3")->Fill(aCluster->etaSample(CaloSampling::EME3));
      //
      hist("hPhi_PresamplerE")->Fill(aCluster->phiSample(CaloSampling::PreSamplerE));
      hist("hPhi_EME1")->Fill(aCluster->phiSample(CaloSampling::EME1));
      hist("hPhi_EME2")->Fill(aCluster->phiSample(CaloSampling::EME2));
      hist("hPhi_EME3")->Fill(aCluster->phiSample(CaloSampling::EME3));
    }
    
    
    // retrieve information from EMShower objects
    const EMShower* shower = eg->detail<EMShower>(m_EMShowerCnt);
    if(shower){
      setCurrentMonGroup(aMonGroup+"/Generic");
      ATH_MSG_DEBUG(" fillElectronHistos() for subgroup: "<<m_currentMonGroup);
      double f1     = shower->f1();
      double f1core = shower->f1core();
      double f3     = shower->f3();
      double f3core = shower->f3core();
      double Ehad1  = shower->ehad1();
      double Reta37 = shower->e277() != 0. ? (shower->e237()) / (shower->e277()) : 0.;
      double Rphi33 = shower->e237() != 0. ? (shower->e233()) / (shower->e237()) : 0.;
      double Ecore  = shower->ecore();
      double Weta2  = shower->weta2();
      double Ws3    = shower->weta1();
      double Wstot  = shower->wtots1();
      double Pos7   = shower->pos7();
      
      hist("hF1")->Fill(f1);
      hist("hF3")->Fill(f3);
      hist("hF1core")->Fill(f1core);
      hist("hF3core")->Fill(f3core);
      hist("hEhad1")->Fill(Ehad1);
      hist("hRe233e237")->Fill(Rphi33);
      hist("hRe237e277")->Fill(Reta37);
      hist("hCoreEM")->Fill(Ecore);
      hist("hWeta1")->Fill(Ws3);
      hist("hWeta2")->Fill(Weta2);
      hist("hWtot")->Fill(Wstot);
      hist("hPos7")->Fill(Pos7);
    }
    else ATH_MSG_DEBUG("Can't retrive EMShower container with the name: "<<m_EMShowerCnt);
    
  } //end condition on division
  
  setCurrentMonGroup(aMonGroup);
  
}

// ===========================================================
StatusCode softEMonTool::proc( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun )
{
  //
  // method to process histograms
  //

  MsgStream log( msgSvc(), name() );
  ATH_MSG_DEBUG("softEMonTool::procHistograms()");
  if(isEndOfEventsBlock || isEndOfLumiBlock ||  isEndOfRun){}
  return StatusCode( StatusCode::SUCCESS );
}

// ===========================================================
bool softEMonTool::passesIsEMCut(const unsigned int Cut , const egamma * eg)
{
  //
  // method to check if object passes selection (isEM) cut
  //

  const unsigned int fullmask = egammaPID::ElectronTightPP;
  const unsigned int mask = eg->isem(fullmask);

  if( ( mask & Cut ) == 0 ) return true;
  return false;
}

// ===========================================================
bool softEMonTool::inBarrel(const CaloCluster * cluster) 
{
  //
  // method to check if object is in barrel
  //

  if( fabs( cluster->eta() ) < 1.37 ) {
    return true;
  }
  return false;
}

// ===========================================================
bool softEMonTool::inEndcap(const CaloCluster * cluster) 
{
  //
  // method to check if object is in end-caps
  //

  if( fabs( cluster->eta() ) > 1.52 &&  
      fabs( cluster->eta() ) < 2.47 ){
    return true;
  }
  return false;
}

// ===========================================================
bool softEMonTool::inCrack(const CaloCluster * cluster) 
{
  //
  // method to check if object is in crack region
  //

  if( fabs( cluster->eta() ) > 1.37 &&
      fabs( cluster->eta() ) < 1.52 ) {
    return true;
  }
  return false;
}
