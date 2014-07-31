/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Authors: K. Benslama, E. Ortega
// Update: Y. Ming
//
//



#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include "egammaPerformance/physicsMonTool.h"

#include "TrigSteeringEvent/Enums.h"

#include "egammaEvent/egamma.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronContainer.h"

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egDetailContainer.h"
#include "egammaEvent/egDetail.h"

#include "xAODEventInfo/EventInfo.h"
//to include the runnumber 
#include <boost/lexical_cast.hpp>

#include "VxVertex/VxContainer.h"
#include "VxVertex/VxTrackAtVertex.h"

#include <sstream>

#include "TH2F.h"
#include "TH1F.h"
#include "TProfile.h"


using CLHEP::GeV;


//constructor
physicsMonTool::physicsMonTool( const std::string & type, const std::string & name,
	       const IInterface* parent )
  :IEgammaMonTool( type, name, parent ),
   m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
   //m_checkLar(false),
   m_intEstLumi(0)
{


  // Name of the electron collection
  declareProperty("ElectronContainer", 
		  m_ElectronCnt = "ElectronCollection",
		  "Name of the electron collection");
  // Items for the trigger selection
  declareProperty("Trigger_Items", 
		  m_triggers,
		  "Items for the trigger selection");
  // Boolean to use trigger information
  declareProperty("UseTrigger",m_UseTrigger,
		  "Boolean to use trigger information");
  // Items for the selection
  declareProperty("Selection_Items", 
		  m_selections,
		  "Items for the selection");
  //
  declareProperty("ProcessName", m_processName);
  //
  declareProperty("doLumi", m_doLumi = false);
  //
  declareProperty("massShift",m_massShift =  91188);
  //
  declareProperty("massElectronClusterEtCut",m_ElectronClusterEtCut = 15*GeV);
  // Lower cut on mass
  declareProperty("massLowerCut",
		  m_massLowerCut = 70*GeV,
		  "Lower cut on mass");
  // Upper cut on mass
  declareProperty("massUpperCut",
		  m_massUpperCut = 110*GeV,
		  "Upper cut on mass");
  // Upper cut on mass
  declareProperty("EtRange",
                  m_EtRange = 100*GeV,
                  "Up limit for the Et distribution");


  // Name of the tool for the trigger decision  
  declareProperty("TrigDecisionTool", 
		  m_trigDec, 
		  "Tool to access the trigger decision");

  //Boolean to switch on EventInfo::LAr checking
//  declareProperty("CheckLAr",
//                  m_checkLar,
//                  "Decision to check Lar EventInfo or not");


  declareProperty("VisXsection",
		  m_visxs=1,
		  "Visible cross section of process");
}

// ===========================================================
//destructor
physicsMonTool::~physicsMonTool()
{
}

