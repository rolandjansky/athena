/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//      Author: Kamal Benslama (University of Regina)
//      Update: E. ortega (Univesity of Regina)
//      Update: Y. Ming (University of Regina)
//      egamma physics offline monitoring Class
//      Jan 06, 2009 this class was created by A.Kalinowski by
//      splitting previous egammaMonTool class and adding new functionalities
//              University of Regina
//
//      date: Wed Jan  7 08:58:58 CET 2009
//
//
/////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include "egammaPerformance/ephysicsMonTool.h"

#include "TrigSteeringEvent/Enums.h"

#include "egammaEvent/egamma.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/ElectronContainer.h"
#include "JetEvent/JetCollection.h"
#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egDetailContainer.h"
#include "egammaEvent/egDetail.h"
//#include "MissingETEvent/MissingET.h"



#include <sstream>
#include <math.h>

#include "TH2F.h"
#include "TH1F.h"
#include "TMath.h"


using CLHEP::GeV;
using CLHEP::pi;
using CLHEP::HepLorentzVector;


//constructor
ephysicsMonTool::ephysicsMonTool( const std::string & type, const std::string & name,
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
  declareProperty("JetContainer", m_JetContainer = "Cone7TowerJets" );
  declareProperty("JetEnergyCut",m_JetEnergyCut = 30.);
  declareProperty("DeltaRCut",m_deltaRcut = 1.);
  declareProperty("triggerXselection",m_triggerXselection);
  declareProperty("TrigDecisionTool", m_trigDec, "Tool to access the trigger decision");
  declareProperty("EtRange", m_EtRange = 150*GeV, "Up limit for the Et distribution");

}

