/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//      Author: Kamal Benslama (University of Regina)
//      Update: Y. Ming (University of Regina)
//      egamma physics offline monitoring Class
//              University of Regina
//
//      date: Wed Jan  7 08:58:58 CET 2009
//
//
/////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include "egammaPerformance/TopphysicsMonTool.h"

#include "TrigSteeringEvent/Enums.h"
#include "egammaEvent/EMShower.h"

#include "egammaEvent/egamma.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronContainer.h"
#include "JetEvent/JetCollection.h"

#include "JetEvent/Jet.h"

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egDetailContainer.h"
#include "egammaEvent/egDetail.h"
//#include "MissingETEvent/MissingET.h"

#include "JetTagInfo/TruthInfo.h"
#include "JetTagInfo/BaseTagInfo.h"

#include "xAODEventInfo/EventInfo.h"

//to include the runnumber 
#include <boost/lexical_cast.hpp>

#include <sstream>

#include <sstream>
#include <math.h>

#include "TH2F.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TMath.h"
#include "TGraphAsymmErrors.h"


using CLHEP::GeV;
using CLHEP::pi;
using CLHEP::HepLorentzVector;
using xAOD::EventInfo;

//constructor
TopphysicsMonTool::TopphysicsMonTool( const std::string & type, const std::string & name,
	       const IInterface* parent )
  :IEgammaMonTool( type, name, parent ),
   m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"){

  declareProperty("ElectronContainer", m_ElectronCnt = "ElectronCollection" );
  declareProperty("UseTrigger",m_UseTrigger=true);
  declareProperty("Trigger_Items", m_triggers);
  declareProperty("Selection_Items", m_selections);
  declareProperty("ProcessName", m_processName);
  declareProperty("massShift",m_massShift =  91188);
  declareProperty("massElectronClusterEtCut",m_ElectronClusterEtCut = 15*GeV);
  declareProperty("massLowerCut",m_massLowerCut = 40*GeV);
  declareProperty("massUpperCut",m_massUpperCut = 140*GeV);
  declareProperty("metName", m_metName = "MET_Final" );
  declareProperty("LeadingElectronClusterPtCut", m_LeadingElectronClusterPtCut = 25.*GeV);
  declareProperty("MissingParticleEtCut", m_MissingParticleEtCut = 40.*GeV); 
  declareProperty("JetContainer", m_JetContainer = "AntiKt4TopoJets" );
  declareProperty("JetEnergyCut",m_JetEnergyCut = 30.);
  declareProperty("DeltaRCut",m_deltaRcut = 1.);
  declareProperty("triggerXselection",m_triggerXselection);
  declareProperty("TrigDecisionTool", m_trigDec, "Tool to access the trigger decision");
  // Name of the egDetail for shower
  declareProperty("EMShowerContainer",
                  m_EMShowerCnt = "egDetailContainer",
                  "Name of the egDetail for shower");
  //Boolean to switch on EventInfo::LAr checking
//  declareProperty("CheckLAr",
//                  m_checkLar,
//                  "Decision to check Lar EventInfo or not");

}

