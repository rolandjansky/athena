/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          JetVertexCharge.cxx  -  Description
                             -------------------

  created : August 2015
  authors: Luca Colasurdo (Radboud University Nijmegen and Nikhef)
  email : luca.colasurdo@cern.ch


  Look at the header file for more information.

 ***************************************************************************/

#include "JetTagTools/JetVertexCharge.h"

#include <cmath>
#include <algorithm>
#include <memory>

#include "CLHEP/Vector/LorentzVector.h"

#include "xAODMuon/MuonContainer.h"

#include "JetTagCalibration/CalibrationBroker.h"
#include "MuonSelectorTools/IMuonSelectionTool.h"
#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
#include "TMVA/Reader.h"
#include "TList.h"
#include "TObjString.h"

#include "TMVA/MethodBase.h"
#include "TH1.h"

#include "xAODBTagging/BTagVertex.h"

#include "ParticleJetTools/ParticleToJetAssociator.h"

const double minProb = 1e-9;

namespace Analysis {


  JetVertexCharge::JetVertexCharge(const std::string& t, const std::string& n, const IInterface*  p) :
    AthAlgTool(t,n,p),
    m_calibrationTool("BTagCalibrationBroker"),
    m_muonSelectorTool("JVC_MuonSelectorTool"),
    m_muonCorrectionTool( "JVC_MuonCorrectionTool" ),
    m_runModus("analysis")
  {

    declareProperty("SecVxFinderName",		m_secVxFinderName);
    declareProperty("Runmodus",                 m_runModus);

    declareProperty("calibrationTool", 		m_calibrationTool);
    declareProperty("muonSelectorTool", 	m_muonSelectorTool);
    declareProperty("muonCorrectionTool", 	m_muonCorrectionTool);
    declareProperty("taggerNameBase",		m_taggerNameBase = "JetVertexCharge");

    declareProperty("useForcedCalibration",  	m_doForcedCalib = false);
    declareProperty("forcedCalibName",		m_ForcedCalibName = "AntiKt4EMTopo");

    declareProperty("muonAssociationName", 	m_muonAssociationName = "Muons");
    declareProperty("trackAssociationName", 	m_trackAssociationName = "BTagTrackToJetAssociator");

    declareProperty("kFactor", 			m_kappa = 1.1);
    declareProperty("kFactorSV", 		m_kappa_SV = 0.7);
    declareProperty("kFactorTV", 		m_kappa_TV = 0.7);

    declareProperty("Trkd0Cut",           	m_Trkd0Cut=3.5);
    declareProperty("Trkz0Cut",           	m_Trkz0Cut=4.5);
    declareProperty("TrkPtCut",           	m_TrkPtCut=500.0);
    declareProperty("TrkChi2Cut",         	m_TrkChi2Cut=5.0);
    declareProperty("CutPrecisionHits",	  	m_CutPrecisionHits= 9 );
    declareProperty("CutPixelHits",		m_CutPixelHits= 1  );
    declareProperty("CutTRTHits",		m_CutTRTHits= 9  );
    declareProperty("CutIBLHits",		m_CutIBLHits= 0 );
    declareProperty("CutSCTHits",		m_CutSCTHits= 4  );
    declareProperty("CutSharedHits",		m_CutSharedHits= 2 );

    declareProperty("MuonQuality",		m_muonQualityCut = xAOD::Muon::Medium );

    declareInterface< ITagTool >(this);

    ClearVars();
  }

///////////
//Destructor
  JetVertexCharge::~JetVertexCharge() { }


///////////
//Initialize method
  StatusCode JetVertexCharge::initialize() {

    StatusCode sc = m_calibrationTool.retrieve();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_calibrationTool);
      return sc;
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_calibrationTool);
    }


    //Retrieve the Muon Selectot tool
    sc = m_muonSelectorTool.retrieve();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_muonSelectorTool);
      return sc;
    } else {
      ATH_MSG_INFO("#BTAG# Retrieved tool " << m_muonSelectorTool);
    }

    //Muon Calibration and Smearing
    sc = m_muonCorrectionTool.retrieve();
    if ( sc.isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_muonCorrectionTool);
      return sc;
    } else {
      ATH_MSG_INFO("#BTAG# Retrieved tool " << m_muonCorrectionTool);
    }

    ATH_CHECK( m_muonCorrectionTool->initialize() );


   //MVA xml files
    m_calibrationTool->registerHistogram(m_taggerNameBase, m_taggerNameBase+"Calib_cat_JC_SVC_noMu");
    m_calibrationTool->registerHistogram(m_taggerNameBase, m_taggerNameBase+"Calib_cat_JC_SVC_incMu");
    m_calibrationTool->registerHistogram(m_taggerNameBase, m_taggerNameBase+"Calib_cat_JC_SVC_TVC_noMu");
    m_calibrationTool->registerHistogram(m_taggerNameBase, m_taggerNameBase+"Calib_cat_JC_SVC_TVC_incMu");
    m_calibrationTool->registerHistogram(m_taggerNameBase, m_taggerNameBase+"Calib_cat_JC_incMu");

    //reference histos
    m_calibrationTool->registerHistogram(m_taggerNameBase, "jvc_JC_SVC_noMu_b");
    m_calibrationTool->registerHistogram(m_taggerNameBase, "jvc_JC_SVC_noMu_bbar");
    m_calibrationTool->registerHistogram(m_taggerNameBase, "jvc_JC_SVC_incMu_b");
    m_calibrationTool->registerHistogram(m_taggerNameBase, "jvc_JC_SVC_incMu_bbar");
    m_calibrationTool->registerHistogram(m_taggerNameBase, "jvc_JC_SVC_TVC_noMu_b");
    m_calibrationTool->registerHistogram(m_taggerNameBase, "jvc_JC_SVC_TVC_noMu_bbar");
    m_calibrationTool->registerHistogram(m_taggerNameBase, "jvc_JC_SVC_TVC_incMu_b");
    m_calibrationTool->registerHistogram(m_taggerNameBase, "jvc_JC_SVC_TVC_incMu_bbar");
    m_calibrationTool->registerHistogram(m_taggerNameBase, "jvc_JC_incMu_b");
    m_calibrationTool->registerHistogram(m_taggerNameBase, "jvc_JC_incMu_bbar");
    m_calibrationTool->registerHistogram(m_taggerNameBase, "jvc_JC_noMu_b");
    m_calibrationTool->registerHistogram(m_taggerNameBase, "jvc_JC_noMu_bbar");
    m_calibrationTool->registerHistogram(m_taggerNameBase, "jvc_SVC_b");
    m_calibrationTool->registerHistogram(m_taggerNameBase, "jvc_SVC_bbar");
    m_calibrationTool->registerHistogram(m_taggerNameBase, "jvc_JC_all_b");
    m_calibrationTool->registerHistogram(m_taggerNameBase, "jvc_JC_all_bbar");


    // m_alreadySetup = false;

    m_catNames[JC_SVC_noMu] = "JC_SVC_noMu";
    m_catNames[JC_SVC_incMu] = "JC_SVC_incMu";
    m_catNames[JC_SVC_TVC_noMu] = "JC_SVC_TVC_noMu";
    m_catNames[JC_SVC_TVC_incMu] = "JC_SVC_TVC_incMu";
    m_catNames[JC_incMu] = "JC_incMu";

    m_tmvaReaders.clear();
    m_tmvaMethod.clear();

    m_histoList_pos.clear();
    m_histoList_neg.clear();

    initializeVariablePtrs();

    return StatusCode::SUCCESS;
  }


