/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonSegmentPlots.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "MuonIdHelpers/MuonStationIndex.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Muon{
  
MuonSegmentPlots::MuonSegmentPlots(PlotBase* pParent, std::string sDir): PlotBase(pParent, sDir)
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
  ,  nPrecisionHits_nTriggerHits(NULL)  

  ,  B_MDT_nPhiLayers_phi(NULL)
  ,  B_MDT_nPhiLayers_eta(NULL)
  ,  B_MDT_zeroPhiLayers_eta_phi(NULL)

  ,  etaIndex(NULL)
  ,  sector(NULL)
  ,  sector_perStation(NULL)
  
  ,  xypos_barrel(NULL)
  ,  xypos_endcap(NULL)
  ,  rzpos_sectorSmall(NULL)
  ,  rzpos_sectorSmall_splitY(NULL)
  ,  rzpos_sectorLarge(NULL)
  ,  rzpos_sectorLarge_splitY(NULL)
  
  ,  etadir(NULL)
  ,  etadir_barrel(NULL)
  ,  etadir_endcap(NULL)
  ,  phidir(NULL)
  ,  etaphidir(NULL)

								       , chamberIndex(NULL)
{  

  //booking histograms
  segmentfitChi2 = Book1D("segmentfitChi2", "Segment fit chi2;#chi^{2};Entries", 20,0,200);
  segmentfitNdof = Book1D("segmentfitNdof", "Segment fit Ndof;N_{dof};Entries", 20,0,20);
  segmentfitChi2oNdof = Book1D("segmentfitChi2oNdof", "Segment fit chi2/Ndof;Segment fit #chi^{2}/N_{dof};Entries", 20,0,20);
  
  t0 = Book1D("t0","t0;t_0;Entries",100,-50,50);
  t0_top = Book1D("t0_top","t0 for y>0;t_0;Entries",100,-50,50);
  t0_bottom = Book1D("t0_bottom","t0 for y<0;t_0;Entries",100,-50,50);
  t0err = Book1D("t0err","t0 error;t_0 error;Entries",100,0,20);
  t0err_top = Book1D("t0err_top","t0 error for y>0;t_0 error;Entries",100,0,20);
  t0err_bottom = Book1D("t0err_bottom","t0 error for y<0;t_0 error;Entries",100,0,20);

  nPrecisionHits = Book1D("nPrecisionHits","Segment precision hits;hits;Entries",20,0,20);
  nPhiLayers = Book1D("nPhiLayers","Segment phi layers;#phi layers;Entries",10,0,10);
  nTrigEtaLayers = Book1D("nTrigEtaLayers","Segment eta trigger layers;#eta trigger layers;Entries",10,0,10);
  nPrecisionHits_nTriggerHits = Book2D("nPrecisionHits_nTriggerHits", "Number of MDT hits vs Tigger station hits; MDT hits; Trigger hits", 20, -0.5, 19.5, 20, -0.5, 19.5);

  B_MDT_nPhiLayers_phi = Book2D("B_MDT_nPhiLayers_phi","(Barrel MDT) Segment phi layers per phi sector;phi sector;#phi Layers",16,0.5,16.5,10,0,10);
  B_MDT_nPhiLayers_eta = Book2D("B_MDT_nPhiLayers_eta","(Barrel BDT) Segment phi layers per eta index;eta index;#phi Layers",17,-8.5,8.5,10,0,10);
  B_MDT_zeroPhiLayers_eta_phi = Book2D("B_MDT_zeroPhiLayers_eta_phi","(Barrel MDT) phi sector & eta station without phi layers;#eta index;#phi sector",17,-8.5,8.5,16,0.5,16.5);
  B_MDT_withPhiLayers_eta_phi = Book2D("B_MDT_withPhiLayers_eta_phi","(Barrel MDT) phi sector & eta station with >0 phi layers;#eta index;#phi sector",17,-8.5,8.5,16,0.5,16.5);

  etaIndex = Book1D("etaIndex","Segment #eta Index ;#eta index;Entries",21,-10.5,10.5);
  sector = Book1D("sector","Segment phi sector;#phi sector;Entries",16,0.5,16.5);
  sector_perStation = Book2D("sector_perStation","Segment #phi sector per station (I/M/O);#phi sector;station",16,0.5,16.5,Muon::MuonStationIndex::StIndexMax,0,Muon::MuonStationIndex::StIndexMax);

  xypos_barrel = Book2D("xypos_barrel","Segment position x-y, barrel;x_{pos};y_{pos}",150,-18000,18000,150,-18000,18000);
  xypos_endcap = Book2D("xypos_endcap","Segment position x-y, endcap;x_{pos};y_{pos}",150,-18000,18000,150,-18000,18000);
  rzpos_sectorLarge = Book2D("rzpos_sectorLarge","Segment position r-z, large sectors;z_{pos};r_{pos}",100,-22000,22000,75,0,18000);
  rzpos_sectorLarge_splitY = Book2D("rzpos_sectorLarge_splitY","Segment position r-z, split top/bottom, large sectors;z_{pos};sgn(y_{pos})r_{pos}",100,-22000,22000,150,-18000,18000);
  rzpos_sectorSmall = Book2D("rzpos_sectorSmall","Segment position r-z, small sectors;z_{pos};r_{pos}",100,-22000,22000,75,0,18000);
  rzpos_sectorSmall_splitY = Book2D("rzpos_sectorSmall_splitY","Segment position r-z, split top/bottom, small sectors;z_{pos};sgn(y_{pos})*r_{pos}",100,-22000,22000,150,-18000,18000);

  etadir = Book1D("etadir","Segment direction eta;#eta_{dir};Entries",100,-5,5);
  etadir_barrel = Book1D("etadir_barrel","Segment direction eta, barrel;#eta_{dir};Entries",100,-5,5);
  etadir_endcap = Book1D("etadir_endcap","Segment direction eta, endcap;#eta_{dir};Entries",100,-5,5);
  phidir = Book1D("phidir","Segment direction phi;#phi_{dir};Entries",64,-3.2,3.2);
  etaphidir = Book2D("etaphidir","Segment direction phi vs eta;#eta_{dir};#phi_{dir}",64,-3.2,3.2,64,-3.2,3.2);

  chamberIndex = Book1D("chamberIndex","Chamber index",Muon::MuonStationIndex::ChIndexMax,0,Muon::MuonStationIndex::ChIndexMax);
  for (int i=1; i<=chamberIndex->GetXaxis()->GetNbins(); i++) chamberIndex->GetXaxis()->SetBinLabel(i,Muon::MuonStationIndex::chName((Muon::MuonStationIndex::ChIndex)chamberIndex->GetBinLowEdge(i)).c_str());
}

