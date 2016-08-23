/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Csc2dSegmentMaker.h"
#include "CscSegmentMakers/ICscSegmentUtilTool.h"
#include <sstream>

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"

#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"

#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"

#include "TrkSegment/Segment.h"
#include "TrkRoad/TrackRoad.h"

#include "MuonRecHelperTools/MuonEDMPrinterTool.h" 

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

/*std::string station_name(int station) {
  if ( station == 1 ) return "CSS";
  if ( station == 2 ) return "CSL";
  return "UNKNOWN_STATION";
}*/

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

Csc2dSegmentMaker::
Csc2dSegmentMaker(const std::string& type, const std::string& aname, const IInterface* parent)
  : AthAlgTool(type, aname, parent),
    m_dumpcount(0), m_dumped(0), m_dump(false), m_pgm(0), m_phelper(0),
    m_segmentTool("CscSegmentUtilTool/CscSegmentUtilTool"),
    m_cscClusterOnTrackCreator("Muon::CscClusterOnTrackCreator/CscClusterOnTrackCreator"),
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool") 
{
  declareInterface<ICscSegmentFinder>(this);
  declareProperty("segmentTool", m_segmentTool);
  declareProperty("cscRotCreator", m_cscClusterOnTrackCreator);
  declareProperty("IdHelper", m_idHelper);
  declareProperty("cscdig_sg_inkey", m_cscdig_sg_inkey = "CSC_Measurements");
}

//******************************************************************************

// Destructor.

Csc2dSegmentMaker::~Csc2dSegmentMaker() {
}

//******************************************************************************

StatusCode Csc2dSegmentMaker::initialize(){

  ATH_MSG_DEBUG ( "Initializing " << name() );
  // Show keys.
  ATH_MSG_DEBUG ( "  SegmentTool: " << m_segmentTool.typeAndName() );
  ATH_MSG_DEBUG ( "  Input cscdig key: " << m_cscdig_sg_inkey );

  // Retrieve the detector descriptor.
  if ( detStore()->retrieve(m_pgm).isFailure() ) {
    ATH_MSG_FATAL ( " Cannot retrieve MuonReadoutGeometry " );
    return StatusCode::FAILURE;
  }

  m_phelper = m_pgm->cscIdHelper();
  
  if ( m_segmentTool.retrieve().isFailure() ) {
    ATH_MSG_ERROR ( "Unable to retrieve CscSegmentUtilTool " << m_segmentTool );
    return StatusCode::FAILURE;
  }
  if ( m_cscClusterOnTrackCreator.retrieve().isFailure() ) {
    ATH_MSG_ERROR ( "Unable to retrieve  " << m_cscClusterOnTrackCreator );
    return StatusCode::FAILURE;
  }
  if ( m_idHelper.retrieve().isFailure() ) {
    ATH_MSG_ERROR ( "Unable to retrieve CscSegmentUtilTool " << m_idHelper );
    return StatusCode::FAILURE;
  }  
  
  return StatusCode::SUCCESS;
}


//******************************************************************************
MuonSegmentCombinationCollection* Csc2dSegmentMaker::find( const std::vector<const Muon::CscPrepDataCollection*>& pcols)
{

  // Construct output segment collection.
  MuonSegmentCombinationCollection* mpsegs = 0;

  bool haveElement = false;
  for ( unsigned int icol=0; icol <pcols.size(); ++icol) {
    const CscPrepDataCollection* clus = pcols[icol];
    if (clus->size() == 0)     continue; // skip zero cluster collection
    haveElement = true;
    break;
  }

  if (haveElement)
    mpsegs = new MuonSegmentCombinationCollection;

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
      
  return mpsegs;
}


//******************************************************************************

MuonSegmentCombination* Csc2dSegmentMaker::findSegmentCombination(const CscPrepDataCollection& clus) {

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


  for ( CscPrepDataCollection::const_iterator iclu=clus.begin();
        iclu!=clus.end(); ++iclu ) {
    CscPrepData* pclu = *iclu;
    Identifier id = pclu->identify();
    int station = m_phelper->stationName(id) - 49;
    int eta = m_phelper->stationEta(id);
    int phisec = m_phelper->stationPhi(id);
    int iwlay = m_phelper->wireLayer(id);
    bool measphi = m_phelper->measuresPhi(id);
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

//     // create CscClusterOnTrack and pass to segment finder.
//     Trk::LocalPosition lpos = pclu->localPosition();

//     Trk::DefinedParameter  locPar(lpos.x(),Trk::locX);
//     Trk::LocalParameters*  ppars = new Trk::LocalParameters(locPar); 
//     Trk::ErrorMatrix* pcerr = new Trk::ErrorMatrix(pclu->localErrorMatrix());

    //    Trk::ParamDefs icor = Trk::loc1;
    //    double positionAlongStrip = lpos.get(icor); // should be carefully estimated

    const Muon::MuonClusterOnTrack* clu = m_cscClusterOnTrackCreator->createRIO_OnTrack(*pclu,pclu->globalPosition());
    const Muon::CscClusterOnTrack* ptclu = dynamic_cast<const Muon::CscClusterOnTrack*>(clu);
    if( !ptclu ){
      ATH_MSG_WARNING("Failed to create CscClusterOnTrack from PRD");
      delete clu;
      continue;
    }
    Amg::Vector3D lpos = gToLocal*ptclu->globalPosition();
//     std::cout << " " << m_idHelper->toString(ptclu->identify()) << " lpos " << lpos << "  locPos " << pclu->localPosition() << std::endl;
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

  //  MuonSegmentCombination* pcol;
  MuonSegmentCombination* pcol = 0;
  if (nHitLayer_eta >=2 || nHitLayer_phi >=2) {
    ATH_MSG_DEBUG( "Csc2dSegment calls get2dMuonSegmentCombination !!!" );
    pcol = m_segmentTool->get2dMuonSegmentCombination(eta_id, phi_id, eta_clus, phi_clus, lpos000 ); 
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
MuonSegmentCombinationCollection* Csc2dSegmentMaker::find( const MuonSegmentCombinationCollection& )
{
  return 0;

}

//******************************************************************************

StatusCode Csc2dSegmentMaker::finalize() {

  ATH_MSG_DEBUG ( "Goodbye" );
  return StatusCode::SUCCESS;
}
 


