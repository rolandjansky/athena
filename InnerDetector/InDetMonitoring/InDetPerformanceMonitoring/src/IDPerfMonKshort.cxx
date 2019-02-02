/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// IDPerfMonKshort.cxx
// AUTHORS: Jed Biesiada, Tobias Golling, Beate Heinemann
// **********************************************************************

#include <cmath>
#include <vector>
#include <algorithm>

#include "TH1.h"
#include "TProfile.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TMath.h"
#include "TRandom.h"


#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/LocalParameters.h"

#include "TrkV0Vertex/V0Hypothesis.h"
#include "TrkV0Vertex/V0Candidate.h"
#include "TrkV0Vertex/V0Container.h"
#include "TrkVertexAnalysisUtils/V0Tools.h"

// ATLAS headers
#include "GaudiKernel/IInterface.h"
#include "StoreGate/StoreGateSvc.h"

#include "InDetPerformanceMonitoring/IDPerfMonKshort.h"


namespace{ //utility functions
  //fill a container with a sequence of values
  template <typename ForwardIterator, typename T>
  void
  sequentialFill(ForwardIterator beg, const ForwardIterator stop,  T value, const T inc){
    while (beg!=stop){
      *beg++ = value;
      value +=inc;
    };
    return;
  }

  //find index in sorted array where the given value lies just above the indexed value
  template <typename Iterator, typename T, typename Compare=std::less<T>>
  long 
  findLevel(const Iterator beg, const Iterator stop, T value, Compare op=std::less<T>()){
    auto place = std::lower_bound(beg, stop, value, op);
    return std::distance(beg, place );
  }
  //in range?
  template <typename T>
  bool
  inRange(const T &v, const T &lo, const T &hi){
    return (v >= lo) and (v<=hi);
  }

}

// *********************************************************************
// Public Methods
// *********************************************************************



IDPerfMonKshort::IDPerfMonKshort( const std::string & type, const std::string & name, const IInterface* parent )
	     :ManagedMonitorToolBase( type, name, parent ),
	      m_triggerChainName("NoTriggerSelection")

{
  declareProperty("tracksName",m_tracksName);
  declareProperty("CheckRate",m_checkrate=1000);
  declareProperty("triggerChainName",m_triggerChainName);
  declareProperty("VxContainerName",m_VxContainerName="V0UnconstrVertices");
  declareProperty("VxPrimContainerName",m_VxPrimContainerName="PrimaryVertices");
}

IDPerfMonKshort::~IDPerfMonKshort() { }


StatusCode IDPerfMonKshort::initialize()
{
  ATH_MSG_DEBUG( "IDPerfMonKshort initialize() started");
  m_histosBooked = 0;
  if (m_tracksName.empty())     ATH_MSG_ERROR( " no track collection given");
  StatusCode sc;
  sc = ManagedMonitorToolBase::initialize();
  ATH_MSG_DEBUG( "IDPerfMonKshort initialize() finished");
  return StatusCode::SUCCESS;
}