///////////
//Finalize method
  StatusCode JetVertexCharge::finalize() {


    //delete readers;
    std::map<std::string, TMVA::Reader*>::iterator pos = m_tmvaReaders.begin();
    for( ; pos != m_tmvaReaders.end(); ++pos ) delete pos->second;
    std::map<std::string, TMVA::MethodBase*>::iterator posm = m_tmvaMethod.begin();
    for( ; posm != m_tmvaMethod.end(); ++posm ) delete posm->second;


    msg(MSG::DEBUG)  << "Finalize successful" << endmsg;
    return StatusCode::SUCCESS;

  }


//////////////////////////////////////////////////////////////////
StatusCode JetVertexCharge::tagJet( xAOD::Jet& jetToTag, xAOD::BTagging* BTag, const std::string jetName) {




   /** author to know which jet algorithm: */
   std::string author = jetName;
   if (m_doForcedCalib) author = m_ForcedCalibName;
   std::string alias = m_calibrationTool->channelAlias(author);


   ClearVars();

   m_jet_uPt = jetToTag.pt();



   // if(m_runModus=="analysis" && !m_alreadySetup ) {
   //   StatusCode sc = SetupReaders(author, alias, JC_SVC_noMu );
   //    if( sc.isFailure() ) {
   // 	ATH_MSG_WARNING("#BTAG# Could not setup the reader for the JC_SVC_noMu category, author = " << author);
   // 	return StatusCode::SUCCESS;
   //    } else ATH_MSG_INFO("#BTAG# Reader correctly setup for the JC_SVC_noMu category, author = " << author);

   //    sc = SetupReaders(author, alias, JC_SVC_incMu );
   //    if( sc.isFailure() ) {
   // 	ATH_MSG_WARNING("#BTAG# Could not setup the reader for the JC_SVC_incMu category, author = " << author);
   // 	return StatusCode::SUCCESS;
   //    } else ATH_MSG_INFO("#BTAG# Reader correctly setup for the JC_SVC_incMu category, author = " << author);

   //    sc = SetupReaders(author, alias, JC_SVC_TVC_noMu );
   //    if( sc.isFailure() ) {
   // 	ATH_MSG_WARNING("#BTAG# Could not setup the reader for the JC_SVC_TVC_noMu category, author = " << author);
   // 	return StatusCode::SUCCESS;
   //    } else ATH_MSG_INFO("#BTAG# Reader correctly setup for the JC_SVC_TVC_noMu category, author = " << author);

   //    sc = SetupReaders(author, alias, JC_SVC_TVC_incMu );
   //    if( sc.isFailure() ) {
   // 	ATH_MSG_WARNING("#BTAG# Could not setup the reader for the JC_SVC_TVC_incMu category, author = " << author);
   // 	return StatusCode::SUCCESS;
   //    } else ATH_MSG_INFO("#BTAG# Reader correctly setup for the JC_SVC_TVC_incMu category, author = " << author);

   //    sc = SetupReaders(author, alias, JC_incMu );
   //    if( sc.isFailure() ) {
   // 	ATH_MSG_WARNING("#BTAG# Could not setup the reader for the JC_incMu category, author = " << author);
   // 	return StatusCode::SUCCESS;
   //    } else ATH_MSG_INFO("#BTAG# Reader correctly setup for the JC_incMu category, author = " << author);

   //    m_alreadySetup =  true;
   // }


   //==============================================================
   //          computing the JetCharge (JC)
   //==============================================================

   std::vector<ElementLink< xAOD::TrackParticleContainer > > tracksInJet;
   tracksInJet = BTag->auxdata< std::vector<ElementLink< xAOD::TrackParticleContainer > > >(m_trackAssociationName);
   if( tracksInJet.size() == 0 ) {
      ATH_MSG_DEBUG("#BTAG#  Could not find tracks associated with name " << m_trackAssociationName);
   } else {

      ATH_MSG_DEBUG("#BTAG# There are "<<tracksInJet.size()<<" tracks associated to the jet.");

      float  charge = 0, denom=0;
      float  charge_all = 0, denom_all=0;
      float  minpt = 0;

      for(std::vector<ElementLink< xAOD::TrackParticleContainer > >::iterator itEL = tracksInJet.begin(); itEL != tracksInJet.end(); ++itEL ) {
  	 const xAOD::TrackParticle* tp = *(*itEL);

         charge_all += ( tp->charge()) * std::pow( tp->pt(), m_kappa );
         denom_all += pow( tp->pt(), m_kappa);

         if( !passTrackCuts(*tp)) continue;
         if( tp->pt() >  minpt)  {
            m_jc_track_pt = tp->pt();
            minpt = tp->pt();
         }
 	 m_ngoodtrk++;
         charge += ( tp->charge()) * std::pow( tp->pt(), m_kappa );
         denom += pow( tp->pt(), m_kappa);
      }

      if(denom != 0) m_jc = charge / denom;
      if(denom_all != 0) m_jc_all= charge_all / denom_all;
      m_jc_jetPt = charge/jetToTag.pt();
      m_jc_all_jetPt = charge_all/jetToTag.pt();


   }


   //==============================================================
   //          computing the Vertex Charges (SVC/TVC)
   //==============================================================


   std::vector< myVtxInfo > myVector;



   std::vector<ElementLink<xAOD::BTagVertexContainer> >  JFVerticesLinks;
   bool ret = BTag->variable<std::vector<ElementLink<xAOD::BTagVertexContainer>>>("JetFitter", "JFvertices", JFVerticesLinks );
   if(ret) {
      if( JFVerticesLinks.size() ==0) {
	ATH_MSG_DEBUG("#BTAG# No JF vertices ");
      } else {

	ATH_MSG_DEBUG("#BTAG# There are "<<JFVerticesLinks.size()<<" JF vertices ");

        std::vector<float> fittedPosition;
        std::vector< float > fittedCov;
        BTag->variable<std::vector< float > >("JetFitter", "fittedPosition", fittedPosition);
        BTag->variable<std::vector< float > >("JetFitter", "fittedCov", fittedCov);



        for( size_t ivx = 0; ivx< JFVerticesLinks.size(); ivx++) {
	   const xAOD::BTagVertex *myBTagVtx = (*JFVerticesLinks.at(ivx));

           //Cutting on the vertices
	   if( myBTagVtx->chi2()/myBTagVtx->NDF()  > 5. )  continue;
	   if( fittedPosition.at(ivx+5)<0. || fittedPosition.at(ivx+5)>250. )  continue;
	   if( sqrt(fittedCov.at(ivx+5)) > 5.  )  continue;


	   myVtxInfo newVtx;
           newVtx.tracks.clear();
	   newVtx.pos = fittedPosition.at(ivx+5);
	   newVtx.err = sqrt(fittedCov.at(ivx+5) );
           const std::vector< ElementLink<xAOD::TrackParticleContainer> > tpLinks = myBTagVtx->track_links();

	   for( unsigned int it=0; it< tpLinks.size(); it++) {
  	     newVtx.tracks.push_back( *tpLinks.at(it)  );
	   }
	   myVector.push_back(newVtx);

        }

        //ordering the Vx according to d0
        std::sort( myVector.begin(), myVector.end(), distOrdering );

      }   //There is at least 1 vtx
   } //if (ret)


   //Merge the "higher-vertices"
   if( myVector.size()>2  ) {
     double sumw = 0;

     for( int ivx = 1; ivx<  (int) myVector.size(); ivx++) {  //skip SV
        double w = 1./std::pow( myVector.at(ivx).err, 2 );
        sumw += w;
        if( ivx>1) myVector.at(1).pos += myVector.at(ivx).pos*w;
        else if( ivx==1) myVector.at(1).pos = myVector.at(ivx).pos*w;
        int size = myVector.at(ivx).tracks.size();
 	for(int itrk=0; (itrk<size && ivx != 1); itrk++) {              //so that I don't count the TV tracks twice
	    const xAOD::TrackParticle *tp =  myVector.at(ivx).tracks.at(itrk);
	    myVector.at(1).tracks.push_back(tp);
 	}
     }

     myVector.at(1).pos /= sumw;
     myVector.at(1).err = 1./sumw;

     myVector.erase( myVector.begin() + 2, myVector.end() );
   }




   //The preparation of the vertices is done, let's compute the charges!


   if( myVector.size() >0 ) {  //Let's work with the SV

      myVtxInfo svx = myVector.at(0);
      m_sv_ntrk = svx.tracks.size();


      float charge = 0, denom = 0;
      float ptmin = 0;
      for(int itrk=0;  itrk< m_sv_ntrk; itrk++) {
         const xAOD::TrackParticle *tp =  svx.tracks.at(itrk);
         if( tp->pt() >  ptmin)  {
            m_sv_track_pt = tp->pt();
            ptmin = tp->pt();
         }
         charge += ( tp->charge()) * std::pow( tp->pt(), m_kappa_SV);
         denom += pow( tp->pt(), m_kappa_SV);
      }
      if(denom != 0)  m_svc = charge/denom;
      m_svc_jetPt = charge/jetToTag.pt();

      m_sv_dist =  svx.pos;
      m_sv_err =  svx.err;

      //Computing the mass
      TLorentzVector bhad;
      for(int itrk=0;  itrk< m_sv_ntrk; itrk++) {
         const xAOD::TrackParticle *tp =  svx.tracks.at(itrk);
         TLorentzVector part; part.SetPtEtaPhiM( tp->pt(), tp->eta(), tp->phi(), 139.570 );   //Pion mass [MeV]
         bhad += part;
      }
      m_sv_mass_pions = bhad.M();
   }


   //-----------------------------------------------------------------

   if( myVector.size() >1 ) {  //Let's work with the TV
      myVtxInfo tvx = myVector.at(1);
      m_tv_ntrk = tvx.tracks.size();
      if( (tvx.tracks.size() % 2) == 1 ) m_tv_ntrk -= 1;  //To use only an even N tracks for the TVC


     //    pT-ordering the tracks
     std::sort( tvx.tracks.begin(), tvx.tracks.end(), ptOrdering);

     float charge = 0, denom = 0;
     for(int itrk=0;  itrk< m_tv_ntrk; itrk++) {
        const xAOD::TrackParticle *tp =  tvx.tracks.at(itrk);
        charge += ( tp->charge()) * std::pow( tp->pt(), m_kappa_TV);
        denom += pow( tp->pt(), m_kappa_TV);
     }
     if(denom != 0) m_tvc = charge/denom;
     m_tv_dist =  tvx.pos;
     m_tv_err =  tvx.err;
     m_tvc_jetPt = charge/jetToTag.pt();



     TLorentzVector chad_k;
     for(size_t itrk=0;  itrk< tvx.tracks.size();  itrk++) {
        const xAOD::TrackParticle *tp =  tvx.tracks.at(itrk);
        TLorentzVector part_k;
        if( itrk == 0)   part_k.SetPtEtaPhiM( tp->pt(), tp->eta(), tp->phi(), 493.667 );   //Kaon mass [MeV]
        else   part_k.SetPtEtaPhiM( tp->pt(), tp->eta(), tp->phi(), 139.570 );   //Pion mass [MeV]
        chad_k += part_k;
     }
     m_tv_mass_kaons = chad_k.M();
   } //check if I have a TV



   //==============================================================
   //          computing the muon variables
   //==============================================================

   std::unique_ptr< xAOD::Muon> myMuon;

   std::vector<ElementLink< xAOD::MuonContainer > > muonsInJet;
   muonsInJet = BTag->auxdata< std::vector<ElementLink< xAOD::MuonContainer > > >(m_muonAssociationName);

   if( muonsInJet.size() == 0 ) {
     ATH_MSG_DEBUG("#BTAG#  Could not find muons associated with name " << m_muonAssociationName);
   }
   else {
     ATH_MSG_DEBUG("#BTAG#  There are "<< muonsInJet.size() <<" associated with the jet");

     for( const auto& muLink : muonsInJet) {
         const xAOD::Muon* mu = *muLink;

         xAOD::Muon* corrMuHelper = 0;
         if( m_muonCorrectionTool->correctedCopy( *mu, corrMuHelper) != CP::CorrectionCode::Ok ) {
            ATH_MSG_WARNING("Cannot apply calibration nor smearing for muons." );
            continue;
         }

         // Make sure that we don't lose it:
         std::unique_ptr< xAOD::Muon > p_corrMu( corrMuHelper );

         // Make all the muon quality cuts...
         xAOD::Muon::Quality quality = m_muonSelectorTool->getQuality(*p_corrMu);
         //if( quality == xAOD::Muon::Tight ) ATH_MSG_INFO("Muon quality is 'Tight'");
         //else if( quality == xAOD::Muon::Medium  ) ATH_MSG_INFO("Muon quality is 'Medium'");
         //else ATH_MSG_INFO("Muon quality is "<<quality);

         //just added this cut
         if( quality > m_muonQualityCut ) continue;

         //cuts on muons:
         if( p_corrMu->muonType() != xAOD::Muon::MuonType::Combined) continue;
         if( p_corrMu->pt() < 5.0 ) continue;
         if( p_corrMu->eta() > 2.7 ) continue;

         TLorentzVector muon = p_corrMu->p4();
         TLorentzVector jet = jetToTag.p4();
         if( muon.DeltaR( jet ) > 0.3 ) continue;

         float chi2=-1;
         int  dof=-1;
         float chi2_match = 999.;
         if( p_corrMu->parameter( chi2, xAOD::Muon::ParamDef::msInnerMatchChi2 )
	          && p_corrMu->parameter(dof, xAOD::Muon::ParamDef::msInnerMatchDOF ) )
         if (dof != 0) chi2_match = chi2/dof;
         if( chi2_match > 5. ) continue;

         //Select the hardest one
         if( myMuon.get() && ( myMuon->pt() > p_corrMu->pt() ) ) {
            continue;
         }

         myMuon = std::move( p_corrMu);
     }
   }


   if( myMuon.get() ) {

     float iso_save = -1;
     if( myMuon->isolation( iso_save,  xAOD::Iso::IsolationType::ptvarcone40 )) m_mu_iso = iso_save;

     TLorentzVector muon = myMuon->p4();
     TLorentzVector jet = jetToTag.p4();
     m_mu_ptRel =  muon.P()*sin( muon.Angle(jet.Vect() + muon.Vect()))/1000.;
     m_mu_ptLong = muon.P()*cos( muon.Angle( jet.Vect() + muon.Vect() ) )/1000.;
     m_mu_jet_dR  = muon.DeltaR( jet );

     double deltaR = 0.08;
     double deltaPt = 0.15*myMuon->pt();
     for( uint ivx=0; ivx< myVector.size(); ivx++) {
        for( uint itrk=0; itrk< myVector.at(ivx).tracks.size(); itrk++) {
          const xAOD::TrackParticle *tp =  myVector.at(ivx).tracks.at(itrk);

          double rtu = myMuon->p4().DeltaR( tp->p4()  );
          double ptu = fabs( myMuon->pt() - tp->pt() );
          if ( rtu < deltaR && ptu<deltaPt  )   {
             deltaR = rtu;
             deltaPt = ptu;
             m_mu_vtx = ivx +1.;
          }
        }
     }
     if( m_mu_vtx < 0) m_mu_vtx = 0.;


     const xAOD::TrackParticle *trackMuon = myMuon->primaryTrackParticle();
     if( trackMuon) m_mu_charge = trackMuon->charge();
   }  else {
      ATH_MSG_DEBUG("#BTAG# No muon passed the selection. ");
   }     //closes if I have myMuon



   //==============================================================
   //                 now the MVA part
   //==============================================================


   int mvaCat = category();

   m_sv_err  = ( m_sv_err>5.)? 5. : m_sv_err;
   m_tv_err  = ( m_tv_err>5.)? 5. : m_tv_err;

   if(mvaCat == JC_SVC_noMu ) { //limit the variables from above...
      m_sv_mass_pions  = ( m_sv_mass_pions>6000.)? 6000. : m_sv_mass_pions;
     m_jc_track_pt  = ( m_jc_track_pt>90000.)? 90000. : m_jc_track_pt;
     m_sv_ntrk  = ( m_sv_ntrk>14.)? 14. : m_sv_ntrk;
     m_sv_dist  = ( m_sv_dist>105.)? 105. : m_sv_dist;
     m_sv_track_pt  = ( m_sv_track_pt>200000.)? 200000. : m_sv_track_pt;
   }
   else if(mvaCat == JC_SVC_incMu ) { //limit the variables from above...
      m_sv_mass_pions  = ( m_sv_mass_pions>6000.)? 6000. : m_sv_mass_pions;
     m_jc_track_pt  = ( m_jc_track_pt>120000.)? 120000. : m_jc_track_pt;
     m_sv_ntrk  = ( m_sv_ntrk>13.)? 13. : m_sv_ntrk;
     m_sv_dist  = ( m_sv_dist>120.)? 120. : m_sv_dist;
     m_mu_ptRel  = ( m_mu_ptRel>20.)? 20. : m_mu_ptRel;
     m_mu_ptLong  = ( m_mu_ptLong>500.)?500. : m_mu_ptLong;
   }
   else if(mvaCat == JC_SVC_TVC_noMu ) { //limit the variables from above...
     m_tv_dist  = ( m_tv_dist>200.)? 200. : m_tv_dist;
     m_tv_ntrk  = ( m_tv_ntrk>10.)? 10. : m_tv_ntrk;
     m_sv_mass_pions  = ( m_sv_mass_pions>6000.)? 6000. : m_sv_mass_pions;
     m_tv_mass_kaons  = ( m_tv_mass_kaons>6000.)? 6000. : m_tv_mass_kaons;
     m_jc_track_pt  = ( m_jc_track_pt>100000.)? 100000. : m_jc_track_pt;
     m_sv_ntrk  = ( m_sv_ntrk>10.)? 10. : m_sv_ntrk;
     m_sv_dist  = ( m_sv_dist>90.)? 90. : m_sv_dist;
     m_sv_track_pt  = ( m_sv_track_pt>250000.)? 250000. : m_sv_track_pt;
   }
   else if(mvaCat == JC_SVC_TVC_incMu ) { //limit the variables from above...
     m_tv_dist  = ( m_tv_dist>200.)? 200. : m_tv_dist;
     m_tv_ntrk  = ( m_tv_ntrk>10.)? 10. : m_tv_ntrk;
     m_sv_mass_pions  = ( m_sv_mass_pions>6000.)? 6000. : m_sv_mass_pions;
     m_tv_mass_kaons  = ( m_tv_mass_kaons>6000.)? 6000. : m_tv_mass_kaons;
     m_jc_track_pt  = ( m_jc_track_pt>120000.)? 120000. : m_jc_track_pt;
     m_sv_ntrk  = ( m_sv_ntrk>10.)? 10. : m_sv_ntrk;
     m_sv_dist  = ( m_sv_dist>90.)? 90. : m_sv_dist;
     m_mu_ptRel  = ( m_mu_ptRel>12.)? 12. : m_mu_ptRel;
     m_mu_ptLong  = ( m_mu_ptLong>400.)? 400. : m_mu_ptLong;
   }
   else if(mvaCat == JC_incMu ) { //limit the variables from above...
     m_ngoodtrk  = ( m_ngoodtrk>28.)? 28. : m_ngoodtrk;
     m_jc_track_pt  = ( m_jc_track_pt>120000.)? 120000. : m_jc_track_pt;
     m_mu_iso = m_mu_iso>700000?700000:m_mu_iso;
     m_mu_ptRel  = ( m_mu_ptRel>20.)? 20. : m_mu_ptRel;
     m_mu_ptLong  = ( m_mu_ptLong>400.)? 400. : m_mu_ptLong;
   }


   PrintVariables();


   if( m_runModus == "reference") {

      BTag->setVariable<float>(m_taggerNameBase, "jet_uncalibrated_pt", m_jet_uPt );
      BTag->setVariable<float>(m_taggerNameBase, "JC_jetPt", m_jc_jetPt);
      BTag->setVariable<float>(m_taggerNameBase, "JC_all_jetPt", m_jc_all_jetPt);
      BTag->setVariable<float>(m_taggerNameBase, "SVC_jetPt", m_svc_jetPt);
      BTag->setVariable<float>(m_taggerNameBase, "TVC_jetPt", m_tvc_jetPt);

      BTag->setVariable<float>(m_taggerNameBase, "JC_all", m_jc_all );
      BTag->setVariable<float>(m_taggerNameBase, "firstGoodTrkPt", m_jc_track_pt );
      BTag->setVariable<float>(m_taggerNameBase, "firstTrkPt_SV", m_sv_track_pt);
      BTag->setVariable<float>(m_taggerNameBase, "massSV_pions", m_sv_mass_pions);
      BTag->setVariable<float>(m_taggerNameBase, "massTV_kaons", m_tv_mass_kaons);

      BTag->setVariable<float>(m_taggerNameBase, "JetCharge", m_jc);
      BTag->setVariable<float>(m_taggerNameBase, "nJCtracks", m_ngoodtrk);

      BTag->setVariable<float>(m_taggerNameBase, "SVC", m_svc);
      BTag->setVariable<float>(m_taggerNameBase, "ntrk_sv", m_sv_ntrk);
      BTag->setVariable<float>(m_taggerNameBase, "dist_SV", m_sv_dist);
      BTag->setVariable<float>(m_taggerNameBase, "err_SV", m_sv_err);

      BTag->setVariable<float>(m_taggerNameBase, "TVC", m_tvc);
      BTag->setVariable<float>(m_taggerNameBase, "ntrk_tv_used", m_tv_ntrk);
      BTag->setVariable<float>(m_taggerNameBase, "dist_TV", m_tv_dist);
      BTag->setVariable<float>(m_taggerNameBase, "err_TV", m_tv_err);

      BTag->setVariable<float>(m_taggerNameBase, "mu_charge", m_mu_charge);
      BTag->setVariable<float>(m_taggerNameBase, "mu_ptRel", m_mu_ptRel);
      BTag->setVariable<float>(m_taggerNameBase, "mu_ptLong", m_mu_ptLong);
      BTag->setVariable<float>(m_taggerNameBase, "mu_iso_ptvar40", m_mu_iso);
      BTag->setVariable<float>(m_taggerNameBase, "mu_jet_dR", m_mu_jet_dR);
      BTag->setVariable<float>(m_taggerNameBase, "mu_vtx", m_mu_vtx);

      BTag->setVariable<int>(m_taggerNameBase, "category", mvaCat );
      BTag->setVariable<double>(m_taggerNameBase, "discriminant", -7. );

   }
   else if( m_runModus == "analysis") {

     double llr;
     if(mvaCat == JC_noMu ) {
       llr = logLikelihoodRatio( JC_noMu, m_jc , author, alias);
       BTag->setVariable<double>(m_taggerNameBase, "discriminant", llr );
       return StatusCode::SUCCESS;
     }
     else if(mvaCat == JC_all ) {
       llr = logLikelihoodRatio( JC_all, m_jc_all , author, alias);
       BTag->setVariable<double>(m_taggerNameBase, "discriminant", llr );
       return StatusCode::SUCCESS;
     }
     else if(mvaCat == SVC ) {
       llr = logLikelihoodRatio( SVC, m_svc , author, alias);
       BTag->setVariable<double>(m_taggerNameBase, "discriminant", llr );
       return StatusCode::SUCCESS;
     }
     else if(mvaCat < 0 ) {   //NULL cat
       BTag->setVariable<double>(m_taggerNameBase, "discriminant", -7. );
       return StatusCode::SUCCESS;
     }

     std::pair<TList*, bool> calib = m_calibrationTool->retrieveTObject<TList>(m_taggerNameBase, author, m_taggerNameBase + "Calib_cat_" + m_catNames[mvaCat]);
     // the bool indicates whether a calibration has "changed". This is the case in particular when retrieving it for the first time.
     if (calib.second) {
       StatusCode sc = SetupReaders(author, alias, mvaCat, calib.first );
       if( sc.isFailure() ) {
	 ATH_MSG_DEBUG("#BTAG# Could not setup the reader for the " << m_catNames[mvaCat] << " category and author " << author);
	 return StatusCode::SUCCESS;
       } else ATH_MSG_DEBUG("#BTAG# Reader correctly setup for the " << m_catNames[mvaCat] << " category and author " << author);
     }


     //Finally compute the weight
     float mvaWeight = -9.;
     std::string reader_name = alias+m_catNames[mvaCat];
     std::map<std::string, TMVA::Reader*>::iterator pos2 = m_tmvaReaders.find(reader_name);
     if(pos2==m_tmvaReaders.end()) {
       int alreadyWarned = std::count(m_undefinedReaders.begin(),m_undefinedReaders.end(),reader_name);
       if(0==alreadyWarned) {
         ATH_MSG_WARNING("#BTAG# no TMVAReader defined for jet collection " << reader_name);
         m_undefinedReaders.push_back(reader_name);
       }
     }
     else {
       std::map<std::string, TMVA::MethodBase*>::iterator itmap2 = m_tmvaMethod.find(reader_name);
       if((itmap2->second)!=0){
         mvaWeight = pos2->second->EvaluateMVA( itmap2->second );
       } else ATH_MSG_WARNING("#BTAG#  kl==0");
     }


    //Now I compute the log-likelihood ratio
    llr = logLikelihoodRatio( mvaCat, mvaWeight , author, alias);
    BTag->setVariable<double>(m_taggerNameBase, "discriminant", llr );


    }  //if runmodus Analysis


    return StatusCode::SUCCESS;

}

