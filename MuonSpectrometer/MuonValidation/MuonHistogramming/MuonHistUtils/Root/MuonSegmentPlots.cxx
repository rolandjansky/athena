/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonSegmentPlots.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "MuonStationIndex/MuonStationIndex.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include <cmath>

namespace Muon{
  
MuonSegmentPlots::MuonSegmentPlots(PlotBase* pParent, const std::string& sDir): PlotBase(pParent, sDir)
  ,  segmentfitChi2(nullptr)
  ,  segmentfitNdof(nullptr)
  ,  segmentfitChi2oNdof(nullptr)

  ,  t0(nullptr)
  ,  t0_top(nullptr)
  ,  t0_bottom(nullptr)
  ,  t0err(nullptr)
  ,  t0err_top(nullptr)
  ,  t0err_bottom(nullptr)

  ,  nPrecisionHits(nullptr)
  ,  nPhiLayers(nullptr)
  ,  nTrigEtaLayers(nullptr)
  ,  nPrecisionHits_nTriggerHits(nullptr)  

  // ,  B_MDT_nPhiLayers_phi(NULL)
  // ,  B_MDT_nPhiLayers_eta(NULL)
  // ,  B_MDT_zeroPhiLayers_eta_phi(NULL)
  // ,  B_MDT_withPhiLayers_eta_phi(NULL)

  ,  etaIndex(nullptr)
  ,  sector(nullptr)
  //,  sector_perStation(NULL)
  
  ,  xypos_barrel(nullptr)
  ,  xypos_endcap(nullptr)
  ,  rzpos_sectorSmall(nullptr)
  //,  rzpos_sectorSmall_splitY(NULL)
  ,  rzpos_sectorLarge(nullptr)
  //,  rzpos_sectorLarge_splitY(NULL)
  
  ,  etadir(nullptr)
  ,  etadir_barrel(nullptr)
  ,  etadir_endcap(nullptr)
  ,  phidir(nullptr)
  ,  etaphidir(nullptr)

	, chamberIndex(nullptr)
  , chamberIndex_perSector(nullptr)

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
  nPrecisionHits_nTriggerHits = Book2D("nPrecisionHits_nTriggerHits", "Number of MDT hits vs Tigger station hits; MDT hits; Trigger hits", 20, -0.5, 19.5, 20, -0.5, 19.5);

  //B_MDT_nPhiLayers_phi = Book2D("B_MDT_nPhiLayers_phi","(Barrel MDT) Segment phi layers per phi sector;phi sector;#phi Layers",16,0.5,16.5,10,0,5);
  //B_MDT_nPhiLayers_eta = Book2D("B_MDT_nPhiLayers_eta","(Barrel BDT) Segment phi layers per eta index;eta index;#phi Layers",17,-8.5,8.5,10,0,5);
  //B_MDT_zeroPhiLayers_eta_phi = Book2D("B_MDT_zeroPhiLayers_eta_phi","(Barrel MDT) phi sector & eta station without phi layers;#eta index;#phi sector",17,-8.5,8.5,16,0.5,16.5);
  //B_MDT_withPhiLayers_eta_phi = Book2D("B_MDT_withPhiLayers_eta_phi","(Barrel MDT) phi sector & eta station with >0 phi layers;#eta index;#phi sector",17,-8.5,8.5,16,0.5,16.5);

  etaIndex = Book1D("etaIndex","Segment #eta Index ;#eta index;Entries",21,-10.5,10.5);
  sector = Book1D("sector","Segment phi sector;#phi sector;Entries",16,0.5,16.5);
  //sector_perStation = Book2D("sector_perStation","Segment #phi sector per station (I/M/O);#phi sector;station",16,0.5,16.5,Muon::MuonStationIndex::StIndexMax,0,Muon::MuonStationIndex::StIndexMax);

  xypos_barrel = Book2D("xypos_barrel","Segment position x-y, barrel;x_{pos};y_{pos}",150,-14000,14000,150,-14000,14000);
  xypos_endcap = Book2D("xypos_endcap","Segment position x-y, endcap;x_{pos};y_{pos}",150,-14000,14000,150,-14000,14000);
  rzpos_sectorLarge = Book2D("rzpos_sectorLarge","Segment position r-z, large sectors normalized by solid angle;z_{pos};r_{pos}",100,-23000,23000,75,0,14000);
  //rzpos_sectorLarge_splitY = Book2D("rzpos_sectorLarge_splitY","Segment position r-z, split top/bottom, large sectors;z_{pos};sgn(y_{pos})r_{pos}",100,-23000,23000,150,-14000,14000);
  rzpos_sectorSmall = Book2D("rzpos_sectorSmall","Segment position r-z, small sectors normalized by solid angle;z_{pos};r_{pos}",100,-23000,23000,75,0,14000);
  //rzpos_sectorSmall_splitY = Book2D("rzpos_sectorSmall_splitY","Segment position r-z, split top/bottom, small sectors;z_{pos};sgn(y_{pos})*r_{pos}",100,-23000,23000,150,-14000,14000);

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

