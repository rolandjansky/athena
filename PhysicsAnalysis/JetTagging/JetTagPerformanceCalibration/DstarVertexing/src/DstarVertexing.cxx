/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DstarVertexing includes
#include "DstarVertexing/DstarVertexing.h"
#include "GaudiKernel/IHistogramSvc.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "xAODEventInfo/EventInfo.h"

#include "xAODJet/JetContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTracking/TrackParticle.h"

#include "DataModel/ElementLink.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

#include "InDetConversionFinderTools/InDetConversionFinderTools.h"
#include "InDetConversionFinderTools/ConversionFinderUtils.h"

#include "TrkVertexAnalysisUtils/V0Tools.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkV0Fitter/TrkV0VertexFitter.h"

#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "DataModel/ElementLink.h"

#include "CLHEP/Matrix/Vector.h"

#include <TLorentzVector.h>

#include <bitset>
extern "C" {
#   include <stdint.h>
}

#include "TH1.h"

using CLHEP::HepLorentzVector;
using CLHEP::HepVector;


DstarVertexing::DstarVertexing(const std::string& t, const std::string& n, const IInterface*  p ) :
          AthAlgTool(t,n,p),
          m_iVertexFitter("Trk::TrkVKalVrtFitter"),
          m_D0VxContainerName("D0VxAug"),
          m_JetsCollection("AntiKt4LCTopoJets"),
          m_JetPtCut(0.0)
  {
   declareProperty("D0VertexOutputName",m_D0VxContainerName);
   declareInterface<DerivationFramework::IAugmentationTool>(this);
   declareProperty("InputJetsColletion", m_JetsCollection);
   declareProperty("JetPtCut", m_JetPtCut);
  }

DstarVertexing::~DstarVertexing() {}


StatusCode DstarVertexing::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}



