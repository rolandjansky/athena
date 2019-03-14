/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTClusterDiagnostics.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigHLTJetRec
//
// AUTHOR:   Ademar Delgado
//
// Description: Hypo for plotting Partial Scan vs Full Scan histograms
//
// ********************************************************************

#include "TrigHLTJetRec/TrigHLTPSvsFSDiagnostics.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"
#include <TProfile.h>
#include <cmath>
#include "TrigSteeringEvent/PhiHelper.h"

ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // legacy trigger code

TrigHLTPSvsFSDiagnostics::TrigHLTPSvsFSDiagnostics(const std::string& name, ISvcLocator* pSvcLocator):
HLT::HypoAlgo(name, pSvcLocator) {
  declareProperty("chainName", m_chainName = "unknown" );

  //!-- prepare automated histograms (see header) -- 
  std::string scanType[2]  = {"PS","FS"};
  std::string objType[3]   = {/*"Clust",*/"Jets","Jets_pass","focalJets"};
  std::string objTitle[3]  = {/*"clusters",*/"jets","Jets passing hypo","focal jets"};

  //!--fill names vectors --
  for(int i=0; i<2 ;++i){   //!-- beguin PS/FS loop
    m_scanType.push_back(scanType[i]);
  }
  for(int i=0; i<3 ;++i){ //!-- beguin Cluster/Jets loop
    m_objType.push_back(objType[i]);
    m_objTitle.push_back(objTitle[i]);	  
  }

  //!-- jet hypo threshold --
  m_hypoEt = 110000.;

  //!-- number of individual events to plot --
  m_nEvToPlot = 15;
}


TrigHLTPSvsFSDiagnostics::~TrigHLTPSvsFSDiagnostics() { }


