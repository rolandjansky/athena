/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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

// #include "AthenaMonitoring/AthenaMonManager.h"
#include "InDetPerformanceMonitoring/IDPerfMonKshort.h"


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
  declareProperty("VxContainerName",m_VxContainerName="SecVertices");
  declareProperty("VxPrimContainerName",m_VxPrimContainerName="PrimaryVertices");
  //  declareProperty("MakeNtuple",m_Ntuple = false);

}

IDPerfMonKshort::~IDPerfMonKshort() { }


StatusCode IDPerfMonKshort::initialize()
{
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "IDPerfMonKshort initialize() started"<< endreq;

   m_histosBooked = 0;

//   // Get StoreGate
//   if ( service("StoreGateSvc",m_storeGate).isFailure() ) {
//     if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Unable to get pointer to StoreGateSvc" << endreq;
//     return StatusCode::FAILURE;
//   }

  if (m_tracksName.empty())     if(msgLvl(MSG::ERROR)) msg(MSG::ERROR) << " no track collection given"<<endreq;

  StatusCode sc;
  sc = ManagedMonitorToolBase::initialize();
  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "IDPerfMonKshort initialize() finished"<< endreq;
  if(!sc.isSuccess()) return StatusCode::SUCCESS;

//   if (m_Ntuple) {
//     if (!NtupleMaker.initializeTools()) {
//       m_Ntuple=false;
//     }
//  }

  return StatusCode::SUCCESS;
}