//destructor
TopphysicsMonTool::~TopphysicsMonTool(){}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode TopphysicsMonTool::bookPhysicsHistos(const std::string & // groupName 
						, const std::string & // subgroupName 
){

  MsgStream log( msgSvc(), name() );
  ATH_MSG_DEBUG("TopphysicsMonTool::bookPhysicsHistos()");
 
  ///Generic physics properties
  //-REMI if(groupName == "all" || groupName == "loose" ||groupName =="medium" || groupName =="tight")
  //-REMI addMonGroup(new MonGroup(this,"egamma/Physics/"+m_processName+"/"+groupName+"/Kinematics",shift,run));
  //-REMI else addMonGroup(new MonGroup(this,"egamma/Physics/"+m_processName+"/"+groupName+"_"+subgroupName+"/Kinematics",shift,run));
    //Probe cluster cluster

    addHistogram(new TH1F("MassWjj","E+4jets: Mass of the Wjj ; Mass [MeV]; Nevents",100, 50000, 160000));
    addHistogram(new TH1F("MassTjjj","E+4jets: Mass of the Tjjj ; Mass [MeV]; Nevents",100, 0, 350000));
    addHistogram(new TH1F("H_t1","E+4jets: H_{T} ;H_{T} [MeV]; Nevents",100, 0, 800000));
    addHistogram(new TH1F("H_t2","2E+2jets: H_{T} ;H_{T} [MeV]; Nevents",100, 0, 800000));

    addHistogram(new TH1F("TransMassW1","E+4jets: Mass_{Tran} of the Wenu ; Mass_{Tran} [MeV]; Nevents",100,0,150000));
    addHistogram(new TH1F("TransMassW2_1","2E+2jets: Mass_{Tran} of the Wenu ; Mass_{Tran} [MeV]; Nevents",100,0,150000));
    addHistogram(new TH1F("TransMassW2_2","2E+2jets: Mass_{Tran} of the Wenu ; Mass_{Tran} [MeV]; Nevents",100,0,150000));
 
    addHistogram(new TH1F("Elept1","E+4jets: Pt of the FIRST leading electron ;Pt_{ele} [MeV]; Nevents",100, 0, 200000));
    addHistogram(new TH1F("Elept2","2E+2jets: Pt of the FIRST leading electron ;Pt_{ele} [MeV]; Nevents",100, 0, 200000));

    addHistogram(new TH1F("Jetpt1","E+4jets: Pt of the FIRST leading jet candidate;Pt_{jet} [MeV]; Nevents",100, 0, 250000));
    addHistogram(new TH1F("Jetpt2","2E+2jets: Pt of the SECOND leading jet candidate;Pt_{jet} [MeV]; Nevents",100, 0, 250000));

    addHistogram(new TH1F("MissingEt1","E+4jets: MissingET; MissingET [MeV]; Nevents",100, 0, 250000));
    addHistogram(new TH1F("MissingEt2","2E+2jets: MissingET; MissingET [MeV]; Nevents",100, 0, 250000));

    addHistogram(new TH1F("ChargeProduct","2E+2jets: Product of the electron charge;Charge; Nevents",3, -1.5, 1.5));

    addHistogram(new TH1F("B_tag1","E+4jets: Btag of the FIRST leading jet ;bjet tag ; Nevents",6, -0.5, 5.5));

    hist("B_tag1")->GetXaxis()->SetBinLabel(1,"None");
    hist("B_tag1")->GetXaxis()->SetBinLabel(2,"IP3D_SV1");
    hist("B_tag1")->GetXaxis()->SetBinLabel(3,"SV0");
    hist("B_tag1")->GetXaxis()->SetBinLabel(4,"Prob");

  
    addHistogram(new TH1F("hIsEM1","E+4jets: First Leading IsEM ;IsEM flag;Nevents",4,-0.5,3.5));
    hist("hIsEM1")->GetXaxis()->SetBinLabel(1,"None");
    hist("hIsEM1")->GetXaxis()->SetBinLabel(2,"Loose");
    hist("hIsEM1")->GetXaxis()->SetBinLabel(3,"Medium");
    hist("hIsEM1")->GetXaxis()->SetBinLabel(4,"Tight");

    addHistogram(new TH1F("B_tag2","2E+2jets: Btag of the FIRST leading jet ;bjet tag ; Nevents",6, -0.5, 5.5));
    hist("B_tag2")->GetXaxis()->SetBinLabel(1,"None");
    hist("B_tag2")->GetXaxis()->SetBinLabel(2,"IP3D_SV1");
    hist("B_tag2")->GetXaxis()->SetBinLabel(3,"SV0");
    hist("B_tag2")->GetXaxis()->SetBinLabel(4,"Prob");


    addHistogram(new TH1F("hIsEM2_1","2E+2jets: First Leading IsEM ;IsEM flag;Nevents",4,-0.5,3.5));
    hist("hIsEM2_1")->GetXaxis()->SetBinLabel(1,"None");
    hist("hIsEM2_1")->GetXaxis()->SetBinLabel(2,"Loose");
    hist("hIsEM2_1")->GetXaxis()->SetBinLabel(3,"Medium");
    hist("hIsEM2_1")->GetXaxis()->SetBinLabel(4,"Tight");

    addHistogram(new TH1F("hIsEM2_2","2E+2jets: Second Leading IsEM ;IsEM flag;Nevents",4,-0.5,3.5));
    hist("hIsEM2_2")->GetXaxis()->SetBinLabel(1,"None");
    hist("hIsEM2_2")->GetXaxis()->SetBinLabel(2,"Loose");
    hist("hIsEM2_2")->GetXaxis()->SetBinLabel(3,"Medium");
    hist("hIsEM2_2")->GetXaxis()->SetBinLabel(4,"Tight");

    std::string tmp2 = "event_num;Num:; Lumi";
    addHistogram(new TH1F("hEventNum1",tmp2.c_str(),100,60000,120000));

    std::string tmp3 = "event_num;Num:; Lumi";
    addHistogram(new TH1F("hEventNum2",tmp3.c_str(),100,60000,120000));


    TGraph* m_graph = new TGraph();
    m_graph->SetNameTitle("eventNumber_E_4jets","eventNumber_E_4jets");
    m_graph->GetXaxis()->SetTitle("EventNumber");
    m_graph->GetYaxis()->SetTitle("LumiBlock");

    TGraph* m_graph2 = new TGraph();
    m_graph2->SetNameTitle("eventNumber_2E_2jets","eventNumber_2E_2jets");
    m_graph2->GetXaxis()->SetTitle("EventNumber");
    m_graph2->GetYaxis()->SetTitle("LumiBlock");

    addHistogram(m_graph);
    addHistogram(m_graph2);


    ///
    ///Mass histograms
//    float minMass = m_massLowerCut;
//    float maxMass = m_massUpperCut;


    return StatusCode::SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode TopphysicsMonTool::book( bool /*isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool isNewRun ){

  MsgStream log( msgSvc(), name() );
  StatusCode sc;
  if(m_UseTrigger) {
    sc = m_trigDec.retrieve();
    
    if( !sc.isSuccess() && m_triggers.size()) {
      ATH_MSG_DEBUG( "!! Unable to retrieve the TrigDecisionTool !!" );
      return sc;
    }
    ATH_MSG_DEBUG("  --> Found AlgTool \"TrigDecisionTool\"" );
  }
  ATH_MSG_DEBUG("TopphysicsMonTool::bookHistograms()\tDefining Monitoring Groups");

if (isNewRun) {
  if(m_UseTrigger) {
    //-REMI for(unsigned int i=0;i<m_triggers.size(); ++i) addMonGroup(new MonGroup(this,"egamma/Physics/"+m_processName+"/"+m_triggers[i],shift,run));
  }
  //-REMI for(unsigned int i=0;i<m_selections.size(); ++i) addMonGroup(new MonGroup(this,"egamma/Physics/"+m_processName+"/"+m_selections[i],shift,run));
}

  ATH_MSG_DEBUG("Size of Physics Trigger vector = "<< m_triggers.size() );

  if( isNewRun ) {   
    if(m_UseTrigger) {
      for(unsigned int i=0;i<m_triggers.size(); ++i){
	sc = bookPhysicsHistos(m_triggers[i],"");
	if(sc != StatusCode::SUCCESS ) {
	  ATH_MSG_ERROR( "Error booking physics histos for trigger items.");
	  return sc;
	}
      }
    }
    for(unsigned int i=0;i<m_selections.size(); ++i){
      sc = bookPhysicsHistos(m_selections[i],"");
      if(sc != StatusCode::SUCCESS ) {
	ATH_MSG_ERROR( "Error booking physics histos for id selecions.");
	return sc;
      }
    }
  }	
  return sc;
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode TopphysicsMonTool::fill(){

  StatusCode sc;

  ATH_MSG_DEBUG(" fillHistograms()");

//check whether Lar signalled event bad
if (this->hasBadLar() && m_checkLar) return StatusCode::SUCCESS;



  ///Fill histograms for events passing trigger items
  if(m_UseTrigger) {
    for(unsigned int i=0;i<m_triggers.size(); ++i){
      if(m_trigDec->isPassed(m_triggers[i]) ) sc = fillHistogramsForItem(m_triggers[i],"");      
      ATH_MSG_DEBUG( "Start to fillTriggerHistogramsForItem: "<<m_selections[i]);
      if(sc!=StatusCode::SUCCESS) ATH_MSG_ERROR( "Failed to fillHistogramsForItem: "<<m_triggers[i]);  
    }
  }

   
 ///Fill histograms for events passing specified selections
  for(unsigned int i=0;i<m_selections.size(); ++i){

      ATH_MSG_DEBUG( "Start to fillHistogramsForItem: "<<m_selections[i]);
      sc = fillHistogramsForItem(m_selections[i],"all");
    if(sc!=StatusCode::SUCCESS) ATH_MSG_ERROR( "Failed to fillHistogramsForItem: "<<m_selections[i]);  
  }

    return sc;
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode TopphysicsMonTool::fillHistogramsForItem(const std::string &, const std::string &){
    return StatusCode::SUCCESS;
    /*  -REMI
  StatusCode sc;
  MsgStream log( msgSvc(), name() );

  ATH_MSG_DEBUG( "START "<<itemName<<"  "<<selectionName);

  ATH_MSG_DEBUG(" fillHistogramsForItem() : "<<itemName);

   if(itemName == "all" || itemName == "loose" ||itemName =="medium" || itemName =="tight"){
     ATH_MSG_DEBUG( "This is for selection");
      setCurrentMonGroup("egamma/Physics/"+m_processName+"/"+itemName+"/Kinematics");}
   else {
      ATH_MSG_DEBUG( "This is for trigger and selection");     
      setCurrentMonGroup("egamma/Physics/"+m_processName+"/"+itemName+"_"+selectionName+"/Kinematics");}

  ATH_MSG_DEBUG(" current group : "<<m_currentMonGroup);

  // Do physics histos filling


  const egammaContainer* electron_container=0;
  sc = m_storeGate->retrieve(electron_container, m_ElectronCnt);

  if(sc.isFailure()  ||  !electron_container){
    ATH_MSG_DEBUG("no electron container found in TDS"
	);
    return StatusCode::SUCCESS;
  }



  // get met container from storegate
  const MissingET * final_met = 0;
  sc = m_storeGate->retrieve(final_met,m_metName);
    if (sc.isFailure()) {
    ATH_MSG_DEBUG("no met container found in TDS"
        ); 
      return StatusCode::SUCCESS;
    }


  // get jet container from storegate

  const JetCollection * jet_container = 0;
//to prevent overwriting events in AOD we copy jet_container by value
 std::vector<Jet>* copy_jet_container = new std::vector<Jet>; 


  sc = m_storeGate->retrieve(jet_container,m_JetContainer);
    if (sc.isFailure()) {
    ATH_MSG_DEBUG("no jet container found in TDS"
        );
      return StatusCode::SUCCESS;
    } 
   
   ATH_MSG_DEBUG( "Selected Jets successfully retrieved - size is " << jet_container->size() << " jets " );
 
//copy the jet_container entries by value
   //JetCollection::const_iterator jetIt  = jet_container->begin();

for(JetCollection::const_iterator jetIt=jet_container->begin() ;jetIt != jet_container->end(); jetIt++)
{
  copy_jet_container->push_back(**jetIt);
}
//WARNING: When returning this function must release the memory...


//std::cout << "jet container size " << copy_jet_container->size() << std::endl;
  
  // ******************
  // Get the missing ET
  // ******************
 double met = final_met->et();

  if(electron_container->size()<1) {
for(std::vector<Jet>::iterator jetItr_sel = copy_jet_container->begin();jetItr_sel != copy_jet_container->end(); jetItr_sel++)
{
  (*jetItr_sel).~Jet();
}

copy_jet_container->clear();
delete copy_jet_container;
copy_jet_container=0;

return sc;
}

  if(copy_jet_container->size()<4){ 
for(std::vector<Jet>::iterator jetItr_sel = copy_jet_container->begin();jetItr_sel != copy_jet_container->end(); jetItr_sel++)
{
  (*jetItr_sel).~Jet();
}

copy_jet_container->clear();
delete copy_jet_container;
copy_jet_container=0;

return sc;
}
  // ***************************
  // Start to loop on electrons
  // ****************************
  egammaContainer::const_iterator eg_iter    = electron_container->begin();
  egammaContainer::const_iterator eg_iterEnd = electron_container->end();

  ATH_MSG_DEBUG("whatissc"<<sc);
  ATH_MSG_DEBUG("Number of electrons: "<<electron_container->size());


  const egamma* leadingEl=*eg_iter;
  const egamma* secondEl=*eg_iter;
  float et1=-999, et2 = -999;
  ///Find leading and second Et candidates
  for (; eg_iter!=eg_iterEnd; eg_iter++){
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

//cut on electrons 

  bool electron1 = false;
  bool electron2 = false;
  
  bool electron1_medium = false; 
  bool electron2_medium = false; 

  int  author1 = -1;
  int  author2 = -1;

  double eta_ele1 = -99; 
  double eta_ele2 = -99; 
  double et_ele1 = -99;
  double et_ele2 = -99;

  double rEt40_1=100;
  double rEt40_2=100;

  double transmass1 = -999;
  double transmass2 = -999;

  electron1_medium = passesIsEMCut(egammaPID::ElectronMediumPP, leadingEl);
  author1 = leadingEl->author();
  eta_ele1 = leadingEl->cluster()->eta();
  et_ele1 = leadingEl->cluster()->et();
  const EMShower* shower_ele = leadingEl->detail<EMShower>(m_EMShowerCnt);
  if(shower_ele){
  double etcone40 = shower_ele->etcone40();
  rEt40_1 = et_ele1>0?etcone40/et_ele1:0;   
  }   

     float dphi1 = signedDeltaPhi(leadingEl->cluster()->phi(),final_met->phi());
     ATH_MSG_DEBUG("~dphi~ : "<<dphi1);
     transmass1 = TMath::Sqrt(2.*(leadingEl->cluster()->et())*(final_met->et())*(1.-TMath::Cos(dphi1)));
     ATH_MSG_DEBUG("~transmass~"<<transmass1);



  if(leadingEl!=secondEl){
  electron2_medium = passesIsEMCut(egammaPID::ElectronMediumPP, secondEl);
  author2 = secondEl->author(); 
  eta_ele2 = secondEl->cluster()->eta();
  et_ele2 = secondEl->cluster()->et();
  const EMShower* shower_ele2 = secondEl->detail<EMShower>(m_EMShowerCnt);
  if(shower_ele2){
  double etcone40 = shower_ele2->etcone40();
  rEt40_2 = et_ele2>0?etcone40/et_ele2:0;
  } 
     float dphi2 = signedDeltaPhi(leadingEl->cluster()->phi(),final_met->phi());
     ATH_MSG_DEBUG("~dphi~ : "<<dphi2);
     transmass2 = TMath::Sqrt(2.*(leadingEl->cluster()->et())*(final_met->et())*(1.-TMath::Cos(dphi2)));
     ATH_MSG_DEBUG("~transmass~"<<transmass2);
  }

  if(electron1_medium && (author1==1 || author1==3 ) && et_ele1>20000 && ((fabs(eta_ele1)<2.47) && (fabs(eta_ele1)<1.35 ||  fabs(eta_ele1)>1.52)) && fabs(rEt40_1)<0.2)electron1=true; 
  if(electron2_medium && (author2==1 || author2==3 ) && et_ele2>20000 && ((fabs(eta_ele2)<2.47) && (fabs(eta_ele2)<1.35 ||  fabs(eta_ele2)>1.52)) && fabs(rEt40_2)<0.2)electron2=true; 



  // ***************************
  // Start to do cut on jets
  // ****************************

//Copy the JetContainer to not change the AOD content from a pointer to the contents

  std::vector<Jet>::iterator  jetItr_sel  = copy_jet_container->begin();
  std::vector<Jet>::iterator  jetItrE_sel = copy_jet_container->end();

  if(jetItr_sel == jetItrE_sel){
for(std::vector<Jet>::iterator jetItr_sel = copy_jet_container->begin();jetItr_sel != copy_jet_container->end(); jetItr_sel++)
{
  (*jetItr_sel).~Jet();
}

copy_jet_container->clear();
delete copy_jet_container;
copy_jet_container=0;

 return sc; 
}
  int number_of_jets = 0;
  HepLorentzVector jet1;
  HepLorentzVector jet2;
  HepLorentzVector jet3;
  HepLorentzVector jet4;

  double Ht = 0;
  double Ht2 = 0;

  Ht = leadingEl->pt();

  if(secondEl ){
   if (leadingEl!=secondEl)Ht2 = leadingEl->pt() + secondEl->pt();
  }

  Jet MostEnergtic = *jetItr_sel;
  for (; jetItr_sel != jetItrE_sel; ++jetItr_sel) {
//the new jet edm
        double emJESfactor = (*jetItr_sel).getMoment("EMJES");
        Jet::hlv_t jet4MomJES =  emJESfactor * (*jetItr_sel).hlv(P4SignalState::JETEMSCALE);
        // Overwrite the final state in the copy only
	(*jetItr_sel).set4Mom(jet4MomJES);



    HepLorentzVector p4((*jetItr_sel).px(),
                        (*jetItr_sel).py(),
                        (*jetItr_sel).pz(),
                        (*jetItr_sel).e());
    // 	if((leadingEl->hlv().deltaR((*jetItr_sel)->hlv())>m_deltaRcut) && (p4.e()>m_JetEnergyCut)) return sc; 
    double eta = (*jetItr_sel).eta();
    double pt =  (*jetItr_sel).pt();
 
    if( fabs(eta)<2.47 && fabs(pt)>20000 ){
//first jet
    if(number_of_jets==0){
    jet1=p4;
    MostEnergtic = *jetItr_sel;
    ATH_MSG_DEBUG(number_of_jets<<"th jet's pt : "<<pt <<"  "<<p4.perp());
    }  
//second jet
    else if(number_of_jets==1){
    if(p4.perp()>jet1.perp()){jet2=jet1;jet1=p4;MostEnergtic = *jetItr_sel;}
    else jet2=p4;
    ATH_MSG_DEBUG(number_of_jets<<"th jet's pt : "<<pt);
    }  
//thrid jet
    else if(number_of_jets==2){
    if(p4.perp()>jet1.perp()){jet3=jet2;jet2=jet1;jet1=p4;MostEnergtic = *jetItr_sel;}
    else if(p4.perp()>jet2.perp()){jet3=jet2;jet2=p4;}
    else jet3=p4;
    ATH_MSG_DEBUG(number_of_jets<<"th jet's pt : "<<pt);
    }  
    else if (number_of_jets==3){
    if(p4.perp()>jet1.perp()){jet4=jet3;jet3=jet2;jet2=jet1;jet1=p4;MostEnergtic = *jetItr_sel;}
    else if(p4.perp()>jet2.perp()){jet4=jet3;jet3=jet2;jet2=p4;}
    else if (p4.perp()>jet3.perp()){jet4=jet3;jet3=p4;}
    else jet4=p4;
    ATH_MSG_DEBUG(number_of_jets<<"th jet's pt : "<<pt);
    } 
    else if (number_of_jets>3){
    if(p4.perp()>jet1.perp()){jet4=jet3;jet3=jet2;jet2=jet1;jet1=p4;MostEnergtic = *jetItr_sel;}
    else if(p4.perp()>jet2.perp()){jet4=jet3;jet3=jet2;jet2=p4;}
    else if (p4.perp()>jet3.perp()){jet4=jet3;jet3=p4;}
    else if (p4.perp()>jet4.perp())jet4=p4;
    ATH_MSG_DEBUG(number_of_jets<<"th jet's pt : "<<pt);
    }
 
//order the jets by pt
    number_of_jets++;  
    Ht = Ht+p4.perp();
    Ht2 = Ht2+p4.perp();
    }
  }

  //add event number
  const EventInfo* p_evt = 0;
  int eventNumber = 0,runNumber,lumiBlock = 0,timeStamp;
  std::string eveN, runN, lumiB, timeS;
  if(m_storeGate->retrieve(p_evt).isSuccess() && p_evt!=0){
    eventNumber = p_evt->eventNumber();
    runNumber = p_evt->runNumber();
    lumiBlock = p_evt->lumiBlock();
    timeStamp = p_evt->timeStamp();
  eveN = boost::lexical_cast<std::string>( eventNumber );//eventNumber.str();
  runN = boost::lexical_cast<std::string>( runNumber );
  lumiB = boost::lexical_cast<std::string>( lumiBlock );
  timeS = boost::lexical_cast<std::string>( timeStamp );

  }

  if( ((electron1 && (!electron2))||(electron2 && (!electron1)))  && number_of_jets >3 && jet1.perp()>40000 && met>25000){

   double mass=0;
   HepLorentzVector choosed_jets[3]; 
   choosed_jets[0]=jet2;choosed_jets[1]=jet3;choosed_jets[2]=jet4;  
   int w1=0;
   int w2=0;

  double mass_cut = 80400;
  double towMass2 = 0 ;
  double diffmass = 0;
  double mass_wjj = 0; 
   for(unsigned int i=0; i<3;i++)
   {
   for(unsigned int j=0; j<3;j++)
   {
  if(i==j)continue;
  towMass2 = (choosed_jets[i]+choosed_jets[j]).m() ;
  diffmass = fabs(towMass2-80400);
  if(diffmass<mass_cut)
   {
   mass_cut=diffmass;
   mass_wjj=towMass2;
   w1=i;
   w2=j;
   } 
   } 
   }//end loop for recon wjj

  if(w1<3 && w1>=0 && w2<3 && w2>=0){
   mass = (choosed_jets[w1] + choosed_jets[w2] + jet1).m(); 
  }
  //  double ip2d =MostEnergtic.getFlavourTagWeight("IP2D");
  //  double ip3d =MostEnergtic.getFlavourTagWeight("IP3D");
  double prob =MostEnergtic.getFlavourTagWeight("JetProb");
  double ip3d_sv1 = MostEnergtic.getFlavourTagWeight();

  double myTagValue = -999;
  const Analysis::BaseTagInfo* myInfo=dynamic_cast<const Analysis::BaseTagInfo*>(MostEnergtic.tagInfo("SV0"));
  if (myInfo) {
  myTagValue=myInfo->tagLikelihood()[0];
  }
  double sv0 = myTagValue; 
  
  hist("TransMassW1")->Fill(transmass1);
  hist("MassWjj")->Fill(mass_wjj);
  hist("MassTjjj")->Fill(mass);
  hist("H_t1")->Fill(Ht);
  hist("MissingEt1")->Fill(met);

  hist("Elept1")->Fill(leadingEl->pt());
  hist("Jetpt1")->Fill(MostEnergtic.pt()); 
  if(ip3d_sv1>4.0)hist("B_tag1")->Fill(1);
  if(sv0>6.4)hist("B_tag1")->Fill(2);
  if((-TMath::Log10(prob))>1.95)hist("B_tag1")->Fill(3); 

  std::string tmpx = hist("hEventNum1")->GetXaxis()->GetTitle();
  tmpx = tmpx + "/" + eveN;
  hist("hEventNum1")->GetXaxis()->SetTitle(tmpx.c_str());
  ATH_MSG_DEBUG("rec mass9 : "<<mass_wjj);
  std::string tmpy = hist("hEventNum1")->GetYaxis()->GetTitle();
  tmpy = tmpy + "/" + lumiB;
  hist("hEventNum1")->GetYaxis()->SetTitle(tmpy.c_str());
  
     hist("hIsEM1")->Fill( passesIsEMCut(egammaPID::ElectronLoosePP, leadingEl) + 
                         passesIsEMCut(egammaPID::ElectronMediumPP, leadingEl) + 
                         passesIsEMCut(egammaPID::ElectronTightPP, leadingEl)); 
  
  int num_1 = graph("eventNumber_E_4jets")->GetN(); 
  graph("eventNumber_E_4jets")->SetPoint(num_1,eventNumber,lumiBlock);
 }

  

  if(electron1 && electron2 && number_of_jets >1 && met>25000 && ( (leadingEl->charge())*(secondEl->charge()) ==-1)){
  double mass = (leadingEl->hlv()+secondEl->hlv()).m();  
  if ((mass>86000 && mass<96000)){
for(std::vector<Jet>::iterator jetItr_sel = copy_jet_container->begin();jetItr_sel != copy_jet_container->end(); jetItr_sel++)
{
  (*jetItr_sel).~Jet();
}

copy_jet_container->clear();
delete copy_jet_container;
copy_jet_container=0;

 return sc;
}

  //  double ip2d =MostEnergtic.getFlavourTagWeight("IP2D");
  //  double ip3d =MostEnergtic.getFlavourTagWeight("IP3D");
  double prob =MostEnergtic.getFlavourTagWeight("JetProb");
  double ip3d_sv1 = MostEnergtic.getFlavourTagWeight();

  double myTagValue = -999;
  const Analysis::BaseTagInfo* myInfo=dynamic_cast<const Analysis::BaseTagInfo*>(MostEnergtic.tagInfo("SV0"));
  if (myInfo) {
  myTagValue=myInfo->tagLikelihood()[0];
  }
  double sv0 = myTagValue;


  hist("TransMassW2_1")->Fill(transmass1);
  hist("TransMassW2_2")->Fill(transmass2);
  hist("H_t2")->Fill(Ht2);
  hist("MissingEt2")->Fill(met);
    
  hist("Elept2")->Fill(leadingEl->pt());
  hist("Jetpt2")->Fill(MostEnergtic.pt());

  if(ip3d_sv1>4.0)hist("B_tag2")->Fill(1);
  if(sv0>6.4)hist("B_tag2")->Fill(2);
  if((-TMath::Log10(prob))>1.95)hist("B_tag2")->Fill(3);
  
  
  std::string tmpx = hist("hEventNum2")->GetXaxis()->GetTitle();
  tmpx = tmpx + "/" + eveN;
  hist("hEventNum2")->GetXaxis()->SetTitle(tmpx.c_str());
  std::string tmpy = hist("hEventNum2")->GetYaxis()->GetTitle();
  tmpy = tmpy + "/" + lumiB;
  hist("hEventNum2")->GetYaxis()->SetTitle(tmpy.c_str());
  
     hist("hIsEM2_1")->Fill( passesIsEMCut(egammaPID::ElectronLoosePP, leadingEl) +
                         passesIsEMCut(egammaPID::ElectronMediumPP, leadingEl) +
                         passesIsEMCut(egammaPID::ElectronTightPP, leadingEl));

     hist("hIsEM2_2")->Fill( passesIsEMCut(egammaPID::ElectronLoosePP, secondEl) +
                         passesIsEMCut(egammaPID::ElectronMediumPP, secondEl) +
                         passesIsEMCut(egammaPID::ElectronTightPP, secondEl));


  int num_2 = graph("eventNumber_2E_2jets")->GetN();
  graph("eventNumber_2E_2jets")->SetPoint(num_2,eventNumber,lumiBlock);

  hist("ChargeProduct")->Fill(leadingEl->charge() * secondEl->charge());
  }



//delete the copy_jet_container

for(std::vector<Jet>::iterator jetItr_sel = copy_jet_container->begin();jetItr_sel != copy_jet_container->end(); jetItr_sel++)
{
  (*jetItr_sel).~Jet();
}

copy_jet_container->clear();
delete copy_jet_container;
copy_jet_container=0;


  return sc;
*/
}


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode TopphysicsMonTool::proc( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun ){
  MsgStream log( msgSvc(), name() );

  ATH_MSG_DEBUG("TopphysicsMonTool::procHistograms()"
     <<" isEndOfEventsBlock: "<< isEndOfEventsBlock
     <<" isEndOfLumiBlock: "<<isEndOfLumiBlock
     <<" isEndOfRun: "<<isEndOfRun);
  return StatusCode( StatusCode::SUCCESS );
}


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int TopphysicsMonTool::isWenu(const CaloCluster* em, double met) const {
  
  int selected = 2;
  
  if (em->pt() > m_LeadingElectronClusterPtCut ) --selected;
  if (met > m_MissingParticleEtCut ) --selected;

  return selected;
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
double TopphysicsMonTool::signedDeltaPhi(double phi1, double phi2) const {
  double phia=phi1;
  if(phi1>pi) phia=phi1-2.*pi;
  double phib=phi2;
  if(phi2>pi) phib=phi2-2.*pi;
  double dphi=phia-phib;
  if(dphi>pi) dphi-=2.*pi; 
  if(dphi<-pi) dphi+=2.*pi;
  return dphi;
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
bool TopphysicsMonTool::passesIsEMCut(const unsigned int Cut , const egamma * eg){
  const unsigned int fullmask = egammaPID::ElectronTightPP;
  const unsigned int mask = eg->isem(fullmask);

  if( ( mask & Cut ) == 0 ) return true;
  return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
bool TopphysicsMonTool::inBarrel(const CaloCluster * cluster) {
  if( fabs( cluster->eta() ) < 1.3 ) {
    return true;
  }
  return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
bool TopphysicsMonTool::inEndcap(const CaloCluster * cluster) {
  if( fabs( cluster->eta() ) > 1.5 ) {
    return true;
  }
  return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
bool TopphysicsMonTool::inCrack(const CaloCluster * cluster) {
  if( fabs( cluster->eta() ) > 1.3 &&
      fabs( cluster->eta() ) < 1.5 ) {
    return true;
  }
  return false;
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void TopphysicsMonTool::divide(TH1 *num, TH1 *den, TH1 *quo) {

   quo->Divide(num, den, 1., 1., "b");
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