HLT::ErrorCode TrigHLTPSvsFSDiagnostics::hltInitialize() {
  ATH_MSG_INFO("Initializing " << name() << "...");
  m_event = 0;
  // Open output root file
  std::string fileName = "PSvsFS_"+m_chainName+"_.root";
  m_fOut = TFile::Open(fileName.c_str(), "RECREATE");

  // Add histograms to map    
  //!-- PS/FS && Cluster/Jets histograms creation
  /** Loop to create the histograms for the options set before:
   - m_scanType[]  - name of the scans to perform, by default {"PS","FS"}
   - m_objType[] - name of the containers to check, by default {"Clust","Jets","Jets_pass","focalJets"}
   - m_objTitle[]- name of the containers to appear in the tytle, by default {"clusters","jets","Jets passing hypo", "focal jets"}
   **/
  addHist(m_hMap1D,"match_leadJet", "Check if the leading jets passing hypo match.;case ; entries", 2,   0.0,  2.0); 
  addHist(m_hMap2D,"efficiency_allJets", "Efficiency for all FS jets passing hypo.;Et (MeV) ; efficiency", 100,   0.0,  1000000.0,11,-1.1,1.1);  
  addHist(m_hMap2D,"efficiency_leadJet", "Efficiency for the leading jets passing hypo.;Et (MeV) ; efficiency", 100,   0.0,  1000000.0,11,-1.1,1.1);  

  //!focal checks
  addHist(m_hMap1D,"focal_has_leadJet", "Check if the leading jets is a focal jet.;case ; entries", 2,   0.0,  2.0);
  addHist(m_hMap1D,"deltaR_leadJet",   "Leading Jets deltaR (PS-FS); #deltaR; entries", 100, 0., 0.2);
  addHist(m_hMapProf,"deltaEtvsEta_focal", " Focal jets normalized deltaEt (PS-FS)/FS over FS eta; FS_eta; #deltaEt/FS_Et", 92, -4.9, 4.9);
  addHist(m_hMapProf,"deltaEtvsEta_leadJet", " Leading jets normalized deltaEt (PS-FS)/FS over FS eta; FS_eta; #deltaEa/FS_Et", 92, -4.9, 4.9);

  //!-- SuperRoI eta phi --
  addHist(m_hMap1D,"nTEs", "Number of TEs per event for L1_J20; number of TEs; entries", 50, 0., 50.);
  addHist(m_hMap1D,"SuperRoI_Total_Eta", "PS RoI eta distribution; #eta; entries", 99, -4.9, 4.9);
  addHist(m_hMap1D,"SuperRoI_Total_Phi", "PS RoI phi distribution; #phi; entries", 64, -3.2, 3.2);
  addHist(m_hMap2D,"PS_2D_SuperRoI","L1 RoIs found ; #eta;#phi",    92, -4.9, 4.9, 64, -3.2, 3.2);/*TMP*/

  for(unsigned int i=0; i<m_scanType.size() ;++i){   //!-- beguin PS/FS loop
    //!-- events accepted --
    //ex:          PS_nEvt_accepted - PS Number of events passing the hypo.;hypo ;nevents
    addHist(m_hMap1D,m_scanType.at(i)+"_nEvt_accepted", m_scanType.at(i)+" Number of events passing the hypo.;hypo ;nevents", 2,   0.0,  2.0);
    addHist(m_hMap1D,m_scanType.at(i)+"_deltaRoI_leadJet", m_scanType.at(i)+" deltaR to RoI center for leading jet.;case ; efficiency", 100,   0.0,  1.0);

    for(unsigned int j=0; j<m_objType.size() ;++j){ //!-- beguin Cluster/Jets loop
      //! If limits of histograms need to be changed, by default between Clusters and Jets or Jets passing hypo.
      //int limits[3][6];

      //!-- containers size --
      //ex:          PS_n_Cluster - PS Number of clusters per event ;number of jets ;nevents
      addHist(m_hMap1D,m_scanType.at(i)+"_n_"+m_objType.at(j), m_scanType.at(i)+" Number of "+m_objTitle.at(j)+" per event ;number of "+m_objTitle.at(j)+" ;nevents", 100,   0.0,  2000.0);

      //!-- Energy --
      //ex:          PS_E_Cluster - PS clusters energy distribution;Energy (MeV) ; entries
      addHist(m_hMap1D,m_scanType.at(i)+"_E_"+m_objType.at(j), m_scanType.at(i)+" "+m_objTitle.at(j)+" energy distribution;Energy (MeV) ; entries", 100,   0.0,  1000000.0);

      //!-- Et --
      //ex:          PS_Et_Cluster - PS clusters Et distribution;Et (MeV) ; entries
      addHist(m_hMap1D,m_scanType.at(i)+"_Et_"+m_objType.at(j), m_scanType.at(i)+" "+m_objTitle.at(j)+" Et distribution; Et (MeV); entries", 100,   0.0,  1000000.0);

      //!-- eta --
      //ex:          PS_Eta_Clust - PS Clusters eta distribution; #eta; entries
      addHist(m_hMap1D,m_scanType.at(i)+"_Eta_"+m_objType.at(j), m_scanType.at(i)+" "+m_objTitle.at(j)+" eta distribution; #eta; entries", 92, -4.9, 4.9);

      //!-- phi --
      //ex:          PS_Phi_Clust - PS Clusters phi distribution; #phi; entries
      addHist(m_hMap1D,m_scanType.at(i)+"_Phi_"+m_objType.at(j), m_scanType.at(i)+" "+m_objTitle.at(j)+" phi distribution; #phi; entries", 64, -3.2, 3.2);


      //!-- E vs Et --
      //ex:          PS_EvsEt_Clust - PS Clusters Energy vs Et;Et (MeV/c);Energy (MeV)
      addHist(m_hMap2D,m_scanType.at(i)+"_EvsEt_"+m_objType.at(j), m_scanType.at(i)+" "+m_objTitle.at(j)+" Energy vs Et;Et (MeV);Energy (MeV)", 100,  0.0,  1000000.0,  100,  0.0,  1000000.0);


      /** histograms only with: (comparing PS to FS)
			- m_objType[] - name of the containers to check, by default {"Clust","Jets","Jets_pass"}
			- m_objTitle[]- name of the containers to appear in the tytle, by default {"clusters","jets","Jets passing hypo"}
       **/
      if(i==0){
        //!-- deltaR --
        //ex:          tot_deltaR_Clust - Clusters deltaR (PS-FS); #deltaR; entries
        addHist(m_hMap1D,"tot_deltaR_"+m_objType.at(j), m_objTitle.at(j)+" deltaR (PS-FS); #deltaR; entries",          100, 0., 0.2);

        //!-- deltaR vs RoI center --
        //ex:          tot_deltaRvsCenter_Clust - Clusters normalized deltaR (PS-FS)/FS vs PS distance to RoI center; #delta to RoI center;#deltaR
        addHist(m_hMapProf,"tot_deltaRvsCenter_"+m_objType.at(j), m_objTitle.at(j)+" normalized deltaR (PS-FS)/FS vs PS distance to RoI center; #delta to RoI center;#deltaR", 100, 0., 1.5);

        //!-- deltaE vs RoI center --
        //ex:          tot_deltaEtvsCenter_Clust - Clusters normalized deltaE (PS-FS)/FS vs distance to RoI center; #delta to RoI center;#deltaE/FS_E
        addHist(m_hMapProf,"tot_deltaEtvsCenter_"+m_objType.at(j), m_objTitle.at(j)+" normalized absolute deltaEt (PS-FS)/FS vs distance to RoI center; #delta to RoI center;#deltaEt/FS_Et", 100, 0., 1.5);

        //!-- deltaEta vs Eta --
        //ex:          tot_deltaEtavsEta_Clust - Clusters normalized deltaEta (PS-FS)/FS over FS eta; FS_eta; #deltaEta/FS_Eta
        addHist(m_hMapProf,"tot_deltaEtavsEta_"+m_objType.at(j), m_objTitle.at(j)+" Clusters normalized deltaEta (PS-FS)/FS over FS eta; FS_eta; #deltaEta/FS_Eta", 92, -4.9, 4.9);

        //!-- deltaPhi vs Phi --
        //ex:          tot_deltaPhivsPhi_Clust - Clusters normalized deltaPhi (PS-FS)/FS over FS phi; FS_phi; #deltaPhi/FS_Phi
        addHist(m_hMapProf,"tot_deltaPhivsPhi_"+m_objType.at(j), m_objTitle.at(j)+" normalized deltaPhi (PS-FS)/FS over FS phi; FS_phi; #deltaPhi/FS_Phi", 64, -3.2, 3.2);

        //!-- deltaE vs E --
        //ex:          tot_deltaEtvsEt_Clust - Clusters normalized deltaE (PS-FS)/FS over FS E; FS_Et; #deltaE/FS_E
        addHist(m_hMap1D,"1D_tot_deltaEtvsEt_"+m_objType.at(j), m_objTitle.at(j)+" normalized deltaEt (PS-FS)/FS over FS Et; #deltaEt/FS_Et; entries", 100, 0., 5.);
        addHist(m_hMapProf,"tot_deltaEtvsEt_"+m_objType.at(j), m_objTitle.at(j)+" normalized deltaEt (PS-FS)/FS over FS Et; FS_Et; #deltaEt/FS_Et", 100, 0., 1000000.);
      }

      /** Loop to create the histograms PER EVENT for the options set before:
			- m_scanType[]  - name of the scans to perform, by default {"PS","FS"}
			- m_objType[] - name of the containers to check, by default {"Clust","Jets","Jets_pass"}
			- m_objTitle[]- name of the containers to appear in the tytle, by default {"clusters","jets","Jets passing hypo"}
			- m_nEvToplot - number of events to plot, by default m_nEvToplot = 15
       **/

      for(int k=0; k<m_nEvToPlot; ++k){
        //!-- 2D position height in E --
        //ex:          PS_2D_Clust - PS Clusters created evt-1; ; #eta;#phi;Energy (MeV)
        addHist(m_hMap2D,histStr(m_scanType.at(i)+"_2D_"+m_objType.at(j)+"_",k+1,""),histStr(m_scanType.at(i)+" "+m_objTitle.at(j)+" created evt-",k+1,"; #eta;#phi;Et (MeV)").c_str(),    92, -4.9, 4.9, 64, -3.2, 3.2);
      }
    }  //!-- end Cluster/Jets loop
  }    //!-- end PS/FS loop


  /* old temporary histograms
  addHist(m_hMap1D,histStr("_deltaR_").c_str(), histStr("Clusters deltaR PS to FS evt-","; #deltaR; entries").c_str(), 100, 0., 0.2);
  addHist(m_hMap2D,histStr("_MatchClusers_").c_str(), histStr("Match Clusters evt-","; #eta;#phi;Energy (MeV)").c_str(),   200, -5., 5., 128, -3.1416, 3.1416);  
  addHist(m_hMapProf,histStr("_deltaRtoCenter_").c_str(), histStr("Clusters deltaR PS to FS vs distance to RoI center evt-","; #delta to RoI center;#deltaR").c_str(), 100, 0., 1.5);
  addHist(m_hMapProf,histStr("_deltaEtoCenter_").c_str(), histStr("Clusters deltaE PS to FS vs distance to RoI center evt-","; #delta to RoI center;#deltaE (MeV)").c_str(), 100, 0., 1.5);

   */
  return HLT::OK; 
}