// ===========================================================
StatusCode physicsMonTool::bookPhysicsHistos(const std::string & groupName) 
{
  //
  // method to book histograms
  //

  MsgStream log( msgSvc(), name() );
  ATH_MSG_DEBUG("physicsMonTool::bookPhysicsHistos()");
  
  ///Generic physics properties
  //-REMI addMonGroup(new MonGroup(this,"egamma/Physics/"+m_processName+"/"+groupName+"/Kinematics",shift,run));
  //Probe cluster cluster
  addHistogram(new TH1F("hClusterEt","E_{T} of probe cluster ;E_{T} [MeV]; Nevents",50, -1000, m_EtRange));
  addHistogram(new TH1F("hClusterEta","#eta of probe cluster ;#eta; Nevents",20, -2.5,2.5));
  addHistogram(new TH1F("hClusterPhi","#phi of probe cluster ;#phi; Nevents",50, -3.2,3.2));
  ///

  addHistogram(new TH1F("hSSClusterEt","E_{T} of probe cluster ;E_{T} [MeV]; Nevents (Same Sign)",50, -1000, m_EtRange));
  addHistogram(new TH1F("hSSClusterEta","#eta of probe cluster ;#eta; Nevents (Same Sign)",20, -2.5,2.5));

  addHistogram(new TH2F("hLeadingClusterEtvsEta","E_{T} vs #eta of leading cluster ;#eta; E_{T} [MeV]",20,-2.5,2.5,50,-1000,m_EtRange));
  addHistogram(new TH2F("hLeadingClusterEtvsEta","E_{T} vs #phi of leading cluster ;#phi; E_{T} [MeV]",50,-3.2,3.2,50,-1000,m_EtRange));
  addHistogram(new TH1F("hLeadingTrackPt","p_{T} of track associated to leading cluster;p_{T} [MeV]; Nevents",50,0,m_EtRange));
  addHistogram(new TH1F("hLeadingTrackEta","#eta of track associated to leading cluster;#eta; Nevents",20,-2.5,2.5));
  addHistogram(new TH1F("hLeadingTrackPhi","#phi of track associated to leading cluster;#phi; Nevents",50,-3.2,3.2));
  addHistogram(new TH2F("hLeadingTrackPtvsEta","p_{T} vs #eta of track associated to leading cluster;#eta;p_{T} [MeV]",
			20,-2.5,2.5,50,0,m_EtRange));
  addHistogram(new TH2F("hLeadingTrackPtvsPhi","p_{T} vs #phi of track associated to leading cluster;#phi;p_{T} [MeV]",
			50,-3.2,3.2,50,0,m_EtRange));
  
  addHistogram(new TH1F("hLooseClusterEt","E_{T} of probe cluster after loose identification;E_{T} [MeV]; Nevents",50, -1000, m_EtRange));
  addHistogram(new TH1F("hLooseClusterEta","#eta of probe cluster after loose identification;#eta; Nevents",20,-2.5,2.5));
  addHistogram(new TH1F("hLooseClusterPhi","#phi of probe cluster after loose identification;#phi; Nevents",50,-3.2,3.2));
  
  addHistogram(new TH1F("hSSLooseClusterEt","E_{T} of probe cluster after loose identification;E_{T} [MeV]; Nevents (Same Sign)",50, -1000, m_EtRange));
  addHistogram(new TH1F("hSSLooseClusterEta","#eta of probe cluster after loose identification;#eta; Nevents (Same Sign)",20,-2.5,2.5));

  addHistogram(new TH1F("hMediumClusterEt","E_{T} of probe cluster after medium identification;E_{T} [MeV]; Nevents",50, -1000, m_EtRange));
  addHistogram(new TH1F("hMediumClusterEta","#eta of probe cluster after medium identification;#eta; Nevents",20,-2.5,2.5));
  addHistogram(new TH1F("hMediumClusterPhi","#phi of probe cluster after medium identification;#phi; Nevents",50,-3.2,3.2));
  
  addHistogram(new TH1F("hTightClusterEt","E_{T} of probe cluster after tight identification;E_{T} [MeV]; Nevents",50, -1000, m_EtRange));
  addHistogram(new TH1F("hTightClusterEta","#eta of probe cluster after tight identification;#eta; Nevents",20,-2.5,2.5));
  addHistogram(new TH1F("hTightClusterPhi","#phi of probe cluster after tight identification;#phi; Nevents",50,-3.2,3.2));
  
  ///Mass histograms
  std::ostringstream hName;
  hName<<"Invariant mass of two electron candidates. [MeV] ";
  std::string tmp = hName.str()+";m [MeV]; Nevents";
  addHistogram(new TH1F("hMass",tmp.c_str(),100,m_massLowerCut,m_massUpperCut));
  tmp = hName.str()+" (Container - Container);m [MeV]; Nevents";
  addHistogram(new TH1F("hMassCC",tmp.c_str(),100,m_massLowerCut,m_massUpperCut));
  tmp = hName.str()+" (Both clusters in barrel);m [MeV]; Nevents";
  addHistogram(new TH1F("hMassBB",tmp.c_str(),100,m_massLowerCut,m_massUpperCut));
  tmp = hName.str()+" (One cluster in barrel, one in endcap);m [MeV]; Nevents";
  addHistogram(new TH1F("hMassBE",tmp.c_str(),100,m_massLowerCut,m_massUpperCut));
  tmp = hName.str()+" (One cluster in barrel, one in crack);m [MeV]; Nevents";
  addHistogram(new TH1F("hMassBC",tmp.c_str(),100,m_massLowerCut,m_massUpperCut));
  tmp = hName.str()+" (One cluster in endcap, one in crack);m [MeV]; Nevents";
  addHistogram(new TH1F("hMassEC",tmp.c_str(),100,m_massLowerCut,m_massUpperCut));
  tmp = hName.str()+" (Both clusters in endcaps);m-m_{0} [MeV]; Nevents";
  addHistogram(new TH1F("hMassEE",tmp.c_str(),100,m_massLowerCut,m_massUpperCut));
  ///Efficiency histograms
  tmp = hName.str()+"Tight-Container;Tight-Container Inv Mass [MeV]; Nevents";
  addHistogram(new TH1F("hMassTA",tmp.c_str(),100,m_massLowerCut,m_massUpperCut));
  tmp = hName.str()+"Tight-Loose;Tight-Loose Inv Mass [MeV]; Nevents";
  addHistogram(new TH1F("hMassTL",tmp.c_str(),100,m_massLowerCut,m_massUpperCut));
  tmp = hName.str()+"Tight-Medium;Tight-Medium Inv Mass [MeV]; Nevents";
  addHistogram(new TH1F("hMassTM",tmp.c_str(),100,m_massLowerCut,m_massUpperCut));
  tmp = hName.str()+"Tight-Tight;Tight-Tight Inv Mass [MeV]; Nevents";
  addHistogram(new TH1F("hMassTT",tmp.c_str(),100,m_massLowerCut,m_massUpperCut));
  //  tmp = hName.str()+"All_tight;m [MeV]; Nevents";
  //  addHistogram(new TH1F("hMassAT",tmp.c_str(),100,m_massLowerCut,m_massUpperCut));
  //  tmp = hName.str()+"Loose_tight;m [MeV]; Nevents";
  //  addHistogram(new TH1F("hMassLT",tmp.c_str(),100,m_massLowerCut,m_massUpperCut));
  //  tmp = hName.str()+"Medium_tight;m [MeV]; Nevents";
  //  addHistogram(new TH1F("hMassMT",tmp.c_str(),100,m_massLowerCut,m_massUpperCut));
  //  tmp = hName.str()+"event_num;Num:; Lumi";
  //  addHistogram(new TH1F("hEventNum",tmp.c_str(),100,m_massLowerCut,m_massUpperCut));  

 
  //-REMI addMonGroup(new MonGroup(this,"egamma/Physics/"+m_processName+"/"+groupName+"/Efficiency",shift,run,"", "weightedEff"));
  
  addProfile(new TProfile("hLooseIdEffVsEt","Loose identification efficiency vs. E_{T}  for candidates passing E_{T}, and mass cuts;E_{T} [MeV];Efficiency",
			50,-1000,m_EtRange));
  addProfile(new TProfile("hLooseIdEffVsEta","Loose identification efficiency vs. #eta  for candidates passing E_{T}, and mass cuts;#eta;Efficiency",
			20,-2.5,2.5));
  addProfile(new TProfile("hLooseIdEffVsPhi","Loose identification efficiency vs. #phi for candidates passing E_{T}, and mass cuts;#phi;Efficiency",
			50,-3.2,3.2));
  
  addProfile(new TProfile("hMediumIdEffVsEt","Medium identification efficiency vs. E_{T}  for candidates passing E_{T}, and mass cuts;E_{T} [MeV];Efficiency",
			50,-1000,m_EtRange));
  addProfile(new TProfile("hMediumIdEffVsEta","Medium identification efficiency vs. #eta  for candidates passing E_{T}, and mass cuts;#eta;Efficiency",
			20,-2.5,2.5));
  addProfile(new TProfile("hMediumIdEffVsPhi","Medium identification efficiency vs. #phi for candidates passing E_{T}, and mass cuts;#phi;Efficiency",
			50,-3.2,3.2));
  
  addProfile(new TProfile("hTightIdEffVsEt","Tight identification efficiency vs. E_{T}  for candidates passing E_{T}, and mass cuts;E_{T} [MeV];Efficiency",
			50,-1000,m_EtRange));
  addProfile(new TProfile("hTightIdEffVsEta","Tight identification efficiency vs. #eta  for candidates passing E_{T}, and mass cuts;#eta;Efficiency",
			20,-2.5,2.5));
  addProfile(new TProfile("hTightIdEffVsPhi","Tight identification efficiency vs. #phi for candidates passing E_{T}, and mass cuts;#phi;Efficiency",
			50,-3.2,3.2));
     addProfile(new TProfile("hLooseIdEffVsNOfPVertices","Loose ID efficiency vs Number of primary vertices;Number of Vertices;Loose IsEM efficiency",10,0.5,50.5));
    addProfile(new TProfile("hMediumIdEffVsNOfPVertices","Medium ID efficiency vs Number of primary vertices;Number of Vertices;Medium IsEM efficiency",10,0.5,50.5));
    addProfile(new TProfile("hTightIdEffVsNOfPVertices","Tight ID efficiency vs Number of primary vertices;Number of Vertices;Tight IsEM efficiency",10,0.5,50.5));
  //monitoring luminosity
  

  if (m_doLumi == true) {
    std::cout << "book histogram " << "egamma/Lumi/"+m_processName+"/"+groupName << std::endl;
    //-REMI addMonGroup(new MonGroup(this,"egamma/Lumi/"+m_processName+"/"+groupName,shift,run));
    
    tmp = hName.str()+";m [MeV]; Nevents";
    addHistogram(new TH1F("hMass",tmp.c_str(),100,m_massLowerCut,m_massUpperCut));
    addHistogram(new TH1F("OffLumi","Official luminosity per lumiblock;L [ub -1]",100,0,3000.));
    addHistogram(new TH1F("EstLumi","Calculated luminosity per lumiblock; L [ub -1]",50,0,50.));
    addHistogram(new TH2F("LumiStable","Official lumi vs estimated lumi;L [ub -1];L [ub -1]",100,0,2000,20,0,10));
  }
 
  return StatusCode::SUCCESS;
}

