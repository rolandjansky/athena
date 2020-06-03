// **********************************************************************
// IDPerfMonKshort.cxx
// AUTHORS: Jed Biesiada, Tobias Golling, Beate Heinemann
// **********************************************************************

#include <sstream>
#include <math.h>
#include <vector>

#include "TH1.h"
#include "TProfile.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TMath.h"
#include "TRandom.h"
#include "TTree.h"

#include "GaudiKernel/IJobOptionsSvc.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"

#include "TrkV0Vertex/V0Hypothesis.h"
#include "TrkV0Vertex/V0Candidate.h"
#include "TrkV0Vertex/V0Container.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"

// ATLAS headers
#include "GaudiKernel/IInterface.h"
#include "StoreGate/StoreGateSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "InDetPerformanceMonitoring/IDPerfMonKshort.h"


// *********************************************************************
// Public Methods
// *********************************************************************

IDPerfMonKshort::IDPerfMonKshort( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
  m_triggerChainName("NoTriggerSelection"),
  m_defaultTreeName("DefaultParams"),
  m_ValidationTreeDescription("Small Tree for Kshort mass fits"),
  m_trackToVertexTool("Reco::TrackToVertex")
{
  declareProperty("tracksName",m_tracksName);
  declareProperty("CheckRate",m_checkrate=1000);
  declareProperty("triggerChainName",m_triggerChainName);
  declareProperty("VxContainerName",m_VxContainerName=m_VxContainerName);
  declareProperty("VxPrimContainerName",m_VxPrimContainerName=m_VxPrimContainerName);
  declareProperty ("TrackToVertexTool", m_trackToVertexTool);
}

IDPerfMonKshort::~IDPerfMonKshort() { }


StatusCode IDPerfMonKshort::initialize()
{
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "IDPerfMonKshort initialize() started"<< endreq;

  m_histosBooked = 0;

  if (m_tracksName.empty())     if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << " no track collection given"<<endreq;

  StatusCode sc;
  sc = ManagedMonitorToolBase::initialize();
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "IDPerfMonKshort initialize() finished"<< endreq;
  if(!sc.isSuccess()) return StatusCode::SUCCESS;


  if (m_trackToVertexTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve " << m_trackToVertexTool );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Retrieved tool" << m_trackToVertexTool );
  }

  return StatusCode::SUCCESS;
}


