/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//////////////////////////////////////////////////////////////////////////////////////
/// Name    : TauExamples.cxx
/// Package : offline/PhysicsAnalysis/TauID/TauTools
/// Author  : Jyothsna Komaragiri and Dugan O'Neil
/// Created : May 2008
/// Last Modified : Oct 2008
///
/// DESCRIPTION:
///
/// This class is a simple example for taus and inherits from AraToolBase from "AraTool", Dual Use Tools.
/// Uses the methods from TruthHandler.h file to get visible pT, hadronic truth taus etc.
/// and uses the methods from RecoHandler.h file to find and select taRec reco candidates..
/// It has methods to find tau reconstruction and selection efficiencies as a function of tau visible pT, 
/// eta and phi for 1 and 3 prongs.
/// This class was written to be run on events reconstructed in 14.x.x
///
//////////////////////////////////////////////////////////////////////////////////////

#include "TauTools/TauExamples.h"

using CLHEP::GeV;
using namespace std;
using std::cout;
using std::endl;
using std::setw;

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor

TauExamples::TauExamples() {

  /** name of the AOD truth particle container to retrieve from StoreGate or Transient tree */
  m_truthParticleContainerName = "SpclMC";
  
  /** name of the AOD taujet container to retrieve from StoreGate or Transient tree */
  m_taujetContainerName = "TauRecContainer";

  /** Cone matching cut */
  m_deltaRMatchCut = 0.2;

  /** Initialise simple counters */
  m_oneProng = 0;
  m_threeProng = 0;

}

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

TauExamples::~TauExamples() {}