StatusCode IDPerfMonKshort::bookHistograms()
{
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "IDPerfMonKshort bookHistograms() started"<< endmsg;
  MonGroup al_kshort_mon ( this, "IDPerfMon/Kshort/" + m_triggerChainName, run);

 
  //ASK CONFIRMATION for ATTRIB_MANAGED
  MonGroup al_kshort_mon_average(this, "IDPerfMon/Kshort/" + m_triggerChainName,run,ATTRIB_MANAGED,"","weightedAverage");


  if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }

  if ( AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }





  if( newRunFlag() ) {

    //if user environment specified we don't want to book new histograms at every run boundary
    //we instead want one histogram per job
    if(m_histosBooked!=0 && AthenaMonManager::environment()==AthenaMonManager::user) return StatusCode::SUCCESS;

    Double_t ptBins[m_nFittedBinsPt] = {1.05,1.85,2.45,3.35,4.5};
    Double_t radiusBins[m_nFittedBinsRadius] = {15.,35.,50.,70.,90.,120.,185.};
    Double_t phiBins[10]{};
    Double_t etaBins[10]{};
    Double_t curvatureDiffBins[6]{};
    sequentialFill(phiBins, std::end(phiBins), (-4.5*M_PI/5.), (M_PI/5.));
    sequentialFill(etaBins, std::end(etaBins),-2.25, 0.5); 
    sequentialFill(curvatureDiffBins, std::end(curvatureDiffBins), -0.001,0.0004);
    m_mass = new TH1F("ks_mass", "Invariant mass of K^{0}_{S} candidate", 60, 0.45, 0.55);
    m_mass->SetYTitle("K^{0}_{S} Candidates");
    m_mass->SetXTitle("Mass (Gev / c^{2})");
    m_mass->SetMarkerStyle(20);
    m_mass->SetMinimum(0.);
    RegisterHisto(al_kshort_mon,m_mass) ;
    m_mass_scaled = new TH1F("ks_mass_scaled", "Invariant mass of K^{0}_{S} candidate scaled to per event", 60, 0.45, 0.55);
    m_mass_scaled->SetYTitle("K^{0}_{S} Candidates");
    m_mass_scaled->SetXTitle("Mass (Gev / c^{2})");
    m_mass_scaled->SetMarkerStyle(20);
    m_mass_scaled->SetMinimum(0.);
    RegisterHisto(al_kshort_mon,m_mass_scaled) ;

    m_massVsPhi = new TH2F("ks_massVsPhi", "Invariant mass - world average of K^{0}_{S} candidate", 10, (-1.0* M_PI), M_PI, 50, -.5, .5);
    m_massVsPhi->SetXTitle("#phi");
    m_massVsPhi->SetYTitle("Mass (Gev / c^{2}) - World Average [MeV]");
    RegisterHisto(al_kshort_mon,m_massVsPhi) ;

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

    m_YVsX_primVertex = new TH2F("primVertex_YVsX", "all primary vertices (reco);PV x [mm];PV y [mm]",300, -1.5,1.5, 300, -1.5, 1.5);
    RegisterHisto(al_kshort_mon,m_YVsX_primVertex) ;

    m_XVsZ_primVertex = new TH2F("primVertex_XVsZ", "all primary vertices (reco);PV z [mm];PV x [mm]",200, -350.,350, 300, -1.5, 1.5);
    RegisterHisto(al_kshort_mon,m_XVsZ_primVertex) ;

    m_YVsZ_primVertex = new TH2F("primVertex_YVsZ", "all primary vertices (reco);PV z [mm];PV y [mm]",200, -350.,350., 100, -1.5, 1.5);
    RegisterHisto(al_kshort_mon,m_YVsZ_primVertex) ;

    m_YVsX_primVertex_Ks = new TH2F("primVertex_YVsX_Ks", "all primary vertices (reco);PV x [mm];PV y [mm]",300, -1.5,1.5, 300, -1.5, 1.5);
    RegisterHisto(al_kshort_mon,m_YVsX_primVertex_Ks) ;

    m_XVsZ_primVertex_Ks = new TH2F("primVertex_XVsZ_Ks", "all primary vertices (reco);PV z [mm];PV x [mm]",200, -350.,350, 300, -1.5, 1.5);
    RegisterHisto(al_kshort_mon,m_XVsZ_primVertex_Ks) ;

    m_YVsZ_primVertex_Ks = new TH2F("primVertex_YVsZ_Ks", "all primary vertices (reco);PV z [mm];PV y [mm]",200, -350.,350., 100, -1.5, 1.5);
    RegisterHisto(al_kshort_mon,m_YVsZ_primVertex_Ks) ;

    m_radius = new TH1F("ks_radius", "Decay radius of K^{0}_{S} candidate", 100, 0., 300.);
    m_radius->SetYTitle("K^{0}_{S} Candidates");
    m_radius->SetXTitle("Decay Radius (mm)");
    m_radius->SetMarkerStyle(20);
    RegisterHisto(al_kshort_mon,m_radius) ;


    m_eta = new TH1F("ks_eta", "#eta of K^{0}_{S} candidate", 10, -2.5, 2.5);
    m_eta->SetYTitle("K^{0}_{S} Candidates");
    m_eta->SetXTitle("#eta");
    m_eta->SetMarkerStyle(20);
    RegisterHisto(al_kshort_mon,m_eta) ;
    m_phi = new TH1F("ks_phi", "#phi of K^{0}_{S} candidate", 10, (-1.0* M_PI), M_PI);
    m_phi->SetYTitle("K^{0}_{S} Candidates");
    m_phi->SetXTitle("#phi");
    m_phi->SetMarkerStyle(20);
    RegisterHisto(al_kshort_mon,m_phi) ;
    //
    //lamda for use below
    auto quickSet=[](TH1F * h){
      h->SetXTitle("Mass (Gev / c^{2})");
      h->SetYTitle("K^{0}_{S} Candidates");
      h->SetMarkerStyle(20);
      h->SetMinimum(0.);
    };
    //
    for(int quickInit=0;quickInit<m_nBinsPt;quickInit++) {
      TString tempName = "MassVptBin";
      TString tempTitle = "Mass, p_{T} = ";
      tempName += quickInit;
      tempTitle += ((Double_t)((quickInit*100)+500))/1000;
      tempTitle += " GeV";
      m_massVPtBinHistos[quickInit] = new TH1F(tempName.Data(),tempTitle.Data(),50,0.445,0.555);
      quickSet(m_massVPtBinHistos[quickInit]);
      
      RegisterHisto(al_kshort_mon,m_massVPtBinHistos[quickInit]) ;
    }
     for(int quickInit=0;quickInit<m_nFittedBinsPt;quickInit++) {
      TString tempName = "MassVptBinFitted";
      TString tempTitle = "Fitted Mass, p_{T} = ";
      tempName += quickInit;
      tempTitle += ptBins[quickInit];
      tempTitle += " GeV";
      m_massVPtBinFittedHistos[quickInit] = new TH1F(tempName.Data(),tempTitle.Data(),50,0.445,0.555);
      quickSet(m_massVPtBinFittedHistos[quickInit]);
      RegisterHisto(al_kshort_mon,m_massVPtBinFittedHistos[quickInit]) ;
    }
   for(int quickInit=0;quickInit<m_nBinsRadius;quickInit++) {
      TString tempName = "MassVradiusBin";
      TString tempTitle = "Mass, Decay Radius = ";
      tempName += quickInit;
      tempTitle += quickInit*10;
      tempTitle += " mm";
      m_massVRadiusBinHistos[quickInit] = new TH1F(tempName.Data(),tempTitle.Data(),50,0.445,0.555);
      quickSet(m_massVRadiusBinHistos[quickInit]);
      RegisterHisto(al_kshort_mon,m_massVRadiusBinHistos[quickInit]) ;
    }
   for(int quickInit=0;quickInit<m_nFittedBinsRadius;quickInit++) {
      TString tempName = "MassVradiusBinFitted";
      TString tempTitle = "Fitted Mass, Decay Radius = ";
      tempName += quickInit;
      tempTitle += radiusBins[quickInit];
      tempTitle += " mm";
      m_massVRadiusBinFittedHistos[quickInit] = new TH1F(tempName.Data(),tempTitle.Data(),50,0.445,0.555);
      quickSet(m_massVRadiusBinFittedHistos[quickInit]);
      RegisterHisto(al_kshort_mon,m_massVRadiusBinFittedHistos[quickInit]) ;
    }
    for(int quickInit=0;quickInit<10;quickInit++) {
      TString tempName = "MassVEtaBin";
      TString tempTitle = "Mass, #eta =  ";
      tempName += quickInit;
      tempTitle += etaBins[quickInit];
      m_massVEtaBinHistos[quickInit] = new TH1F(tempName.Data(),tempTitle.Data(),50,0.445,0.555);
      quickSet(m_massVEtaBinHistos[quickInit]) ;
      RegisterHisto(al_kshort_mon,m_massVEtaBinHistos[quickInit]) ;

      tempName = "MassVPhiBin";
      tempTitle = "Mass, #phi = ";
      tempName += quickInit;
      tempTitle += ((Double_t)((Int_t)(phiBins[quickInit]*100)))/100;
      m_massVPhiBinHistos[quickInit] = new TH1F(tempName.Data(),tempTitle.Data(),50,0.445,0.555);
      quickSet(m_massVPhiBinHistos[quickInit]);
      RegisterHisto(al_kshort_mon,m_massVPhiBinHistos[quickInit]) ;
    }

   for(int quickInit=0;quickInit<6;quickInit++) {
      TString tempName = "MassVCurvatureDiffBin";
      TString tempTitle = "Mass, CurvatureDiff =  ";
      tempName += quickInit;
      tempTitle += curvatureDiffBins[quickInit];
      m_massVCurvatureDiffBinHistos[quickInit] = new TH1F(tempName.Data(),tempTitle.Data(),50,0.445,0.555);
      quickSet(m_massVCurvatureDiffBinHistos[quickInit]);
      RegisterHisto(al_kshort_mon,m_massVCurvatureDiffBinHistos[quickInit]) ;
    }

    m_massVersusPt_merged = new TH1F("KsMassVersusPt_Merged","Dummy TH1F Before Merging",10,-1,1);
    m_widthVersusPt_merged = new TH1F("KsWidthVersusPt_Merged","Dummy TH1F Before Merging",10,-1,1);
    m_massVersusRadius_merged = new TH1F("KsMassVersusRadius_Merged","Dummy TH1F Before Merging",10,-1,1);
    m_widthVersusRadius_merged = new TH1F("KsWidthVersusRadius_Merged","Dummy TH1F Before Merging",10,-1,1);
    m_massVersusEta_merged = new TH1F("KsMassVersusEta_Merged","Dummy TH1F Before Merging",10,-1,1);
    m_widthVersusEta_merged = new TH1F("KsWidthVersusEta_Merged","Dummy TH1F Before Merging",10,-1,1);
    m_massVersusPhi_merged = new TH1F("KsMassVersusPhi_Merged","Dummy TH1F Before Merging",10,-1,1);
    m_widthVersusPhi_merged = new TH1F("KsWidthVersusPhi_Merged","Dummy TH1F Before Merging",10,-1,1);
    m_massVersusCurvatureDiff_merged = new TH1F("KsMassVersusCurvatureDiff_Merged","Dummy TH1F Before Merging",10,-1,1);
    m_widthVersusCurvatureDiff_merged = new TH1F("KsWidthVersusCurvatureDiff_Merged","Dummy TH1F Before Merging",10,-1,1);

    RegisterHisto(al_kshort_mon_average,m_massVersusPt_merged) ;
    RegisterHisto(al_kshort_mon_average,m_widthVersusPt_merged) ;
    RegisterHisto(al_kshort_mon_average,m_massVersusRadius_merged) ;
    RegisterHisto(al_kshort_mon_average,m_widthVersusRadius_merged) ;
    RegisterHisto(al_kshort_mon_average,m_massVersusEta_merged) ;
    RegisterHisto(al_kshort_mon_average,m_widthVersusEta_merged) ;
    RegisterHisto(al_kshort_mon_average,m_massVersusPhi_merged) ;
    RegisterHisto(al_kshort_mon_average,m_widthVersusPhi_merged) ;
    RegisterHisto(al_kshort_mon_average,m_massVersusCurvatureDiff_merged) ;
    RegisterHisto(al_kshort_mon_average,m_widthVersusCurvatureDiff_merged) ;

    m_Nevents = new TH1F("Nevents","Number of events processed",1,-.5,.5);
    RegisterHisto(al_kshort_mon,m_Nevents);

    ATH_MSG_DEBUG( "IDPerfMonKshort bookHistograms done");

    m_histosBooked++;

  }



  return StatusCode::SUCCESS;
}