//===============================================================
//                   Helper functions
//===============================================================

bool JetVertexCharge::passTrackCuts( const xAOD::TrackParticle &track) const {


   double d0 = track.d0();
   double z0 = track.z0();
   double theta = track.theta();
   if (track.numberDoF() == 0) return false;
   double chi2 = track.chiSquared() / track.numberDoF();
   double deltaZ0 = fabs( z0 - m_primVtx->z() + track.vz() );


   if( fabs(d0) > m_Trkd0Cut)  return false;
   if( deltaZ0*sin(theta) > m_Trkz0Cut) return false;
   if( track.pt() < m_TrkPtCut) return false;
   if( fabs(track.eta()) > 2.5 ) return false;
   if( chi2 > m_TrkChi2Cut) return false;



   uint8_t PixelHits = 0;
   if( !(track.summaryValue( PixelHits, xAOD::numberOfPixelHits)) || (PixelHits < m_CutPixelHits)  )  return false;
   uint8_t SCTHits = 0;
   if( !(track.summaryValue( SCTHits, xAOD::numberOfSCTHits)) || (SCTHits < m_CutSCTHits)  )  return false;

   if( (PixelHits + SCTHits) < m_CutPrecisionHits) return false;

   uint8_t TRTHits = 0;
   if( !(track.summaryValue( TRTHits, xAOD::numberOfTRTHits)) || (TRTHits < m_CutTRTHits)  )  return false;

   uint8_t IBLHits = 0;
   if( !(track.summaryValue( IBLHits, xAOD::numberOfInnermostPixelLayerHits)) || (IBLHits < m_CutIBLHits)  )  return false;
   uint8_t IBLSharedHits = 0;
   if( !(track.summaryValue( IBLSharedHits, xAOD::numberOfInnermostPixelLayerSharedHits))) return false;
   uint8_t PixelSharedHits = 0;
   if( !(track.summaryValue( PixelSharedHits, xAOD::numberOfPixelSharedHits)))  return false;
   uint8_t SCTSharedHits = 0;
   if( !(track.summaryValue( SCTSharedHits, xAOD::numberOfSCTSharedHits)))  return false;

   if( ( IBLSharedHits + PixelSharedHits + SCTSharedHits )  > m_CutSharedHits) return false;


   return true;

}

