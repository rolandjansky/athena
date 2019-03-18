/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file InDetGlobalTopBottomMonTool.cxx
* Implementation of inner detector global top bottom monitoring tool
*
* @author
* Anne-Catherine Le Bihan <Anne-Catherine.Le.Bihan@cern.ch> @n
* Heidi Sandaker <Heidi.Sandaker@cern.ch> @n
* Arshak Tonoyan <Arshak.Tonyoan@cern.ch> @n
* Thomas Burgess <Thomas.Burgess@cern.ch> @n
* Alex Kastanas <Alex.Kastanas@cern.ch>       
*
* $Id: InDetGlobalTopBottomMonTool.cxx,v 1.27 2008-12-17 17:31:15 sandaker Exp $
*
*****************************************************************************/


//Local
#include "InDetGlobalTopBottomMonTool.h"
//Framework
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/ReadHandleKey.h"
//Standard c++
#include <string>
//Root 
#include "TH1F.h"
#include "TMath.h"
#include <cmath>

#include "EventPrimitives/EventPrimitivesHelpers.h"

using namespace TMath;

InDetGlobalTopBottomMonTool::InDetGlobalTopBottomMonTool(
    const std::string & type, 
    const std::string & name,
    const IInterface* parent)
    :InDetGlobalMotherMonTool(type, name, parent),
     m_detector("ID"),
     m_tracks_top(nullptr), m_tracks_bottom(nullptr),
     m_nTopTrks(0),
     m_nBottomTrks(0),
     m_top_d0(0), 
     m_top_z0(0),
     m_top_phi(0), 
     m_top_eta(0),
     m_top_chi2(0),
     m_bottom_d0(0), 
     m_bottom_z0(0),
     m_bottom_phi(0),
     m_bottom_eta(0),
     m_bottom_chi2(0),
     m_top_d0_1trk(0), 
     m_top_z0_1trk(0),
     m_top_phi_1trk(0), 
     m_top_eta_1trk(0),
     m_top_chi2_1trk(0),
     m_bottom_d0_1trk(0), 
     m_bottom_z0_1trk(0),
     m_bottom_phi_1trk(0),
     m_bottom_eta_1trk(0),
     m_bottom_chi2_1trk(0),
     m_top_bottom_dd0(0), 
     m_top_bottom_dz0(0),
     m_top_bottom_dphi(0), 
     m_top_bottom_deta(0),
     m_top_bottom_d0_pull(0), 
     m_top_bottom_z0_pull(0),
     m_top_bottom_phi_pull(0), 
     m_top_bottom_eta_pull(0),
     m_Top_eta(),
     m_Top_phi(),
     m_Top_d0(),
     m_Top_z0(),
     m_Top_chi2(),
     m_Bottom_eta(),
     m_Bottom_phi(),
     m_Bottom_d0(),
     m_Bottom_z0(),
     m_Bottom_chi2(),
     m_Top_deta(),
     m_Top_dphi(),
     m_Top_dd0(),
     m_Top_dz0(),
     m_Bottom_deta(),
     m_Bottom_dphi(),
     m_Bottom_dd0(),
     m_Bottom_dz0(),
     m_doTopBottom(false)
{
    declareProperty("SCTTrackName",         m_SCTTracksName="ResolvedSCTTracks");
    declareProperty("TRTTrackName",         m_TRTTracksName="StandaloneTRTTracks");
    declareProperty("PixelTrackName",       m_PIXTracksName="ResolvedPixelTracks");
    declareProperty("CombinedTrackName",    m_CombinedTracksName="CombinedInDetTracks");
    declareProperty("doTopBottom",          m_doTopBottom);
    declareProperty("Detector", m_detector); 
}


StatusCode InDetGlobalTopBottomMonTool::initialize() {
    m_sctTracksUpKey = m_SCTTracksName + "Up";
    m_sctTracksLowKey = m_SCTTracksName + "Low";
    m_trtTracksUpKey = m_TRTTracksName + "Up";
    m_trtTracksLowKey = m_TRTTracksName + "Low";
    m_pixTracksUpKey = m_PIXTracksName + "Up";
    m_pixTracksLowKey = m_PIXTracksName + "Low";
    m_combinedTracksUpKey = m_CombinedTracksName + "Up";
    m_combinedTracksLowKey = m_CombinedTracksName + "Low";

    ATH_CHECK( m_sctTracksUpKey.initialize(m_detector == "sct") );
    ATH_CHECK( m_sctTracksLowKey.initialize(m_detector == "sct") );
    ATH_CHECK( m_trtTracksUpKey.initialize(m_detector == "trt") );
    ATH_CHECK( m_trtTracksLowKey.initialize(m_detector == "trt") );
    ATH_CHECK( m_pixTracksUpKey.initialize(m_detector == "pixel") );
    ATH_CHECK( m_pixTracksLowKey.initialize(m_detector == "pixel") );
    ATH_CHECK( m_combinedTracksUpKey.initialize(m_detector == "ID") );
    ATH_CHECK( m_combinedTracksLowKey.initialize(m_detector == "ID") );

    return StatusCode::SUCCESS;
}