// ===========================================================
StatusCode physicsMonTool::book( bool /*isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool isNewRun ) 
{
  //
  // method for booking
  //

  MsgStream log( msgSvc(), name() );
  StatusCode sc;
  
  // retrieve trigger information
  if(m_UseTrigger) {
    sc = m_trigDec.retrieve();
    if( !sc.isSuccess() && m_triggers.size()) {
      ATH_MSG_ERROR( "!! Unable to retrieve the TrigDecisionTool !!" );
      return sc;
    }
    ATH_MSG_DEBUG( "  --> Found AlgTool \"TrigDecisionTool\"" );
  }
  ATH_MSG_DEBUG("physicsMonTool::bookHistograms()\tDefining Monitoring Groups");

  if(isNewRun){
    if(m_UseTrigger) {
      //-REMI for(unsigned int i=0;i<m_triggers.size(); ++i) addMonGroup(new MonGroup(this,"egamma/Physics/"+m_processName+"/"+m_triggers[i],shift,run));
    }
    //-REMI for(unsigned int i=0;i<m_selections.size(); ++i) addMonGroup(new MonGroup(this,"egamma/Physics/"+m_processName+"/"+m_selections[i],shift,run));
  }
  ATH_MSG_DEBUG( "Size of Physics Trigger vector = "<< m_triggers.size() );
  

  
  //get the lumiblocks
  if (m_doLumi == true) {
    
    if(isNewRun){
      if(m_UseTrigger) {
	//-REMI for(unsigned int i=0;i<m_triggers.size(); ++i) addMonGroup(new MonGroup(this,"egamma/Lumi/"+m_processName+"/"+m_triggers[i],shift,run));
	for(unsigned int i=0;i<m_triggers.size(); ++i) ATH_MSG_DEBUG("addMonGroup " << "egamma/Lumi/" << m_processName << "/" << m_triggers[i] );
      }
      //-REMI addMonGroup(new MonGroup(this,"egamma/Lumi/"+m_processName+"/all",shift,run));
      ATH_MSG_DEBUG("addMonGroup " << "egamma/Lumi/" << m_processName << "/all");
    }

    lumivec.clear();
    // Fill lumivec 
    double OffLum = IEgammaMonTool::getLumi(m_triggers,0,0,lumivec);
    ATH_MSG_DEBUG("physicsMonTool : "  << OffLum << " ");
  }

  if( isNewRun ) {   
    if(m_UseTrigger) {
      for(unsigned int i=0;i<m_triggers.size(); ++i){
	sc = bookPhysicsHistos(m_triggers[i]);
	if(sc != StatusCode::SUCCESS ) {
	  ATH_MSG_ERROR( "Error booking physics histos for trigger items.");
	  return sc;
	}
      }
    }
    
    for(unsigned int i=0;i<m_selections.size(); ++i){
      sc = bookPhysicsHistos(m_selections[i]);
      if(sc != StatusCode::SUCCESS ) {
	ATH_MSG_ERROR( "Error booking physics histos for id selecions.");
	return sc;
      }
    }
  }	
  return sc;
}

// ===========================================================
StatusCode physicsMonTool::fill() 
{
  //
  // method to fill histograms
  //

  StatusCode sc;
  MsgStream log( msgSvc(), name() );

  ATH_MSG_DEBUG(" fillHistograms()");





  ///Fill histograms for events passing trigger items
  if(m_UseTrigger) {
    for(unsigned int i=0;i<m_triggers.size(); ++i){
      if(m_trigDec->isPassed(m_triggers[i]) ) sc = fillHistogramsForItem(m_triggers[i]);      
      if(sc!=StatusCode::SUCCESS) ATH_MSG_ERROR( "Failed to fillHistogramsForItem: "<<m_triggers[i]);  
    }
  }

 ///Fill histograms for events passing specified selections
  for(unsigned int i=0;i<m_selections.size(); ++i){
    if(m_selections[i]=="all") sc = fillHistogramsForItem(m_selections[i]);
    if(sc!=StatusCode::SUCCESS) ATH_MSG_ERROR( "Failed to fillHistogramsForItem: "<<m_selections[i]);  
  }


    return sc;
}

// ===========================================================
StatusCode physicsMonTool::fillHistogramsForItem(const std::string &itemName) 
{
  //
  // method to fill histograms
  //

m_intEstLumi=0;

//check whether Lar signalled event bad
bool LarIsBad = this->hasBadLar();


  StatusCode sc(StatusCode::SUCCESS, true);
  MsgStream log( msgSvc(), name() );

  ATH_MSG_DEBUG(" fillHistogramsForItem() : "<<itemName);

  if (LarIsBad) return StatusCode::SUCCESS;

  setCurrentMonGroup("egamma/Physics/"+m_processName+"/"+itemName+"/Kinematics");

  std::string efficiencyGroup = "egamma/Physics/"+m_processName+"/"+itemName+"/Efficiency";

  ATH_MSG_DEBUG(" current group : "<<m_currentMonGroup);

  // Do physics histos filling
  const egammaContainer* electron_container=0;
  sc = m_storeGate->retrieve(electron_container, m_ElectronCnt);
  if(sc.isFailure()  ||  !electron_container){
    ATH_MSG_VERBOSE("no electron container found in TDS");
    return StatusCode::SUCCESS;
  }
  
  // iterators for egamma
  egammaContainer::const_iterator eg_iter    = electron_container->begin();
  egammaContainer::const_iterator eg_iterEnd = electron_container->end();



  ATH_MSG_DEBUG("Number of electrons: "<<electron_container->size());

  if(electron_container->size()<2) return sc;

  const egamma* leadingEl=*eg_iter;
  const egamma* secondEl=*eg_iter;
  float et1=-999, et2 = -999;
  ///Find leading and second Et candidates
  for (; eg_iter!=eg_iterEnd; eg_iter++){

    //Check Author 
    if ( !(*eg_iter)->author(egammaParameters::AuthorElectron)) continue;
    
    const CaloCluster * clus = (*eg_iter)->cluster();
    if(clus){      
      ATH_MSG_DEBUG("electron (eta,phi,et): ("
		    <<clus->eta()<<", "<<clus->phi()
		    <<", "<<clus->et()<<")");

      if(et1 < clus->et()) {
	// make current highest be new 2nd highest     
	secondEl = leadingEl;
	leadingEl = *eg_iter;
	et2 = et1;
	et1 = clus->et();
	ATH_MSG_DEBUG("fillHistogramsForItem(): Got leading cluster with"
	    <<" Et: "<<leadingEl->cluster()->et());
      }
      else if(et2 < clus->et() ) {
	secondEl = *eg_iter;
	et2 =  clus->et();
	ATH_MSG_DEBUG("fillHistogramsForItem(): Got second cluster"
	    <<" Et: "<<secondEl->cluster()->et());
      }
    }
  }
  /////////
  if(leadingEl!=secondEl){
  ATH_MSG_DEBUG("Leading electron (eta,phi,et): ("
      <<leadingEl->eta()<<", "<<leadingEl->phi()
      <<", "<<leadingEl->et()<<")");

  ATH_MSG_DEBUG("Second electron (eta,phi,et): ("
      <<secondEl->eta()<<", "<<secondEl->phi()
      <<", "<<secondEl->et()<<")");
  }
  else{
    ATH_MSG_DEBUG("Did not found second electron.");
    return sc;
  }
    

  float mass = (leadingEl->hlv()+secondEl->hlv()).m(); 
  hist("hMassCC")->Fill(mass);

  ATH_MSG_DEBUG("fillHistogramsForItem() electrons invariant mass: "
      <<mass<<" mass cuts: lower: "<<m_massLowerCut<<" upper: "<<m_massUpperCut
      <<" Et1: "<<leadingEl->cluster()->et()<<" cut1: "<<m_ElectronClusterEtCut
      <<" Et2: "<<secondEl->cluster()->et()<<" cut2: "<<m_ElectronClusterEtCut);

  ATH_MSG_DEBUG("egamma/Physics/"<<m_processName<<" "<<itemName<<"/Kinematics "<<mass);
  if(leadingEl->cluster()->et() > m_ElectronClusterEtCut &&
      secondEl->cluster()->et() > m_ElectronClusterEtCut &&
     mass > m_massLowerCut && mass < m_massUpperCut &&
     fabs(leadingEl->eta()) < 2.47 && fabs(secondEl->eta()) < 2.47) {
    if ( leadingEl->charge() * secondEl->charge() ==-1 ) {

    hist2("hLeadingClusterEtvsEta")->Fill(leadingEl->cluster()->eta(),leadingEl->cluster()->et());
    hist2("hLeadingClusterEtvsEta")->Fill(leadingEl->cluster()->phi(),leadingEl->cluster()->et());

    const Rec::TrackParticle * leadingElTrack = leadingEl->trackParticle();
    const Rec::TrackParticle * secondElTrack = secondEl->trackParticle();

    if(!secondElTrack)  ATH_MSG_WARNING(" Associated track not found for second electron");

    if(leadingElTrack){

      hist("hLeadingTrackPt")->Fill(leadingElTrack->pt());
      hist("hLeadingTrackEta")->Fill(leadingElTrack->eta());
      hist("hLeadingTrackPhi")->Fill(leadingElTrack->phi());
      hist2("hLeadingTrackPtvsEta")->Fill(leadingElTrack->eta(),leadingElTrack->pt());
      hist2("hLeadingTrackPtvsPhi")->Fill(leadingElTrack->phi(),leadingElTrack->pt());
    }
    else ATH_MSG_WARNING(" Associated track not found for leading electron");
    
    float massShifted = mass;
    
    ////Require Loose selection for one candidate, and Tight for the other.
    if( leadingEl->isem(egammaPID::ElectronTightPP) == 0 ||
	secondEl->isem(egammaPID::ElectronTightPP) == 0 ){

      ATH_MSG_DEBUG("fillHistogramsForItem() Candidates pass identification cuts");
//      float massShifted = mass-m_massShift;
      bool weight = true;
      hist("hMass")->Fill(massShifted, weight);

      //add event number
      const xAOD::EventInfo* p_evt = 0;
      int eventNumber,runNumber,lumiBlock,timeStamp;
      std::string eveN, runN, lumiB, timeS;
      if(m_storeGate->retrieve(p_evt).isSuccess() && p_evt!=0){
	eventNumber = p_evt->eventNumber();
	runNumber   = p_evt->runNumber();
	lumiBlock   = p_evt->lumiBlock();
	timeStamp   = p_evt->timeStamp();
	eveN = boost::lexical_cast<std::string>( eventNumber );//eventNumber.str();
	runN = boost::lexical_cast<std::string>( runNumber ); 
	lumiB = boost::lexical_cast<std::string>( lumiBlock );
	timeS = boost::lexical_cast<std::string>( timeStamp );
    //  std::string tmpx = hist("hEventNum")->GetXaxis()->GetTitle();
    //  tmpx = tmpx + "/" + eveN; 
    //  hist("hEventNum")->GetXaxis()->SetTitle(tmpx.c_str());
    //  std::string tmpy = hist("hEventNum")->GetYaxis()->GetTitle(); 
    //  tmpy = tmpy + "/" + lumiB;
    //  hist("hEventNum")->GetYaxis()->SetTitle(tmpy.c_str());
  

 
  }else {
    ATH_MSG_WARNING("Unable to retrieve EventInfo from StoreGate. Return failure.") ;
  }


      if(inBarrel(leadingEl->cluster()) && inBarrel(secondEl->cluster()))
	hist("hMassBB")->Fill(massShifted);

      if((inBarrel(leadingEl->cluster()) && inEndcap(secondEl->cluster())) ||
	 (inEndcap(leadingEl->cluster()) && inBarrel(secondEl->cluster())))
	hist("hMassBE")->Fill(massShifted);

      if((inBarrel(leadingEl->cluster()) && inCrack(secondEl->cluster())) ||
	 (inCrack(leadingEl->cluster()) && inBarrel(secondEl->cluster())))
	hist("hMassBC")->Fill(massShifted);

      weight = (inEndcap(leadingEl->cluster()) && inCrack(secondEl->cluster())) ||
        (inCrack(leadingEl->cluster()) && inEndcap(secondEl->cluster()));
      if (weight)
	hist("hMassEC")->Fill(massShifted);

      if(inEndcap(leadingEl->cluster()) && inEndcap(secondEl->cluster()))
	hist("hMassEE")->Fill(massShifted);
    }
    ////Tag & probe efficiency



    //    -----------------Getting number of primary vertices-------------

    //retrieve vertex container information
    const VxContainer* vertices;
    StatusCode scv = evtStore()->retrieve(vertices,"VxPrimaryCandidate");
    if (scv.isFailure()) {
      msg(MSG::ERROR) << "No Collection with name " << "VxPrimaryCandidate" <<" found in StoreGate" << endreq;
    } 
    else if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Collection with name " << "VxPrimaryCandidate" << " with size " << vertices->size() <<" found in StoreGate" << endreq;


    //iterators on vetex container
    VxContainer::const_iterator vxItr  = vertices->begin();
    VxContainer::const_iterator vxItrE = vertices->end();

    //define some variables
    int nPriVtx = 0;
    int nPileUpVtx=0;
    int nVtx=0;  

    //  loop on vertices
    for (; vxItr != vxItrE; ++vxItr) {
      if ((*vxItr)->vertexType() == Trk::PriVtx) nPriVtx++; //sum number of primary vertices
      if ((*vxItr)->vertexType() == Trk::PileUp) nPileUpVtx++;
      nVtx++;//sum total number of vertices
    }    
    //----------------------------------------------


    // Highest pT electron passes Tight
    if (leadingEl->isem(egammaPID::ElectronTightPP) == 0) {

      hist("hClusterEt")->Fill(secondEl->cluster()->et());
      hist("hClusterEta")->Fill(secondEl->cluster()->eta());
      hist("hClusterPhi")->Fill(secondEl->cluster()->phi());
      hist("hMassTA")->Fill(massShifted);

      if(secondEl->isem(egammaPID::ElectronLoosePP) == 0){
	hist("hLooseClusterEt")->Fill(secondEl->cluster()->et());
	hist("hLooseClusterEta")->Fill(secondEl->cluster()->eta());
	hist("hLooseClusterPhi")->Fill(secondEl->cluster()->phi());
	profile("hLooseIdEffVsEt",efficiencyGroup)->Fill(secondEl->cluster()->et(),1.);
	profile("hLooseIdEffVsEta",efficiencyGroup)->Fill(secondEl->cluster()->eta(),1.);
	profile("hLooseIdEffVsPhi",efficiencyGroup)->Fill(secondEl->cluster()->phi(),1.);
	profile("hLooseIdEffVsNOfPVertices",efficiencyGroup)->Fill(nPriVtx+nPileUpVtx,1.);
	hist("hMassTL")->Fill(massShifted);
      } else {
	profile("hLooseIdEffVsEt",efficiencyGroup)->Fill(secondEl->cluster()->et(),0.);
	profile("hLooseIdEffVsEta",efficiencyGroup)->Fill(secondEl->cluster()->eta(),0.);
	profile("hLooseIdEffVsPhi",efficiencyGroup)->Fill(secondEl->cluster()->phi(),0.);
	profile("hLooseIdEffVsNOfPVertices",efficiencyGroup)->Fill(nPriVtx+nPileUpVtx,0.);
      }

      if(secondEl->isem(egammaPID::ElectronMediumPP) == 0){
	hist("hMediumClusterEt")->Fill(secondEl->cluster()->et());
	hist("hMediumClusterEta")->Fill(secondEl->cluster()->eta());
	hist("hMediumClusterPhi")->Fill(secondEl->cluster()->phi());
	profile("hMediumIdEffVsEt",efficiencyGroup)->Fill(secondEl->cluster()->et(),1.);
	profile("hMediumIdEffVsEta",efficiencyGroup)->Fill(secondEl->cluster()->eta(),1.);
	profile("hMediumIdEffVsPhi",efficiencyGroup)->Fill(secondEl->cluster()->phi(),1.);
	profile("hMediumIdEffVsNOfPVertices",efficiencyGroup)->Fill(nPriVtx+nPileUpVtx,1.);
	hist("hMassTM")->Fill(massShifted);
      } else {
	profile("hMediumIdEffVsEt",efficiencyGroup)->Fill(secondEl->cluster()->et(),0.);
	profile("hMediumIdEffVsEta",efficiencyGroup)->Fill(secondEl->cluster()->eta(),0.);
	profile("hMediumIdEffVsPhi",efficiencyGroup)->Fill(secondEl->cluster()->phi(),0.);
	profile("hMediumIdEffVsNOfPVertices",efficiencyGroup)->Fill(nPriVtx+nPileUpVtx,0.);
      }

      if(secondEl->isem(egammaPID::ElectronTightPP) == 0){
	hist("hTightClusterEt")->Fill(secondEl->cluster()->et());
	hist("hTightClusterEta")->Fill(secondEl->cluster()->eta());
	hist("hTightClusterPhi")->Fill(secondEl->cluster()->phi());
	profile("hTightIdEffVsEt",efficiencyGroup)->Fill(secondEl->cluster()->et(),1.);
	profile("hTightIdEffVsEta",efficiencyGroup)->Fill(secondEl->cluster()->eta(),1.);
	profile("hTightIdEffVsPhi",efficiencyGroup)->Fill(secondEl->cluster()->phi(),1.);
	profile("hTightIdEffVsNOfPVertices",efficiencyGroup)->Fill(nPriVtx+nPileUpVtx,1.);
	hist("hMassTT")->Fill(massShifted);
      } else {
	profile("hTightIdEffVsEt",efficiencyGroup)->Fill(secondEl->cluster()->et(),0.);
	profile("hTightIdEffVsEta",efficiencyGroup)->Fill(secondEl->cluster()->eta(),0.);
	profile("hTightIdEffVsPhi",efficiencyGroup)->Fill(secondEl->cluster()->phi(),0.);
	profile("hTightIdEffVsNOfPVertices",efficiencyGroup)->Fill(nPriVtx+nPileUpVtx,0.);
      }

    }
    // Second Highest pT electron passes Tight
    if(secondEl->isem(egammaPID::ElectronTightPP) == 0){

      hist("hClusterEt")->Fill(leadingEl->cluster()->et());
      hist("hClusterEta")->Fill(leadingEl->cluster()->eta());
      hist("hClusterPhi")->Fill(leadingEl->cluster()->phi());
      hist("hMassTA")->Fill(massShifted);

      if(leadingEl->isem(egammaPID::ElectronLoosePP) == 0){
	hist("hLooseClusterEt")->Fill(leadingEl->cluster()->et());
	hist("hLooseClusterEta")->Fill(leadingEl->cluster()->eta());
	hist("hLooseClusterPhi")->Fill(leadingEl->cluster()->phi());
	profile("hLooseIdEffVsEt",efficiencyGroup)->Fill(leadingEl->cluster()->et(),1.);
	profile("hLooseIdEffVsEta",efficiencyGroup)->Fill(leadingEl->cluster()->eta(),1.);
	profile("hLooseIdEffVsPhi",efficiencyGroup)->Fill(leadingEl->cluster()->phi(),1.);
	profile("hLooseIdEffVsNOfPVertices",efficiencyGroup)->Fill(nPriVtx+nPileUpVtx,1.);
	hist("hMassTL")->Fill(massShifted);
      } else {
	profile("hLooseIdEffVsEt",efficiencyGroup)->Fill(leadingEl->cluster()->et(),0.);
	profile("hLooseIdEffVsEta",efficiencyGroup)->Fill(leadingEl->cluster()->eta(),0.);
	profile("hLooseIdEffVsPhi",efficiencyGroup)->Fill(leadingEl->cluster()->phi(),0.);
	profile("hLooseIdEffVsNOfPVertices",efficiencyGroup)->Fill(nPriVtx+nPileUpVtx,0.);
      }

      if(leadingEl->isem(egammaPID::ElectronMediumPP) == 0){
	hist("hMediumClusterEt")->Fill(leadingEl->cluster()->et());
	hist("hMediumClusterEta")->Fill(leadingEl->cluster()->eta());
	hist("hMediumClusterPhi")->Fill(leadingEl->cluster()->phi());
	profile("hMediumIdEffVsEt",efficiencyGroup)->Fill(leadingEl->cluster()->et(),1.);
	profile("hMediumIdEffVsEta",efficiencyGroup)->Fill(leadingEl->cluster()->eta(),1.);
	profile("hMediumIdEffVsPhi",efficiencyGroup)->Fill(leadingEl->cluster()->phi(),1.);
	profile("hMediumIdEffVsNOfPVertices",efficiencyGroup)->Fill(nPriVtx+nPileUpVtx,1.);
	hist("hMassTM")->Fill(massShifted);
      } else {
	profile("hMediumIdEffVsEt",efficiencyGroup)->Fill(leadingEl->cluster()->et(),0.);
	profile("hMediumIdEffVsEta",efficiencyGroup)->Fill(leadingEl->cluster()->eta(),0.);
	profile("hMediumIdEffVsPhi",efficiencyGroup)->Fill(leadingEl->cluster()->phi(),0.);
	profile("hMediumIdEffVsNOfPVertices",efficiencyGroup)->Fill(nPriVtx+nPileUpVtx,0.);
      }

      if(leadingEl->isem(egammaPID::ElectronTightPP) == 0){
	hist("hTightClusterEt")->Fill(leadingEl->cluster()->et());
	hist("hTightClusterEta")->Fill(leadingEl->cluster()->eta());
	hist("hTightClusterPhi")->Fill(leadingEl->cluster()->phi());
	profile("hTightIdEffVsEt",efficiencyGroup)->Fill(leadingEl->cluster()->et(),1.);
	profile("hTightIdEffVsEta",efficiencyGroup)->Fill(leadingEl->cluster()->eta(),1);
	profile("hTightIdEffVsPhi",efficiencyGroup)->Fill(leadingEl->cluster()->phi(),1.);
	profile("hTightIdEffVsNOfPVertices",efficiencyGroup)->Fill(nPriVtx+nPileUpVtx,1.);
	hist("hMassTT")->Fill(massShifted);
      } else {
	profile("hTightIdEffVsEt",efficiencyGroup)->Fill(leadingEl->cluster()->et(),0.);
	profile("hTightIdEffVsEta",efficiencyGroup)->Fill(leadingEl->cluster()->eta(),0.);
	profile("hTightIdEffVsPhi",efficiencyGroup)->Fill(leadingEl->cluster()->phi(),0.);
	profile("hTightIdEffVsNOfPVertices",efficiencyGroup)->Fill(nPriVtx+nPileUpVtx,0.);
      }

    }
    } else {
      
      // Same Sign Pairs
      if(leadingEl->isem(egammaPID::ElectronTightPP) == 0) {
	hist("hSSClusterEt")->Fill(secondEl->cluster()->et());
	hist("hSSClusterEta")->Fill(secondEl->cluster()->eta());
	
	if(secondEl->isem(egammaPID::ElectronLoosePP) == 0) {
	  hist("hSSLooseClusterEt")->Fill(secondEl->cluster()->et());
	  hist("hSSLooseClusterEta")->Fill(secondEl->cluster()->eta());
	}
      }
      if(secondEl->isem(egammaPID::ElectronTightPP) == 0) {
	hist("hSSClusterEt")->Fill(leadingEl->cluster()->et());
	hist("hSSClusterEta")->Fill(leadingEl->cluster()->eta());

	if(leadingEl->isem(egammaPID::ElectronLoosePP) == 0) {
	  hist("hSSLooseClusterEt")->Fill(leadingEl->cluster()->et());
	  hist("hSSLooseClusterEta")->Fill(leadingEl->cluster()->eta());
	}
      }

    }
  }
      

  //we only calculate the lumi if the visible cross-section is given (non zero)
  if (!m_doLumi) return sc;

  //separate mongroup with interval of lumiblocks.
  m_intEstLumi+=this->estLumi();
 
 for(unsigned int i=0;i<lumivec.size();i++) {
   
   setCurrentMonGroup("egamma/Lumi/"+m_processName+"/all");
   double OffLum=lumivec.at(i);
   
   //get lumi of this lumiblock
   ATH_MSG_DEBUG( "egamma/Lumi/"+m_processName+"/all" );
   ATH_MSG_DEBUG( "Triggers used " << m_triggers.size() );
   ATH_MSG_DEBUG( "Official Lumi " << OffLum );
   ATH_MSG_DEBUG( "Estimate Lumi " << this->estLumi());
   //estimated lumi by Z counting
   
   hist("hMass")->Fill((leadingEl->hlv()+secondEl->hlv()).m());
   hist("OffLumi")->Fill(OffLum);
   hist("EstLumi")->Fill(this->estLumi());
   hist2("LumiStable")->Fill(OffLum,this->estLumi());
   
   //reset the integrated lumi over the collected blocks
   m_intEstLumi=0;
 }      
 return sc;

}

// ===========================================================
StatusCode physicsMonTool::proc( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun ) 
{
  //
  // method to process
  //
  ATH_MSG_DEBUG("physicsMonTool::procHistograms()"
     <<" isEndOfEventsBlock: "<< isEndOfEventsBlock
     <<" isEndOfLumiBlock: "<<isEndOfLumiBlock
     <<" isEndOfRun: "<<isEndOfRun);

  return StatusCode( StatusCode::SUCCESS );
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
double physicsMonTool::estLumi()
{
//estimates luminosity on given physics process with known visible cross section
//and number of events measured. Lumi = N / visxs

//take histogram with least background
std::string group = "egamma/Physics/"+m_processName+"/all/Kinematics";
ATH_MSG_DEBUG("** " << group << "  -  "<< hist("hMass",group)->GetEntries() );
double lumiest = ((double) hist("hMass",group)->GetEntries())/m_visxs;

//estimate background contribution
//TODO

ATH_MSG_DEBUG("estLumi() returns " << lumiest << " for process " << m_processName);

return lumiest;
}




////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
bool physicsMonTool::passesIsEMCut(const unsigned int Cut , const egamma * eg){
  //  const unsigned int fullmask = egammaPID::ElectronTightPP;
  //  const unsigned int mask = eg->isem(fullmask);

  //  if( ( mask & Cut ) == 0 ) return true;
  //  return false;

  if (eg->isem(Cut) == 0) return true;

  return false;

}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
bool physicsMonTool::inBarrel(const CaloCluster * cluster) {
  if( fabs( cluster->eta() ) < 1.3 ) {
    return true;
  }
  return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
bool physicsMonTool::inEndcap(const CaloCluster * cluster) {
  if( fabs( cluster->eta() ) > 1.5 ) {
    return true;
  }
  return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
bool physicsMonTool::inCrack(const CaloCluster * cluster) {
  if( fabs( cluster->eta() ) > 1.3 &&
      fabs( cluster->eta() ) < 1.5 ) {
    return true;
  }
  return false;
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void physicsMonTool::divide(TH1 *num, TH1 *den, TH1 *quo) {

  num->Sumw2();
  den->Sumw2();
  quo->Divide(num, den, 1., 1., "B");
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