HLT::ErrorCode TrigHLTPSvsFSDiagnostics::hltFinalize(){
  ATH_MSG_INFO ("Finalizing " << name() << "...");  
  // Save histograms and close file 
  m_fOut->Write();
  m_fOut->Close();
  // Clear histogram maps
  m_hMap2D.clear();
  m_hMap1D.clear();
  m_hMapProf.clear();
  return HLT::OK;
}


// Add 1D histograms to map
void TrigHLTPSvsFSDiagnostics::addHist(std::map<TString,TH1D*> &hMap, TString label, TString title,
    const int &bins, double min, double max) {
  hMap[label] = new TH1D(label, title, bins, min, max);
}

// Add 2D histograms to map
void TrigHLTPSvsFSDiagnostics::addHist(std::map<TString,TH2D*> &hMap, TString label, TString title, 
    const int &binsX, double minX, double maxX, const int &binsY, double minY, double maxY) {
  hMap[label] = new TH2D(label, title, binsX, minX, maxX, binsY, minY, maxY);
}

// Add profile histograms to map
void TrigHLTPSvsFSDiagnostics::addHist(std::map<TString,TProfile*> &hMap, TString label, TString title, const int &binsX, double minX, double maxX){
  hMap[label] = new TProfile(label, title, binsX, minX, maxX, "s");
}

// Ensure that phi is between -pi and pi
double TrigHLTPSvsFSDiagnostics::fixPhi(double phi) {
  while(phi >=  M_PI) phi -= 2.0*M_PI;
  while(phi <  -M_PI) phi += 2.0*M_PI;
  return phi;	 
}


HLT::ErrorCode TrigHLTPSvsFSDiagnostics::hltExecute(const HLT::TriggerElement* inputTE, bool& pass) {
  ATH_MSG_DEBUG("Executing " << name() << "...");
  m_event++;
  pass = true;

  //TODO inpruve load containers
  //! right now the container key need to be manually updated
  //! right now only works in PS chain and because FS chain runs before. ( booth containers exist at the end os PS chain.)
  bool haveContainers = loadContainers(inputTE);


  // Get JetContainer from input trigger element
  /*const xAOD::JetContainer* j_container = 0;
  HLT::ErrorCode hltStatus = getFeature(inputTE, j_container);  
  if (hltStatus == HLT::OK) {
    ATH_MSG_DEBUG("Retrieved JetContainer, address" << j_container);
  } else {
    ATH_MSG_ERROR("Failed to retrieve JetContainer.");
    return hltStatus;
  }
  if (j_container == 0){
    ATH_MSG_INFO("JetContainer address = 0, giving up");
    return HLT::OK;
  }

  //Find conteiner key 
  std::string jetCollKey;
  if ( getStoreGateKey( j_container, jetCollKey) == HLT::OK) {
    ATH_MSG_DEBUG("TEST::Key - jetContainer key: " << jetCollKey);    
  }*/

  if(haveContainers){
    //clustersCheck();
    jetsCheck();
  }

  return HLT::OK;
}

std::string TrigHLTPSvsFSDiagnostics::histStr(std::string preName){
  std::ostringstream tmp_str;
  tmp_str<<preName<<m_event;

  //std::printf("TEST::histStr - tmp_str=%s \n",tmp_str.str().c_str());
  return tmp_str.str().c_str();
}
std::string TrigHLTPSvsFSDiagnostics::histStr(std::string preName, std::string postname){
  std::ostringstream tmp_str;
  tmp_str<<preName<<m_event<<postname;

  //std::printf("TEST::histStr - tmp_str=%s \n",tmp_str.str().c_str());
  return tmp_str.str();
}
std::string TrigHLTPSvsFSDiagnostics::histStr(std::string preName, int number, std::string postname){
  std::ostringstream tmp_str;
  tmp_str<<preName<<number<<postname;

  //std::printf("TEST::histStr - tmp_str=%s \n",tmp_str.str().c_str());
  return tmp_str.str();
}


