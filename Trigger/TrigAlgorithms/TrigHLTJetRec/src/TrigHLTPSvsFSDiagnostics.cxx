/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetRec/TrigHLTPSvsFSDiagnostics.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"
#include <TProfile.h>
#include <cmath>
#include "TrigSteeringEvent/PhiHelper.h"


TrigHLTPSvsFSDiagnostics::TrigHLTPSvsFSDiagnostics(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {
  declareProperty("chainName", m_chainName = "unknown" );
  
  //!-- prepare automated histograms (see header) -- 
	std::string scanType[2]  = {"PS","FS"};
  std::string objType[2]   = {/*"Clust",*/"Jets","Jets_pass"};
  std::string objTitle[2]  = {/*"clusters",*/"jets","Jets passing hypo"};
  
  //!--fill names vectors --
  for(int i=0; i<2 ;++i){   //!-- beguin PS/FS loop
    m_scanType.push_back(scanType[i]);
	}
	for(int i=0; i<2 ;++i){ //!-- beguin Cluster/Jets loop
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
  fOut = TFile::Open(fileName.c_str(), "RECREATE");
  
  // Add histograms to map    
  //!-- PS/FS && Cluster/Jets histograms creation
  /** Loop to create the histograms for the options set before:
   - m_scanType[]  - name of the scans to perform, by default {"PS","FS"}
   - m_objType[] - name of the containers to check, by default {"Clust","Jets","Jets_pass"}
   - m_objTitle[]- name of the containers to appear in the tytle, by default {"clusters","jets","Jets passing hypo"}
  **/
  addHist(hMap1D,"match_leadJet", "Check if the leading jets passing hypo match.;case ; efficiency", 2,   0.0,  2.0);  
  addHist(hMap2D,"efficiency_allJets", "Efficiency for all FS jets passing hypo.;Et (MeV) ; efficiency", 100,   0.0,  1000000.0,20,0.,2.1);  
  addHist(hMap2D,"efficiency_leadJet", "Efficiency for the leading jets passing hypo.;Et (MeV) ; efficiency", 100,   0.0,  1000000.0,20,0.,2.1);  
  			
  //!-- SuperRoI eta phi --
  addHist(hMap1D,"SuperRoI_Total_Eta", "PS RoI eta distribution; #eta; entries", 100, -5., 5.);
  addHist(hMap1D,"SuperRoI_Total_Phi", "PS RoI phi distribution; #phi; entries", 64, -3.1416, 3.1416);
  
  for(unsigned int i=0; i<m_scanType.size() ;++i){   //!-- beguin PS/FS loop
  	//!-- events accepted --
  	//ex:          PS_nEvt_accepted - PS Number of events passing the hypo.;hypo ;nevents
  	addHist(hMap1D,m_scanType.at(i)+"_nEvt_accepted", m_scanType.at(i)+" Number of events passing the hypo.;hypo ;nevents", 2,   0.0,  2.0);

		for(unsigned int j=0; j<m_objType.size() ;++j){ //!-- beguin Cluster/Jets loop
		  //! If limits of histograms need to be changed, by default between Clusters and Jets or Jets passing hypo.
		  //int limits[3][6];
		  
			//!-- containers size --
			//ex:          PS_n_Cluster - PS Number of clusters per event ;number of jets ;nevents
			addHist(hMap1D,m_scanType.at(i)+"_n_"+m_objType.at(j), m_scanType.at(i)+" Number of "+m_objTitle.at(j)+" per event ;number of "+m_objTitle.at(j)+" ;nevents", 100,   0.0,  2000.0);

			//!-- Energy --
			//ex:          PS_E_Cluster - PS clusters energy distribution;Energy (MeV) ; entries
			addHist(hMap1D,m_scanType.at(i)+"_E_"+m_objType.at(j), m_scanType.at(i)+" "+m_objTitle.at(j)+" energy distribution;Energy (MeV) ; entries", 100,   0.0,  1000000.0);  

			//!-- Et --
			//ex:          PS_Et_Cluster - PS clusters Et distribution;Et (MeV) ; entries
			addHist(hMap1D,m_scanType.at(i)+"_Et_"+m_objType.at(j), m_scanType.at(i)+" "+m_objTitle.at(j)+" Et distribution; Et (MeV); entries", 100,   0.0,  1000000.0);

			//!-- eta --
			//ex:          PS_Eta_Clust - PS Clusters eta distribution; #eta; entries
			addHist(hMap1D,m_scanType.at(i)+"_Eta_"+m_objType.at(j), m_scanType.at(i)+" "+m_objTitle.at(j)+" eta distribution; #eta; entries", 100, -5., 5.);
  
			//!-- phi --
			//ex:          PS_Phi_Clust - PS Clusters phi distribution; #phi; entries
			addHist(hMap1D,m_scanType.at(i)+"_Phi_"+m_objType.at(j), m_scanType.at(i)+" "+m_objTitle.at(j)+" phi distribution; #phi; entries", 64, -3.1416, 3.1416);	

					
			//!-- E vs Et --
			//ex:          PS_EvsEt_Clust - PS Clusters Energy vs Et;Et (MeV/c);Energy (MeV)
			addHist(hMap2D,m_scanType.at(i)+"_EvsEt_"+m_objType.at(j), m_scanType.at(i)+" "+m_objTitle.at(j)+" Energy vs Et;Et (MeV);Energy (MeV)", 100,  0.0,  1000000.0,  100,  0.0,  1000000.0);	
 
  
			/** histograms only with: (comparing PS to FS)
			- m_objType[] - name of the containers to check, by default {"Clust","Jets","Jets_pass"}
			- m_objTitle[]- name of the containers to appear in the tytle, by default {"clusters","jets","Jets passing hypo"}
			**/
			if(i==0){
			  //!-- deltaR --
			  //ex:          tot_deltaR_Clust - Clusters deltaR (PS-FS); #deltaR; entries
			  addHist(hMap1D,"tot_deltaR_"+m_objType.at(j), m_objTitle.at(j)+" deltaR (PS-FS); #deltaR; entries",          100, 0., 0.2);

			  //!-- deltaR vs RoI center --
			  //ex:          tot_deltaRvsCenter_Clust - Clusters normalized deltaR (PS-FS)/FS vs PS distance to RoI center; #delta to RoI center;#deltaR
			  addHist(hMapProf,"tot_deltaRvsCenter_"+m_objType.at(j), m_objTitle.at(j)+" normalized deltaR (PS-FS)/FS vs PS distance to RoI center; #delta to RoI center;#deltaR", 100, 0., 1.5);

			  //!-- deltaE vs RoI center --
			  //ex:          tot_deltaEtvsCenter_Clust - Clusters normalized deltaE (PS-FS)/FS vs distance to RoI center; #delta to RoI center;#deltaE/FS_E
			  addHist(hMapProf,"tot_deltaEtvsCenter_"+m_objType.at(j), m_objTitle.at(j)+" normalized absolute deltaEt (PS-FS)/FS vs distance to RoI center; #delta to RoI center;#deltaEt/FS_Et", 100, 0., 1.5);    
	
			  //!-- deltaEta vs Eta --
			  //ex:          tot_deltaEtavsEta_Clust - Clusters normalized deltaEta (PS-FS)/FS over FS eta; FS_eta; #deltaEta/FS_Eta
			  addHist(hMapProf,"tot_deltaEtavsEta_"+m_objType.at(j), m_objTitle.at(j)+" Clusters normalized deltaEta (PS-FS)/FS over FS eta; FS_eta; #deltaEta/FS_Eta", 100, -5., 5.);
		
			  //!-- deltaPhi vs Phi --
			  //ex:          tot_deltaPhivsPhi_Clust - Clusters normalized deltaPhi (PS-FS)/FS over FS phi; FS_phi; #deltaPhi/FS_Phi
			  addHist(hMapProf,"tot_deltaPhivsPhi_"+m_objType.at(j), m_objTitle.at(j)+" normalized deltaPhi (PS-FS)/FS over FS phi; FS_phi; #deltaPhi/FS_Phi", 64, -3.1416, 3.1416);
			 
			  //!-- deltaE vs E --
			  //ex:          tot_deltaEtvsEt_Clust - Clusters normalized deltaE (PS-FS)/FS over FS E; FS_Et; #deltaE/FS_E
			  addHist(hMap1D,"1D_tot_deltaEtvsEt_"+m_objType.at(j), m_objTitle.at(j)+" normalized deltaEt (PS-FS)/FS over FS Et; #deltaEt/FS_Et; entries", 100, 0., 5.);  
			  addHist(hMapProf,"tot_deltaEtvsEt_"+m_objType.at(j), m_objTitle.at(j)+" normalized deltaEt (PS-FS)/FS over FS Et; FS_Et; #deltaEt/FS_Et", 100, 0., 1000000.);  
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
			  addHist(hMap2D,histStr(m_scanType.at(i)+"_2D_"+m_objType.at(j)+"_",k+1,""),histStr(m_scanType.at(i)+" "+m_objTitle.at(j)+" created evt-",k+1,"; #eta;#phi;Et (MeV)").c_str(),    100, -5., 5., 64, -3.1416, 3.1416);
			}
		}  //!-- end Cluster/Jets loop
  }    //!-- end PS/FS loop
  
  
  /* old temporary histograms
  addHist(hMap1D,histStr("_deltaR_").c_str(), histStr("Clusters deltaR PS to FS evt-","; #deltaR; entries").c_str(), 100, 0., 0.2);
  addHist(hMap2D,histStr("_MatchClusers_").c_str(), histStr("Match Clusters evt-","; #eta;#phi;Energy (MeV)").c_str(),   200, -5., 5., 128, -3.1416, 3.1416);  
  addHist(hMapProf,histStr("_deltaRtoCenter_").c_str(), histStr("Clusters deltaR PS to FS vs distance to RoI center evt-","; #delta to RoI center;#deltaR").c_str(), 100, 0., 1.5);
  addHist(hMapProf,histStr("_deltaEtoCenter_").c_str(), histStr("Clusters deltaE PS to FS vs distance to RoI center evt-","; #delta to RoI center;#deltaE (MeV)").c_str(), 100, 0., 1.5);
  
  */
  return HLT::OK; 
}


HLT::ErrorCode TrigHLTPSvsFSDiagnostics::hltFinalize(){
  ATH_MSG_INFO ("Finalizing " << name() << "...");  
  // Save histograms and close file 
  fOut->Write();
  fOut->Close();
  // Clear histogram maps
  hMap2D.clear();
  hMap1D.clear();
  hMapProf.clear();
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
  // const TrigSuperRoi* superRoi;
  
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

  
  if (!evtStore()->contains<xAOD::JetContainer>(PSjetCollKey)) {
    ATH_MSG_DEBUG("TEST::Container - Didn't found any xAOD::JetContainer with Key"<<PSjetCollKey);
    return false;
  }
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
    if(getFeature(inputTE, m_superRoi, "") == HLT::OK) {
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
  hMap1D["PS_n_Clust"]->Fill(m_PSclusterCont->size());
  hMap1D["FS_n_Clust"]->Fill(m_FSclusterCont->size());
  
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
      	hMap1D["FS_E_Clust"]->Fill(FS_Cluster->e());
      	hMap1D["FS_Et_Clust"]->Fill((FS_Cluster->p4()).Et());
      	hMap1D["FS_Eta_Clust"]->Fill(FS_Cluster->eta());
      	hMap1D["FS_Phi_Clust"]->Fill(FS_Cluster->phi());
      	hMap2D["FS_EvsEt_Clust"]->Fill((FS_Cluster->p4()).Et(), FS_Cluster->e());

      	//!Plot all FS clusters for the first 15 events
      	if(m_event<15){
      		hMap2D[histStr("FS_2D_Clust_").c_str()]->Fill(FS_Cluster->eta(),FS_Cluster->phi(),(FS_Cluster->p4()).Et());
      	}
      }
    }//!-- finish loop over FS clusters --
    
    //!Plot PS clusters observables
    hMap1D["PS_E_Clust"]->Fill(PS_Cluster->e());
    hMap1D["PS_Et_Clust"]->Fill((PS_Cluster->p4()).Et());
    hMap1D["PS_Eta_Clust"]->Fill(PS_Cluster->eta());
    hMap1D["PS_Phi_Clust"]->Fill(PS_Cluster->phi());
    hMap2D["PS_EvsEt_Clust"]->Fill((PS_Cluster->p4()).Et(), PS_Cluster->e());
    
    //!Plot all PS clusters for the first 15 events
    if(m_event<15){
    	//!Print closests clusters 
      /*std::printf("TEST::CLUST - min_delta_R=%f \n",min_delta_R);
      if(min_delta_R<0.005){
        hMap2D[histStr("_MatchClusers_").c_str()]->Fill(PSCluster->eta(),PSCluster->phi(),PSCluster->e()); 
      }else{
      	std::printf("TEST::CLUST - PSCluster - evt=%d - pt=%f, eta=%f  phi=%f m=%f e=%f rapidity=%f \n",m_event,PSCluster->pt(),PSCluster->eta(),PSCluster->phi(),PSCluster->m(),PSCluster->e(),PSCluster->rapidity());
        std::printf("TEST::CLUST - FSCluster - evt=%d - pt=%f, eta=%f  phi=%f m=%f e=%f rapidity=%f \n",m_event,FSmatchCluster->pt(),FSmatchCluster->eta(),FSmatchCluster->phi(),FSmatchCluster->m(),FSmatchCluster->e(),FSmatchCluster->rapidity());
        std::printf("TEST::CLUST ----------------\n");
      }*/
      //!Delta R per event
      hMap2D[histStr("PS_2D_Clust_").c_str()]->Fill(PS_Cluster->eta(),PS_Cluster->phi(),(PS_Cluster->p4()).Et());
    }
    
    //! PS to FS deltaR profile plot
    hMap1D["tot_deltaR_Clust"]->Fill(min_delta_R);
    
    //!Fix for TProfile show 0.000 entries
    double deltaEta     = (PS_Cluster->eta()-FSmatchCluster->eta())/FSmatchCluster->eta();
    double deltaPhi     = (PS_Cluster->phi()-FSmatchCluster->phi())/FSmatchCluster->phi();
    double deltaEt      = ((PS_Cluster->p4()).Et()-(FSmatchCluster->p4()).Et())/(FSmatchCluster->p4()).Et();
    double fix_deltaEta = deltaEta!=0?deltaEta:0.0000000001;
    double fix_deltaPhi = deltaPhi!=0?deltaPhi:0.0000000001;    
    //double fix_deltaE   = deltaE!=0?deltaE:0.0000000001;
    
    hMapProf["tot_deltaEtavsEta_Clust"]->Fill(FSmatchCluster->eta(),fix_deltaEta);    
    hMapProf["tot_deltaPhivsPhi_Clust"]->Fill(FSmatchCluster->phi(),fix_deltaPhi);   
    hMapProf["tot_deltaEtvsEt_Clust"]->Fill((FSmatchCluster->p4()).Et(),deltaEt);
    hMap1D["1D_tot_deltaEtvsEt_Clust"]->Fill(deltaEt);
    
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
      
      hMapProf["tot_deltaRvsCenter_Clust"]->Fill(min_deltaRoI,fix_delta_R);
      hMapProf["tot_deltaEtvsCenter_Clust"]->Fill(min_deltaRoI,fabs_Et);
    }
    //----------------------------------------------------------
    
    
    PS++;
    FS++;
  }//!-- finish loop over PS clusters --
  
  //std::printf("TEST::CLUST - PSvsFS - evt=%d - PScluster->size()=%d, PS=%d  FScluster->size()=%d FS=%d \n",m_event,PSclusterCont->size(),PS,FSclusterCont->size(),FS);
}


