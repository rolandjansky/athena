/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************************
// tauMonTool.cxx : Implementation of offline tau Data Quality
//                  histogramming, inheriting from ManagedMonitorToolBase
//
// authors:  C Cuenca Almenar, S Demers, E Ideal, A Leister, YALE
//			Felix Friedrich (basic xAOD migration)
//			Lianyou SHAN  ( update for run2 )
//*********************************************************************

#include "tauMonitoring/tauMonTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/TrackParticle.h"
#include "TrigDecisionTool/TrigDecisionTool.h" 

using xAOD::EventInfo;
using CLHEP::GeV;

#define CRACK_MIN 1.3
#define CRACK_MAX 1.7

#define PHIMIN -3.1415936
#define PHIMAX 3.1415936 

tauMonTool::tauMonTool( const std::string & type,
		const std::string & name,
		const IInterface* parent ) :ManagedMonitorToolBase( type, name, parent ),
		m_tauJetKey("TauJets"),
                m_doTrigger(false), 
		m_maxNLB(1000) 
// m_trigDecTool("Trig::TrigDecisionTool/TrigDecisionTool") 
// lets use m_trigDecToolTool from base class
{
	declareProperty("doTrigger", m_doTrigger, "Run trigger aware monitoring. Only if trigger data is available");
	declareProperty("doLowStat", m_doLS, "monitor per 20 LumiBlock ");
	declareProperty( "TauEleJetMbtsTriggerItems", m_trigItems, "vector<vector<string> > to itemize tau like triggers ");
}

tauMonTool::~tauMonTool() {}

//--------------------------------------------------------------------------------
// Book Histograms
//--------------------------------------------------------------------------------
StatusCode tauMonTool::bookHistograms()
{
	ATH_MSG_INFO("StatusCode tauMonTool::bookHistograms, doTrigger = " << m_doTrigger);

	//--------------------
	// Top level folder
	//--------------------
	std::string folderName = "Tau";
	MonGroup mainFolder (this, folderName, run);
        MonGroup lowStatFds( this, folderName, lowStat ) ;

        if ( m_doLS && ( newRunFlag() || newLowStatIntervalFlag() ) )
        { 
          if ( bookBasicPlots(m_basic_LS, lowStatFds,"" ).isFailure() )  
            ATH_MSG_ERROR("Couldn't book Low Stat basic histograms");
	  if ( bookHistos( m_tauLS, folderName , lowStat ).isFailure() )		
            ATH_MSG_ERROR("Couldn't book Low Stat histograms");
        } ;

	if ( bookBasicPlots(m_basicPlots, mainFolder,"" ).isFailure() )	 	ATH_MSG_ERROR("Couldn't book basicPlots histograms");
	// Tau Barrel Monitoring groups
	if ( bookHistos(m_tauB, folderName + "/TauB", run).isFailure())		ATH_MSG_ERROR("Couldn't book barrel histograms");
	// Tau Endcap Monitoring groups
	if ( bookHistos(m_tauE, folderName + "/TauE", run).isFailure())		ATH_MSG_ERROR("Couldn't book endcap histograms");
	// Tau Crack Monitoring groups
	if ( bookHistos(m_tauCR, folderName + "/TauCR", run).isFailure()) 	ATH_MSG_ERROR("Couldn't book crack histograms");

	//--------------------
	// Tau Trigger-Aware Monitoring Groups
	//--------------------

        // We wish retrieve twice won'nt invite problem.
	m_Trigged = false ; 
	if ( m_doTrigger ) {
        if ( m_trigDecTool.retrieve().isSuccess() ) m_Trigged = true;
		if ( m_Trigged ) {
			//--------------------
			// create trigger folder and trigger summary histogram
			//--------------------
			MonGroup tau_Trigger (this, "Tau/Trigger", run);
			StatusCode sc = Book1DHist ( &m_triggers, &tau_Trigger, "TriggerChains", "Fired triggers per tau candidate", 5, 0.5, 5.5);
			if (sc.isSuccess()) {
				m_triggers->GetXaxis()->SetBinLabel(1,"EM");
				m_triggers->GetXaxis()->SetBinLabel(2,"Jet");
				m_triggers->GetXaxis()->SetBinLabel(3,"Taus");
				m_triggers->GetXaxis()->SetBinLabel(4,"MBTS");
				m_triggers->GetXaxis()->SetBinLabel(5,"Unchecked");

			} else {
				ATH_MSG_ERROR("Couldn't book m_triggers histogram");
			}

                        MonGroup tau_Trigger_Tau (this, "Tau/Trigger/TauTrig" , run);
			MonGroup tau_Trigger_Em  (this, "Tau/Trigger/EleTrig"  , run);
                        MonGroup tau_Trigger_Jet (this, "Tau/Trigger/JetTrig" , run);
			MonGroup tau_Trigger_Mbts(this, "Tau/Trigger/Mbts", run);

			//--------------------
			// book trigger plots
			//--------------------
                        CHECK( Book2DHist ( &m_triggers_vs_LB, &tau_Trigger, "TriggerChains_LB", "Triggered chain vs Lumiblock; trigger;Lumiblock", 5, 0.5, 5.5, m_maxNLB/10+1, -5.0, (double)m_maxNLB+5.0) );
			m_triggers_vs_LB->GetXaxis()->SetBinLabel(1,"EM");
			m_triggers_vs_LB->GetXaxis()->SetBinLabel(2,"Jet");
			m_triggers_vs_LB->GetXaxis()->SetBinLabel(3,"Taus");
			m_triggers_vs_LB->GetXaxis()->SetBinLabel(4,"MBTS");
			m_triggers_vs_LB->GetXaxis()->SetBinLabel(5,"Unchecked");


                        if ( bookBasicPlots(m_tauBasePlots, tau_Trigger_Tau ,"tau_" ).isFailure() ) ATH_MSG_ERROR("Couldn't book Tau trigger histograms");
                        if ( bookHistos( m_tauPlots, "Tau/Trigger/TauTrig", run).isFailure()) 
          ATH_MSG_ERROR("Couldn't book Tau trigger histograms");

                        if ( bookBasicPlots(m_jetPlots,  tau_Trigger_Jet ,"jet_" ).isFailure() )
          ATH_MSG_ERROR("Couldn't book jetTrigger histograms");
			if ( bookBasicPlots(m_emPlots,   tau_Trigger_Em  ,"em_"  ).isFailure() )
          ATH_MSG_ERROR("Couldn't book emTrigger histograms");
                        if ( bookBasicPlots(m_mbtsPlots, tau_Trigger_Mbts,"mbts_").isFailure() )
          ATH_MSG_ERROR("Couldn't book mbtsTrigger histograms");

		} else {
			ATH_MSG_ERROR("couldn't retrieve trigger info");
			return StatusCode::FAILURE;
		}
	}

	//------------------------
	// book physics histograms
	//------------------------
	if (bookPhysicsHistograms().isFailure() ) ATH_MSG_ERROR("Failed to book physics histograms");

	return StatusCode::SUCCESS;
}


