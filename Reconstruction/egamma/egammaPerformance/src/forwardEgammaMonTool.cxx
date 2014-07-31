/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     forwardEgammaMonTool.cxx
PACKAGE:  offline/Reconstruction/egamma/egammaPerformance

AUTHORS: Sebastian Koenig - Mohamed Aharrouche

PURPOSE: forward egamma offline monitoring tool
UPDATED:
        Nov. 09, 2009 (FD) add protections + comments
                           + add protection when using trigger
********************************************************************/


#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "egammaPerformance/forwardEgammaMonTool.h"

#include "TrigSteeringEvent/Enums.h"


#include "egammaEvent/egamma.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronContainer.h"




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


// ===========================================================
//constructor
forwardEgammaMonTool::forwardEgammaMonTool( const std::string & type, const std::string & name,
					    const IInterface* parent )
  :IEgammaMonTool( type, name, parent ),
   m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool")
{

  // Name of the electron collection
  declareProperty("ElectronContainer", 
		  m_electronContainerName = "egammaForwardCollection",
		  "Name of the electron collection");
  // Items for the trigger selection
  declareProperty("Electron_Trigger_Items", 
		  m_e_triggers,
		  "Items for the trigger selection");
  // Items for the electron selection
  declareProperty("Electron_Selection_Items", 
		  m_e_selections,
		  "Items for the electron selection");
  // Boolean to use trigger information
  declareProperty("UseTrigger", m_UseTrigger=true,
		  "Boolean to use trigger information");
  // Name of the tool for the trigger decision  
  declareProperty("TrigDecisionTool", 
		  m_trigDec, 
		  "Tool to access the trigger decision");
  //Boolean to switch on EventInfo::LAr checking
//  declareProperty("CheckLAr",
//                  m_checkLar,
//                  "Decision to check Lar EventInfo or not");

}

// ===========================================================
//destructor
forwardEgammaMonTool::~forwardEgammaMonTool()
{
}

