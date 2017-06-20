/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Csc4dSegmentMaker.h"
#include <sstream>
#include "CscSegmentMakers/ICscSegmentUtilTool.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/TrapezoidBounds.h"

#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"

#include "TrkEventPrimitives/FitQuality.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "MuonSegment/MuonSegment.h"

#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "CscClusterization/ICscStripFitter.h"
#include "CscClusterization/ICscClusterFitter.h"

#include "TrkSegment/Segment.h"
#include "TrkRoad/TrackRoad.h"

#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

using Muon::CscPrepData;
using Muon::CscPrepDataCollection;
using Muon::CscPrepDataContainer;

using MuonGM::CscReadoutElement;

using Muon::CscClusterOnTrack;
using Muon::MuonSegment;
using Muon::MuonSegmentCombination;

using Muon::MuonClusterOnTrack;
using Muon::MdtDriftCircleOnTrack;

/*namespace {
  std::string station_name(int station) {
    if ( station == 1 ) return "CSS";
    if ( station == 2 ) return "CSL";
    return "UNKNOWN_STATION";
  }
  std::string measphi_name(bool measphi) {
    if ( measphi ) return "phi";
    return "r";
  }
}*/

//******************************************************************************

Csc4dSegmentMaker::
Csc4dSegmentMaker(const std::string& type, const std::string& aname, const IInterface* parent)
: AthAlgTool(type, aname, parent),
  m_dumped(0), m_dump(false), m_pgm(0), m_phelper(0),
  m_segmentTool("CscSegmentUtilTool/CscSegmentUtilTool"),
  m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool")
{

  declareInterface<ICscSegmentFinder>(this);
  declareInterface<Muon::IMuonSegmentMaker>(this);
  declareProperty("dump_count", m_dumpcount =-1);
  declareProperty("max_chisquare", m_max_chisquare = 25);
  declareProperty("max_slope_r", m_max_slope_r = 0.2);
  declareProperty("max_slope_phi", m_max_slope_phi = 0.2);
  declareProperty("max_seg_per_chamber", m_max_seg_per_chamber = 50);
  declareProperty("segmentTool", m_segmentTool);
}

//******************************************************************************

// Destructor.

Csc4dSegmentMaker::~Csc4dSegmentMaker() {
}

//******************************************************************************

StatusCode Csc4dSegmentMaker::initialize(){

  ATH_MSG_DEBUG ( "Initializing" );

  // Show keys.
  ATH_MSG_DEBUG ( "  Dump count: " << m_dumpcount );
  ATH_MSG_DEBUG ( "  Max chi-square: " << m_max_chisquare );
  ATH_MSG_DEBUG ( "  Max r slope: " << m_max_slope_r );
  ATH_MSG_DEBUG ( "  Max phi slope: " << m_max_slope_phi );
  ATH_MSG_DEBUG ( "  Max segments/chamber: " << m_max_seg_per_chamber );
  ATH_MSG_DEBUG ( "  SegmentTool: " << m_segmentTool.typeAndName() );

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

  if (m_printer.retrieve().isFailure()){
    ATH_MSG_ERROR("Failed to initialize " << m_printer );
    return StatusCode::FAILURE;
  }

 
  return StatusCode::SUCCESS;
}

//******************************************************************************

