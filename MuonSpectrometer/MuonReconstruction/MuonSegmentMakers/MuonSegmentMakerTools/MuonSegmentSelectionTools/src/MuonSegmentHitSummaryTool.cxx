/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentHitSummaryTool.h"
 
#include "GaudiKernel/MsgStream.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonSegment/MuonSegmentQuality.h"
#include <map>

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonIdHelpers/MdtIdHelper.h"


namespace Muon {

  MuonSegmentHitSummaryTool::MuonSegmentHitSummaryTool(const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa),
      m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"), 
      m_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
      m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
      m_detMgr(0)
  {
    declareInterface<IMuonSegmentHitSummaryTool>(this);

    declareProperty("PositionAlongTubeCut",m_positionAlongTubeCut = -200., "Cut on distance from tube end used for holes, negative value means in the tube");
    declareProperty("MinimumADCValue",m_lowerADCBound = 70. );
  }


  MuonSegmentHitSummaryTool::~MuonSegmentHitSummaryTool(){}


  StatusCode MuonSegmentHitSummaryTool::initialize()
  {
    if ( AthAlgTool::initialize().isFailure() ) {
      return StatusCode::FAILURE;
    }

    StoreGateSvc* detStore=0;
    if( serviceLocator()->service("DetectorStore", detStore).isSuccess() ) {
      if ( detStore->retrieve( m_detMgr ).isFailure() ) {
	ATH_MSG_ERROR( " Cannot retrieve MuonDetDescrMgr ");
	return StatusCode::FAILURE;
      }
    } else {
      ATH_MSG_ERROR(" MuonDetDescrMgr not found in DetectorStore ");
      return StatusCode::FAILURE;
    }

    if(m_helperTool.retrieve().isFailure()){
      ATH_MSG_ERROR("Could not get " << m_helperTool);
      return StatusCode::FAILURE;
    }

    if(m_printer.retrieve().isFailure()){
      ATH_MSG_ERROR("Could not get " << m_printer);
      return StatusCode::FAILURE;
    }
    
    if(m_idHelperTool.retrieve().isFailure()){
      ATH_MSG_ERROR("Could not get " << m_idHelperTool);
      return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
  }

  StatusCode MuonSegmentHitSummaryTool::finalize()
  {
    if( AthAlgTool::finalize().isFailure() ) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
  }

  MuonSegmentHitSummaryTool::HitCounts MuonSegmentHitSummaryTool::getHitCounts( const MuonSegment& seg ) const {

    HitCounts hitCounts;
        

    // calculate shortest channel
    double shortestTube = 1e9;
    const MdtDriftCircleOnTrack* mdtShortest = 0;
    bool transformIsSet = false;
    Amg::Transform3D gToAMDB;
    Amg::Vector3D lpos(0., 0., 0.);
    Amg::Vector3D ldir;
    double dxdy = 1.;

    Identifier chid = m_helperTool->chamberId(seg);
    MuonStationIndex::StIndex stIndex = m_idHelperTool->stationIndex(chid);
    if( (!m_idHelperTool->isCsc(chid) && stIndex == MuonStationIndex::EI) || 
	stIndex == MuonStationIndex::BO )       hitCounts.nexpectedTrigHitLayers = 1;
    else if( stIndex == MuonStationIndex::BM )  hitCounts.nexpectedTrigHitLayers = 2;
    else if( stIndex == MuonStationIndex::EM )  hitCounts.nexpectedTrigHitLayers = 3;
  
    // loop over hits
    std::vector<const Trk::MeasurementBase*>::const_iterator mit = seg.containedMeasurements().begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = seg.containedMeasurements().end();
    for( ;mit!=mit_end;++mit ){
      
      // get id and check that it is a muon hit id
      Identifier id = m_helperTool->getIdentifier(**mit);
      if( !id.is_valid() || !m_idHelperTool->isMuon(id) ) continue;

      // check if MDT is so increase ml counter
      if( m_idHelperTool->isMdt(id) ){
	// get layer index
	int ml  = m_idHelperTool->mdtIdHelper().multilayer(id);
	int lay = m_idHelperTool->mdtIdHelper().tubeLayer(id);
	int tube = m_idHelperTool->mdtIdHelper().tube(id); 
	int layIndex = 4*(ml-1) + (lay-1); // start at 0 rather than at 1
	MdtLayerIntersect& layIntersect = hitCounts.mdtHitHolePerLayerCounts[layIndex];
	++layIntersect.nhits;

	const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(*mit);
	if( mdt ){

	  const MuonGM::MdtReadoutElement* detEl = mdt->prepRawData() ? mdt->prepRawData()->detectorElement() : m_detMgr->getMdtReadoutElement(id);
	  if( !detEl ){
	    ATH_MSG_WARNING(" could not get MdtReadoutElement for tube " << m_idHelperTool->toString(id));
	    continue;
	  }
	  
	  double tubeLen = detEl->getActiveTubeLength(lay,tube);
	  layIntersect.tubeLength = tubeLen;
	  if( !transformIsSet ){
	    gToAMDB = detEl->GlobalToAmdbLRSTransform();
	    lpos = gToAMDB*seg.globalPosition();
	    ldir = (gToAMDB*seg.globalDirection()).unit();
	    dxdy = fabs(ldir.y()) > 0.001 ? ldir.x()/ldir.y() : 1000.;
	    transformIsSet = true;
	  }
	  if( !mdtShortest || tubeLen < shortestTube ){
	    mdtShortest = mdt;
	    shortestTube = tubeLen;
	  }
	}
	if( m_idHelperTool->mdtIdHelper().multilayer(id) == 1 ) ++hitCounts.nmdtHitsMl1; 
	else                                                    ++hitCounts.nmdtHitsMl2;

	if (mdt){
        if( mdt->prepRawData() ){
	  int adc = mdt->prepRawData()->adc();
	  if( adc > m_lowerADCBound ) ++hitCounts.nmdtHighADCHits;
	  if( adc > hitCounts.adcMax ) hitCounts.adcMax = adc;
	}
        }

      }else if( m_idHelperTool->isTrigger(id) ){
	// get gasgap ID (same for eta/phi projection)
	Identifier gasGapId = m_idHelperTool->gasGapId(id);
	bool measuresPhi = m_idHelperTool->measuresPhi(id);
	if( measuresPhi ) ++hitCounts.hitCountsPerLayer[gasGapId].nphiHits;
	else              ++hitCounts.hitCountsPerLayer[gasGapId].netaHits;
      }else{
	bool measuresPhi = m_idHelperTool->measuresPhi(id);
	if( measuresPhi ) ++hitCounts.ncscHitsPhi;
	else              ++hitCounts.ncscHitsEta;
      }
    }

    const MuonSegmentQuality* quality = dynamic_cast<const MuonSegmentQuality*>(seg.fitQuality());
    if( quality ){
      std::vector<Identifier>::const_iterator hit = quality->channelsWithoutHit().begin();
      std::vector<Identifier>::const_iterator hit_end = quality->channelsWithoutHit().end();
      for( ;hit!=hit_end;++hit ){

	const Identifier& id = *hit;
	// get layer index
	int ml  = m_idHelperTool->mdtIdHelper().multilayer(id);
	int lay = m_idHelperTool->mdtIdHelper().tubeLayer(id);
	int tube = m_idHelperTool->mdtIdHelper().tube(id);
	int layIndex = 4*(ml-1) + (lay-1); // subtract 1 as fields start with 1 instead of 0
	MdtLayerIntersect& layIntersect = hitCounts.mdtHitHolePerLayerCounts[layIndex];
	++layIntersect.nholes;

	if( transformIsSet ){
	  const MuonGM::MdtReadoutElement* detEl = m_detMgr->getMdtReadoutElement(id);
	  if( !detEl ){
	    ATH_MSG_WARNING(" could not get MdtReadoutElement for tube " << m_idHelperTool->toString(id));
	    continue;
	  }
	  double tubeLen = detEl->getActiveTubeLength(lay,tube);
	  double ytube = (gToAMDB*detEl->center(id)).y();
	  double xint = dxdy*( ytube - lpos.y() ) + lpos.x();
	  layIntersect.distFromTubeEnd = xint;
	  layIntersect.tubeLength = tubeLen;
	}
      }
    }


    // now loop over map and get the counts for the trigger hits
    std::map<Identifier,EtaPhiHitCount>::iterator it = hitCounts.hitCountsPerLayer.begin();
    std::map<Identifier,EtaPhiHitCount>::iterator it_end = hitCounts.hitCountsPerLayer.end();
    for( ;it!=it_end;++it ){

      EtaPhiHitCount& counts = it->second;
      
      // increase eta/phi hit counts
      if( counts.nphiHits != 0 ) ++hitCounts.nphiTrigHitLayers;
      if( counts.netaHits != 0 ){
	++hitCounts.netaTrigHitLayers;

	// if both eta/phi projection have entries increase paired counters
	if( counts.nphiHits != 0 ) ++hitCounts.npairedTrigHitLayers;
      }
    }

    int currentEnclosedHoles = 0;
    bool firstLayerWithHits = false;
    // now loop over mdt map and get the number of enclosed holes
    MdtLayerIntersectMap::iterator lit = hitCounts.mdtHitHolePerLayerCounts.begin();
    MdtLayerIntersectMap::iterator lit_end = hitCounts.mdtHitHolePerLayerCounts.end();
    for( ;lit!=lit_end;++lit ){
      //std::cout << " layer " << lit->first << " hits " << lit->second.nhits << " holes " << lit->second.nholes << std::endl;
      hitCounts.nmdtHoles += lit->second.nholes;
      if( lit->second.nhits == 0 ){
	++currentEnclosedHoles;
	// count holes within chamber bounds
	if( fabs(lit->second.distFromTubeEnd) - 0.5*lit->second.tubeLength < m_positionAlongTubeCut ) ++hitCounts.nmdtHolesInChamber;
      }else{
	if( firstLayerWithHits ) hitCounts.nmdtEnclosedHoles += currentEnclosedHoles;
	currentEnclosedHoles = 0;
	firstLayerWithHits = true;
      }
    }

    if( mdtShortest ){
      double posAlongTube = (mdtShortest->associatedSurface().transform().inverse()*seg.globalPosition()).z();
      if( 0.5*shortestTube - fabs(posAlongTube) < 100. ) hitCounts.closeToChamberEdge = true;
    }

    if( seg.fitQuality() ){
      // combine chi2 with missing hit information and apply cut
      hitCounts.segmentQuality = seg.fitQuality()->chiSquared() + 5.1*( hitCounts.nmdtHoles );
      if( seg.fitQuality()->numberDoF() > 0 ) hitCounts.segmentQuality /= seg.fitQuality()->numberDoF();
    }
      

      
    return hitCounts;
  }

}
