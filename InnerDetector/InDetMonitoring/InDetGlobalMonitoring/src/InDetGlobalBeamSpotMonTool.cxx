/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:  $

#include <sstream>
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"



#include "StoreGate/ReadHandle.h"

#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkTrack/TrackCollection.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/VxContainer.h"

#include "xAODTracking/TrackParticle.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "InDetGlobalBeamSpotMonTool.h"


InDetGlobalBeamSpotMonTool::InDetGlobalBeamSpotMonTool( const std::string & type, const std::string & name, const IInterface* parent )
  :InDetGlobalMotherMonTool( type, name, parent ),
   m_hTrNPt(nullptr),
   m_hTrPt(nullptr),
   m_hTrDPhi(nullptr),
   m_hTrDPhiCorr(nullptr),
   m_hBsX(nullptr),
   m_hBsY(nullptr),
   m_hBsZ(nullptr),
   m_hBsTiltX(nullptr),
   m_hBsTiltY(nullptr),
   m_hPvN(nullptr),
   m_hPvNPriVtx(nullptr),
   m_hPvNPileupVtx(nullptr),
   m_hPvX(nullptr),
   m_hPvY(nullptr),
   m_hPvZ(nullptr),
   m_hPvErrX(nullptr),
   m_hPvErrY(nullptr),
   m_hPvErrZ(nullptr),
   m_hPvChiSqDoF(nullptr),
   m_hPvXZ(nullptr),
   m_hPvYZ(nullptr),
   m_hPvYX(nullptr),
   m_hPvNTracksAll(nullptr),
   m_hPvNTracks(nullptr),
   m_hPvTrackPt(nullptr),
   m_hPvTrackEta(nullptr)
{
  declareProperty("useBeamspot",m_useBeamspot=true);
  declareProperty("vxContainerWithBeamConstraint",m_vxContainerWithBeamConstraint=false);
  declareProperty("minTracksPerVtx",m_minTracksPerVtx=4);
  declareProperty("minTrackPt",m_minTrackPt=500);  // MeV
  declareProperty("histFolder",m_histFolder="InDetGlobal/BeamSpot");
}


InDetGlobalBeamSpotMonTool::~InDetGlobalBeamSpotMonTool() {
}


StatusCode InDetGlobalBeamSpotMonTool::initialize() {
  StatusCode sc;                                      
  sc = ManagedMonitorToolBase::initialize();
  if(!sc.isSuccess()) return sc;
  ATH_CHECK(m_beamSpotKey.initialize(m_useBeamspot));  

  ATH_CHECK(m_vxContainerName.initialize());
  ATH_CHECK(m_trackContainerName.initialize());

  return StatusCode::SUCCESS;
}