void IDPerfMonKshort::RegisterHisto(MonGroup& mon, TH1* histo) {
  ATH_MSG_VERBOSE( "IDPerfMonKshort RegisterHisto() started");
  histo->Sumw2();
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    ATH_MSG_ERROR( "Cannot book TH1 Histogram:" );
  }
}

void IDPerfMonKshort::RegisterHisto(MonGroup& mon, TProfile* histo) {
  ATH_MSG_VERBOSE( "IDPerfMonKshort RegisterHisto() started");
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    ATH_MSG_ERROR( "Cannot book TProfile Histogram:" );
  }
}

void IDPerfMonKshort::RegisterHisto(MonGroup& mon, TGraph* graph) {
  ATH_MSG_VERBOSE( "IDPerfMonKshort RegisterHisto() started");
  StatusCode sc = mon.regGraph(graph);
  if (sc.isFailure() ) {
    ATH_MSG_ERROR( "Cannot book TGraph:" );
  }
}


StatusCode IDPerfMonKshort::fillHistograms()
{
  ATH_MSG_VERBOSE( "IDPerfMonKshort fillHistogram() started");
  const xAOD::TrackParticleContainer* tracks(0);
  StatusCode sc = evtStore()->retrieve(tracks,m_tracksName);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG( "No Collection with name "<<m_tracksName<<" found in StoreGate" );
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG( "Collection with name "<<m_tracksName<<" found in StoreGate" );
  }

  const xAOD::VertexContainer* PrimVxContainer(0);
  if(evtStore()->contains<xAOD::VertexContainer>(m_VxPrimContainerName)){
    if ( evtStore()->retrieve(PrimVxContainer,m_VxPrimContainerName).isFailure()) {
      ATH_MSG_DEBUG("Could not retrieve collection with name "<<m_VxPrimContainerName<<" found in StoreGate");
      return StatusCode::FAILURE;
    }
    else
      ATH_MSG_DEBUG("Successfully retrieved collection with name "<<m_VxPrimContainerName);
  }
  else {
    ATH_MSG_DEBUG("No collection with name "<<m_VxPrimContainerName<<" found in StoreGate");
    return StatusCode::SUCCESS;
  }
  const xAOD::Vertex *primaryVertex= std::begin(*PrimVxContainer)[0];

  const xAOD::VertexContainer* SecVxContainer(0);
 if(evtStore()->contains<xAOD::VertexContainer>(m_VxContainerName)){
   if (evtStore()->retrieve(SecVxContainer,m_VxContainerName).isFailure()) {
     ATH_MSG_DEBUG("Could not retrieve collection with name "<<m_VxContainerName<<" found in StoreGate");
     return StatusCode::FAILURE;
   }
   else
     ATH_MSG_DEBUG("Successfully retrieved collection with name "<<m_VxContainerName);
 }
 else {
   ATH_MSG_DEBUG("No collection with name "<<m_VxContainerName<<" found in StoreGate");
   return StatusCode::SUCCESS;
 }

 m_Nevents->Fill(0.);
  double ksMassPDG = 497.648;
  ATH_MSG_DEBUG("@todo : masspdf" <<ksMassPDG );
  ATH_MSG_DEBUG("@todo Looping over SecVxContainer name : "<< m_VxContainerName);
  ATH_MSG_DEBUG("@todo >> V0UnconstrVerices container size >> " << SecVxContainer->size());
  //bin curvature from -0.0008 to 0.0008 in bins of 0.0004
  std::array<float, 5> curvatureBinning{};
  sequentialFill(curvatureBinning.begin(), curvatureBinning.end(),-0.0008, 0.0004);
  //bin phi from -pi to pi in bins of pi/5
  std::array<double,10> ksPhiBinning{};
  sequentialFill(ksPhiBinning.begin(), ksPhiBinning.end(), -M_PI, M_PI * 0.2);
  //bin ksEta from -2.5 to 2.5 in bins of 0.5
  std::array<double,10> ksEtaBinning{};
  sequentialFill(ksEtaBinning.begin(), ksEtaBinning.end(),-2.5, 0.5);
  //bin ksPt, variable bins
  std::array<double,4> ksPtBinning{1600,2100, 2800,3900};
  //bin radius, variable binning
  std::array<double,6> radiusBinning{30,40, 60,80,100,140};
  //
  for (const auto* secVx_elem : *SecVxContainer) {
   ATH_MSG_DEBUG("Looping over SecVxContainer name : "<< m_VxContainerName);
   double ksMass = secVx_elem->auxdata< float >("Kshort_mass");
   double ksPt = secVx_elem->auxdata< float >("pT");
   double ksPx = secVx_elem->auxdata< float >("px");
   double ksPy = secVx_elem->auxdata< float >("py");
   double ksPz = secVx_elem->auxdata< float >("pz");
   ATH_MSG_DEBUG( " mass : "<<ksMass << " pt : "<< ksPt << " px : "<< ksPx <<  " py : "<< ksPy << " pz : "<< ksPz);
   CLHEP::Hep3Vector ksMomentumVector = CLHEP::Hep3Vector(ksPx,ksPy,ksPz);
   double ksMomentum = ksMomentumVector.mag();
   double transverseFlightDistance, totalFlightDistance;
   Amg::Vector3D flightVector;
   const auto & secVxPosition(secVx_elem->position());
   if(primaryVertex) {
     if(primaryVertex->nTrackParticles() > 3){
       ATH_MSG_DEBUG("NTrk of primary vertices : "<< primaryVertex->nTrackParticles());
       const auto & position (primaryVertex->position());
       m_YVsX_primVertex->Fill(position.x(),position.y());
       m_XVsZ_primVertex->Fill(position.z(),position.x());
       m_YVsZ_primVertex->Fill(position.z(),position.y());
     }
     auto vert = secVx_elem->position()-primaryVertex->position();
     double dx = vert.x();
     double dy = vert.y();
     Amg::Vector3D mom(ksPx,ksPy,ksPz);
     double dxy = (mom.x()*dx + mom.y()*dy)/mom.perp();
     transverseFlightDistance =dxy;
     Amg::Vector3D vertex(secVxPosition.x(),secVxPosition.y(),secVxPosition.z());
     totalFlightDistance = (vertex-primaryVertex->position()).mag();
     flightVector = vertex-primaryVertex->position();
     ATH_MSG_DEBUG("dx : "<<dx<<" dy: "<<dy<<" dxy: "<<dxy<< "flight distance (total): "<<totalFlightDistance);
   }
   else {
     transverseFlightDistance = secVxPosition.perp();
     Amg::Vector3D vertex(secVxPosition.x(),secVxPosition.y(),secVxPosition.z());
     totalFlightDistance = vertex.mag();
     flightVector = vertex;
   }
   double properDecayTime = 1./Gaudi::Units::c_light*ksMassPDG/ksMomentum*totalFlightDistance;


   double flightX = flightVector.x();
   double flightY = flightVector.y();
   double cosThetaPointing = (ksPx*flightX+ksPy*flightY)/std::sqrt(ksPx*ksPx+ksPy*ksPy)/std::sqrt(flightX*flightX+flightY*flightY);
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
       Info("execute()", "V0: TP link = %d %s ", link.isValid(), link.dataID().c_str() );
       if(ntrk == 2){
	 ATH_MSG_DEBUG("Exactly two track particles!");
	 if( (*link)->charge() > 0. ) {
	   trackPos = *link;
	   ATH_MSG_DEBUG("Track with positive charge!");
	 }
	 else if( (*link)->charge() < 0. ){
	   trackNeg = *link;
	   ATH_MSG_DEBUG("Track with negative charge!");	   }
       }
     }//trackparticles
   }//ntrk
   if(trackPos!=0) {
      uint8_t dummy(-1);
      trackPos_nSVTHits = trackPos->summaryValue(  dummy , xAOD::numberOfSCTHits  )? dummy :-1;
      trackPos_d0 = trackPos->d0();
      trackPos_d0_wrtPV = trackPos->d0() - (primaryVertex->position().y()*std::cos(trackPos->phi0()) - primaryVertex->position().x()*std::sin(trackPos->phi0()));

    }
    //std::cout <<"@todo : check (2) " << std::endl;

    if(trackNeg!=0) {
      uint8_t dummy(-1);
      trackNeg_nSVTHits = trackNeg->summaryValue(  dummy , xAOD::numberOfSCTHits  )? dummy :-1;
      trackNeg_d0 = trackNeg->d0();
      trackNeg_d0_wrtPV = trackNeg->d0() - (primaryVertex->position().y()*cos(trackNeg->phi0()) - primaryVertex->position().x()*sin(trackNeg->phi0()));
    }
    int selectorValue = 0;
    ATH_MSG_DEBUG( "ksTau = " << properDecayTime << " Lxy = " <<transverseFlightDistance<< " cosTheta = " << cosThetaPointing );
    ATH_MSG_DEBUG( "trackPos nSVThits = " << trackPos_nSVTHits << " trackNeg nSVThits = " << trackNeg_nSVTHits );
    ATH_MSG_DEBUG( "ksPt = " << ksPt );
    double secVertex_radius = secVx_elem->auxdata< float >("Rxy");
    ATH_MSG_DEBUG("secondary vertex radius : " << secVertex_radius);
    m_radius_secVertices->Fill(secVertex_radius);
    m_radiusVsZ_secVertex->Fill(secVxPosition.z(),secVertex_radius);
    m_YVsX_secVertex->Fill(secVxPosition.x(),secVxPosition.y());
    //    }
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
      m_radiusVsZ_secVertex_sel->Fill(secVxPosition.z(),secVertex_radius);
      m_YVsX_secVertex_sel->Fill(secVxPosition.x(),secVxPosition.y());
    }



    if( 1
	&& properDecayTime > 0.004
	&& transverseFlightDistance > 12.
	&& cosThetaPointing > 0.998
	&& ksMass>400.&&ksMass<600.
	&& trackPos_nSVTHits > 2 && trackNeg_nSVTHits > 2
	) selectorValue = 1;
    if(selectorValue != 1) continue;
    m_radiusVsZ_secVertex_Ks->Fill(secVxPosition.z(),secVertex_radius);
    m_YVsX_secVertex_Ks->Fill(secVxPosition.x(),secVxPosition.y());
    m_YVsX_primVertex_Ks->Fill(primaryVertex->position().x(),primaryVertex->position().y());
    m_XVsZ_primVertex_Ks->Fill(primaryVertex->position().z(),primaryVertex->position().x());
    m_YVsZ_primVertex_Ks->Fill(primaryVertex->position().z(),primaryVertex->position().y());
    m_mass->Fill(ksMass*0.001);
    double ksEta = ksMomentumVector.pseudoRapidity();
    double ksPhi = ksMomentumVector.phi();
    double piPlusPt = trackPos->p4().Perp();
    double piMinusPt = trackNeg->p4().Perp();
    m_massVsPhi->Fill(ksPhi,ksMass-ksMassPDG);
    m_pt->Fill(ksPt*0.001);
    m_eta->Fill(ksEta);
    m_phi->Fill(ksPhi);
    Float_t curvatureDiff = (1./(piPlusPt)) - (1./(piMinusPt));
    const auto fillValue(ksMass*0.001);
    auto curvatureIdx = findLevel(curvatureBinning.begin(), curvatureBinning.end(),curvatureDiff);
    m_massVCurvatureDiffBinHistos[curvatureIdx]->Fill(fillValue);
    if (ksPhi>=-M_PI and ksPhi<M_PI){
      auto ksPhiIdx = findLevel(ksPhiBinning.begin(), ksPhiBinning.end(), ksPhi, std::less_equal<double>());
      m_massVPhiBinHistos[ksPhiIdx]->Fill(fillValue);
    }
    if (ksEta>=-2.5 and ksEta<2.5){
      auto ksEtaIdx = findLevel(ksEtaBinning.begin(), ksEtaBinning.end(), ksEta, std::less_equal<double>());
      m_massVEtaBinHistos[ksEtaIdx]->Fill(fillValue);
    }
    if (ksPt>=0.){
      auto ksPtIdx = findLevel(ksPtBinning.begin(), ksPtBinning.end(), ksPt, std::less_equal<double>());
      m_massVPtBinFittedHistos[ksPtIdx]->Fill(fillValue);
    }

    if (ksPt<500) ksPt = 500;
    if (ksPt>5000) ksPt=5000;
    Int_t quickBin = (Int_t)ksPt;
    quickBin -= quickBin%100;
    quickBin -= 500;
    quickBin /= 100;
    m_massVPtBinHistos[quickBin]->Fill(fillValue);
    double radius = secVx_elem->auxdata< float >("Rxy");
    m_radius->Fill(radius);
    if (radius>=0.){
      auto radiusIdx = findLevel(radiusBinning.begin(), radiusBinning.end(), radius, std::less_equal<double>());
      m_massVRadiusBinFittedHistos[radiusIdx]->Fill(fillValue);
    }
    if(radius>700) radius = 700;
    Int_t radiusTemp = (Int_t)radius;
    radiusTemp -= radiusTemp%10;
    m_massVRadiusBinHistos[(Int_t)radiusTemp/10]->Fill(ksMass/1000.);
  }
  return StatusCode::SUCCESS;
}