//===============================================================================

int JetVertexCharge::category() {

   if( m_jc > -2 && m_svc > -2 && m_tvc <-2 && fabs(m_mu_charge) <0.5 ) return JC_SVC_noMu;
   else if( m_jc > -2 && m_svc > -2 && m_tvc < -2 && fabs(m_mu_charge) >0.5 ) return JC_SVC_incMu;
   else if( m_jc > -2 && m_svc > -2 && m_tvc > -2 && fabs(m_mu_charge) < 0.5 ) return JC_SVC_TVC_noMu;
   else if( m_jc > -2 && m_svc > -2 && m_tvc > -2 && fabs(m_mu_charge) > 0.5 ) return JC_SVC_TVC_incMu;
   else if( m_jc > -2 && m_svc < -2 && fabs(m_mu_charge) > 0.5 ) return JC_incMu;
   else if( m_jc > -2 && m_svc < -2 && fabs(m_mu_charge) < 0.5 ) return JC_noMu;
   else if( m_jc < -2 && m_svc > -2 ) return SVC;
   else if( m_jc < -2 && m_jc_all > -2  ) return JC_all;
   else return -1;

}

//////////////////////////////////////////////////////
float  JetVertexCharge::logLikelihoodRatio( int mvaCat, float mvaWeight, std::string author, std::string alias)  {


  std::pair<TH1*,bool> histo_pos;
  std::pair<TH1*,bool> histo_neg;

  if( mvaCat == JC_noMu) {
    histo_pos = m_calibrationTool->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_noMu_bbar");
    histo_neg = m_calibrationTool->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_noMu_b");
  }
  else if( mvaCat == JC_SVC_noMu) {
    histo_pos = m_calibrationTool->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_SVC_noMu_bbar");
    histo_neg = m_calibrationTool->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_SVC_noMu_b");
  }
  else if( mvaCat == JC_SVC_incMu) {
    histo_pos = m_calibrationTool->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_SVC_incMu_bbar");
    histo_neg = m_calibrationTool->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_SVC_incMu_b");
  }
  else if( mvaCat == JC_SVC_TVC_noMu) {
    histo_pos = m_calibrationTool->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_SVC_TVC_noMu_bbar");
    histo_neg = m_calibrationTool->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_SVC_TVC_noMu_b");
  }
  else if( mvaCat == JC_incMu) {
    histo_pos = m_calibrationTool->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_incMu_bbar");
    histo_neg = m_calibrationTool->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_incMu_b");
  }
  else if( mvaCat == JC_SVC_TVC_incMu) {
    histo_pos = m_calibrationTool->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_SVC_TVC_incMu_bbar");
    histo_neg = m_calibrationTool->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_SVC_TVC_incMu_b");
  }
  else if( mvaCat == SVC) {
    histo_pos = m_calibrationTool->retrieveHistogram(m_taggerNameBase, author, "jvc_SVC_bbar");
    histo_neg = m_calibrationTool->retrieveHistogram(m_taggerNameBase, author, "jvc_SVC_b");
  }
  else if( mvaCat == JC_all) {
    histo_pos = m_calibrationTool->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_all_bbar");
    histo_neg = m_calibrationTool->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_all_b");
  }
  else if(mvaCat == -1) {
    return -7;
  }

  bool histosHaveChanged = (histo_pos.second || histo_neg.second);
  if(histosHaveChanged) {

    if( histo_pos.first ==NULL ) {
      ATH_MSG_WARNING("#BTAG# BBAR HISTO can't be retrieved -> no calibration for "<< m_taggerNameBase );
      return -3.;
    }
    if(  histo_neg.first==NULL) {
      ATH_MSG_WARNING("#BTAG# B HISTO can't be retrieved -> no calibration for "<< m_taggerNameBase );
      return -3;
    }

    if( mvaCat == JC_noMu) {
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, "jvc_JC_noMu_bbar", false);
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, "jvc_JC_noMu_b", false);
    }
    else if( mvaCat == JC_SVC_noMu) {
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, "jvc_JC_SVC_noMu_bbar", false);
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, "jvc_JC_SVC_noMu_b", false);
    }
    else if( mvaCat == JC_SVC_incMu) {
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, "jvc_JC_SVC_incMu_bbar", false);
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, "jvc_JC_SVC_incMu_b", false);
    }
    else if( mvaCat == JC_SVC_TVC_noMu) {
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, "jvc_JC_SVC_TVC_noMu_bbar", false);
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, "jvc_JC_SVC_TVC_noMu_b", false);
    }
    else if( mvaCat == JC_SVC_TVC_incMu) {
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, "jvc_JC_SVC_TVC_incMu_bbar", false);
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, "jvc_JC_SVC_TVC_incMu_b", false);
    }
    else if( mvaCat == SVC) {
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, "jvc_SVC_bbar", false);
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, "jvc_SVC_b", false);
    }
    else if( mvaCat == JC_all) {
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, "jvc_JC_all_bbar", false);
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, "jvc_JC_all_b", false);
    }
    else if( mvaCat == JC_incMu) {
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, "jvc_JC_incMu_bbar", false);
      m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, "jvc_JC_incMu_b", false);
    }


    TH1F* hp = (TH1F*) histo_pos.first;
    TH1F* hn = (TH1F*) histo_neg.first;

    std::map<int, TH1F*>::iterator pos;
    std::map<int, TH1F*>::iterator neg;

    pos = m_histoList_pos.find(mvaCat);
    if(pos!=m_histoList_pos.end()) {
      m_histoList_pos.erase(pos);
    }
    neg = m_histoList_neg.find(mvaCat);
    if(neg!=m_histoList_neg.end()) {
      m_histoList_neg.erase(neg);
    }

    m_histoList_pos.insert( std::make_pair( mvaCat, hp ) );
    m_histoList_neg.insert( std::make_pair( mvaCat, hn ) );

  }  //if something changed


  float histo_bbar = minProb, histo_b = minProb;

  std::map<int, TH1F*>::iterator itmap_pos = m_histoList_pos.find(mvaCat);
  if (itmap_pos != m_histoList_pos.end() && itmap_pos->second != 0) {
    TH1F * hhh  = (TH1F*) itmap_pos->second;
    int bin = hhh->FindBin( mvaWeight );
    histo_bbar = std::max(hhh->GetBinContent( bin ), minProb);
  } else {
    ATH_MSG_DEBUG("#BTAG# BBAR HISTO for category " << categoryToString(mvaCat) << " not found -- no meaningful LLR calculation is possible");
  }

  std::map<int, TH1F*>::iterator itmap_neg = m_histoList_neg.find(mvaCat);
  if (itmap_neg != m_histoList_neg.end() && itmap_neg->second != 0) {
    TH1F * hhh  = (TH1F*) itmap_neg->second;
    int bin = hhh->FindBin( mvaWeight );
    histo_b = std::max(hhh->GetBinContent( bin ), minProb);
  } else {
    ATH_MSG_DEBUG("#BTAG# B HISTO for category " << categoryToString(mvaCat) << " not found -- no meaningful LLR calculation is possible");
  }

  ATH_MSG_DEBUG("#BTAG# probabilities for category " << categoryToString(mvaCat) << " and weight " << mvaWeight << ": "
		<< histo_bbar << "(bbar), " << histo_b << "(b)");
  return log( histo_bbar/histo_b);
}