/////////////////////////////////////////////////////////////////////////////////////
void TauExamples::CreateHistos(){

  /** Profiles for 1prong and 3prong reco & reco+ID efficiencies*/
  m_eff_reco_1P_TauRec     = new TProfile("m_eff_reco_1P_TauRec"," Reco Eff 1P vs P_{T}",20, 0.0, 100.0, 0.0, 100.0);
  m_eff_reco_3P_TauRec     = new TProfile("m_eff_reco_3P_TauRec"," Reco Eff 3P vs P_{T}",20, 0.0, 100.0, 0.0, 100.0);
  m_eff_reco_13P_TauRec    = new TProfile("m_eff_reco_13P_TauRec","Reco Eff All prong vs P_{T}",20, 0.0, 100.0, 0.0, 100.0);

  m_eff_reco_1Peta_TauRec  = new TProfile("m_eff_reco_1Peta_TauRec","Reco Eff 1P vs #eta",20, -3., 3., 0.0, 100.0);
  m_eff_reco_3Peta_TauRec  = new TProfile("m_eff_reco_3Peta_TauRec","Reco Eff 3P vs #eta",20, -3., 3., 0.0, 100.0);
  m_eff_reco_13Peta_TauRec = new TProfile("m_eff_reco_13Peta_TauRec","Reco Eff All prong vs #eta",20, -3., 3., 0.0, 100.0);

  m_eff_reco_1Pphi_TauRec  = new TProfile("m_eff_reco_1Pphi_TauRec","Reco Eff 1P vs #phi",20, -3.5, 3.5, 0.0, 100.0);
  m_eff_reco_3Pphi_TauRec  = new TProfile("m_eff_reco_3Pphi_TauRec","Reco Eff 3P vs #phi",20, -3.5, 3.5, 0.0, 100.0);
  m_eff_reco_13Pphi_TauRec = new TProfile("m_eff_reco_13Pphi_TauRec","Reco Eff All prong vs #phi",20, -3.5, 3.5, 0.0, 100.0);

  m_eff_id_1P_TauRec       = new TProfile("m_eff_id_1P_TauRec","Reco+ID Eff 1P vs P_{T}",20, 0.0, 100.0, 0.0, 100.0);
  m_eff_id_3P_TauRec       = new TProfile("m_eff_id_3P_TauRec","Reco+ID Eff 3P vs P_{T}",20, 0.0, 100.0, 0.0, 100.0);
  m_eff_id_13P_TauRec      = new TProfile("m_eff_id_13P_TauRec","Reco+ID Eff All prong vs P_{T}",20, 0.0, 100.0, 0.0, 100.0);

  m_eff_id_1Peta_TauRec    = new TProfile("m_eff_id_1Peta_TauRec","Reco+ID Eff 1P vs #eta",20, -3., 3., 0.0, 100.0);
  m_eff_id_3Peta_TauRec    = new TProfile("m_eff_id_3Peta_TauRec","Reco+ID Eff 3P vs #eta",20, -3., 3., 0.0, 100.0);
  m_eff_id_13Peta_TauRec   = new TProfile("m_eff_id_13Peta_TauRec","Reco+ID Eff All prong vs #eta",20, -3., 3., 0.0, 100.0);

  m_eff_id_1Pphi_TauRec    = new TProfile("m_eff_id_1Pphi_TauRec","Reco+ID Eff 1P vs #phi",20, -3.5, 3.5, 0.0, 100.0);
  m_eff_id_3Pphi_TauRec    = new TProfile("m_eff_id_3Pphi_TauRec","Reco+ID Eff 3P vs #phi",20, -3.5, 3.5, 0.0, 100.0);
  m_eff_id_13Pphi_TauRec   = new TProfile("m_eff_id_13Pphi_TauRec","Reco+ID Eff All prong vs #phi",20, -3.5, 3.5, 0.0, 100.0);

  /** Resolutions */
  m_etRes_reco_TauRec          = new TH1F("m_etRes_reco_TauRec", "Reco E_{T} Res tauRec" , 100, -1.0, 1.0);
  m_etaRes_reco_TauRec         = new TH1F("m_etaRes_reco_TauRec", "Reco deltaEta tauRec" , 100, -0.2, 0.2);
  m_phiRes_reco_TauRec         = new TH1F("m_phiRes_reco_TauRec", "Reco deltaPhi tauRec" , 100, -0.2, 0.2);

  m_etRes_id_TauRec            = new TH1F("m_etRes_id_TauRec", "Reco+ID E_{T} Res tauRec" , 100, -1.0, 1.0);
  m_etaRes_id_TauRec           = new TH1F("m_etaRes_id_TauRec", "Reco+ID deltaEta tauRec" , 100, -0.2, 0.2);
  m_phiRes_id_TauRec           = new TH1F("m_phiRes_id_TauRec", "Reco+ID deltaPhi tauRec" , 100, -0.2, 0.2);

}
/////////////////////////////////////////////////////////////////////////////////////
void TauExamples::StoreHistos(TFile *m_OutputRootFile) { // saves all the histograms in the TClonesArray 

  //store histograms
  m_OutputRootFile->cd();

  m_eff_reco_1P_TauRec->Write();
  m_eff_reco_3P_TauRec->Write();
  m_eff_reco_13P_TauRec->Write();

  m_eff_reco_1Pphi_TauRec->Write();
  m_eff_reco_3Pphi_TauRec->Write();
  m_eff_reco_13Pphi_TauRec->Write();

  m_eff_reco_1Peta_TauRec->Write();
  m_eff_reco_3Peta_TauRec->Write();
  m_eff_reco_13Peta_TauRec->Write();

  m_eff_id_1P_TauRec->Write();
  m_eff_id_3P_TauRec->Write();
  m_eff_id_13P_TauRec->Write();

  m_eff_id_1Pphi_TauRec->Write();
  m_eff_id_3Pphi_TauRec->Write();
  m_eff_id_13Pphi_TauRec->Write();

  m_eff_id_1Peta_TauRec->Write();
  m_eff_id_3Peta_TauRec->Write();
  m_eff_id_13Peta_TauRec->Write();

  m_etRes_reco_TauRec->Write();
  m_etaRes_reco_TauRec->Write();
  m_phiRes_reco_TauRec->Write();

  m_etRes_id_TauRec->Write();
  m_etaRes_id_TauRec->Write();
  m_phiRes_id_TauRec->Write();
  
}