    sector_etaIndex.push_back(Book2D(Form("%s_etastation", temp_chambername), Form("Number of Segment in %s; #phi Sector; #eta Index", temp_chambername), 18, -0.5, 17.5, 19, -9.5, 9.5));
    sector_etaIndex_nPrechit.push_back(Book2D(Form("%s_etastation_nPrechit", temp_chambername), Form("Number of Segment Prec hit in %s; #phi Sector; #eta Index", temp_chambername), 18, -0.5, 17.5, 19, -9.5, 9.5));
    sector_etaIndex_nTrighit.push_back(Book2D(Form("%s_etastation_nTrighit", temp_chambername), Form("Number of Segment Phi + Trigeta hit in %s; #phi Sector; #eta Index", temp_chambername), 18, -0.5, 17.5, 19, -9.5, 9.5));
    eff_sector_etaIndex_nPrechit.push_back(Book2D(Form("eff_%s_etastation_nPrechit", temp_chambername), Form("Segment Prec hit eff in %s; #phi Sector; #eta Index", temp_chambername), 18, -0.5, 17.5, 19, -9.5, 9.5));
    eff_sector_etaIndex_nTrighit.push_back(Book2D(Form("eff_%s_etastation_nTrighit", temp_chambername), Form("Segment Phi + Trigeta hit eff in %s; #phi Sector; #eta Index", temp_chambername), 18, -0.5, 17.5, 19, -9.5, 9.5));
  }
}

