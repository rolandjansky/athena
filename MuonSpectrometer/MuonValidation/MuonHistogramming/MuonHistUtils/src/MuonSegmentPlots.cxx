/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonSegmentPlots.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "MuonIdHelpers/MuonStationIndex.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Muon{

MuonSegmentPlots::MuonSegmentPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir)
{  

  //booking histograms
  segmentfitChi2 = Book1D("segmentfitChi2", "Segment fit chi2;#chi^{2};Entries", 20,0,200);
  segmentfitNdof = Book1D("segmentfitNdof", "Segment fit Ndof;N_{dof};Entries", 20,0,20);
  segmentfitChi2oNdof = Book1D("segmentfitChi2oNdof", "Segment fit chi2/Ndof;Segment fit #chi^{2}/N_{dof};Entries", 20,0,20);
  
  t0 = Book1D("t0","t0;t_0;Entries",100,-50,50);
  t0err = Book1D("t0err","t0 error;t_0 error;Entries",100,0,20);

  sector = Book1D("sector","Segment phi sector;#phi sector;Entries",16,0.5,16.5);
  nPrecisionHits = Book1D("nPrecisionHits","Segment precision hits;hits;Entries",20,0,20);

  nPhiLayers = Book1D("nPhiLayers","Segment phi layers;#phi layers;Entries",10,0,10);
  nTrigEtaLayers = Book1D("nTrigEtaLayers","Segment eta trigger layers;#eta trigger layers;Entries",10,0,10);

  xypos_barrel = Book2D("xypos_barrel","Segment position x-y, barrel;x_{pos};y_{pos}",100,-12000,12000,100,-12000,12000);
  xypos_endcap = Book2D("xypos_endcap","Segment position x-y, endcap;x_{pos};y_{pos}",100,-12000,12000,100,-12000,12000);
  rzpos_sectorLarge = Book2D("rzpos_sectorLarge","Segment position r-z, large sectors;z_{pos};r_{pos}",100,-22000,22000,50,0,12000);
  rzpos_sectorSmall = Book2D("rzpos_sectorSmall","Segment position r-z, small sectors;z_{pos};r_{pos}",100,-22000,22000,50,0,12000);

  etadir = Book1D("etadir","Segment direction eta;#eta_{dir};Entries",100,-5,5);
  etadir_barrel = Book1D("etadir_barrel","Segment direction eta, barrel;#eta_{dir};Entries",100,-5,5);
  etadir_endcap = Book1D("etadir_endcap","Segment direction eta, endcap;#eta_{dir};Entries",100,-5,5);
  phidir = Book1D("phidir","Segment direction phi;#phi_{dir};Entries",64,-3.2,3.2);
  etaphidir = Book2D("etaphidir","Segment direction phi vs eta;#eta_{dir};#phi_{dir}",64,-3.2,3.2,64,-3.2,3.2);

  sector_perStation = Book2D("sector_perStation","Segment #phi sector per detector region/station;#phi sector;station",16,0.5,16.5,Muon::MuonStationIndex::StIndexMax,0,Muon::MuonStationIndex::StIndexMax);

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

  t0->Fill(muSeg.t0());
  t0err->Fill(muSeg.t0error());

  sector->Fill(muSeg.sector());
  nPrecisionHits->Fill(muSeg.nPrecisionHits());
  nPhiLayers->Fill(muSeg.nPhiLayers());
  nTrigEtaLayers->Fill(muSeg.nTrigEtaLayers());


  int chIndex = muSeg.chamberIndex();

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

  float myPi = 3.14159;

  Amg::Vector3D globalDir(muSeg.px(),muSeg.py(),muSeg.pz());
  float eta = globalDir.eta();
  float phi = globalDir.phi();
  if (phi>myPi) phi-=2*myPi;
  etadir->Fill(eta);
  phidir->Fill(phi);
  etaphidir->Fill(eta,phi);

  if (isBarrel) {
    xypos_barrel->Fill(muSeg.x(),muSeg.y());
    etadir_barrel->Fill(eta);
  } else {
    xypos_endcap->Fill(muSeg.x(),muSeg.y());
    etadir_endcap->Fill(eta);
  }
  if (isSectorLarge) {
    rzpos_sectorLarge->Fill(z,r);
  } else {
    rzpos_sectorSmall->Fill(z,r);
  }

}

} // closing namespace Muon