// ===========================================================
StatusCode forwardEgammaMonTool::bookFwdEgHistos(const std::string & /* groupName */)
{
  //
  // method to book histograms
  //

  MsgStream log( msgSvc(), name() );
  
  ATH_MSG_DEBUG("forwardEgammaMonTool::bookFwdEgHistos()");
  ///Egamma forward candidate properties
    //-REMI addMonGroup(new MonGroup(this,"egamma/Forward/"+groupName+"/Generic",shift,run));
  
  addHistogram(new TH1F("hClusterEt","Egamma cluster E_{T} ;E_{T} [MeV]; Nevents",100, -1000, 250000));
  addHistogram(new TH1F("hClusterEta","Egamma cluster #eta ;#eta; Nevents",50, -5.0,5.0));
  addHistogram(new TH1F("hClusterPhi","Egamma cluster #phi ;#phi; Nevents",50, -3.2,3.2));
  addHistogram(new TH2F("hEtVsEta","Egamma cluster Et vs. #eta ;#eta;E_{T} [MeV]",50, -5.0, 5.0,100, 0, 150000));
  addHistogram(new TH2F("hEtVsPhi","Egamma cluster Et vs. #phi ;#phi;E_{T} [MeV]",50, -3.2, 3.2,100, 0, 150000));
  
  ///Egamma counters in high et bins
  addHistogram(new TH1F("hHighEtRanges","Egammas with E_{T} > 100, 500 and 1000 GeV ;El. with E_{T} over th.;Nevents",3,-0.5,2.5));
  hist("hHighEtRanges")->GetXaxis()->SetBinLabel(1,"E_{T}>100");
  hist("hHighEtRanges")->GetXaxis()->SetBinLabel(2,"E_{T}>500");
  hist("hHighEtRanges")->GetXaxis()->SetBinLabel(3,"E_{T}>1000");
  
  addHistogram(new TH1F("h_energy_density","First Moment of Energy Density ;FistEngDens; Nevents",200,0.,2.));
  addHistogram(new TH1F("h_frac_max","Fraction of most energetic cell ;FacMax;Nevents",50,0.,1.));
  addHistogram(new TH1F("h_longitudinal","Normalized Longitudinal Moment ;Longitudinal;Nevents",100,0.,1.));
  addHistogram(new TH1F("h_second_lambda","Second Moment of Lambda ;SecondLambda;Nevents",500,0.,10000.));
  addHistogram(new TH1F("h_lateral","Normalized Lateral Moment ;Lateral;Nevents",100,0.,1.));
  addHistogram(new TH1F("h_second_r","Second Moment of R ;SecondR;Nevents",500,0.,20000.));
  addHistogram(new TH1F("h_center_lambda","distance of shower center from calo front face;CenterLambda;Nevents",500,0,2000));
  
  ///Egamma E, eta and phi in EM calorimeter samplings in EMEC
    //-REMI addMonGroup(new MonGroup(this,"egamma/Forward/"+groupName+"/EMEC_IW",shift,run));
  //
  addHistogram(new TH1F("hE_EME_IW1","Egamma cluster energy in CaloSampling::EME_IW1 sampling;E [MeV]; Nevents",50, -5000., 50000.));
  addHistogram(new TH1F("hE_EME_IW2","Egamma cluster energy in CaloSampling::EME_IW2 sampling;E [MeV]; Nevents",100, -100000., 1000000.));
  addHistogram(new TH1F("hE_EME_IW3","Egamma cluster energy in CaloSampling::EME_IW3 sampling;E [MeV]; Nevents",50, -5000., 50000.));
  //
  addHistogram(new TH1F("hEta_EME_IW1","Egamma cluster #eta in CaloSampling::EME_IW1 sampling;#eta; Nevents",50, -5.0, 5.0));
  addHistogram(new TH1F("hEta_EME_IW2","Egamma cluster #eta in CaloSampling::EME_IW2 sampling;#eta; Nevents",50, -5.0, 5.0));
  addHistogram(new TH1F("hEta_EME_IW3","Egamma cluster #eta in CaloSampling::EME_IW3 sampling;#eta; Nevents",50, -5.0, 5.0));
  //
  addHistogram(new TH1F("hPhi_EME_IW1","Egamma cluster #phi in CaloSampling::EME_IW1 sampling;#phi; Nevents",50, -3.2, 3.2));
  addHistogram(new TH1F("hPhi_EME_IW2","Egamma cluster #phi in CaloSampling::EME_IW2 sampling;#phi; Nevents",50, -3.2, 3.2));
  addHistogram(new TH1F("hPhi_EME_IW3","Egamma cluster #phi in CaloSampling::EME_IW3 sampling;#phi; Nevents",50, -3.2, 3.2));
  
  // shower shapes
  addHistogram(new TH1F("h_energy_density_emec","First Moment of Energy Density in EMEC;FistEngDens; Nevents",200,0.,2.));
  addHistogram(new TH1F("h_frac_max_emec","Fraction of most energetic cell in EMEC;FacMax;Nevents",50,0.,1.));
  addHistogram(new TH1F("h_longitudinal_emec","Normalized Longitudinal Moment in EMEC;Longitudinal;Nevents",100,0.,1.));
  addHistogram(new TH1F("h_second_lambda_emec","Second Moment of Lambda in EMEC;SecondLambda;Nevents",500,0.,10000.));
  addHistogram(new TH1F("h_lateral_emec","Normalized Lateral Moment in EMEC;Lateral;Nevents",100,0.,1.));
  addHistogram(new TH1F("h_second_r_emec","Second Moment of R in EMEC;SecondR;Nevents",500,0.,20000.));
  addHistogram(new TH1F("h_center_lambda_emec","distance of shower center from calo front face in EMEC;CenterLambda;Nevents",500,0,2000));
  //
  
  ///Egamma E, eta and phi in FCal
    //-REMI addMonGroup(new MonGroup(this,"egamma/Forward/"+groupName+"/FCal",shift,run));
  //
  addHistogram(new TH1F("hE_FCAL0","Egamma cluster energy in CaloSampling::FCAL0 sampling;E [MeV]; Nevents",100, -10000., 1000000.));
  addHistogram(new TH1F("hE_FCAL1","Egamma cluster energy in CaloSampling::FCAL1 sampling;E [MeV]; Nevents",100, -10000., 500000.));
  addHistogram(new TH1F("hE_FCAL2","Egamma cluster energy in CaloSampling::FCAL2 sampling;E [MeV]; Nevents",50, -5000., 50000.));
  //
  addHistogram(new TH1F("hEta_FCAL0","Egamma cluster #eta in CaloSampling::FCAL0 sampling;#eta; Nevents",50, -5.0, 5.0));
  addHistogram(new TH1F("hEta_FCAL1","Egamma cluster #eta in CaloSampling::FCAL1 sampling;#eta; Nevents",50, -5.0, 5.0));
  addHistogram(new TH1F("hEta_FCAL2","Egamma cluster #eta in CaloSampling::FCAL2 sampling;#eta; Nevents",50, -5.0, 5.0));
  //
  addHistogram(new TH1F("hPhi_FCAL0","Egamma cluster #phi in CaloSampling::FCAL0 sampling;#phi; Nevents",50, -3.2, 3.2));
  addHistogram(new TH1F("hPhi_FCAL1","Egamma cluster #phi in CaloSampling::FCAL1 sampling;#phi; Nevents",50, -3.2, 3.2));
  addHistogram(new TH1F("hPhi_FCAL2","Egamma cluster #phi in CaloSampling::FCAL2 sampling;#phi; Nevents",50, -3.2, 3.2));
  
  // shower shapes
  addHistogram(new TH1F("h_energy_density_fcal","First Moment of Energy Density in FCAL;FistEngDens; Nevents",200,0.,2.));
  addHistogram(new TH1F("h_frac_max_fcal","Fraction of most energetic cell in FCAL;FacMax;Nevents",50,0.,1.));
  addHistogram(new TH1F("h_longitudinal_fcal","Normalized Longitudinal Moment in FCAL;Longitudinal;Nevents",100,0.,1.));
  addHistogram(new TH1F("h_second_lambda_fcal","Second Moment of Lambda in FCAL;SecondLambda;Nevents",500,0.,10000.));
  addHistogram(new TH1F("h_lateral_fcal","Normalized Lateral Moment in FCAL;Lateral;Nevents",100,0.,1.));
  addHistogram(new TH1F("h_second_r_fcal","Second Moment of R in FCAL;SecondR;Nevents",500,0.,20000.));
  addHistogram(new TH1F("h_center_lambda_fcal","distance of shower center from calo front face in FCAL;CenterLambda;Nevents",500,0,2000));
  
  return StatusCode::SUCCESS;
}