/////////////////////////////////////////////////////////////////////////////////////
/// get the tauRec 1prong and 3prong reco & reco+ID efficiency as a function of tau visible pT
StatusCode TauExamples::getTauRecProngEfficiency(const Analysis::TauJetContainer *tau, const TruthParticleContainer *mcpart){//getTauRecProngEfficiency
 
  // Hadronic truth tau container
  const TruthParticleContainer* hadTau = 0;

  /** retrieve a container of hadronic taus and define iterators*/
  hadTau = truthHandler.getHadronicTruth(mcpart);
  //truthHandler.finalize();
  
  TruthParticleContainer::const_iterator truthTauItr  = hadTau->begin();
  TruthParticleContainer::const_iterator truthTauItrE = hadTau->end();

  matchtool.MatchingTools::setdeltaRMatchCut(m_deltaRMatchCut);

  // match tau candidates to hadronic truth taus
  for (; truthTauItr != truthTauItrE; ++truthTauItr) {//truth loop

    CLHEP::HepLorentzVector visSum = truthHandler.getTauVisibleSumTruth(*truthTauItr);

    double pTvis  = visSum.et()/GeV;
    double etavis = visSum.eta();
    double phivis = visSum.phi();

    if (pTvis < 10.) continue;
    if (fabs(etavis) > 2.5) continue;
    
    int nPiCh = truthHandler.nProngTruth(*truthTauItr);
    // cout << ">>>> nPiCh " << nPiCh << endl;
    if (nPiCh == 1) m_oneProng++;
    if (nPiCh == 3) m_threeProng++;

    float Effic = 0.;   float EfficSel = 0.;
    float Effic1P = 0.; float EfficSel1P = 0.;
    float Effic3P = 0.; float EfficSel3P = 0.;

    bool isMatched = false;
    
    const Analysis::TauJet* chosen = matchtool.matchTruthGeomOnly( (*truthTauItr), tau, isMatched);
    
    if( (isMatched == true) && (chosen!=NULL) ) {//matched      
      
      //Look at all calo seeded candidates
      if(recoHandler.TauType(chosen) == TAU_REC || recoHandler.TauType(chosen) == TAU_BOTH) {
	
	int ntrack = chosen->numTrack(); //use loose track for TAU_REC
	
	Effic = 1.0;
	if ((nPiCh == 1) && (ntrack == 1)) Effic1P = 1.0;
	if ((nPiCh == 3) && (ntrack == 3)) Effic3P = 1.0;
	
	//use Likelihood2008
	if(this->recoHandler.selectTauRecJet( chosen, 5, 4.0, true) ) {
	  EfficSel = 1.0;
	  if ((nPiCh == 1) && (ntrack == 1)) EfficSel1P = 1.0;
	  if ((nPiCh == 3) && (ntrack == 3)) EfficSel3P = 1.0;
	}
      }
    }//matched
    
    //cout << ">>> Effic " << Effic << " EfficSel " << EfficSel << endl;
    
    //eff histos
    if ( nPiCh == 1 ) {
      
      //rec eff histos
      m_eff_reco_1P_TauRec->Fill(pTvis,Effic1P );
      m_eff_reco_1Pphi_TauRec->Fill(phivis,Effic1P );
      m_eff_reco_1Peta_TauRec->Fill(etavis,Effic1P );
      
      //id eff histos
      m_eff_id_1P_TauRec->Fill(pTvis,EfficSel1P );
      m_eff_id_1Pphi_TauRec->Fill(phivis,EfficSel1P );
      m_eff_id_1Peta_TauRec->Fill(etavis,EfficSel1P );    

    }
   
    if ( nPiCh == 3 ) {
      
      //rec eff histos
      m_eff_reco_3P_TauRec->Fill(pTvis,Effic3P );
      m_eff_reco_3Pphi_TauRec->Fill(phivis,Effic3P );
      m_eff_reco_3Peta_TauRec->Fill(etavis,Effic3P );
      
      //id eff histos
      m_eff_id_3P_TauRec->Fill(pTvis,EfficSel3P );
      m_eff_id_3Pphi_TauRec->Fill(phivis,EfficSel3P );
      m_eff_id_3Peta_TauRec->Fill(etavis,EfficSel3P );
      
    }
    
    //rec eff histos
    m_eff_reco_13P_TauRec->Fill(pTvis,Effic );
    m_eff_reco_13Pphi_TauRec->Fill(phivis,Effic );
    m_eff_reco_13Peta_TauRec->Fill(etavis,Effic );
    
    //id eff histos
    m_eff_id_13P_TauRec->Fill(pTvis,EfficSel );
    m_eff_id_13Pphi_TauRec->Fill(phivis,EfficSel );
    m_eff_id_13Peta_TauRec->Fill(etavis,EfficSel );
      
    
  }//truth loop

  return StatusCode::SUCCESS;

}//getTauRecProngEfficiency

