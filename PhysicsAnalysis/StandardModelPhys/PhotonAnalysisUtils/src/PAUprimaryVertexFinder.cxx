/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// PAUprimaryVertexFinder.cxx, (c) ATLAS Detector software 2007
///////////////////////////////////////////////////////////////////////////////
// author: Iro Koletsou, koletsou@lal.in2p3.fr
///////////////////////////////////////////////////////////////////////////////
// This is a tool that determinates the primary vertex position on z. The entries are the z pointed by the calorimeter and its uncertainty. Then a likelihood value is associated to each one of the event's vertices, determinated by its distance from the z pointed by the calorimeter and the sum of the tracks' PT2 for every vertex. The output value is the z of the vertex with the maximum likelihood value, as it is given by the inner detector.
///////////////////////////////////////////////////////////////////////////////

// Trk
#include "PhotonAnalysisUtils/PAUprimaryVertexFinder.h"
#include "PhotonAnalysisUtils/IPAUprimaryVertexFinder.h"
#include "VxVertex/VxContainer.h"	
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"
#include "TFile.h"
#include "PathResolver/PathResolver.h"

#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkTrack/LinkToTrack.h"

#include <TMath.h>
#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooKeysPdf.h"

#include "GaudiKernel/MsgStream.h"
PAUprimaryVertexFinder::likelihoodPV::likelihoodPV() :
  vertex(0), zvertex(-999999.),likelihood_ratio(-999999.) {}