// ===========================================================
StatusCode forwardEgammaMonTool::book( bool isNewEventsBlock, bool isNewLumiBlock, bool isNewRun )
{
  //
  // method to book histograms
  //

  MsgStream log( msgSvc(), name() );
  ATH_MSG_DEBUG("forwardEgammaMonTool::bookHistograms()");
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
  
  ATH_MSG_DEBUG("forwardEgammaMonTool::bookHistograms()\tDefining Monitoring Groups");

  if (isNewRun) {
    if(m_UseTrigger) {
      //-REMI for(unsigned int i=0;i<m_e_triggers.size(); ++i) addMonGroup(new MonGroup(this,"egamma/Forward/"+m_e_triggers[i],shift,run));
    }

    //-REMI for(unsigned int i=0;i<m_e_selections.size(); ++i) addMonGroup(new MonGroup(this,"egamma/Forward/"+m_e_selections[i],shift,run));
  }

 
  ATH_MSG_DEBUG( "Size of central Electron Trigger vector = "<< m_e_triggers.size() );
  
  if(isNewRun) {
    if(m_UseTrigger){ 
      // Book histos for trigger signature
      for(unsigned int i=0;i<m_e_triggers.size(); ++i){
	sc = bookFwdEgHistos(m_e_triggers[i]);
	if(sc != StatusCode::SUCCESS ) {
	  ATH_MSG_ERROR("Error booking forward histos for trigger items.");
	  return sc;
	}
      }
    }
     
    for(unsigned int i=0;i<m_e_selections.size(); ++i){
      sc = bookFwdEgHistos(m_e_selections[i]);
      if(sc != StatusCode::SUCCESS ) {
	ATH_MSG_ERROR("Error booking forward histos for id selecions.");
	return sc;
      }
    }
  }
  if(isNewEventsBlock || isNewLumiBlock) {}
  return sc;
}