bool TrigHLTPSvsFSDiagnostics::loadContainers(const HLT::TriggerElement* inputTE){
  m_havePSClustContainer = false;
  m_haveFSClustContainer = false;
  m_haveSuperRoi         = false;
  m_havePSJetContainer   = false;
  m_haveFSJetContainer   = false;
  std::string PSclustCollKey = "HLTAutoKey_TrigCaloClusterMaker_1219821989_2_to_0";
  std::string FSclustCollKey = "HLTAutoKey_TrigCaloClusterMaker_topo_fullscan_1219821989_3_to_0";
  std::string PSjetCollKey = "HLTAutoKey_TrigHLTJetRec_1244316195_0_to_0";
  std::string FSjetCollKey = "HLTAutoKey_TrigHLTJetRec_1244316195_0_to_1";

  // const xAOD::CaloClusterContainer* PSclusterCont; 
  // const xAOD::CaloClusterContainer* FSclusterCont;

  //----------------------------------------------------------------------------
  //! Find keys:
  const xAOD::CaloClusterContainer* c_container = 0;
  const xAOD::JetContainer*         j_container = 0;
  HLT::ErrorCode hltStatus = getFeature(inputTE, c_container);
  hltStatus = getFeature(inputTE, j_container);  
  std::string c_CollKey, j_CollKey;
  if ( getStoreGateKey( c_container, c_CollKey) == HLT::OK) {
    ATH_MSG_DEBUG("TEST::Key - "<<name() << " clustersContainer key: " << c_CollKey);   
  }  
  if ( getStoreGateKey( j_container, j_CollKey) == HLT::OK) {
    ATH_MSG_DEBUG("TEST::Key - "<<name() << " jetContainer key: " << j_CollKey);   
  }
  //----------------------------------------------------------------------------

  if (!evtStore()->contains<xAOD::JetContainer>(FSjetCollKey)) {
    ATH_MSG_DEBUG("TEST::Container - Didn't found any xAOD::JetContainer with Key"<<PSjetCollKey);
    return false;
  }

  //! Not working in the moment
  /*
  if((evtStore()->retrieve(m_PSclusterCont, PSclustCollKey)).isSuccess()) {
    ATH_MSG_DEBUG("TEST::Container - retrieve partial cluster container with size="<<m_PSclusterCont->size());
    m_havePSClustContainer=true;
  }
  if((evtStore()->retrieve(m_FSclusterCont, FSclustCollKey)).isSuccess()) {
    ATH_MSG_DEBUG("TEST::Container - retrieve full cluster container with size="<<m_FSclusterCont->size());
    m_haveFSClustContainer=true;
  }
   */

  if((evtStore()->retrieve(m_PS_j_container, PSjetCollKey)).isSuccess()) {
    ATH_MSG_DEBUG("TEST::Container - retrieve partial jet container with size="<<m_PS_j_container->size());
    m_havePSJetContainer=true;
  }
  if((evtStore()->retrieve(m_FS_j_container, FSjetCollKey)).isSuccess()) {
    ATH_MSG_DEBUG("TEST::Container - retrieve full jet container with size="<<m_FS_j_container->size());
    m_haveFSJetContainer=true;
  }

  if(name().find("_PS") != std::string::npos){ 
    if(getFeature(inputTE, m_superRoi, "") == HLT::OK && m_superRoi->composite() ) {
      m_haveSuperRoi=true;
      ATH_MSG_DEBUG("TEST::Container - retrieve superRoi with size="<<m_superRoi->size());
    }
  }else {
    ATH_MSG_DEBUG("TEST::ERROR - PSvsFS disgnostics - retrieve superRoI from evtstore not implemented yet!!!!");
    return false;
  }

  return true;
}


void TrigHLTPSvsFSDiagnostics::clustersCheck(){
  xAOD::CaloClusterContainer::const_iterator PS_clusterIter  = m_PSclusterCont->begin();
  xAOD::CaloClusterContainer::const_iterator FS_clusterIter  = m_FSclusterCont->begin();
  xAOD::CaloClusterContainer::const_iterator PS_clusterIterEnd = m_PSclusterCont->end();
  xAOD::CaloClusterContainer::const_iterator FS_clusterIterEnd = m_FSclusterCont->end();
  xAOD::CaloClusterContainer::const_iterator tmpClusterIter;

  //! number of clusters
  m_hMap1D["PS_n_Clust"]->Fill(m_PSclusterCont->size());
  m_hMap1D["FS_n_Clust"]->Fill(m_FSclusterCont->size());

  double delta_R = 0.;
  int PS=0;
  int FS=0;
  //!-- starting loop over PS clusters --
  for(; PS_clusterIter!=PS_clusterIterEnd; ++PS_clusterIter){
    const xAOD::CaloCluster* PS_Cluster = *PS_clusterIter;
    const xAOD::CaloCluster* FSmatchCluster = new xAOD::CaloCluster();

    double min_delta_R = 100000.;
    tmpClusterIter=FS_clusterIter;
    //!-- starting loop over FS clusters --
    for(; tmpClusterIter!=FS_clusterIterEnd;++tmpClusterIter){
      const xAOD::CaloCluster* FS_Cluster = *tmpClusterIter;

      //!Compute delta R
      double delta_eta = PS_Cluster->eta() - FS_Cluster->eta();
      double delta_phi = PS_Cluster->phi() - FS_Cluster->phi();
      double delta_phiCorr = HLT::wrapPhi(delta_phi);
      delta_R = std::sqrt((delta_eta*delta_eta) + (delta_phiCorr*delta_phiCorr));

      //!Found the closest FS cluster to the PS one in evaluation
      if(delta_R<min_delta_R){
        min_delta_R=delta_R;
        FSmatchCluster=FS_Cluster;
      }

      //!Plot FS clusters observables
      if(FS==0){
        m_hMap1D["FS_E_Clust"]->Fill(FS_Cluster->e());
        m_hMap1D["FS_Et_Clust"]->Fill((FS_Cluster->p4()).Et());
        m_hMap1D["FS_Eta_Clust"]->Fill(FS_Cluster->eta());
        m_hMap1D["FS_Phi_Clust"]->Fill(FS_Cluster->phi());
        m_hMap2D["FS_EvsEt_Clust"]->Fill((FS_Cluster->p4()).Et(), FS_Cluster->e());

        //!Plot all FS clusters for the first 15 events
        if(m_event<15){
          m_hMap2D[histStr("FS_2D_Clust_").c_str()]->Fill(FS_Cluster->eta(),FS_Cluster->phi(),(FS_Cluster->p4()).Et());
        }
      }
    }//!-- finish loop over FS clusters --

    //!Plot PS clusters observables
    m_hMap1D["PS_E_Clust"]->Fill(PS_Cluster->e());
    m_hMap1D["PS_Et_Clust"]->Fill((PS_Cluster->p4()).Et());
    m_hMap1D["PS_Eta_Clust"]->Fill(PS_Cluster->eta());
    m_hMap1D["PS_Phi_Clust"]->Fill(PS_Cluster->phi());
    m_hMap2D["PS_EvsEt_Clust"]->Fill((PS_Cluster->p4()).Et(), PS_Cluster->e());

    //!Plot all PS clusters for the first 15 events
    if(m_event<15){
      //!Print closests clusters
      /*std::printf("TEST::CLUST - min_delta_R=%f \n",min_delta_R);
      if(min_delta_R<0.005){
        m_hMap2D[histStr("_MatchClusers_").c_str()]->Fill(PSCluster->eta(),PSCluster->phi(),PSCluster->e()); 
      }else{
      	std::printf("TEST::CLUST - PSCluster - evt=%d - pt=%f, eta=%f  phi=%f m=%f e=%f rapidity=%f \n",m_event,PSCluster->pt(),PSCluster->eta(),PSCluster->phi(),PSCluster->m(),PSCluster->e(),PSCluster->rapidity());
        std::printf("TEST::CLUST - FSCluster - evt=%d - pt=%f, eta=%f  phi=%f m=%f e=%f rapidity=%f \n",m_event,FSmatchCluster->pt(),FSmatchCluster->eta(),FSmatchCluster->phi(),FSmatchCluster->m(),FSmatchCluster->e(),FSmatchCluster->rapidity());
        std::printf("TEST::CLUST ----------------\n");
      }*/
      //!Delta R per event
      m_hMap2D[histStr("PS_2D_Clust_").c_str()]->Fill(PS_Cluster->eta(),PS_Cluster->phi(),(PS_Cluster->p4()).Et());
    }

    //! PS to FS deltaR profile plot
    m_hMap1D["tot_deltaR_Clust"]->Fill(min_delta_R);

    //!Fix for TProfile show 0.000 entries
    double deltaEta     = (PS_Cluster->eta()-FSmatchCluster->eta())/FSmatchCluster->eta();
    double deltaPhi     = (PS_Cluster->phi()-FSmatchCluster->phi())/FSmatchCluster->phi();
    double deltaEt      = ((PS_Cluster->p4()).Et()-(FSmatchCluster->p4()).Et())/(FSmatchCluster->p4()).Et();
    double fix_deltaEta = deltaEta!=0?deltaEta:0.0000000001;
    double fix_deltaPhi = deltaPhi!=0?deltaPhi:0.0000000001;    
    //double fix_deltaE   = deltaE!=0?deltaE:0.0000000001;

    m_hMapProf["tot_deltaEtavsEta_Clust"]->Fill(FSmatchCluster->eta(),fix_deltaEta);    
    m_hMapProf["tot_deltaPhivsPhi_Clust"]->Fill(FSmatchCluster->phi(),fix_deltaPhi);   
    m_hMapProf["tot_deltaEtvsEt_Clust"]->Fill((FSmatchCluster->p4()).Et(),deltaEt);
    m_hMap1D["1D_tot_deltaEtvsEt_Clust"]->Fill(deltaEt);

    //!--- deltaR, deltaE to RoI center -------------------------
    //! loop over superRoI RoI's
    if(m_haveSuperRoi && m_superRoi->size()>0 ){
      double min_deltaRoI = 100000.;
      double tmp_deltaRoI;
      for(unsigned int i=0; i<m_superRoi->size(); i++){
        double delta_eta = PS_Cluster->eta() - m_superRoi->at(i)->eta();
        double delta_phi = PS_Cluster->phi() - m_superRoi->at(i)->phi();
        double delta_phiCorr = HLT::wrapPhi(delta_phi);
        tmp_deltaRoI = std::sqrt((delta_eta*delta_eta) + (delta_phiCorr*delta_phiCorr));

        if(tmp_deltaRoI<min_deltaRoI){
          min_deltaRoI=tmp_deltaRoI;
        }

        // if(FS==0 && m_event<15){
        // m_superRoiPos->Fill(m_superRoiEta.at(i),m_superRoiPhi.at(i));
        // }
      }

      //!Fix for TProfile show 0.000 entries
      double fix_delta_R = min_delta_R!=0?min_delta_R:0.0000000001;
      double fabs_Et = fabs(((PS_Cluster->p4()).Et()-(FSmatchCluster->p4()).Et())/(FSmatchCluster->p4()).Et());
      if(fabs_Et==0.0){
        fabs_Et= 0.0000000001;
      }

      m_hMapProf["tot_deltaRvsCenter_Clust"]->Fill(min_deltaRoI,fix_delta_R);
      m_hMapProf["tot_deltaEtvsCenter_Clust"]->Fill(min_deltaRoI,fabs_Et);
    }
    //----------------------------------------------------------


    PS++;
    FS++;
    
    delete FSmatchCluster;
  }//!-- finish loop over PS clusters --

  //std::printf("TEST::CLUST - PSvsFS - evt=%d - PScluster->size()=%d, PS=%d  FScluster->size()=%d FS=%d \n",m_event,PSclusterCont->size(),PS,FSclusterCont->size(),FS);
}


