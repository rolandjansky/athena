/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "Csc2dSegmentMaker.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "TrkSegment/Segment.h"
#include "TrkRoad/TrackRoad.h"
#include "MuonCondInterface/ICSCConditionsSvc.h"

#include <sstream>
#include <cmath>

using Muon::CscPrepDataContainer;
using Muon::CscPrepDataCollection;
using Muon::CscPrepData;
using Muon::CscStripPrepDataContainer;

using MuonGM::CscReadoutElement;
using Muon::CscClusterOnTrack;
using Muon::MuonSegment;
using Muon::MuonSegmentCombination;

using Muon::MuonClusterOnTrack;
using Muon::MdtDriftCircleOnTrack;

namespace {

std::string measphi_name(bool measphi) {
  if ( measphi ) return "phi";
  return "eta";
}

// Convert chamber identifier to string.
std::string chamber(int istation, int zsec, int phi) {
  std::ostringstream ssout;
  if ( istation == 1 ) ssout << "CSS";
  else if ( istation ==2 ) ssout << "CSL";
  else ssout << "???";
  if ( zsec == -1 ) ssout << "-";
  else if ( zsec == 1 ) ssout << "+";
  else ssout << "?";
  ssout << phi;
  return ssout.str();
}

}

//******************************************************************************

Csc2dSegmentMaker::Csc2dSegmentMaker(const std::string& type, const std::string& aname, const IInterface* parent) :
    AthAlgTool(type, aname, parent)
{
  declareInterface<ICscSegmentFinder>(this);
}

//******************************************************************************

StatusCode Csc2dSegmentMaker::initialize(){
  ATH_MSG_DEBUG ( "Initializing " << name() );
  // Show keys.
  ATH_MSG_DEBUG ( "  SegmentTool: " << m_segmentTool.typeAndName() );
  ATH_MSG_DEBUG ( "  Input cscdig key: " << m_cscdig_sg_inkey );
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_segmentTool.retrieve());
  ATH_CHECK(m_cscClusterOnTrackCreator.retrieve());
  ATH_CHECK(m_printer.retrieve());
  return StatusCode::SUCCESS;
}


//******************************************************************************
std::unique_ptr<MuonSegmentCombinationCollection> Csc2dSegmentMaker::find( const std::vector<const Muon::CscPrepDataCollection*>& pcols) const
{

  // Construct output segment collection.
  std::unique_ptr<MuonSegmentCombinationCollection> mpsegs(new MuonSegmentCombinationCollection);

  for ( unsigned int icol=0; icol <pcols.size(); ++icol) {
    const CscPrepDataCollection* clus = pcols[icol];
    if (clus->size() == 0)     continue; // skip zero cluster collection
    break;
  }

  unsigned int col_count = 0;
  // Loop over collections in the container.
  for ( unsigned int icol=0; icol <pcols.size(); ++icol) {
    const CscPrepDataCollection* clus = pcols[icol];
    ++col_count;
    ATH_MSG_VERBOSE ( "** Collection " << col_count << " has "
                      << clus->size() << " clusters" );

    if (clus->size() == 0)     continue; // skip zero cluster collection
    MuonSegmentCombination* pcol =findSegmentCombination(*clus);
    if (pcol) {
      mpsegs->push_back(pcol);
      ATH_MSG_DEBUG("Found 2d CSC segment " << m_printer->print( *pcol ));
    }
  }

  if(!mpsegs->empty()) ATH_MSG_DEBUG("found "<<mpsegs->size()<<" 2d csc segments");
  else mpsegs.reset();
      
  return mpsegs;
}


//******************************************************************************

