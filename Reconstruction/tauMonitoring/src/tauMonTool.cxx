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
#include "xAODTracking/VertexContainer.h"
#include "TrigDecisionTool/TrigDecisionTool.h" 
#include <sstream>  // for  std::ostringstream
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
                m_maxNLB(1205.),
                m_NevtLB( 0 ),
                m_extendLB( false ),
                m_reCurrent(true) 
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
StatusCode tauMonTool::bookHists()
//StatusCode tauMonTool::bookHistograms()
{
  m_NevtLB = 0 ;

  return StatusCode::SUCCESS ;
}

//StatusCode tauMonTool::bookHists()
StatusCode tauMonTool::bookHistograms()
{
	ATH_MSG_INFO("StatusCode tauMonTool::bookHistograms, doTrigger = " << m_doTrigger);

	//--------------------
	// Top level folder
	//--------------------
	std::string folderName = "Tau";
	MonGroup mainFolder (this, folderName, run);
        MonGroup lowStatFds( this, folderName + "LS" , lowStat ) ;

        if ( m_doLS && ( newRunFlag() || newLowStatIntervalFlag() ) )
        { 
          if ( bookBasicPlots(m_basic_LS, lowStatFds, "LS_" ).isFailure() )  
              ATH_MSG_ERROR("Couldn't book Low Stat basic histograms");

          if ( bookHistos( m_tauLS, folderName + "LS" , lowStat ).isFailure() )		
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
                m_Trigged = m_trigDecTool.retrieve();
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
                        CHECK( Book2DHist ( &m_triggers_vs_LB, &tau_Trigger, "TriggerChains_LB", "Triggered chain vs Lumiblock; trigger;Lumiblock", 5, 0.5, 5.5, (int)m_maxNLB/10 + 1, -5., m_maxNLB ) );
			m_triggers_vs_LB->GetXaxis()->SetBinLabel(1,"EM");
			m_triggers_vs_LB->GetXaxis()->SetBinLabel(2,"Jet");
			m_triggers_vs_LB->GetXaxis()->SetBinLabel(3,"Taus");
			m_triggers_vs_LB->GetXaxis()->SetBinLabel(4,"MBTS");
			m_triggers_vs_LB->GetXaxis()->SetBinLabel(5,"Unchecked");


                        if ( bookBasicPlots(m_tauBasePlots, tau_Trigger_Tau ,"tauTriggered_" ).isFailure() ) ATH_MSG_ERROR("Couldn't book Tau trigger histograms");
                        if ( bookHistos( m_tauPlots, "Tau/Trigger/TauTrig", run).isFailure()) 
          ATH_MSG_ERROR("Couldn't book Tau trigger histograms");

                        if ( bookBasicPlots(m_jetPlots,  tau_Trigger_Jet ,"jetTriggered_" ).isFailure() )
          ATH_MSG_ERROR("Couldn't book jetTrigger histograms");
			if ( bookBasicPlots(m_emPlots,   tau_Trigger_Em  ,"emTriggered_"  ).isFailure() )
          ATH_MSG_ERROR("Couldn't book emTrigger histograms");
                        if ( bookBasicPlots(m_mbtsPlots, tau_Trigger_Mbts,"mbtsTriggered_").isFailure() )
          ATH_MSG_ERROR("Couldn't book mbtsTrigger histograms");

		} else {
			ATH_MSG_ERROR("couldn't retrieve trigger info");
			return StatusCode::FAILURE;
		}
	}

	//------------------------
	// book physics histograms
	//------------------------
//        if (bookPhysicsHistograms().isFailure() ) ATH_MSG_ERROR("Failed to book physics histograms");

	return StatusCode::SUCCESS;
}


//--------------------------------------------------------------------------------
// fill histograms
//--------------------------------------------------------------------------------
StatusCode tauMonTool::fillHistograms()
{
        privtx_z0 = 0 ;
        const xAOD::VertexContainer* vtxCont = 0 ;
        if ( ! ( evtStore()->retrieve( vtxCont, "PrimaryVertices" ) ) . isFailure() ) 
        {
          const xAOD::Vertex *privtx = static_cast< const xAOD::Vertex * >( *(vtxCont->begin()) );
          if (    privtx->vertexType() == xAOD::VxType::PriVtx && privtx->nTrackParticles() >= 2  )
            privtx_z0 = privtx->position().z() ;
        }

        m_NevtLB ++ ;
//        if ( m_NevtLB > 500 && ! newLowStatIntervalFlag()  ) return StatusCode::SUCCESS ;

        // sometime LHC present very loong run 
        if ( m_currentLB >= m_maxNLB && ! m_extendLB )
        {
          m_maxNLB = 2005. ;
          m_extendLB = true ;
        }

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

          if ( m_doLS && m_reCurrent ) 
          {
            fillBasicPlots( m_basic_LS, tau);
          }

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
//             if(fillPhysicsHistograms(tau).isFailure()) ATH_MSG_ERROR("Failed to fill physics histograms");

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
  if( endOfRunFlag() || endOfLumiBlockFlag() ) {
          //fill the vs_LB plots
    // numbers of tau averaged by the total EventNumber in the LumiBloc
    if (m_currentLB <= m_maxNLB) 
    {
      int nbin = ( m_currentLB - m_basicPlots.h_ntaus_vs_LB->GetBinCenter(1) )/10 + 1 ;
      m_basicPlots.h_ntaus_vs_LB->SetBinContent( nbin, m_basicPlots.h_ntausLB/( m_NevtLB + 0.01 ) );
      if (m_doTrigger && m_Trigged ) 
        m_tauBasePlots.h_ntaus_vs_LB->SetBinContent( nbin, m_tauBasePlots.h_ntausLB/( m_NevtLB + 0.01 ) );

      if ( m_doLS  ) 
      {
         nbin = m_currentLB - m_basic_LS.h_ntaus_vs_LB->GetBinCenter(1) + 1 ;
         m_basic_LS.h_ntaus_vs_LB->SetBinContent( nbin, m_basic_LS.h_ntausLB/( m_NevtLB + 0.01 ) );
      }

    }
	  //reset counters
    m_NevtLB = 0 ;
    m_basicPlots.h_ntausLB = 0;
    if ( m_doLS )  m_basic_LS.h_ntausLB = 0;
    if (m_doTrigger && m_Trigged  ) m_tauBasePlots.h_ntausLB = 0;

  }

  return StatusCode::SUCCESS;
}