void TrigHLTPSvsFSDiagnostics::jetsCheck(){
  xAOD::JetContainer::const_iterator PS_jetIter    = m_PS_j_container->begin();
  xAOD::JetContainer::const_iterator FS_jetIter    = m_FS_j_container->begin();
  xAOD::JetContainer::const_iterator PS_jetIterEnd = m_PS_j_container->end();
  xAOD::JetContainer::const_iterator FS_jetIterEnd = m_FS_j_container->end();
  xAOD::JetContainer::const_iterator tmpJetIter;


  /** -----------------------|
  |---  Containers maker  ---|
  |------------------------**/  
  //!leading jet container maker
  xAOD::JetContainer* PS_leadJets = new xAOD::JetContainer(SG::VIEW_ELEMENTS);
  xAOD::JetContainer* FS_leadJets = new xAOD::JetContainer(SG::VIEW_ELEMENTS);
  findLeadingJet(m_PS_j_container, PS_leadJets);
  findLeadingJet(m_FS_j_container, FS_leadJets);

  //!focal jet container maker
  xAOD::JetContainer* PS_focalJets = new xAOD::JetContainer(SG::VIEW_ELEMENTS);
  xAOD::JetContainer* FS_focalJets = new xAOD::JetContainer(SG::VIEW_ELEMENTS);
  findFocalJets(m_PS_j_container, PS_focalJets);
  findFocalJets(m_FS_j_container, FS_focalJets);

  //!passing hypo jet container maker
  m_PS_jetsPassingCont = new xAOD::JetContainer(SG::VIEW_ELEMENTS);
  m_FS_jetsPassingCont = new xAOD::JetContainer(SG::VIEW_ELEMENTS);
  findPassJets(m_PS_j_container, m_PS_jetsPassingCont);
  findPassJets(m_FS_j_container, m_FS_jetsPassingCont);
  xAOD::JetContainer::const_iterator PS_jetPassIter    = m_PS_jetsPassingCont->begin();
  xAOD::JetContainer::const_iterator FS_jetPassIter    = m_FS_jetsPassingCont->begin();
  xAOD::JetContainer::const_iterator PS_jetPassIterEnd = m_PS_jetsPassingCont->end();
  xAOD::JetContainer::const_iterator FS_jetPassIterEnd = m_FS_jetsPassingCont->end();

  /** -----------------------|
  |---  superRoI plots    ---|
  |------------------------**/
  //!number of RoIS founded
  m_hMap1D["nTEs"]->Fill(m_superRoi->size());

  for(unsigned int i=0; i<m_superRoi->size(); i++){
    m_hMap1D["SuperRoI_Total_Eta"]->Fill(m_superRoi->at(i)->eta());
    m_hMap1D["SuperRoI_Total_Phi"]->Fill(m_superRoi->at(i)->phi());
    m_hMap2D["PS_2D_SuperRoI"]->Fill(m_superRoi->at(i)->eta(),m_superRoi->at(i)->phi());
  }

  /** -----------------------|
  |---  leading jet       ---|
  |------------------------**/
  auto PS_leadingJet = PS_leadJets->at(0);
  auto FS_leadingJet = FS_leadJets->at(0);

  //! Check if the leading jets match
  std::string jetMatchTag = compareJets(PS_leadingJet, FS_leadingJet);
  m_hMap1D["match_leadJet"]->Fill(jetMatchTag.c_str(),1.);

  //! leading jets efficiency plot
  if((FS_leadingJet->p4()).Et()>=m_hypoEt){
    if((PS_leadingJet->p4()).Et()>=m_hypoEt){     //FS and PS pass      | value=1
      m_hMap2D["efficiency_leadJet"]->Fill((FS_leadingJet->p4()).Et(),1.);
    }else{                          //FS pass and PS not  | value=0
      m_hMap2D["efficiency_leadJet"]->Fill((FS_leadingJet->p4()).Et(),0.);
      ATH_MSG_DEBUG("TEST::Failling hypo events " << m_event);
    }
  }else if((PS_leadingJet->p4()).Et()>=m_hypoEt){ //PS pass and FS not  | value=2
    m_hMap2D["efficiency_leadJet"]->Fill((FS_leadingJet->p4()).Et(),-1.);
    ATH_MSG_DEBUG("TEST::Failling hypo events " << m_event);
  }

  double deltaEt = ((PS_leadingJet->p4()).Et()-(FS_leadingJet->p4()).Et())/(FS_leadingJet->p4()).Et();
  m_hMapProf["deltaEtvsEta_leadJet"]->Fill(FS_leadingJet->eta(),deltaEt);
  m_hMap1D["deltaR_leadJet"]->Fill(deltaR( (PS_leadingJet->eta()-FS_leadingJet->eta()) , (PS_leadingJet->phi()-FS_leadingJet->phi()) ));

  //! deltaRoI leading jet
  m_hMap1D["PS_deltaRoI_leadJet"]->Fill(minDeltaRoI(PS_leadingJet->eta(), PS_leadingJet->phi()));
  m_hMap1D["FS_deltaRoI_leadJet"]->Fill(minDeltaRoI(FS_leadingJet->eta(), FS_leadingJet->phi()));



  /** ----------------------|
  |---  focal jets plots ---|
  |-----------------------**/
  //if(strContains(m_objType,"focalJets")){
  //! number of jets
  m_hMap1D["PS_n_focalJets"]->Fill(PS_focalJets->size());
  m_hMap1D["FS_n_focalJets"]->Fill(FS_focalJets->size());

  std::string PS_tag = "PS no lead jet";
  std::string FS_tag = "FS no lead jet";
  for(unsigned int i=0; i<m_superRoi->size();++i){
    //focal is leading jet?
    PS_tag = ( PS_tag == "PS lead is focal" || (
        PS_focalJets->at(i)->eta() == PS_leadingJet->eta() &&
        PS_focalJets->at(i)->phi() == PS_leadingJet->phi()))?"PS lead is focal":"PS lead isn't focal";
    FS_tag = ( FS_tag == "FS lead is focal" || (
        FS_focalJets->at(i)->eta() == FS_leadingJet->eta() &&
        FS_focalJets->at(i)->phi() == FS_leadingJet->phi()))?"FS lead is focal":"FS lead isn't focal";

    //end focal is leading jet?
    ATH_MSG_DEBUG("TEST::Focal PS jets - eta - " <<PS_focalJets->at(i)->eta()<<" | phi - "<<PS_focalJets->at(i)->phi());
    ATH_MSG_DEBUG("TEST::Focal FS jets - eta - " <<FS_focalJets->at(i)->eta()<<" | phi - "<<FS_focalJets->at(i)->phi());

    fillHists("PS", "focalJets", PS_focalJets->at(i));
    fillHists("FS", "focalJets", FS_focalJets->at(i));

    double tmp_deltaR = deltaR( (PS_focalJets->at(i)->eta()-FS_focalJets->at(i)->eta()) , (PS_focalJets->at(i)->phi()-FS_focalJets->at(i)->phi()) );
    m_hMap1D["tot_deltaR_focalJets"]->Fill(tmp_deltaR);
    m_hMapProf["tot_deltaRvsCenter_focalJets"]->Fill(minDeltaRoI(FS_focalJets->at(i)->eta(), FS_focalJets->at(i)->phi()),tmp_deltaR);

    double deltaEt = ((PS_focalJets->at(i)->p4()).Et()-(FS_focalJets->at(i)->p4()).Et())/(FS_focalJets->at(i)->p4()).Et();
    m_hMapProf["tot_deltaEtvsEt_focalJets"]->Fill((FS_focalJets->at(i)->p4()).Et(),deltaEt);
    m_hMapProf["deltaEtvsEta_focal"]->Fill(FS_focalJets->at(i)->eta(),deltaEt);
  }
  m_hMap1D["focal_has_leadJet"]->Fill(PS_tag.c_str(),1);
  m_hMap1D["focal_has_leadJet"]->Fill(FS_tag.c_str(),1);
  //}

  ATH_MSG_DEBUG("TEST:: Find error - 1 ");
  /** -----------------------|
  |---  jet passing hypo  ---|
  |------------------------**/ 
  //! number of jets
  m_hMap1D["PS_n_Jets_pass"]->Fill(m_PS_jetsPassingCont->size());
  m_hMap1D["FS_n_Jets_pass"]->Fill(m_FS_jetsPassingCont->size());

  for(; PS_jetPassIter!=PS_jetPassIterEnd ; ++PS_jetPassIter){
    auto PS_Jet = *PS_jetPassIter;

    //!Plot PS jets observables
    fillHists("PS", "Jets_pass", const_cast<xAOD::Jet_v1*>(PS_Jet));

    /*m_hMap1D["tot_deltaR_Jets_pass"]->Fill(min_delta_R);

    m_hMapProf["tot_deltaEtavsEta_Jets_pass"]->Fill(FSmatchJet->eta(),fix_deltaEta);    
    m_hMapProf["tot_deltaPhivsPhi_Jets_pass"]->Fill(FSmatchJet->phi(),fix_deltaPhi);   
    m_hMapProf["tot_deltaEtvsEt_Jets_pass"]->Fill((FSmatchJet->p4()).Et(),deltaEt);
    m_hMap1D["1D_tot_deltaEtvsEt_Jets_pass"]->Fill(deltaEt);

    //!--- deltaR, deltaE to RoI center -------------------------
    m_hMapProf["tot_deltaRvsCenter_Jets_pass"]->Fill(min_deltaRoI,fix_delta_R);
    m_hMapProf["tot_deltaEtvsCenter_Jets_pass"]->Fill(min_deltaRoI,fabs_Et);*/
  }//end PS loop

  for(; FS_jetPassIter!=FS_jetPassIterEnd ; ++FS_jetPassIter){
    auto FS_Jet = *FS_jetPassIter;
    //!Plot FS jets observables
    fillHists("FS", "Jets_pass", const_cast<xAOD::Jet_v1*>(FS_Jet));
  }//end FS loop

  ATH_MSG_DEBUG("TEST:: Find error - 2 ");  
  /** -----------------------|
  |---  all jets          ---|
  |------------------------**/
  //! number of jets
  m_hMap1D["PS_n_Jets"]->Fill(m_PS_j_container->size());
  m_hMap1D["FS_n_Jets"]->Fill(m_FS_j_container->size());

  double tmpDelta_R = 0.;
  int PS=0;
  int FS=0;
  // int PS_JetsAccepted = 0;
  // int FS_JetsAccepted = 0;
  //!-- starting loop over PS jets --
  for(; PS_jetIter!=PS_jetIterEnd; ++PS_jetIter){  	
    auto PS_Jet = *PS_jetIter;
    auto FSmatchJet = *FS_jetIter;

    double min_delta_R = 100000.;
    tmpJetIter=FS_jetIter;
    //!-- starting loop over FS jets --
    for(; tmpJetIter!=FS_jetIterEnd;++tmpJetIter){
      auto FS_Jet = *tmpJetIter;

      //!Compute delta R
      tmpDelta_R = deltaR( (PS_Jet->eta()-FS_Jet->eta()) , (PS_Jet->phi()-FS_Jet->phi()) );

      //!Found the closest FS cluster to the PS one in evaluation
      if(tmpDelta_R<min_delta_R){
        min_delta_R=tmpDelta_R;
        FSmatchJet=FS_Jet;
      }

      //!Plot FS jets observables
      if(FS==0){
        fillHists("FS", "Jets", const_cast<xAOD::Jet_v1*>(FS_Jet));
      }

    }//end FS loop

    //!Plot PS jets observables
    fillHists("PS", "Jets", const_cast<xAOD::Jet_v1*>(PS_Jet));

    //! PS to FS deltaR profile plot
    m_hMap1D["tot_deltaR_Jets"]->Fill(min_delta_R);

    //!Fix for TProfile show 0.000 entries
    double deltaEta     = (PS_Jet->eta()-FSmatchJet->eta())/FSmatchJet->eta();
    double deltaPhi     = (PS_Jet->phi()-FSmatchJet->phi())/FSmatchJet->phi();
    double deltaEt      = ((PS_Jet->p4()).Et()-(FSmatchJet->p4()).Et())/(FSmatchJet->p4()).Et();
    double fix_deltaEta = deltaEta!=0?deltaEta:0.0000000001;
    double fix_deltaPhi = deltaPhi!=0?deltaPhi:0.0000000001;    
    //double fix_deltaE   = deltaE!=0?deltaE:0.0000000001;

    m_hMapProf["tot_deltaEtavsEta_Jets"]->Fill(FSmatchJet->eta(),fix_deltaEta);    
    m_hMapProf["tot_deltaPhivsPhi_Jets"]->Fill(FSmatchJet->phi(),fix_deltaPhi);   
    m_hMapProf["tot_deltaEtvsEt_Jets"]->Fill((FSmatchJet->p4()).Et(),deltaEt);
    m_hMap1D["1D_tot_deltaEtvsEt_Jets"]->Fill(deltaEt);

    //!--- deltaR, deltaE to RoI center -------------------------
    double min_deltaRoI = minDeltaRoI(PS_Jet->eta(), PS_Jet->phi());
    double fabs_Et = fabs(((PS_Jet->p4()).Et()-(FSmatchJet->p4()).Et())/(FSmatchJet->p4()).Et());
    m_hMapProf["tot_deltaRvsCenter_Jets"]->Fill(min_deltaRoI,min_delta_R);
    m_hMapProf["tot_deltaEtvsCenter_Jets"]->Fill(min_deltaRoI,fabs_Et);

    //----------------------------------------------------------

    //! all jets passing hypo plot
    if((FSmatchJet->p4()).Et()>=m_hypoEt){
      if((PS_Jet->p4()).Et()>=m_hypoEt){     //FS and PS pass      | value=1
        m_hMap2D["efficiency_allJets"]->Fill((FSmatchJet->p4()).Et(),1.);
      }else{                          //FS pass and PS not  | value=0
        m_hMap2D["efficiency_allJets"]->Fill((FSmatchJet->p4()).Et(),0.);
      }
    }else if((PS_Jet->p4()).Et()>=m_hypoEt){ //PS pass and FS not  | value=2
      m_hMap2D["efficiency_allJets"]->Fill((FSmatchJet->p4()).Et(),2.);
    }


    PS++;
    FS++;
  }//!-- finish loop over PS jets --


  //! number of events accepted
  std::string PS_jetTag = m_PS_jetsPassingCont->size() > 0?"accepted":"rejected";
  std::string FS_jetTag = m_FS_jetsPassingCont->size() > 0?"accepted":"rejected";
  m_hMap1D["PS_nEvt_accepted"]->Fill(PS_jetTag.c_str(),1);
  m_hMap1D["FS_nEvt_accepted"]->Fill(FS_jetTag.c_str(),1);
  //std::printf("TEST::CLUST - PSvsFS - evt=%d - PScluster->size()=%d, PS=%d  FScluster->size()=%d FS=%d \n",m_event,PSclusterCont->size(),PS,FSclusterCont->size(),FS);  
  ATH_MSG_DEBUG("TEST:: Find error - 3 ");

  delete PS_leadJets;
}