StatusCode IDPerfMonKshort::bookHistograms()
{
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "IDPerfMonKshort bookHistograms() started"<< endreq;

  MonGroup al_kshort_mon ( this, "IDPerfMon/Kshort/" + m_triggerChainName, run);

  //ASK CONFIRMATION for ATTRIB_MANAGED
  MonGroup al_kshort_mon_average(this, "IDPerfMon/Kshort/" + m_triggerChainName,run,ATTRIB_MANAGED,"","weightedAverage");

  if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }

  if ( AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  if ( newLowStatFlag() || newLumiBlockFlag() ) {

  }

  if( newRunFlag() ) {

    //if user environment specified we don't want to book new histograms at every run boundary
    //we instead want one histogram per job
    if(m_histosBooked!=0 && AthenaMonManager::environment()==AthenaMonManager::user) return StatusCode::SUCCESS;

    m_mass = new TH1F("ks_mass", "Invariant mass of K^{0}_{S} candidate", 100, 0.3, 0.7);
    m_mass->SetYTitle("K^{0}_{S} Candidates");
    m_mass->SetXTitle("Mass (Gev / c^{2})");
    m_mass->SetMarkerStyle(20);
    m_mass->SetMinimum(0.);
    RegisterHisto(al_kshort_mon,m_mass) ;

    m_pt = new TH1F("ks_pt", "p_{T} of K^{0}_{S} candidate", 100, 0., 10.);
    m_pt->SetYTitle("K^{0}_{S} Candidates");
    m_pt->SetXTitle("p_{T} (Gev / c)");
    m_pt->SetMarkerStyle(20);
    RegisterHisto(al_kshort_mon,m_pt) ;

    m_radiusVsZ_secVertex = new TH2F("secVertex_radiusVsZ", "all sec.vertices (reco);z [mm];Decay radius [mm]",180, -600., 600.,180.,0.,180.);
    RegisterHisto(al_kshort_mon,m_radiusVsZ_secVertex) ;

    m_YVsX_secVertex = new TH2F("secVertex_YVsX", "all sec. vertices (reco);x [mm];y [mm]",200, -150.,150., 200, -150., 150.);
    RegisterHisto(al_kshort_mon,m_YVsX_secVertex) ;


    m_radiusVsZ_secVertex_sel = new TH2F("secVertex_radiusVsZ_sel", "all sec.vertices (reco);z [mm];Decay radius [mm]",180, -600., 600.,180.,0.,180.);
    RegisterHisto(al_kshort_mon,m_radiusVsZ_secVertex_sel) ;

    m_YVsX_secVertex_sel = new TH2F("secVertex_YVsX_sel", "all sec. vertices (reco);x [mm];y [mm]",200, -150.,150., 200, -150., 150.);
    RegisterHisto(al_kshort_mon,m_YVsX_secVertex_sel) ;


    m_radiusVsZ_secVertex_Ks = new TH2F("secVertex_radiusVsZ_Ks", "sec.vertices (reco) of K^{0}_{S} candidates;z [mm];Decay radius [mm]",180, -600., 600.,180.,0.,180.);
    RegisterHisto(al_kshort_mon,m_radiusVsZ_secVertex_Ks) ;

    m_YVsX_secVertex_Ks = new TH2F("secVertex_YVsX_Ks", "sec. vertices (reco) of K^{0}_{S} candidates;x [mm];y [mm]",200, -150.,150., 200, -150., 150.);
    RegisterHisto(al_kshort_mon,m_YVsX_secVertex_Ks) ;


    m_radius_secVertices = new TH1F("radius_secVertices", "Decay radius of secondary vertices", 600, 0., 300.);
    RegisterHisto(al_kshort_mon,m_radius_secVertices) ;

    m_radius_secVertices_sel = new TH1F("radius_secVertices_sel", "Decay radius of secondary vertices", 600, 0., 300.);
    RegisterHisto(al_kshort_mon,m_radius_secVertices_sel) ;


    m_YVsX_primVertex = new TH2F("primVertex_YVsX", "all primary vertices (reco);PV x [mm];PV y [mm]",300, -1.0,0.0, 300, -1.0, 0.0);
    RegisterHisto(al_kshort_mon,m_YVsX_primVertex) ;

    m_XVsZ_primVertex = new TH2F("primVertex_XVsZ", "all primary vertices (reco);PV z [mm];PV x [mm]",200, -200.,200, 300, -1.0, 0.0);
    RegisterHisto(al_kshort_mon,m_XVsZ_primVertex) ;

    m_YVsZ_primVertex = new TH2F("primVertex_YVsZ", "all primary vertices (reco);PV z [mm];PV y [mm]",200, -200.,200., 100, -1.0, 0.0);
    RegisterHisto(al_kshort_mon,m_YVsZ_primVertex) ;
    
    m_YVsX_primVertex_Ks = new TH2F("primVertex_YVsX_Ks", "all primary vertices (reco);PV x [mm];PV y [mm]",300, -1.0,0.0, 300, -1.0, 0.0);
    RegisterHisto(al_kshort_mon,m_YVsX_primVertex_Ks) ;

    m_XVsZ_primVertex_Ks = new TH2F("primVertex_XVsZ_Ks", "all primary vertices (reco);PV z [mm];PV x [mm]",200, -200.,200, 300, -1.0, 0.0);
    RegisterHisto(al_kshort_mon,m_XVsZ_primVertex_Ks) ;

    m_YVsZ_primVertex_Ks = new TH2F("primVertex_YVsZ_Ks", "all primary vertices (reco);PV z [mm];PV y [mm]",200, -200.,200., 100, -1.0, 0.0);
    RegisterHisto(al_kshort_mon,m_YVsZ_primVertex_Ks) ;

    m_radius = new TH1F("ks_radius", "Decay radius of K^{0}_{S} candidate", 100, 0., 300.);
    m_radius->SetYTitle("K^{0}_{S} Candidates");
    m_radius->SetXTitle("Decay Radius (mm)");
    m_radius->SetMarkerStyle(20);
    RegisterHisto(al_kshort_mon,m_radius) ;

    m_eta = new TH1F("ks_eta", "#eta of K^{0}_{S} candidate", 50, -2.5, 2.5);
    m_eta->SetYTitle("K^{0}_{S} Candidates");
    m_eta->SetXTitle("#eta");
    m_eta->SetMarkerStyle(20);
    RegisterHisto(al_kshort_mon,m_eta) ;

    m_phi = new TH1F("ks_phi", "#phi of K^{0}_{S} candidate", 50, (-1.0* TMath::Pi()), TMath::Pi());
    m_phi->SetYTitle("K^{0}_{S} Candidates");
    m_phi->SetXTitle("#phi");
    m_phi->SetMarkerStyle(20);
    RegisterHisto(al_kshort_mon,m_phi) ;

    m_Nevents = new TH1F("Nevents","Number of events processed",1,-.5,.5);
    RegisterHisto(al_kshort_mon,m_Nevents);

    m_Nevents_sel = new TH1F("NeventsSel","Number of events selected",1,-.5,.5);
    RegisterHisto(al_kshort_mon,m_Nevents_sel);

    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "IDPerfMonKshort bookHistograms done"<< endreq;

    m_histosBooked++;

    m_defaultTree = new TTree(m_defaultTreeName.c_str(), m_ValidationTreeDescription.c_str());

    m_defaultTree->Branch("runNumber"      ,  &m_runNumber,  "runNumber/I");
    m_defaultTree->Branch("eventNumber"    ,  &m_evtNumber,  "eventNumber/I");
    m_defaultTree->Branch("lumi_block"     ,  &m_lumi_block,  "lumi_block/I");

    m_defaultTree->Branch("Ks_x"     ,  &m_Ks_x,  "Ks_x/D");
    m_defaultTree->Branch("Ks_y"     ,  &m_Ks_y,  "Ks_y/D");
    m_defaultTree->Branch("Ks_z"     ,  &m_Ks_z,  "Ks_z/D");

    m_defaultTree->Branch("pv_x"     ,  &m_pv_x,  "pv_x/D");
    m_defaultTree->Branch("pv_y"     ,  &m_pv_y,  "pv_y/D");
    m_defaultTree->Branch("pv_z"     ,  &m_pv_z,  "pv_z/D");

    m_defaultTree->Branch("Negative_Px",  &m_negative_px,  "Negative_Px/D");
    m_defaultTree->Branch("Negative_Py",  &m_negative_py,  "Negative_Py/D");
    m_defaultTree->Branch("Negative_Pz",  &m_negative_pz,  "Negative_Pz/D");
    m_defaultTree->Branch("Negative_z0",  &m_negative_z0,  "Negative_z0/D");
    m_defaultTree->Branch("Negative_d0",  &m_negative_d0,  "Negative_d0/D");
    m_defaultTree->Branch("Negative_z0_err",  &m_negative_z0_err,  "Negative_z0_err/D");
    m_defaultTree->Branch("Negative_d0_err",  &m_negative_d0_err,  "Negative_d0_err/D");

    m_defaultTree->Branch("Positive_Px",  &m_positive_px,  "Positive_Px/D");
    m_defaultTree->Branch("Positive_Py",  &m_positive_py,  "Positive_Py/D");
    m_defaultTree->Branch("Positive_Pz",  &m_positive_pz,  "Positive_Pz/D");
    m_defaultTree->Branch("Positive_z0",  &m_positive_z0,  "Positive_z0/D");
    m_defaultTree->Branch("Positive_d0",  &m_positive_d0,  "Positive_d0/D");
    m_defaultTree->Branch("Positive_z0_err",  &m_positive_z0_err,  "Positive_z0_err/D");
    m_defaultTree->Branch("Positive_d0_err",  &m_positive_d0_err,  "Positive_d0_err/D");

    RegisterTree(al_kshort_mon,m_defaultTree);

    m_defaultTree->TTree::SetAutoFlush(-30000000);

  }

  return StatusCode::SUCCESS;
}