StatusCode InDetGlobalBeamSpotMonTool::bookHistogramsRecurrent() {

    MonGroup al_beamspot_shift( this, m_histFolder,  run, ATTRIB_UNMANAGED );
    MonGroup al_beamspot_expert( this, m_histFolder, run, ATTRIB_UNMANAGED );

  if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }

  if ( AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }
  

  if( newRunFlag() ) {

    // Histograms for track-based beam spot monitoring
    m_hTrDPhi       = makeAndRegisterTH2F(al_beamspot_shift,"trkDPhi","DCA vs Phi;#varphi (rad);d_{0} (#mum)",100,-3.5,3.5,100,-1000,1000);

    m_hTrPt         = makeAndRegisterTH1F(al_beamspot_expert,"trkPt","Track Pt;P_{t} (GeV)",100,0,20);
    m_hTrNPt        = makeAndRegisterTH1F(al_beamspot_expert,"trkNPt","Number of Tracks per event (after Pt cut);Number of tracks",100,0,1000);

    // Histograms of assumed beam spot position
    if (m_useBeamspot) {
      m_hTrDPhiCorr   = makeAndRegisterTH2F(al_beamspot_shift,"trkDPhiCorr","DCA vs Phi wrt Beamspot;#varphi (rad);d_{0} (#mum)",100,-3.5,3.5,100,-500,500); 
      m_hBsX          = makeAndRegisterTH1F(al_beamspot_shift,"bsX","Beam spot position: x;x (mm)",100,-10,10);
      m_hBsY          = makeAndRegisterTH1F(al_beamspot_shift,"bsY","Beam spot position: y;y (mm)",100,-10,10);
      m_hBsZ          = makeAndRegisterTH1F(al_beamspot_shift,"bsZ","Beam spot position: z;z (mm)",100,-500,500);
      m_hBsTiltX      = makeAndRegisterTH1F(al_beamspot_shift,"bsTiltX","Beam spot tile angle: x-z plane; Tilt angle (#murad)",100,-1e3,1e3);
      m_hBsTiltY      = makeAndRegisterTH1F(al_beamspot_shift,"bsTiltY","Beam spot tile angle: y-z plane; Tilt angle (#murad)",100,-1e3,1e3);
    }

    // Histograms for vertex-based beam spot monitoring
    if (! m_vxContainerWithBeamConstraint) {

      // The following histograms are made either relative to the current beamspot (from BeamCondSvc),
      // or relative to the nomial beamspot at (0,0,0) without any tilt.
      if (m_useBeamspot) {
        m_hPvX          = makeAndRegisterTH1F(al_beamspot_shift,"pvX","Primary vertex: x - x_{beam};x-x_{beam} (#mum)",100,-500,500);
        m_hPvY          = makeAndRegisterTH1F(al_beamspot_shift,"pvY","Primary vertex: y - y_{beam};y-y_{beam} (#mum)",100,-500,500);
        m_hPvZ          = makeAndRegisterTH1F(al_beamspot_shift,"pvZ","Primary vertex: z - z_{beam};z-z_{beam} (mm)",100,-500,500);
      } else {
        m_hPvX          = makeAndRegisterTH1F(al_beamspot_shift,"pvX","Primary vertex: x;x (mm)",100,-10,10);
        m_hPvY          = makeAndRegisterTH1F(al_beamspot_shift,"pvY","Primary vertex: y;y (mm)",100,-10,10);
        m_hPvZ          = makeAndRegisterTH1F(al_beamspot_shift,"pvZ","Primary vertex: z;z (mm)",100,-500,500);
      }

      // Histograms that are independent of the useBeamSpot parameters
      m_hPvXZ         = makeAndRegisterTH2F(al_beamspot_shift,"pvXZ","Primary vertex: x vs z;z (mm);x (mm)",100,-500,500,100,-10,10);
      m_hPvYZ         = makeAndRegisterTH2F(al_beamspot_shift,"pvYZ","Primary vertex: y vs z;z (mm);y (mm)",100,-500,500,100,-10,10);
      m_hPvYX         = makeAndRegisterTH2F(al_beamspot_shift,"pvYX","Primary vertex: y vs x;x (mm);y (mm)",100,-10,10,100,-10,10);
      m_hPvN          = makeAndRegisterTH1F(al_beamspot_expert,"pvN","Number of vertices ("+m_vxContainerName.key()+", excluding dummy vertex);Number of vertices",100,0,100);
      m_hPvNPriVtx    = makeAndRegisterTH1F(al_beamspot_expert,"pvNPriVtx","Number of primary vertices ("+m_vxContainerName.key()+");Number of vertices",3,0,3);
      m_hPvNPileupVtx = makeAndRegisterTH1F(al_beamspot_expert,"pvNPileupVtx","Number of pileup vertices ("+m_vxContainerName.key()+");Number of vertices",100,0,100);
      m_hPvErrX       = makeAndRegisterTH1F(al_beamspot_expert,"pvErrX","Primary vertex: #sigma_{x}; #sigma_{x} (mm)",100,0,.5);
      m_hPvErrY       = makeAndRegisterTH1F(al_beamspot_expert,"pvErrY","Primary vertex: #sigma_{y}; #sigma_{y} (mm)",100,0,.5);
      m_hPvErrZ       = makeAndRegisterTH1F(al_beamspot_expert,"pvErrZ","Primary vertex: #sigma_{z}; #sigma_{z} (mm)",100,0,.5);
      m_hPvChiSqDoF   = makeAndRegisterTH1F(al_beamspot_shift,"pvChiSqDof","Primary vertex: #Chi^{2}/DoF of vertex fit;#Chi^{2}/DoF",100,0,10);
      m_hPvNTracksAll = makeAndRegisterTH1F(al_beamspot_expert,"pvNTracksAll","Number of tracks in primary vertex;Number of tracks",100,0,500);
      m_hPvNTracks    = makeAndRegisterTH1F(al_beamspot_expert,"pvNTracks","Number of tracks in primary vertex;Number of tracks",100,0,500);
      m_hPvTrackPt    = makeAndRegisterTH1F(al_beamspot_expert,"pvTrackPt","Primary vertex: original track p_{t};p_{t} (GeV)",100,0,20);
      m_hPvTrackEta   = makeAndRegisterTH1F(al_beamspot_expert,"pvTrackEta","Primary vertex: original track #eta; #eta",100,-3,3);
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode InDetGlobalBeamSpotMonTool::fillHistograms() {

  // Get beamspot information, if available
  float beamSpotX = 0.;
  float beamSpotY = 0.;
  float beamSpotZ = 0.;
  float beamTiltX = 0.;
  float beamTiltY = 0.;
  float scaleFactor = 1.;
  if (m_useBeamspot) {
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey };
    ATH_CHECK(beamSpotHandle.isValid());
    const Amg::Vector3D &bpos = beamSpotHandle->beamPos();

    beamSpotX = bpos.x();
    beamSpotY = bpos.y();
    beamSpotZ = bpos.z();
    beamTiltX = beamSpotHandle->beamTilt(0);
    beamTiltY = beamSpotHandle->beamTilt(1);
    scaleFactor = 1000.;   // Use microns for some histograms when showing distance relative to beamspot
    m_hBsX->Fill(beamSpotX);
    m_hBsY->Fill(beamSpotY);
    m_hBsZ->Fill(beamSpotZ);
    m_hBsTiltX->Fill(1e6*beamTiltX);
    m_hBsTiltY->Fill(1e6*beamTiltY);
    ATH_MSG_DEBUG("Beamspot from " << beamSpotHandle.retrieve() << ": x0 = " << beamSpotX << ", y0 = " << beamSpotY
          << ", z0 = " << beamSpotZ << ", tiltX = " << beamTiltX
          << ", tiltY = " << beamTiltY);
  }
    
  SG::ReadHandle<xAOD::TrackParticleContainer> trackCollection(m_trackContainerName);
  if(!(trackCollection.isValid())){
    ATH_MSG_DEBUG ("Could not retrieve TrackParticleContainer container with key "+m_trackContainerName.key());
    return StatusCode::SUCCESS;
  }
  
  // Track monitoring
  int nTracks = 0;
  for (xAOD::TrackParticleContainer::const_iterator trkItr = trackCollection->begin(); trkItr!=trackCollection->end(); ++trkItr) {

    const xAOD::TrackParticle* tpb = *trkItr;
    if (!tpb) {
      ATH_MSG_DEBUG( "Null pointer to TrackParticleBase" );
      continue;
    }    
    const Trk::Perigee* perigee = &(tpb->perigeeParameters());
    if (!perigee) {
      ATH_MSG_DEBUG( "Null pointer to track perigee" );
      continue;
    }

    float theta = perigee->parameters()[Trk::theta];
    float qOverPt = perigee->parameters()[Trk::qOverP]/sin(theta);
    float charge = perigee->charge();
    float z0 = perigee->parameters()[Trk::z0];
    float phi0 = perigee->parameters()[Trk::phi0];
    float d0 = perigee->parameters()[Trk::d0];
    if ( qOverPt != 0 ){
      float pT = (1/qOverPt)*(charge);
      // For all tracks
      m_hTrPt->Fill(pT/1000.);
  
      // Select tracks to use for remaining histograms
      if (pT<m_minTrackPt) continue;
    }

    nTracks++;
    m_hTrDPhi->Fill(phi0,d0*1e3);

    // Currently we do the direct calculation of d0corr. We could
    // also use an extrapolator to calculate d0 wrt a
    // Trk::StraightLineSurface constructed along the beam line.
    if(m_useBeamspot){
      float trkbeamlineTiltX=tpb->beamlineTiltX();
      float trkbeamlineTiltY=tpb->beamlineTiltY();
      float trkbeamspotx=tpb->vx();
      float trkbeamspoty=tpb->vy();
      float trkbeamspotz=tpb->vz();
      float beamX = (beamSpotX-trkbeamspotx) + tan(beamTiltX-trkbeamlineTiltX) * (z0-beamSpotZ+trkbeamspotz);
      float beamY = (beamSpotY-trkbeamspoty) + tan(beamTiltY-trkbeamlineTiltY) * (z0-beamSpotZ+trkbeamspotz);
      float d0corr = d0 - ( -sin(phi0)*beamX + cos(phi0)*beamY );
      m_hTrDPhiCorr->Fill(phi0,d0corr*1e3); 
    }
  }
  m_hTrNPt->Fill(nTracks);

  // Primary vertex monitoring - only if we have a primary vertex collection determined
  // without beam constraint
  if (! m_vxContainerWithBeamConstraint) {

    // Basic primary vertex monitoring
    SG::ReadHandle<xAOD::VertexContainer_v1> vxContainer(m_vxContainerName);
    if (!vxContainer.isValid()) {
      ATH_MSG_DEBUG ("Could not retrieve primary vertex container with key "+m_vxContainerName.key());
      return StatusCode::SUCCESS;
    }

    m_hPvN->Fill(vxContainer->size()-1);  // exclude dummy vertex
    int nPriVtx = 0;
    int nPileupVtx = 0;
    for (xAOD::VertexContainer_v1::const_iterator vxIter = vxContainer->begin(); vxIter != vxContainer->end(); ++vxIter) {
      if ( !(*vxIter) ) continue;

      // Count different types of vertices
      if ((*vxIter)->vertexType() == xAOD::VxType::PriVtx) nPriVtx++;
      if ((*vxIter)->vertexType() == xAOD::VxType::PileUp) nPileupVtx++;
      
      // Select primary vertex
      if ((*vxIter)->vertexType() != xAOD::VxType::PriVtx) continue;
      if ((*vxIter)->numberDoF() <= 0) continue;
      
      if ((*vxIter)->nTrackParticles() < m_minTracksPerVtx) continue;
      
      // Found good VxCandidate to monitor - now fill histograms
      float x = (*vxIter)->position().x();
      float y = (*vxIter)->position().y();
      float z = (*vxIter)->position().z();
      float beamX = beamSpotX + tan(beamTiltX) * (z-beamSpotZ);
      float beamY = beamSpotY + tan(beamTiltY) * (z-beamSpotZ);
      float beamZ = beamSpotZ;

      m_hPvX->Fill((x-beamX)*scaleFactor);
      m_hPvY->Fill((y-beamY)*scaleFactor);
      m_hPvZ->Fill(z-beamZ);
      m_hPvErrX->Fill( Amg::error( (*vxIter)->covariancePosition(), Trk::x) );
      m_hPvErrY->Fill( Amg::error( (*vxIter)->covariancePosition(), Trk::y) );
      m_hPvErrZ->Fill( Amg::error( (*vxIter)->covariancePosition(), Trk::z) );
      m_hPvChiSqDoF->Fill( (*vxIter)->chiSquared() / (*vxIter)->numberDoF() );
      m_hPvNTracks->Fill( (*vxIter)->nTrackParticles() );

      m_hPvXZ->Fill(z,x);
      m_hPvYZ->Fill(z,y);
      m_hPvYX->Fill(x,y);

      // Histograms on original tracks used for primary vertex
      for (unsigned int trkIter=0; trkIter!=(*vxIter)->nTrackParticles(); ++trkIter) {
        const xAOD::TrackParticle* tp = (*vxIter)->trackParticle(trkIter);
        if(!tp){
          ATH_MSG_DEBUG ("Could not retrieve track particle.");
          continue;
        }
        const Trk::Perigee measuredPerigee = tp->perigeeParameters();
        m_hPvTrackEta->Fill(measuredPerigee.eta());
        m_hPvTrackPt->Fill(measuredPerigee.pT()/1000.);   // Histo is in GeV, not MeV
      }
    }
    m_hPvNPriVtx->Fill(nPriVtx);
    m_hPvNPileupVtx->Fill(nPileupVtx);
  }

  return StatusCode::SUCCESS;
}


StatusCode InDetGlobalBeamSpotMonTool::procHistograms()
{
  return StatusCode::SUCCESS;
}


TH1F_LW* InDetGlobalBeamSpotMonTool::makeAndRegisterTH1F(MonGroup& mon,
               const char* hName, std::string hTitle, int nBins, float minX, float maxX) {
  TH1F_LW* h = TH1F_LW::create(hName,hTitle.c_str(),nBins,minX,maxX);
  //h->Sumw2();
  if (mon.regHist(h).isFailure()) {
    ATH_MSG_WARNING ("Unable to book histogram with name = "+std::string(hName));
  }
  return h;
}


TH2F_LW* InDetGlobalBeamSpotMonTool::makeAndRegisterTH2F(MonGroup& mon,
               const char* hName, std::string hTitle,
               int nBinsX, float minX, float maxX,
               int nBinsY, float minY, float maxY) {
  TH2F_LW* h = TH2F_LW::create(hName,hTitle.c_str(),nBinsX,minX,maxX,nBinsY,minY,maxY);
  //h->Sumw2();
  //h->SetOption("colz");
  if (mon.regHist(h).isFailure()) {
    ATH_MSG_WARNING ("Unable to book histogram with name = "+std::string(hName));
  }
  return h;
}
