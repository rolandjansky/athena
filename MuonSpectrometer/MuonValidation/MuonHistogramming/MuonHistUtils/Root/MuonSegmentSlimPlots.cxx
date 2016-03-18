/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonSegmentSlimPlots.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "MuonIdHelpers/MuonStationIndex.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Muon{
  
MuonSegmentSlimPlots::MuonSegmentSlimPlots(PlotBase* pParent, std::string sDir): PlotBase(pParent, sDir)
  ,  segmentfitChi2(NULL)
  ,  segmentfitNdof(NULL)
  ,  segmentfitChi2oNdof(NULL)

  ,  t0(NULL)
  ,  t0_top(NULL)
  ,  t0_bottom(NULL)
  ,  t0err(NULL)
  ,  t0err_top(NULL)
  ,  t0err_bottom(NULL)

  ,  nPrecisionHits(NULL)
  ,  nPhiLayers(NULL)
  ,  nTrigEtaLayers(NULL)

  ,  etaIndex(NULL)
  ,  sector(NULL)
  
  ,  etadir(NULL)
  ,  etadir_barrel(NULL)
  ,  etadir_endcap(NULL)
  ,  phidir(NULL)
  ,  etaphidir(NULL)

	, chamberIndex(NULL)
  , chamberIndex_perSector(NULL)
  , eff_chamberIndex_perSector_numerator(NULL)
  , eff_chamberIndex_perSector_denominator(NULL)
  , eff_chamberIndex_perSector(NULL)
  //,chamberIndex_dtheta(NULL)
{  

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

void MuonSegmentSlimPlots::fill(const xAOD::MuonSegment& muSeg)
{
  float chi2 = muSeg.chiSquared();
  float ndof = muSeg.numberDoF();
  segmentfitChi2->Fill(chi2);
  segmentfitNdof->Fill(ndof);
  if (ndof>0) segmentfitChi2oNdof->Fill(muSeg.chiSquared()/muSeg.numberDoF());

  float y=muSeg.y();

  float segt0 = muSeg.t0();
  float segt0err = muSeg.t0error();

  t0->Fill(segt0);
  t0err->Fill(segt0err);
  if (y>0) { 
    t0_top->Fill(segt0);
    t0err_top->Fill(segt0err);
  } else {
    t0_bottom->Fill(segt0);
    t0err_bottom->Fill(segt0err);
  }

  sector->Fill(muSeg.sector());

  nPrecisionHits->Fill(muSeg.nPrecisionHits());
  nPhiLayers->Fill(muSeg.nPhiLayers());
  nTrigEtaLayers->Fill(muSeg.nTrigEtaLayers());

  int chIndex = muSeg.chamberIndex();
  float chambernorm = 1/Chamberarea[chIndex];//weight of the segment using the chamber eta-phi area
  chamberIndex->Fill(chIndex);
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
  if ( !(muSeg.px()) || !(muSeg.py()) || !(muSeg.pz()) ) return;
  if ( (muSeg.px()==0) || (muSeg.py()==0) || (muSeg.pz()==0) ) return; 

  Amg::Vector3D globalDir(muSeg.px(),muSeg.py(),muSeg.pz());
  float eta = globalDir.eta();
  //if (globalDir.z() != 0 ) eta = atan2(globalDir.perp(), globalDir.z());//fix the global eta direction
  float phi = globalDir.phi();
  if (phi>myPi) phi-=2*myPi;
  etadir->Fill(eta);
  phidir->Fill(phi);
  etaphidir->Fill(eta,phi);

}


} // closing namespace Muon