//destructor
ephysicsMonTool::~ephysicsMonTool(){}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode ephysicsMonTool::bookPhysicsHistos(const std::string & /* groupName */, const std::string & /* subgroupName */){

  MsgStream log( msgSvc(), name() );
  ATH_MSG_DEBUG("ephysicsMonTool::bookPhysicsHistos()");
 
    ///Generic physics properties
    /*-REMI
    if(groupName == "all" || groupName == "loose" ||groupName =="medium" || groupName =="tight")
          addMonGroup(new MonGroup(this,"egamma/Physics/"+m_processName+"/"+groupName+"/Kinematics",shift,run));
     else addMonGroup(new MonGroup(this,"egamma/Physics/"+m_processName+"/"+groupName+"_"+subgroupName+"/Kinematics",shift,run));
    */
    //Probe cluster cluster
    addHistogram(new TH1F("hClusterEt","E_{T} of leading cluster ;E_{T} [MeV]; Nevents",100, -1000, m_EtRange));
    addHistogram(new TH1F("hClusterEta","#eta of leading cluster ;#eta; Nevents",50, -2.5,2.5));
    addHistogram(new TH1F("hClusterPhi","#phi of leading cluster ;#phi; Nevents",50, -3.2,3.2));


    addHistogram(new TH2F("MtvsEta","Transverse mass of W vs #eta of leading electron ;#eta^{cluster}; M_{T} [GeV]",50, -2.5,2.5,100,m_massLowerCut,m_massUpperCut));
    addHistogram(new TH2F("MtvsPhi","Transverse mass of W vs #phi of leading electron ;#phi^{cluster}; M_{T} [GeV]",50, -3.2,3.2,100,m_massLowerCut,m_massUpperCut));
    addHistogram(new TH2F("MtvsdeltaPhi","Transverse mass of W vs #Delta#phi between leading electron and Missing ET;#Delta#phi; M_{T} [GeV]",100, -4.,4.,100,m_massLowerCut,m_massUpperCut));

    ///
    ///Mass histograms
    float minMass = m_massLowerCut;
    float maxMass = m_massUpperCut;

    std::ostringstream hName;
    hName<<"Transverse mass of  ";
    /* 
    std::string tmp = hName.str()+";m_{T} [MeV]; Nevents";
    addHistogram(new TH1F("hMass",tmp.c_str(),100,minMass,maxMass));
    tmp = hName.str()+" (Cluster in barrel);m_{T} [MeV]; Nevents";
    addHistogram(new TH1F("hMassB",tmp.c_str(),100,minMass,maxMass));

    tmp = hName.str()+" (Cluster in crack);m_{T} [MeV]; Nevents";
    addHistogram(new TH1F("hMassC",tmp.c_str(),100,minMass,maxMass));
    tmp = hName.str()+" (Cluster in endcap);m_{T} [MeV]; Nevents";
    addHistogram(new TH1F("hMassE",tmp.c_str(),100,minMass,maxMass));
    */
    std::string tmp = hName.str()+" (W);Transverse mass [GeV]; Nevents";
    addHistogram(new TH1F("hMassWenu",tmp.c_str(),100,minMass,maxMass));

    addHistogram(new TH1F("hIsEM","Wenu IsEM ;IsEM flag;Nevents",4,-0.5,3.5));
    hist("hIsEM")->GetXaxis()->SetBinLabel(1,"None");
    hist("hIsEM")->GetXaxis()->SetBinLabel(2,"Loose");
    hist("hIsEM")->GetXaxis()->SetBinLabel(3,"Medium");
    hist("hIsEM")->GetXaxis()->SetBinLabel(4,"Tight");
    return StatusCode::SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode ephysicsMonTool::book( bool /*isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool isNewRun ){

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
  ATH_MSG_DEBUG("ephysicsMonTool::bookHistograms()\tDefining Monitoring Groups");
  if (isNewRun){
    if(m_UseTrigger) {
      //-REMI for(unsigned int i=0;i<m_triggers.size(); ++i) addMonGroup(new MonGroup(this,"egamma/Physics/"+m_processName+"/"+m_triggers[i],shift,run));
    }
    //-REMI for(unsigned int i=0;i<m_selections.size(); ++i) addMonGroup(new MonGroup(this,"egamma/Physics/"+m_processName+"/"+m_selections[i],shift,run));
    
    ATH_MSG_DEBUG("Size of Physics Trigger vector = "<< m_triggers.size() );
  }

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
    for(unsigned int i=0;i<m_triggerXselection.size(); i=i+2){
      sc = bookPhysicsHistos(m_triggerXselection[i],m_triggerXselection[i+1]);
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
StatusCode ephysicsMonTool::fill(){

  bool LarIsBad = this->hasBadLar();

  StatusCode sc(StatusCode::SUCCESS, true);
  MsgStream log( msgSvc(), name() );

  ATH_MSG_DEBUG(" fillHistograms()");

  if (LarIsBad) return StatusCode::SUCCESS;//removes events if event is flagged bad due to LAr Noise

 
  ///Fill histograms for events passing trigger items
  if(m_UseTrigger) {
    for(unsigned int i=0;i<m_triggers.size(); ++i){
      if(m_trigDec->isPassed(m_triggers[i]) ) sc = fillHistogramsForItem(m_triggers[i],"");      
      ATH_MSG_DEBUG( "Start to fillTriggerHistogramsForItem: "<<m_selections[i]);
      if(sc!=StatusCode::SUCCESS) ATH_MSG_ERROR( "Failed to fillHistogramsForItem: "<<m_triggers[i]);  
    }
  }

  for(unsigned int i=0;i<m_triggerXselection.size();i=i+2){
    ATH_MSG_DEBUG( "Start to fill triggerXselection "<<i<<" "<<m_triggerXselection[i]<<m_triggerXselection[i+1]);
    if(m_triggerXselection[i]=="all" || m_triggerXselection[i]=="loose" || m_triggerXselection[i]=="medium" || m_triggerXselection[i]=="tight")
       sc = fillHistogramsForItem(m_triggerXselection[i],m_triggerXselection[i+1]);
    else
      if(m_UseTrigger) {
	if(m_trigDec->isPassed(m_triggerXselection[i]))
	  sc = fillHistogramsForItem(m_triggerXselection[i],m_triggerXselection[i+1]);
	if(sc!=StatusCode::SUCCESS) ATH_MSG_ERROR( "Failed to fillHistogramsForItem: "<<m_triggerXselection[i]<<"  "<<m_triggerXselection[i+1]);
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
StatusCode ephysicsMonTool::fillHistogramsForItem(const std::string &, const std::string &){
    return StatusCode::SUCCESS;
  /* -REMI

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
  sc = m_storeGate->retrieve(jet_container,m_JetContainer);
    if (sc.isFailure()) {
    ATH_MSG_DEBUG("no jet container found in TDS"
        );
      return StatusCode::SUCCESS;
    } 
   
   ATH_MSG_DEBUG( "Selected Jets successfully retrieved - size is " << jet_container->size() << " jets " );
 
  
  // ******************
  // Get the missing ET
  // ******************
   double met = final_met->et();

  if(electron_container->size()<1) return sc;
  //***************************
  // Start to loop on electrons
  //****************************
  egammaContainer::const_iterator eg_iter    = electron_container->begin();
  egammaContainer::const_iterator eg_iterEnd = electron_container->end();

  ATH_MSG_DEBUG("whatissc"<<sc);
  ATH_MSG_DEBUG("Number of electrons: "<<electron_container->size());


  const egamma* leadingEl=*eg_iter;
  //const egamma* secondEl=*eg_iter;
  float et1=-999, et2 = -999; float t_dphi = 0; float t_transmass = 0;
  ///Find leading and second Et candidates
  for (; eg_iter!=eg_iterEnd; eg_iter++){
    const CaloCluster * clus = (*eg_iter)->cluster();
    if(clus){      
      ATH_MSG_DEBUG("electron (eta,phi,et): ("
      <<clus->eta()<<", "<<clus->phi()
      <<", "<<clus->et()<<")");

      t_dphi = signedDeltaPhi(clus->phi(),final_met->phi());
      t_transmass = TMath::Sqrt(2.*(clus->et()/GeV)*(final_met->et()/GeV)*(1.-TMath::Cos(t_dphi)));

      if(et1 < clus->et() && (t_transmass > m_massLowerCut && t_transmass < m_massUpperCut)) {
	// make current highest be new 2nd highest     
	//secondEl = leadingEl;
	leadingEl = *eg_iter;
	et2 = et1;
	et1 = clus->et();
	ATH_MSG_DEBUG("fillHistogramsForItem(): Got leading cluster with"
	    <<" Et: "<<leadingEl->cluster()->et());
      }
      else if(et2 < clus->et() && (t_transmass > m_massLowerCut && t_transmass < m_massUpperCut) ) {
	//secondEl = *eg_iter;
	et2 =  clus->et();
	//ATH_MSG_DEBUG("fillHistogramsForItem(): Got second cluster"
	//    <<" Et: "<<secondEl->cluster()->et());
      }
    }
  }

  //***************************
  // Start to do cut on jets
  //****************************
  JetCollection::const_iterator jetItr_sel  = jet_container->begin();
  JetCollection::const_iterator jetItrE_sel = jet_container->end();
  for (; jetItr_sel != jetItrE_sel; ++jetItr_sel) {

    HepLorentzVector p4((*jetItr_sel)->px(),
                        (*jetItr_sel)->py(),
                        (*jetItr_sel)->pz(),
                        (*jetItr_sel)->e());
  
    ATH_MSG_DEBUG("deltaR is : "<< (leadingEl->hlv().deltaR((*jetItr_sel)->hlv()))<<" , jet energy is : "<<p4.e());
     	if((leadingEl->hlv().deltaR((*jetItr_sel)->hlv())>m_deltaRcut) && (p4.e()>m_JetEnergyCut)) return sc; 
  
  }


    int selected = isWenu(leadingEl->cluster(), met);
    ATH_MSG_DEBUG("electron pt is : "<< leadingEl->cluster()->pt()<<" , met is : "<<met);
    ATH_MSG_DEBUG("selected--"<<selected);

    bool Selectionflag = false;
    if(itemName=="all")
      Selectionflag = true;
    else if(itemName=="loose")
      Selectionflag=passesIsEMCut(egammaPID::ElectronLoosePP, leadingEl);
    else if(itemName=="medium")
      Selectionflag=passesIsEMCut(egammaPID::ElectronMediumPP, leadingEl);
    else if(itemName=="tight")
      Selectionflag=passesIsEMCut(egammaPID::ElectronTightPP, leadingEl);
    else{
        if(itemName=="all")
           Selectionflag = true;
         else if(selectionName=="loose")
           Selectionflag=passesIsEMCut(egammaPID::ElectronLoosePP, leadingEl);
         else if(selectionName=="medium")
           Selectionflag=passesIsEMCut(egammaPID::ElectronMediumPP, leadingEl);
          else 
           Selectionflag=passesIsEMCut(egammaPID::ElectronTightPP, leadingEl);
    }

    if (selected == 0 && Selectionflag) {
      double transmass = 0.;
      float dphi = signedDeltaPhi(leadingEl->cluster()->phi(),final_met->phi());
     ATH_MSG_DEBUG("~dphi~"<<dphi);
      transmass = TMath::Sqrt(2.*(leadingEl->cluster()->et()/GeV)*(final_met->et()/GeV)*(1.-TMath::Cos(dphi)));
     ATH_MSG_DEBUG("~transmass~"<<transmass);
     //hist("hMassWenu")->Fill(transmass); 
     HepLorentzVector leadinglv=leadingEl->cluster()->hlv();       
     double px = leadinglv.px();
     double py = leadinglv.py();
     double pt = sqrt(px*px+py*py);
     double etx = final_met->etx();
     double ety = final_met->ety();
     double et = sqrt(etx*etx+ety*ety);

     ATH_MSG_DEBUG(final_met->etx()<<"---"<<final_met->ety()<<"----"<<final_met->et()<<"---"<<final_met->sumet());
     ATH_MSG_DEBUG("pt"<<pt<<"===et_fromleading=="<<leadingEl->cluster()->et());
     ATH_MSG_DEBUG("et"<<et<<"===et_fromfina=="<<final_met->et());
     double transmass2=sqrt(2*fabs(pt*et-px*etx-py*ety))/GeV;
     hist("hMassWenu")->Fill(transmass2); 
     ATH_MSG_DEBUG("-transmassII-"<<transmass2);

     HepLorentzVector p4(final_met->etx(),
                         final_met->ety(),
                         0,
                         0);
     double delta_phi = leadingEl->hlv().getV().deltaPhi(p4.getV());
     ATH_MSG_DEBUG("-delta_phi-"<<delta_phi);

   
     hist("hIsEM")->Fill( passesIsEMCut(egammaPID::ElectronLoosePP, leadingEl) + 
     			 passesIsEMCut(egammaPID::ElectronMediumPP, leadingEl) + 
     			 passesIsEMCut(egammaPID::ElectronTightPP, leadingEl));    

     hist("hClusterEt")->Fill(leadingEl->cluster()->et());
     hist("hClusterEta")->Fill(leadingEl->cluster()->eta());
     hist("hClusterPhi")->Fill(leadingEl->cluster()->phi());
     hist2("MtvsEta")->Fill(leadingEl->cluster()->eta(),transmass2);
     hist2("MtvsPhi")->Fill(leadingEl->cluster()->phi(),transmass2);
     hist2("MtvsdeltaPhi")->Fill(delta_phi,transmass2);
    }
  return sc;
  */
}


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode ephysicsMonTool::proc( bool isEndOfEventsBlock, bool isEndOfLumiBlock, bool isEndOfRun ){
  MsgStream log( msgSvc(), name() );

  ATH_MSG_DEBUG("ephysicsMonTool::procHistograms()"
     <<" isEndOfEventsBlock: "<< isEndOfEventsBlock
     <<" isEndOfLumiBlock: "<<isEndOfLumiBlock
     <<" isEndOfRun: "<<isEndOfRun);
  return StatusCode( StatusCode::SUCCESS );
}


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
int ephysicsMonTool::isWenu(const CaloCluster* em, double met) const {
  
  int selected = 2;
  
  if (em->pt() > m_LeadingElectronClusterPtCut ) --selected;
  if (met > m_MissingParticleEtCut ) --selected;

  return selected;
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
double ephysicsMonTool::signedDeltaPhi(double phi1, double phi2) const {
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
bool ephysicsMonTool::passesIsEMCut(const unsigned int Cut , const egamma * eg){
  const unsigned int fullmask = egammaPID::ElectronTightPP;
  const unsigned int mask = eg->isem(fullmask);

  if( ( mask & Cut ) == 0 ) return true;
  return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
bool ephysicsMonTool::inBarrel(const CaloCluster * cluster) {
  if( fabs( cluster->eta() ) < 1.3 ) {
    return true;
  }
  return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
bool ephysicsMonTool::inEndcap(const CaloCluster * cluster) {
  if( fabs( cluster->eta() ) > 1.5 ) {
    return true;
  }
  return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
bool ephysicsMonTool::inCrack(const CaloCluster * cluster) {
  if( fabs( cluster->eta() ) > 1.3 &&
      fabs( cluster->eta() ) < 1.5 ) {
    return true;
  }
  return false;
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
void ephysicsMonTool::divide(TH1 *num, TH1 *den, TH1 *quo) {

   quo->Divide(num, den, 1., 1., "b");
}
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
