/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleDetailedTruthAssociationTool.h"

#include "AthenaKernel/errorcheck.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "Particle/TrackParticle.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include <map>

namespace D3PD {

TrackParticleDetailedTruthAssociationTool::TrackParticleDetailedTruthAssociationTool (const std::string& type,
									  const std::string& name,
									  const IInterface* parent)
  : Base (type, name, parent)
{
  declareProperty ("MapKey", m_TruthMap = "TrackParticleTruthMap");
  declareProperty ("SGKey", m_TrkContainer = "TrackParticleCandidate");

  book().ignore();  // Avoid coverity warnings.
}


StatusCode TrackParticleDetailedTruthAssociationTool::initialize()
{
  CHECK( Base::initialize() );
  return StatusCode::SUCCESS;
}


StatusCode TrackParticleDetailedTruthAssociationTool::book ()
{

  CHECK( Base::book() );

  CHECK( addVariable ("nCommonPixHits", m_npixhits_common) );
  CHECK( addVariable ("nCommonSCTHits", m_nscthits_common) );
  CHECK( addVariable ("nCommonTRTHits", m_ntrthits_common) );

  CHECK( addVariable ("nRecoPixHits", m_npixhits_reco) );
  CHECK( addVariable ("nRecoSCTHits", m_nscthits_reco) );
  CHECK( addVariable ("nRecoTRTHits", m_ntrthits_reco) );

  CHECK( addVariable ("nTruthPixHits", m_npixhits_truth) );
  CHECK( addVariable ("nTruthSCTHits", m_nscthits_truth) );
  CHECK( addVariable ("nTruthTRTHits", m_ntrthits_truth) );

  CHECK( addVariable ("begVtx_barcode", m_begVtx_barcode) );
  CHECK( addVariable ("endVtx_barcode", m_endVtx_barcode) );

  CHECK( addVariable ("barcode", m_barcode) );

  return StatusCode::SUCCESS;
}

StatusCode TrackParticleDetailedTruthAssociationTool::invalidate(StatusCode sc){

  m_itr = m_end;
  return sc;
}

StatusCode TrackParticleDetailedTruthAssociationTool::reset(const Rec::TrackParticle& track){

  /// Get Track and TrackTruth Collections

  // -- TrackParticles:
  const TrackCollection* tc;
  StatusCode sc = evtStore()->retrieve(tc, m_TrkContainer);  
  if(sc.isFailure() || !tc) {
    REPORT_MESSAGE (MSG::WARNING) << "Could not retrieve: " << m_TrkContainer;
    return invalidate(StatusCode::SUCCESS);
  }
  // -- TrackParticleDetailedTruth:
  const DetailedTrackTruthCollection* tm;
  sc = evtStore()->retrieve(tm, m_TruthMap);
  if(sc.isFailure() || !tm) {
    REPORT_MESSAGE (MSG::WARNING) << "Could not retrieve: " << m_TruthMap;
    return invalidate(StatusCode::SUCCESS);
  }

  const Trk::Track *originalTrack = track.originalTrack();

  ElementLink<TrackCollection> tlink;
  tlink.setElement(originalTrack);
  tlink.setStorableObject(*tc);
  DetailedTrackTruthCollection::const_iterator found = tm->find(tlink);

  if(found==tm->end()){
    return invalidate(StatusCode::SUCCESS);
  }

  Range range = tm->equal_range(found->first);

  m_itr = range.first;
  m_end = range.second;

  return StatusCode::SUCCESS;
}

const HepMC::GenParticle *TrackParticleDetailedTruthAssociationTool::next (){

  if(m_itr==m_end) return 0;

  const DetailedTrackTruth dtt = m_itr->second;
  m_itr++;

  *m_npixhits_common = dtt.statsCommon()[SubDetHitStatistics::Pixel];
  *m_nscthits_common = dtt.statsCommon()[SubDetHitStatistics::SCT];
  *m_ntrthits_common = dtt.statsCommon()[SubDetHitStatistics::TRT];
  
  *m_npixhits_reco = dtt.statsTrack()[SubDetHitStatistics::Pixel];
  *m_nscthits_reco = dtt.statsTrack()[SubDetHitStatistics::SCT];
  *m_ntrthits_reco = dtt.statsTrack()[SubDetHitStatistics::TRT];
  
  *m_npixhits_truth = dtt.statsTruth()[SubDetHitStatistics::Pixel];
  *m_nscthits_truth = dtt.statsTruth()[SubDetHitStatistics::SCT];
  *m_ntrthits_truth = dtt.statsTruth()[SubDetHitStatistics::TRT];
  
  const HepMcParticleLink& blink = *(dtt.trajectory().rbegin());
  const HepMcParticleLink& elink = *(dtt.trajectory().begin());

  const HepMC::GenParticle* particle;
  if(!blink.isValid()){
    particle = &m_dummy;
  }else{
    particle = blink.cptr();
    *m_barcode = particle->barcode();
    HepMC::GenVertex *pv = particle->production_vertex();
    if(pv){
      *m_begVtx_barcode = pv->barcode();
    }
    if(elink.isValid()){
      HepMC::GenVertex *epv = particle->end_vertex();
      if(epv){
	*m_endVtx_barcode = epv->barcode();
      }
    }
  }

  if(!particle){
    REPORT_MESSAGE (MSG::WARNING) << "Pointer to particle is not valid";
  }

  return particle;
}

} // namespace D3PD