void IDPerfMonKshort::RegisterHisto(MonGroup& mon, TH1* histo) {
  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "IDPerfMonKshort RegisterHisto() started"<< endreq;

  histo->Sumw2();
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Cannot book TH1 Histogram:" << endreq;
  }
}

void IDPerfMonKshort::RegisterHisto(MonGroup& mon, TProfile* histo) {
  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "IDPerfMonKshort RegisterHisto() started"<< endreq;

  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Cannot book TProfile Histogram:" << endreq;
  }
}

void IDPerfMonKshort::RegisterHisto(MonGroup& mon, TGraph* graph) {
  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "IDPerfMonKshort RegisterHisto() started"<< endreq;

  StatusCode sc = mon.regGraph(graph);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Cannot book TGraph:" << endreq;
  }
}

void IDPerfMonKshort::RegisterTree(MonGroup& mon, TTree* tree) {
  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "IDPerfMonKshort RegisterTree() started"<< endreq;

  StatusCode sc = mon.regTree(tree);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Cannot book TTree:" << endreq;
  }
}


StatusCode IDPerfMonKshort::fillHistograms()
{
  if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "IDPerfMonKshort fillHistogram() started"<< endreq;

  const xAOD::TrackParticleContainer* tracks(0);
  StatusCode sc = evtStore()->retrieve(tracks,m_tracksName);
  if (sc.isFailure()) {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No Collection with name "<<m_tracksName<<" found in StoreGate" << endreq;
    return StatusCode::SUCCESS;
  } else {
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Collection with name "<<m_tracksName<<" found in StoreGate" << endreq;
  }

  const xAOD::VertexContainer* PrimVxContainer(0);
  if(evtStore()->contains<xAOD::VertexContainer>(m_VxPrimContainerName)){
    if ( evtStore()->retrieve(PrimVxContainer,m_VxPrimContainerName).isFailure()) {
      ATH_MSG_DEBUG("Could not retrieve collection with name "<<m_VxPrimContainerName<<" found in StoreGate");
      return false;
    }
    else
      ATH_MSG_DEBUG("Successfully retrieved collection with name "<<m_VxPrimContainerName);
  }
  else {
    ATH_MSG_DEBUG("No collection with name "<<m_VxPrimContainerName<<" found in StoreGate");
    return StatusCode::SUCCESS;
  }
  xAOD::Vertex *primaryVertex= std::begin(*PrimVxContainer)[0];


  const xAOD::VertexContainer* SecVxContainer(0);
    if(evtStore()->contains<xAOD::VertexContainer>(m_VxContainerName)){
      if (evtStore()->retrieve(SecVxContainer,m_VxContainerName).isFailure()) {
        ATH_MSG_DEBUG("Could not retrieve collection with name "<<m_VxContainerName<<" found in StoreGate");
        return false;
      }
      else ATH_MSG_DEBUG("Successfully retrieved collection with name "<<m_VxContainerName);
    }
    else {
      ATH_MSG_DEBUG("No collection with name "<<m_VxContainerName<<" found in StoreGate");
      return StatusCode::SUCCESS;
    }
  
  m_Nevents->Fill(0.);

  if(primaryVertex->nTrackParticles() > 3){
    ATH_MSG_DEBUG("NTrk of primary vertices : "<< primaryVertex->nTrackParticles());
    m_YVsX_primVertex->Fill(primaryVertex->position().x(),primaryVertex->position().y());
    m_XVsZ_primVertex->Fill(primaryVertex->position().z(),primaryVertex->position().x());
    m_YVsZ_primVertex->Fill(primaryVertex->position().z(),primaryVertex->position().y());
  }

  double ksMassPDG = 497.611;
  ATH_MSG_DEBUG("Looping over SecVxContainer name : "<< m_VxContainerName);
  ATH_MSG_DEBUG("V0UnconstrVerices container size >> " << SecVxContainer->size());

  for (const auto* secVx_elem : *SecVxContainer) {

    double ksMass = secVx_elem->auxdata< float >("Kshort_mass");
    double ksPt = secVx_elem->auxdata< float >("pT");
    double ksPx = secVx_elem->auxdata< float >("px");
    double ksPy = secVx_elem->auxdata< float >("py");
    double ksPz = secVx_elem->auxdata< float >("pz");

    CLHEP::Hep3Vector ksMomentumVector = CLHEP::Hep3Vector(ksPx,ksPy,ksPz);

    double ksMomentum = ksMomentumVector.mag();
    double transverseFlightDistance, totalFlightDistance;

    Amg::Vector3D flightVector;

    if(primaryVertex!=NULL) {
      auto vert = secVx_elem->position()-primaryVertex->position();
      double dx = vert.x();
      double dy = vert.y();
      Amg::Vector3D mom(ksPx,ksPy,ksPz);
      double dxy = (mom.x()*dx + mom.y()*dy)/mom.perp();
      transverseFlightDistance =dxy;

      Amg::Vector3D vertex(secVx_elem->position().x(),secVx_elem->position().y(),secVx_elem->position().z());
      totalFlightDistance = (vertex-primaryVertex->position()).mag();

      flightVector = vertex-primaryVertex->position();
    }

    else {
      transverseFlightDistance = secVx_elem->position().perp();
      Amg::Vector3D vertex(secVx_elem->position().x(),secVx_elem->position().y(),secVx_elem->position().z());
      totalFlightDistance = vertex.mag();
      flightVector = vertex;
    }

    double properDecayTime = 1./Gaudi::Units::c_light*ksMassPDG/ksMomentum*totalFlightDistance;

    double flightX = flightVector.x();
    double flightY = flightVector.y();
    double cosThetaPointing = (ksPx*flightX+ksPy*flightY)/sqrt(ksPx*ksPx+ksPy*ksPy)/sqrt(flightX*flightX+flightY*flightY);
    int trackPos_nSVTHits = 0;
    int trackNeg_nSVTHits = 0;
    double trackPos_d0 = 0;
    double trackPos_d0_wrtPV = 0;
    double trackNeg_d0 = 0;
    double trackNeg_d0_wrtPV = 0;
    const xAOD::TrackParticle* trackPos(0);
    const xAOD::TrackParticle* trackNeg(0);

    int ntrk(-1);
    ntrk = secVx_elem->nTrackParticles();
    ATH_MSG_DEBUG("track particles associated to vertex : "<<ntrk );


    if(ntrk>0){
      auto tpLinks = secVx_elem->trackParticleLinks();
      for (auto link: tpLinks){
        if (link.isValid()) {
          if(ntrk == 2){
            ATH_MSG_DEBUG("Exactly two track particles!");
            if( (*link)->charge() > 0. ) {
              trackPos = *link;
              ATH_MSG_DEBUG("Track with positive charge!");
            } else if( (*link)->charge() < 0. ){
              trackNeg = *link;
              ATH_MSG_DEBUG("Track with negative charge!");
            }
          }
        } else ATH_MSG_ERROR("Could not retrieve track particle.");
      }
    }


    if(trackPos!=0) {
      uint8_t dummy(-1);
      trackPos_nSVTHits = trackPos->summaryValue(  dummy , xAOD::numberOfSCTHits  )? dummy :-1;
      trackPos_d0 = trackPos->d0();
      trackPos_d0_wrtPV = trackPos->d0() - (primaryVertex->position().y()*cos(trackPos->phi0()) - primaryVertex->position().x()*sin(trackPos->phi0()));
    }


    if(trackNeg!=0) {
      uint8_t dummy(-1);
      trackNeg_nSVTHits = trackNeg->summaryValue(  dummy , xAOD::numberOfSCTHits  )? dummy :-1;
      trackNeg_d0 = trackNeg->d0();
      trackNeg_d0_wrtPV = trackNeg->d0() - (primaryVertex->position().y()*cos(trackNeg->phi0()) - primaryVertex->position().x()*sin(trackNeg->phi0()));
    }

    int selectorValue = 0;

    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ksTau = " << properDecayTime << " Lxy = " <<transverseFlightDistance<< " cosTheta = " << cosThetaPointing <<endreq;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "trackPos nSVThits = " << trackPos_nSVTHits << " trackNeg nSVThits = " << trackNeg_nSVTHits <<endreq;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ksPt = " << ksPt <<endreq;


    double secVertex_radius = secVx_elem->auxdata< float >("Rxy");
    ATH_MSG_DEBUG("secondary vertex radius : " << secVertex_radius);
    m_radius_secVertices->Fill(secVertex_radius);
    m_radiusVsZ_secVertex->Fill(secVx_elem->position().z(),secVertex_radius);
    m_YVsX_secVertex->Fill(secVx_elem->position().x(),secVx_elem->position().y());


    ATH_MSG_DEBUG("trackneg d0 : " << trackNeg_d0 << " trackpos d0 : "<< trackPos_d0);
    ATH_MSG_DEBUG("trackneg d0 (PV): " << trackNeg_d0_wrtPV << " trackpos d0 (PV) : "<< trackPos_d0_wrtPV);

    if(secVx_elem->chiSquared()/secVx_elem->numberDoF() < 4.5
      && ksPt > 300.
      && abs(trackNeg_d0_wrtPV) > 5.
      && abs(trackPos_d0_wrtPV) > 5.
      && trackPos_nSVTHits > 2
      && trackNeg_nSVTHits > 2
      && secVertex_radius > 20.
    ){
      m_radius_secVertices_sel->Fill(secVertex_radius);
      m_radiusVsZ_secVertex_sel->Fill(secVx_elem->position().z(),secVertex_radius);
      m_YVsX_secVertex_sel->Fill(secVx_elem->position().x(),secVx_elem->position().y());
    }



    // if( 1
    //   && properDecayTime > 0.004
    //   && transverseFlightDistance > 12.
    //   && cosThetaPointing > 0.998
    //   && ksMass>400.&&ksMass<600.
    //   && trackPos_nSVTHits > 2 && trackNeg_nSVTHits > 2
    // ) selectorValue = 1;

    if( 1
      && transverseFlightDistance > 4.
      && cosThetaPointing > 0.999
      && ksMass>300.&&ksMass<700.
      && trackPos_nSVTHits > 2 && trackNeg_nSVTHits > 2
    ) selectorValue = 1;

    if(selectorValue != 1) continue;

    m_radiusVsZ_secVertex_Ks->Fill(secVx_elem->position().z(),secVertex_radius);
    m_YVsX_secVertex_Ks->Fill(secVx_elem->position().x(),secVx_elem->position().y());

    m_YVsX_primVertex_Ks->Fill(primaryVertex->position().x(),primaryVertex->position().y());
    m_XVsZ_primVertex_Ks->Fill(primaryVertex->position().z(),primaryVertex->position().x());
    m_YVsZ_primVertex_Ks->Fill(primaryVertex->position().z(),primaryVertex->position().y());

    m_Ks_x = secVx_elem->position().x();
    m_Ks_y = secVx_elem->position().y();
    m_Ks_z = secVx_elem->position().z();

    m_pv_x = primaryVertex->position().x();
    m_pv_y = primaryVertex->position().y();
    m_pv_z = primaryVertex->position().z();


    m_mass->Fill(ksMass/1000.);

    double ksEta = ksMomentumVector.pseudoRapidity();
    double ksPhi = ksMomentumVector.phi();

    // double piPlusPt = trackPos->p4().Perp();
    // double piMinusPt = trackNeg->p4().Perp();

    m_pt->Fill(ksPt/1000.);
    m_eta->Fill(ksEta);
    m_phi->Fill(ksPhi);

    double radius = secVx_elem->auxdata< float >("Rxy");;

    m_radius->Fill(radius);

    m_Nevents_sel->Fill(0.);

    ATH_MSG_DEBUG("Retrieving event info.");
    const EventInfo * eventInfo;
    if (evtStore()->retrieve(eventInfo).isFailure())
      ATH_MSG_ERROR("Could not retrieve event info.");
    else
    {
      m_runNumber = eventInfo->event_ID()->run_number();
      m_evtNumber = eventInfo->event_ID()->event_number();
      m_lumi_block = eventInfo->event_ID()->lumi_block();
      ATH_MSG_DEBUG(" Execute() starting on --> Run: " << m_runNumber << "  event: " << m_evtNumber);
    }

    const Trk::TrackParameters* bPer1;
    const Trk::TrackParameters* bPer2;

    if (  secVx_elem->vxTrackAtVertexAvailable() ) // This will run in the reco from raw
    {
      ATH_MSG_DEBUG("Using vxTrackAtVertex");
      bPer1 = secVx_elem->vxTrackAtVertex()[0].perigeeAtVertex();                                                                                                                    
      bPer2 = secVx_elem->vxTrackAtVertex()[1].perigeeAtVertex();
    }
    else // This from ESD, for now
    {
      ATH_MSG_DEBUG("Using trackToVertexTool");
      bPer1 = m_trackToVertexTool->perigeeAtVertex( *trackPos, secVx_elem->position() );
      bPer2 = m_trackToVertexTool->perigeeAtVertex( *trackNeg, secVx_elem->position() );
    }

    
    m_positive_px = bPer1->momentum().x();
    m_positive_py = bPer1->momentum().y();
    m_positive_pz = bPer1->momentum().z();
    m_positive_d0 = bPer1->parameters()[Trk::d0];
    m_positive_z0 = bPer1->parameters()[Trk::z0];

    const AmgSymMatrix(5)* matrix_positive = bPer1->covariance();
    if (matrix_positive){
      m_positive_d0_err = sqrt( (*matrix_positive)(Trk::d0,Trk::d0) );
      m_positive_z0_err = sqrt( (*matrix_positive)(Trk::z0,Trk::z0) );
    }

    m_negative_px = bPer2->momentum().x();
    m_negative_py = bPer2->momentum().y();
    m_negative_pz = bPer2->momentum().z();
    m_negative_d0 = bPer2->parameters()[Trk::d0];
    m_negative_z0 = bPer2->parameters()[Trk::z0];

    const AmgSymMatrix(5)* matrix_negative = bPer2->covariance();
    if (matrix_negative){
      m_negative_d0_err = sqrt( (*matrix_negative)(Trk::d0,Trk::d0) );
      m_negative_z0_err = sqrt( (*matrix_negative)(Trk::z0,Trk::z0) );
    }

    m_defaultTree->Fill();

  }
  
  return StatusCode::SUCCESS;
}