StatusCode IDPerfMonKshort::bookHistograms()
{
  Double_t myPi = TMath::Pi();

  MonGroup al_kshort_mon ( this, "IDPerfMon/Kshort/" + m_triggerChainName, run);

  //"" is for chain, "weightedAverage" is for merge, run is stored in interval
  //MonGroup al_kshort_mon_average ( this, "IDPerfMon/Kshort/" + m_triggerChainName,  run, "", "weightedAverage" );

  //ASK CONFIRMATION for ATTRIB_MANAGED
  MonGroup al_kshort_mon_average(this, "IDPerfMon/Kshort/" + m_triggerChainName,run,ATTRIB_MANAGED,"","weightedAverage");

  //  MonGroup al_kshort_debug ( this, "IDPerfMon/Kshort/" + m_triggerChainName,  debug, run);

  if ( AthenaMonManager::environment() == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }

  if ( AthenaMonManager::dataType() == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }





  if ( newLowStat || newLumiBlock ) {

  }

  if( newRun ) {

    //if user environment specified we don't want to book new histograms at every run boundary
    //we instead want one histogram per job
    if(m_histosBooked!=0 && AthenaMonManager::environment()==AthenaMonManager::user) return StatusCode::SUCCESS;

    Double_t ptBins[m_nFittedBinsPt] = {1.05,1.85,2.45,3.35,4.5};
    Double_t radiusBins[m_nFittedBinsRadius] = {15.,35.,50.,70.,90.,120.,185.};
    Double_t phiBins[10] = { (-4.5*myPi / 5) ,
			     (-3.5*myPi / 5) ,
			     (-2.5*myPi / 5) ,
			     (-1.5*myPi / 5) ,
			     (-0.5*myPi / 5) ,
			     (0.5*myPi / 5) ,
			     (1.5*myPi / 5) ,
			     (2.5*myPi / 5) ,
			     (3.5*myPi / 5) ,
			     (4.5*myPi / 5) };
    Double_t etaBins[10] = {-2.25,-1.75,-1.25,-0.75,-0.25,0.25,0.75,1.25,1.75,2.25};
    Double_t curvatureDiffBins[6] = {-0.0010,-0.0006,-0.0002,0.0002,0.0006,0.0010};

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

    m_pt = new TH1F("ks_pt", "p_{T} of K^{0}_{S} candidate", 100, 0., 10.);
    m_pt->SetYTitle("K^{0}_{S} Candidates");
    m_pt->SetXTitle("p_{T} (Gev / c)");
    m_pt->SetMarkerStyle(20);
    RegisterHisto(al_kshort_mon,m_pt) ;
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
    m_phi = new TH1F("ks_phi", "#phi of K^{0}_{S} candidate", 10, (-1.0* myPi), myPi);
    m_phi->SetYTitle("K^{0}_{S} Candidates");
    m_phi->SetXTitle("#phi");
    m_phi->SetMarkerStyle(20);
    RegisterHisto(al_kshort_mon,m_phi) ;
//     TH1F* tempHisto[m_nBinsPt];
//     m_massVPtBinHistos = tempHisto;
    for(int quickInit=0;quickInit<m_nBinsPt;quickInit++) {
      TString tempName = "MassVptBin";
      TString tempTitle = "Mass, p_{T} = ";
      tempName += quickInit;
      tempTitle += ((Double_t)((quickInit*100)+500))/1000;
      tempTitle += " GeV";
      m_massVPtBinHistos[quickInit] = new TH1F(tempName.Data(),tempTitle.Data(),50,0.445,0.555);
      m_massVPtBinHistos[quickInit]->SetXTitle("Mass (Gev / c^{2})");
      m_massVPtBinHistos[quickInit]->SetYTitle("K^{0}_{S} Candidates");
      m_massVPtBinHistos[quickInit]->SetMarkerStyle(20);
      m_massVPtBinHistos[quickInit]->SetMinimum(0.);
      RegisterHisto(al_kshort_mon,m_massVPtBinHistos[quickInit]) ;
    }
     for(int quickInit=0;quickInit<m_nFittedBinsPt;quickInit++) {
      TString tempName = "MassVptBinFitted";
      TString tempTitle = "Fitted Mass, p_{T} = ";
      tempName += quickInit;
      tempTitle += ptBins[quickInit];
      tempTitle += " GeV";
      m_massVPtBinFittedHistos[quickInit] = new TH1F(tempName.Data(),tempTitle.Data(),50,0.445,0.555);
      m_massVPtBinFittedHistos[quickInit]->SetXTitle("Mass (Gev / c^{2})");
      m_massVPtBinFittedHistos[quickInit]->SetYTitle("K^{0}_{S} Candidates");
      m_massVPtBinFittedHistos[quickInit]->SetMarkerStyle(20);
      m_massVPtBinFittedHistos[quickInit]->SetMinimum(0.);
      RegisterHisto(al_kshort_mon,m_massVPtBinFittedHistos[quickInit]) ;
    }
   for(int quickInit=0;quickInit<m_nBinsRadius;quickInit++) {
      TString tempName = "MassVradiusBin";
      TString tempTitle = "Mass, Decay Radius = ";
      tempName += quickInit;
      tempTitle += quickInit*10;
      tempTitle += " mm";
      m_massVRadiusBinHistos[quickInit] = new TH1F(tempName.Data(),tempTitle.Data(),50,0.445,0.555);
      m_massVRadiusBinHistos[quickInit]->SetXTitle("Mass (Gev / c^{2})");
      m_massVRadiusBinHistos[quickInit]->SetYTitle("K^{0}_{S} Candidates");
      m_massVRadiusBinHistos[quickInit]->SetMarkerStyle(20);
      m_massVRadiusBinHistos[quickInit]->SetMinimum(0.);
      RegisterHisto(al_kshort_mon,m_massVRadiusBinHistos[quickInit]) ;
    }
   for(int quickInit=0;quickInit<m_nFittedBinsRadius;quickInit++) {
      TString tempName = "MassVradiusBinFitted";
      TString tempTitle = "Fitted Mass, Decay Radius = ";
      tempName += quickInit;
      tempTitle += radiusBins[quickInit];
      tempTitle += " mm";
      m_massVRadiusBinFittedHistos[quickInit] = new TH1F(tempName.Data(),tempTitle.Data(),50,0.445,0.555);
      m_massVRadiusBinFittedHistos[quickInit]->SetXTitle("Mass (Gev / c^{2})");
      m_massVRadiusBinFittedHistos[quickInit]->SetYTitle("K^{0}_{S} Candidates");
      m_massVRadiusBinFittedHistos[quickInit]->SetMarkerStyle(20);
      m_massVRadiusBinFittedHistos[quickInit]->SetMinimum(0.);
      RegisterHisto(al_kshort_mon,m_massVRadiusBinFittedHistos[quickInit]) ;
    }
    for(int quickInit=0;quickInit<10;quickInit++) {
      TString tempName = "MassVEtaBin";
      TString tempTitle = "Mass, #eta =  ";
      tempName += quickInit;
      tempTitle += etaBins[quickInit];
      m_massVEtaBinHistos[quickInit] = new TH1F(tempName.Data(),tempTitle.Data(),50,0.445,0.555);
      m_massVEtaBinHistos[quickInit]->SetXTitle("Mass (Gev / c^{2})");
      m_massVEtaBinHistos[quickInit]->SetYTitle("K^{0}_{S} Candidates");
      m_massVEtaBinHistos[quickInit]->SetMarkerStyle(20);
      m_massVEtaBinHistos[quickInit]->SetMinimum(0.);
      RegisterHisto(al_kshort_mon,m_massVEtaBinHistos[quickInit]) ;

      tempName = "MassVPhiBin";
      tempTitle = "Mass, #phi = ";
      tempName += quickInit;
      tempTitle += ((Double_t)((Int_t)(phiBins[quickInit]*100)))/100;
      m_massVPhiBinHistos[quickInit] = new TH1F(tempName.Data(),tempTitle.Data(),50,0.445,0.555);
      m_massVPhiBinHistos[quickInit]->SetXTitle("Mass (Gev / c^{2})");
      m_massVPhiBinHistos[quickInit]->SetYTitle("K^{0}_{S} Candidates");
      m_massVPhiBinHistos[quickInit]->SetMarkerStyle(20);
      m_massVPhiBinHistos[quickInit]->SetMinimum(0.);
      RegisterHisto(al_kshort_mon,m_massVPhiBinHistos[quickInit]) ;
    }

   for(int quickInit=0;quickInit<6;quickInit++) {
      TString tempName = "MassVCurvatureDiffBin";
      TString tempTitle = "Mass, CurvatureDiff =  ";
      tempName += quickInit;
      tempTitle += curvatureDiffBins[quickInit];
      m_massVCurvatureDiffBinHistos[quickInit] = new TH1F(tempName.Data(),tempTitle.Data(),50,0.445,0.555);
      m_massVCurvatureDiffBinHistos[quickInit]->SetXTitle("Mass (Gev / c^{2})");
      m_massVCurvatureDiffBinHistos[quickInit]->SetYTitle("K^{0}_{S} Candidates");
      m_massVCurvatureDiffBinHistos[quickInit]->SetMarkerStyle(20);
      m_massVCurvatureDiffBinHistos[quickInit]->SetMinimum(0.);
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

    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "IDPerfMonKshort bookHistograms done"<< endreq;

    m_histosBooked++;

  }

//   if (m_Ntuple) {
//   StatusCode sc = al_kshort_mon.regTree(NtupleMaker.makeTree());
//   if (sc.isFailure()){
//    msg(MSG::WARNING) << "Cannot book TTree" << endreq;
//    m_Ntuple=false;
//   }
//   }

  return StatusCode::SUCCESS;
}

void IDPerfMonKshort::RegisterHisto(MonGroup& mon, TH1* histo) {


  histo->Sumw2();
  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Cannot book TH1 Histogram:" << endreq;
  }
}

void IDPerfMonKshort::RegisterHisto(MonGroup& mon, TProfile* histo) {


  StatusCode sc = mon.regHist(histo);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Cannot book TProfile Histogram:" << endreq;
  }
}

void IDPerfMonKshort::RegisterHisto(MonGroup& mon, TGraph* graph) {


  StatusCode sc = mon.regGraph(graph);
  if (sc.isFailure() ) {
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Cannot book TGraph:" << endreq;
  }
}


StatusCode IDPerfMonKshort::fillHistograms()
{
  Double_t myPi = TMath::Pi();
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
	ATH_MSG_DEBUG("No Collection with name "<<m_VxPrimContainerName<<" found in StoreGate");
	return false;
      }
 }
 else {
	ATH_MSG_DEBUG("Collection with name "<<m_VxPrimContainerName<<" found in StoreGate");
	return StatusCode::SUCCESS;
 }
 xAOD::Vertex *primaryVertex= std::begin(*PrimVxContainer)[0];



