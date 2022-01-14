/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "MuonAnalysisInterfaces/IMuonSelectionTool.h" 
#include "MuonAnalysisInterfaces/IMuonCalibrationAndSmearingTool.h"
#include "TMVA/Reader.h"
#include "TList.h"
#include "TObjString.h"

#include "TMVA/MethodBase.h"
#include "TH1.h"

#include "xAODBTagging/BTagVertex.h"

const double minProb = 1e-9;

namespace Analysis { 


  JetVertexCharge::JetVertexCharge(const std::string& t, const std::string& n, const IInterface*  p) :
    base_class(t,n,p),
    m_muonSelectorTool("JVC_MuonSelectorTool", this),
    m_muonCorrectionTool( "JVC_MuonCorrectionTool", this),
    m_runModus("analysis")
  { 

    declareProperty("SecVxFinderName",		m_secVxFinderName);
    declareProperty("Runmodus",                 m_runModus);

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

  }
  
///////////
//Destructor
  JetVertexCharge::~JetVertexCharge() { }
  

///////////
//Initialize method
  StatusCode JetVertexCharge::initialize() {

    //Retrieve the Muon Selectot tool
    StatusCode sc = m_muonSelectorTool.retrieve();
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

    // prepare readKey for calibration data:
    ATH_CHECK(m_readKey.initialize());

    m_catNames[JC_SVC_noMu] = "JC_SVC_noMu";
    m_catNames[JC_SVC_incMu] = "JC_SVC_incMu";
    m_catNames[JC_SVC_TVC_noMu] = "JC_SVC_TVC_noMu";
    m_catNames[JC_SVC_TVC_incMu] = "JC_SVC_TVC_incMu";
    m_catNames[JC_incMu] = "JC_incMu";

    return StatusCode::SUCCESS;
  }
  

///////////
//Finalize method
  StatusCode JetVertexCharge::finalize()
  {
    ATH_MSG_DEBUG( "Finalize successful" );
    return StatusCode::SUCCESS;
  } 


//////////////////////////////////////////////////////////////////  
StatusCode JetVertexCharge::tagJet( const xAOD::Vertex& priVtx,
                                    const xAOD::Jet& jetToTag,
                                    xAOD::BTagging& BTag,
                                    const std::string &jetName) const
{
    //Retrieval of Calibration Condition Data objects
    SG::ReadCondHandle<JetTagCalibCondData> readCdo(m_readKey);


    /** author to know which jet algorithm: */ 
    std::string author = jetName;
    if (m_doForcedCalib) author = m_ForcedCalibName;
    std::string alias = readCdo->getChannelAlias(author);

    Vars vars;

    vars[Vars::JET_UNCALIBRATED_PT] = jetToTag.pt();

    //          computing the JetCharge (JC) 
    //==============================================================

    std::vector<ElementLink< xAOD::TrackParticleContainer > > tracksInJet;        
    tracksInJet = BTag.auxdata< std::vector<ElementLink< xAOD::TrackParticleContainer > > >(m_trackAssociationName);
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
          
         if( !passTrackCuts(priVtx, *tp)) continue; 
         if( tp->pt() >  minpt)  { 
            vars[Vars::TRACK_GOOD_PT] = tp->pt();
            minpt = tp->pt();
         } 
 	 vars[Vars::NGOODTRK]++;
         charge += ( tp->charge()) * std::pow( tp->pt(), m_kappa ); 
         denom += pow( tp->pt(), m_kappa);
      }