MuonSegmentCombination* Csc2dSegmentMaker::findSegmentCombination(const CscPrepDataCollection& clus) const {

  // check whether input not empty
  if( clus.empty() ) return 0;

  int col_station = 0;
  int col_eta = 0;
  int col_phisec = 0;
  Identifier eta_id, phi_id;

  ChamberTrkClusters eta_clus, phi_clus;
  
  const MuonGM::CscReadoutElement* detEl = clus.front()->detectorElement();
  if( !detEl ){
    ATH_MSG_WARNING("Failed to obtain CscReadoutElement for cluster");
    return 0;
  }
  Amg::Transform3D gToLocal = detEl->GlobalToAmdbLRSTransform();
  Amg::Vector3D lpos000 = gToLocal*Amg::Vector3D(0.0, 0.0, 0.0);

  int stationEta=detEl->getStationEta();
  int stationPhi=detEl->getStationPhi();
  int chamberLayer=detEl->ChamberLayer();
  std::string stationName=detEl->getStationName();
  std::string redName=stationName.substr(0,3); //CSS or CSL, all that the CscIdHelper wants
  std::string isPhi;
  ATH_MSG_DEBUG("in station "<<stationName<<" eta "<<stationEta<<", phi "<<stationPhi<<", chamber layer "<<chamberLayer);
  //new method: we no longer assume automatically that eta and phi go together
  //this is the usual reason for problems due to broken wires, but it need not always be the case
  //instead, pass the status of every layer to the CscSegmentUtilTool
  //if there are only 2 layers for eta or phi, do 2-layer segment finding for eta or phi
  //if there are fewer than two good layers for eta or phi, so no eta or phi segment can be made, the final segment need not include information from the missing side

  int layStatus[2]={0,0}; //eta=0,phi=1; each decimal place will be 0 for good, 1 for bad, so 1001 means first and 4th layers are bad, etc.
  for(int iLay=0;iLay<4;iLay++){ //loop over the layers in this chamber
    for(int iPhi=0;iPhi<2;iPhi++){ //eta or phi
      int nbad=0;
      if(iPhi) isPhi="phi";
      else isPhi="eta";
      //ATH_MSG_DEBUG("get hashes for "<<detEl->maxNumberOfStrips(iPhi)<<" strips ");
      for(int iStrip=0;iStrip<detEl->maxNumberOfStrips(iPhi);iStrip++){
	ATH_MSG_DEBUG("get strip quality for "<<isPhi<<" layer "<<iLay<<" strip "<<iStrip);
	Identifier stripId=m_idHelperSvc->cscIdHelper().channelID(redName,stationEta,stationPhi,chamberLayer,iLay+1,iPhi,iStrip+1);
	IdentifierHash hashID;
	m_idHelperSvc->cscIdHelper().get_channel_hash(stripId,hashID);
	if(!m_segmentTool->isGood(hashID)){
	  ATH_MSG_DEBUG("bad strip");
	  nbad++;
	}
      }
      ATH_MSG_DEBUG("found "<<nbad<<" bad strips out of "<<detEl->maxNumberOfStrips(iPhi)<<" in "<<isPhi<<" layer "<<iLay);
      if(nbad==detEl->maxNumberOfStrips(iPhi)){
	layStatus[iPhi]+=pow(10,iLay);
      }
    }
  }
  ATH_MSG_DEBUG("final status for eta is "<<layStatus[0]<<" and phi "<<layStatus[1]);

  for ( CscPrepDataCollection::const_iterator iclu=clus.begin(); iclu!=clus.end(); ++iclu ) {
    const CscPrepData* pclu = *iclu;
    Identifier id = pclu->identify();
    int station = m_idHelperSvc->cscIdHelper().stationName(id) - 49;
    int eta = m_idHelperSvc->cscIdHelper().stationEta(id);
    int phisec = m_idHelperSvc->cscIdHelper().stationPhi(id);
    int iwlay = m_idHelperSvc->cscIdHelper().wireLayer(id);
    bool measphi = m_idHelperSvc->cscIdHelper().measuresPhi(id);
    
    if ( iclu == clus.begin() ) {
      col_station = station;
      col_eta = eta;
      col_phisec = phisec;
    }
    ATH_MSG_DEBUG ( "****** " << chamber(station,eta,phisec)
		    << measphi_name(measphi) << iwlay );
    
    if ( station!=col_station || eta!=col_eta || phisec!=col_phisec ) {
      ATH_MSG_WARNING ( "Inconsistent collection contents" );
      return 0;
    }

    const Muon::MuonClusterOnTrack* clu = m_cscClusterOnTrackCreator->createRIO_OnTrack(*pclu,pclu->globalPosition());
    const Muon::CscClusterOnTrack* ptclu = dynamic_cast<const Muon::CscClusterOnTrack*>(clu);
    if( !ptclu ){
      ATH_MSG_WARNING("Failed to create CscClusterOnTrack from PRD");
      delete clu;
      continue;
    }
    Amg::Vector3D lpos = gToLocal*ptclu->globalPosition();
    if ( measphi ) {
      phi_id = id;
      phi_clus[iwlay-1].push_back(Cluster(lpos,ptclu,measphi));
    } else {
      eta_id = id;
      eta_clus[iwlay-1].push_back(Cluster(lpos,ptclu,measphi));
    }
  }


  // Insert r-phi-segments.


  int nHitLayer_eta =0;
  int nHitLayer_phi =0;
  for (int i=0; i<4; ++i) {
    if (eta_clus[i].size() >0) ++nHitLayer_eta;
    if (phi_clus[i].size() >0) ++nHitLayer_phi;
  }
  ATH_MSG_DEBUG("there are "<<nHitLayer_eta<<" eta hit layers and "<<nHitLayer_phi<<" phi hit layers");

  //  MuonSegmentCombination* pcol;
  MuonSegmentCombination* pcol = 0;
  if (nHitLayer_eta >=2 || nHitLayer_phi >=2) {
    ATH_MSG_DEBUG( "Csc2dSegment calls get2dMuonSegmentCombination !!!" );
    pcol = m_segmentTool->get2dMuonSegmentCombination(eta_id, phi_id, eta_clus, phi_clus, lpos000, layStatus[0], layStatus[1]); 
  }
  
  // to avoid memory leak
  for ( unsigned int i=0; i<4; ++i) {
    for ( TrkClusters::iterator itclus =eta_clus[i].begin();
          itclus != eta_clus[i].end(); ++itclus ) {
      delete itclus->cl;
    }
    eta_clus[i].clear();
    for ( TrkClusters::iterator itclus =phi_clus[i].begin();
          itclus != phi_clus[i].end(); ++itclus ) {
      delete itclus->cl;
    }
    phi_clus[i].clear();
  }



  return pcol;
}
  
//******************************************************************************
std::unique_ptr<MuonSegmentCombinationCollection> Csc2dSegmentMaker::find( const MuonSegmentCombinationCollection& ) const
{
  return std::unique_ptr<MuonSegmentCombinationCollection>();

}