StatusCode JetVertexCharge::SetupReaders( std::string /*author*/, std::string alias , int mvaCat, TList* list) {

   ATH_MSG_DEBUG("#BTAG# setting up reader for category "<<mvaCat);


   // std::pair<TList*, bool> calib;

   // if( mvaCat == JC_SVC_noMu ) calib = m_calibrationTool->retrieveTObject<TList>(m_taggerNameBase, author, m_taggerNameBase+"Calib_cat_JC_SVC_noMu");
   // else if(mvaCat == JC_SVC_incMu ) calib = m_calibrationTool->retrieveTObject<TList>(m_taggerNameBase, author, m_taggerNameBase+"Calib_cat_JC_SVC_incMu");
   // else if(mvaCat == JC_SVC_TVC_noMu ) calib = m_calibrationTool->retrieveTObject<TList>(m_taggerNameBase, author, m_taggerNameBase+"Calib_cat_JC_SVC_TVC_noMu");
   // else if(mvaCat == JC_SVC_TVC_incMu ) calib = m_calibrationTool->retrieveTObject<TList>(m_taggerNameBase, author, m_taggerNameBase+"Calib_cat_JC_SVC_TVC_incMu");
   // else if(mvaCat == JC_incMu ) calib = m_calibrationTool->retrieveTObject<TList>(m_taggerNameBase, author, m_taggerNameBase+"Calib_cat_JC_incMu");


   // bool calibHasChanged = calib.second;
   // if(calibHasChanged) {

   ATH_MSG_DEBUG("#BTAG# " << m_taggerNameBase << " calib updated -> try to retrieve");
   // if(!calib.first) {
   if (! list) {
     ATH_MSG_WARNING("#BTAG# Tlist can't be retrieved -> no calibration for "<< m_taggerNameBase );
     return StatusCode::FAILURE;
   }


   m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, m_taggerNameBase+"Calib_cat_" + m_catNames[mvaCat], false);
      // if( mvaCat == JC_SVC_noMu) m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, m_taggerNameBase+"Calib_cat_JC_SVC_noMu", false);
      // else if( mvaCat == JC_SVC_incMu) m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, m_taggerNameBase+"Calib_cat_JC_SVC_incMu", false);
      // else if( mvaCat == JC_SVC_TVC_noMu) m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, m_taggerNameBase+"Calib_cat_JC_SVC_TVC_noMu", false);
      // else if( mvaCat == JC_SVC_TVC_incMu) m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, m_taggerNameBase+"Calib_cat_JC_SVC_TVC_incMu", false);
      // else if( mvaCat == JC_incMu) m_calibrationTool->updateHistogramStatus(m_taggerNameBase, alias, m_taggerNameBase+"Calib_cat_JC_incMu", false);

      // //now the new part istringstream
      // TList* list = calib.first;

   // Note: the variables and their ranges (to be used in TMVA::Reader::AddVariable() calls) are extracted from the XML object itself.
   // This is somewhat fragile, as it assumes that the expressions used are either simple variable names or expressions implementing a minimum or maximum bound, like
   //         "(distSV&gt;105.)?105.:distSV"
   // where it is assumed that the actual variable name follows the ":", and the "&gt;" is an XML representation of ">" (and hence needs to be replaced with the latter)

   std::vector<std::string> inputVars;
   std::ostringstream iss;
   for(int i=0; i<list->GetSize(); ++i) {
     TObjString* ss = (TObjString*)list->At(i);
     std::string sss = ss->String().Data();
     //KM: if it doesn't find "<" in the string, it starts from non-space character
     std::string::size_type posi = sss.find('<')!=std::string::npos ? sss.find('<') : sss.find_first_not_of(" ");
     std::string tmp = sss.erase(0,posi);
     iss << tmp.data();
     if (tmp.find("<Variable")!=std::string::npos ) {
       if ( tmp.find("Variable VarIndex")!=std::string::npos ) {
	 // Retrieve the original expression for use with a new TMVA::Reader object
	 std::string varIndex  =tmp.substr(tmp.find("=\"")+2, tmp.find("\" ")-(tmp.find("=\"")+2));
	 std::string tmpVar  = tmp.erase(0,tmp.find("Expression=\"")+12);
	 std::string varExpress=tmp.substr(0, tmp.find("\""));
	 posi = varExpress.find("&lt;");
	 while (posi != std::string::npos) {
	   varExpress.replace(posi, 4, "<");
	   posi = varExpress.find("&lt;");
	 }
	 posi = varExpress.find("&gt;");
	 while (posi != std::string::npos) {
	   varExpress.replace(posi, 4, ">");
	   posi = varExpress.find("&gt;");
	 }
	 posi = varExpress.find("&amp;");
	 while (posi != std::string::npos) {
	   varExpress.replace(posi, 5, "&");
	   posi = varExpress.find("&amp;");
	 }
	 inputVars.push_back(varExpress);
       }
     }
   }




   // now configure the TMVAReaders:
   TMVA::Reader* tmvaReader = new TMVA::Reader();

   std::vector<float*> varptrs;
   for (auto expression : inputVars) {
     std::string var = expression.find_last_of(":") == std::string::npos ? expression : expression.substr(expression.find_last_of(":")+1);
     if (m_variablePtr.find(var) == m_variablePtr.end()) {
       ATH_MSG_WARNING("#BTAG#  cannot interpret variable name " << var << " in category " << m_catNames[mvaCat] << " for tagger instance " << m_taggerNameBase);
       delete tmvaReader;
       return StatusCode::FAILURE;
     }
     ATH_MSG_DEBUG("#BTAG# adding variable " << var << " in category " << m_catNames[mvaCat] << " for tagger instance " << m_taggerNameBase);
     tmvaReader->AddVariable(expression.c_str(), m_variablePtr[var]);
   }

   // if( mvaCat == JC_SVC_noMu ) {
   //   tmvaReader->AddVariable( "JC",		&m_jc );
   //   tmvaReader->AddVariable( "SVC", 	&m_svc );
   //   tmvaReader->AddVariable( "(track_good_pt>90000.)?90000.:track_good_pt", 	&m_jc_track_pt );
   //   tmvaReader->AddVariable( "(ntrk0>14.)?14.:ntrk0",	&m_sv_ntrk );
   //   tmvaReader->AddVariable( "(distSV>105.)?105.:distSV",  	&m_sv_dist );
   //   tmvaReader->AddVariable( "(errSV>5.)?5.:errSV", 	&m_sv_err );
   //   tmvaReader->AddVariable( "(track_sv_pt>200000.)?200000.:track_sv_pt", 	&m_sv_track_pt );
   // }
   // else if( mvaCat == JC_SVC_incMu ) {
   //   tmvaReader->AddVariable( "JC",	&m_jc );
   //   tmvaReader->AddVariable( "SVC", 	&m_svc );
   //   tmvaReader->AddVariable( "mu_charge",  	&m_mu_charge );
   //   tmvaReader->AddVariable( "(track_good_pt>120000.)?120000.:track_good_pt", 	&m_jc_track_pt );
   //   tmvaReader->AddVariable( "(ntrk0>13.)?13.:ntrk0", 	&m_sv_ntrk );
   //   tmvaReader->AddVariable( "(distSV>120.)?120.:distSV", 	&m_sv_dist );
   //   tmvaReader->AddVariable( "(errSV>5.)?5.:errSV",  	&m_sv_err );
   //   tmvaReader->AddVariable( "(mu_ptRel>20.)?20.:mu_ptRel",	&m_mu_ptRel );
   //   tmvaReader->AddVariable( "(mu_ptLong>500.)?500.:mu_ptLong", 	&m_mu_ptlong );
   // }
   // else if( mvaCat == JC_SVC_TVC_noMu ) {
   //   tmvaReader->AddVariable( "JC",	&m_jc );
   //   tmvaReader->AddVariable( "SVC",	&m_svc );
   //   tmvaReader->AddVariable( "TVC",	&m_tvc );
   //   tmvaReader->AddVariable( "(track_good_pt>100000.)?100000.:track_good_pt", &m_jc_track_pt );
   //   tmvaReader->AddVariable( "(ntrk0>10.)?10.:ntrk0", &m_sv_ntrk );
   //   tmvaReader->AddVariable( "(distSV>90.)?90.:distSV", &m_sv_dist );
   //   tmvaReader->AddVariable( "(errSV>5.)?5.:errSV", &m_sv_err );
   //   tmvaReader->AddVariable( "(track_sv_pt>250000.)?250000.:track_sv_pt", &m_sv_track_pt );
   //   tmvaReader->AddVariable( "(massSV_pions>6000.)?6000.:massSV_pions", &m_sv_mass_pions );
   //   tmvaReader->AddVariable( "(ntrk1_used>10.)?10.:ntrk1_used", &m_tv_ntrk );
   //   tmvaReader->AddVariable( "(distTV>200.)?200.:distTV", &m_tv_dist );
   //   tmvaReader->AddVariable( "(errTV>5.)?5.:errTV", &m_tv_err );
   //   tmvaReader->AddVariable( "(massTV_kaons>6000.)?6000.:massTV_kaons", &m_tv_mass_kaons );
   // }
   // else if(mvaCat == JC_SVC_TVC_incMu) {
   //   tmvaReader->AddVariable( "JC",	&m_jc );
   //   tmvaReader->AddVariable( "SVC",	&m_svc );
   //   tmvaReader->AddVariable( "TVC",	&m_tvc );
   //   tmvaReader->AddVariable( "mu_charge",  	&m_mu_charge );
   //   tmvaReader->AddVariable( "(track_good_pt>120000.)?120000.:track_good_pt", &m_jc_track_pt );
   //   tmvaReader->AddVariable( "(ntrk0>10.)?10.:ntrk0", &m_sv_ntrk );
   //   tmvaReader->AddVariable( "(distSV>90.)?90.:distSV", &m_sv_dist );
   //   tmvaReader->AddVariable( "(errSV>5.)?5.:errSV", &m_sv_err );
   //   tmvaReader->AddVariable( "(ntrk1_used>10.)?10.:ntrk1_used", &m_tv_ntrk );
   //   tmvaReader->AddVariable( "(distTV>200.)?200.:distTV", &m_tv_dist );
   //   tmvaReader->AddVariable( "(errTV>5.)?5.:errTV", &m_tv_err );
   //   tmvaReader->AddVariable( "(massTV_kaons>6000.)?6000.:massTV_kaons", &m_tv_mass_kaons );
   //   tmvaReader->AddVariable( "(mu_ptRel>12.)?12.:mu_ptRel",	&m_mu_ptRel );
   //   tmvaReader->AddVariable( "(mu_ptLong>400.)?400.:mu_ptLong",	&m_mu_ptLong );
   // }
   // else if(mvaCat == JC_incMu) {
   //   tmvaReader->AddVariable( "JC",	&m_jc );
   //   tmvaReader->AddVariable( "mu_charge",	&m_mu_charge );
   //   tmvaReader->AddVariable( "(ngoodtrk>28.)?28.:ngoodtrk",	&m_ngoodtrk );
   //   tmvaReader->AddVariable( "(track_good_pt>120000.)?120000.:track_good_pt",	&m_jc_track_pt );
   //   tmvaReader->AddVariable( "(mu_ptRel>20.)?20.:mu_ptRel",	&m_mu_ptRel );
   //   tmvaReader->AddVariable( "(mu_ptLong>400.)?400.:mu_ptLong",	&m_mu_ptLong );
   //   tmvaReader->AddVariable( "(mu_iso_ptvar40>700000.)?700000.:mu_iso_ptvar40",	&m_mu_iso );
   //   tmvaReader->AddVariable( "mu_jet_dR",	&m_mu_jet_dR );
   // }


   TMVA::IMethod* method= tmvaReader->BookMVA(TMVA::Types::kMLP, iss.str().data());
   auto kl = dynamic_cast<TMVA::MethodBase*>(method);

   // add it or overwrite it in the map of readers:
   std::string reader_name = alias+m_catNames[mvaCat];
   auto pos = m_tmvaReaders.find(reader_name);
   if (pos != m_tmvaReaders.end()) {
     delete pos->second;
     m_tmvaReaders.erase(pos);
   }
   auto itmap = m_tmvaMethod.find(reader_name);
   if (itmap != m_tmvaMethod.end()) {
     delete itmap->second;
     m_tmvaMethod.erase(itmap);
   }
   m_tmvaReaders.insert( std::make_pair( reader_name, tmvaReader ) );
   m_tmvaMethod.insert( std::make_pair( reader_name, kl ) );
   //   }  //if calibhaschanged

   return StatusCode::SUCCESS;

}  //closes SetupReaders