const xAOD::VertexContainer* SecVxContainer(0);
 if(evtStore()->contains<xAOD::VertexContainer>(m_VxContainerName)){
   if (evtStore()->retrieve(SecVxContainer,m_VxContainerName).isFailure()) {
     ATH_MSG_DEBUG("No Collection with name "<<m_VxContainerName<<" found in StoreGate");
     return false;
   }
 } else {
   ATH_MSG_DEBUG("Collection with name "<<m_VxContainerName<<" found in StoreGate");
   return StatusCode::SUCCESS;
 }

 m_Nevents->Fill(0.);


 ToolHandle <Trk::V0Tools> myV0Tools("Trk::V0Tools");
 // = new Trk::V0Tools("V0Tools","myV0Tools",m_parent);
 //const xAOD::Vertex* theVxCandidate;
 double piMass = 139.57018;
 double ksMassPDG = 497.648;
 xAOD::VertexContainer::const_iterator v0Itr = SecVxContainer->begin();
 for ( v0Itr=SecVxContainer->begin(); v0Itr!=SecVxContainer->end(); ++v0Itr ) {
   // for (const auto* secVx_elem : *SecVxContainer) {
   //theVxCandidate = secVx_elem;

   xAOD::Vertex* theVxCandidate = (*v0Itr);
   double ksMass = myV0Tools->invariantMass(theVxCandidate,piMass,piMass);
   double ksPt = myV0Tools->V04Momentum(theVxCandidate,ksMassPDG).Perp();
   double ksMomentum = myV0Tools->V04Momentum(theVxCandidate,ksMassPDG).Vect().Mag();
   //NEED TO BE MIGRATED?
   CLHEP::Hep3Vector ksMomentumVector = CLHEP::Hep3Vector(myV0Tools->V04Momentum(theVxCandidate,ksMassPDG).Px(),
							  myV0Tools->V04Momentum(theVxCandidate,ksMassPDG).Py(),
							  myV0Tools->V04Momentum(theVxCandidate,ksMassPDG).Pz());

   double transverseFlightDistance, totalFlightDistance;
   Amg::Vector3D flightVector;
   if(primaryVertex!=NULL) {
     transverseFlightDistance = myV0Tools->lxy(theVxCandidate,primaryVertex);
     totalFlightDistance = (myV0Tools->vtx(theVxCandidate)-primaryVertex->position()).mag();
     flightVector = myV0Tools->vtx(theVxCandidate)-primaryVertex->position();
   }
   else {
     transverseFlightDistance = myV0Tools->rxy(theVxCandidate);
     totalFlightDistance = (myV0Tools->vtx(theVxCandidate)).mag();
     flightVector = myV0Tools->vtx(theVxCandidate);
   }

   double properDecayTime = 1./Gaudi::Units::c_light*ksMassPDG/ksMomentum*totalFlightDistance;

   double ksPx = ksMomentumVector.x();
   double ksPy = ksMomentumVector.y();
   double flightX = flightVector.x();
   double flightY = flightVector.y();
   double cosThetaPointing = (ksPx*flightX+ksPy*flightY)/sqrt(ksPx*ksPx+ksPy*ksPy)/sqrt(flightX*flightX+flightY*flightY);

   int trackPos_nSVTHits = 0;
   int trackNeg_nSVTHits = 0;

   const xAOD::TrackParticle* trackPos = myV0Tools->positiveOrigTrack(theVxCandidate);
   if(trackPos!=0) {
     uint8_t dummy(-1);
     trackPos_nSVTHits = trackPos->summaryValue(  dummy , xAOD::numberOfSCTHits  )? dummy :-1;
   }
   const xAOD::TrackParticle* trackNeg = myV0Tools->negativeOrigTrack(theVxCandidate);
   if(trackNeg!=0) {
     uint8_t dummy(-1);
     trackNeg_nSVTHits = trackNeg->summaryValue(  dummy , xAOD::numberOfSCTHits  )? dummy :-1;
   }

   int selectorValue = 0;

   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ksTau = " << properDecayTime << " Lxy = " <<transverseFlightDistance<< " cosTheta = " << cosThetaPointing <<endreq;
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "trackPos nSVThits = " << trackPos_nSVTHits << " trackNeg nSVThits = " << trackNeg_nSVTHits <<endreq;
    // if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ksMass = " << ksMass<< " ksMassConstrained = " << ksMassConstrained << endreq;

    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ksPt = " << ksPt <<endreq;

    if( 1
	&& properDecayTime > 0.004
	&& transverseFlightDistance > 12.
	&& cosThetaPointing > 0.998
	&& ksMass>400.&&ksMass<600.
	&& trackPos_nSVTHits > 2 && trackNeg_nSVTHits > 2
	) selectorValue = 1;
    if(selectorValue != 1) continue;

    m_mass->Fill(ksMass/1000.);
    //    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ksMass = " << ksMass<< " ksMassConstrained = " << ksMassConstrained <<endreq;

    //   if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "ksPt = " << ksPt <<endreq;
    double ksEta = myV0Tools->V04Momentum(theVxCandidate,ksMassPDG).PseudoRapidity();
    double ksPhi = myV0Tools->V04Momentum(theVxCandidate,ksMassPDG).Phi();
    double piPlusPt = myV0Tools->positiveTrack4Momentum(theVxCandidate,piMass).Perp();
    //    double piPlusEta = myV0Tools->positiveTrack4Momentum((const Trk::ExtendedVxCandidate*)theVxCandidate,piMass).pseudoRapidity();
    //    double piPlusPhi = myV0Tools->positiveTrack4Momentum((const Trk::ExtendedVxCandidate*)myV0Hypothesis,piMass).phi();
    double piMinusPt = myV0Tools->negativeTrack4Momentum(theVxCandidate,piMass).Perp();
    //    double piMinusEta = myV0Tools->negativeTrack4Momentum((const Trk::ExtendedVxCandidate*)theVxCandidate,piMass).pseudoRapidity();
    //    double piMinusPhi = myV0Tools->negativeTrack4Momentum((const Trk::ExtendedVxCandidate*)myV0Hypothesis,piMass).phi();

    m_pt->Fill(ksPt/1000.);
    m_eta->Fill(ksEta);
    m_phi->Fill(ksPhi);

    //    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "piEta = " << piEta <<endreq;
    Float_t curvatureDiff = (1./(piPlusPt)) - (1./(piMinusPt));
    //    if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "curvatureDiff = " << curvatureDiff <<endreq;

    if( curvatureDiff <= -0.0008) m_massVCurvatureDiffBinHistos[0]->Fill(ksMass/1000.);
    if( curvatureDiff >  -0.0008 && curvatureDiff <= -0.0004) m_massVCurvatureDiffBinHistos[1]->Fill(ksMass/1000.);
    if( curvatureDiff >  -0.0004 && curvatureDiff <=  0.0000) m_massVCurvatureDiffBinHistos[2]->Fill(ksMass/1000.);
    if( curvatureDiff >   0.0000 && curvatureDiff <=  0.0004) m_massVCurvatureDiffBinHistos[3]->Fill(ksMass/1000.);
    if( curvatureDiff >   0.0004 && curvatureDiff <=  0.0008) m_massVCurvatureDiffBinHistos[4]->Fill(ksMass/1000.);
    if( curvatureDiff >   0.0008 ) m_massVCurvatureDiffBinHistos[5]->Fill(ksMass/1000.);

    if( ksPhi>=(-5.0*myPi / 5)&&ksPhi<(-4.0*myPi / 5) ) m_massVPhiBinHistos[0]->Fill(ksMass/1000.);
    if( ksPhi>=(-4.0*myPi / 5)&&ksPhi<(-3.0*myPi / 5) ) m_massVPhiBinHistos[1]->Fill(ksMass/1000.);
    if( ksPhi>=(-3.0*myPi / 5)&&ksPhi<(-2.0*myPi / 5) ) m_massVPhiBinHistos[2]->Fill(ksMass/1000.);
    if( ksPhi>=(-2.0*myPi / 5)&&ksPhi<(-1.0*myPi / 5) ) m_massVPhiBinHistos[3]->Fill(ksMass/1000.);
    if( ksPhi>=(-1.0*myPi / 5)&&ksPhi<(0.0*myPi / 5) ) m_massVPhiBinHistos[4]->Fill(ksMass/1000.);
    if( ksPhi>=(0.0*myPi / 5)&&ksPhi<(1.0*myPi / 5) ) m_massVPhiBinHistos[5]->Fill(ksMass/1000.);
    if( ksPhi>=(1.0*myPi / 5)&&ksPhi<(2.0*myPi / 5) ) m_massVPhiBinHistos[6]->Fill(ksMass/1000.);
    if( ksPhi>=(2.0*myPi / 5)&&ksPhi<(3.0*myPi / 5) ) m_massVPhiBinHistos[7]->Fill(ksMass/1000.);
    if( ksPhi>=(3.0*myPi / 5)&&ksPhi<(4.0*myPi / 5) ) m_massVPhiBinHistos[8]->Fill(ksMass/1000.);
    if( ksPhi>=(4.0*myPi / 5)&&ksPhi<(5.0*myPi / 5) ) m_massVPhiBinHistos[9]->Fill(ksMass/1000.);
    if(ksEta>=-2.5&&ksEta<-2.0) m_massVEtaBinHistos[0]->Fill(ksMass/1000.);
    if(ksEta>=-2.0&&ksEta<-1.5) m_massVEtaBinHistos[1]->Fill(ksMass/1000.);
    if(ksEta>=-1.5&&ksEta<-1.0) m_massVEtaBinHistos[2]->Fill(ksMass/1000.);
    if(ksEta>=-1.0&&ksEta<-0.5) m_massVEtaBinHistos[3]->Fill(ksMass/1000.);
    if(ksEta>=-0.5&&ksEta<0.0) m_massVEtaBinHistos[4]->Fill(ksMass/1000.);
    if(ksEta>=0.0&&ksEta<0.5) m_massVEtaBinHistos[5]->Fill(ksMass/1000.);
    if(ksEta>=0.5&&ksEta<1.0) m_massVEtaBinHistos[6]->Fill(ksMass/1000.);
    if(ksEta>=1.0&&ksEta<1.5) m_massVEtaBinHistos[7]->Fill(ksMass/1000.);
    if(ksEta>=1.5&&ksEta<2.0) m_massVEtaBinHistos[8]->Fill(ksMass/1000.);
    if(ksEta>=2.0&&ksEta<2.5) m_massVEtaBinHistos[9]->Fill(ksMass/1000.);

    if(ksPt>=0 && ksPt<1600) m_massVPtBinFittedHistos[0]->Fill(ksMass/1000.);
    if(ksPt>=1600 && ksPt<2100) m_massVPtBinFittedHistos[1]->Fill(ksMass/1000.);
    if(ksPt>=2100 && ksPt<2800) m_massVPtBinFittedHistos[2]->Fill(ksMass/1000.);
    if(ksPt>=2800 && ksPt<3900) m_massVPtBinFittedHistos[3]->Fill(ksMass/1000.);
    if(ksPt>=3900) m_massVPtBinFittedHistos[4]->Fill(ksMass/1000.);

    if (ksPt > 5000) ksPt = 5000;
    if(ksPt < 500) ksPt = 500;
    Int_t quickBin = (Int_t)ksPt;
    //    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "quickBin = " << quickBin <<endreq;
    quickBin -= quickBin%100;
    //    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "quickBin = " << quickBin <<endreq;
    quickBin -= 500;
    //    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "quickBin = " << quickBin <<endreq;
    quickBin /= 100;
    //    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "quickBin = " << quickBin <<endreq;
    m_massVPtBinHistos[quickBin]->Fill(ksMass/1000.);
    //    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "histo[" << quickBin << "]->GetEntries() = " << m_massVPtBinHistos[quickBin]->GetEntries() << endreq;

    double radius = myV0Tools->rxy(theVxCandidate);

    m_radius->Fill(radius);

    if(radius>=0 && radius<30) m_massVRadiusBinFittedHistos[0]->Fill(ksMass/1000.);
    if(radius>=30 && radius<40) m_massVRadiusBinFittedHistos[1]->Fill(ksMass/1000.);
    if(radius>=40 && radius<60) m_massVRadiusBinFittedHistos[2]->Fill(ksMass/1000.);
    if(radius>=60 && radius<80) m_massVRadiusBinFittedHistos[3]->Fill(ksMass/1000.);
    if(radius>=80 && radius<100) m_massVRadiusBinFittedHistos[4]->Fill(ksMass/1000.);
    if(radius>=100 && radius<140) m_massVRadiusBinFittedHistos[5]->Fill(ksMass/1000.);
    if(radius>=140) m_massVRadiusBinFittedHistos[6]->Fill(ksMass/1000.);

    if(radius>700) radius = 700;
    Int_t radiusTemp = (Int_t)radius;
    radiusTemp -= radiusTemp%10;
    m_massVRadiusBinHistos[(Int_t)radiusTemp/10]->Fill(ksMass/1000.);

//     const std::vector<Trk::VxTrackAtVertex*>& trackAtVertexVec = *(myV0Hypothesis->vxTrackAtVertex());
//     for (unsigned int i = 0; i < trackAtVertexVec.size(); i++) {
//       Trk::LinkToTrackParticleBase* newLinkToTrackPB = new Trk::LinkToTrackParticleBase();
//       if(charge1 > 0.) {
// 	if(i == 0) newLinkToTrackPB->setElement((*tpIt1));
// 	if(i == 1) newLinkToTrackPB->setElement((*tpIt2));
//       } else {
// 	if(i == 0) newLinkToTrackPB->setElement((*tpIt2));
// 	if(i == 1) newLinkToTrackPB->setElement((*tpIt1));
//       }
//       newLinkToTrackPB->setStorableObject(*TPC);
//       trackAtVertexVec[i]->setOrigTrack(newLinkToTrackPB);
//     }
  }

  //******************
  //NtupleMaker
  //******************