//---------------------------------------------------------
StatusCode InDetGlobalTopBottomMonTool::bookHistogramsRecurrent()
{  
    MonGroup monGr_shift ( this, "InDetGlobal/TopBottom",  run);
    MonGroup monGr_exp   ( this, "InDetGlobal/TopBottom", run);
  
    if (newRunFlag()) {
	// Number of tracks

	registerHist(
	    monGr_shift,m_nBottomTrks=
	    new TH1F("m_nBottomTrks","number of bottom segments",5,0,5));
	registerHist(
	    monGr_shift,m_nTopTrks=
	    new TH1F("m_nTopTrks","number of top segments",5,0,5));
 
	// Track parameters at perigee
    
	registerHist(
	    monGr_exp,m_top_phi=
	    new TH1F("m_top_phi","phi of top segment",100,-M_PI,M_PI));
	registerHist(monGr_exp,m_top_eta=
		     new TH1F("m_top_eta","eta of top segment",100,-1.,1.));
	registerHist(
	    monGr_exp,m_top_d0=
	    new TH1F("m_top_d0","transverse impact parameter of top segment",
		     200,-500,500));
	registerHist(monGr_exp,m_top_z0=
		     new TH1F("m_top_z0","z0 of top segment",200,-1000,1000));
	registerHist(monGr_exp,m_top_chi2=
		     new TH1F("m_top_chi2","chi2 of top segment",100,0,100));
	registerHist(
	    monGr_exp,m_bottom_phi=new TH1F(
		"m_bottom_phi","phi of bottom segment",100,-M_PI,M_PI));
	registerHist(
	    monGr_exp,m_bottom_eta=
	    new TH1F("m_bottom_eta","eta of bottom segment",100,-1.,1.));
	registerHist(
	    monGr_exp,m_bottom_d0=new TH1F(
		"m_bottom_d0","transverse impact parameter of bottom segment",
		200,-500,500));
	registerHist(
	    monGr_exp,m_bottom_z0=
	    new TH1F("m_bottom_z0","z0 of bottom segment",200,-1000,1000));
	registerHist(
	    monGr_exp,m_bottom_chi2=
	    new TH1F("m_bottom_chi2","chi2 of bottom segment",100,0,100));
  
	registerHist(
	    monGr_exp,m_top_phi_1trk=new TH1F(
		"m_top_phi_1trk",
		"phi of top segment-only 1 segment per event",
		100,-M_PI,M_PI));
	registerHist(
	    monGr_exp,m_top_eta_1trk=new TH1F(
		"m_top_eta_1trk",
		"eta of top segment-only 1 segment per event",
		100,-1.,1.));
	registerHist(
	    monGr_exp,m_top_d0_1trk=new TH1F(
		"m_top_d0_1trk","transverse impact parameter of top segment"
		"-only 1 segment per event",200,-500,500));
	registerHist(
	    monGr_exp,m_top_z0_1trk=
	    new TH1F("m_top_z0_1trk",
		     "z0 of top segment-only 1 segment per event",
		     200,-1000,1000));
	registerHist(
	    monGr_exp,m_top_chi2_1trk=new TH1F(
		"m_top_chi2_1trk",
		"chi2 of top segment-only 1 segment per event"
		,100,0,100));
	registerHist(
	    monGr_exp,m_bottom_phi_1trk=new TH1F(
		"m_bottom_phi_1trk","phi of bottom segment-"
		"only 1 segment per event",100,-M_PI,M_PI));
	registerHist(
	    monGr_exp,m_bottom_eta_1trk=new TH1F(
		"m_bottom_eta_1trk",
		"eta of bottom segment-only 1 segment per event",
		100,-1.,1.));
	registerHist(
	    monGr_exp,m_bottom_d0_1trk=new TH1F(
		"m_bottom_d0_1trk",
		"transverse impact parameter of bottom segment "
		"- only 1 segment per event",200,-500,500));
	registerHist(
	    monGr_exp,m_bottom_z0_1trk=new TH1F(
		"m_bottom_z0_1trk",
		"z0 of bottom segment-only 1 segment per event"
		,200,-1000,1000));
	registerHist(
	    monGr_exp,m_bottom_chi2_1trk=new TH1F(
		"m_bottom_chi2_1trk",
		"chi2 of bottom segment-only 1 segment per event",
		100,0,100));   
	registerHist(
	    monGr_exp,m_top_bottom_dphi=new TH1F(
		"m_top_bottom_dphi",
		"Delta(phi) between top and bottom segments",
		100,-0.05,0.05));
	registerHist(
	    monGr_exp,m_top_bottom_deta=new TH1F(
		"m_top_bottom_deta",
		"Delta(eta) between top and bottom segments",
		100,-0.2,0.2));
	registerHist(
	    monGr_exp,m_top_bottom_dd0=new TH1F(
		"m_top_bottom_dd0","Delta(d0) between top and bottom segments",
		400,-10,10));
	registerHist(
	    monGr_exp,m_top_bottom_dz0=new TH1F(
		"m_top_bottom_dz0","Delta(z0) between top and bottom segments",
		200,-100,100));
	registerHist(
	    monGr_exp,m_top_bottom_phi_pull=new TH1F(
		"m_top_bottom_phi_pull",
		"Delta(phi) pull between top and bottom segments",
		200,-100,100));
	registerHist(
	    monGr_exp,m_top_bottom_eta_pull=new TH1F(
		"m_top_bottom_eta_pull",
		"Delta(eta) pull between top and bottom segments",
		200,-0.1,0.1));
	registerHist(
	    monGr_exp,m_top_bottom_d0_pull=new TH1F(
		"m_top_bottom_d0_pull",
		"Delta(d0) pull between top and bottom segments",100,-50,50));
	registerHist(
	    monGr_exp,m_top_bottom_z0_pull=new TH1F(
		"m_top_bottom_z0_pull",
		"Delta(z0) pull between top and bottom segments",
		200,-30,30));   
    }

    return StatusCode::SUCCESS;
}