void JetVertexCharge::PrintVariables()  {




   ATH_MSG_DEBUG("#BTAG# ===============================================================================");
   ATH_MSG_DEBUG("#BTAG# Printing input variables: ");
   ATH_MSG_DEBUG("#BTAG# JC="<<m_jc<<"  SVC="<<m_svc<<"  TVC="<<m_tvc<<"  mu charge="<<m_mu_charge<<" JC all="<<m_jc_all);
   ATH_MSG_DEBUG("#BTAG# JC_jetPt="<<m_jc_jetPt<<"  SVC_jetPt="<<m_svc_jetPt<<"  TVC_jetPt="<<m_tvc_jetPt<<"  mu charge="<<m_mu_charge<<" JC_all_jetPt="<<m_jc_all_jetPt);
   ATH_MSG_DEBUG("#BTAG# ngood trk="<<m_ngoodtrk<<"  JC pt="<<m_jc_track_pt<<"  SV pt= "<<m_sv_track_pt);
   ATH_MSG_DEBUG("#BTAG# ntrkSV="<<m_sv_ntrk<<"  distSV="<<m_sv_dist<<"  errSV="<<m_sv_err<<" mass SV="<<m_sv_mass_pions);
   ATH_MSG_DEBUG("#BTAG# ntrkTV="<<m_tv_ntrk<<"  distTV="<<m_tv_dist<<"  errTV="<<m_tv_err<<" mass TV="<<m_tv_mass_kaons);
   ATH_MSG_DEBUG("#BTAG# mu ptRel="<<m_mu_ptRel<<"  mu_ptLong="<<m_mu_ptLong<<"  mu dR="<< m_mu_jet_dR<<"  mu_isolation="<<m_mu_iso<<"  mu_vtx="<<m_mu_vtx);
   ATH_MSG_DEBUG("#BTAG# ===============================================================================");

}

