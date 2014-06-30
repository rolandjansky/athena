/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentConverterTool.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "Identifier/Identifier.h"

namespace Muon {

  MuonSegmentConverterTool::MuonSegmentConverterTool(const std::string& t, const std::string& n, const IInterface* p) 
    : AthAlgTool(t,n,p),
      m_hitSummaryTool("Muon::MuonSegmentHitSummaryTool/MuonSegmentHitSummaryTool"),
      m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
      m_edmHelper("Muon::MuonEDMHelperTool/MuonEDMHelperTool")
  {
    declareInterface<xAODMaker::IMuonSegmentConverterTool>(this);
  }
    
  MuonSegmentConverterTool::~MuonSegmentConverterTool() {
    
  }
    
  StatusCode MuonSegmentConverterTool::initialize() {

    ATH_CHECK(m_hitSummaryTool.retrieve());
    ATH_CHECK(m_idHelper.retrieve());
    ATH_CHECK(m_edmHelper.retrieve());
    return StatusCode::SUCCESS;
  }

  StatusCode MuonSegmentConverterTool::finalize() {

    return StatusCode::SUCCESS;
  }

  xAOD::MuonSegment* MuonSegmentConverterTool::convert( const ElementLink< ::Trk::SegmentCollection >& segLink, xAOD::MuonSegmentContainer* container ) const {
    
    if( !segLink.isValid() || !*segLink ){
      ATH_MSG_WARNING(" Got invalid element link");
      return 0;
    }
    const MuonSegment* seg = dynamic_cast<const MuonSegment*>(*segLink);
    if( !seg ){
      ATH_MSG_WARNING(" Trk::Segment is not a MuonSegment ");
      return 0;
    }
    
    xAOD::MuonSegment* xaodSeg = convert(*seg,container);
    if( xaodSeg ) xaodSeg->setMuonSegment(segLink);
    return xaodSeg;
  }



  xAOD::MuonSegment* MuonSegmentConverterTool::convert( const MuonSegment& seg, xAOD::MuonSegmentContainer* container ) const{
    
    xAOD::MuonSegment* xaodSeg = new xAOD::MuonSegment();
    if( container ) container->push_back(xaodSeg);
    else            xaodSeg->makePrivateStore();
    
    xaodSeg->setPosition(seg.globalPosition().x(),seg.globalPosition().y(),seg.globalPosition().z());
    xaodSeg->setDirection(seg.globalDirection().x(),seg.globalDirection().y(),seg.globalDirection().z());

    if( seg.hasFittedT0() ) xaodSeg->setT0Error(seg.time(), seg.errorTime());

    const Trk::FitQuality* fq = seg.fitQuality();
    if( fq ) xaodSeg->setFitQuality(fq->chiSquared(),fq->numberDoF());

    Identifier id = m_edmHelper->chamberId(seg);
    int eta = m_idHelper->stationEta(id);
    int sector = m_idHelper->sector(id);
    MuonStationIndex::ChIndex chIndex = m_idHelper->chamberIndex(id);
    MuonStationIndex::TechnologyIndex technology = m_idHelper->technologyIndex(id);
    xaodSeg->setIdentifier(sector,chIndex,eta,technology);

    IMuonSegmentHitSummaryTool::HitCounts hitCounts = m_hitSummaryTool->getHitCounts(seg);

    xaodSeg->setNHits(hitCounts.nmdtHitsMl1+hitCounts.nmdtHitsMl2+hitCounts.ncscHitsEta,
		      hitCounts.nphiTrigHitLayers,hitCounts.netaTrigHitLayers);
    
    return xaodSeg;
  } 


} //end of namespace