MuonSegmentPlots::~MuonSegmentPlots() 
{
}

  void MuonSegmentPlots::fill(const xAOD::MuonSegment& muSeg, float weight)
{
  float chi2 = muSeg.chiSquared();
  float ndof = muSeg.numberDoF();
  segmentfitChi2->Fill(chi2,weight);
  segmentfitNdof->Fill(ndof,weight);
  if (ndof>0) segmentfitChi2oNdof->Fill(muSeg.chiSquared()/muSeg.numberDoF(), weight);

  float x=muSeg.x();
  float y=muSeg.y();

  float segt0 = muSeg.t0();
  float segt0err = muSeg.t0error();

  t0->Fill(segt0,weight);
  t0err->Fill(segt0err,weight);
  if (y>0) { 
    t0_top->Fill(segt0,weight);
    t0err_top->Fill(segt0err,weight);
  } else {
    t0_bottom->Fill(segt0,weight);
    t0err_bottom->Fill(segt0err,weight);
  }

  sector->Fill(muSeg.sector(),weight);
  etaIndex->Fill(muSeg.etaIndex(),weight);

  nPrecisionHits->Fill(muSeg.nPrecisionHits(),weight);
  nPhiLayers->Fill(muSeg.nPhiLayers(),weight);
  nTrigEtaLayers->Fill(muSeg.nTrigEtaLayers(),weight);
  // not sure how to implement weights here JEF 8/4/2021
  nPrecisionHits_nTriggerHits->Fill(muSeg.nPrecisionHits(), muSeg.nPhiLayers() + muSeg.nTrigEtaLayers(),weight); ///@@@!!! phi hits not trigger hits (CSC?)

  // if (muSeg.technology()==Muon::MuonStationIndex::MDT && (muSeg.chamberIndex()>=Muon::MuonStationIndex::BIS && muSeg.chamberIndex()<=Muon::MuonStationIndex::BEE)){
  //   B_MDT_nPhiLayers_phi->Fill(muSeg.sector(),muSeg.nPhiLayers());
  //   B_MDT_nPhiLayers_eta->Fill(muSeg.etaIndex(),muSeg.nPhiLayers());
  //   if (muSeg.nPhiLayers()==0) B_MDT_zeroPhiLayers_eta_phi->Fill(muSeg.etaIndex(),muSeg.sector());
  //   else B_MDT_withPhiLayers_eta_phi->Fill(muSeg.etaIndex(),muSeg.sector());
  // }

  // not sure how to implement weights here for these chamber/sector Index plots JEF 8/4/2021
  int chIndex = muSeg.chamberIndex();
  float chambernorm = 1/Chamberarea[chIndex];//weight of the segment using the chamber eta-phi area
  chamberIndex->Fill(chIndex,weight);
  int sectorIndex = muSeg.sector();
  int etaIndex = muSeg.etaIndex();
  //fill the count of segments; switch the sign here to make the plots
  if (muSeg.z() < 0) { sectorIndex = - sectorIndex;}
  chamberIndex_perSector->Fill(sectorIndex, chIndex, chambernorm*weight);
  eff_chamberIndex_perSector_numerator->Fill(sectorIndex, chIndex, weight*((muSeg.nPrecisionHits() > Chamberexpectedhits[chIndex]) ? Chamberexpectedhits[chIndex]:muSeg.nPrecisionHits()));
  eff_chamberIndex_perSector_denominator->Fill(sectorIndex, chIndex, weight*Chamberexpectedhits[chIndex]);
  //update sector eta index plots; switch the sign back here to make the plots
  sectorIndex = muSeg.sector(); 
  sector_etaIndex[chIndex]->Fill(sectorIndex, etaIndex);//for weighted average
  //double currentfill = sector_etaIndex[chIndex]->GetBinContent(sector_etaIndex[chIndex]->GetXaxis()->FindBin(sectorIndex), sector_etaIndex[chIndex]->GetYaxis()->FindBin(etaIndex));
  sector_etaIndex_nPrechit[chIndex]->Fill(sectorIndex, etaIndex, weight*muSeg.nPrecisionHits());
  sector_etaIndex_nTrighit[chIndex]->Fill(sectorIndex, etaIndex, weight*(muSeg.nPhiLayers() + muSeg.nTrigEtaLayers()));
  eff_sector_etaIndex_nPrechit[chIndex]->Fill(sectorIndex, etaIndex, weight*Chamberexpectedhits[chIndex]);
  eff_sector_etaIndex_nTrighit[chIndex]->Fill(sectorIndex, etaIndex, weight*Chamberexpectedtrighits[chIndex]);
  
  bool isBarrel = (chIndex<Muon::MuonStationIndex::BEE)? true: false; // BEE -> endcap
  bool isSectorLarge = ( (isBarrel && chIndex%2==1) || (!isBarrel && chIndex%2==0 && chIndex!=Muon::MuonStationIndex::BEE) )? true : false; ////BEE only in small sectors

  // if (isBarrel) {
  //   if (chIndex==Muon::MuonStationIndex::BIL || chIndex==Muon::MuonStationIndex::BIS) 
  //     sector_perStation->Fill(muSeg.sector(),Muon::MuonStationIndex::BI);  
  //   else if (chIndex==Muon::MuonStationIndex::BML || chIndex==Muon::MuonStationIndex::BMS)
  //     sector_perStation->Fill(muSeg.sector(),Muon::MuonStationIndex::BM);
  //   else if (chIndex==Muon::MuonStationIndex::BOL || chIndex==Muon::MuonStationIndex::BOS)
  //     sector_perStation->Fill(muSeg.sector(),Muon::MuonStationIndex::BO);
  // }
  // else { //endcap
  //   if (chIndex==Muon::MuonStationIndex::EIL || chIndex==Muon::MuonStationIndex::EIS) 
  //     sector_perStation->Fill(muSeg.sector(),Muon::MuonStationIndex::EI);  
  //   else if (chIndex==Muon::MuonStationIndex::EML || chIndex==Muon::MuonStationIndex::EMS) 
  //     sector_perStation->Fill(muSeg.sector(),Muon::MuonStationIndex::EM);
  //   else if (chIndex==Muon::MuonStationIndex::EOL || chIndex==Muon::MuonStationIndex::EOS)
  //     sector_perStation->Fill(muSeg.sector(),Muon::MuonStationIndex::EO);
  //   else if (chIndex==Muon::MuonStationIndex::EEL || chIndex==Muon::MuonStationIndex::EES)
  //     sector_perStation->Fill(muSeg.sector(),Muon::MuonStationIndex::EE);
  //   else if (chIndex==Muon::MuonStationIndex::BEE) //BEE only in small sectors
  //     sector_perStation->Fill(muSeg.sector(),Muon::MuonStationIndex::BE);
  // }


  //position and direction plots

  //protect against cases with no hit information!
  if ( !(muSeg.px()) || !(muSeg.py()) || !(muSeg.pz()) ) return;
  if ( (muSeg.px()==0) || (muSeg.py()==0) || (muSeg.pz()==0) ) return; 
  //set up direction vectors
  Amg::Vector3D globalPos(muSeg.x(),muSeg.y(),muSeg.z());
  float r = globalPos.perp();
  float z = globalPos.z();
  //fill the rz plots
  if (isSectorLarge) {rzpos_sectorLarge->Fill(z,r, chambernorm*weight);}
  else {rzpos_sectorSmall->Fill(z,r, chambernorm*weight);}

  Amg::Vector3D globalDir(muSeg.px(),muSeg.py(),muSeg.pz());
  float eta = globalDir.eta();
  //if (globalDir.z() != 0 ) eta = atan2(globalDir.perp(), globalDir.z());//fix the global eta direction
  float phi = globalDir.phi();
  if (phi>M_PI) phi-=2*M_PI;
  etadir->Fill(eta,weight);
  phidir->Fill(phi,weight);
  etaphidir->Fill(eta,phi);

  
  if (isBarrel) {
    xypos_barrel->Fill(x,y, chambernorm*weight);
    etadir_barrel->Fill(eta,weight);
  } else {
    xypos_endcap->Fill(x,y, chambernorm*weight);
    etadir_endcap->Fill(eta,weight);
  }

}

} // closing namespace Muon