//--------------------------------------------------------------------------------
// fill histograms
//--------------------------------------------------------------------------------
StatusCode tauMonTool::fillHistograms()
{
	//--------------------
	//figure out current LB
	//--------------------
	const DataHandle<EventInfo> evtInfo;
	if ( (evtStore()->retrieve(evtInfo)) . isFailure()) {
		ATH_MSG_ERROR("couldn't retrieve event info");
		return StatusCode::FAILURE;
	}

	//LAr event veto: skip events rejected by LAr

	if(evtInfo->errorState(EventInfo::LAr)==EventInfo::Error){
		ATH_MSG_WARNING("SKIP for LAR error");
		return StatusCode::SUCCESS;
	}

	m_currentLB = evtInfo->lumiBlock();
	//--------------------
	//Figure out trigger stuff
	//--------------------
	bool jetPassed  = false;
	bool emPassed   = false;
	bool tauPassed  = false;
	bool mbtsPassed = false;
        double emrndm = 0 ;
        double jetrndm = 0 ;


	if ( m_doTrigger ) 
        {
          if ( m_Trigged ) 
          {
            for ( unsigned int grp = 0 ; grp < m_trigItems.size() ; grp ++ )
            {
              int pass = 0 ;
              for ( unsigned int t = 0 ; t < ( m_trigItems[grp]).size() ; t ++ )
              {
                const char *grpItm = ( m_trigItems[grp][t] ).c_str() ;
/**
                const Trig::ChainGroup* cg 
                  = m_trigDecTool->getChainGroup( ( m_trigItems[grp][t] ).c_str() ) ;
                if ( cg->isPassed() ) pass ++ ;
**/
                if ( m_trigDecTool->isPassed( grpItm ) ) pass++ ;
              }
              if ( pass > 0 )
              { 
                if ( grp == 0 ) tauPassed = true ;
                else if ( grp == 1 && ! tauPassed ) emPassed = true ;
                else if ( grp == 2 && ! ( emPassed || tauPassed ) ) jetPassed = true ;
                else {  if ( grp == 3 && ! ( jetPassed || emPassed || tauPassed  ) )  
                          mbtsPassed = true ; }
              }
            }
          } else 
          { 
            ATH_MSG_ERROR("couldn't read trigger info");
            return StatusCode::FAILURE;
          }
        }  
   
	//--------------------
	//Get Tau container
	//--------------------
	const xAOD::TauJetContainer  *tau_container;
	if ( (evtStore()->retrieve(tau_container,m_tauJetKey)).isFailure() ) {
		ATH_MSG_WARNING("Cannot retrieve " << m_tauJetKey);
		return StatusCode::FAILURE;
	}

	int ntaus   = tau_container->size();

	int ntausB  = 0;
	int ntausCR = 0;
	int ntausE  = 0;
	int nHighPTtausB  = 0;
	int nHighPTtausCR = 0;
	int nHighPTtausE  = 0;

	//----------------
	// Loop over taus
	//----------------
	for (auto tau : *tau_container) {

		//--------------------
		// fill other basic histograms
		//--------------------
          fillBasicPlots( m_basicPlots, tau);
          if ( m_doLS ) fillBasicPlots( m_basic_LS, tau);

          if (m_doTrigger) {
            if ( m_Trigged ) {
              if ( tauPassed) { 
                if (fillHistogramsTauTrig(m_tauPlots, tau).isFailure()) 
                  ATH_MSG_ERROR("Failed to fill Tau Trigger histograms");  
                fillBasicPlots(m_tauBasePlots, tau) ;
                m_triggers->Fill(3); 
                m_triggers_vs_LB->Fill( 3, m_currentLB ) ;
              }
              else if (  emPassed && emrndm < 0.5 ) { 
                fillBasicPlots(m_emPlots, tau) ;
                m_triggers->Fill(1); 
                m_triggers_vs_LB->Fill( 1, m_currentLB ) ;
              }
              else if ( jetPassed && jetrndm < 0.5 ) { 
                fillBasicPlots(m_jetPlots, tau) ;
                m_triggers->Fill(2); 
                m_triggers_vs_LB->Fill( 2, m_currentLB ) ;
              }
              else if (mbtsPassed) { 
                fillBasicPlots(m_mbtsPlots, tau) ;
                m_triggers->Fill(4); 
                m_triggers_vs_LB->Fill( 4, m_currentLB ) ;
              }
              else { 
                m_triggers->Fill(5) ;
                m_triggers_vs_LB->Fill( 5, m_currentLB ) ;
              }
            }
	  }

		//--------------------
		// fill histograms
		//--------------------
          float pTGev = tau->pt() / GeV;
          float abs_eta = fabs(tau->eta());

          if (abs_eta <= CRACK_MIN ) {
            if (fillHistograms(m_tauB, tau).isFailure()) ATH_MSG_ERROR("Failed to fill barrel histograms");
            ntausB++;
            if (pTGev > 100.0){ nHighPTtausB++; }
          }
          if (abs_eta > CRACK_MAX ) {
            if (fillHistograms(m_tauE, tau).isFailure()) ATH_MSG_ERROR("Failed to fill endcap histograms");
            ntausE++;
            if (pTGev > 100.0){ nHighPTtausE++; }
          }
          if (abs_eta > CRACK_MIN && abs_eta < CRACK_MAX ) {
            if (fillHistograms(m_tauCR, tau).isFailure()) ATH_MSG_ERROR("Failed to fill crack histograms");
            ntausCR++;
            if (pTGev > 100.0){ nHighPTtausCR++; }
	  }

          if ( m_doLS )
            if (fillHistogramsLowStat(m_tauLS, tau).isFailure()) 
              ATH_MSG_ERROR("Failed to fill Low Stat histograms");

		//-----------------------
		//Fill physics histograms
		//-----------------------
          if(fillPhysicsHistograms(tau).isFailure()) ATH_MSG_ERROR("Failed to fill physics histograms");

	} // end of loop over taus

	//--------------------
	// fill ntaus
	//--------------------
	m_basicPlots.h_ntaus->Fill(ntaus);
        int hitau = nHighPTtausB + nHighPTtausCR + nHighPTtausE ;
	m_basicPlots.h_nHighPTtaus->Fill( hitau );
        if ( m_doLS )
        {
	  m_basic_LS.h_ntaus->Fill(ntaus);
	  m_basic_LS.h_nHighPTtaus->Fill( hitau );
        }

	if (m_doTrigger)
		if ( m_Trigged ) {
			if ( tauPassed) 
                        {
                          m_tauBasePlots.h_ntaus->Fill(ntaus);
                          m_tauBasePlots.h_nHighPTtaus->Fill( 
                            nHighPTtausB + nHighPTtausCR + nHighPTtausE );
                        } 

			if ( jetPassed) m_jetPlots .h_ntaus->Fill(ntaus);
			if (  emPassed) m_emPlots  .h_ntaus->Fill(ntaus);
			if (mbtsPassed) m_mbtsPlots.h_ntaus->Fill(ntaus);
		}

	m_tauB .kinFolder.h_ntaus->Fill(ntausB);
	m_tauCR.kinFolder.h_ntaus->Fill(ntausCR);
	m_tauE .kinFolder.h_ntaus->Fill(ntausE);

	m_tauB .trkFolder.h_nHighPTtaus->Fill(nHighPTtausB);
	m_tauCR.trkFolder.h_nHighPTtaus->Fill(nHighPTtausCR);
	m_tauE .trkFolder.h_nHighPTtaus->Fill(nHighPTtausE);
        if ( m_doLS ) m_tauLS.trkFolder.h_nHighPTtaus->Fill( hitau  ) ;

	return StatusCode::SUCCESS;
}


//--------------------------------------------------------------------------------
// post processing
//--------------------------------------------------------------------------------
StatusCode tauMonTool::procHistograms()
{
  if( endOfRunFlag() || endOfLumiBlockFlag()) {
          //fill the vs_LB plots
    if (m_currentLB <= m_maxNLB) 
    {
      m_basicPlots.h_ntaus_vs_LB->Fill( m_currentLB, m_basicPlots.h_ntausLB );
      if ( m_doLS ) m_basic_LS.h_ntaus_vs_LB->Fill( m_currentLB,m_basic_LS.h_ntausLB );
      if (m_doTrigger && m_Trigged ) 
        m_tauBasePlots.h_ntaus_vs_LB->Fill( m_currentLB, m_tauBasePlots.h_ntausLB);
    }
	  //reset counters
    m_basicPlots.h_ntausLB = 0;
    if ( m_doLS ) m_basic_LS.h_ntausLB = 0;
    if (m_doTrigger && m_Trigged  ) m_tauBasePlots.h_ntausLB = 0;

  }

  return StatusCode::SUCCESS;
}


