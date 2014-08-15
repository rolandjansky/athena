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

  //booking profile histograms
  segmentfitChi2 = Book1D("segmentfitChi2", "Segment fit chi2;#chi^{2};Entries", 20,0,200);
  segmentfitNdof = Book1D("segmentfitNdof", "Segment fit Ndof;N_{dof};Entries", 20,0,20);
  segmentfitChi2oNdof = Book1D("segmentfitChi2oNdof", "Segment fit chi2/Ndof;Segment fit #chi^{2}/N_{dof};Entries", 20,0,20);
  
  t0 = Book1D("t0","t0;t_0;Entries",100,-50,50);
  t0err = Book1D("t0err","t0 error;t_0 error;Entries",100,0,20);

  sector = Book1D("sector","Segment phi sector;#phi sector;Entries",16,0.5,16.5);
  nPrecisionHits = Book1D("nPrecisionHits","Segment precision hits;hits;Entries",20,0,20);

  nPhiLayers = Book1D("nPhiLayers","Segment phi layers;#phi layers;Entries",20,0,20);
  nTrigEtaLayers = Book1D("nTrigEtaLayers","Segment eta trigger layers;#eta trigger layers;Entries",20,0,20);

  positionXY = Book2D("positionXY","position x-y;position x;position y",100,-12000,12000,100,-12000,12000);
  positionRZ = Book2D("positionRZ","position r-z;position z;position r",100,-22000,22000,50,0,12000);

  directionEta = Book1D("directionEta","Segment direction eta;#eta direction;Entries",64,-3.2,3.2);
  directionPhi = Book1D("directionPhi","Segment direction phi;#phi direction;Entries",64,-3.2,3.2);
  directionPhiVsEta = Book2D("directionPhiVsEta","Segment direction phi vs eta;#eta direction;#phi direction",64,-3.2,3.2,64,-3.2,3.2);

  for( unsigned int i=0; i<Muon::MuonStationIndex::StIndexMax; ++i ) {
    std::string stationName = Muon::MuonStationIndex::stName((Muon::MuonStationIndex::StIndex)i);
    sector_perStation.push_back( Book1D("sector_"+stationName,"Segment phi sector in "+stationName+";#phi sector;Entries",16,0.5,16.5) );
  }

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
  int stIndex = -1;
  if (chIndex==Muon::MuonStationIndex::BIS || chIndex==Muon::MuonStationIndex::BIL) stIndex  = Muon::MuonStationIndex::BI;
  else if (chIndex==Muon::MuonStationIndex::BMS || chIndex==Muon::MuonStationIndex::BML) stIndex  = Muon::MuonStationIndex::BM;
  else if (chIndex==Muon::MuonStationIndex::BOS || chIndex==Muon::MuonStationIndex::BOL) stIndex  = Muon::MuonStationIndex::BO;
  else if (chIndex==Muon::MuonStationIndex::BEE) stIndex  = Muon::MuonStationIndex::BE;
  else if (chIndex==Muon::MuonStationIndex::EIS || chIndex==Muon::MuonStationIndex::EIL ) stIndex  = Muon::MuonStationIndex::EI;
  else if (chIndex==Muon::MuonStationIndex::EMS || chIndex==Muon::MuonStationIndex::EML ) stIndex  = Muon::MuonStationIndex::EM;
  else if (chIndex==Muon::MuonStationIndex::EOS || chIndex==Muon::MuonStationIndex::EOL ) stIndex  = Muon::MuonStationIndex::EO;
  else if (chIndex==Muon::MuonStationIndex::EES || chIndex==Muon::MuonStationIndex::EEL ) stIndex  = Muon::MuonStationIndex::EE;
  //else if (chIndex==Muon::MuonStationIndex::CSS || chIndex==Muon::MuonStationIndex::CSL ) stIndex  = Muon::MuonStationIndex::CS;
  if (stIndex>=0) sector_perStation[stIndex]->Fill(muSeg.sector());


  //position and direction plots

  //protect against cases with no hit information!
  if ( !(muSeg.px()) || !(muSeg.py()) || !(muSeg.pz()) ) return;
  if ( (muSeg.px()==0) || (muSeg.py()==0) || (muSeg.pz()==0) ) return; 

  Amg::Vector3D globalPos(muSeg.x(),muSeg.y(),muSeg.z());
  float r = globalPos.perp();
  float z = globalPos.z();
  positionXY->Fill(muSeg.x(),muSeg.y());
  positionRZ->Fill(z,r);

  Amg::Vector3D globalDir(muSeg.px(),muSeg.py(),muSeg.pz());
  float eta = globalDir.eta();
  float phi = globalDir.phi();
  directionEta->Fill(eta);
  directionPhi->Fill(phi);
  directionPhiVsEta->Fill(eta,phi);

}

} // closing namespace Muon