StatusCode IDPerfMonKshort::procHistograms()
{
  ATH_MSG_VERBOSE( "IDPerfMonKshort procHistograms() started");
  if( endOfRunFlag() ) {
    MonGroup al_kshort_mon ( this, "IDPerfMon/Kshort/" + m_triggerChainName, run);
    //CHECK ATTRIB MANAGED
    MonGroup al_kshort_mon_average ( this, "IDPerfMon/Kshort/" + m_triggerChainName, run, ATTRIB_MANAGED, "", "weightedAverage" );
    TF1 *func = new TF1("func","gaus(0)+expo(3)",0.450,0.550);
    func->SetLineColor(4);
    func->SetParameters(10.,0.500,0.010,2.,-.001);
    func->SetParLimits(0,0.,10000.);
    func->SetParLimits(1,0.450,0.550);
    func->SetParLimits(2,0.,0.100);
    func->SetParLimits(3,0.,10000.);
    func->SetParLimits(4,-1000.,0.);
    func->SetParNames("Constant","Mean","Width","Constant","Slope");
    Double_t massBins[m_nFittedBinsPt], massErrorBins[m_nFittedBinsPt], widthBins[m_nFittedBinsPt], widthErrorBins[m_nFittedBinsPt];
    const Int_t nPtBinsHisto = m_nFittedBinsPt+1;
    Double_t ptBins[nPtBinsHisto] = {0.5,1.6,2.1,2.8,3.9,5.1};
    for(int binFill=0;binFill<m_nFittedBinsPt;binFill++) {
      massBins[binFill] = func->GetParameter(1);
      massErrorBins[binFill] = func->GetParError(1);
      widthBins[binFill] = func->GetParameter(2);
      widthErrorBins[binFill] = func->GetParError(2);
    }
    const Double_t* ptBinsFinal = ptBins;
    const Double_t* massBinsFinal = massBins;
    const Double_t* massErrorBinsFinal = massErrorBins;
    const Double_t* widthBinsFinal = widthBins;
    const Double_t* widthErrorBinsFinal = widthErrorBins;
    //lamda utilities for repetitive code
    auto setYTitleAndMarker =[](TH1F * theHist,const bool mass){
      if (mass){ 
        theHist->SetYTitle("Mass (Gev / c^{2})");
      }else { 
        theHist->SetYTitle("Width (Gev / c^{2})");
      };
      theHist->SetMarkerStyle(20);
    };
    //
    auto fillFromSource = [](const double * source, const double * uncertainty, TH1F * target, unsigned int nbins){
      for(unsigned int bin=0;bin<nbins;bin++) {
        double binContent = source[bin];
        double binError = uncertainty[bin];
        target->SetBinContent(bin+1, binContent);
        target->SetBinError(bin+1,binError);
      }
    };
    constexpr bool massTitle{true};
    constexpr bool widthTitle{false};
    if(m_nFittedBinsPt) {
      m_massVersusPt = new TH1F("KsMassVersusPt","",m_nFittedBinsPt,ptBinsFinal);
      RegisterHisto(al_kshort_mon_average,m_massVersusPt);
      m_massVersusPt->SetXTitle("p_{T} (Gev / c)");
      setYTitleAndMarker(m_massVersusPt, massTitle);
      fillFromSource(massBinsFinal, massErrorBinsFinal, m_massVersusPt, m_nFittedBinsPt);
      m_widthVersusPt = new TH1F("KsWidthVersusPt","",m_nFittedBinsPt,ptBinsFinal);
      RegisterHisto(al_kshort_mon_average,m_widthVersusPt);
      m_widthVersusPt->SetXTitle("p_{T} (Gev / c)");
      setYTitleAndMarker(m_widthVersusPt, widthTitle);
      fillFromSource(widthBinsFinal, widthErrorBinsFinal, m_widthVersusPt, m_nFittedBinsPt);
    }

    Double_t massVradiusBins[m_nFittedBinsRadius], massVradiusErrorBins[m_nFittedBinsRadius], widthVradiusBins[m_nFittedBinsRadius], widthVradiusErrorBins[m_nFittedBinsRadius];
    const Int_t nRadiusBinsHisto = m_nFittedBinsRadius+1;
    Double_t radiusBins[nRadiusBinsHisto] = {0.,30.,40.,60.,80.,100.,140.,230};
    for(int binFill=0;binFill<m_nFittedBinsRadius;binFill++) {
      massVradiusBins[binFill] = func->GetParameter(1);
      massVradiusErrorBins[binFill] = func->GetParError(1);
      widthVradiusBins[binFill] = func->GetParameter(2);
      widthVradiusErrorBins[binFill] = func->GetParError(2);
    }

    const Double_t* radiusBinsFinal = radiusBins;
    const Double_t* massVradiusBinsFinal = massVradiusBins;
    const Double_t* massVradiusErrorBinsFinal = massVradiusErrorBins;
    const Double_t* widthVradiusBinsFinal = widthVradiusBins;
    const Double_t* widthVradiusErrorBinsFinal = widthVradiusErrorBins;

    if(m_nFittedBinsRadius) {
      m_massVersusRadius = new TH1F("KsMassVersusRadius","",m_nFittedBinsRadius,radiusBinsFinal);
      RegisterHisto(al_kshort_mon_average,m_massVersusRadius);
      m_massVersusRadius->SetXTitle("Decay Radius (mm)");
      setYTitleAndMarker(m_massVersusRadius, massTitle);
      fillFromSource(massVradiusBinsFinal, massVradiusErrorBinsFinal, m_massVersusRadius, m_nFittedBinsRadius);
      m_widthVersusRadius = new TH1F("KsWidthVersusRadius","",m_nFittedBinsRadius,radiusBinsFinal);
      RegisterHisto(al_kshort_mon_average,m_widthVersusRadius);
      m_widthVersusRadius->SetXTitle("Decay Radius (mm)");
      setYTitleAndMarker(m_widthVersusRadius, widthTitle);
      fillFromSource(widthVradiusBinsFinal, widthVradiusErrorBinsFinal, m_widthVersusRadius, m_nFittedBinsRadius);
    }

    Double_t etaBins[11] = {-2.5,-2.0,-1.5,-1.0,-0.5,0.0,0.5,1.0,1.5,2.0,2.5};
    Double_t massVetaBins[10], massVetaErrorBins[10], widthVetaBins[10], widthVetaErrorBins[10];
    for(int binFill=0;binFill<10;binFill++) {
      massVetaBins[binFill] = func->GetParameter(1);
      massVetaErrorBins[binFill] = func->GetParError(1);
      widthVetaBins[binFill] = func->GetParameter(2);
      widthVetaErrorBins[binFill] = func->GetParError(2);
    }
    m_massVersusEta = new TH1F("KsMassVersusEta","",10,etaBins);
    RegisterHisto(al_kshort_mon_average,m_massVersusEta);
    m_massVersusEta->SetXTitle("#eta");
    setYTitleAndMarker(m_massVersusEta, massTitle);
    fillFromSource(massVetaBins, massVetaErrorBins, m_massVersusEta, 10);
    m_widthVersusEta = new TH1F("KsWidthVersusEta","",10,etaBins);
    RegisterHisto(al_kshort_mon_average,m_widthVersusEta);
    m_widthVersusEta->SetXTitle("#eta");
    setYTitleAndMarker(m_massVersusEta, widthTitle);
    fillFromSource(widthVetaBins, widthVetaErrorBins, m_widthVersusEta, 10);
    Double_t phiBins[11]{}; 
    sequentialFill(phiBins, phiBins+11, -M_PI, M_PI/5.);
    Double_t massVphiBins[10], massVphiErrorBins[10], widthVphiBins[10], widthVphiErrorBins[10];
    for(int binFill=0;binFill<10;binFill++) {
      massVphiBins[binFill] = func->GetParameter(1);
      massVphiErrorBins[binFill] = func->GetParError(1);
      widthVphiBins[binFill] = func->GetParameter(2);
      widthVphiErrorBins[binFill] = func->GetParError(2);
    }
    m_massVersusPhi = new TH1F("KsMassVersusPhi","",10,phiBins);
    RegisterHisto(al_kshort_mon_average,m_massVersusPhi);
    m_massVersusPhi->SetXTitle("#phi");
    setYTitleAndMarker(m_massVersusPhi, massTitle);
    fillFromSource(massVphiBins, massVphiErrorBins, m_massVersusPhi, 10);
    m_widthVersusPhi = new TH1F("KsWidthVersusPhi","",10,phiBins);
    RegisterHisto(al_kshort_mon_average,m_widthVersusPhi);
    m_widthVersusPhi->SetXTitle("#phi");
    setYTitleAndMarker(m_widthVersusPhi, widthTitle);
    fillFromSource(widthVphiBins, widthVphiErrorBins, m_widthVersusPhi, 10);
    Double_t curvatureDiffBins[7] = {-0.0012,-0.0008,-0.0004,0.0000,0.0004,0.0008,0.0012};
    Double_t massVcurvatureDiffBins[6], massVcurvatureDiffErrorBins[6], widthVcurvatureDiffBins[6], widthVcurvatureDiffErrorBins[6];
    for(int binFill=0;binFill<6;binFill++) {
      massVcurvatureDiffBins[binFill] = func->GetParameter(1);
      massVcurvatureDiffErrorBins[binFill] = func->GetParError(1);
      widthVcurvatureDiffBins[binFill] = func->GetParameter(2);
      widthVcurvatureDiffErrorBins[binFill] = func->GetParError(2);
    }
    m_massVersusCurvatureDiff = new TH1F("KsMassVersusCurvatureDiff","",6,curvatureDiffBins);
    RegisterHisto(al_kshort_mon_average,m_massVersusCurvatureDiff);
    m_massVersusCurvatureDiff->SetXTitle("1/p_{T}(#pi^{+}) - 1/p_{T}(#pi^{-}) [GeV^{-1}]");
    setYTitleAndMarker(m_massVersusCurvatureDiff, massTitle);
    fillFromSource(massVcurvatureDiffBins, massVcurvatureDiffErrorBins, m_massVersusCurvatureDiff, 6);
    m_widthVersusCurvatureDiff = new TH1F("KsWidthVersusCurvatureDiff","",6,curvatureDiffBins);
    RegisterHisto(al_kshort_mon_average,m_widthVersusCurvatureDiff);
    m_widthVersusCurvatureDiff->SetXTitle("1/p_{T}(#pi^{+}) - 1/p_{T}(#pi^{-}) [GeV^{-1}]");
    setYTitleAndMarker(m_widthVersusCurvatureDiff, widthTitle);
    fillFromSource(widthVcurvatureDiffBins, widthVcurvatureDiffErrorBins, m_widthVersusCurvatureDiff, 6);
  }

  return StatusCode::SUCCESS;

}