/**---------------------------------------------------------
/      Finds the leading jet (highest Et)
---------------------------------------------------------**/
void TrigHLTPSvsFSDiagnostics::findLeadingJet(const xAOD::JetContainer* jetContainer, xAOD::JetContainer* leadJets){
  xAOD::JetContainer::const_iterator jetIterBeg = jetContainer->begin();
  xAOD::JetContainer::const_iterator jetIterEnd = jetContainer->end();
  auto leadJet = *jetIterBeg;

  for(; jetIterBeg!=jetIterEnd; ++jetIterBeg){
    auto tmpjet = *jetIterBeg;

    if((leadJet->p4()).Et()<(tmpjet->p4()).Et()){
      leadJet = tmpjet;
    }
  }

  leadJets->push_back(const_cast<xAOD::Jet_v1*>(leadJet));
}

/**---------------------------------------------------------
/      Finds the focal jets (most RoI cental jet)
---------------------------------------------------------**/
void TrigHLTPSvsFSDiagnostics::findFocalJets(const xAOD::JetContainer* jetContainer, xAOD::JetContainer* focalJets){
  xAOD::JetContainer::const_iterator jetIterBeg = jetContainer->begin();
  xAOD::JetContainer::const_iterator jetIterEnd = jetContainer->end();
  xAOD::JetContainer::const_iterator tmpIter;
  auto tmpJet = *jetIterBeg;
  double min_deltaR, tmp_deltaR;

  for(unsigned int i=0; i<m_superRoi->size(); i++){
    min_deltaR = 1000.;
    for(tmpIter = jetIterBeg; tmpIter!=jetIterEnd; ++tmpIter){
      auto jet = *tmpIter;
      tmp_deltaR = deltaR( (jet->eta()-m_superRoi->at(i)->eta()) , (jet->phi()-m_superRoi->at(i)->phi()) );


      if(tmp_deltaR<min_deltaR){
        min_deltaR=tmp_deltaR;
        tmpJet = *tmpIter;
      }
    }
    ATH_MSG_DEBUG("TEST::Focal RoIs - eta - " <<m_superRoi->at(i)->eta()<<" | phi - "<<m_superRoi->at(i)->phi());

    focalJets->push_back(const_cast<xAOD::Jet_v1*>(tmpJet));
  }
}