//---------------------------------------------------------
StatusCode InDetGlobalTopBottomMonTool::fillHistograms()
{
	
	
  StatusCode sc;

  if (m_detector == "sct") { 
    
    SG::ReadHandle<TrackCollection> sctTracksUp(m_sctTracksUpKey);
    if (!sctTracksUp.isValid()) {
      ATH_MSG_DEBUG("No SCT segments in StoreGate ");
      return StatusCode::SUCCESS;
    } else {
      ATH_MSG_ERROR("found SCT segments in StoreGate " <<
		    m_sctTracksUpKey.key() << " " << sctTracksUp->size());
    }
    
    SG::ReadHandle<TrackCollection> sctTracksLow(m_sctTracksLowKey);
    if (!sctTracksLow.isValid()) {
      ATH_MSG_DEBUG("No SCT segments in StoreGate ");
      return StatusCode::SUCCESS;
    } else {
      ATH_MSG_DEBUG("found SCT segments in StoreGate " <<
		    m_sctTracksLowKey.key() << " " << sctTracksLow->size());
    }
    
    m_tracks_top = sctTracksUp.get();
    m_tracks_bottom = sctTracksLow.get();
    
  }
  
  else if (m_detector == "pixel") { 

  SG::ReadHandle<TrackCollection> pixTracksUp(m_pixTracksUpKey);
  if (!pixTracksUp.isValid()) {
    ATH_MSG_DEBUG("No PIX segments in StoreGate ");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("found PIX segments in StoreGate " <<
		  m_pixTracksUpKey.key() << " " << pixTracksUp->size());
  }
  
  SG::ReadHandle<TrackCollection> pixTracksLow(m_pixTracksLowKey);
  if (!pixTracksLow.isValid()) {
    ATH_MSG_DEBUG("No PIX segments in StoreGate ");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("found PIX segments in StoreGate " <<
		  m_pixTracksLowKey.key() << " " << pixTracksLow->size());
  }
  
  m_tracks_top = pixTracksUp.get();
  m_tracks_bottom = pixTracksLow.get();
  
  }

  else if(m_detector=="trt"){  

  SG::ReadHandle<TrackCollection> trtTracksUp(m_trtTracksUpKey);
  if (!trtTracksUp.isValid()) {
    ATH_MSG_DEBUG("No TRT segments in StoreGate ");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("found TRT segments in StoreGate " <<
		  m_trtTracksUpKey.key() << " " << trtTracksUp->size());
  }
  
  SG::ReadHandle<TrackCollection> trtTracksLow(m_trtTracksLowKey);
  if (!trtTracksLow.isValid()) {
    ATH_MSG_DEBUG("No TRT segments in StoreGate ");
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("found TRT segments in StoreGate " <<
		  m_trtTracksLowKey.key() << " " << trtTracksLow->size());
  }
  
  
  m_tracks_top = trtTracksUp.get();
  m_tracks_bottom = trtTracksLow.get();

  }

  else{

  SG::ReadHandle<TrackCollection> combinedTracksUp(m_combinedTracksUpKey);
  if (!combinedTracksUp.isValid()) {
    ATH_MSG_DEBUG("No combined segments in StoreGate ");
    if(m_detector=="ID") return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("found combined segments in StoreGate " <<
		  m_combinedTracksUpKey.key() << " " << combinedTracksUp->size());
  }
  
  SG::ReadHandle<TrackCollection> combinedTracksLow(m_combinedTracksLowKey);
  if (!combinedTracksLow.isValid()) {
    ATH_MSG_DEBUG("No combined segments in StoreGate ");
    if(m_detector=="ID") return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("found combined segments in StoreGate " <<
		  m_combinedTracksLowKey.key() << " " << combinedTracksLow->size());
  }
  m_tracks_top = 0; 
  m_tracks_bottom = 0; 
  }
  
    //------------ Loop over bottom segments ------------------
  
    unsigned int nBottomTracks=0;
  
    if (m_tracks_bottom) {
	for(TrackCollection::const_iterator ibottom = m_tracks_bottom->begin();
	    ibottom != m_tracks_bottom->end(); ++ibottom) {
	    const Trk::Track *track_bottom=(*ibottom);

	    if (track_bottom == 0) { 
		msg(MSG::WARNING)
		      << "no pointer to bottom track!!!" << endmsg;
		continue;
	    }
    
	    // Track at perigee
    
	    const Trk::Perigee *measPerigee=
		dynamic_cast<const Trk::Perigee *>(
		    track_bottom->perigeeParameters());
   
	    if (measPerigee&&nBottomTracks<s_nMaxTracks) {   
		m_Bottom_eta[nBottomTracks]=
		    measPerigee->eta();
		m_Bottom_phi[nBottomTracks]=
		    measPerigee->parameters()[Trk::phi0];
		m_Bottom_d0 [nBottomTracks]=
		    measPerigee->parameters()[Trk::d0];
		m_Bottom_z0 [nBottomTracks]=
		    measPerigee->parameters()[Trk::z0];
		const AmgSymMatrix(5) * ErrorMat =
		    measPerigee->covariance();
		if ( ErrorMat )
		{
		    m_Bottom_deta[nBottomTracks]= -log(tan( Amg::error( *ErrorMat, Trk::theta ) / 2. ) );
		    m_Bottom_dphi[nBottomTracks]=Amg::error( *ErrorMat, Trk::phi0);
		    m_Bottom_dd0 [nBottomTracks]=Amg::error( *ErrorMat, Trk::d0  );
		    m_Bottom_dz0 [nBottomTracks]=Amg::error( *ErrorMat, Trk::z0  );
		}
	    } else { 
		msg(MSG::ERROR) 
		      << "no measurement at perigee !!!" << endmsg;
	    }
	    if ( nBottomTracks<s_nMaxTracks)
		m_Bottom_chi2[nBottomTracks]=
		    (track_bottom->fitQuality()->chiSquared())/
		    (track_bottom->fitQuality()->numberDoF());

	    nBottomTracks++;  
	}
    }
   
    m_nBottomTrks->Fill(nBottomTracks);
   
  
    //------------ Loop over top segments ------------------
 
    unsigned int nTopTracks=0;
  
    if (m_tracks_top) { 
	for(TrackCollection::const_iterator itop = m_tracks_top->begin();
	    itop != m_tracks_top->end(); ++itop) {
	    const Trk::Track *track_top=(*itop);
	    if (track_top == 0){ 
		msg(MSG::WARNING) 
		      << "no pointer to top track!!!" << endmsg;
		continue;
	    } 
	    // Track at perigee    
	    const Trk::Perigee *measPerigee=
		dynamic_cast<const Trk::Perigee *>(
		    track_top->perigeeParameters());
	    if (measPerigee&&nTopTracks<s_nMaxTracks) {   
		m_Top_eta[nTopTracks]=measPerigee->eta();
		m_Top_phi[nTopTracks]=measPerigee->parameters()[Trk::phi0];
		m_Top_d0 [nTopTracks]=measPerigee->parameters()[Trk::d0];
		m_Top_z0 [nTopTracks]=measPerigee->parameters()[Trk::z0];

		const AmgSymMatrix(5) * ErrorMat=
		    measPerigee->covariance();
		if ( ErrorMat )
		{
		    m_Top_deta[nTopTracks]= -log(tan( Amg::error( *ErrorMat, Trk::theta )/2. ) );
		    m_Top_dphi[nTopTracks]=Amg::error( *ErrorMat, Trk::phi0 );
		    m_Top_dd0 [nTopTracks]=Amg::error( *ErrorMat, Trk::d0   );
		    m_Top_dz0 [nTopTracks]=Amg::error( *ErrorMat, Trk::z0   );
		}
	    } else {
		msg(MSG::ERROR) 
		      << "no measurement at perigee !!!" << endmsg;
	    } 
	    if ( nTopTracks<s_nMaxTracks) {   
		m_Top_chi2[nTopTracks]=
		    (track_top->fitQuality()->chiSquared())/
		    (track_top->fitQuality()->numberDoF());
	    }
	    nTopTracks++;     
	}
    }

    m_nTopTrks->Fill(nTopTracks);
  
    // Fill remaining histograms
    if(nTopTracks>0) {
	m_top_d0->Fill(m_Top_d0[0]);
	m_top_z0->Fill(m_Top_z0[0]);
	m_top_phi->Fill(m_Top_phi[0]);
	m_top_eta->Fill(m_Top_eta[0]);
	m_top_chi2->Fill(m_Top_chi2[0]);
    }
  
    if(nBottomTracks>0) {
	m_bottom_d0->Fill(m_Bottom_d0[0]);
	m_bottom_z0->Fill(m_Bottom_z0[0]);
	m_bottom_phi->Fill(m_Bottom_phi[0]);
	m_bottom_eta->Fill(m_Bottom_eta[0]);
	m_bottom_chi2->Fill(m_Bottom_chi2[0]);
    }
       
    if (nTopTracks==1 && nBottomTracks==1) {       
	m_top_bottom_deta->Fill(m_Top_eta[0]-m_Bottom_eta[0]);
	m_top_bottom_dphi->Fill(m_Top_phi[0]-m_Bottom_phi[0]);
	m_top_bottom_dd0->Fill(m_Top_d0[0]-m_Bottom_d0[0]);
	m_top_bottom_dz0->Fill(m_Top_z0[0]-m_Bottom_z0[0]);
	m_top_bottom_eta_pull->Fill(
	    (m_Top_eta[0]- m_Bottom_eta[0])/sqrt(
		(pow(m_Top_deta[0],2)+pow(m_Bottom_deta[0],2))));
	m_top_bottom_phi_pull->Fill(
	    (m_Top_phi[0]- m_Bottom_phi[0])/sqrt(
		(pow(m_Top_dphi[0],2)+pow(m_Bottom_dphi[0],2))));
	m_top_bottom_d0_pull->Fill(
	    (m_Top_d0[0]-m_Bottom_d0[0])/sqrt(
		(pow(m_Top_dd0[0],2)+pow(m_Bottom_dd0[0],2))));
	m_top_bottom_z0_pull->Fill(
	    (m_Top_z0[0]-m_Bottom_z0[0])/sqrt(
		(pow(m_Top_dz0[0],2)+pow(m_Bottom_dz0[0],2))));
	
	m_top_d0_1trk->Fill(m_Top_d0[0]);
	m_top_z0_1trk->Fill(m_Top_z0[0]);
	m_top_phi_1trk->Fill(m_Top_phi[0]);
	m_top_eta_1trk->Fill(m_Top_eta[0]);  
	m_top_chi2_1trk->Fill(m_Top_chi2[0]);
  
	m_bottom_d0_1trk->Fill(m_Bottom_d0[0]);
	m_bottom_z0_1trk->Fill(m_Bottom_z0[0]);
	m_bottom_phi_1trk->Fill(m_Bottom_phi[0]);
	m_bottom_eta_1trk->Fill(m_Bottom_eta[0]);    
	m_bottom_chi2_1trk->Fill(m_Bottom_chi2[0]);     
    }      
    
    return StatusCode::SUCCESS;
}