//==========================================
PAUprimaryVertexFinder::PAUprimaryVertexFinder(const std::string& t, const std::string& n, const IInterface* p) : AthAlgTool(t,n,p) {
  declareInterface<IPAUprimaryVertexFinder>(this);
}
//==========================================
PAUprimaryVertexFinder::~PAUprimaryVertexFinder(){}
//==========================================
StatusCode PAUprimaryVertexFinder::initialize()
{
  MsgStream mlog(msgSvc(), name());
  
  std::string RefFile = "PT2dist.root" ;
  std::string InputFilePath = PathResolver::find_file (RefFile, "DATAPATH");
  if ( InputFilePath == "" ) {
    mlog << MSG::ERROR << "Can't access input PDF file " << RefFile << endreq;
    return StatusCode::FAILURE;
  }
  mlog << MSG::INFO << "PrimaryVertex pdf file found at " << InputFilePath <<endreq;
  m_inputPDFfile = new TFile( InputFilePath.c_str(), "READ" );
  if ( m_inputPDFfile->IsOpen() ) {
    m_signal = (TH1*) m_inputPDFfile->Get("PT2_higgs") ;
    m_bruit  = (TH1*) m_inputPDFfile->Get("PT2_MB") ;
  } else {
    mlog << MSG::ERROR << "Can't open pdf file " << InputFilePath <<endreq;
    return StatusCode::FAILURE;
  }


  std::string PDFSigFile = "PDF_sumpt2_sig.root" ;
  std::string InputFilePDFSigPath = PathResolver::find_file (PDFSigFile, "DATAPATH");
  if ( InputFilePDFSigPath == "" ) {
    mlog << MSG::ERROR << "Can't access input signal PDF file " << PDFSigFile << endreq;
    return StatusCode::FAILURE;
  }
  mlog << MSG::INFO << "Signal PDF file found at " << InputFilePDFSigPath <<endreq;
  m_inputPDFSigfile = new TFile( InputFilePDFSigPath.c_str(), "READ" );
  if ( m_inputPDFSigfile->IsOpen() ) {
    m_w_sig = dynamic_cast<RooWorkspace*>(m_inputPDFSigfile->Get("w")) ;
  } else {
    mlog << MSG::ERROR << "Can't open signal pdf file " << InputFilePDFSigPath <<endreq;
    return StatusCode::FAILURE;
  }

  std::string PDFBkgFile = "PDF_sumpt2_bkg.root" ;
  std::string InputFilePDFBkgPath = PathResolver::find_file (PDFBkgFile, "DATAPATH");
  if ( InputFilePDFBkgPath == "" ) {
    mlog << MSG::ERROR << "Can't access input background PDF file " << PDFBkgFile << endreq;
    return StatusCode::FAILURE;
  }
  mlog << MSG::INFO << "Background PDF file found at " << InputFilePDFBkgPath <<endreq;
  m_inputPDFBkgfile = new TFile( InputFilePDFBkgPath.c_str(), "READ" );
  if ( m_inputPDFBkgfile->IsOpen() ) {
    m_w_bkg = dynamic_cast<RooWorkspace*>(m_inputPDFBkgfile->Get("w")) ;
  } else {
    mlog << MSG::ERROR << "Can't open background pdf file " << InputFilePDFBkgPath <<endreq;
    return StatusCode::FAILURE;
  }

  
  mlog <<  MSG::INFO  << name() <<" initialize() successful" << endreq;    
  return StatusCode::SUCCESS;
}
//==========================================
StatusCode PAUprimaryVertexFinder::finalize()
{
  MsgStream mlog(msgSvc(), name());
  mlog <<  MSG::INFO  << name() <<" finalize() successful" << endreq;

  delete m_w_bkg;
  delete m_w_sig;
  delete m_signal;
  delete m_bruit;

  return StatusCode::SUCCESS;
}
//==========================================
const std::vector<double> PAUprimaryVertexFinder::PrimaryVxFinder(const double z, const double sigma_z) const {
  
  MsgStream mlog(msgSvc(), name());
  mlog << MSG::VERBOSE << "entering PrimaryVxFinder" << endreq ;
  double z_current_vertex=0;
  double likelihood_ratio=0;
  double max_likelihood_ratio=-99999;
  double z_max_likelihood_ratio=0;
  
  std::vector<double> result_for_output;

  /// get the primary vertex
  StatusCode sc;
  const VxContainer* vxTES;
  sc=evtStore()->retrieve(vxTES,"VxPrimaryCandidate");
  if (sc.isFailure() || !vxTES) {
    mlog << MSG::WARNING
	 << "No VxPrimaryCandidate  found in TDS"
	 << endreq;
    return result_for_output;
  }

  int current_vertex=0;
  int index_max_likelihood_ratio=-999;
  int Ntvec=vxTES->size();

  VxContainer::const_iterator vtxItr  = vxTES->begin();
  VxContainer::const_iterator vtxItrE = vxTES->end();

  for (; vtxItr != vtxItrE; vtxItr++) {   // loop over the event's vertices
    const Trk::VxCandidate* vxcand = (*vtxItr);
    const Trk::RecVertex* vert = &(vxcand->recVertex());
    z_current_vertex = vert->position()[2];
    
    const std::vector<Trk::VxTrackAtVertex*>* trackAtVertex= vxcand->vxTrackAtVertex();
    int Ntrk=trackAtVertex->size();
    double Sum_pt2_tracks_at_vertex=0;
    
    for (int itr=0;itr<Ntrk;itr++) { // loop over each vertex tracks 
      Trk::ITrackLink* link = ((*trackAtVertex)[itr])->trackOrParticleLink();
      const Trk::LinkToTrackParticleBase* tr_part = dynamic_cast<const Trk::LinkToTrackParticleBase*>(link);
      if(!tr_part) continue;
      const Rec::TrackParticle* tp = dynamic_cast<const Rec::TrackParticle*>(**tr_part); 
      if(!tp) continue;
      const Trk::TrackParameters *aMeasPer = tp->measuredPerigee();
      Sum_pt2_tracks_at_vertex+=aMeasPer->momentum().perp()*aMeasPer->momentum().perp();
    }

    double vtx_pt2 = Sum_pt2_tracks_at_vertex*sqrt((double)Ntrk);
    
    if (Ntvec==1 && vtx_pt2<0.0001) { //failing of vertex fitting, then don't change z
      result_for_output.push_back(z);
      result_for_output.push_back(0.);
      result_for_output.push_back(0.);
      return result_for_output;
    }
    
    //calculation of the part of the likelihood_ratio that is associated to the pointing
    double likelihood_ratio_calo=exp(-(z_current_vertex-z)*(z_current_vertex-z)/(2*sigma_z*sigma_z))/sqrt(2*M_PI*sigma_z*sigma_z);
    
    // calculation of the pt2 part of the likelihood_ratio
    double pt2_higgs=m_signal->GetBinContent((m_signal->GetXaxis())->FindBin(Sum_pt2_tracks_at_vertex/1.e6));
    double pt2_MB=m_bruit->GetBinContent((m_bruit->GetXaxis())->FindBin(Sum_pt2_tracks_at_vertex/1.e6));

    double likelihood_ratio_pt2=1; //if division by 0, put likelihood=1 : no weight
      
    if (pt2_MB) //prevent division by 0
      likelihood_ratio_pt2=pt2_higgs/pt2_MB;
    
    likelihood_ratio=likelihood_ratio_calo*likelihood_ratio_pt2;
    
    if (likelihood_ratio>max_likelihood_ratio) {
      max_likelihood_ratio=likelihood_ratio;
      z_max_likelihood_ratio=z_current_vertex;
      index_max_likelihood_ratio=current_vertex;
    }
    
    current_vertex++;
  }// end of the vertice's loop
  
  result_for_output.push_back(z_max_likelihood_ratio);
  result_for_output.push_back(max_likelihood_ratio);
  result_for_output.push_back(double(index_max_likelihood_ratio));
  return result_for_output;
}