// ===========================================================
StatusCode forwardEgammaMonTool::fill()
{
  //
  // Filling of histograms
  //
bool LarIsBad = this->hasBadLar();

  StatusCode sc(StatusCode::SUCCESS, true);
  MsgStream log( msgSvc(), name() );
  
  ATH_MSG_DEBUG(" fillHistograms() m_storeGate: "<<m_storeGate);

//check whether Lar signalled event bad
if (LarIsBad) return StatusCode::SUCCESS;



  /// Do electron histos filling
  const egammaContainer* electron_container=0;
  sc = m_storeGate->retrieve(electron_container, m_electronContainerName);
  if(sc.isFailure()  ||  !electron_container){
    ATH_MSG_VERBOSE("no electron container found in TDS");
    return StatusCode::SUCCESS;
  }
  egammaContainer::const_iterator eg_iter    = electron_container->begin();
  egammaContainer::const_iterator eg_iterEnd = electron_container->end();


  ATH_MSG_DEBUG("Number of egammaForward electrons: "<<electron_container->size());

  ///Fill histograms for specific selections
  for (; eg_iter!=eg_iterEnd; eg_iter++){
    // select objects which are reconstructed by the forward algorithm
    if ((*eg_iter)->author(egammaParameters::AuthorFrwd)==0) continue;
    
    ATH_MSG_DEBUG(" fwdeg Eta  "<< (*eg_iter)->eta()<<"\t fwdeg Phi  " << (*eg_iter)->phi());
    
    for(unsigned int i=0;i<m_e_selections.size(); ++i){
      if(m_e_selections[i]=="all"){
	setCurrentMonGroup("egamma/Forward/"+m_e_selections[i]);
	fillFwdEgHistos(*eg_iter);
      }

      // selection of tight objects
      if(m_e_selections[i]=="tight" && passesIsEMCut(egammaPID::frwdElectronTight, (*eg_iter))){
	setCurrentMonGroup("egamma/Forward/"+m_e_selections[i]);
	fillFwdEgHistos(*eg_iter);
      }
      
      // selection of loose objects
      if(m_e_selections[i]=="loose" && passesIsEMCut(egammaPID::frwdElectronLoose, (*eg_iter))){
	setCurrentMonGroup("egamma/Forward/"+m_e_selections[i]);
	fillFwdEgHistos(*eg_iter);
      }
      
    }
  } 

  // in case trigger is used
  if(m_UseTrigger) {
    ATH_MSG_DEBUG("trigger size : "<<m_e_triggers.size());
    for(unsigned int i=0;i<m_e_triggers.size(); ++i){
      Trig::FeatureContainer ef = m_trigDec->features(m_e_triggers[i]);
      Trig::FeatureContainer::combination_const_iterator cIt;
      for ( cIt = ef.getCombinations().begin(); cIt != ef.getCombinations().end(); ++cIt ) {
	std::vector< Trig::Feature<egammaContainer> > trigEgamma = cIt->get<egammaContainer>();
	if( ! trigEgamma.empty()){
	  setCurrentMonGroup("egamma/Forward/"+m_e_triggers[i]);
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
		    fillFwdEgHistos(*itrEg);
		  }
		}//loop in trigegammacontainer
	      }
	  }//loop in trigegamma vector
	}//if trigEgamma Vector not empty
      }//loop in  combinations
    }//loop through triggers
  }//if use trigger
  
  return sc;
}