/**---------------------------------------------------------
/      Finds the jets passing hypotesis 
---------------------------------------------------------**/
void TrigHLTPSvsFSDiagnostics::findPassJets(const xAOD::JetContainer* jetContainer, xAOD::JetContainer* passJets){
  xAOD::JetContainer::const_iterator jetIterBeg = jetContainer->begin();
  xAOD::JetContainer::const_iterator jetIterEnd = jetContainer->end();

  for(; jetIterBeg!=jetIterEnd; ++jetIterBeg){
    auto tmpJet = *jetIterBeg;

    if((tmpJet->p4()).Et()>=m_hypoEt){
      passJets->push_back(const_cast<xAOD::Jet_v1*>(tmpJet));
    }
  }
}


/**---------------------------------------------------------
/      Calculates the deltaR
---------------------------------------------------------**/
double TrigHLTPSvsFSDiagnostics::deltaR(double delta_eta, double delta_phi){
  double delta_phiCorr = HLT::wrapPhi(delta_phi);
  return std::sqrt((delta_eta*delta_eta) + (delta_phiCorr*delta_phiCorr));
}

/**---------------------------------------------------------
/      Calculates the deltaRoI
---------------------------------------------------------**/
double TrigHLTPSvsFSDiagnostics::minDeltaRoI(double eta, double phi){
  double min_deltaR = 1000., tmp_deltaR;

  for(unsigned int i=0; i<m_superRoi->size(); i++){
    tmp_deltaR = deltaR( (eta-m_superRoi->at(i)->eta()) , (phi-m_superRoi->at(i)->phi()) );

    if(tmp_deltaR<min_deltaR){
      min_deltaR=tmp_deltaR;
    }
  }
  return min_deltaR;
}

