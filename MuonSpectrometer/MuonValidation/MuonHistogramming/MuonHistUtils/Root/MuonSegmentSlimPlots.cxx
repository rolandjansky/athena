/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonSegmentSlimPlots.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "MuonStationIndex/MuonStationIndex.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Muon{
  
MuonSegmentSlimPlots::MuonSegmentSlimPlots(PlotBase* pParent, const std::string& sDir): PlotBase(pParent, sDir) {  

  //booking histograms
  segmentfitChi2 = Book1D("segmentfitChi2", "Segment fit #chi^{2};#chi^{2};Entries", 100,0,200);
  segmentfitNdof = Book1D("segmentfitNdof", "Segment fit N_{dof};N_{dof};Entries", 100,0,20);
  segmentfitChi2oNdof = Book1D("segmentfitChi2oNdof", "Segment fit #chi^{2}/N_{dof};Segment fit #chi^{2}/N_{dof};Entries", 100,0,20);
  
  t0 = Book1D("t0","Segment Refit t_{0};t_{0};Entries",100,-25,25);
  t0_top = Book1D("t0_top","Segment Refit t_{0} for y>0;t_{0};Entries",100,-25,25);
  t0_bottom = Book1D("t0_bottom","Segment Refit t_{0} for y<0;t_{0};Entries",100,-25,25);
  t0err = Book1D("t0err","Segment Refit t_{0} error;t_{0} error;Entries",100,0,10);
  t0err_top = Book1D("t0err_top","Segment Refit t_{0} error for y>0;t_{0} error;Entries",100,0,10);
  t0err_bottom = Book1D("t0err_bottom","Segment Refit t_{0} error for y<0;t_{0} error;Entries",100,0,10);

  nPrecisionHits = Book1D("nPrecisionHits","Segment precision hits;hits;Entries",20,0,20);
  nPhiLayers = Book1D("nPhiLayers","Segment phi layers;#phi layers;Entries",10,0,10);
  nTrigEtaLayers = Book1D("nTrigEtaLayers","Segment eta trigger layers;#eta trigger layers;Entries",10,0,10);

  etaIndex = Book1D("etaIndex","Segment #eta Index ;#eta index;Entries",21,-10.5,10.5);
  sector = Book1D("sector","Segment phi sector;#phi sector;Entries",16,0.5,16.5);

  etadir = Book1D("etadir","Segment pointing direction eta;#eta_{dir};Entries",100,-5,5);
  etadir_barrel = Book1D("etadir_barrel","Segment pointing direction eta, barrel;#eta_{dir};Entries",100,-5,5);
  etadir_endcap = Book1D("etadir_endcap","Segment pointing direction eta, endcap;#eta_{dir};Entries",100,-5,5);
  phidir = Book1D("phidir","Segment pointing direction phi;#phi_{dir};Entries",64,-3.2,3.2);
  etaphidir = Book2D("etaphidir","Segment pointing direction phi vs eta;#eta_{dir};#phi_{dir}",64,-3.2,3.2,64,-3.2,3.2);

  chamberIndex = Book1D("chamberIndex","Chamber index; Chamber Index",Muon::MuonStationIndex::ChIndexMax,0,Muon::MuonStationIndex::ChIndexMax);
  chamberIndex_perSector = Book2D("chamberIndex_perSector","Number of Segments per Chamber, normalized by solid angle; Sector; Chamber Index ", 33, -16.5, 16.5, Muon::MuonStationIndex::ChIndexMax,0,Muon::MuonStationIndex::ChIndexMax);
  eff_chamberIndex_perSector_numerator = Book2D("eff_chamberIndex_perSector_numerator","Number of expected hits for Segments per Chamber; Sector; Chamber Index ", 33, -16.5, 16.5, Muon::MuonStationIndex::ChIndexMax,0,Muon::MuonStationIndex::ChIndexMax);
  eff_chamberIndex_perSector_denominator = Book2D("eff_chamberIndex_perSector_denominator","Number of recorded precision hits for Segments per Chamber; Sector; Chamber Index ", 33, -16.5, 16.5, Muon::MuonStationIndex::ChIndexMax,0,Muon::MuonStationIndex::ChIndexMax);
  eff_chamberIndex_perSector = Book2D("eff_chamberIndex_perSector","precision layer hit efficiency per chamber; Sector; Chamber Index ", 33, -16.5, 16.5, Muon::MuonStationIndex::ChIndexMax,0,Muon::MuonStationIndex::ChIndexMax);
  //chamberIndex_dtheta = Book2D("chamberIndex_dtheta","Segment #Delta#theta between position and momentum; #Delta#theta; Chamber Index ", 180, -90.0, 90.0, Muon::MuonStationIndex::ChIndexMax,0,Muon::MuonStationIndex::ChIndexMax);
  for (int i=1; i<=chamberIndex->GetXaxis()->GetNbins(); i++){
    const char *temp_chambername = Muon::MuonStationIndex::chName((Muon::MuonStationIndex::ChIndex)chamberIndex->GetBinLowEdge(i)).c_str();
    chamberIndex->GetXaxis()->SetBinLabel(i, temp_chambername);
    chamberIndex_perSector->GetYaxis()->SetBinLabel(i, temp_chambername);
    eff_chamberIndex_perSector_numerator->GetYaxis()->SetBinLabel(i, temp_chambername);
    eff_chamberIndex_perSector_denominator->GetYaxis()->SetBinLabel(i, temp_chambername);
    eff_chamberIndex_perSector->GetYaxis()->SetBinLabel(i, temp_chambername);
  }
}