MuonSegmentCombinationCollection*
Csc4dSegmentMaker::find( const MuonSegmentCombinationCollection& segcols)
{

  // Set dump flag.
  m_dump = m_dumped < m_dumpcount || m_dumpcount < 0;
  if ( m_dump ) ++m_dumped;
    
  MuonSegmentCombinationCollection* pcols = 0;
  if ( segcols.empty() ) return pcols;
  
  pcols = new MuonSegmentCombinationCollection;

  for ( MuonSegmentCombinationCollection::const_iterator icom=segcols.begin();
        icom!=segcols.end(); ++icom ) {
    MuonSegmentCombination& insegs = **icom;

    int nsta = insegs.numberOfStations();
    if ( nsta != 2 ) { // This can be happened if segment is not made at all in case of two hit layers !!
      // NOW, 2d segment is not made.... 
      //      ATH_MSG_FATAL ( "  Combination has " << nsta << " stations where 2 are expected" );
      return pcols;
    }

    ATH_MSG_DEBUG ( "Csc4dSegmentMaker called get4dMuonSegmentCombination");
    
    MuonSegmentCombination* pcol = m_segmentTool->get4dMuonSegmentCombination( &insegs );
    if (pcol) {
      pcols->push_back(pcol);
      ATH_MSG_DEBUG("Found 4d CSC segment " << std::endl << m_printer->print( *pcol ));
    }
  }

  ATH_MSG_DEBUG ( "    Created " << pcols->size() << " 4D segments." );
  return pcols;

}
//******************************************************************************
// For MuGirls... Seeded SegmentMaker part.... 
//Note: this code was used by the old MuGirl but is not part of the current reconstruction
std::vector<const MuonSegment*>* Csc4dSegmentMaker::find( const Amg::Vector3D& /*gpos*/, const Amg::Vector3D& /*gdir*/,
                                                          const std::vector< const MdtDriftCircleOnTrack* > & ,
                                                          const std::vector< const MuonClusterOnTrack* > & pcots,
                                                          bool, double) {

  if( pcots.empty() ) return 0;
  const Muon::CscClusterOnTrack* csc = dynamic_cast<const Muon::CscClusterOnTrack*>(pcots.front());
  if( !csc ){
    ATH_MSG_WARNING("Cluster input should only contain CSC hits");
    return 0;
  }
  const MuonGM::CscReadoutElement* detEl = csc->detectorElement();
  if( !detEl ){
    ATH_MSG_WARNING("Failed to obtain CscReadoutElement for cluster");
    return 0;
  }
  Amg::Transform3D gToLocal = detEl->GlobalToAmdbLRSTransform();
  Amg::Vector3D lpos000 = gToLocal*Amg::Vector3D(0.0, 0.0, 0.0);

  ChamberTrkClusters etaclus, phiclus;
  Identifier eta_id, phi_id, chid;
  for ( unsigned int icot=0; icot<pcots.size(); ++icot ) {

    const Muon::CscClusterOnTrack* csc = dynamic_cast<const Muon::CscClusterOnTrack*>(pcots[icot]);
    if( !csc ){
      ATH_MSG_WARNING("Failed to obtain CscReadoutElement for cluster");
      return 0;
    }
    ATH_MSG_VERBOSE ( " +++++++++ Errors from prio " << Amg::error(csc->localCovariance(),Trk::locX) << " ");
    
    Amg::Vector3D lpos = gToLocal*csc->globalPosition();
    const Identifier& id = csc->identify();
    bool measphi = m_phelper->measuresPhi(id);
    int iwlay = m_phelper->wireLayer(id);
    if ( measphi ) {
      phi_id = id;
      phiclus[iwlay-1].push_back(Cluster(lpos,csc,measphi));
    } else {
      eta_id = id;
      etaclus[iwlay-1].push_back(Cluster(lpos,csc,measphi));
    }
  }

  std::vector<const MuonSegment*>* segments = 0;
  //  std::vector<const MuonSegment*>* segments = 0


  int nHitLayer_eta =0;
  int nHitLayer_phi =0;
  for (int i=0; i<4; ++i) {
    //    ATH_MSG_DEBUG ( "No of clusters in layer " << i << " " << eta_clus[i].size() << " " << phi_clus[i].size() );
    if (etaclus[i].size() >0) ++nHitLayer_eta;
    if (phiclus[i].size() >0) ++nHitLayer_phi;
  }

  if (nHitLayer_eta >=2 || nHitLayer_phi >=2) {
    ATH_MSG_DEBUG( "Csc4dSegment calls getMuonSegments!!! " << nHitLayer_eta << " " << nHitLayer_phi );
    segments = m_segmentTool->getMuonSegments(eta_id, phi_id, etaclus, phiclus,lpos000);
  }
  
  return segments;

} //find

//Note: this code was used by the old MuGirl but is not part of the current reconstruction
//******************************************************************************
std::vector<const MuonSegment*>* Csc4dSegmentMaker::find( const Trk::TrackRoad& road,
                                                          const std::vector< std::vector< const MdtDriftCircleOnTrack* > >& ,
                                                          const std::vector< std::vector< const MuonClusterOnTrack* > >& clusters,
                                                          bool, double) {
  
  // Here, according to give TrackRoad, I do re-estimate errors of each COT and repeat segment fit
  
  // Reference
  // MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerTools/DCMathSegmentMaker/src/DCMathSegmentMaker.cxx#553

  const Amg::Vector3D& gpos = road.globalPosition();
  const Amg::Vector3D& gdir = road.globalDirection();
  
  // copy all clusters into one vector
  std::vector< const MdtDriftCircleOnTrack* > pmots;
  std::vector< const MuonClusterOnTrack* > pcots;
  std::vector< std::vector< const MuonClusterOnTrack* > >::const_iterator cit = clusters.begin();
  std::vector< std::vector< const MuonClusterOnTrack* > >::const_iterator cit_end = clusters.end();
  for(; cit!=cit_end;++cit ){
    std::copy( cit->begin(), cit->end(), std::back_inserter(pcots) );
  }

  return find(gpos, gdir, pmots, pcots, false, 0.0);
  
}



//******************************************************************************

StatusCode Csc4dSegmentMaker::finalize() {
  ATH_MSG_DEBUG ( "Goodbye" );
  return StatusCode::SUCCESS;
}
 

//************dummy function for interface******************************************************************

MuonSegmentCombinationCollection* Csc4dSegmentMaker::find(const std::vector<const Muon::CscPrepDataCollection*>& /*pcols*/){
  return 0;
}

std::vector<const MuonSegment*>* Csc4dSegmentMaker::find( const std::vector<const Trk::RIO_OnTrack*>& /*rios*/ ) {
  return 0;
}

std::vector<const MuonSegment*>* Csc4dSegmentMaker::find( const std::vector<const Trk::RIO_OnTrack*>& /*rios1*/,
                                                          const std::vector<const Trk::RIO_OnTrack*>& /*rios2*/ ) {
  return 0;
}
std::vector<const MuonSegment*>* Csc4dSegmentMaker::find( const std::vector<const MdtDriftCircleOnTrack*>& /*mdts*/,
                                                          const std::vector<const MuonClusterOnTrack*>&  /*clusters*/){
  return 0;
}