StatusCode DstarVertexing::addBranches() const{ 
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  
  const xAOD::EventInfo* eventInfo = 0;
  CHECK(evtStore()->retrieve(eventInfo, "EventInfo"));
  
  // int n_evt = eventInfo->eventNumber();
    
  xAOD::VertexContainer* D0VxContainer = new xAOD::VertexContainer;
  xAOD::VertexAuxContainer* D0AuxVxContainer = new xAOD::VertexAuxContainer;
  D0VxContainer->setStore(D0AuxVxContainer);

  double pt_cut=1000.0;
  double kpi_deltaR_cut  =  1.0;
  double D0pi_deltaR_cut =  0.4;
  double d0_mass_cut=40;
  double md0 = 1864.84;
  double mk  = 493.68;
  double mpi = 139.57;

  // Get ID tracks
  const xAOD::TrackParticleContainer* importedTrackCollection(0);

  if(evtStore()->retrieve(importedTrackCollection,"InDetTrackParticles").isFailure()){
      ATH_MSG_WARNING("No TrackParticle collection with name " << "InDetTrackParticles" << " found in StoreGate!");
      return StatusCode::SUCCESS;

 } else {
      ATH_MSG_DEBUG("Found TrackParticle collection " << "InDetTrackParticles" << " in StoreGate!");
  }

  // Select ID tracks
  typedef std::vector<const xAOD::TrackParticle*> TrackBag;
  TrackBag theIDTracksAfterSelection;
  
  
  xAOD::TrackParticleContainer::const_iterator trkCItr;

  for (trkCItr=importedTrackCollection->begin(); trkCItr!=importedTrackCollection->end(); ++trkCItr) {
    const xAOD::TrackParticle* TP = (*trkCItr);

    uint8_t numberOfSCTHits = 0;
    if( TP->summaryValue(numberOfSCTHits,xAOD::numberOfSCTHits) ){}
    if(numberOfSCTHits<6) continue;


    uint8_t numberOfPixelHits = 0;
    if( TP->summaryValue(numberOfPixelHits,xAOD::numberOfPixelHits) ){}
    if( numberOfPixelHits <= 0) continue;

    theIDTracksAfterSelection.push_back(TP);

  }
  
  //Get and select the jets
  const xAOD::JetContainer* jets = 0;
  CHECK( evtStore()->retrieve( jets, m_JetsCollection) );
  
  typedef std::vector<const xAOD::Jet*> JetBag;
  JetBag theJetsAfterSelection;
  
  xAOD::JetContainer::const_iterator jet;
  for ( jet=jets->begin();jet!=jets->end();++jet ) {
    const xAOD::Jet* JP = (*jet);
    if((JP->pt()/1000)<m_JetPtCut)continue;
    theJetsAfterSelection.push_back(JP);
  }
  
  //Loop over the jets and pair the tracks for vertex fitting
  std::vector<const xAOD::Jet*>::const_iterator i_jet = theJetsAfterSelection.begin();
  std::vector<const xAOD::Jet*>::const_iterator jetEnd = theJetsAfterSelection.end();
  std::vector<const xAOD::TrackParticle*>::const_iterator i_trk = theIDTracksAfterSelection.begin();
  
  for (i_jet=theJetsAfterSelection.begin();i_jet != jetEnd; ++i_jet){
    TLorentzVector i_jet_p4 = (*i_jet)->p4();
    TrackBag theIDTracksAssoToJet;
    
    std::vector<const xAOD::IParticle *> trackVector = (*i_jet)->getAssociatedObjects<xAOD::IParticle>(xAOD::JetAttribute::GhostTrack);
    std::vector<const xAOD::IParticle *>::const_iterator trkItr;
    for (trkItr = trackVector.begin(); trkItr != trackVector.end(); trkItr++) {
      const xAOD::TrackParticle * trackPart = dynamic_cast<const xAOD::TrackParticle *>(*trkItr);
      
      uint8_t numberOfSCTHits = 0;
      if( trackPart->summaryValue(numberOfSCTHits,xAOD::numberOfSCTHits) ){}
      if(numberOfSCTHits<6) continue;
      
      
      uint8_t numberOfPixelHits = 0;
      if( trackPart->summaryValue(numberOfPixelHits,xAOD::numberOfPixelHits) ){}
      if( numberOfPixelHits <= 0) continue;
      theIDTracksAssoToJet.push_back(trackPart);
      
    }
    
    std::vector<const xAOD::TrackParticle*>::const_iterator trk_k = theIDTracksAssoToJet.begin();
    std::vector<const xAOD::TrackParticle*>::const_iterator trk_pi = theIDTracksAssoToJet.begin();
    std::vector<const xAOD::TrackParticle*>::const_iterator trk_pis = theIDTracksAssoToJet.begin();
    std::vector<const xAOD::TrackParticle*>::const_iterator trkEnd = theIDTracksAssoToJet.end();
    TLorentzVector ptrk_k, ptrk_pi, ptrk_pis;
    int i=0, j=0, k=0;

    for (theIDTracksAssoToJet.begin(); trk_k != theIDTracksAssoToJet.end(); ++trk_k, ++i){
      j = 0;
      k = 0;
      
      double p_k = 1./fabs((*trk_k)->qOverP());
      double ch_k = (*trk_k)->charge();
      double pt_k = (*trk_k)->pt();
      
      if(pt_k < pt_cut) continue;

      ptrk_k = (*trk_k)->p4();

      for (trk_pi = theIDTracksAssoToJet.begin(); trk_pi != theIDTracksAssoToJet.end(); ++trk_pi, ++j){
        k = 0;
        double pt_pi = (*trk_pi)->pt();
        if( (trk_pi==trk_k)|| (pt_pi < pt_cut)) continue;
        double p_pi = 1./fabs((*trk_pi)->qOverP());
        double ch_pi = (*trk_pi)->charge();
	
        ptrk_pi = (*trk_pi)->p4();
	
        if(ch_k*ch_pi>=0) continue;
	
	
        if (ptrk_k.DeltaR(ptrk_pi) > kpi_deltaR_cut) continue;
	
        ptrk_k.SetE(sqrt(p_k*p_k + mk*mk));
        ptrk_pi.SetE(sqrt(p_pi*p_pi + mpi*mpi));
	
        std::vector<const xAOD::TrackParticle*> theTracks;
        theTracks.clear();
        theTracks.push_back(*trk_k);
        theTracks.push_back(*trk_pi);
	
        Amg::Vector3D startingPoint(0,0,0);
	
        xAOD::Vertex* D0VxCandidate = m_iVertexFitter->fit(theTracks,startingPoint);
        bool bStore = false;
	

	if(D0VxCandidate!=NULL){
	  TLorentzVector pd0(ptrk_k+ptrk_pi);
	  //if(D0VxCandidate->chiSquared() < 50.0){
	  if (fabs(pd0.M()-md0) <= d0_mass_cut) {
	    for (trk_pis = theIDTracksAssoToJet.begin(); trk_pis != theIDTracksAssoToJet.end(); ++trk_pis, ++k){
	      if ((trk_pis==trk_pi) || (trk_pis==trk_k)) continue;
	      double p_pis = 1./fabs((*trk_pis)->qOverP());
	      double ch_pis = (*trk_pis)->charge();
	      if (ch_k*ch_pis>=0) continue;
	      
	      ptrk_pis = (*trk_pis)->p4();
	      
	      if (pd0.DeltaR(ptrk_pis) > D0pi_deltaR_cut) continue;

	      ptrk_pis.SetE(sqrt(p_pis*p_pis + mpi*mpi));
	      
              TLorentzVector pds(pd0+ptrk_pis);
	      double pt_dstar = sqrt(pds.Px()*pds.Px() + pds.Py()*pds.Py());
	      
	      double dm = pds.M() - pd0.M();
	      
	      if (dm > 170) continue;

	      if (pt_dstar < 4500) continue;
	      if ( pds.DeltaR(i_jet_p4) > 0.3) continue;
	      double momentum_DsonJet = fabs(pds.Px()*i_jet_p4[0] + pds.Py()*i_jet_p4[1] + pds.Pz()*i_jet_p4[2])/sqrt(i_jet_p4[0]*i_jet_p4[0] + i_jet_p4[1]*i_jet_p4[1] + i_jet_p4[2]*i_jet_p4[2]);
	      if((momentum_DsonJet/i_jet_p4[3]) < 0.5)continue;
	      bStore = true;
	      
	    }//trk_pis
	  }
	  
	  if(bStore == true){
	    ATH_MSG_DEBUG("Found D0 canidate, x of D0: " << D0VxCandidate->x() );
	    static SG::AuxElement::Decorator< std::vector<double> > KaonTrackDecor("KaonTrack");
	    static SG::AuxElement::Decorator< std::vector<double> > PionTrackDecor("PionTrack");
	    static SG::AuxElement::Decorator< std::vector<double> > SlowPionTrackDecor("SlowPionTrack"); 

	    std::vector<double> kaonTrack;
	    kaonTrack.push_back(ptrk_k[0]);kaonTrack.push_back(ptrk_k[1]);kaonTrack.push_back(ptrk_k[2]);kaonTrack.push_back(ptrk_k[3]);
	    std::vector<double> pionTrack;
	    pionTrack.push_back(ptrk_pi[0]);pionTrack.push_back(ptrk_pi[1]);pionTrack.push_back(ptrk_pi[2]);pionTrack.push_back(ptrk_pi[3]);
	    std::vector<double> slowpionTrack;
	    slowpionTrack.push_back(ptrk_pis[0]);slowpionTrack.push_back(ptrk_pis[1]);slowpionTrack.push_back(ptrk_pis[2]);slowpionTrack.push_back(ptrk_pis[3]); 
	    
	    KaonTrackDecor(*D0VxCandidate) = kaonTrack;
	    PionTrackDecor(*D0VxCandidate) = pionTrack;
	    SlowPionTrackDecor(*D0VxCandidate) = slowpionTrack;
	    
	    D0VxContainer->push_back(D0VxCandidate);}
	  else{delete D0VxCandidate;}

	}//D0VxCandidta
      }//trk_pi
    }//trk_k
  }//i_jet
  
  if (!evtStore()->contains<xAOD::VertexContainer>(m_D0VxContainerName)){
    CHECK (evtStore()->record(D0VxContainer,m_D0VxContainerName));}
  
  
  if (!evtStore()->contains<xAOD::VertexAuxContainer>(m_D0VxContainerName+"Aux.")){
    CHECK (evtStore()->record(D0AuxVxContainer,m_D0VxContainerName+"Aux."));}

  return StatusCode::SUCCESS;
}//addBranches()


StatusCode DstarVertexing::finalize() {
  return StatusCode::SUCCESS;
}