//--------------------------------------------------------------------------------
// fill methods
//--------------------------------------------------------------------------------
void tauMonTool::fillBasicPlots(s_basicPlots& someBasicPlots, const xAOD::TauJet* tau)
{
	someBasicPlots.h_ntausLB ++;

	float eta     = tau->eta();
	float et      = tau->pt() / GeV;
	float phi     = tau->phi();
	int   charge  = (int) tau->charge();
	int numTracks = (int) tau->nTracks();
        int numClusters = (int) tau->detail<int>(xAOD::TauJetParameters::numTopoClusters) ;
	someBasicPlots.h_eta      ->Fill( eta );
	someBasicPlots.h_phi      ->Fill( phi );
	someBasicPlots.h_et       ->Fill( et );
	someBasicPlots.h_charge   ->Fill( charge);
	someBasicPlots.h_numTracks->Fill( numTracks );
        someBasicPlots.h_nclst    ->Fill( numClusters ) ;

	someBasicPlots.h_EtVsEta  ->Fill( eta, et);
	someBasicPlots.h_EtVsPhi  ->Fill( phi, et);
	someBasicPlots.h_PhiVsEta ->Fill( eta, phi);

	if ( et > 15.0 ) someBasicPlots.h_PhiVsEta_et15 ->Fill( eta, phi);
	if ( et > 15.0 && tau->isTau(xAOD::TauJetParameters::JetBDTSigLoose) ) 
          someBasicPlots.h_PhiVsEta_et15_BDTLoose ->Fill( eta, phi);

	someBasicPlots.h_Eta_vs_LB->Fill( eta, m_currentLB);
	someBasicPlots.h_Phi_vs_LB->Fill( phi, m_currentLB);

	return;
}

StatusCode tauMonTool::fillHistograms(s_mainFolder& mainFolder, const xAOD::TauJet* tau)
{
        if ( fillKinHistos  (mainFolder.kinFolder, tau) . isFailure())  
          ATH_MSG_ERROR("Failed to fill kinematic histograms");
	if ( fillIDHistos   (mainFolder.idFolder, tau) . isFailure())   		ATH_MSG_ERROR("Failed to fill identification histograms");
	if ( fillTrackHistos(mainFolder.trkFolder, tau) . isFailure())  		ATH_MSG_ERROR("Failed to fill traking histograms");
	if ( fillCaloHistos (mainFolder.caloFolder, tau) . isFailure()) 		ATH_MSG_ERROR("Failed to fill calorimeter histograms");
	if ( fillSubStructureHistos (mainFolder.sbstrctFolder, tau) . isFailure())	ATH_MSG_ERROR("Failed to fill calorimeter histograms");
	return  StatusCode::SUCCESS;
}

StatusCode tauMonTool::fillHistogramsLowStat(s_mainFolder& mainFolder, const xAOD::TauJet* tau)
{
	if ( fillTrackHistos(mainFolder.trkFolder, tau) . isFailure())  		ATH_MSG_ERROR("Failed to fill traking histograms");
	if ( fillCaloHistos (mainFolder.caloFolder, tau) . isFailure()) 		ATH_MSG_ERROR("Failed to fill calorimeter histograms");
	return  StatusCode::SUCCESS;
}


StatusCode tauMonTool::fillHistogramsTauTrig(s_mainFolder& mainFolder, const xAOD::TauJet* tau)
{
	if ( fillIDHistos   (mainFolder.idFolder, tau) . isFailure())   		ATH_MSG_ERROR("Failed to fill identification histograms");
	if ( fillTrackHistos(mainFolder.trkFolder, tau) . isFailure())  		ATH_MSG_ERROR("Failed to fill traking histograms");
	if ( fillCaloHistos (mainFolder.caloFolder, tau) . isFailure()) 		ATH_MSG_ERROR("Failed to fill calorimeter histograms");
	if ( fillSubStructureHistos (mainFolder.sbstrctFolder, tau) . isFailure())	ATH_MSG_ERROR("Failed to fill calorimeter histograms");
	return  StatusCode::SUCCESS;
}

