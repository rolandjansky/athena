/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonSegmentTruthRelatedPlots.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Muon{

MuonSegmentTruthRelatedPlots::MuonSegmentTruthRelatedPlots(PlotBase* pParent, const std::string& sDir):
  PlotBase(pParent, sDir),
  nPrecisionHits(this,"diff_nPrecisionHits","diff(reco-truth) segment precision hits;hits;Entries",-5,5),
  nPhiLayers(this,"diff_nPhiLayers","diff(reco-truth) segment phi layers;#phi layers;Entries",-5,5),
  nTrigEtaLayers(this,"diff_nTrigEtaLayers","diff(reco-truth) segment eta trigger layers;#eta trigger layers;Entries",-5,5)
{  
  //booking histograms
  dxpos = Book1D("dxpos","Resolution x-position;#Delta x_{pos};Entries",400,-14000,14000);
  dypos = Book1D("dypos","Resolution y-position;#Delta y_{pos};Entries",400,-14000,14000);
  dzpos = Book1D("dzpos","Resolution z-position;#Delta z_{pos};Entries",500,-22000,22000);

  detapos = Book1D("detapos","Resolution eta-position;#Delta#eta_{pos};Entries",64,-3.2,3.2);
  dphipos = Book1D("dphipos","Resolution phi-position;#Delta#phi_{pos};Entries",64,-3.2,3.2);

  detadir = Book1D("detadir","Resolution eta-direction;#Delta#eta_{dir};Entries",100,-10.,10.);
  dphidir = Book1D("dphidir","Resolution phi-direction;#Delta#phi_{dir};Entries",100,-10.,10.);

}

MuonSegmentTruthRelatedPlots::~MuonSegmentTruthRelatedPlots() 
{
}

  void MuonSegmentTruthRelatedPlots::fill(const xAOD::MuonSegment& muSeg,const xAOD::MuonSegment& truthMuSeg, float weight)
{

  //// hit difference plots

  float muSegEta = 0;
  float muSegPhi = 0;
  Amg::Vector3D globalPos(0,0,0);
  Amg::Vector3D truthGlobalPos(0,0,0);

  if ( (muSeg.x()) && (muSeg.y()) && (muSeg.z()) && muSeg.x()!=0 && muSeg.y()!=0 && muSeg.z()!=0 ) {
    //protect against cases with no hit information!
    globalPos = Amg::Vector3D(muSeg.x(),muSeg.y(),muSeg.z());    
    muSegEta = globalPos.eta();
    muSegPhi = globalPos.phi();
  }

  nPrecisionHits.fill(truthMuSeg.nPrecisionHits()-muSeg.nPrecisionHits(), muSegEta, muSegPhi);
  nPhiLayers.fill(truthMuSeg.nPhiLayers()-muSeg.nPhiLayers(), muSegEta, muSegPhi);
  nTrigEtaLayers.fill(truthMuSeg.nTrigEtaLayers()-muSeg.nTrigEtaLayers(), muSegEta, muSegPhi);
 

  //// resolution plots

  //protect against cases with no hit information!
  if ( !( (muSeg.x()) && (muSeg.y()) && (muSeg.z()) && muSeg.x()!=0 && muSeg.y()!=0 && muSeg.z()!=0) ) return;
  if ( !( (truthMuSeg.x()) && (truthMuSeg.y()) && (truthMuSeg.z()) && truthMuSeg.x()!=0 && truthMuSeg.y()!=0 && truthMuSeg.z()!=0) ) return; 
  if ( !( (muSeg.px()) && (muSeg.py()) && (muSeg.pz()) && muSeg.px()!=0 && muSeg.py()!=0 && muSeg.pz()!=0) ) return;
  if ( !( (truthMuSeg.px()) && (truthMuSeg.py()) && (truthMuSeg.pz()) && truthMuSeg.px()!=0 && truthMuSeg.py()!=0 && truthMuSeg.pz()!=0) ) return; 


  dxpos->Fill(muSeg.x()-truthMuSeg.x(), weight);
  dypos->Fill(muSeg.y()-truthMuSeg.y(), weight);
  dzpos->Fill(muSeg.z()-truthMuSeg.z(), weight);
  
  truthGlobalPos = Amg::Vector3D(truthMuSeg.x(),truthMuSeg.y(),truthMuSeg.z());

  detapos->Fill(globalPos.eta()-truthGlobalPos.eta(), weight);
  dphipos->Fill(globalPos.deltaPhi(truthGlobalPos), weight);

  Amg::Vector3D globalDir(muSeg.px(),muSeg.py(),muSeg.pz());
  Amg::Vector3D truthGlobalDir(truthMuSeg.px(),truthMuSeg.py(),truthMuSeg.pz());

  dphidir->Fill(globalDir.deltaPhi(truthGlobalDir),weight);
  detadir->Fill(globalDir.eta()-truthGlobalDir.eta(),weight);

}

} // closing namespace Muon