// ===========================================================
void forwardEgammaMonTool::fillFwdEgHistos(const egamma *eg)
{
  //
  // method to fill histograms
  //

  MsgStream log( msgSvc(), name() );
  
  std::string aMonGroup = m_currentMonGroup ;
  
  ATH_MSG_DEBUG("fillFwdEgHistos() for group: "<< aMonGroup);
  
  // retrieve the corresponding cluster
  const CaloCluster *assCluster = eg->cluster();
  if(assCluster){
    setCurrentMonGroup(aMonGroup+"/Generic");
    ATH_MSG_DEBUG("fillFwdEgHistos() for subgroup: "<< m_currentMonGroup );
    
    // transverse energy from cluster
    float et = assCluster->et();
    // eta position from the cluster
    float eta = assCluster->eta();
    // phi position from the cluster
    float phi = assCluster->phi();
    
    hist("hClusterEt")->Fill(et);
    hist("hClusterEta")->Fill(eta);
    hist("hClusterPhi")->Fill(phi);
    hist2("hEtVsEta")->Fill(eta,et);

    // shower shapes
    hist2("hEtVsPhi")->Fill(phi,et);
    hist("h_energy_density")->Fill(assCluster->getMomentValue(CaloClusterMoment::FIRST_ENG_DENS,1));
    hist("h_frac_max")->Fill(assCluster->getMomentValue(CaloClusterMoment::ENG_FRAC_MAX,1));
    hist("h_longitudinal")->Fill(assCluster->getMomentValue(CaloClusterMoment::LONGITUDINAL,1));
    hist("h_second_lambda")->Fill(assCluster->getMomentValue(CaloClusterMoment::SECOND_LAMBDA,1));
    hist("h_lateral")->Fill(assCluster->getMomentValue(CaloClusterMoment::LATERAL,1));
    hist("h_second_r")->Fill(assCluster->getMomentValue(CaloClusterMoment::SECOND_R,1));
    hist("h_center_lambda")->Fill(assCluster->getMomentValue(CaloClusterMoment::CENTER_LAMBDA,1));
    

    if(et>100000)
      hist("hHighEtRanges")->Fill(( et > 500000 ) + ( et > 1000000 ));
    //
    if(inEMEC(assCluster)){
      setCurrentMonGroup(aMonGroup+"/EMEC_IW");
      ATH_MSG_DEBUG("fillFwdEgHistos() for subgroup: "<< m_currentMonGroup );
      hist("hE_EME_IW1")->Fill(assCluster->eSample(CaloSampling::EME1));
      hist("hE_EME_IW2")->Fill(assCluster->eSample(CaloSampling::EME2));
      hist("hE_EME_IW3")->Fill(assCluster->eSample(CaloSampling::EME3));
      //
      hist("hEta_EME_IW1")->Fill(assCluster->etaSample(CaloSampling::EME1));
      hist("hEta_EME_IW2")->Fill(assCluster->etaSample(CaloSampling::EME2));
      hist("hEta_EME_IW3")->Fill(assCluster->etaSample(CaloSampling::EME3));
      //
      hist("hPhi_EME_IW1")->Fill(assCluster->phiSample(CaloSampling::EME1));
      hist("hPhi_EME_IW2")->Fill(assCluster->phiSample(CaloSampling::EME2));
      hist("hPhi_EME_IW3")->Fill(assCluster->phiSample(CaloSampling::EME3));
      //
      hist("h_energy_density_emec")->Fill(assCluster->getMomentValue(CaloClusterMoment::FIRST_ENG_DENS,1));
      hist("h_frac_max_emec")->Fill(assCluster->getMomentValue(CaloClusterMoment::ENG_FRAC_MAX,1));
      hist("h_longitudinal_emec")->Fill(assCluster->getMomentValue(CaloClusterMoment::LONGITUDINAL,1));
      hist("h_second_lambda_emec")->Fill(assCluster->getMomentValue(CaloClusterMoment::SECOND_LAMBDA,1));
      hist("h_lateral_emec")->Fill(assCluster->getMomentValue(CaloClusterMoment::LATERAL,1));
      hist("h_second_r_emec")->Fill(assCluster->getMomentValue(CaloClusterMoment::SECOND_R,1));
      hist("h_center_lambda_emec")->Fill(assCluster->getMomentValue(CaloClusterMoment::CENTER_LAMBDA,1));
    }
    //
    if(inFCal(assCluster)){
      setCurrentMonGroup(aMonGroup+"/FCal");
      ATH_MSG_DEBUG("fillElectronHistos() for subgroup: "<< m_currentMonGroup);
      hist("hE_FCAL0")->Fill(assCluster->eSample(CaloSampling::FCAL0));
      hist("hE_FCAL1")->Fill(assCluster->eSample(CaloSampling::FCAL1));
      hist("hE_FCAL2")->Fill(assCluster->eSample(CaloSampling::FCAL2));
      //
      hist("hEta_FCAL0")->Fill(assCluster->etaSample(CaloSampling::FCAL0));
      hist("hEta_FCAL1")->Fill(assCluster->etaSample(CaloSampling::FCAL1));
      hist("hEta_FCAL2")->Fill(assCluster->etaSample(CaloSampling::FCAL2));
      //
      hist("hPhi_FCAL0")->Fill(assCluster->phiSample(CaloSampling::FCAL0));
      hist("hPhi_FCAL1")->Fill(assCluster->phiSample(CaloSampling::FCAL1));
      hist("hPhi_FCAL2")->Fill(assCluster->phiSample(CaloSampling::FCAL2));
      //
      hist("h_energy_density_fcal")->Fill(assCluster->getMomentValue(CaloClusterMoment::FIRST_ENG_DENS,1));
      hist("h_frac_max_fcal")->Fill(assCluster->getMomentValue(CaloClusterMoment::ENG_FRAC_MAX,1));
      hist("h_longitudinal_fcal")->Fill(assCluster->getMomentValue(CaloClusterMoment::LONGITUDINAL,1));
      hist("h_second_lambda_fcal")->Fill(assCluster->getMomentValue(CaloClusterMoment::SECOND_LAMBDA,1));
      hist("h_lateral_fcal")->Fill(assCluster->getMomentValue(CaloClusterMoment::LATERAL,1));
      hist("h_second_r_fcal")->Fill(assCluster->getMomentValue(CaloClusterMoment::SECOND_R,1));
      hist("h_center_lambda_fcal")->Fill(assCluster->getMomentValue(CaloClusterMoment::CENTER_LAMBDA,1));
    }
  }
  else ATH_MSG_DEBUG( "Can't get CaloCluster" );
  setCurrentMonGroup(aMonGroup);

}