/////////////////////////////////////////////////////////////////////////////////////
/// get the tauRec resolution
StatusCode TauExamples::getTauRecResolution(const Analysis::TauJetContainer *tau, const TruthParticleContainer *mcpart){//getTauRecResolution

  // Hadronic truth tau container
  const TruthParticleContainer* hadTau = 0;
  
  /** retrieve a container of hadronic taus and define iterators*/
  hadTau = truthHandler.getHadronicTruth(mcpart);
  
  TruthParticleContainer::const_iterator truthTauItr  = hadTau->begin();
  TruthParticleContainer::const_iterator truthTauItrE = hadTau->end();

  matchtool.MatchingTools::setdeltaRMatchCut(m_deltaRMatchCut);

  // match tau candidates to hadronic truth taus
  for (; truthTauItr != truthTauItrE; ++truthTauItr) {//truth loop
    
    CLHEP::HepLorentzVector visSum = truthHandler.getTauVisibleSumTruth(*truthTauItr);
    
    float pTvis  = visSum.et();
    float etavis = visSum.eta();
    float phivis = visSum.phi();

    bool isMatched = false;

    const Analysis::TauJet* chosen = matchtool.matchTruthGeomOnly( (*truthTauItr), tau, isMatched);
    
    if( (isMatched == true) && (chosen!=NULL) ) {//matched

      //Look at all calo seeded candidates
      if(recoHandler.TauType(chosen) == TAU_REC || recoHandler.TauType(chosen) == TAU_BOTH) {
	
	//for calo seeded candidates get (eta,phi) is obtained from calorimeter
	
	const Analysis::TauCommonDetails*  p_taudetails(0);  
	p_taudetails = chosen->details<const Analysis::TauCommonDetails>(); 
	
	if(p_taudetails){//make sure the pointer to details is valid!!!
	  
	  double et  = chosen->et();
	  double eta = p_taudetails->seedCalo_eta();
	  double phi = p_taudetails->seedCalo_phi();
	
	  m_etRes_reco_TauRec->Fill(( pTvis - et)/pTvis);
	  m_etaRes_reco_TauRec->Fill(etavis - eta);
	  m_phiRes_reco_TauRec->Fill(phivis - phi);
	  
	  //use Likelihood2008      
	  if(this->recoHandler.selectTauRecJet( chosen, 5, 4.0, true) ) {//selectTauJet
	    
	    m_etRes_id_TauRec->Fill(( pTvis - et)/pTvis);
	    m_etaRes_id_TauRec->Fill(etavis - eta);
	    m_phiRes_id_TauRec->Fill(phivis - phi);
	    
	  }//selectTauJet
	}//details
      }
    }//matched
  }//truthloop
  
  return StatusCode::SUCCESS;
  
}//getTauRecResolution

/////////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode TauExamples::finalizeProng() {

  // Rather than dumping summary information to the message log send it to 
  // special summary file. This way it won't get lost in error messages.
  std::ofstream* outfile = new std::ofstream("TauToolProngExample.log");

  *outfile << " 1Prong: " << m_oneProng << endl;
  *outfile << " 3Prong: " << m_threeProng << endl;

  outfile->close();
  delete outfile;

  TFile histofile("TauToolProngExample.root","RECREATE");  
  histofile.cd();

  m_eff_reco_1P_TauRec->Write();
  m_eff_reco_3P_TauRec->Write(); 
  m_eff_reco_13P_TauRec->Write();
  
  m_eff_reco_1Pphi_TauRec->Write();
  m_eff_reco_3Pphi_TauRec->Write(); 
  m_eff_reco_13Pphi_TauRec->Write();
  
  m_eff_reco_1Peta_TauRec->Write();
  m_eff_reco_3Peta_TauRec->Write(); 
  m_eff_reco_13Peta_TauRec->Write();
  
  m_eff_id_1P_TauRec->Write();
  m_eff_id_3P_TauRec->Write(); 
  m_eff_id_13P_TauRec->Write();
  
  m_eff_id_1Pphi_TauRec->Write();
  m_eff_id_3Pphi_TauRec->Write(); 
  m_eff_id_13Pphi_TauRec->Write();
  
  m_eff_id_1Peta_TauRec->Write();
  m_eff_id_3Peta_TauRec->Write(); 
  m_eff_id_13Peta_TauRec->Write();

  histofile.Write();
  histofile.Close();

  return StatusCode::SUCCESS;

}

/////////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode TauExamples::finalizeResolution() {

  TFile histofile("TauToolResolutionExample.root","RECREATE");  
  histofile.cd();

  m_etRes_reco_TauRec->Write();
  m_etaRes_reco_TauRec->Write();
  m_phiRes_reco_TauRec->Write();
  
  m_etRes_id_TauRec->Write();
  m_etaRes_id_TauRec->Write();
  m_phiRes_id_TauRec->Write();

  histofile.Write();
  histofile.Close();

  return StatusCode::SUCCESS;
}
/////////////////////////////////////////////////////////////////////////////////////