PAUprimaryVertexFinder::likelihoodPV PAUprimaryVertexFinder::RooPrimaryVxFinder(const double z, const double sigma_z) {
  
  MsgStream mlog(msgSvc(), name());
  mlog << MSG::VERBOSE << "entering PrimaryVxFinder" << endreq ;

  double z_current_vertex=0;
  double likelihood_ratio=0;
  double max_likelihood_ratio=-99999;
  double z_max_likelihood_ratio=0;


  RooRealVar* sumPt2_sig = m_w_sig->var("PV_sumPt2_log") ;
  RooKeysPdf* model1_sig = static_cast<RooKeysPdf*> ( m_w_sig->pdf("kest2") ) ;
  RooRealVar* sumPt2_bkg = m_w_bkg->var("PV_sumPt2_log") ;
  RooKeysPdf* model1_bkg = static_cast<RooKeysPdf*> ( m_w_bkg->pdf("kest2") ) ;

  
  likelihoodPV likPV;
  likPV.vertex = 0;
  likPV.zvertex = -999999.;
  likPV.likelihood_ratio = -999999.;


  /// get the primary vertex
  StatusCode sc;
  const VxContainer* vxTES;
  sc=evtStore()->retrieve(vxTES,"VxPrimaryCandidate");
  if (sc.isFailure() || !vxTES) {
    mlog << MSG::WARNING
	 << "No VxPrimaryCandidate  found in TDS"
	 << endreq;
    return likPV;
  }

  int current_vertex=0;
  int Ntvec=vxTES->size();

  VxContainer::const_iterator vtxItr  = vxTES->begin();
  VxContainer::const_iterator vtxItrE = vxTES->end();
  const Trk::VxCandidate* vertex = 0 ;

  if (vxTES->size()==1) { // if only dummy vertex
    mlog << MSG::WARNING
	 << "VxPrimaryCandidate container of size 1 ---> return the dummy vertex"
	 << endreq;
    const Trk::VxCandidate* vxcand = (*vxTES->begin());
    likPV.vertex = vxcand;
    return likPV;
  }
 
  for (; vtxItr != vtxItrE-1; vtxItr++) {   // loop over the event's vertices
    const Trk::VxCandidate* vxcand = (*vtxItr);
    const Trk::RecVertex* vert = &(vxcand->recVertex());
    z_current_vertex = vert->position()[2];
    
    const std::vector<Trk::VxTrackAtVertex*>* trackAtVertex= vxcand->vxTrackAtVertex();
    int Ntrk=trackAtVertex->size();
    double Sum_pt2_tracks_at_vertex=0;
    
    for (int itr=0;itr<Ntrk;itr++) { // loop over each vertex tracks 
      Trk::ITrackLink* link = ((*trackAtVertex)[itr])->trackOrParticleLink();
      const Trk::LinkToTrackParticleBase* tr_part = dynamic_cast<const Trk::LinkToTrackParticleBase*>(link);
      if(!tr_part) continue;
      const Rec::TrackParticle* tp = dynamic_cast<const Rec::TrackParticle*>(**tr_part); 
      if(!tp) continue;
      const Trk::TrackParameters *aMeasPer = tp->measuredPerigee();
      Sum_pt2_tracks_at_vertex += aMeasPer->momentum().perp()*aMeasPer->momentum().perp();
    }

    double vtx_pt2 = Sum_pt2_tracks_at_vertex;//*sqrt((double)Ntrk);
    
    if (Ntvec==1 && vtx_pt2<0.0001) { //failing of vertex fitting, then don't change z
      return likPV;
    }
    
    //calculation of the part of the likelihood_ratio that is associated to the pointing
    double likelihood_ratio_calo=exp(-(z_current_vertex-z)*(z_current_vertex-z)/(2*sigma_z*sigma_z))/sqrt(2*M_PI*sigma_z*sigma_z);
    

    // calculation of the pt2 part of the likelihood_ratio
    double pt2_higgs = 0;
    double pt2_MB = 0;
    if ( Sum_pt2_tracks_at_vertex>0 ) {
      sumPt2_sig->setVal(TMath::Log10(Sum_pt2_tracks_at_vertex));
      pt2_higgs = (double)model1_sig->getVal();
      sumPt2_bkg->setVal(TMath::Log10(Sum_pt2_tracks_at_vertex));
      pt2_MB = (double)model1_bkg->getVal();
    } 
    
    double likelihood_ratio_pt2=1; //if division by 0, put likelihood=1 : no weight
      
    if (pt2_MB) //prevent division by 0
      likelihood_ratio_pt2=pt2_higgs/pt2_MB;
    
    likelihood_ratio=likelihood_ratio_calo*likelihood_ratio_pt2;
    
    if (likelihood_ratio>max_likelihood_ratio) {
      max_likelihood_ratio=likelihood_ratio;
      z_max_likelihood_ratio=z_current_vertex;
      vertex = vxcand;
    }
    
    current_vertex++;
  }// end of the vertice's loop
  
  likPV.vertex = vertex;
  likPV.zvertex = z_max_likelihood_ratio;
  likPV.likelihood_ratio = max_likelihood_ratio;

  return likPV;
}
