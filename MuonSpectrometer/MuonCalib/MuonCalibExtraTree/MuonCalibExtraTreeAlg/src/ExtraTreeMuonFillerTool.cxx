/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ExtraTreeMuonFillerTool.h"
#include "muonEvent/MuonContainer.h"
#include "muonEvent/Muon.h"
#include "Particle/TrackParticle.h"
#include "TrkTrack/Track.h"
#include <string>
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"
#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "MuonSegment/MuonSegment.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkGeometry/MagneticFieldProperties.h"

namespace MuonCalib {

ExtraTreeMuonFillerTool :: ExtraTreeMuonFillerTool(const std::string & type, const std::string & name, const IInterface * parent): 
	ExtraTreeTrackFillerTool(type, name, parent),
	m_muonContainer("Muons"),
	m_hitsForSA(true),
	m_hitsForSAE(true),
	m_hitsForCombined(true),
	m_hitsForStatCombined(true),
	m_edmHelper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
	m_propagator("Trk::StraightLinePropagator/MuonStraightLinePropagator")
	{
	declareProperty("MuonContainer", m_muonContainer);
	declareProperty("AuthorOffset", m_authorOffset);
	declareProperty("HitsForSA", m_hitsForSA);
	declareProperty("HitsForSAE", m_hitsForSAE);
	declareProperty("HitsForCombined", m_hitsForCombined);
	declareProperty("HitsForStatCombined", m_hitsForStatCombined);
	declareProperty("MuonEDMHelperTool", m_edmHelper);
	declareProperty("Propagator", m_propagator);
	}

StatusCode ExtraTreeMuonFillerTool::initialize()
	{
	if(!retrieveTools().isSuccess())
		{
		ATH_MSG_FATAL("Retrieve Tools failed!");
		return StatusCode::FAILURE;
		}
	if(!m_edmHelper.retrieve().isSuccess())
		{
		ATH_MSG_FATAL("Failed to retrieve MuonEDMHelperTool!");
		return StatusCode::FAILURE;		
		}
	if(!m_propagator.retrieve().isSuccess())
		{
		ATH_MSG_FATAL("Failed to retrieve Propagator!");
		return StatusCode::FAILURE;		
		}
	return StatusCode::SUCCESS;	
	}

StatusCode ExtraTreeMuonFillerTool::finalize()
	{
	return StatusCode::SUCCESS;
	}
	
StatusCode ExtraTreeMuonFillerTool::writeTracks(unsigned int &index)
	{
	const Analysis::MuonContainer *mounContainer;
	if(!evtStore()->retrieve(mounContainer, m_muonContainer).isSuccess())
		{
		ATH_MSG_WARNING("Failed to retrieve MuonContainer "<<m_muonContainer);
		return StatusCode::SUCCESS;
		}
	for(Analysis::MuonContainer :: const_iterator it=mounContainer->begin(); it!=mounContainer->end(); it++)
		{
		const Analysis::Muon *muon=(*it);
		if(muon->muonSpectrometerTrackParticle())
			{
			const Rec::TrackParticle *part=muon->muonSpectrometerTrackParticle();
			const Rec::TrackParticle *prim=muon->track();
			bool has_track(false);
			if(part) {
//			has_track=true;
			if(!writeTrackPartice(part, part==prim, m_hitsForSA, index, 0).isSuccess())
				{
				ATH_MSG_FATAL("Failed to write SA track!");
				return StatusCode::FAILURE;
				}
			}
			part=muon->muonExtrapolatedTrackParticle();
			if(part) {
//			has_track=true;			
			if(!writeTrackPartice(part, part==prim, m_hitsForSAE, index, 1).isSuccess())
				{
				ATH_MSG_FATAL("Failed to write extrapolated SA track!");
				return StatusCode::FAILURE;
				}
			}
			part=muon->combinedMuonTrackParticle();
			if(part) {
			has_track=true;
			if(!writeTrackPartice(part, part==prim, m_hitsForCombined, index, 2).isSuccess())
				{
				ATH_MSG_FATAL("Failed to write extrapolated SA track!");
				return StatusCode::FAILURE;
				}
			}
			part=muon->statCombinedMuonTrackParticle();
			if(part) {
			has_track=true;
			if(!writeTrackPartice(part, part==prim, m_hitsForStatCombined, index, 3).isSuccess())
				{
				ATH_MSG_FATAL("Failed to write extrapolated SA track!");
				return StatusCode::FAILURE;
				}
			}
			part=muon->innerExtrapolatedTrackParticle();
			if(part) {
			has_track=true;
			if(!writeTrackPartice(part, part==prim, m_hitsForStatCombined, index, 4).isSuccess())
				{
				ATH_MSG_FATAL("Failed to write extrapolated InnerEx track!");
				return StatusCode::FAILURE;
				}
			}
			if(!has_track){
			if(muon->numberOfSegments())
				{
				Trk::Track *trk=createTaggedMuonTrack(*muon);
				if(trk)
					{
					handleTrack(trk, index, m_authorOffset + 5 + (muon->inDetTrackParticle()==prim)*10, true);
					delete trk;
					}
				}
			}}
		}
	return StatusCode::SUCCESS;
	}

inline StatusCode ExtraTreeMuonFillerTool::writeTrackPartice(const Rec::TrackParticle *& part, bool  isPrimaryAuthor, bool writeHits, unsigned int & index, int author)
	{
	const Trk::Track * track=part->originalTrack();
	if(!track)
		{
		ATH_MSG_FATAL("Track is not stored! Arey you running on AOD?");
		return StatusCode::FAILURE;
		}
	handleTrack(track, index, author + m_authorOffset + 10*isPrimaryAuthor, writeHits);
	return StatusCode::SUCCESS;
	}

inline Trk::Track* ExtraTreeMuonFillerTool::createTaggedMuonTrack( const Analysis::Muon& muon ) const {

  const Rec::TrackParticle* idtp = muon.inDetTrackParticle();
  const Trk::Track* track = idtp ? idtp->originalTrack() : 0;
  if (!track) return 0;
  ATH_MSG_VERBOSE(" makeSegmentTrack ");
  
  if( !track->trackStateOnSurfaces() || track->trackStateOnSurfaces()->empty() ) return 0;

  Trk::TrackInfo::TrackPatternRecoInfo author =  Trk::TrackInfo::MuGirlUnrefitted;
  if (muon.author() == MuonParameters::MuTag) author =  Trk::TrackInfo::StacoLowPt;
  if (muon.author() == MuonParameters::MuTagIMO) author =  Trk::TrackInfo::StacoLowPt;
  ATH_MSG_VERBOSE(" author " << muon.author());
  DataVector<const Trk::TrackStateOnSurface>*  trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>;

  //  Copy ID track
// if(!m_useMuonHitsOnly){
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it  = track->trackStateOnSurfaces()->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it_end = track->trackStateOnSurfaces()->end(); 
    for ( ; it!=it_end; ++it) trackStateOnSurfaces->push_back( (*it)->clone() );
  //}

  //  Loop over segments   
  int nseg = muon.numberOfSegments();
  for (int i = 0; i < nseg; ++i) {
    const Trk::Segment* tseg = muon.muonSegment(i);
    const Muon::MuonSegment* seg  = dynamic_cast<const  Muon::MuonSegment* > (tseg);
    if( !seg ) continue;

    // create pars for muon and loop over hits
    double momentum = 1e8;
    double charge   = 0.;
    const Trk::TrackParameters* pars = m_edmHelper->createTrackParameters( *seg, momentum, charge );
    std::vector<const Trk::MeasurementBase*>::const_iterator mit = seg->containedMeasurements().begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator mit_end = seg->containedMeasurements().end();
    for( ;mit!=mit_end;++mit ){

      const Trk::MeasurementBase& meas = **mit;

      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
      typePattern.set(Trk::TrackStateOnSurface::Measurement);

      const Trk::TrackParameters* exPars = m_propagator->propagateParameters(*pars,meas.associatedSurface(),
									     Trk::anyDirection, false, Trk::MagneticFieldProperties());
      if(!exPars){
	ATH_MSG_VERBOSE("Could not propagate Track to segment surface");
      }

      const Trk::TrackStateOnSurface* trackState = new Trk::TrackStateOnSurface( meas.clone(), exPars, 0, 0, typePattern );
      trackStateOnSurfaces->push_back( trackState ); 
    } // end segment loop
    delete pars;
  }
  Trk::TrackInfo info(Trk::TrackInfo::Unknown,Trk::muon);
  info.setPatternRecognitionInfo(author);
  Trk::Track* newtrack=  new Trk::Track(info, trackStateOnSurfaces,(track->fitQuality())->clone());
  return newtrack;
}


}//namespace MuonCalib