// ===========================================================
StatusCode forwardEgammaMonTool::proc( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun )
{
  //
  // method to process histograms
  //

  MsgStream log( msgSvc(), name() );
  ATH_MSG_DEBUG("forwardEgammaMonTool::procHistograms()");

  if(isEndOfEventsBlock || isEndOfLumiBlock ||  isEndOfRun){}

  return StatusCode( StatusCode::SUCCESS );
}

// ===========================================================
bool forwardEgammaMonTool::passesIsEMCut(const unsigned int Cut , const egamma * eg)
{
  //
  // method to check if object passes selection (isEM) cut
  //

  const unsigned int fullmask = egammaPID::frwdElectronTight;
  const unsigned int mask = eg->isem(fullmask);

  if( ( mask & Cut ) == 0 ) return true;
  return false;
}

// ===========================================================
bool forwardEgammaMonTool::inEMEC(const CaloCluster * cluster)
{
  //
  // method to check if object is in EMEC
  //

  if( fabs( cluster->eta() ) > 2.5 &&
      fabs( cluster->eta() ) < 3.2 ) {
    return true;
  }
  return false;
}

// ===========================================================
bool forwardEgammaMonTool::inFCal(const CaloCluster * cluster)
{
  //
  // method to check if object is in FCAL
  //

  if( fabs( cluster->eta() ) > 3.2 ) {
    return true;
  }
  return false;
}