std::string JetVertexCharge::categoryToString(int cat) const {
  auto it = m_catNames.find(cat);
  if (it != m_catNames.end()) return it->second;

  // this is for the non-MVA categories
  switch (cat) {
  case JC_noMu:
    return "JC_noMu";
  case SVC:
    return "SVC";
  case JC_all:
    return "JC_all";
  default:
    return "unknown";
  };
}

void JetVertexCharge::initializeVariablePtrs() {
  // The only purpose of this method is to determine the mapping from string to member variable pointer.
  m_variablePtr["mu_ptRel"]      = &m_mu_ptRel;
  m_variablePtr["mu_ptLong"]     = &m_mu_ptLong;
  m_variablePtr["mu_charge"]     = &m_mu_charge;
  m_variablePtr["mu_jet_dR"]     = &m_mu_jet_dR;
  m_variablePtr["mu_iso_ptvar40"]= &m_mu_iso;
  m_variablePtr["TVC"]           = &m_tvc;
  m_variablePtr["distTV"]        = &m_tv_dist;
  m_variablePtr["errTV"]         = &m_tv_err;
  m_variablePtr["massTV_kaons"]  = &m_tv_mass_kaons;
  m_variablePtr["ntrk1_used"]    = &m_tv_ntrk;
  m_variablePtr["SVC"]           = &m_svc;
  m_variablePtr["distSV"]        = &m_sv_dist;
  m_variablePtr["errSV"]         = &m_sv_err;
  m_variablePtr["massSV_pions"]  = &m_sv_mass_pions;
  m_variablePtr["ntrk0"]         = &m_sv_ntrk;
  m_variablePtr["track_sv_pt"]   = &m_sv_track_pt;
  m_variablePtr["JC"]            = &m_jc;
  m_variablePtr["track_good_pt"] = &m_jc_track_pt;
  m_variablePtr["ngoodtrk"]      = &m_ngoodtrk;
  m_variablePtr["jet_uncalibrated_pt"]      = &m_jet_uPt;
  m_variablePtr["JC_jetPt"]      = &m_jc_jetPt;
  m_variablePtr["JC_all_jetPt"]  = &m_jc_all_jetPt;
  m_variablePtr["SVC_jetPt"]     = &m_svc_jetPt;
  m_variablePtr["TVC_jetPt"]     = &m_tvc_jetPt;
}

void JetVertexCharge::ClearVars()  {

  m_jet_uPt = -999.;
  m_jc_jetPt = -999.;
  m_jc_all_jetPt = -999.;
  m_svc_jetPt = -9.;
  m_tvc_jetPt = -9.;

  m_jc = -3;
  m_jc_all = -3;
  m_svc = -3;
  m_tvc = -3;

  m_ngoodtrk = 0;
  m_jc_track_pt = -999.;

  m_sv_ntrk = -1.;
  m_sv_dist = -999.;
  m_sv_err = -999.;

  m_sv_track_pt = -999.;
  m_sv_mass_pions = -999.;

  m_tv_ntrk = -1.;
  m_tv_dist = -999.;
  m_tv_err = -999.;
  m_tv_mass_kaons = -999.;

  m_mu_charge = 0.;
  m_mu_ptRel = -999.;
  m_mu_ptLong = -999.;
  m_mu_jet_dR = -1.;
  m_mu_iso = -1.;
  m_mu_vtx = -1;

}



}  //End of namespace