//   if (m_Ntuple)
//     {
//       int evtNum=0;
//       int runNum=0;
//       int LBNum=0;
//       const EventInfo* eventInfo; // EventInfo
//       StatusCode sc = evtStore()->retrieve(eventInfo);
//       if (sc.isSuccess()) {
// 	EventID* eventID = eventInfo->event_ID(); // Get EventInfo
// 	evtNum = eventID->event_number();
// 	runNum = eventID->run_number();
// 	LBNum = eventID->lumi_block();
//       }
//       NtupleMaker.setPrimaryVertex(primaryVertex);
//       NtupleMaker.setTrackMass(piMass);
//       NtupleMaker.setResonancePDGMass(ksMassPDG);
//       NtupleMaker.fill(evtNum,runNum,LBNum,theVxContainer);
//       //tmp fix after switching to secVtxFinder
//       NtupleMaker.calculateError(false);
//     }

  return StatusCode::SUCCESS;
}


StatusCode IDPerfMonKshort::procHistograms()
{
  Double_t myPi = TMath::Pi();

  if( endOfLowStat || endOfLumiBlock ) {


  }

  if( endOfRun ) {

    MonGroup al_kshort_mon ( this, "IDPerfMon/Kshort/" + m_triggerChainName, run);
    //CHECK ATTRIB MANAGED
    MonGroup al_kshort_mon_average ( this, "IDPerfMon/Kshort/" + m_triggerChainName, run, ATTRIB_MANAGED, "", "weightedAverage" );
    //    TF1 *func1 = new TF1("func1","gaus",0.450,0.550);
    //    TF1 *func3 = new TF1("func3","gaus",405,595);
    //    TF1 *func2 = new TF1("func2","expo",0.450,0.550);
    TF1 *func = new TF1("func","gaus(0)+expo(3)",0.450,0.550);
    //    TF1 *func = new TF1("func","func1+func2",0.450,0.550);
    //    TF1 *doubleGauss = new TF1("doubleGauss","func1+func3",405,595);

    func->SetLineColor(4);
    func->SetParameters(10.,0.500,0.010,2.,-.001);
    func->SetParLimits(0,0.,10000.);
    func->SetParLimits(1,0.450,0.550);
    func->SetParLimits(2,0.,0.100);
    func->SetParLimits(3,0.,10000.);
    func->SetParLimits(4,-1000.,0.);
    func->SetParNames("Constant","Mean","Width","Constant","Slope");

//     doubleGauss->SetParameters(50.,497.,0.,8.,496.,0.);
//     doubleGauss->SetParLimits(0,0.,10000.);
//     doubleGauss->SetParLimits(1,450.,550.);
//     doubleGauss->SetParLimits(2,0.,50.);
//     doubleGauss->SetParLimits(3,0.,1000.);
//     doubleGauss->SetParLimits(4,450.,550.);
//     doubleGauss->SetParLimits(5,0.,50.);
//     doubleGauss->SetParNames("C_{core}","#mu_{core}","#sigma_{core}","C_{tail}","#mu_{tail}","#sigma_{tail}");

//     Int_t BinCounter=0;
//     Double_t ptBins[m_nBinsPt], ptBinsErrors[m_nBinsPt], massBins[m_nBinsPt], massErrorBins[m_nBinsPt];
//     for(int binFill=0;binFill<m_nBinsPt;binFill++) {
//       ptBins[BinCounter] = (binFill*100)+500;
//       m_massVPtBinFittedHistos[BinCounter] = (TH1F*)m_massVPtBinHistos[binFill]->Clone();
//       while(m_massVPtBinFittedHistos[BinCounter]->GetEntries()<50) {
// 	binFill++;
// 	if(binFill==m_nBinsPt) break;
// 	m_massVPtBinFittedHistos[BinCounter]->Add(m_massVPtBinHistos[binFill]);
//       }
//       if(binFill==m_nBinsPt) {
// 	ptBinsErrors[BinCounter] = (5000 - ptBins[BinCounter])/2;
// 	ptBins[BinCounter] = (ptBins[BinCounter]+5000)/2;
//       } else {
// 	ptBinsErrors[BinCounter] = (((binFill+1)*100)+500 - ptBins[BinCounter])/2;
// 	ptBins[BinCounter] = (ptBins[BinCounter]+((binFill+1)*100)+500)/2;
//       }
//       ptBinsErrors[BinCounter] /= 1000.;
//       ptBins[BinCounter] /= 1000.;
//       m_massVPtBinFittedHistos[BinCounter]->Fit(func,"lmhq");
//       massBins[BinCounter] = func->GetParameter(1);
//       massErrorBins[BinCounter] = func->GetParError(1);
//       //      cout<<binFill<<" "<<ptBins[BinCounter]<<" "<<massBins[BinCounter]<<" "<<massErrorBins[BinCounter]<<endl;
//       BinCounter++;
//     }
    // int MinStat=100;
    //    if (m_mass->GetEntries()>=MinStat) m_mass->Fit(func,"lhqn");

    Double_t massBins[m_nFittedBinsPt], massErrorBins[m_nFittedBinsPt], widthBins[m_nFittedBinsPt], widthErrorBins[m_nFittedBinsPt];
    const Int_t nPtBinsHisto = m_nFittedBinsPt+1;
    Double_t ptBins[nPtBinsHisto] = {0.5,1.6,2.1,2.8,3.9,5.1};
    //    Double_t ptBins[m_nFittedBinsPt] = {1.05,1.85,2.45,3.35,4.5};
    //    Double_t ptBinsErrors[m_nFittedBinsPt] = {0.55,0.25,0.35,0.55,0.6};
    for(int binFill=0;binFill<m_nFittedBinsPt;binFill++) {
      //   if (m_massVPtBinFittedHistos[binFill]->GetEntries()>=MinStat) m_massVPtBinFittedHistos[binFill]->Fit(func,"lmhqn");
      massBins[binFill] = func->GetParameter(1);
      massErrorBins[binFill] = func->GetParError(1);
      widthBins[binFill] = func->GetParameter(2);
      widthErrorBins[binFill] = func->GetParError(2);
    }

    const Double_t* ptBinsFinal = ptBins;
    //    const Double_t* ptBinsErrorsFinal = ptBinsErrors;
    const Double_t* massBinsFinal = massBins;
    const Double_t* massErrorBinsFinal = massErrorBins;
    const Double_t* widthBinsFinal = widthBins;
    const Double_t* widthErrorBinsFinal = widthErrorBins;

    if(m_nFittedBinsPt) {
      //      m_massVersusPt = new TGraphErrors(m_nFittedBinsPt,ptBinsFinal,massBinsFinal,ptBinsErrorsFinal,massErrorBinsFinal);
      m_massVersusPt = new TH1F("KsMassVersusPt","",m_nFittedBinsPt,ptBinsFinal);
      RegisterHisto(al_kshort_mon_average,m_massVersusPt);
      //      m_massVersusPt->SetErrorOption("s");
      m_massVersusPt->SetXTitle("p_{T} (Gev / c)");
      m_massVersusPt->SetYTitle("Mass (Gev / c^{2})");
      m_massVersusPt->SetMarkerStyle(20);
      for(int binFill=0;binFill<m_nFittedBinsPt;binFill++) {
	Double_t binContent = massBinsFinal[binFill];
	Double_t binError = massErrorBinsFinal[binFill];
	m_massVersusPt->SetBinContent(binFill+1,binContent);
	m_massVersusPt->SetBinError(binFill+1,binError);
      }

      //      m_widthVersusPt = new TGraphErrors(m_nFittedBinsPt,ptBinsFinal,widthBinsFinal,ptBinsErrorsFinal,widthErrorBinsFinal);
      m_widthVersusPt = new TH1F("KsWidthVersusPt","",m_nFittedBinsPt,ptBinsFinal);
      RegisterHisto(al_kshort_mon_average,m_widthVersusPt);
      //      m_widthVersusPt->SetErrorOption("s");
      m_widthVersusPt->SetXTitle("p_{T} (Gev / c)");
      m_widthVersusPt->SetYTitle("Width (Gev / c^{2})");
      m_widthVersusPt->SetMarkerStyle(20);
      for(int binFill=0;binFill<m_nFittedBinsPt;binFill++) {
	Double_t binContent = widthBinsFinal[binFill];
	Double_t binError = widthErrorBinsFinal[binFill];
	m_widthVersusPt->SetBinContent(binFill+1,binContent);
	m_widthVersusPt->SetBinError(binFill+1,binError);
      }

//       RegisterHisto(al_kshort_mon_average,m_massVersusPt);
//       RegisterHisto(al_kshort_mon_average,m_widthVersusPt);
    }

    Double_t massVradiusBins[m_nFittedBinsRadius], massVradiusErrorBins[m_nFittedBinsRadius], widthVradiusBins[m_nFittedBinsRadius], widthVradiusErrorBins[m_nFittedBinsRadius];
    const Int_t nRadiusBinsHisto = m_nFittedBinsRadius+1;
    Double_t radiusBins[nRadiusBinsHisto] = {0.,30.,40.,60.,80.,100.,140.,230};
    //    Double_t radiusBins[m_nFittedBinsRadius] = {15.,35.,50.,70.,90.,120.,185.};
    //    Double_t radiusBinsErrors[m_nFittedBinsRadius] = {15.,5.,10.,10.,10.,20.,45.};
    for(int binFill=0;binFill<m_nFittedBinsRadius;binFill++) {
      //  if (m_massVRadiusBinFittedHistos[binFill]->GetEntries()>=MinStat) m_massVRadiusBinFittedHistos[binFill]->Fit(func,"lmhqn");
      massVradiusBins[binFill] = func->GetParameter(1);
      massVradiusErrorBins[binFill] = func->GetParError(1);
      widthVradiusBins[binFill] = func->GetParameter(2);
      widthVradiusErrorBins[binFill] = func->GetParError(2);
    }

    const Double_t* radiusBinsFinal = radiusBins;
    //    const Double_t* radiusBinsErrorsFinal = radiusBinsErrors;
    const Double_t* massVradiusBinsFinal = massVradiusBins;
    const Double_t* massVradiusErrorBinsFinal = massVradiusErrorBins;
    const Double_t* widthVradiusBinsFinal = widthVradiusBins;
    const Double_t* widthVradiusErrorBinsFinal = widthVradiusErrorBins;

    if(m_nFittedBinsRadius) {
      //      m_massVersusRadius = new TGraphErrors(m_nFittedBinsRadius,radiusBinsFinal,massVradiusBinsFinal,radiusBinsErrorsFinal,massVradiusErrorBinsFinal);
      m_massVersusRadius = new TH1F("KsMassVersusRadius","",m_nFittedBinsRadius,radiusBinsFinal);
      RegisterHisto(al_kshort_mon_average,m_massVersusRadius);
      //      m_massVersusRadius->SetErrorOption("s");
      m_massVersusRadius->SetXTitle("Decay Radius (mm)");
      m_massVersusRadius->SetYTitle("Mass (Gev / c^{2})");
      m_massVersusRadius->SetMarkerStyle(20);
      for(int binFill=0;binFill<m_nFittedBinsRadius;binFill++) {
	Double_t binContent = massVradiusBinsFinal[binFill];
	Double_t binError = massVradiusErrorBinsFinal[binFill];
	m_massVersusRadius->SetBinContent(binFill+1,binContent);
	m_massVersusRadius->SetBinError(binFill+1,binError);
      }
      //      m_widthVersusRadius = new TGraphErrors(m_nFittedBinsRadius,radiusBinsFinal,widthVradiusBinsFinal,radiusBinsErrorsFinal,widthVradiusErrorBinsFinal);
      m_widthVersusRadius = new TH1F("KsWidthVersusRadius","",m_nFittedBinsRadius,radiusBinsFinal);
      RegisterHisto(al_kshort_mon_average,m_widthVersusRadius);
      //      m_widthVersusRadius->SetErrorOption("s");
      m_widthVersusRadius->SetXTitle("Decay Radius (mm)");
      m_widthVersusRadius->SetYTitle("Width (Gev / c^{2})");
      m_widthVersusRadius->SetMarkerStyle(20);
      for(int binFill=0;binFill<m_nFittedBinsRadius;binFill++) {
	Double_t binContent = widthVradiusBinsFinal[binFill];
	Double_t binError = widthVradiusErrorBinsFinal[binFill];
	m_widthVersusRadius->SetBinContent(binFill+1,binContent);
	m_widthVersusRadius->SetBinError(binFill+1,binError);
      }

//       RegisterHisto(al_kshort_mon_average,m_massVersusRadius);
//       RegisterHisto(al_kshort_mon_average,m_widthVersusRadius);
    }

    Double_t etaBins[11] = {-2.5,-2.0,-1.5,-1.0,-0.5,0.0,0.5,1.0,1.5,2.0,2.5};
    //    Double_t etaBins[10] = {-2.25,-1.75,-1.25,-0.75,-0.25,0.25,0.75,1.25,1.75,2.25};
    //    Double_t etaErrorBins[10] = {0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25,0.25};
    Double_t massVetaBins[10], massVetaErrorBins[10], widthVetaBins[10], widthVetaErrorBins[10];
    for(int binFill=0;binFill<10;binFill++) {
      // if (m_massVEtaBinHistos[binFill]->GetEntries()>=MinStat) m_massVEtaBinHistos[binFill]->Fit(func,"lmhqn");
      massVetaBins[binFill] = func->GetParameter(1);
      massVetaErrorBins[binFill] = func->GetParError(1);
      widthVetaBins[binFill] = func->GetParameter(2);
      widthVetaErrorBins[binFill] = func->GetParError(2);
    }
    m_massVersusEta = new TH1F("KsMassVersusEta","",10,etaBins);
    RegisterHisto(al_kshort_mon_average,m_massVersusEta);
    //    m_massVersusEta->SetErrorOption("s");
    m_massVersusEta->SetXTitle("#eta");
    m_massVersusEta->SetYTitle("Mass (Gev / c^{2})");
    m_massVersusEta->SetMarkerStyle(20);
    for(int binFill=0;binFill<10;binFill++) {
      Double_t binContent = massVetaBins[binFill];
      Double_t binError = massVetaErrorBins[binFill];
      m_massVersusEta->SetBinContent(binFill+1,binContent);
      m_massVersusEta->SetBinError(binFill+1,binError);
    }
    m_widthVersusEta = new TH1F("KsWidthVersusEta","",10,etaBins);
    RegisterHisto(al_kshort_mon_average,m_widthVersusEta);
    //    m_widthVersusEta->SetErrorOption("s");
    m_widthVersusEta->SetXTitle("#eta");
    m_widthVersusEta->SetYTitle("Width (Gev / c^{2})");
    m_widthVersusEta->SetMarkerStyle(20);
    for(int binFill=0;binFill<10;binFill++) {
      Double_t binContent = widthVetaBins[binFill];
      Double_t binError = widthVetaErrorBins[binFill];
      m_widthVersusEta->SetBinContent(binFill+1,binContent);
      m_widthVersusEta->SetBinError(binFill+1,binError);
    }
    //    m_widthVersusEta = new TGraphErrors(10,etaBins,widthVetaBins,etaErrorBins,widthVetaErrorBins);

//     RegisterHisto(al_kshort_mon_average,m_massVersusEta);
//     RegisterHisto(al_kshort_mon_average,m_widthVersusEta);

    Double_t phiBins[11] = { (-5.0*myPi / 5) ,
			     (-4.0*myPi / 5) ,
			     (-3.0*myPi / 5) ,
			     (-2.0*myPi / 5) ,
			     (-1.0*myPi / 5) ,
			     (0.0*myPi / 5) ,
			     (1.0*myPi / 5) ,
			     (2.0*myPi / 5) ,
			     (3.0*myPi / 5) ,
			     (4.0*myPi / 5) ,
			     (5.0*myPi / 5) };
//     Double_t phiBins[10] = { (-4.5*myPi / 5) ,
// 			     (-3.5*myPi / 5) ,
// 			     (-2.5*myPi / 5) ,
// 			     (-1.5*myPi / 5) ,
// 			     (-0.5*myPi / 5) ,
// 			     (0.5*myPi / 5) ,
// 			     (1.5*myPi / 5) ,
// 			     (2.5*myPi / 5) ,
// 			     (3.5*myPi / 5) ,
// 			     (4.5*myPi / 5) };
//     Double_t phiErrorBins[10] = {0.5*myPi/5,
// 				 0.5*myPi/5,
// 				 0.5*myPi/5,
// 				 0.5*myPi/5,
// 				 0.5*myPi/5,
// 				 0.5*myPi/5,
// 				 0.5*myPi/5,
// 				 0.5*myPi/5,
// 				 0.5*myPi/5,
// 				 0.5*myPi/5 };
    Double_t massVphiBins[10], massVphiErrorBins[10], widthVphiBins[10], widthVphiErrorBins[10];
    for(int binFill=0;binFill<10;binFill++) {
      // if (m_massVPhiBinHistos[binFill]->GetEntries()>=MinStat) m_massVPhiBinHistos[binFill]->Fit(func,"lmhqn");
      massVphiBins[binFill] = func->GetParameter(1);
      massVphiErrorBins[binFill] = func->GetParError(1);
      widthVphiBins[binFill] = func->GetParameter(2);
      widthVphiErrorBins[binFill] = func->GetParError(2);
    }
    m_massVersusPhi = new TH1F("KsMassVersusPhi","",10,phiBins);
    RegisterHisto(al_kshort_mon_average,m_massVersusPhi);
    //    m_massVersusPhi->SetErrorOption("s");
    m_massVersusPhi->SetXTitle("#phi");
    m_massVersusPhi->SetYTitle("Mass (Gev / c^{2})");
    m_massVersusPhi->SetMarkerStyle(20);
    for(int binFill=0;binFill<10;binFill++) {
      Double_t binContent = massVphiBins[binFill];
      Double_t binError = massVphiErrorBins[binFill];
      m_massVersusPhi->SetBinContent(binFill+1,binContent);
      m_massVersusPhi->SetBinError(binFill+1,binError);
    }
    //    m_massVersusPhi = new TGraphErrors(10,phiBins,massVphiBins,phiErrorBins,massVphiErrorBins);
    m_widthVersusPhi = new TH1F("KsWidthVersusPhi","",10,phiBins);
    RegisterHisto(al_kshort_mon_average,m_widthVersusPhi);
    //    m_widthVersusPhi->SetErrorOption("s");
    m_widthVersusPhi->SetXTitle("#phi");
    m_widthVersusPhi->SetYTitle("Width (Gev / c^{2})");
    m_widthVersusPhi->SetMarkerStyle(20);
    for(int binFill=0;binFill<10;binFill++) {
      Double_t binContent = widthVphiBins[binFill];
      Double_t binError = widthVphiErrorBins[binFill];
      m_widthVersusPhi->SetBinContent(binFill+1,binContent);
      m_widthVersusPhi->SetBinError(binFill+1,binError);
    }

//     RegisterHisto(al_kshort_mon_average,m_massVersusPhi);
//     RegisterHisto(al_kshort_mon_average,m_widthVersusPhi);

    Double_t curvatureDiffBins[7] = {-0.0012,-0.0008,-0.0004,0.0000,0.0004,0.0008,0.0012};
    Double_t massVcurvatureDiffBins[6], massVcurvatureDiffErrorBins[6], widthVcurvatureDiffBins[6], widthVcurvatureDiffErrorBins[6];
    for(int binFill=0;binFill<6;binFill++) {
      // if (m_massVCurvatureDiffBinHistos[binFill]->GetEntries()>=MinStat) m_massVCurvatureDiffBinHistos[binFill]->Fit(func,"lmhqn");
      massVcurvatureDiffBins[binFill] = func->GetParameter(1);
      massVcurvatureDiffErrorBins[binFill] = func->GetParError(1);
      widthVcurvatureDiffBins[binFill] = func->GetParameter(2);
      widthVcurvatureDiffErrorBins[binFill] = func->GetParError(2);
    }
    m_massVersusCurvatureDiff = new TH1F("KsMassVersusCurvatureDiff","",6,curvatureDiffBins);
    RegisterHisto(al_kshort_mon_average,m_massVersusCurvatureDiff);
    //    m_massVersusCurvatureDiff->SetErrorOption("s");
    m_massVersusCurvatureDiff->SetXTitle("1/p_{T}(#pi^{+}) - 1/p_{T}(#pi^{-}) [GeV^{-1}]");
    m_massVersusCurvatureDiff->SetYTitle("Mass (Gev / c^{2})");
    m_massVersusCurvatureDiff->SetMarkerStyle(20);
    for(int binFill=0;binFill<6;binFill++) {
      Double_t binContent = massVcurvatureDiffBins[binFill];
      Double_t binError = massVcurvatureDiffErrorBins[binFill];
      m_massVersusCurvatureDiff->SetBinContent(binFill+1,binContent);
      m_massVersusCurvatureDiff->SetBinError(binFill+1,binError);
    }
    m_widthVersusCurvatureDiff = new TH1F("KsWidthVersusCurvatureDiff","",6,curvatureDiffBins);
    RegisterHisto(al_kshort_mon_average,m_widthVersusCurvatureDiff);
    //    m_widthVersusCurvatureDiff->SetErrorOption("s");
    m_widthVersusCurvatureDiff->SetXTitle("1/p_{T}(#pi^{+}) - 1/p_{T}(#pi^{-}) [GeV^{-1}]");
    m_widthVersusCurvatureDiff->SetYTitle("Width (Gev / c^{2})");
    m_widthVersusCurvatureDiff->SetMarkerStyle(20);
    for(int binFill=0;binFill<6;binFill++) {
      Double_t binContent = widthVcurvatureDiffBins[binFill];
      Double_t binError = widthVcurvatureDiffErrorBins[binFill];
      m_widthVersusCurvatureDiff->SetBinContent(binFill+1,binContent);
      m_widthVersusCurvatureDiff->SetBinError(binFill+1,binError);
    }
    //    m_widthVersusCurvatureDiff = new TGraphErrors(10,curvatureDiffBins,widthVcurvatureDiffBins,curvatureDiffErrorBins,widthVcurvatureDiffErrorBins);
  }

  return StatusCode::SUCCESS;

}
