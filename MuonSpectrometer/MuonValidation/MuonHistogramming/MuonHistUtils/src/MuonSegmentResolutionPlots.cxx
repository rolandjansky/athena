/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonSegmentResolutionPlots.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "MuonIdHelpers/MuonStationIndex.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Muon{

MuonSegmentResolutionPlots::MuonSegmentResolutionPlots(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir)
{  

  //booking profile histograms

  deltaPosX = Book1D("deltaPosX","Resolution x-position;#Delta x_{pos};Entries",100,-1000,1000);
  deltaPosY = Book1D("deltaPosY","Resolution y-position;#Delta y_{pos};Entries",100,-1000,1000);
  deltaPosZ = Book1D("deltaPosZ","Resolution z-position;#Delta z_{pos};Entries",100,-1000,1000);

  deltaPosEta = Book1D("deltaPosEta","Resolution eta-position;#Delta #eta_{pos};Entries",64,-3.2,3.2);
  deltaPosPhi = Book1D("deltaPosPhi","Resolution phi-position;#Delta #phi_{pos};Entries",64,-3.2,3.2);

  deltaDirPhi = Book1D("deltaDirPhi","Resolution phi-direction;#Delta #phi_{dir};Entries",100,500,500);
  deltaDirEta = Book1D("deltaDirEta","Resolution eta-direction;#Delta #eta_{dir};Entries",100,500,500);

  // for( unsigned int i=0; i<Muon::MuonStationIndex::StIndexMax; ++i ) {
  //   std::string stationName = Muon::MuonStationIndex::stName((Muon::MuonStationIndex::StIndex)i);
  //   sector_perStation.push_back( Book1D("sector_"+stationName,"Segment phi sector in "+stationName+";#phi sector;Entries",16,0.5,16.5) );
  // }

}

MuonSegmentResolutionPlots::~MuonSegmentResolutionPlots() 
{
}

void MuonSegmentResolutionPlots::fill(const xAOD::MuonSegment& muSeg,const xAOD::MuonSegment& truthMuSeg)
{

  //protect against cases with no hit information!
  if ( !(muSeg.px()) || !(muSeg.py()) || !(muSeg.pz()) ) return;
  if ( (muSeg.px()==0) || (muSeg.py()==0) || (muSeg.pz()==0) ) return; 

  if ( !(truthMuSeg.px()) || !(truthMuSeg.py()) || !(truthMuSeg.pz()) ) return;
  if ( (truthMuSeg.px()==0) || (truthMuSeg.py()==0) || (truthMuSeg.pz()==0) ) return; 


  deltaPosX->Fill(muSeg.x()-truthMuSeg.x());
  deltaPosY->Fill(muSeg.y()-truthMuSeg.y());
  deltaPosZ->Fill(muSeg.z()-truthMuSeg.z());

  Amg::Vector3D globalPos(muSeg.x(),muSeg.y(),muSeg.z());
  Amg::Vector3D truthGlobalPos(truthMuSeg.x(),truthMuSeg.y(),truthMuSeg.z());

  deltaPosEta->Fill(globalPos.eta());
  deltaPosPhi->Fill(globalPos.phi());

  // std::cout << "positions and directions: " << std::endl;
  // std::cout << muSeg.x() << " " << muSeg.y() << " " << muSeg.z() << std::endl;
  // std::cout << muSeg.px() << " " << muSeg.py() << " " << muSeg.pz() << std::endl;   
  // std::cout << truthMuSeg.x() << " " << truthMuSeg.y() << " " << truthMuSeg.z() << std::endl;
  // std::cout << truthMuSeg.px() << " " << truthMuSeg.py() << " " << truthMuSeg.pz() << std::endl;   

  Amg::Vector3D globalDir(muSeg.px(),muSeg.py(),muSeg.pz());
  Amg::Vector3D truthGlobalDir(truthMuSeg.px(),truthMuSeg.py(),truthMuSeg.pz());

  deltaDirPhi->Fill(globalDir.phi()-truthGlobalDir.phi());
  deltaDirEta->Fill(globalDir.eta()-truthGlobalDir.eta());

  // int chIndex = muSeg.chamberIndex();
  // int stIndex = -1;
  // if (chIndex==Muon::MuonStationIndex::BIS || chIndex==Muon::MuonStationIndex::BIL) stIndex  = Muon::MuonStationIndex::BI;
  // else if (chIndex==Muon::MuonStationIndex::BMS || chIndex==Muon::MuonStationIndex::BML) stIndex  = Muon::MuonStationIndex::BM;
  // else if (chIndex==Muon::MuonStationIndex::BOS || chIndex==Muon::MuonStationIndex::BOL) stIndex  = Muon::MuonStationIndex::BO;
  // else if (chIndex==Muon::MuonStationIndex::BEE) stIndex  = Muon::MuonStationIndex::BE;
  // else if (chIndex==Muon::MuonStationIndex::EIS || chIndex==Muon::MuonStationIndex::EIL ) stIndex  = Muon::MuonStationIndex::EI;
  // else if (chIndex==Muon::MuonStationIndex::EMS || chIndex==Muon::MuonStationIndex::EML ) stIndex  = Muon::MuonStationIndex::EM;
  // else if (chIndex==Muon::MuonStationIndex::EOS || chIndex==Muon::MuonStationIndex::EOL ) stIndex  = Muon::MuonStationIndex::EO;
  // else if (chIndex==Muon::MuonStationIndex::EES || chIndex==Muon::MuonStationIndex::EEL ) stIndex  = Muon::MuonStationIndex::EE;
  // //else if (chIndex==Muon::MuonStationIndex::CSS || chIndex==Muon::MuonStationIndex::CSL ) stIndex  = Muon::MuonStationIndex::CS;
  // if (stIndex>=0) sector_perStation[stIndex]->Fill(muSeg.sector());
}

} // closing namespace Muon