      if(denom != 0) vars[Vars::JC] = charge / denom;
      if(denom_all != 0) vars[Vars::JC_ALL]= charge_all / denom_all;
      vars[Vars::JC_JETPT] = charge/jetToTag.pt();
      vars[Vars::JC_ALL_JETPT] = charge_all/jetToTag.pt();


    }


   //==============================================================
   //          computing the Vertex Charges (SVC/TVC) 
   //==============================================================

  
   std::vector< myVtxInfo > myVector; 



   std::vector<ElementLink<xAOD::BTagVertexContainer> >  JFVerticesLinks; 
   bool ret = BTag.variable<std::vector<ElementLink<xAOD::BTagVertexContainer>>>("JetFitter", "JFvertices", JFVerticesLinks );
   if(ret) {
      if( JFVerticesLinks.size() ==0) { 
	ATH_MSG_DEBUG("#BTAG# No JF vertices ");  
      } else {  

	ATH_MSG_DEBUG("#BTAG# There are "<<JFVerticesLinks.size()<<" JF vertices ");  

        std::vector<float> fittedPosition;
        std::vector< float > fittedCov; 
        BTag.variable<std::vector< float > >("JetFitter", "fittedPosition", fittedPosition);
        BTag.variable<std::vector< float > >("JetFitter", "fittedCov", fittedCov);
          


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
      vars[Vars::NTRK0] = svx.tracks.size(); 


      float charge = 0, denom = 0;
      float ptmin = 0;
      for(int itrk=0;  itrk< vars[Vars::NTRK0]; itrk++) {
         const xAOD::TrackParticle *tp =  svx.tracks.at(itrk);
         if( tp->pt() >  ptmin)  {
            vars[Vars::TRACK_SV_PT] = tp->pt();
            ptmin = tp->pt();
         } 
         charge += ( tp->charge()) * std::pow( tp->pt(), m_kappa_SV); 
         denom += pow( tp->pt(), m_kappa_SV);
      }
      if(denom != 0)  vars[Vars::SVC] = charge/denom;
      vars[Vars::SVC_JETPT] = charge/jetToTag.pt();

      vars[Vars::DISTSV] =  svx.pos;
      vars[Vars::ERRSV] =  svx.err;

      //Computing the mass
      TLorentzVector bhad;
      for(int itrk=0;  itrk< vars[Vars::NTRK0]; itrk++) {
         const xAOD::TrackParticle *tp =  svx.tracks.at(itrk);
         TLorentzVector part; part.SetPtEtaPhiM( tp->pt(), tp->eta(), tp->phi(), 139.570 );   //Pion mass [MeV]
         bhad += part;
      }
      vars[Vars::MASSSV_PIONS] = bhad.M();
   }


   //-----------------------------------------------------------------

   if( myVector.size() >1 ) {  //Let's work with the TV
      myVtxInfo tvx = myVector.at(1);
      //Use only an even N tracks for the TVC
      vars[Vars::NTRK1_USED] = (tvx.tracks.size() & ~1u);


     //    pT-ordering the tracks 
     std::sort( tvx.tracks.begin(), tvx.tracks.end(), ptOrdering);

     float charge = 0, denom = 0;
     for(int itrk=0;  itrk< vars[Vars::NTRK1_USED]; itrk++) {
        const xAOD::TrackParticle *tp =  tvx.tracks.at(itrk);
        charge += ( tp->charge()) * std::pow( tp->pt(), m_kappa_TV); 
        denom += pow( tp->pt(), m_kappa_TV);
     }
     if(denom != 0) vars[Vars::TVC] = charge/denom;
     vars[Vars::DISTTV] =  tvx.pos;
     vars[Vars::ERRTV] =  tvx.err;
     vars[Vars::TVC_JETPT] = charge/jetToTag.pt();



     TLorentzVector chad_k;
     for(size_t itrk=0;  itrk< tvx.tracks.size();  itrk++) {
        const xAOD::TrackParticle *tp =  tvx.tracks.at(itrk);
        TLorentzVector part_k;
        if( itrk == 0)   part_k.SetPtEtaPhiM( tp->pt(), tp->eta(), tp->phi(), 493.667 );   //Kaon mass [MeV]
        else   part_k.SetPtEtaPhiM( tp->pt(), tp->eta(), tp->phi(), 139.570 );   //Pion mass [MeV]
        chad_k += part_k;
     }
     vars[Vars::MASSTV_KAONS] = chad_k.M();
   } //check if I have a TV



   //==============================================================
   //          computing the muon variables 
   //==============================================================
 
   std::unique_ptr< xAOD::Muon> myMuon;

   std::vector<ElementLink< xAOD::MuonContainer > > muonsInJet;       
   muonsInJet = BTag.auxdata< std::vector<ElementLink< xAOD::MuonContainer > > >(m_muonAssociationName); 

   if( muonsInJet.size() == 0 ) {
     ATH_MSG_DEBUG("#BTAG#  Could not find muons associated with name " << m_muonAssociationName);
   } 
   else {
     ATH_MSG_DEBUG("#BTAG#  There are "<< muonsInJet.size() <<" associated with the jet");

     for( const auto& muLink : muonsInJet) {
         const xAOD::Muon* mu = *muLink;

         xAOD::Muon* corrMuHelper = nullptr;
         if( m_muonCorrectionTool->correctedCopy( *mu, corrMuHelper) != CP::CorrectionCode::Ok ) {
            ATH_MSG_WARNING("Cannot apply calibration nor smearing for muons." ); 
            continue;
         }

         // Make sure that we don't lose it:
         std::unique_ptr< xAOD::Muon > p_corrMu( corrMuHelper );
   
         // Make all the muon quality cuts...
         xAOD::Muon::Quality quality = m_muonSelectorTool->getQuality(*p_corrMu);

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
     if( myMuon->isolation( iso_save,  xAOD::Iso::IsolationType::ptvarcone40 )){
       vars[Vars::MU_ISO_PTVAR40] = iso_save;
     }

     TLorentzVector muon = myMuon->p4();      
     TLorentzVector jet = jetToTag.p4();      
     vars[Vars::MU_PTREL] =  muon.P()*sin( muon.Angle(jet.Vect() + muon.Vect()))/1000.; 
     vars[Vars::MU_PTLONG] = muon.P()*cos( muon.Angle( jet.Vect() + muon.Vect() ) )/1000.;
     vars[Vars::MU_JET_DR]  = muon.DeltaR( jet );

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
             vars[Vars::MU_VTX] = ivx +1.;
          }
        }
     }
     if( vars[Vars::MU_VTX] < 0) vars[Vars::MU_VTX] = 0.; 


     const xAOD::TrackParticle *trackMuon = myMuon->primaryTrackParticle();
     if( trackMuon) vars[Vars::MU_CHARGE] = trackMuon->charge(); 
   }  else {
      ATH_MSG_DEBUG("#BTAG# No muon passed the selection. ");
   }     //closes if I have myMuon
        


   //==============================================================
   //                 now the MVA part  
   //==============================================================


   int mvaCat = vars.category(); 

   vars.clip (Vars::ERRSV, 5);
   vars.clip (Vars::ERRTV, 5);

   if(mvaCat == JC_SVC_noMu ) { //limit the variables from above...
     vars.clip (Vars::MASSSV_PIONS, 6000);
     vars.clip (Vars::TRACK_GOOD_PT, 90000);
     vars.clip (Vars::NTRK0, 14);
     vars.clip (Vars::DISTSV, 105);
     vars.clip (Vars::TRACK_SV_PT, 200000);
   } 
   else if(mvaCat == JC_SVC_incMu ) { //limit the variables from above... 
     vars.clip (Vars::MASSSV_PIONS, 6000);
     vars.clip (Vars::TRACK_GOOD_PT, 120000);
     vars.clip (Vars::NTRK0, 13);
     vars.clip (Vars::DISTSV, 120);
     vars.clip (Vars::MU_PTREL, 20);
     vars.clip (Vars::MU_PTLONG, 500);
   } 
   else if(mvaCat == JC_SVC_TVC_noMu ) { //limit the variables from above...
     vars.clip (Vars::DISTTV, 200);
     vars.clip (Vars::NTRK1_USED, 10);
     vars.clip (Vars::MASSSV_PIONS, 6000);
     vars.clip (Vars::MASSTV_KAONS, 6000);
     vars.clip (Vars::TRACK_GOOD_PT, 100000);
     vars.clip (Vars::NTRK0, 10);
     vars.clip (Vars::DISTSV, 90);
     vars.clip (Vars::TRACK_SV_PT, 250000);
   } 
   else if(mvaCat == JC_SVC_TVC_incMu ) { //limit the variables from above... 
     vars.clip (Vars::DISTTV, 200);
     vars.clip (Vars::NTRK1_USED, 10);
     vars.clip (Vars::MASSSV_PIONS, 6000);
     vars.clip (Vars::MASSTV_KAONS, 6000);
     vars.clip (Vars::TRACK_GOOD_PT, 120000);
     vars.clip (Vars::NTRK0, 10);
     vars.clip (Vars::DISTSV, 90);
     vars.clip (Vars::MU_PTREL, 12);
     vars.clip (Vars::MU_PTLONG, 400);
   } 
   else if(mvaCat == JC_incMu ) { //limit the variables from above...  
     vars.clip (Vars::NGOODTRK, 28);
     vars.clip (Vars::TRACK_GOOD_PT, 120000);
     vars.clip (Vars::MU_ISO_PTVAR40, 700000);
     vars.clip (Vars::MU_PTREL, 20);
     vars.clip (Vars::MU_PTLONG, 400);
   }


   vars.print(msg());

   
   if( m_runModus == "reference") {

      BTag.setVariable<float>(m_taggerNameBase, "jet_uncalibrated_pt", vars[Vars::JET_UNCALIBRATED_PT] );
      BTag.setVariable<float>(m_taggerNameBase, "JC_jetPt", vars[Vars::JC_JETPT]);
      BTag.setVariable<float>(m_taggerNameBase, "JC_all_jetPt", vars[Vars::JC_ALL_JETPT]);
      BTag.setVariable<float>(m_taggerNameBase, "SVC_jetPt", vars[Vars::SVC_JETPT]);
      BTag.setVariable<float>(m_taggerNameBase, "TVC_jetPt", vars[Vars::TVC_JETPT]);

      BTag.setVariable<float>(m_taggerNameBase, "JC_all", vars[Vars::JC_ALL] );
      BTag.setVariable<float>(m_taggerNameBase, "firstGoodTrkPt", vars[Vars::TRACK_GOOD_PT] );
      BTag.setVariable<float>(m_taggerNameBase, "firstTrkPt_SV", vars[Vars::TRACK_SV_PT]);
      BTag.setVariable<float>(m_taggerNameBase, "massSV_pions", vars[Vars::MASSSV_PIONS]);
      BTag.setVariable<float>(m_taggerNameBase, "massTV_kaons", vars[Vars::MASSTV_KAONS]);

      BTag.setVariable<float>(m_taggerNameBase, "JetCharge", vars[Vars::JC]);
      BTag.setVariable<float>(m_taggerNameBase, "nJCtracks", vars[Vars::NGOODTRK]);
     
      BTag.setVariable<float>(m_taggerNameBase, "SVC", vars[Vars::SVC]);
      BTag.setVariable<float>(m_taggerNameBase, "ntrk_sv", vars[Vars::NTRK0]);
      BTag.setVariable<float>(m_taggerNameBase, "dist_SV", vars[Vars::DISTSV]);
      BTag.setVariable<float>(m_taggerNameBase, "err_SV", vars[Vars::ERRSV]);
    
      BTag.setVariable<float>(m_taggerNameBase, "TVC", vars[Vars::TVC]);
      BTag.setVariable<float>(m_taggerNameBase, "ntrk_tv_used", vars[Vars::NTRK1_USED]);
      BTag.setVariable<float>(m_taggerNameBase, "dist_TV", vars[Vars::DISTTV]);
      BTag.setVariable<float>(m_taggerNameBase, "err_TV", vars[Vars::ERRTV]);
    
      BTag.setVariable<float>(m_taggerNameBase, "mu_charge", vars[Vars::MU_CHARGE]);
      BTag.setVariable<float>(m_taggerNameBase, "mu_ptRel", vars[Vars::MU_PTREL]);
      BTag.setVariable<float>(m_taggerNameBase, "mu_ptLong", vars[Vars::MU_PTLONG]);
      BTag.setVariable<float>(m_taggerNameBase, "mu_iso_ptvar40", vars[Vars::MU_ISO_PTVAR40]);
      BTag.setVariable<float>(m_taggerNameBase, "mu_jet_dR", vars[Vars::MU_JET_DR]);
      BTag.setVariable<float>(m_taggerNameBase, "mu_vtx", vars[Vars::MU_VTX]);

      BTag.setVariable<int>(m_taggerNameBase, "category", mvaCat );
      BTag.setVariable<float>(m_taggerNameBase, "discriminant", -7. ); 

    } 
    else if( m_runModus == "analysis") {   

      float llr;
      if(mvaCat == JC_noMu ) { 
        llr = logLikelihoodRatio( JC_noMu, vars[Vars::JC] , author);  
        BTag.setVariable<float>(m_taggerNameBase, "discriminant", llr );
        return StatusCode::SUCCESS;
      } 
      else if(mvaCat == JC_all ) { 
        llr = logLikelihoodRatio( JC_all, vars[Vars::JC_ALL] , author); 
        BTag.setVariable<float>(m_taggerNameBase, "discriminant", llr );
        return StatusCode::SUCCESS;
      }  
      else if(mvaCat == SVC ) { 
        llr = logLikelihoodRatio( SVC, vars[Vars::SVC] , author); 
        BTag.setVariable<float>(m_taggerNameBase, "discriminant", llr );
        return StatusCode::SUCCESS;
      } 
      else if(mvaCat < 0 ) {   //NULL cat
        BTag.setVariable<float>(m_taggerNameBase, "discriminant", -7. );
        return StatusCode::SUCCESS;
      } 

      TList* calib = readCdo->retrieveTObject<TList>(m_taggerNameBase, author, m_taggerNameBase + "Calib_cat_" + categoryToString(mvaCat));
      TMVA::Reader reader;
      std::unique_ptr<TMVA::MethodBase> method = 
         SetupReaders(vars, reader, author, alias, mvaCat, calib );
      if( !method ) {
        ATH_MSG_DEBUG("#BTAG# Could not setup the reader for the " << categoryToString(mvaCat) << " category and author " << author);
        return StatusCode::SUCCESS;
      } else ATH_MSG_DEBUG("#BTAG# Reader correctly setup for the " << categoryToString(mvaCat) << " category and author " << author); 


      //Finally compute the weight 
      float mvaWeight = reader.EvaluateMVA( method.get() ); 


      //Now I compute the log-likelihood ratio
      llr = logLikelihoodRatio( mvaCat, mvaWeight , author); 
      BTag.setVariable<float>(m_taggerNameBase, "discriminant", llr );


    }  //if runmodus Analysis


    return StatusCode::SUCCESS;

}