MuonSegmentPlots::~MuonSegmentPlots() 
{
}

void MuonSegmentPlots::fill(const xAOD::MuonSegment& muSeg)
{
  float chi2 = muSeg.chiSquared();
  float ndof = muSeg.numberDoF();
  segmentfitChi2->Fill(chi2);
  segmentfitNdof->Fill(ndof);
  if (ndof>0) segmentfitChi2oNdof->Fill(muSeg.chiSquared()/muSeg.numberDoF());

  float x=muSeg.x();
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
  etaIndex->Fill(muSeg.etaIndex());


  nPrecisionHits->Fill(muSeg.nPrecisionHits());
  nPhiLayers->Fill(muSeg.nPhiLayers());
  nTrigEtaLayers->Fill(muSeg.nTrigEtaLayers());
  nPrecisionHits_nTriggerHits->Fill(muSeg.nPrecisionHits(), muSeg.nPhiLayers() + muSeg.nTrigEtaLayers()); ///@@@!!! phi hits not trigger hits (CSC?)

  if (muSeg.technology()==Muon::MuonStationIndex::MDT && (muSeg.chamberIndex()>=Muon::MuonStationIndex::BIS && muSeg.chamberIndex()<=Muon::MuonStationIndex::BEE)){
    B_MDT_nPhiLayers_phi->Fill(muSeg.sector(),muSeg.nPhiLayers());
    B_MDT_nPhiLayers_eta->Fill(muSeg.etaIndex(),muSeg.nPhiLayers());
    if (muSeg.nPhiLayers()==0) B_MDT_zeroPhiLayers_eta_phi->Fill(muSeg.etaIndex(),muSeg.sector());
    else B_MDT_withPhiLayers_eta_phi->Fill(muSeg.etaIndex(),muSeg.sector());
  }

  int chIndex = muSeg.chamberIndex();
  chamberIndex->Fill(chIndex);
  
  bool isBarrel = (chIndex<Muon::MuonStationIndex::BEE)? true: false; // BEE -> endcap
  bool isSectorLarge = ( (isBarrel && chIndex%2==1) || (!isBarrel && chIndex%2==0 && chIndex!=Muon::MuonStationIndex::BEE) )? true : false; ////BEE only in small sectors

  if (isBarrel) {
    if (chIndex==Muon::MuonStationIndex::BIL || chIndex==Muon::MuonStationIndex::BIS) 
      sector_perStation->Fill(muSeg.sector(),Muon::MuonStationIndex::BI);  
    else if (chIndex==Muon::MuonStationIndex::BML || chIndex==Muon::MuonStationIndex::BMS)
      sector_perStation->Fill(muSeg.sector(),Muon::MuonStationIndex::BM);
    else if (chIndex==Muon::MuonStationIndex::BOL || chIndex==Muon::MuonStationIndex::BOS)
      sector_perStation->Fill(muSeg.sector(),Muon::MuonStationIndex::BO);
  }
  else { //endcap
    if (chIndex==Muon::MuonStationIndex::EIL || chIndex==Muon::MuonStationIndex::EIS) 
      sector_perStation->Fill(muSeg.sector(),Muon::MuonStationIndex::EI);  
    else if (chIndex==Muon::MuonStationIndex::EML || chIndex==Muon::MuonStationIndex::EMS) 
      sector_perStation->Fill(muSeg.sector(),Muon::MuonStationIndex::EM);
    else if (chIndex==Muon::MuonStationIndex::EOL || chIndex==Muon::MuonStationIndex::EOS)
      sector_perStation->Fill(muSeg.sector(),Muon::MuonStationIndex::EO);
    else if (chIndex==Muon::MuonStationIndex::EEL || chIndex==Muon::MuonStationIndex::EES)
      sector_perStation->Fill(muSeg.sector(),Muon::MuonStationIndex::EE);
    else if (chIndex==Muon::MuonStationIndex::BEE) //BEE only in small sectors
      sector_perStation->Fill(muSeg.sector(),Muon::MuonStationIndex::BE);
  }


  //position and direction plots

  //protect against cases with no hit information!
  if ( !(muSeg.px()) || !(muSeg.py()) || !(muSeg.pz()) ) return;
  if ( (muSeg.px()==0) || (muSeg.py()==0) || (muSeg.pz()==0) ) return; 

  Amg::Vector3D globalPos(muSeg.x(),muSeg.y(),muSeg.z());
  float r = globalPos.perp();
  float z = globalPos.z();

  Amg::Vector3D globalDir(muSeg.px(),muSeg.py(),muSeg.pz());
  float eta = globalDir.eta();
  float phi = globalDir.phi();
  if (phi>myPi) phi-=2*myPi;
  etadir->Fill(eta);
  phidir->Fill(phi);
  etaphidir->Fill(eta,phi);

  if (isBarrel) {
    xypos_barrel->Fill(x,y);
    etadir_barrel->Fill(eta);
  } else {
    xypos_endcap->Fill(x,y);
    etadir_endcap->Fill(eta);
  }
  if (isSectorLarge) {
    rzpos_sectorLarge->Fill(z,r);
    rzpos_sectorLarge_splitY->Fill(z,r*y/fabs(y));
  } else {
    rzpos_sectorSmall->Fill(z,r);
    rzpos_sectorSmall_splitY->Fill(z,r*y/fabs(y));
  }

}


// bool MuonSegmentPlots::goodSegmentQuality(float chi2, int nhits, int TechIndex, int station)
// {
//   // CSC seem to have tight internal constraints anyway(really?!)...0 is MDT
//   if ( TechIndex > 0 ) {return true;}

//   int mdtNhitsMin   = m_bMdtnhitsmin;
//   float mdtChi2Max  = m_bMdtchi2max;

//   if (station >= 4 ) {
//     mdtNhitsMin = m_eMdtnhitsmin;
//     mdtChi2Max  = m_eMdtchi2max;
//   }
//   // BI and EI station has 8 layers instead of 6.
//   // Adjust accordingly, add one more hit/hole
//   if ( station == 4 || station == 0) {
//     mdtNhitsMin += 1;   
//   }
//   // Cuts for case where dealing with MDT
//   // Otherwise, dealing with CSC
//   if ( chi2   > mdtChi2Max )  return false;
//   if ( nhits  < mdtNhitsMin ) return false;
  
//   return true;
// }


} // closing namespace Muon