/**---------------------------------------------------------
/      Fill some plots
---------------------------------------------------------**/
void TrigHLTPSvsFSDiagnostics::fillHists(std::string scanType, std::string objType, xAOD::Jet_v1* jet){
  //!Plot jets observables
  m_hMap1D[(scanType+"_E_"+objType).c_str()]->Fill(jet->e());
  m_hMap1D[(scanType+"_Et_"+objType).c_str()]->Fill((jet->p4()).Et());
  m_hMap1D[(scanType+"_Eta_"+objType).c_str()]->Fill(jet->eta());
  m_hMap1D[(scanType+"_Phi_"+objType).c_str()]->Fill(jet->phi());
  m_hMap2D[(scanType+"_EvsEt_"+objType).c_str()]->Fill((jet->p4()).Et(), jet->e());

  //!Plot all jets for the first 15 events
  if(m_event<m_nEvToPlot){
    m_hMap2D[histStr((scanType+"_2D_"+objType+"_")).c_str()]->Fill(jet->eta(),jet->phi(),(jet->p4()).Et());
  }
}

/**---------------------------------------------------------
/      Compare jets
---------------------------------------------------------**/
std::string TrigHLTPSvsFSDiagnostics::compareJets(xAOD::Jet_v1* jet1, xAOD::Jet_v1* jet2){
  if(deltaR( (jet1->eta()-jet2->eta()) , (jet1->phi()-jet2->phi()) ) < 0.01){    
    return "match";
  }

  return "don't match";
}


/**---------------------------------------------------------
/      Find str in strList 
---------------------------------------------------------**/
bool TrigHLTPSvsFSDiagnostics::strContains(std::vector<std::string> strList, std::string strToFind){
  for(unsigned i=0; i<strList.size(); ++i){
    if(strList.at(i)==strToFind){
      return true;
    }
  }

  return false;
}