MuonSegmentSlimPlots::~MuonSegmentSlimPlots() 
{
}

  void MuonSegmentSlimPlots::fill(const xAOD::MuonSegment& muSeg, float weight)
{
  float chi2 = muSeg.chiSquared();
  float ndof = muSeg.numberDoF();
  segmentfitChi2->Fill(chi2, weight);
  segmentfitNdof->Fill(ndof, weight);
  if (ndof>0) segmentfitChi2oNdof->Fill(muSeg.chiSquared()/muSeg.numberDoF(), weight);

  float y=muSeg.y();

  float segt0 = muSeg.t0();
  float segt0err = muSeg.t0error();

  t0->Fill(segt0, weight);
  t0err->Fill(segt0err, weight);
  if (y>0) { 
    t0_top->Fill(segt0, weight);
    t0err_top->Fill(segt0err,weight);
  } else {
    t0_bottom->Fill(segt0, weight);
    t0err_bottom->Fill(segt0err, weight);
  }

  sector->Fill(muSeg.sector(), weight);

  nPrecisionHits->Fill(muSeg.nPrecisionHits(), weight);
  nPhiLayers->Fill(muSeg.nPhiLayers(), weight);
  nTrigEtaLayers->Fill(muSeg.nTrigEtaLayers(), weight);

  int chIndex = muSeg.chamberIndex();
  float chambernorm = 1/Chamberarea[chIndex];//weight of the segment using the chamber eta-phi area
  chamberIndex->Fill(chIndex, weight);
  int sectorIndex = muSeg.sector();
  //fill the count of segments; switch the sign here to make the plots
  if (muSeg.z() < 0) { sectorIndex = - sectorIndex;}
  chamberIndex_perSector->Fill(sectorIndex, chIndex, chambernorm);
  eff_chamberIndex_perSector_numerator->Fill(sectorIndex, chIndex, (muSeg.nPrecisionHits() > Chamberexpectedhits[chIndex]) ? Chamberexpectedhits[chIndex]:muSeg.nPrecisionHits());
  eff_chamberIndex_perSector_denominator->Fill(sectorIndex, chIndex, Chamberexpectedhits[chIndex]);
  //update sector eta index plots; switch the sign back here to make the plots
  sectorIndex = muSeg.sector(); 
  
  //position and direction plots
  //protect against cases with no hit information!
   Amg::Vector3D globalDir{muSeg.px(),muSeg.py(),muSeg.pz()};
 
  if ( globalDir.mag() < DBL_EPSILON ) return;

  float eta = globalDir.eta();
  float phi = globalDir.phi();
  etadir->Fill(eta, weight);
  phidir->Fill(phi, weight);
  etaphidir->Fill(eta,phi);

}


} // closing namespace Muon