StatusCode tauMonTool::fillKinHistos(s_kinFolder& folder, const xAOD::TauJet* tau)
{

	float eta     = tau->eta();
	float et      = tau->pt() / GeV;
	float phi     = tau->phi();
        int   charge  = (int) tau->charge();

 	folder.h_eta->Fill( eta );
	folder.h_phi->Fill( phi );
	folder.h_et->Fill( et );
	folder.h_charge->Fill( charge);

	folder.h_PhiVsEta->Fill( eta, phi);
	folder.h_Eta_vs_LB->Fill( eta, m_currentLB);
	folder.h_Phi_vs_LB->Fill( phi, m_currentLB);

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::fillIDHistos(s_idFolder& folder,  const xAOD::TauJet* tau)
{
	folder.h_tauBDTLoose         ->Fill(tau->isTau(xAOD::TauJetParameters::JetBDTSigLoose));
	folder.h_tauBDTMedium        ->Fill(tau->isTau(xAOD::TauJetParameters::JetBDTSigMedium));
	folder.h_tauBDTTight         ->Fill(tau->isTau(xAOD::TauJetParameters::JetBDTSigTight));
	// folder.h_tauLlhLoose         ->Fill(tau->isTau(xAOD::TauJetParameters::TauLlhLoose));
	// folder.h_tauLlhMedium        ->Fill(tau->isTau(xAOD::TauJetParameters::TauLlhMedium));
	// folder.h_tauLlhTight         ->Fill(tau->isTau(xAOD::TauJetParameters::TauLlhTight));
	// folder.h_electronVetoLoose   ->Fill(tau->isTau(xAOD::TauJetParameters::ElectronVetoLoose));
	// folder.h_electronVetoMedium  ->Fill(tau->isTau(xAOD::TauJetParameters::ElectronVetoMedium));
	// folder.h_electronVetoTight   ->Fill(tau->isTau(xAOD::TauJetParameters::ElectronVetoTight));
	folder.h_muonVeto            ->Fill(tau->isTau(xAOD::TauJetParameters::MuonVeto));
	folder.h_eleBDTLoose         ->Fill(tau->isTau(xAOD::TauJetParameters::EleBDTLoose));
	folder.h_eleBDTMedium        ->Fill(tau->isTau(xAOD::TauJetParameters::EleBDTMedium));
	folder.h_eleBDTTight         ->Fill(tau->isTau(xAOD::TauJetParameters::EleBDTTight));
	folder.h_BDTJetScore         ->Fill(tau->discriminant(xAOD::TauJetParameters::BDTJetScore));

	folder.h_BDTJetScoreSigTrans ->Fill(tau->discriminant(xAOD::TauJetParameters::BDTJetScoreSigTrans));
	//folder.h_BDTJetScoreBkgTrans ->Fill(tau->discriminant(xAOD::TauJetParameters::BDTJetScoreBkgTrans));

	if(tau->isTau(xAOD::TauJetParameters::JetBDTSigLoose) && tau->pt() >= 15000.0 ){
		if ( fillBDTHistos   (folder.BDTLooseFolder, tau) . isFailure()) ATH_MSG_ERROR("Failed to fill JetBDTLoose histograms");
	}

	if(tau->isTau(xAOD::TauJetParameters::JetBDTSigMedium)){
		if ( fillBDTHistos   (folder.BDTMedFolder, tau) . isFailure()) ATH_MSG_ERROR("Failed to fill JetBDTMedium histograms");
	}

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::fillTrackHistos(s_trkFolder& folder, const xAOD::TauJet* tau)
{
	if ( tau->nTracks() == 0) return StatusCode::SUCCESS;

	folder.h_massTrkSys            ->Fill( tau->detail<float>(xAOD::TauJetParameters::massTrkSys) / GeV);
	folder.h_trkWidth2             ->Fill( tau->detail<float>(xAOD::TauJetParameters::trkWidth2) );
	folder.h_trFlightPathSig       ->Fill( tau->detail<float>(xAOD::TauJetParameters::trFlightPathSig) );
	folder.h_ipSigLeadTrk          ->Fill( tau->detail<float>(xAOD::TauJetParameters::ipSigLeadTrk) );
	folder.h_ipZ0SinThetaSigLeadTrk->Fill( tau->detail<float>(xAOD::TauJetParameters::ipZ0SinThetaSigLeadTrk) );
	folder.h_leadTrkPt             ->Fill( tau->detail<float>(xAOD::TauJetParameters::leadTrkPt) / GeV );
	folder.h_etOverPtLeadTrack	   ->Fill( tau->detail<float>(xAOD::TauJetParameters::etOverPtLeadTrk) );

	//--------------------
	// track info
	//--------------------
//     if ( tau->nTracks() == 0) return StatusCode::SUCCESS;

	const xAOD::TrackParticle* track = tau->track(0)->track();
	const Trk::Perigee perigee = track->perigeeParameters();

	uint8_t dummy(0);
	if (track->summaryValue(dummy, xAOD::numberOfTRTHighThresholdHits)) 	folder.h_numberOfTRTHighThresholdHits	 ->Fill( dummy );
	if (track->summaryValue(dummy, xAOD::numberOfTRTHits)) 					folder.h_numberOfTRTHits                 ->Fill( dummy );
	if (track->summaryValue(dummy, xAOD::numberOfSCTHits)) 					folder.h_numberOfSCTHits                 ->Fill( dummy );
	if (track->summaryValue(dummy, xAOD::numberOfPixelHits)) 				folder.h_numberOfPixelHits               ->Fill( dummy );
	if (track->summaryValue(dummy, xAOD::numberOfTRTHighThresholdOutliers)) folder.h_numberOfTRTHighThresholdOutliers->Fill( dummy );
	if (track->summaryValue(dummy, xAOD::numberOfTRTOutliers)) 				folder.h_numberOfTRTOutliers             ->Fill( dummy );

	folder.h_d0                              ->Fill(perigee.parameters()[Trk::d0]);
	folder.h_z0                              ->Fill(perigee.parameters()[Trk::z0]);
	folder.h_phi                             ->Fill(perigee.parameters()[Trk::phi]);
	folder.h_eta                             ->Fill(perigee.eta());
	folder.h_pT                              ->Fill(perigee.pT()/ GeV);
	folder.h_z0_vs_LB                        ->Fill(perigee.parameters()[Trk::z0], m_currentLB);

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::fillCaloHistos(s_caloFolder& folder, const xAOD::TauJet* tau)
{
	folder.h_eta                   ->Fill( tau->eta() );
	folder.h_phi                   ->Fill( tau->phi() );
	// folder.h_etEMCalib             ->Fill( tau->detail<float>(xAOD::TauJetParameters::etEMCalib) / GeV);
	// folder.h_etHadCalib            ->Fill( tau->detail<float>(xAOD::TauJetParameters::etHadCalib) / GeV);
	folder.h_etEMAtEMScale         ->Fill( tau->detail<float>(xAOD::TauJetParameters::etEMAtEMScale) / GeV);
	folder.h_etHadAtEMScale        ->Fill( tau->detail<float>(xAOD::TauJetParameters::etHadAtEMScale) / GeV);
	folder.h_EMRadius              ->Fill( tau->detail<float>(xAOD::TauJetParameters::EMRadius) );
	folder.h_centFrac              ->Fill( tau->detail<float>(xAOD::TauJetParameters::centFrac) );
	folder.h_hadRadius             ->Fill( tau->detail<float>(xAOD::TauJetParameters::hadRadius) );
	folder.h_isolFrac              ->Fill( tau->detail<float>(xAOD::TauJetParameters::isolFrac) );
	folder.h_stripWidth2           ->Fill( tau->detail<float>(xAOD::TauJetParameters::stripWidth2) );
	folder.h_nStrip                ->Fill( tau->detail<int>(xAOD::TauJetParameters::nStrip) );
	folder.h_trkAvgDist            ->Fill( tau->detail<float>(xAOD::TauJetParameters::trkAvgDist) );
	folder.h_dRmax                 ->Fill( tau->detail<float>(xAOD::TauJetParameters::dRmax) );
	folder.h_centFrac_vs_LB        ->Fill( tau->detail<float>(xAOD::TauJetParameters::centFrac), m_currentLB);
	folder.h_isolFrac_vs_LB        ->Fill( tau->detail<float>(xAOD::TauJetParameters::isolFrac), m_currentLB);

	return  StatusCode::SUCCESS;
}

StatusCode tauMonTool::fillBDTHistos(s_BDTFolder& folder, const xAOD::TauJet* tau)
{
	float eta     = tau->eta();
	float et      = tau->pt() / GeV;
	float phi     = tau->phi();
	int numTracks = (int) tau->nTracks();

	folder.h_et     ->Fill(et);
	folder.h_eta    ->Fill(eta);
	folder.h_phi    ->Fill(phi);
	folder.h_nTracks->Fill(numTracks);

	return StatusCode::SUCCESS;
}


StatusCode tauMonTool::fillSubStructureHistos( s_sbstrctFolder& folder, const xAOD::TauJet* tau)
{

   //  CaloCluster variabls
        folder.h_InvMass->Fill( tau->detail<float>( xAOD::TauJetParameters::effTopoInvMass ) / GeV ) ;
        folder.h_L2EOverAllClusterE->Fill( tau->detail<float>( xAOD::TauJetParameters::lead2ClusterEOverAllClusterE ) ) ;
        folder.h_IsoCorr->Fill( tau->detail<float>( xAOD::TauJetParameters::caloIsoCorrected ) / GeV ) ;

  // Track ( association ) variabls
        folder.h_PSSFrac->Fill( tau->detail<float>( xAOD::TauJetParameters::PSSFraction ) ) ;
        folder.h_EMFracTrk->Fill( tau->detail<float>( xAOD::TauJetParameters::ChPiEMEOverCaloEME ) ) ;
        folder.h_RptApprox->Fill( tau->detail<float>( xAOD::TauJetParameters::ptRatioEflowApprox ) ) ;

        int nShot = tau->nShotPFOs() ;
        folder.h_nShot->Fill( nShot ) ;
        for ( int s = 0 ; s < nShot ; s++ ) 
        {
          const xAOD::PFO* shot = tau->shotPFO( s ) ;
          if ( shot != NULL ) 
          {
            float pt3 = -9.0 ;
            shot->attribute(xAOD::PFODetails::PFOAttributes::tauShots_pt3, pt3 ) ;
            folder.h_stpt3->Fill( pt3 / GeV ) ;
          }
        }

       // charged pion substraction 

        unsigned nNeutPFO = tau->nProtoNeutralPFOs(); ;
        folder.h_nNeutPFO->Fill( nNeutPFO ) ;
        for ( unsigned int np = 0 ; np < nNeutPFO ; np ++ ) 
        {
          const xAOD::PFO* npfo = tau->protoNeutralPFO( np ) ;
          float bdtScore = npfo->bdtPi0Score();
          folder.h_pi0bdt->Fill( bdtScore ) ;
        }

       //  results from panTau 
        int panmode = -1 ;
        tau->panTauDetail(xAOD::TauJetParameters::PanTau_DecayMode, panmode); 
        folder.h_panmode->Fill( panmode ) ;

        float pphi = tau->phiPanTauCellBased() ;
        if ( pphi > -100 )
        {
          folder.h_panpt->Fill( tau->ptPanTauCellBased() / GeV  ) ;
          folder.h_paneta->Fill( tau->etaPanTauCellBased() ) ;
          folder.h_panphi->Fill( pphi ) ;
        }

        return StatusCode::SUCCESS;
}


StatusCode tauMonTool::fillPhysicsHistograms(const xAOD::TauJet* tau)
{
	float eta = tau->eta();
	float pt = tau->pt() / GeV;

	m_eta_Tau_Z->Fill(eta);
	m_pTVsEta_Tau_Z->Fill(pt, eta);
	m_pTVsEta_Tau_W->Fill(pt, eta);

	return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------------
// book hist methods
//--------------------------------------------------------------------------------
StatusCode tauMonTool::bookBasicPlots(s_basicPlots& someBasicPlots, MonGroup &aGroup, std::string prefix)
{
	ATH_MSG_INFO("StatusCode tauMonTool::bookBasicPlots, " << prefix);

	someBasicPlots.h_ntausLB = 0;

	CHECK( Book1DHist ( &someBasicPlots.h_ntaus, &aGroup, prefix + "nTauCandidates", "Number of tau candidates;Number of Taus per Event", 31, -0.5, 30.5) );
	CHECK( Book1DHist ( &someBasicPlots.h_eta, &aGroup, prefix + "tauEta", "Eta of tau candidates);Eta);Number of Candidates", 51, -2.55, 2.55) );
	CHECK( Book1DHist ( &someBasicPlots.h_phi, &aGroup, prefix + "tauPhi", "Phi of tau candidates);Phi);Number of Candidates", 65, -3.1415926-0.098174/2., 3.1415926+0.098174/2.) );
	CHECK( Book1DHist ( &someBasicPlots.h_et, &aGroup, prefix + "tauEt", "Et of tau candidates);Transverse Energy (GeV) );Number of Candidates", 60, 0.0, 300.0) );
	CHECK( Book1DHist ( &someBasicPlots.h_charge, &aGroup, prefix + "tauCharge", "Charge of tau candidates);Charge);Number of Candidates", 11, -5.5, 5.5) );
	CHECK( Book1DHist ( &someBasicPlots.h_numTracks, &aGroup, prefix + "tauNumTracks", "Number of Tracks for tau candidates);Number of Tracks);Number of Candidates", 21, -0.5, 20.5) );
	CHECK( Book1DHist ( &someBasicPlots.h_nclst, &aGroup, prefix + "nCluster","Number of CaloTopoClusters", 40, 0, 40 ) );
	CHECK( Book1DHist ( &someBasicPlots.h_nHighPTtaus, &aGroup, prefix + "nHighPtTauCandidates", "Number of High pT tau candidates;Number of Taus per Event", 21, -0.5, 20.5) );
	CHECK( Book2DHist ( &someBasicPlots.h_EtVsEta, &aGroup, prefix + "tauEtVsEta", "Tau Et vs. Eta);Eta);Transverse Energy (GeV) );Number of Candidates",  51, -2.55, 2.55, 100, 0, 200) );
	CHECK( Book2DHist ( &someBasicPlots.h_EtVsPhi, &aGroup, prefix + "tauEtVsPhi", "Tau Et vs. Phi);Phi);Transverse Energy (GeV)", 65, PHIMIN+PHIMIN/64., PHIMAX+PHIMAX/64., 100, 0, 300) );
	CHECK( Book2DHist ( &someBasicPlots.h_PhiVsEta, &aGroup, prefix + "tauPhiVsEta", "Tau Phi vs. Eta;Eta);Phi);Number of Candidates", 51, -2.55, 2.55, 65, PHIMIN+PHIMIN/64., PHIMAX+PHIMAX/64.) );
	CHECK( Book2DHist ( &someBasicPlots.h_PhiVsEta_et15, &aGroup, prefix + "tauPhiVsEta_et15", "Tau Phi vs. Eta (Et>15GeV); Eta);Phi);Number of Candidates", 51, -2.55, 2.55, 65, PHIMIN+PHIMIN/64., PHIMAX+PHIMAX/64.) );

	CHECK( Book2DHist ( &someBasicPlots.h_PhiVsEta_et15_BDTLoose, &aGroup, prefix + "tauPhiVsEta_et15_BDTLoose", "Tau Phi vs. Eta (BDTLoose,Et>15GeV); Eta);Phi);Number of Candidates", 51, -2.55, 2.55, 65, PHIMIN+PHIMIN/64., PHIMAX+PHIMAX/64.) );

	CHECK( Book2DHist ( &someBasicPlots.h_Eta_vs_LB, &aGroup, prefix + "tauEtaVsLB", "Tau Eta vs Lumiblock);Eta);Lumiblock", 51, -2.55, 2.55, m_maxNLB/10+1, -5.0, (double)m_maxNLB+5.0 ) );
	CHECK( Book2DHist ( &someBasicPlots.h_Phi_vs_LB, &aGroup, prefix + "tauPhiVsLB", "Tau Phi vs Lumiblock);Phi);Lumiblock", 65, PHIMIN+PHIMIN/64, PHIMAX+PHIMAX/64, m_maxNLB/10+1, -5.0, (double)m_maxNLB+5.0 ) );
	CHECK( Book1DHist ( &someBasicPlots.h_ntaus_vs_LB, &aGroup, prefix + "nTauPerLB", "Total number of tau candidates per LB);Luminosity Block);Number of Candidates", m_maxNLB, 0, m_maxNLB) );

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::bookHistos(s_mainFolder& mainFolder, std::string folderName, Interval_t interval)
{
	ATH_MSG_INFO("StatusCode tauMonTool::bookHistos, folderName = " << folderName);

	MonGroup folder(this, folderName, interval);

        if ( ! ( folderName == "Tau/Trigger/TauTrig" || folderName == "Tau" ) )
        {
	  if ( bookKinHistos(mainFolder.kinFolder, folder ) .isFailure() ) 				    ATH_MSG_ERROR("Couldn't book kinematic histograms");
        }

	if ( bookCaloHistos(mainFolder.caloFolder, folderName, interval).isFailure() )	ATH_MSG_ERROR("Couldn't book calorimeter histograms");
	if ( bookTrackHistos(mainFolder.trkFolder, folderName, interval).isFailure() )	ATH_MSG_ERROR("Couldn't book track histograms");
// these folders are assumed not DIRECTLY booked under Tau, there shold be middle layer.
        if ( folderName != "Tau" )
        {
          if ( bookIDHistos(mainFolder.idFolder, folderName, interval).isFailure() )		ATH_MSG_ERROR("Couldn't book identification histograms");
          if ( bookSubStructureHistos(mainFolder.sbstrctFolder, folderName, interval).isFailure() ) ATH_MSG_ERROR("Couldn't book SubStructure histograms");
        }

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::bookKinHistos(s_kinFolder& folder,  MonGroup &aGroup)
{
	std::string prefix = this->fixName(aGroup.system());

	CHECK( Book1DHist ( &folder.h_ntaus, &aGroup, prefix + "_nTauCandidates", "Number of tau candidates;Number of Taus per Event", 31, -0.5, 30.5) );
	CHECK( Book1DHist ( &folder.h_eta, &aGroup, prefix + "_tauEta", "Eta of tau candidates;Eta;Number of Candidates", 51, -2.55, 2.55) );
	CHECK( Book1DHist ( &folder.h_phi, &aGroup, prefix + "_tauPhi", "Phi of tau candidates;Phi;Number of Candidates", 65, -3.1415936-0.098174/2., 3.1415936+0.098174/2.) );
	CHECK( Book1DHist ( &folder.h_et, &aGroup, prefix + "_tauEt", "Et of tau candidates;Transverse Energy (GeV) );Number of Candidates", 60, 0.0, 300.0) );
	CHECK( Book1DHist ( &folder.h_charge, &aGroup, prefix + "_tauCharge", "Charge of tau candidates;Charge;Number of Candidates", 11, -5.5, 5.5) );
	CHECK( Book2DHist ( &folder.h_PhiVsEta, &aGroup, prefix + "_tauPhiVsEta", "Tau Phi vs. Eta;Eta;Phi;Number of Candidates", 51, -2.55, 2.55, 65, PHIMIN+PHIMIN/64., PHIMAX+PHIMAX/64.) );
	CHECK( Book2DHist ( &folder.h_Eta_vs_LB, &aGroup, prefix + "_tauEtaVsLB", "Tau Eta vs Lumiblock;Eta;Lumiblock", 51, -2.55, 2.55, m_maxNLB/10+1, -5.0, (double)m_maxNLB+5.0) );
	CHECK( Book2DHist ( &folder.h_Phi_vs_LB, &aGroup, prefix + "_tauPhiVsLB", "Tau Phi vs Lumiblock;Phi;Lumiblock", 65, PHIMIN+PHIMIN/64, PHIMAX+PHIMAX/64, m_maxNLB/10+1, -0.5, (double)m_maxNLB+0.5) );

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::bookIDHistos(s_idFolder& folder,std::string folderName, Interval_t interval)
{
	folderName = folderName + "/Identification";
	MonGroup aGroup(this, folderName, interval);

	if ( bookBDTLooseHistos(folder.BDTLooseFolder, folderName, interval).isFailure() ) 	ATH_MSG_ERROR("Couldn't book BDTLoose histograms");
	if ( bookBDTMedHistos(folder.BDTMedFolder, folderName, interval).isFailure() )  	ATH_MSG_ERROR("Couldn't book BDTMedium histograms");

	folderName = this->fixName(folderName);

	CHECK( Book1DHist ( &folder.h_tauBDTLoose,          &aGroup, folderName + "_tauBDTLoose",          "Identification Flag: tauBDTLoose",          2, -0.5, 1.5) );
	addBinLabelIDHistos(folder.h_tauBDTLoose);
	CHECK( Book1DHist ( &folder.h_tauBDTMedium,         &aGroup, folderName + "_tauBDTMedium",         "Identification Flag: tauBDTMedium",         2, -0.5, 1.5) );
	addBinLabelIDHistos(folder.h_tauBDTMedium);
	CHECK( Book1DHist ( &folder.h_tauBDTTight,          &aGroup, folderName + "_tauBDTTight",          "Identification Flag: tauBDTTight",          2, -0.5, 1.5) );
	addBinLabelIDHistos(folder.h_tauBDTTight);
	CHECK( Book1DHist ( &folder.h_tauLlhLoose,          &aGroup, folderName + "_tauLlhLoose",          "Identification Flag: tauLlhLoose",          2, -0.5, 1.5) );
	addBinLabelIDHistos(folder.h_tauLlhLoose);
	CHECK( Book1DHist ( &folder.h_tauLlhMedium,         &aGroup, folderName + "_tauLlhMedium",         "Identification Flag: tauLlhMedium",         2, -0.5, 1.5) );
	addBinLabelIDHistos(folder.h_tauLlhMedium);
	CHECK( Book1DHist ( &folder.h_tauLlhTight,          &aGroup, folderName + "_tauLlhTight",          "Identification Flag: tauLlhTight",          2, -0.5, 1.5) );
	addBinLabelIDHistos(folder.h_tauLlhTight);
	CHECK( Book1DHist ( &folder.h_electronVetoLoose,    &aGroup, folderName + "_electronVetoLoose",    "Loose electron Veto",                       2, -0.5, 1.5) );
	addBinLabelIDHistos(folder.h_electronVetoLoose);
	CHECK( Book1DHist ( &folder.h_electronVetoMedium,   &aGroup, folderName + "_electronVetoMedium",   "Medium electron Veto",                      2, -0.5, 1.5) );
	addBinLabelIDHistos(folder.h_electronVetoMedium);
	CHECK( Book1DHist ( &folder.h_electronVetoTight,    &aGroup, folderName + "_electronVetoTight",    "Tight electron Veto",                       2, -0.5, 1.5) );
	addBinLabelIDHistos(folder.h_electronVetoTight);
	CHECK( Book1DHist ( &folder.h_muonVeto,             &aGroup, folderName + "_muonVeto",             "Muon Veto",                                 2, -0.5, 1.5) );
	addBinLabelIDHistos(folder.h_muonVeto);
	CHECK( Book1DHist ( &folder.h_eleBDTLoose,          &aGroup, folderName + "_eleBDTLoose",          "Loose EleBDT",                                 2, -0.5, 1.5) );
	addBinLabelIDHistos(folder.h_eleBDTLoose);
	CHECK( Book1DHist ( &folder.h_eleBDTMedium,         &aGroup, folderName + "_eleBDTMedium",         "Medium EleBDT",                                2, -0.5, 1.5) );
	addBinLabelIDHistos(folder.h_eleBDTMedium);
	CHECK( Book1DHist ( &folder.h_eleBDTTight,          &aGroup, folderName + "_eleBDTTight",          "Tight EleBDT",                                 2, -0.5, 1.5) );
	addBinLabelIDHistos(folder.h_eleBDTTight);

	CHECK( Book1DHist ( &folder.h_BDTJetScore,         &aGroup, folderName + "_BDTJetScore",         "BDT Score for Jet Rejection;Boosted Decision Tree Score",                             48, -0.1, 1.1) );

	CHECK( Book1DHist ( &folder.h_BDTJetScoreSigTrans, &aGroup, folderName + "_BDTJetScoreSigTrans", "Signal Transformed BDT Score for Jet Rejection;Boosted Decision Tree Score",         48, -0.1, 1.1) );
	CHECK( Book1DHist ( &folder.h_BDTJetScoreBkgTrans, &aGroup, folderName + "_BDTJetScoreBkgTrans", "Background Transformed BDT Score for Electron Rejection;Boosted Decision Tree Score", 48, -0.1, 1.1) );

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::bookTrackHistos(s_trkFolder& folder,std::string folderName, Interval_t interval)
{
	folderName = folderName + "/Track";
	MonGroup aGroup(this, folderName, interval);
	folderName = this->fixName(folderName);

	CHECK( Book1DHist ( &folder.h_leadTrkPt, &aGroup, folderName + "_leadTrkPt","Pt of Leading track;Transverse Momentum (GeV)", 50, 0, 200) );
	CHECK( Book1DHist ( &folder.h_massTrkSys, &aGroup, folderName + "_massTrkSys","Mass of the Track System;Invariant Mass (GeV)", 30, -1.0, 5.0) );
	CHECK( Book1DHist ( &folder.h_trkWidth2, &aGroup, folderName + "_trkWidth2","Weighted Track Width;Momentum-Weighted Width of Track System", 25, 0.0, 0.1) );
	CHECK( Book1DHist ( &folder.h_trFlightPathSig, &aGroup, folderName + "_trFlightPathSig","Track Transverse Flight Path Significance;Transverse Flight Path Significance", 50, -5.0, 5.0) );
	CHECK( Book1DHist ( &folder.h_ipSigLeadTrk, &aGroup, folderName + "_ipSigLeadTrk","Impact Parameter Significance of Leading Track;Transverse Impact Parameter Significance", 50, -5.0, 5.0) );
	CHECK( Book1DHist ( &folder.h_ipZ0SinThetaSigLeadTrk, &aGroup, folderName + "_ipZ0SinThetaSigLeadTrk","Impact Parameter z0 Sine Theta Significance of Leading Track;Z0SinTheta Significance", 50, -10.0, 10.0) );
	CHECK( Book1DHist ( &folder.h_d0, &aGroup, folderName + "_d0","Track d0;Transverse Impact Parameter (mm)", 50, -5.0, 15.0) );
	CHECK( Book1DHist ( &folder.h_z0, &aGroup, folderName + "_z0","Track z0;Longitudinal Impact Parameter (mm)", 50, -35.0, 35.0) );
	CHECK( Book1DHist ( &folder.h_phi, &aGroup, folderName + "_phi","Track Phi;Phi", 65, PHIMIN+PHIMIN/64., PHIMAX+PHIMAX/64.) );
	CHECK( Book1DHist ( &folder.h_eta, &aGroup, folderName + "_eta","Track Eta;Eta", 51, -2.55, 2.55) );
	CHECK( Book1DHist ( &folder.h_pT, &aGroup, folderName + "_pT","Track pT;Transverse Momentum (GeV)", 50, 0.0, 200.0) );
	CHECK( Book1DHist ( &folder.h_nHighPTtaus, &aGroup, folderName + "_nHighPTtaus","Number of High-pT Tau Candidates;Number of Taus per Event", 21, -0.5, 20.5) );
	CHECK( Book1DHist ( &folder.h_numberOfTRTHighThresholdHits, &aGroup, folderName + "_numberOfTRTHighThresholdHits","Number of TRT High Threshold Hits;Number of High Threshold TRT Hits", 15, -0.5, 14.5) );
	CHECK( Book1DHist ( &folder.h_numberOfTRTHits, &aGroup, folderName + "_numberOfTRTHits","Number of TRT Low Threshold Hits;Number of Low Threshold TRT Hits", 101, -0.5, 100.5) );
	CHECK( Book1DHist ( &folder.h_numberOfTRTHighThresholdOutliers, &aGroup, folderName + "_numberOfTRTHighThresholdOutliers","Number of TRT High Threshold Outliers;Number of TRT High Threshold Outliers", 26, -0.5, 25.5) );
	CHECK( Book1DHist ( &folder.h_numberOfTRTOutliers, &aGroup, folderName + "_numberOfTRTOutliers","Number of TRT Low Threshold Outliers;Number of TRT Low Threshold Outliers", 31, -0.5, 30.5) );
	CHECK( Book1DHist ( &folder.h_numberOfSCTHits, &aGroup, folderName + "_numberOfSCTHits","Number of SCT Hits;Number of SCT Hits", 51, -0.5, 50.5) );
	CHECK( Book1DHist ( &folder.h_numberOfPixelHits, &aGroup, folderName + "_numberOfPixelHits","Number of Pixel Hits;Number of Pixel Hits", 26, -0.5, 25.5) );
	CHECK( Book2DHist ( &folder.h_z0_vs_LB, &aGroup, folderName + "_z0VsLB", "Track z0 vs Lumiblock;Longitudinal Impact Parameter (mm) );Lumiblock" , 50, -30.0, 30.0 , m_maxNLB/10+1, -5.0, (double)m_maxNLB+5.0) );
	CHECK( Book1DHist ( &folder.h_etOverPtLeadTrack, &aGroup, folderName + "_etOverPtLeadTrack","Et over Pt of lead track of tau candidates;Et/Pt;Number of Candidates", 50, 0.0, 12.0) );

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::bookCaloHistos(s_caloFolder& folder,std::string folderName, Interval_t interval)
{
	folderName = folderName + "/Calo";
	MonGroup aGroup(this, folderName, interval);
	folderName = this->fixName(folderName);

	CHECK( Book1DHist ( &folder.h_eta, &aGroup, folderName + "_eta","Calorimeter eta of tau candidates;Eta;Number of Candidates", 51, -2.55, 2.55) );
	CHECK( Book1DHist ( &folder.h_phi, &aGroup, folderName + "_phi","Calorimeter phi of tau candidates;Phi;Number of Candidates", 65, PHIMIN+PHIMIN/64., PHIMAX+PHIMAX/64.) );
	CHECK( Book1DHist ( &folder.h_etEMCalib, &aGroup, folderName + "_etEMCalib","Calibrated EM ET of tau candidates;Calibrated EM ET (in GeV) );Number of Candidates", 50, 0.0, 150.0) );
	CHECK( Book1DHist ( &folder.h_etHadCalib, &aGroup, folderName + "_etHadCalib","Calibrated Had ET of tau candidates;Calibrated Hadronic ET (in GeV) );Number of Candidates", 50, 0.0, 150.0) );
	CHECK( Book1DHist ( &folder.h_etHadAtEMScale, &aGroup, folderName + "_etHadAtEMScale","Hadronic Energy at the EM Scale;Had Et (GeV) );Number of Candidates", 50, 0.0, 200.0) );
	CHECK( Book1DHist ( &folder.h_etEMAtEMScale, &aGroup, folderName + "_etEMAtEMScale","EM energy at the EM scale;EM Et (GeV) );Number of Candidates", 50, 0.0, 200.0) );
	CHECK( Book1DHist ( &folder.h_EMRadius, &aGroup, folderName + "_EMRadius","Uncalibrated EM Radius;EM Radius;Number of Candidates", 50, 0.0, 1.2 ) );
	CHECK( Book1DHist ( &folder.h_centFrac, &aGroup, folderName + "_centralityFraction","Centrality fraction of tau candidates;Centrality Fraction;Number of Candidates", 51, 0.0, 1.02) );
	CHECK( Book1DHist ( &folder.h_hadRadius, &aGroup, folderName + "_hadRadius","Hadronic Radius of tau candidates;Hadronic Radius;Number of Candidates", 50, 0.0, 1.0) );
	CHECK( Book1DHist ( &folder.h_isolFrac, &aGroup, folderName + "_isolFrac","Isolation Fraction;Et Isolation Fraction;Number of Candidates", 51, 0.0, 1.02) );
	CHECK( Book1DHist ( &folder.h_stripWidth2, &aGroup, folderName + "_stripWidth2","Strip Width of tau candidates;Strip Width;Number of Candidates", 50, -0.1, 0.12 ) );
	CHECK( Book1DHist ( &folder.h_nStrip, &aGroup, folderName + "_nStrip","Number of strip cells of tau candidates;Number of Strip Cells;Number of Candidates", 56, -0.5, 55.5) );
	CHECK( Book1DHist ( &folder.h_trkAvgDist, &aGroup, folderName + "_trkAvgDist","Average Track Distance from Calorimeter Seed;Distance (mm) );Number of Candidates", 50, -1.0, 1.0) );
	CHECK( Book1DHist ( &folder.h_dRmax, &aGroup, folderName + "_dRmax", "Maximum delta R;delta R;Number of Candidates", 42, 0.0, 0.21) );
	CHECK( Book2DHist ( &folder.h_centFrac_vs_LB, &aGroup, folderName + "_centFracVsLB", "Centrality Fraction vs Lumiblock;Centrality Fraction;Lumiblock", 51, 0.0, 1.02, m_maxNLB/10+1, -5.0, (double)m_maxNLB+5.0 ) );
	CHECK( Book2DHist ( &folder.h_isolFrac_vs_LB, &aGroup, folderName + "_isolFracVsLB", "Isolation Fraction vs Lumiblock;Isolation Fraction;Lumiblock", 51, 0.0, 1.02, m_maxNLB/10+1, -5.0, (double)m_maxNLB+5.0 ) );

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::bookBDTLooseHistos(s_BDTFolder& folder, std::string folderName, Interval_t interval)
{
	folderName = folderName + "/BDTLoose";
	MonGroup aGroup(this,folderName,interval);
	folderName = this->fixName(folderName);

	CHECK( Book1DHist ( &folder.h_et, &aGroup, folderName + "_et", "Et of Tau Candidate;Et (GeV) );Number of Candidates", 50, 0.0, 250.0) );
	CHECK( Book1DHist ( &folder.h_eta, &aGroup, folderName + "_eta", "Eta of Tau Candidate (Et>15GeV);Eta;Number of Candidates", 51, -2.55, 2.55) );
	CHECK( Book1DHist ( &folder.h_phi, &aGroup, folderName + "_phi", "Phi of Tau Candidate (Et>15GeV);Phi;Number of Candidates", 65, PHIMIN+PHIMIN/64, PHIMAX+PHIMAX/64) );
	CHECK( Book1DHist ( &folder.h_nTracks, &aGroup, folderName + "_numTracks", "Number of Tracks (Et>15GeV);Number of Tracks;Number of Candidates", 21, -0.5, 20.5) );

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::bookBDTMedHistos(s_BDTFolder& folder, std::string folderName, Interval_t interval)
{
	folderName = folderName + "/BDTMedium";
	MonGroup aGroup(this,folderName,interval);
	folderName = this->fixName(folderName);

	CHECK( Book1DHist ( &folder.h_et, &aGroup, folderName + "_et", "Et of Tau Candidate;Et (GeV) );Number of Candidates", 50, 0.0, 250.0) );
	CHECK( Book1DHist ( &folder.h_eta, &aGroup, folderName + "_eta", "Eta of Tau Candidate;Eta;Number of Candidates", 51, -2.55, 2.55) );
	CHECK( Book1DHist ( &folder.h_phi, &aGroup, folderName + "_phi", "Phi of Tau Candidate;Phi;Number of Candidates", 65, PHIMIN+PHIMIN/64, PHIMAX+PHIMAX/64) );
	CHECK( Book1DHist ( &folder.h_nTracks, &aGroup, folderName + "_numTracks", "Number of Tracks;Number of Tracks;Number of Candidates", 21,-0.5, 20.5) );

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::bookPhysicsHistograms()
{
	ATH_MSG_INFO("StatusCode tauMonTool::bookPhysicsHistograms");

	//**********
	// Z details
	//**********
	MonGroup tau_Z (this, "Tau/Physics/Z", run);
	CHECK( Book1DHist (&m_eta_Tau_Z, &tau_Z, "tau_eta", "Eta of Tau Candidates;Eta", 51, -2.55, 2.55) );
	CHECK( Book2DHist (&m_pTVsEta_Tau_Z, &tau_Z, "tau_pTVsEta", "Pt vs. Eta of Tau Candidates;Eta;Pt (GeV)", 51, -2.55, 2.55, 100, 0.0, 150.0) );
	CHECK( Book2DHist (&m_pTVsEta_Lepton_Z, &tau_Z, "lepton_pTVsEta", "Pt vs. Eta of Lepton Candidates;Eta;Pt (GeV)", 51, -2.55, 2.55, 100, 0.0, 150.0) );

	//**********
	// W details
	//**********
	MonGroup tau_W (this, "Tau/Physics/W", run);
	CHECK( Book2DHist (&m_pTVsEta_Tau_W, &tau_W, "tau_pTVsEta", "Pt vs. Eta of Tau Candidates;Eta;Pt (GeV)", 51, -2.55, 2.55, 100, 0.0, 150.0) );

	return StatusCode::SUCCESS;
}


StatusCode tauMonTool::bookSubStructureHistos( s_sbstrctFolder& folder,std::string folderName, Interval_t interval)
{

	folderName = folderName + "/SubStructure";
	MonGroup aGroup(this, folderName, interval);
	folderName = this->fixName(folderName);

	CHECK( Book1DHist ( &folder.h_nShot, &aGroup, folderName + "_nShot","number of shots ; shot number ", 20, 0, 20 ) );
	CHECK( Book1DHist ( &folder.h_InvMass, &aGroup, folderName + "_InvMassEffClusters","Invariant mass of effective clusters in shot; invariant mass (GeV)", 30, 0, 15. ) );
	CHECK( Book1DHist ( &folder.h_L2EOverAllClusterE, &aGroup, folderName + "_EfracL2EffCluster","Energy fraction of leading two effective clusters in shot; energy fraction", 15, 0, 1.5 ) );
	CHECK( Book1DHist ( &folder.h_PSSFrac, &aGroup, folderName + "_PSSFracEffCluster","Energy fraction for PreSampling and sampling layers in effective clusters in shot; Sampling energy fraction", 10, 0, 1.0 ) );
	CHECK( Book1DHist ( &folder.h_stpt3, &aGroup, folderName + "_shots_pt3","weighted cell pt in 3x3 window in shots; pt3 (GeV) ", 24, 0, 12 ) );

	CHECK( Book1DHist ( &folder.h_RptApprox, &aGroup, folderName + "_ptRatioApprox","Ratio of pt to shot total energy for associated tracks; track pt ratio", 20, 0, 2.0 ) );
	CHECK( Book1DHist ( &folder.h_EMFracTrk, &aGroup, folderName + "_EMFracTrk","Ratio of pt to shot electromagnetic energy for associated tracks; track pt ratio in EM", 15, 0, 1.5 ) );
	CHECK( Book1DHist ( &folder.h_IsoCorr, &aGroup, folderName + "_EisoEffCluster","Isolation Energy after correction in effective clusters ; isolation energy (GeV)", 10, 0, 50.0 ) );

	CHECK( Book1DHist ( &folder.h_nNeutPFO, &aGroup, folderName + "_NumNeutPFO","Number of neutral ParticleFlow objects ; PFO number", 20, 0, 20 ) );
	CHECK( Book1DHist ( &folder.h_pi0bdt, &aGroup, folderName + "_BDTscoreAsP0","BDT score indentifying pion zero ; BDT score", 15, 0, 1.2 ) );
	CHECK( Book1DHist ( &folder.h_panmode, &aGroup, folderName + "_PanMode","tau decay mode from PanTau ; mode ", 8, 0, 8 ) );
	CHECK( Book1DHist ( &folder.h_panpt, &aGroup, folderName + "_PanPt","tau Pt from PanTau ; substructure pt (GeV)", 20, 0, 200 ) );
	CHECK( Book1DHist ( &folder.h_paneta, &aGroup, folderName + "_PanEta","tau Eta from PanTau ; substructure Eta", 16, -3.2, 3.2 ) );
	CHECK( Book1DHist ( &folder.h_panphi, &aGroup, folderName + "_PanPhi","tau Phi from PanTau ; substructure Phi", 16, -3.2, 3.2 ) );

        return StatusCode::SUCCESS;
}


//--------------------------------------------------------------------------------
// book and register a 1D histogram
//--------------------------------------------------------------------------------
StatusCode tauMonTool::Book1DHist (TH1 ** hist, MonGroup * monName, std::string  histName, std::string  histTitle, int NBins, double lowBin, double highBin)
{
	*hist = new TH1F( histName.c_str(), histTitle.c_str(), NBins, lowBin, highBin);
	StatusCode sc = monName->regHist( *hist);
	if ( sc.isFailure() )
	{
		ATH_MSG_FATAL("Failed to register " << histName);
		return sc;
	}
	return sc;
}


//--------------------------------------------------------------------------------
// book and register a 2D histogram
//--------------------------------------------------------------------------------
StatusCode tauMonTool::Book2DHist (TH2 ** hist, MonGroup * monName, std::string  histName, std::string  histTitle, int NXBins, double lowXBin, double highXBin, int NYBins, double lowYBin, double highYBin)
{
	*hist = new TH2F( histName.c_str(), histTitle.c_str(), NXBins, lowXBin, highXBin, NYBins, lowYBin, highYBin);
	StatusCode sc = monName->regHist( *hist);
	if ( sc.isFailure() )
	{
		ATH_MSG_FATAL("Failed to register " << histName);
		return sc;
	}
	return sc;
}

//--------------------------------------------------------------------------------
// helpers
//--------------------------------------------------------------------------------
std::string tauMonTool::fixName(std::string name)
{
	std::string::size_type start = 0;
	while ( (start = name.find('/')) != std::string::npos) name.replace(start,1,"_");
	return name;
}

void tauMonTool::addBinLabelIDHistos(TH1* h)
{
	h->GetXaxis()->SetBinLabel(1, "False");
	h->GetXaxis()->SetBinLabel(2, "True");
}