//===============================================================
//                   Helper functions 
//===============================================================

bool JetVertexCharge::passTrackCuts( const xAOD::Vertex& priVtx,
                                     const xAOD::TrackParticle &track) const {


   double d0 = track.d0();
   double z0 = track.z0();
   double theta = track.theta();
   if (track.numberDoF() == 0) return false;
   double chi2 = track.chiSquared() / track.numberDoF();
   double deltaZ0 = fabs( z0 - priVtx.z() + track.vz() );


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

int JetVertexCharge::Vars::category() const {

   if( m_v[JC] > -2 && m_v[SVC] > -2 && m_v[TVC] <-2 && fabs(m_v[MU_CHARGE]) <0.5 ) return JetVertexCharge::JC_SVC_noMu;
   else if( m_v[JC] > -2 && m_v[SVC] > -2 && m_v[TVC] < -2 && fabs(m_v[MU_CHARGE]) >0.5 ) return JetVertexCharge::JC_SVC_incMu;
   else if( m_v[JC] > -2 && m_v[SVC] > -2 && m_v[TVC] > -2 && fabs(m_v[MU_CHARGE]) < 0.5 ) return JetVertexCharge::JC_SVC_TVC_noMu;
   else if( m_v[JC] > -2 && m_v[SVC] > -2 && m_v[TVC] > -2 && fabs(m_v[MU_CHARGE]) > 0.5 ) return JetVertexCharge::JC_SVC_TVC_incMu;
   else if( m_v[JC] > -2 && m_v[SVC] < -2 && fabs(m_v[MU_CHARGE]) > 0.5 ) return JetVertexCharge::JC_incMu;
   else if( m_v[JC] > -2 && m_v[SVC] < -2 && fabs(m_v[MU_CHARGE]) < 0.5 ) return JetVertexCharge::JC_noMu; 
   else if( m_v[JC] < -2 && m_v[SVC] > -2 ) return JetVertexCharge::SVC;
   else if( m_v[JC] < -2 && m_v[JC_ALL] > -2  ) return JetVertexCharge::JC_all;
   else return -1;  

}

//////////////////////////////////////////////////////
float  JetVertexCharge::logLikelihoodRatio( int mvaCat, float mvaWeight, std::string author)  const {


  TH1* histo_pos = nullptr;
  TH1* histo_neg = nullptr;

  //Retrieval of Calibration Condition Data objects
  SG::ReadCondHandle<JetTagCalibCondData> readCdo(m_readKey);
  
  if( mvaCat == JC_noMu) {
    histo_pos = readCdo->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_noMu_bbar"); 
    histo_neg = readCdo->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_noMu_b"); 
  }
  else if( mvaCat == JC_SVC_noMu) { 
    histo_pos = readCdo->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_SVC_noMu_bbar"); 
    histo_neg = readCdo->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_SVC_noMu_b"); 
  }
  else if( mvaCat == JC_SVC_incMu) {
    histo_pos = readCdo->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_SVC_incMu_bbar"); 
    histo_neg = readCdo->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_SVC_incMu_b"); 
  }
  else if( mvaCat == JC_SVC_TVC_noMu) {
    histo_pos = readCdo->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_SVC_TVC_noMu_bbar"); 
    histo_neg = readCdo->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_SVC_TVC_noMu_b"); 
  }
  else if( mvaCat == JC_incMu) {
    histo_pos = readCdo->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_incMu_bbar"); 
    histo_neg = readCdo->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_incMu_b"); 
  }
  else if( mvaCat == JC_SVC_TVC_incMu) {
    histo_pos = readCdo->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_SVC_TVC_incMu_bbar"); 
    histo_neg = readCdo->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_SVC_TVC_incMu_b"); 
  }
  else if( mvaCat == SVC) {
    histo_pos = readCdo->retrieveHistogram(m_taggerNameBase, author, "jvc_SVC_bbar"); 
    histo_neg = readCdo->retrieveHistogram(m_taggerNameBase, author, "jvc_SVC_b"); 
  }
  else if( mvaCat == JC_all) {
    histo_pos = readCdo->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_all_bbar"); 
    histo_neg = readCdo->retrieveHistogram(m_taggerNameBase, author, "jvc_JC_all_b"); 
  }
  else if(mvaCat == -1) {
    return -7; 
  }

  if( histo_pos == nullptr ) {
    ATH_MSG_WARNING("#BTAG# BBAR HISTO can't be retrieved -> no calibration for "<< m_taggerNameBase );
    return -3.; 
  }
  if( histo_neg== nullptr) {
    ATH_MSG_WARNING("#BTAG# B HISTO can't be retrieved -> no calibration for "<< m_taggerNameBase );
    return -3;
  }

  int bin_pos = histo_pos->FindBin( mvaWeight );
  float histo_bbar = std::max(histo_pos->GetBinContent( bin_pos ), minProb);

  int bin_neg = histo_neg->FindBin( mvaWeight );
  float histo_b = std::max(histo_neg->GetBinContent( bin_neg ), minProb);

  ATH_MSG_DEBUG("#BTAG# probabilities for category " << categoryToString(mvaCat) << " and weight " << mvaWeight << ": "
		<< histo_bbar << "(bbar), " << histo_b << "(b)");
  return log( histo_bbar/histo_b); 
}


std::unique_ptr<TMVA::MethodBase>
JetVertexCharge::SetupReaders( Vars& vars,
                               TMVA::Reader& reader,
                               const std::string& /*author*/,
                               const std::string& /*alias*/,
                               int mvaCat, TList* list) const
{
   ATH_MSG_DEBUG("#BTAG# setting up reader for category "<<mvaCat);

   if (! list) {
     ATH_MSG_WARNING("#BTAG# Tlist can't be retrieved -> no calibration for "<< m_taggerNameBase );
     return std::unique_ptr<TMVA::MethodBase>();
   }


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

   for (auto expression : inputVars) {
     std::string var = expression.find_last_of(":") == std::string::npos ? expression : expression.substr(expression.find_last_of(":")+1);
     ATH_MSG_DEBUG("#BTAG# adding variable " << var << " in category "
                   << categoryToString(mvaCat)
                   << " for tagger instance " << m_taggerNameBase);
     reader.AddVariable(expression.c_str(), &vars[var]);
   }

   TMVA::IMethod* method= reader.BookMVA(TMVA::Types::kMLP, iss.str().data());
   return std::unique_ptr<TMVA::MethodBase>
     (dynamic_cast<TMVA::MethodBase*>(method));
}  //closes SetupReaders


void JetVertexCharge::Vars::print (MsgStream& msg) const
{
   if (msg.level() > MSG::DEBUG) return;
   msg << MSG::DEBUG << "#BTAG# ===============================================================================" << endmsg;
   msg << MSG::DEBUG << "#BTAG# Printing input variables: " << endmsg;
   msg << MSG::DEBUG << "#BTAG# JC="<<m_v[JC]<<"  SVC="<<m_v[SVC]<<"  TVC="<<m_v[TVC]<<"  mu charge="<<m_v[MU_CHARGE]<<" JC all="<<m_v[JC_ALL] << endmsg;
   msg << MSG::DEBUG << "#BTAG# JC_jetPt="<<m_v[JC_JETPT]<<"  SVC_jetPt="<<m_v[SVC_JETPT]<<"  TVC_jetPt="<<m_v[TVC_JETPT]<<"  mu charge="<<m_v[MU_CHARGE]<<" JC_all_jetPt="<<m_v[JC_ALL_JETPT] << endmsg;
   msg << MSG::DEBUG << "#BTAG# ngood trk="<<m_v[NGOODTRK]<<"  JC pt="<<m_v[TRACK_GOOD_PT]<<"  SV pt= "<<m_v[TRACK_SV_PT] << endmsg;
   msg << MSG::DEBUG << "#BTAG# ntrkSV="<<m_v[NTRK0]<<"  distSV="<<m_v[DISTSV] <<"  errSV="<<m_v[ERRSV]<<" mass SV="<<m_v[MASSSV_PIONS] << endmsg;
   msg << MSG::DEBUG << "#BTAG# ntrkTV="<<m_v[NTRK1_USED]<<"  distTV="<<m_v[DISTTV]<<"  errTV="<<m_v[ERRTV]<<" mass TV="<<m_v[MASSTV_KAONS] << endmsg;
   msg << MSG::DEBUG << "#BTAG# mu ptRel="<<m_v[MU_PTREL]<<"  mu_ptLong="<<m_v[MU_PTLONG]<<"  mu dR="<< m_v[MU_JET_DR]<<"  mu_isolation="<<m_v[MU_ISO_PTVAR40]<<"  mu_vtx="<<m_v[MU_VTX] << endmsg;
   msg << MSG::DEBUG << "#BTAG# ===============================================================================" << endmsg;

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

const std::unordered_map<std::string, JetVertexCharge::Vars::Var>
JetVertexCharge::Vars::s_namemap =
  {
   { "mu_ptRel", MU_PTREL },
   { "mu_ptLong", MU_PTLONG },
   { "mu_charge", MU_CHARGE },
   { "mu_jet_dR", MU_JET_DR },
   { "mu_iso_ptvar40", MU_ISO_PTVAR40 },
   { "TVC", TVC },
   { "distTV", DISTTV },
   { "errTV", ERRTV },
   { "massTV_kaons", MASSTV_KAONS },
   { "ntrk1_used", NTRK1_USED },
   { "SVC", SVC },
   { "distSV", DISTSV },
   { "errSV", ERRSV },
   { "massSV_pions", MASSSV_PIONS },
   { "ntrk0", NTRK0 },
   { "track_sv_pt", TRACK_SV_PT },
   { "JC", JC },
   { "track_good_pt", TRACK_GOOD_PT },
   { "ngoodtrk", NGOODTRK },
   { "jet_uncalibrated_pt", JET_UNCALIBRATED_PT },
   { "JC_jetPt", JC_JETPT },
   { "JC_all_jetPt", JC_ALL_JETPT },
   { "SVC_jetPt", SVC_JETPT },
   { "TVC_jetPt", TVC_JETPT },
  };

JetVertexCharge::Vars::Vars()
{
  m_v[MU_PTREL] = -999.;
  m_v[MU_PTLONG] = -999.;
  m_v[MU_CHARGE] = 0.;
  m_v[MU_JET_DR] = -1.;
  m_v[MU_ISO_PTVAR40] = -1.;

  m_v[TVC] = -3;
  m_v[DISTTV] = -999.;
  m_v[ERRTV] = -999.;
  m_v[MASSTV_KAONS] = -999.;  
  m_v[NTRK1_USED] = -1.;

  m_v[SVC] = -3;
  m_v[DISTSV] = -999.;
  m_v[ERRSV] = -999.;
  m_v[MASSSV_PIONS] = -999.; 
  m_v[NTRK0] = -1.;
  m_v[TRACK_SV_PT] = -999.; 

  m_v[JC] = -3;
  m_v[TRACK_GOOD_PT] = -999.;
  m_v[NGOODTRK] = 0;
  m_v[JET_UNCALIBRATED_PT] = -999.;
  m_v[JC_JETPT] = -999.;
  m_v[JC_ALL_JETPT] = -999.;
  m_v[SVC_JETPT] = -9.;
  m_v[TVC_JETPT] = -9.;

  m_v[JC_ALL] = -3;
  m_v[MU_VTX] = -1;
}


float& JetVertexCharge::Vars::operator[] (const std::string& name)
{
  auto it = s_namemap.find (name);
  if (it == s_namemap.end()) {
    throw std::out_of_range (name);
  }
  return m_v[it->second];
}

}  //End of namespace