void TrigHLTPSvsFSDiagnostics::jetsCheck(){
  xAOD::JetContainer::const_iterator PS_jetIter    = m_PS_j_container->begin();
  xAOD::JetContainer::const_iterator FS_jetIter    = m_FS_j_container->begin();
  xAOD::JetContainer::const_iterator PS_jetIterEnd = m_PS_j_container->end();
  xAOD::JetContainer::const_iterator FS_jetIterEnd = m_FS_j_container->end();
  xAOD::JetContainer::const_iterator tmpJetIter;
  
  m_PS_jetsPassingCont = new xAOD::JetContainer(SG::VIEW_ELEMENTS);
  m_FS_jetsPassingCont = new xAOD::JetContainer(SG::VIEW_ELEMENTS);
    
  //! number of jets
  hMap1D["PS_n_Jets"]->Fill(m_PS_j_container->size());
  hMap1D["FS_n_Jets"]->Fill(m_FS_j_container->size());
  
  double delta_R = 0.;
  int PS=0;
  int FS=0;
  int PS_JetsAccepted = 0;
  int FS_JetsAccepted = 0;
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
      double delta_eta = PS_Jet->eta() - FS_Jet->eta();
      double delta_phi = PS_Jet->phi() - FS_Jet->phi();
      double delta_phiCorr = HLT::wrapPhi(delta_phi);
      delta_R = std::sqrt((delta_eta*delta_eta) + (delta_phiCorr*delta_phiCorr));
      
      //!Found the closest FS cluster to the PS one in evaluation
      if(delta_R<min_delta_R){
        min_delta_R=delta_R;
        FSmatchJet=FS_Jet;
      }
      
      //!Plot FS jets observables
      if(FS==0){
      	hMap1D["FS_E_Jets"]->Fill(FS_Jet->e());
      	hMap1D["FS_Et_Jets"]->Fill((FS_Jet->p4()).Et());
      	hMap1D["FS_Eta_Jets"]->Fill(FS_Jet->eta());
      	hMap1D["FS_Phi_Jets"]->Fill(FS_Jet->phi());
      	hMap2D["FS_EvsEt_Jets"]->Fill((FS_Jet->p4()).Et(), FS_Jet->e());
      	
      	//!Plot all FS jets for the first 15 events
      	if(m_event<15){
      		hMap2D[histStr("FS_2D_Jets_").c_str()]->Fill(FS_Jet->eta(),FS_Jet->phi(),(FS_Jet->p4()).Et());
      	}
      }
      
      /** -----------------------|
      |---  Jets passing hypo ---|
      |------------------------**/
      if((FS_Jet->p4()).Et()>=m_hypoEt){
      	FS_JetsAccepted++;
      	//!Plot FS jets observables
      	if(FS==0){
      		hMap1D["FS_E_Jets_pass"]->Fill(FS_Jet->e());
      		hMap1D["FS_Et_Jets_pass"]->Fill((FS_Jet->p4()).Et());
      		hMap1D["FS_Eta_Jets_pass"]->Fill(FS_Jet->eta());
      		hMap1D["FS_Phi_Jets_pass"]->Fill(FS_Jet->phi());
      		hMap2D["FS_EvsEt_Jets_pass"]->Fill((FS_Jet->p4()).Et(), FS_Jet->e());
      		
      		//!Plot all FS jets for the first 15 events
      		if(m_event<15){
      			hMap2D[histStr("FS_2D_Jets_pass_").c_str()]->Fill(FS_Jet->eta(),FS_Jet->phi(),(FS_Jet->p4()).Et());
      		}
      	}
      }
      
    }//!-- finish loop over FS jets --
    
    //!Plot PS jets observables
    hMap1D["PS_E_Jets"]->Fill(PS_Jet->e());
    hMap1D["PS_Et_Jets"]->Fill((PS_Jet->p4()).Et());
    hMap1D["PS_Eta_Jets"]->Fill(PS_Jet->eta());
    hMap1D["PS_Phi_Jets"]->Fill(PS_Jet->phi());
    hMap2D["PS_EvsEt_Jets"]->Fill((PS_Jet->p4()).Et(), PS_Jet->e());
    
    //!Plot all PS jets for the first 15 events
    if(m_event<15){
      //!Delta R per event
      hMap2D[histStr("PS_2D_Jets_").c_str()]->Fill(PS_Jet->eta(),PS_Jet->phi(),(PS_Jet->p4()).Et());
    }
    
    //! PS to FS deltaR profile plot
    hMap1D["tot_deltaR_Jets"]->Fill(min_delta_R);

    //!Fix for TProfile show 0.000 entries
    double deltaEta     = (PS_Jet->eta()-FSmatchJet->eta())/FSmatchJet->eta();
    double deltaPhi     = (PS_Jet->phi()-FSmatchJet->phi())/FSmatchJet->phi();
    double deltaEt      = ((PS_Jet->p4()).Et()-(FSmatchJet->p4()).Et())/(FSmatchJet->p4()).Et();
    double fix_deltaEta = deltaEta!=0?deltaEta:0.0000000001;
    double fix_deltaPhi = deltaPhi!=0?deltaPhi:0.0000000001;    
    //double fix_deltaE   = deltaE!=0?deltaE:0.0000000001;
    
    hMapProf["tot_deltaEtavsEta_Jets"]->Fill(FSmatchJet->eta(),fix_deltaEta);    
    hMapProf["tot_deltaPhivsPhi_Jets"]->Fill(FSmatchJet->phi(),fix_deltaPhi);   
    hMapProf["tot_deltaEtvsEt_Jets"]->Fill((FSmatchJet->p4()).Et(),deltaEt);
    hMap1D["1D_tot_deltaEtvsEt_Jets"]->Fill(deltaEt);
    
    //!--- deltaR, deltaE to RoI center -------------------------
    //! loop over superRoI RoI's
    double min_deltaRoI = 0;
    double fix_delta_R  = 0;
    double fabs_Et = 0;
    if(m_haveSuperRoi && m_superRoi->size()>0 ){
      min_deltaRoI = 100000.;
      double tmp_deltaRoI;
      for(unsigned int i=0; i<m_superRoi->size(); i++){
        double delta_eta = PS_Jet->eta() - m_superRoi->at(i)->eta();
        double delta_phi = PS_Jet->phi() - m_superRoi->at(i)->phi();
        double delta_phiCorr = HLT::wrapPhi(delta_phi);
        tmp_deltaRoI = std::sqrt((delta_eta*delta_eta) + (delta_phiCorr*delta_phiCorr));
        
        if(tmp_deltaRoI<min_deltaRoI){
          min_deltaRoI=tmp_deltaRoI;
        }
        
        // if(FS==0 && m_event<15){
        // m_superRoiPos->Fill(m_superRoiEta.at(i),m_superRoiPhi.at(i));
        // }
        //! Print only one time
        if(PS_jetIter==m_PS_j_container->begin()){
        	hMap1D["SuperRoI_Total_Eta"]->Fill(m_superRoi->at(i)->eta());
        	hMap1D["SuperRoI_Total_Phi"]->Fill(m_superRoi->at(i)->phi());
        }
      }
      
      //!Fix for TProfile show 0.000 entries
      fix_delta_R = min_delta_R!=0?min_delta_R:0.0000000001;
      fabs_Et = fabs(((PS_Jet->p4()).Et()-(FSmatchJet->p4()).Et())/(FSmatchJet->p4()).Et());
      if(fabs_Et==0.0){
        fabs_Et= 0.0000000001;
      }

      hMapProf["tot_deltaRvsCenter_Jets"]->Fill(min_deltaRoI,fix_delta_R);
      hMapProf["tot_deltaEtvsCenter_Jets"]->Fill(min_deltaRoI,fabs_Et);
    }
    //----------------------------------------------------------
    
    /** -----------------------|
    |---  Jets passing hypo ---|
    |------------------------**/
    if((PS_Jet->p4()).Et()>=m_hypoEt){
    	PS_JetsAccepted++;
    	
    	//!Plot PS jets observables
    	hMap1D["PS_E_Jets_pass"]->Fill(PS_Jet->e());
    	hMap1D["PS_Et_Jets_pass"]->Fill((PS_Jet->p4()).Et());
    	hMap1D["PS_Eta_Jets_pass"]->Fill(PS_Jet->eta());
    	hMap1D["PS_Phi_Jets_pass"]->Fill(PS_Jet->phi());
    	hMap2D["PS_EvsEt_Jets_pass"]->Fill((PS_Jet->p4()).Et(), PS_Jet->e());
    	
    	//!Plot all PS jets for the first 15 events
    	if(m_event<15){
    		hMap2D[histStr("PS_2D_Jets_pass_").c_str()]->Fill(PS_Jet->eta(),PS_Jet->phi(),(PS_Jet->p4()).Et());
    	}
    	
    	//! PS to FS deltaR profile plot
    	hMap1D["tot_deltaR_Jets_pass"]->Fill(min_delta_R);
    	
    	hMapProf["tot_deltaEtavsEta_Jets_pass"]->Fill(FSmatchJet->eta(),fix_deltaEta);    
    	hMapProf["tot_deltaPhivsPhi_Jets_pass"]->Fill(FSmatchJet->phi(),fix_deltaPhi);   
    	hMapProf["tot_deltaEtvsEt_Jets_pass"]->Fill((FSmatchJet->p4()).Et(),deltaEt);
    	hMap1D["1D_tot_deltaEtvsEt_Jets_pass"]->Fill(deltaEt);

    	//!--- deltaR, deltaE to RoI center -------------------------
    	hMapProf["tot_deltaRvsCenter_Jets_pass"]->Fill(min_deltaRoI,fix_delta_R);
      hMapProf["tot_deltaEtvsCenter_Jets_pass"]->Fill(min_deltaRoI,fabs_Et);
    }
      
    /** ----------------------|
    |---  efficiency plots ---|
    |-----------------------**/
    //! Conteiner to be used in the leading jets plot  
    if((FSmatchJet->p4()).Et()>=m_hypoEt || (PS_Jet->p4()).Et()>=m_hypoEt){
    	m_PS_jetsPassingCont->push_back(const_cast<xAOD::Jet_v1*>(PS_Jet));
    	m_FS_jetsPassingCont->push_back(const_cast<xAOD::Jet_v1*>(FSmatchJet));
    }
    //! all jets passing hypo plot
    if((FSmatchJet->p4()).Et()>=m_hypoEt){
    	if((PS_Jet->p4()).Et()>=m_hypoEt){     //FS and PS pass      | value=1
    		hMap2D["efficiency_allJets"]->Fill((FSmatchJet->p4()).Et(),1.);
    	}else{                          //FS pass and PS not  | value=0
    		hMap2D["efficiency_allJets"]->Fill((FSmatchJet->p4()).Et(),0.);
    	}
    }else if((PS_Jet->p4()).Et()>=m_hypoEt){ //PS pass and FS not  | value=2
    	hMap2D["efficiency_allJets"]->Fill((FSmatchJet->p4()).Et(),2.);
    }
  
          
    PS++;
    FS++;
  }//!-- finish loop over PS jets --
  
  //! find leading passing jet
  xAOD::Jet_v1* PS_leadingJet = new xAOD::Jet_v1();
  xAOD::Jet_v1* FS_leadingJet = new xAOD::Jet_v1();
  int PS_leadingJetIndex = 0;
  int FS_leadingJetIndex = 0;
  for(unsigned int i=0; i<m_PS_jetsPassingCont->size();++i){
  	if(i==0){
  		PS_leadingJet = m_PS_jetsPassingCont->at(i);
  		PS_leadingJetIndex=i;
  	}

  	if((PS_leadingJet->p4()).Et()<(m_PS_jetsPassingCont->at(i)->p4()).Et()){
  		PS_leadingJet = m_PS_jetsPassingCont->at(i);
  		PS_leadingJetIndex=i;
  	}
  }
  for(unsigned int i=0; i<m_FS_jetsPassingCont->size();++i){
  	if(i==0){
  		FS_leadingJet = m_FS_jetsPassingCont->at(i);
  		FS_leadingJetIndex=i;
  	}
  	
  	if((FS_leadingJet->p4()).Et()<(m_FS_jetsPassingCont->at(i)->p4()).Et()){
  		FS_leadingJet = m_FS_jetsPassingCont->at(i);
  		FS_leadingJetIndex=i;
  	}
  }
  
  //! Check if the leading jets match
  std::string jetMatchTag = PS_leadingJetIndex == FS_leadingJetIndex?"match":"don't match";
  hMap1D["match_leadJet"]->Fill(jetMatchTag.c_str(),1.);

  
  //! leading jets passing hypo plot
  if(m_FS_jetsPassingCont->size() > 0){
    if((FS_leadingJet->p4()).Et()>=m_hypoEt){
      if((PS_leadingJet->p4()).Et()>=m_hypoEt){     //FS and PS pass      | value=1
        hMap2D["efficiency_leadJet"]->Fill((FS_leadingJet->p4()).Et(),1.);
      }else{                          //FS pass and PS not  | value=0
        hMap2D["efficiency_leadJet"]->Fill((FS_leadingJet->p4()).Et(),0.);
        ATH_MSG_DEBUG("TEST::Failling hypo events " << m_event);
      }
    }else if((PS_leadingJet->p4()).Et()>=m_hypoEt){ //PS pass and FS not  | value=2
      hMap2D["efficiency_leadJet"]->Fill((FS_leadingJet->p4()).Et(),2.);
      ATH_MSG_DEBUG("TEST::Failling hypo events " << m_event);
    }
  }
  
  //! number of jets passing hypo
  hMap1D["PS_n_Jets_pass"]->Fill(PS_JetsAccepted);
  hMap1D["FS_n_Jets_pass"]->Fill(FS_JetsAccepted);
  
  //! number of events accepted
  std::string PS_jetTag = PS_JetsAccepted > 0?"accepted":"rejected";
  std::string FS_jetTag = FS_JetsAccepted > 0?"accepted":"rejected";
  hMap1D["PS_nEvt_accepted"]->Fill(PS_jetTag.c_str(),1);
  hMap1D["FS_nEvt_accepted"]->Fill(FS_jetTag.c_str(),1);

  //std::printf("TEST::CLUST - PSvsFS - evt=%d - PScluster->size()=%d, PS=%d  FScluster->size()=%d FS=%d \n",m_event,PSclusterCont->size(),PS,FSclusterCont->size(),FS);  
}