//--------------------------------------------------------------------------------
// fill methods
//--------------------------------------------------------------------------------
void tauMonTool::fillBasicPlots(s_basicPlots& someBasicPlots, const xAOD::TauJet* tau)
{
        if ( tau == NULL ) return ;

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

        someBasicPlots.h_pT_TES_MVA ->Fill( tau->ptFinalCalib() /GeV ) ;
        if ( tau->isAvailable<float>( "pt_combined" ) )
          someBasicPlots.h_pT_combined ->Fill( tau->auxdata<float>( "pt_combined" )/GeV ) ;

	someBasicPlots.h_EtVsEta  ->Fill( eta, et);
	someBasicPlots.h_EtVsPhi  ->Fill( phi, et);
	someBasicPlots.h_PhiVsEta ->Fill( eta, phi);

	if ( et > 15.0 ) someBasicPlots.h_PhiVsEta_et15 ->Fill( eta, phi);
	if ( et > 15.0 && tau->isTau(xAOD::TauJetParameters::JetBDTSigLoose) ) 
          someBasicPlots.h_PhiVsEta_et15_BDTLoose ->Fill( eta, phi);

        int coreTrk = tau->nTracks( xAOD::TauJetParameters::coreTrack ) ;
        if ( et > 15.0 ) someBasicPlots.h_numCoreTracks_vs_LB->Fill( m_currentLB, coreTrk );

        someBasicPlots.h_Eta_vs_LB->Fill( m_currentLB , eta  );
	someBasicPlots.h_Phi_vs_LB->Fill(  m_currentLB , phi );
	someBasicPlots.h_Et_vs_LB->Fill( m_currentLB, et );
	if(tau->isTau(xAOD::TauJetParameters::JetBDTSigLoose) && et > 15.0  ){
          someBasicPlots.h_Phi_vs_LB_et15_BDTLoose->Fill( m_currentLB, phi );
          someBasicPlots.h_Eta_vs_LB_et15_BDTLoose->Fill( m_currentLB, eta );
        }
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
        mainFolder.idFolder.h_tauBDTLoose         ->Fill(tau->isTau(xAOD::TauJetParameters::JetBDTSigLoose));
        mainFolder.idFolder.h_tauBDTMedium        ->Fill(tau->isTau(xAOD::TauJetParameters::JetBDTSigMedium));
        mainFolder.idFolder.h_tauBDTTight         ->Fill(tau->isTau(xAOD::TauJetParameters::JetBDTSigTight));
        mainFolder.idFolder.h_muonVeto            ->Fill(tau->isTau(xAOD::TauJetParameters::MuonVeto));
        mainFolder.idFolder.h_JetBDTBkgMedium         ->Fill(tau->isTau(xAOD::TauJetParameters::JetBDTBkgMedium));
        mainFolder.idFolder.h_eleBDTMedium        ->Fill(tau->isTau(xAOD::TauJetParameters::EleBDTMedium));
        mainFolder.idFolder.h_eleBDTTight         ->Fill(tau->isTau(xAOD::TauJetParameters::EleBDTTight));
        mainFolder.idFolder.h_BDTJetScore         ->Fill(tau->discriminant(xAOD::TauJetParameters::BDTJetScore));
	mainFolder.idFolder.h_BDTJetScoreSigTrans ->Fill(tau->discriminant(xAOD::TauJetParameters::BDTJetScoreSigTrans));
        float eleBDT = 0.5 ;
        if ( tau->isAvailable<float>( "BDTEleScoreSigTrans" ) ) 
          eleBDT = tau->auxdata<float>( "BDTEleScoreSigTrans" ) ; 
        mainFolder.idFolder.h_BDTEleScoreSigTrans ->Fill( eleBDT );

        if(tau->isTau(xAOD::TauJetParameters::JetBDTSigLoose) && tau->pt() >= 15000.0 )
        {
          if ( fillBDTHistos   ( mainFolder.idFolder.BDTLooseFolder, tau) . isFailure()) 
            ATH_MSG_ERROR("Failed to fill JetBDTLoose histograms");
        }

	if ( fillSubStructureHistos (mainFolder.sbstrctFolder, tau) . isFailure())	ATH_MSG_ERROR("Failed to fill calorimeter histograms");
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
	folder.h_muonVeto            ->Fill(tau->isTau(xAOD::TauJetParameters::MuonVeto));
	folder.h_JetBDTBkgMedium         ->Fill(tau->isTau(xAOD::TauJetParameters::JetBDTBkgMedium));
	folder.h_eleBDTMedium        ->Fill(tau->isTau(xAOD::TauJetParameters::EleBDTMedium));
	folder.h_eleBDTTight         ->Fill(tau->isTau(xAOD::TauJetParameters::EleBDTTight));
	folder.h_BDTJetScore         ->Fill(tau->discriminant(xAOD::TauJetParameters::BDTJetScore));

	folder.h_BDTJetScoreSigTrans ->Fill(tau->discriminant(xAOD::TauJetParameters::BDTJetScoreSigTrans));

        float eleBDT = 0.5 ;
        if ( tau->isAvailable<float>( "BDTEleScoreSigTrans" ) ) 
          eleBDT = tau->auxdata<float>( "BDTEleScoreSigTrans" ) ; 
        folder.h_BDTEleScoreSigTrans ->Fill( eleBDT );

	if(tau->isTau(xAOD::TauJetParameters::JetBDTSigLoose) && tau->pt() >= 15000.0 )
        {
          if ( fillBDTHistos   (folder.BDTLooseFolder, tau) . isFailure()) 
            ATH_MSG_ERROR("Failed to fill JetBDTLoose histograms");
	}

        if ( fillJetsigBDTHistos( folder.tauJetinputs, tau ).isFailure() ) ATH_MSG_ERROR("Failed to fill tau BDT inputs histograms");

        if ( fillEleVetoHistos( folder.EleVetoinputs, tau ).isFailure() ) ATH_MSG_ERROR("Failed to fill Electron Veto BDT inputs histograms");

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::fillTrackHistos(s_trkFolder& folder, const xAOD::TauJet* tau)
{
	if ( tau->nTracks() == 0) return StatusCode::SUCCESS;

	folder.h_trkWidth2             ->Fill( tau->detail<float>(xAOD::TauJetParameters::trkWidth2) );
	folder.h_ipZ0SinThetaSigLeadTrk->Fill( tau->detail<float>(xAOD::TauJetParameters::ipZ0SinThetaSigLeadTrk) );
	folder.h_leadTrkPt             ->Fill( tau->detail<float>(xAOD::TauJetParameters::leadTrkPt) / GeV );

	//--------------------
	// track info
	//--------------------

        int ncore = 0 ;
        float maxpt = -9.9, ldeta = 0., ldphi = 0. ;
//        std::vector< ElementLink< xAOD::TauTrackContainer > > & tauTrackCont = tau->allTauTrackLinks() ;
        for( ElementLink< xAOD::TauTrackContainer > link : tau->allTauTrackLinks() )
//        for( const xAOD::TauTrack* tau_trk : (*tauTrackCont) ) 
        {
          if ( ! link.isValid() ) continue ; 
          if ( (*link)->flagWithMask( ( 1<<xAOD::TauJetParameters::TauTrackFlag::coreTrack ) ) ) ncore ++ ;

          folder.h_z0sinThetaTJVA->Fill( (*link)->z0sinThetaTJVA(  *tau ) ) ;
          //       dynamic_cast < const xAOD::IParticle > ( tau->jet() ) 
          folder.h_dRJetSeedAxis ->Fill( (*link)->dRJetSeedAxis( *tau ) ) ;

          // const xAOD::TrackParticle* track = tau->track(0) ;
          const xAOD::TrackParticle* track = (*link)->track() ;
          if ( ! track ) continue ;
          
          if ( (*link)->pt() > maxpt )
          {
            ldeta = (*link)->eta() - tau->etaJetSeed() ;
            ldphi = (*link)->phi() - tau->phiJetSeed() ;
          }

          uint8_t dummy(0);
          if (track->summaryValue(dummy, xAOD::numberOfTRTHighThresholdHits)) 	
            folder.h_numberOfTRTHighThresholdHits	 ->Fill( dummy );
          if (track->summaryValue(dummy, xAOD::numberOfTRTHighThresholdOutliers)) 
            folder.h_numberOfTRTHighThresholdOutliers->Fill( dummy );
          if (track->summaryValue(dummy, xAOD::numberOfTRTHits))
            folder.h_numberOfTRTHits                 ->Fill( dummy );
          if (track->summaryValue(dummy, xAOD::numberOfTRTOutliers)) 
            folder.h_numberOfTRTOutliers             ->Fill( dummy );

          if (track->summaryValue(dummy, xAOD::numberOfSCTHits))
            folder.h_numberOfSCTHits                 ->Fill( dummy );
          if (track->summaryValue(dummy, xAOD::numberOfSCTSharedHits )) 
            folder.h_numberOfSCTSharedHits        ->Fill( dummy );

          if (track->summaryValue(dummy, xAOD::numberOfPixelHits)) 	
            folder.h_numberOfPixelHits               ->Fill( dummy );
          if (track->summaryValue(dummy, xAOD::numberOfPixelSharedHits )) 
            folder.h_numberOfPixelSharedHits        ->Fill( dummy );
          if (track->summaryValue(dummy, xAOD::numberOfInnermostPixelLayerHits )) 
            folder.h_numberOfInnermostPixelLayerHits        ->Fill( dummy );

          float ePht = 0. ;
          if (track->summaryValue( ePht, xAOD::eProbabilityHT )) 
            folder.h_eProbabilityHT                  ->Fill( ePht ) ;

          // be careful not fall apart from possible evolusion in TauTracks
          float trkd0 = track->d0() ;
          float trkpt = track->pt() ;
          folder.h_rConv->Fill(  std::sqrt(std::fabs(trkd0)*trkpt/(.3 /*0.15*2.*/)) ) ;
          folder.h_rConvII->Fill(  
             std::sqrt( std::fabs( trkd0 * trkpt ) / (0.3)  )*( trkd0/fabs(trkd0))*track->charge() ) ;

          folder.h_d0                              ->Fill( track->d0() );
          float z0_wrt_privtx = ( track->z0() + track->vz() - privtx_z0 )*sin( track->theta() ) ;
          folder.h_z0sinThetaPriVtx                              ->Fill( z0_wrt_privtx );

          folder.h_pT                              ->Fill( trkpt / GeV);
          folder.h_phi                             ->Fill( track->phi() );
          folder.h_eta                             ->Fill( track->eta());
          folder.h_z0_vs_LB                        ->Fill( m_currentLB, z0_wrt_privtx );
        }


        folder.h_leadTrackDeltaEta->Fill( ldeta ) ;
        if ( ldphi < -3.14159265 ) ldphi += 6.28319 ;
        if ( ldphi > 3.14159265 ) ldphi -= 6.28319 ;
        folder.h_leadTrackDeltaPhi->Fill( ldphi ) ;
        return StatusCode::SUCCESS;
}

StatusCode tauMonTool::fillCaloHistos(s_caloFolder& folder, const xAOD::TauJet* tau)
{
	folder.h_eta                   ->Fill( tau->etaJetSeed() );
	folder.h_phi                   ->Fill( tau->phiJetSeed() );
        folder.h_jetSeedPt             ->Fill( tau->ptJetSeed() /GeV ) ;

        bool test = false ; float dummy = -1.0 ;
        test = tau->detail( xAOD::TauJetParameters::Detail::etEMAtEMScale , dummy ) ;
        if ( test ) folder.h_etEMAtEMScale         ->Fill( dummy / GeV);
        test = tau->detail( xAOD::TauJetParameters::Detail::etHadAtEMScale , dummy ) ;
        if ( test ) folder.h_etHadAtEMScale        ->Fill( dummy / GeV);
        test = tau->detail( xAOD::TauJetParameters::Detail::EMRadius , dummy ) ;
        if ( test ) folder.h_EMRadius              ->Fill( dummy );
        test = tau->detail( xAOD::TauJetParameters::Detail::hadRadius , dummy ) ;
        if ( test ) folder.h_hadRadius             ->Fill( dummy );
        test = tau->detail( xAOD::TauJetParameters::Detail::stripWidth2 , dummy ) ;
        if ( test ) folder.h_stripWidth2           ->Fill( dummy );
        test = tau->detail( xAOD::TauJetParameters::Detail::centFrac , dummy ) ;
        if ( test ) 
        {
	  folder.h_CentFrac_vs_LB        ->Fill( dummy , m_currentLB);
        }
        test = tau->detail( xAOD::TauJetParameters::Detail::isolFrac , dummy ) ;
	if ( test ) 
        {
          folder.h_isolFrac              ->Fill( dummy );
	  folder.h_isolFrac_vs_LB        ->Fill( dummy, m_currentLB);
        }
        int strp = 0 ;
        test = tau->detail( xAOD::TauJetParameters::Detail::nStrip , strp ) ;
        if ( test )   folder.h_nStrip                ->Fill( strp ); 


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

        int numClusters = (int) tau->detail<int>(xAOD::TauJetParameters::numTopoClusters) ;
        folder.h_nclst    ->Fill( numClusters ) ;

        int panmode = -1 ;
        if ( tau->panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayMode, panmode ) )
          folder.h_panmode->Fill( panmode ) ;

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::fillJetsigBDTHistos(  s_tauJetBDT & folder, const xAOD::TauJet* tau )
{

        folder.h_ptIntermediateAxis->Fill( tau->ptIntermediateAxis() / GeV ) ;
        bool test = false ; float dummy = -1.0 ;
        test = tau->detail( xAOD::TauJetParameters::Detail::SumPtTrkFrac , dummy ) ;
        if ( test ) folder.h_SumPtTrkFrac->Fill(  dummy  ) ;
        test = tau->detail( xAOD::TauJetParameters::Detail::etOverPtLeadTrk , dummy ) ;
        if ( test ) folder.h_etOverPtLeadTrk->Fill(  dummy  ) ;
        test = tau->detail( xAOD::TauJetParameters::Detail::ipSigLeadTrk , dummy ) ;
        if ( test ) folder.h_absipSigLeadTrk->Fill(  std::fabs( dummy ) ) ;
        test = tau->detail( xAOD::TauJetParameters::Detail::trFlightPathSig , dummy ) ;
        if ( test ) folder.h_trFlightPathSig->Fill(  dummy  ) ;
        test = tau->detail( xAOD::TauJetParameters::Detail::massTrkSys , dummy ) ;
        if ( test ) folder.h_massTrkSys->Fill(  dummy/GeV   ) ;
        test = tau->detail( xAOD::TauJetParameters::Detail::EMPOverTrkSysP , dummy ) ;
        if ( test ) folder.h_EMPOverTrkSysP->Fill(  dummy  ) ;
        test = tau->detail( xAOD::TauJetParameters::Detail::mEflowApprox , dummy ) ;
        if ( test ) folder.h_mEflowApprox->Fill(  dummy/GeV  ) ;
        test = tau->detail( xAOD::TauJetParameters::Detail::ptRatioEflowApprox, dummy ) ;
        if ( test )  folder.h_ptRatioEflowApprox->Fill( dummy ) ;
        test = tau->detail( xAOD::TauJetParameters::Detail::trkAvgDist , dummy ) ;
        if ( test ) folder.h_trkAvgDist            ->Fill( dummy  );
        test = tau->detail( xAOD::TauJetParameters::Detail::dRmax  ,dummy ) ;
        if ( test ) folder.h_dRmax                 ->Fill( dummy );
        test = tau->detail( xAOD::TauJetParameters::Detail::centFrac , dummy ) ;
        if ( test ) folder.h_CentFrac->Fill( dummy ) ;

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::fillEleVetoHistos(  s_EleVeto & folder, const xAOD::TauJet* tau )
{

        if ( tau->isAvailable<float>( "etHotShotWinOverPtLeadTrk" ) )
          folder.h_etHotShotWinOverPtLeadTrk->Fill( tau->auxdata<float>( "etHotShotWinOverPtLeadTrk" ) ) ;
        if ( tau->isAvailable<float>( "EMFracFixed" ) ) 
          folder.h_EMFracFixed->Fill( tau->auxdata<float>( "EMFracFixed" ) ) ;
        if ( tau->isAvailable<float>( "hadLeakFracFixed" )  )
          folder.h_hadLeakFracFixed->Fill( tau->auxdata<float>( "hadLeakFracFixed" ) ) ;

        float dummy = 0 ;
        if ( tau->detail( xAOD::TauJetParameters::Detail::PSSFraction, dummy ) )
          folder.h_PSSFrac->Fill( dummy ) ;

	return StatusCode::SUCCESS;
}


StatusCode tauMonTool::fillSubStructureHistos( s_sbstrctFolder& folder, const xAOD::TauJet* tau)
{

   //  CaloCluster variabls
        float dummy = 0 ;
        if ( tau->detail( xAOD::TauJetParameters::Detail::effTopoInvMass, dummy ) ) 
          folder.h_InvMass->Fill( dummy / GeV ) ;
        if ( tau->detail( xAOD::TauJetParameters::Detail::lead2ClusterEOverAllClusterE, dummy ) ) 
          folder.h_L2EOverAllClusterE->Fill( dummy ) ;
        if ( tau->detail( xAOD::TauJetParameters::Detail::caloIsoCorrected, dummy ) ) 
          folder.h_IsoCorr->Fill( dummy / GeV ) ;

  // Track ( association ) variabls
        if ( tau->detail( xAOD::TauJetParameters::Detail::ChPiEMEOverCaloEME, dummy ) ) 
          folder.h_EMFracTrk->Fill( dummy ) ;

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
        if (    tau->isTau(xAOD::TauJetParameters::JetBDTSigMedium ) 
             && ( tau->trackFilterProngs() == 1 || tau->trackFilterProngs() == 3 ) 
           )  
        {
          int panmode = -1 ;
          if ( tau->panTauDetail(xAOD::TauJetParameters::PanTauDetails::PanTau_DecayMode, panmode ) )
            folder.h_panmode->Fill( panmode ) ;
        }

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
StatusCode tauMonTool::bookBasicPlots(s_basicPlots& someBasicPlots, MonGroup &aGroup, std::string prefix )
{
	ATH_MSG_INFO("StatusCode tauMonTool::bookBasicPlots, " << prefix  );

        int LBintervalStart = -5. ;

	someBasicPlots.h_ntausLB = 0;

	CHECK( Book1DHist ( &someBasicPlots.h_ntaus, &aGroup, prefix + "nTauCandidates", "Number of tau candidates;Number of Taus per Event", 41, -0.5, 40.5) );
	CHECK( Book1DHist ( &someBasicPlots.h_eta, &aGroup, prefix + "tauEta", "Eta of tau candidates);Eta);Number of Candidates", 56, -2.8, 2.8 ) );
	CHECK( Book1DHist ( &someBasicPlots.h_phi, &aGroup, prefix + "tauPhi", "Phi of tau candidates);Phi);Number of Candidates", 65, -3.1415926-0.098174/2., 3.1415926+0.098174/2.) );
	CHECK( Book1DHist ( &someBasicPlots.h_et, &aGroup, prefix + "tauEt", "Et of tau candidates);Transverse Energy (GeV) );Number of Candidates", 60, 0.0, 300.0) );
	CHECK( Book1DHist ( &someBasicPlots.h_charge, &aGroup, prefix + "tauCharge", "Charge of tau candidates);Charge);Number of Candidates", 11, -5.5, 5.5) );
	CHECK( Book1DHist ( &someBasicPlots.h_numTracks, &aGroup, prefix + "NumTracks", "Number of Tracks for tau candidates);Number of Tracks);Number of Candidates", 21, -0.5, 20.5) );
	CHECK( Book1DHist ( &someBasicPlots.h_nclst, &aGroup, prefix + "nCluster","Number of CaloTopoClusters", 40, 0, 40 ) );
	CHECK( Book1DHist ( &someBasicPlots.h_nHighPTtaus, &aGroup, prefix + "nHighPtTauCandidates", "Number of High pT tau candidates;Number of Taus per Event", 16, -0.5, 15.5) );

	CHECK( Book1DHist ( &someBasicPlots.h_pT_TES_MVA, &aGroup, prefix + "PtTESMVA", "TES pT (MVA) of tau candidates);Transverse Energy (GeV) );Number of Candidates", 60, 0.0, 300.0) );

	CHECK( Book1DHist ( &someBasicPlots.h_pT_combined, &aGroup, prefix + "Ptcombined", "combined pT of tau candidates);Transverse Energy (GeV) );Number of Candidates", 60, 0.0, 300.0) );

	CHECK( Book2DHist ( &someBasicPlots.h_EtVsEta, &aGroup, prefix + "tauEtVsEta", "Tau Et vs. Eta);Eta);Transverse Energy (GeV) );Number of Candidates",  56, -2.8, 2.8, 100, 0, 200) );
	CHECK( Book2DHist ( &someBasicPlots.h_EtVsPhi, &aGroup, prefix + "tauEtVsPhi", "Tau Et vs. Phi);Phi);Transverse Energy (GeV)", 65, PHIMIN+PHIMIN/64., PHIMAX+PHIMAX/64., 100, 0, 300) );
	CHECK( Book2DHist ( &someBasicPlots.h_PhiVsEta, &aGroup, prefix + "tauPhiVsEta", "Tau Phi vs. Eta;Eta);Phi);Number of Candidates", 56, -2.8, 2.8, 65, PHIMIN+PHIMIN/64., PHIMAX+PHIMAX/64.) );
	CHECK( Book2DHist ( &someBasicPlots.h_PhiVsEta_et15, &aGroup, prefix + "tauPhiVsEtaEt15", "Tau Phi vs. Eta (Et>15GeV); Eta);Phi);Number of Candidates", 56, -2.8, 2.8, 65, PHIMIN+PHIMIN/64., PHIMAX+PHIMAX/64.) );

	CHECK( Book2DHist ( &someBasicPlots.h_PhiVsEta_et15_BDTLoose, &aGroup, prefix + "tauPhiVsEtaEt15BDTLoose", "Tau Phi vs. Eta (BDTLoose,Et>15GeV); Eta);Phi);Number of Candidates", 56, -2.8, 2.8, 65, PHIMIN+PHIMIN/64., PHIMAX+PHIMAX/64.) );

        if ( prefix != "LS_" )
        {
          int nLumiBin = (int)m_maxNLB/10 + 1 ;
          CHECK( Book1DHistVsLB ( &someBasicPlots.h_ntaus_vs_LB, &aGroup, prefix + "nTauPerLB", "Total number of tau candidates per LB);Luminosity Block);Number of Candidates", nLumiBin, LBintervalStart , m_maxNLB ) );
          CHECK( Book2DHistVsLB ( &someBasicPlots.h_Eta_vs_LB, &aGroup, prefix + "tauEtaVsLB", "Tau Eta vs Lumiblock);Lumiblock); Eta",  nLumiBin, LBintervalStart , m_maxNLB, 56, -2.8, 2.8 ) );
          CHECK( Book2DHistVsLB ( &someBasicPlots.h_Phi_vs_LB, &aGroup, prefix + "tauPhiVsLB", "Tau Phi vs Lumiblock);Lumiblock); Phi",  nLumiBin, LBintervalStart, m_maxNLB, 65, PHIMIN+PHIMIN/64, PHIMAX+PHIMAX/64 ) );
          CHECK( Book2DHistVsLB ( &someBasicPlots.h_Et_vs_LB, &aGroup, prefix + "tauEtVsLB", "Tau Et vs Lumiblock);Lumiblock); Et",  nLumiBin, LBintervalStart, m_maxNLB, 60, 0., 300. ) );
          CHECK( Book2DHistVsLB ( &someBasicPlots.h_Phi_vs_LB_et15_BDTLoose, &aGroup, prefix + "tauPhiVsLBet15BDTLoose", "Tau Phi vs Lumiblock, 15GeV BDT Loose; Lumiblock; Phi", nLumiBin, LBintervalStart, m_maxNLB , 65, PHIMIN+PHIMIN/64, PHIMAX+PHIMAX/64 ) );
          CHECK( Book2DHistVsLB ( &someBasicPlots.h_Eta_vs_LB_et15_BDTLoose, &aGroup, prefix + "tauEtaVsLBet15BDTLoose", "Tau Eta vs Lumiblock, 15GeV BDT Loose; Lumiblock; Eta", nLumiBin, LBintervalStart, m_maxNLB , 56, -2.8, 2.8 ) );
          CHECK( Book2DHistVsLB ( &someBasicPlots.h_numCoreTracks_vs_LB, &aGroup, prefix + "NumCoreTrkVsLB", "Tau Core tracks vs Lumiblock); LumiBlock ; NumCoreTrk", nLumiBin, LBintervalStart, m_maxNLB, 41, -0.5, 40.5 ) );
        }

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::bookHistogramsRecurrent( )
{
  StatusCode sc = StatusCode::SUCCESS ;
  m_reCurrent = true ;

  const DataHandle<EventInfo> evtInfo;
  sc = evtStore()->retrieve(evtInfo) ;
  if ( ! sc ) 
  {
    ATH_MSG_ERROR("couldn't retrieve event info");
    m_reCurrent = false ;
    return sc ;
  }

  //  pass this to each lumiBlock and event
  m_currentLB = evtInfo->lumiBlock() ;

// newRun ! book before any event !
  if ( ( newRunFlag() || newLowStatIntervalFlag() ) && m_doLS ) 
  {
    sc = bookPlots_LB_dependent( m_basic_LS, "TauLS", m_currentLB ) ;
  }

  if ( ! sc ) 
  {
    ATH_MSG_ERROR("Couldn't book LumiBlock dependent Low Stat basic histograms");
    m_reCurrent = false ;
  }
  
  return sc ;
}

StatusCode tauMonTool::bookPlots_LB_dependent( s_basicPlots& m_basic_LS, std::string pfx, int start  )
{
  int LBintervalStart = ( start > 20 ?  ( ( start - 1 )/ 20 ) * 20 + 1 : 1 ) ;

  MonGroup lowStatFds( this, pfx , lowStat, ATTRIB_UNMANAGED ) ;

  std::string prefix = "LS_" ;

  CHECK( Book1DHist ( &m_basic_LS.h_ntaus_vs_LB, &lowStatFds, prefix + "nTau_vsLB", "Total number of tau candidates per LB);Luminosity Block);Number of Candidates", 21, LBintervalStart - 0.5 , LBintervalStart + 20.5 ) );

  Histos_vsLB.push_back( MgmtParams<TH1>( m_basic_LS.h_ntaus_vs_LB, lowStatFds ) ) ;

  CHECK( Book2DHist ( &m_basic_LS.h_Eta_vs_LB, &lowStatFds, prefix + "tauEta_vsLB", "Tau Eta vs Lumiblock);Lumiblock ; Eta ", 21, LBintervalStart - 0.5 , LBintervalStart + 20.5 , 56, -2.8, 2.8  ) );
  CHECK( Book2DHist ( &m_basic_LS.h_Phi_vs_LB, &lowStatFds, prefix + "tauPhi_vsLB", "Tau Phi vs Lumiblock);Lumiblock ; Phi ",  21, LBintervalStart - 0.5 , LBintervalStart + 20.5 , 65, PHIMIN+PHIMIN/64, PHIMAX+PHIMAX/64 ) );
  CHECK( Book2DHist ( &m_basic_LS.h_Et_vs_LB, &lowStatFds, prefix + "tauEt_vsLB", "Tau Et vs Lumiblock);Lumiblock; Et",  21, LBintervalStart - 0.5 , LBintervalStart + 20.5 , 60, 0., 300. ) );
  CHECK( Book2DHist ( &m_basic_LS.h_Phi_vs_LB_et15_BDTLoose, &lowStatFds, prefix + "tauPhiEt15BDTLoose_vsLB", "Tau Phi vs Lumiblock, 15GeV BDT Loose); Lumiblock ; Phi ", 21, LBintervalStart - 0.5 , LBintervalStart + 20.5 , 65,PHIMIN+PHIMIN/64, PHIMAX+PHIMAX/64 ) ); 
  CHECK( Book2DHist ( &m_basic_LS.h_Eta_vs_LB_et15_BDTLoose, &lowStatFds, prefix + "tauEtaEt15BDTLoose_vsLB", "Tau Eta vs Lumiblock, 15GeV BDT Loose; Lumiblock ; Eta ", 21, LBintervalStart - 0.5 ,  LBintervalStart + 20.5 , 56, -2.8, 2.8 )  );
  CHECK( Book2DHist ( &m_basic_LS.h_numCoreTracks_vs_LB, &lowStatFds, prefix + "NumCoreTrk_vsLB", "Tau Core tracks vs Lumiblock); Lumiblock ; NumCoreTk ", 21, LBintervalStart - 0.5 , LBintervalStart + 20.5 , 41, -0.5, 40.5 ) );

  return StatusCode::SUCCESS ;
}

StatusCode tauMonTool::bookHistos(s_mainFolder& mainFolder, std::string folderName, Interval_t interval)
{
	ATH_MSG_INFO("StatusCode tauMonTool::bookHistos, folderName = " << folderName );

	MonGroup folder(this, folderName, interval);

        if ( ! ( folderName == "Tau/Trigger/TauTrig" || folderName == "Tau" || folderName == "TauLS" ) )
        {
	  if ( bookKinHistos(mainFolder.kinFolder, folder ) .isFailure() )
            ATH_MSG_ERROR("Couldn't book kinematic histograms");
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

	CHECK( Book1DHist ( &folder.h_ntaus, &aGroup, prefix + "_nTauCandidates", "Number of tau candidates;Number of Taus per Event", 41, -0.5, 40.5) );
	CHECK( Book1DHist ( &folder.h_eta, &aGroup, prefix + "_tauEta", "Eta of tau candidates;Eta;Number of Candidates", 56, -2.8, 2.8 ) );
	CHECK( Book1DHist ( &folder.h_phi, &aGroup, prefix + "_tauPhi", "Phi of tau candidates;Phi;Number of Candidates", 65, -3.1415936-0.098174/2., 3.1415936+0.098174/2.) );
	CHECK( Book1DHist ( &folder.h_et, &aGroup, prefix + "_tauEt", "Et of tau candidates;Transverse Energy (GeV) );Number of Candidates", 60, 0.0, 300.0) );
	CHECK( Book1DHist ( &folder.h_charge, &aGroup, prefix + "_tauCharge", "Charge of tau candidates;Charge;Number of Candidates", 11, -5.5, 5.5) );
	CHECK( Book2DHist ( &folder.h_PhiVsEta, &aGroup, prefix + "_tauPhiVsEta", "Tau Phi vs. Eta;Eta;Phi;Number of Candidates", 56, -2.8, 2.8, 65, PHIMIN+PHIMIN/64., PHIMAX+PHIMAX/64.) );
	CHECK( Book2DHist ( &folder.h_Eta_vs_LB, &aGroup, prefix + "_tauEtaVsLB", "Tau Eta vs Lumiblock;Eta;Lumiblock", 56, -2.8, 2.8, (int)m_maxNLB/10 + 1, -5., m_maxNLB ) ) ;
	CHECK( Book2DHist ( &folder.h_Phi_vs_LB, &aGroup, prefix + "_tauPhiVsLB", "Tau Phi vs Lumiblock;Phi;Lumiblock", 65, PHIMIN+PHIMIN/64, PHIMAX+PHIMAX/64, (int)m_maxNLB/10 + 1, -5., m_maxNLB ) ) ;

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::bookTrackHistos(s_trkFolder& folder,std::string folderName, Interval_t interval)
{
	std::string fldName = folderName + "/Track";
	MonGroup aGroup(this, fldName, interval);
	fldName = this->fixName(fldName);

	CHECK( Book1DHist ( &folder.h_leadTrkPt, &aGroup, fldName + "_leadTrkPt","Pt of Leading track;Transverse Momentum (GeV)", 60, 0, 300. ) );
	CHECK( Book1DHist ( &folder.h_trkWidth2, &aGroup, fldName + "_trkWidth2","Weighted Track Width;Momentum-Weighted Width of Track System", 25, 0.0, 0.1) );
	CHECK( Book1DHist ( &folder.h_ipZ0SinThetaSigLeadTrk, &aGroup, fldName + "_ipZ0SinThetaSigLeadTrk","Impact Parameter z0 Sine Theta Significance of Leading Track;Z0SinTheta Significance", 50, -10.0, 10.0) );
        
	CHECK( Book1DHist ( &folder.h_d0, &aGroup, fldName + "_d0","Track d0;Transverse Impact Parameter (mm)", 50, -5.0, 5.0) );
	CHECK( Book1DHist ( &folder.h_z0sinThetaTJVA, &aGroup, fldName + "_z0TJVA","z0 respected to Tau vertex , Z0SinTheta (mm)", 50, -35.0, 35.0) );
	CHECK( Book1DHist ( &folder.h_z0sinThetaPriVtx, &aGroup, fldName + "_z0PriVtx"," z0 respected to primary vertex ; Z0 SinTheta (mm)", 50, -35.0, 35.0) );

	CHECK( Book1DHist ( &folder.h_dRJetSeedAxis, &aGroup, fldName + "_dRJetSeedAxis","DeltaR from Track to JetSeedAxis; dR ", 30, 0.0, 1.5 ) );


	CHECK( Book1DHist ( &folder.h_phi, &aGroup, fldName + "_phi","Track Phi;Phi", 65, PHIMIN+PHIMIN/64., PHIMAX+PHIMAX/64.) );
	CHECK( Book1DHist ( &folder.h_eta, &aGroup, fldName + "_eta","Track Eta;Eta", 53, -2.65, 2.65) );
	CHECK( Book1DHist ( &folder.h_pT, &aGroup, fldName + "_pT","Track pT;Transverse Momentum (GeV)", 50, 0.0, 300.0) );
	CHECK( Book1DHist ( &folder.h_nHighPTtaus, &aGroup, fldName + "_nHighPTtaus","Number of High-pT Tau Candidates;Number of Taus per Event", 11, -0.5, 10.5) );
	CHECK( Book1DHist ( &folder.h_numberOfTRTHighThresholdHits, &aGroup, fldName + "_numberOfTRTHighThresholdHits","Number of TRT High Threshold Hits;Number of High Threshold TRT Hits", 15, -0.5, 14.5) );
	CHECK( Book1DHist ( &folder.h_numberOfTRTHits, &aGroup, fldName + "_numberOfTRTHits","Number of TRT Low Threshold Hits;Number of Low Threshold TRT Hits", 101, -0.5, 100.5) );
	CHECK( Book1DHist ( &folder.h_numberOfTRTHighThresholdOutliers, &aGroup, fldName + "_numberOfTRTHighThresholdOutliers","Number of TRT High Threshold Outliers;Number of TRT High Threshold Outliers", 26, -0.5, 25.5) );
	CHECK( Book1DHist ( &folder.h_numberOfTRTOutliers, &aGroup, fldName + "_numberOfTRTOutliers","Number of TRT Low Threshold Outliers;Number of TRT Low Threshold Outliers", 41, -0.5, 40.5) );
	CHECK( Book1DHist ( &folder.h_eProbabilityHT, &aGroup, fldName + "_eProbabilityHT","Probability of High Threshold ;Probability of TRT High Threshold ", 12, -0.1, 1.1 ) );

	CHECK( Book1DHist ( &folder.h_numberOfSCTHits, &aGroup, fldName + "_numberOfSCTHits","Number of SCT Hits;Number of SCT Hits", 26, -0.5, 25.5) );
	CHECK( Book1DHist ( &folder.h_numberOfSCTSharedHits, &aGroup, fldName + "_numberOfSCTSharedHits","Number of SCT shared Hits;Number of shared SCT Hits", 21, -0.5, 20.5) );
	CHECK( Book1DHist ( &folder.h_numberOfPixelHits, &aGroup, fldName + "_numberOfPixelHits","Number of Pixel Hits;Number of Pixel Hits", 16, -0.5, 15.5) );
	CHECK( Book1DHist ( &folder.h_numberOfPixelSharedHits, &aGroup, fldName + "_numberOfPixelSharedHits","Number of Pixel shared Hits;Number of shared Pixel Hits", 16, -0.5, 15.5) );
	CHECK( Book1DHist ( &folder.h_numberOfInnermostPixelLayerHits, &aGroup, fldName + "_numberOfInnermostPixelLayerHits","Number of InnerMost Pixel layer Hits; Number of MostInner Pixel Hits", 11, -0.5, 10.5) );
	CHECK( Book1DHist ( &folder.h_rConv, &aGroup, fldName + "_rConv","rConv; rConv", 81, -0.5, 80.5) );
	CHECK( Book1DHist ( &folder.h_rConvII, &aGroup, fldName + "_rConvII","rConvII; rConvII", 81, -0.5, 80.5) );


	CHECK( Book2DHist ( &folder.h_z0_vs_LB, &aGroup, fldName + "_z0VsLB", "Track z0 vs Lumiblock;Longitudinal Impact Parameter (mm) );Lumiblock" , (int)m_maxNLB/10 + 1, -5., m_maxNLB , 50, -35.0, 35.0 ) );

	CHECK( Book1DHist ( &folder.h_leadTrackDeltaEta, &aGroup, fldName + "_leadTrackDeltaEta","Leading Track Delta Eta; Leading track Delta Eta from jetSeed, EleVeto BDT input", 30, -1.5, 1.5 ) );
	CHECK( Book1DHist ( &folder.h_leadTrackDeltaPhi, &aGroup, fldName + "_leadTrackDeltaPhi","Leading Track Delta Phi; Leading track Delta Phi from jetSeed, EleVeto BDT input", 30, -1.5, 1.5 ) );

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::bookCaloHistos(s_caloFolder& folder,std::string folderName, Interval_t interval)
{
	std::string fldName = folderName + "/Calo";

	MonGroup aGroup(this, fldName, interval);
	fldName = this->fixName(fldName);

	CHECK( Book1DHist ( &folder.h_eta, &aGroup, fldName + "_jetSeedEta","Calorimeter eta of tau candidates;Eta;Number of Candidates", 56, -2.8, 2.8 ) );
	CHECK( Book1DHist ( &folder.h_phi, &aGroup, fldName + "_jetSeedPhi","Calorimeter phi of tau candidates;Phi;Number of Candidates", 65, PHIMIN+PHIMIN/64., PHIMAX+PHIMAX/64.) );
	CHECK( Book1DHist ( &folder.h_jetSeedPt, &aGroup, fldName + "_jetSeedPt","Calorimeter pt of tau candidates; Et;Number of Candidates", 100, 0., 300.) );
	CHECK( Book1DHist ( &folder.h_etHadAtEMScale, &aGroup, fldName + "_etHadAtEMScale","Hadronic Energy at the EM Scale;Had Et (GeV) );Number of Candidates", 50, 0.0, 200.0) );
	CHECK( Book1DHist ( &folder.h_etEMAtEMScale, &aGroup, fldName + "_etEMAtEMScale","EM energy at the EM scale;EM Et (GeV) );Number of Candidates", 50, 0.0, 200.0) );
	CHECK( Book1DHist ( &folder.h_EMRadius, &aGroup, fldName + "_EMRadius","Uncalibrated EM Radius;EM Radius;Number of Candidates", 50, 0.0, 1.2 ) );
	CHECK( Book1DHist ( &folder.h_hadRadius, &aGroup, fldName + "_hadRadius","Hadronic Radius of tau candidates;Hadronic Radius;Number of Candidates", 50, 0.0, 1.0) );
	CHECK( Book1DHist ( &folder.h_isolFrac, &aGroup, fldName + "_isolFrac","Isolation Fraction;Et Isolation Fraction;Number of Candidates", 51, 0.0, 1.02) );
	CHECK( Book1DHist ( &folder.h_stripWidth2, &aGroup, fldName + "_stripWidth2","Strip Width of tau candidates;Strip Width;Number of Candidates", 50, -0.1, 0.12 ) );
	CHECK( Book1DHist ( &folder.h_nStrip, &aGroup, fldName + "_nStrip","Number of strip cells of tau candidates;Number of Strip Cells;Number of Candidates", 56, -0.5, 55.5) );
	CHECK( Book2DHist ( &folder.h_CentFrac_vs_LB, &aGroup, fldName + "_CentFracVsLB", "Centrality Fraction vs Lumiblock;Centrality Fraction;Lumiblock", 51, 0.0, 1.02, (int)m_maxNLB/10 + 1, -5., (double)m_maxNLB  ) );
	CHECK( Book2DHist ( &folder.h_isolFrac_vs_LB, &aGroup, fldName + "_isolFracVsLB", "Isolation Fraction vs Lumiblock;Isolation Fraction;Lumiblock", 51, 0.0, 1.02, (int)m_maxNLB/10 + 1, -5., m_maxNLB  ) );

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::bookIDHistos(s_idFolder& folder,std::string folderName, Interval_t interval)
{
	std::string fldName = folderName + "/Identification";
	MonGroup aGroup(this, fldName, interval);

        if ( bookBDTLooseHistos(folder.BDTLooseFolder, fldName, interval).isFailure() ) 	
          ATH_MSG_ERROR("Couldn't book BDTLoose histograms");
        if (  folderName != "TauLS" )
        {
          if ( bookJetsigBDTHistos(folder.tauJetinputs, fldName, interval).isFailure() )  	
            ATH_MSG_ERROR("Couldn't book tau BDT inputs histograms");
          if ( bookEleVetoHistos(folder.EleVetoinputs, fldName, interval).isFailure() )  	
           ATH_MSG_ERROR("Couldn't book Electron veto BDT inputs histograms");
        }

	fldName = this->fixName(fldName);
	CHECK( Book1DHist ( &folder.h_tauBDTLoose,          &aGroup, fldName + "_tauBDTLoose",          "Identification Flag: tauBDTLoose",          2, -0.5, 1.5 ) );
	addBinLabelIDHistos(folder.h_tauBDTLoose);
	CHECK( Book1DHist ( &folder.h_tauBDTMedium,         &aGroup, fldName + "_tauBDTMedium",         "Identification Flag: tauBDTMedium",         2, -0.5, 1.5 ) );
	addBinLabelIDHistos(folder.h_tauBDTMedium);
	CHECK( Book1DHist ( &folder.h_tauBDTTight,          &aGroup, fldName + "_tauBDTTight",          "Identification Flag: tauBDTTight",          2, -0.5, 1.5 ) );
	addBinLabelIDHistos(folder.h_tauBDTTight);
	CHECK( Book1DHist ( &folder.h_muonVeto,             &aGroup, fldName + "_muonVeto",             "Muon Veto",                                 2, -0.5, 1.5 ) );
	addBinLabelIDHistos(folder.h_muonVeto);
	CHECK( Book1DHist ( &folder.h_JetBDTBkgMedium,          &aGroup, fldName + "_JetBDTBkgMedium",          "Loose EleBDT",                                 2, -0.5 , 1.5 ) );
	addBinLabelIDHistos(folder.h_JetBDTBkgMedium);
	CHECK( Book1DHist ( &folder.h_eleBDTMedium,         &aGroup, fldName + "_eleBDTMedium",         "Medium EleBDT",                                2, -0.5, 1.5) );
	addBinLabelIDHistos(folder.h_eleBDTMedium);
	CHECK( Book1DHist ( &folder.h_eleBDTTight,          &aGroup, fldName + "_eleBDTTight",          "Tight EleBDT",                                 2, -0.5, 1.5 ) );
	addBinLabelIDHistos(folder.h_eleBDTTight);

	CHECK( Book1DHist ( &folder.h_BDTJetScore,         &aGroup, fldName + "_BDTJetScore",         "BDT Score for Jet Rejection;Boosted Decision Tree Score",                             48, -1.1, 1.1) );

	CHECK( Book1DHist ( &folder.h_BDTJetScoreSigTrans, &aGroup, fldName + "_BDTJetScoreSigTrans", "Flattened Signal Transformed BDT Score for Jet Rejection;Boosted Decision Tree Score",         24, -0.1, 1.1) );

	CHECK( Book1DHist ( &folder.h_BDTEleScoreSigTrans, &aGroup, fldName + "_BDTEleScoreSigTrans", " Flattened Signal Transformed BDT  for Ele Rejection;Boosted Decision Tree Score",         24, -0.1, 1.1) );

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::bookJetsigBDTHistos( s_tauJetBDT& folder, std::string folderName, Interval_t interval)
{
	std::string fldName = folderName + "/tauJetBDTinputs" ;

	MonGroup aGroup(this, fldName, interval);
	fldName = this->fixName(fldName);

        CHECK( Book1DHist ( &folder.h_ptIntermediateAxis, &aGroup, fldName + "_ptIntermediateAxis", "Pt of tau candidates in IntermediateAxis;Transverse Energy (GeV) );Number of Candidates", 60, 0.0, 150.0) );
        CHECK( Book1DHist ( &folder.h_CentFrac, &aGroup, fldName + "_CentFrac","Centrality fraction of tau candidates;Centrality Fraction;Number of Candidates", 50, 0.0, 1.5 ) );
	CHECK( Book1DHist ( &folder.h_dRmax, &aGroup, fldName + "_dRmax", "Maximum delta R;delta R;Number of Candidates", 42, 0.0, 0.21) );
	CHECK( Book1DHist ( &folder.h_trkAvgDist, &aGroup, fldName + "_trkAvgDist","Average Track Distance from Calorimeter Seed;Distance (mm) );Number of Candidates", 21, -0.05, 1.0) );
        CHECK( Book1DHist ( &folder.h_SumPtTrkFrac, &aGroup, fldName + "_SumPtTrkFrac", "Track PtSum Fraction ", 10    , 0.    , 1.0 ) );
        CHECK( Book1DHist ( &folder.h_etOverPtLeadTrk, &aGroup, fldName + "_etOverPtLeadTrk", "Leading track Et over pT",  20   , 0.    , 12.0 ) );
        CHECK( Book1DHist ( &folder.h_EMPOverTrkSysP, &aGroup, fldName + "_EMPOverTrkSysP", "momenta EM over tracks",   40  , 0    , 10.0 ) );
        CHECK( Book1DHist ( &folder.h_mEflowApprox, &aGroup, fldName + "_mEflowApprox", "Approx Energy flow", 50    , 0.    , 20. ) );
        CHECK( Book1DHist ( &folder.h_ptRatioEflowApprox, &aGroup, fldName + "_ptRatioEflowApprox","Ratio of pt to shot total energy for associated tracks; track pt ratio", 20, 0, 2.0 ) );

        CHECK( Book1DHist ( &folder.h_massTrkSys, &aGroup, fldName + "_massTrkSys","Mass of the Track System;Invariant Mass (GeV)", 30, -1.0, 5.0) );
        CHECK( Book1DHist ( &folder.h_trFlightPathSig, &aGroup, fldName + "_trFlightPathSig","Track Transverse Flight Path Significance;Transverse Flight Path Significance", 130, -5.0, 8.0) );
        CHECK( Book1DHist ( &folder.h_absipSigLeadTrk, &aGroup, fldName + "_absipSigLeadTrk","Absolute Impact Parameter Significance of Leading Track;Transverse Impact Parameter Significance", 50, .0, 5.0) );


  return StatusCode::SUCCESS;
}

StatusCode tauMonTool::bookEleVetoHistos( s_EleVeto & folder, std::string folderName, Interval_t interval)
{
	std::string fldName = folderName + "/EleVetoBDTinputs";
	MonGroup aGroup(this, fldName, interval);
	fldName = this->fixName(fldName);

	CHECK( Book1DHist ( &folder.h_PSSFrac, &aGroup, fldName + "_PSSFrac","Energy fraction for PreSampling and sampling layers in effective clusters in shot; Sampling energy fraction", 10, 0, 1.0 ) );
	CHECK( Book1DHist ( &folder.h_etHotShotWinOverPtLeadTrk, &aGroup, fldName + "_etHotShotWinOverPtLeadTrk", "etHotShotWinOverPtLeadTrk", 20, 0., 12.) );

	CHECK( Book1DHist ( &folder.h_EMFracFixed, &aGroup, fldName + "_EMFracFixed", "EMFracFixed", 20, 0., 2.) );
	CHECK( Book1DHist ( &folder.h_hadLeakFracFixed, &aGroup, fldName + "_hadLeakFracFixed", "hadLeakFracFixed", 20, 0., 2.) );

  return StatusCode::SUCCESS;
}

StatusCode tauMonTool::bookBDTLooseHistos(s_BDTFolder& folder, std::string folderName, Interval_t interval)
{
	std::string fldName = folderName + "/BDTLoose15GeV";
	MonGroup aGroup(this,fldName,interval);
	fldName = this->fixName(fldName);

	CHECK( Book1DHist ( &folder.h_et, &aGroup, fldName + "_et", "Et of Tau Candidate;Et (GeV) );Number of Candidates", 50, 0.0, 250.0) );
	CHECK( Book1DHist ( &folder.h_eta, &aGroup, fldName + "_eta", "Eta of Tau Candidate;Eta;Number of Candidates", 56, -2.8, 2.8 ) );
	CHECK( Book1DHist ( &folder.h_phi, &aGroup, fldName + "_phi", "Phi of Tau Candidate;Phi;Number of Candidates", 65, PHIMIN+PHIMIN/64, PHIMAX+PHIMAX/64) );
	CHECK( Book1DHist ( &folder.h_nTracks, &aGroup, fldName + "_numTracks", "Number of Tracks;Number of Tracks;Number of Candidates", 21,-0.5, 20.5) );

	CHECK( Book1DHist ( &folder.h_nclst, &aGroup, fldName + "_nCluster","Number of CaloTopoClusters", 40, 0, 40 ) );
	CHECK( Book1DHist ( &folder.h_panmode, &aGroup, fldName + "_PanMode","tau decay mode from PanTau upon JetBDTSigMedium; mode ", 5, 0, 5 ) );

        folder.h_panmode->GetXaxis()->SetBinLabel( 1, "1p0n" ) ;
        folder.h_panmode->GetXaxis()->SetBinLabel( 2, "1p1n" ) ;
        folder.h_panmode->GetXaxis()->SetBinLabel( 3, "1pXn" ) ;
        folder.h_panmode->GetXaxis()->SetBinLabel( 4, "3p0n" ) ;
        folder.h_panmode->GetXaxis()->SetBinLabel( 5, "3pXn" ) ;

	return StatusCode::SUCCESS;
}

StatusCode tauMonTool::bookSubStructureHistos( s_sbstrctFolder& folder,std::string folderName, Interval_t interval)
{

	std::string fldName = folderName + "/SubStructure";
	MonGroup aGroup(this, fldName, interval);
	fldName = this->fixName(fldName);

	CHECK( Book1DHist ( &folder.h_nShot, &aGroup, fldName + "_nShot","number of shots ; shot number ", 20, 0, 20 ) );
	CHECK( Book1DHist ( &folder.h_InvMass, &aGroup, fldName + "_InvMassEffClusters","Invariant mass of effective clusters in shot; invariant mass (GeV)", 40, 0., 8. ) );
	CHECK( Book1DHist ( &folder.h_L2EOverAllClusterE, &aGroup, fldName + "_EfracL2EffCluster","Energy fraction of leading two effective clusters in shot; energy fraction", 15, 0, 1.5 ) );
	CHECK( Book1DHist ( &folder.h_stpt3, &aGroup, fldName + "_shots_pt3","weighted cell pt in 3x3 window in shots; pt3 (GeV) ", 24, 0, 12 ) );

	CHECK( Book1DHist ( &folder.h_EMFracTrk, &aGroup, fldName + "_EMFracTrk","Ratio of pt to shot electromagnetic energy for associated tracks; track pt ratio in EM", 15, 0, 1.5 ) );
	CHECK( Book1DHist ( &folder.h_IsoCorr, &aGroup, fldName + "_EisoEffCluster","Isolation Energy after correction in effective clusters ; isolation energy (GeV)", 10, 0, 50.0 ) );

	CHECK( Book1DHist ( &folder.h_nNeutPFO, &aGroup, fldName + "_NumNeutPFO","Number of neutral ParticleFlow objects ; PFO number", 20, 0, 20 ) );
	CHECK( Book1DHist ( &folder.h_pi0bdt, &aGroup, fldName + "_BDTscoreAsP0","BDT score indentifying pion zero ; BDT score", 15, 0, 1.2 ) );
	CHECK( Book1DHist ( &folder.h_panmode, &aGroup, fldName + "_PanMode","tau decay mode from PanTau upon JetBDTSigMedium; mode ", 5, 0, 5 ) );

        folder.h_panmode->GetXaxis()->SetBinLabel( 1, "1p0n" ) ;
        folder.h_panmode->GetXaxis()->SetBinLabel( 2, "1p1n" ) ;
        folder.h_panmode->GetXaxis()->SetBinLabel( 3, "1pXn" ) ;
        folder.h_panmode->GetXaxis()->SetBinLabel( 4, "3p0n" ) ;
        folder.h_panmode->GetXaxis()->SetBinLabel( 5, "3pXn" ) ;

	CHECK( Book1DHist ( &folder.h_panpt, &aGroup, fldName + "_PanPt","tau Pt from PanTau ; substructure pt (GeV)", 20, 0, 200 ) );
	CHECK( Book1DHist ( &folder.h_paneta, &aGroup, fldName + "_PanEta","tau Eta from PanTau ; substructure Eta", 56, -2.8, 2.8 ) );
	CHECK( Book1DHist ( &folder.h_panphi, &aGroup, fldName + "_PanPhi","tau Phi from PanTau ; substructure Phi", 64, -3.2, 3.2 ) );

        return StatusCode::SUCCESS;
}


//--------------------------------------------------------------------------------
// book and register a 1D histogram
//--------------------------------------------------------------------------------
StatusCode tauMonTool::Book1DHist (TH1 ** hist, MonGroup * monName, std::string  histName, std::string  histTitle, int NBins, double lowBin, double highBin)
{
	*hist = new TH1F( histName.c_str(), histTitle.c_str(), NBins, lowBin, highBin);
	StatusCode sc = monName->regHist( *hist);

	if ( sc.isFailure() ) ATH_MSG_FATAL("Failed to register " << histName);

	return sc;
}

StatusCode tauMonTool::Book1DHistVsLB (TH1 ** hist, MonGroup * monName, std::string  histName, std::string  histTitle, int NBins, double lowBin, double highBin)
{
        MonGroup * groupLB = new MonGroup( this, monName->system(), monName->interval(), 
                                 ATTRIB_X_VS_LB, monName->chain(), "merge" );

	*hist = new TH1F( histName.c_str(), histTitle.c_str(), NBins, lowBin, highBin);
	StatusCode sc = groupLB->regHist( *hist);

	if ( sc.isFailure() ) ATH_MSG_FATAL("Failed to register " << histName);

        delete groupLB ;

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

StatusCode tauMonTool::Book2DHistVsLB (TH2 ** hist, MonGroup * monName, std::string  histName, std::string  histTitle, int NXBins, double lowXBin, double highXBin, int NYBins, double lowYBin, double highYBin)
{
        MonGroup *groupLB = new MonGroup ( this, monName->system(), monName->interval(), 
                                 ATTRIB_X_VS_LB, monName->chain(), "merge" );
	*hist = new TH2F( histName.c_str(), histTitle.c_str(), NXBins, lowXBin, highXBin, NYBins, lowYBin, highYBin);
	StatusCode sc = groupLB->regHist( *hist);
	if ( sc.isFailure() )
	{
		ATH_MSG_FATAL("Failed to register " << histName);
		return sc;
	}
   
        delete groupLB ;

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

