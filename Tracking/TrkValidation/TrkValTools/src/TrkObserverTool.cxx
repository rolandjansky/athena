///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrkObserverTool.cxx 
// Implementation file for class TrkObserverTool
// Author: C.Rauchegger<christoph.rauchegger@cern.ch>

// Description:
//	This tool monitors a track candidate through the whole 
//	ambiguity solving process
//		(DenseEnvironmentsAmbiguityProcessorTool.cxx and						
//		 InDetDenseEnvAmbiTrackSelectionTool.cxx).
//	Every change (score, subtrack created, track rejected) is 
//	recorded and in the end a xAOD output file is created.

//	The output file contains all the tracks which occur during 
//	the ambiguity solving (input tracks, final tracks, and only
//	temporarly tracks as well). The tracks in the file contain 
//	all information the ambiguity solver has (pixel hits, SCT 
//	hits, split hits, etc.). And additional information by the							
//	TrkObserverTool (uniqueId, parentId, score, rejection location).							

//	Every track has ONLY one "direct subtrack". But this subtrack
//	can have another subtrack, ...

// The memory address of a track serves as Id for the tool. But the
// AmbiProcessorTool deletes tracks, this means the address can occur
// multiple times in the map holding the track information. We have
// to save the tracks anyway (to write them in the xAOD file), 
// therefore in the map the unique address is saved as well, which
// then is used in the xAOD file (also unique parent address).


// Small data model
/*
	-------------------------------------				
	|  ProcessorTool, SelectionTool     |
	-------------------------------------
	| 	Id1 (not unique)  						  |
  |																	  |
	-------------------------------------
								|
	-------------------------------------
	|	m_observedTrkMap                  |
	-------------------------------------
	|	Id1 (not unique) as key						|
	|	parentId1 (not unique), score, 	 	|
	|	reject location, Id2 (UNIQUE)			|
	|	parentId2 (UNIQUE)								|
	-------------------------------------
							|
	-------------------------------------
	| 	m_savedTracks (in Store gate)		|
	-------------------------------------
	| 	Id2 (unique)										|
	|																	  |
	-------------------------------------


	-------------------------------------
	| 	xAOD output file								|
	-------------------------------------
	|	Id2 (unique), parentId2 (unique) 	|
	|	score, reject location			 			|
	-------------------------------------
*/
/////////////////////////////////////////////////////////////////// 

// Package includes
#include "TrkValTools/TrkObserverTool.h"
#include <sstream> 

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"

#include "TrkTrack/TrackCollection.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
Trk::TrkObserverTool::TrkObserverTool( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent ) : 
  ::AthAlgTool  ( type, name, parent   ),
  m_savedTracksName("ObservedTrackCollection"),
  m_observedTrksXaodName("ObservedTracks"),
  m_observedTrkMap(0),
  m_particleCreator("Trk::TrackParticleCreatorTool/TrackParticleCreatorTool"),
  m_savedTracks(0)
{
  declareInterface< ITrkObserverTool >(this);
  
  declareProperty("ObservedTrackCollectionName" , m_savedTracksName="ObservedTrackCollection");
  declareProperty("ObservedTracksXaodName" ,			m_observedTrksXaodName="ObservedTracks");  
  declareProperty("TrackParticleCreator" ,				m_particleCreator );
	
}

// Destructor
///////////////
Trk::TrkObserverTool::~TrkObserverTool()
{}

// Athena algtool's Hooks
////////////////////////////
StatusCode Trk::TrkObserverTool::initialize()
{
	StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) 
  {
    ATH_MSG_FATAL( "AlgTool::initialize failed" );
    return StatusCode::FAILURE;
  }

	sc = m_particleCreator.retrieve();
  if (sc.isFailure()) 
  {
    ATH_MSG_FATAL( "Failed to retrieve tool " << m_particleCreator );
    return StatusCode::FAILURE;
  } 
  else 
    ATH_MSG_INFO ( "Retrieved tool " << m_particleCreator );

	
  return StatusCode::SUCCESS;
}

StatusCode Trk::TrkObserverTool::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

void Trk::TrkObserverTool::storeInputTracks(const TrackCollection& trackCollection) {
// This method stores the input track in the store gate (m_savedTracks)
// it also puts the tracks in m_observedTrkMap
// (contains all tracks which occur during the ambiguity solving)

	m_savedTracks = new TrackCollection(trackCollection);

	if (!evtStore()->contains<TrackCollection> (m_savedTracksName)) {		// we only record it if it is not already in the store gate
		StatusCode sc = evtStore()->record(m_savedTracks, m_savedTracksName ,true);	// allow modifications

		if (sc.isFailure())
			ATH_MSG_ERROR    ("Could not record input tracks");
		else {
			ATH_MSG_INFO    ("Saved "<<m_savedTracks->size()<<" input tracks");
		}
	}

	// insert all tracks in the map (with score -1, no rejection location (=0), no parend Id (=0) and -2 for holes/shared/split hits )
	for( const auto& track : *m_savedTracks ) {
		m_observedTrkMap->insert( std::make_pair(track, std::make_tuple(static_cast<Trk::Track*>(0), -1, 0, track, static_cast<Trk::Track*>(0), -2, -2, -2, -2, -2, -2, -2)) );		// -2 means not filled for holes, shared/split hits
		// Id, parentId, score, rejection location, uniqueId, uniqueParentId, numPixelHoles, numSCTHoles, numSplitSharedPixel, numSplitSharedSCT, numSharedOrSplit, numSharedOrSplitPixels, numShared
	}
}

void Trk::TrkObserverTool::dumpTrackMap() {
// prints out/dumps all entries in m_observedTrkMap

	ATH_MSG_INFO ("Dump observedTrkMap (size=" << m_observedTrkMap->size() << ")");

	int notRejected = 0;		// counts the tracks which not got rejected (this number should equal finalTracks)
	for (auto& itrMap : *m_observedTrkMap) {
		ATH_MSG_DEBUG (
			   "Id: " << itrMap.first 
			<< " parentId: " 								<< std::get<0>(itrMap.second) 
			<< " score: " 									<< std::get<1>(itrMap.second) 
			<< " rejectPlace: "				 			<< std::get<2>(itrMap.second) 
			<< " uniqueId: " 								<< std::get<3>(itrMap.second) 
			<< " uniqueParentId: " 					<< std::get<4>(itrMap.second) 
			<< " numPixelHoles: " 					<< std::get<5>(itrMap.second) 
			<< " numSCTHoles: " 						<< std::get<6>(itrMap.second) 
			<< " numSplitSharedPixel: " 		<< std::get<7>(itrMap.second) 
			<< " numSplitSharedSCT: " 			<< std::get<8>(itrMap.second) 
			<< " numSharedOrSplit: " 				<< std::get<9>(itrMap.second) 
			<< " numSharedOrSplitPixels: " 	<< std::get<10>(itrMap.second) 
			<< " numShared: " 							<< std::get<11>(itrMap.second) 
		);
		if (std::get<2>(itrMap.second) == 0)		// 
			notRejected++;
	}
	ATH_MSG_INFO ("Number of rejectLocation=0 (should equal final tracks): " << notRejected);
}

void Trk::TrkObserverTool::reset() {
	if (m_observedTrkMap) {		
		m_observedTrkMap->clear();
		delete m_observedTrkMap;
	}

	m_observedTrkMap = new ObservedTracksMap;
}

void Trk::TrkObserverTool::updateTrackMap(const Trk::Track& track, double score, int rejectPlace) {
	auto it_range = m_observedTrkMap->equal_range(&track);		// because same address is used multiple times we have to search by range

	if (it_range.first == it_range.second) {
		ATH_MSG_WARNING ("updateTrackMap: trackId << " << track << " not found in observedTrkMap");
	}
	else {	// range found
		// get last element from range, because it is the most recent added tracks (with the same address)
		auto lastElement = it_range.second;		//is the upper bound
		lastElement--;	// to really get the last Element
		std::get<1>(lastElement->second) = score;	// score
		std::get<2>(lastElement->second) = rejectPlace;	// location where it was rejected
	}
}

void Trk::TrkObserverTool::updateScore(const Trk::Track& track, double score) {
	auto it_range = m_observedTrkMap->equal_range(&track);		// because same address is used multiple times we have to search by range

	if (it_range.first == it_range.second) {
		ATH_MSG_WARNING("updateScore: trackId << " << track << " not found in observedTrkMap");
	}
	else {	// range found
		// get last element from range, becaus it is the most recent added tracks (with the same address)
		auto lastElement = it_range.second;		//is the upper bound
		lastElement--;	// to really get the last Element
		std::get<1>(lastElement->second) = score;	// score
	}
}

void Trk::TrkObserverTool::updateHolesSharedHits(const Trk::Track& track, int numPixelHoles, int numSCTHoles, int numSplitSharedPixel, int numSplitSharedSCT, int numSharedOrSplit, int numSharedOrSplitPixels, int numShared) const {
	auto it_range = m_observedTrkMap->equal_range(&track);		// because same address is used multiple times we have to search by range

	if (it_range.first == it_range.second) {
		ATH_MSG_WARNING("updateHolesSharedHits: trackId << " << track << " not found in observedTrkMap");
	}
	else {	// range found
		// get last element from range, becaus it is the most recent added tracks (with the same address)
		auto lastElement = it_range.second;		//is the upper bound
		lastElement--;	// to really get the last Element
		std::get<5>(lastElement->second) = numPixelHoles;	
		std::get<6>(lastElement->second) = numSCTHoles;	
		std::get<7>(lastElement->second) = numSplitSharedPixel;	// Number of Pixel clusters comptaible with being split that are also shared
		std::get<8>(lastElement->second) = numSplitSharedSCT;	// Number of SCT clusters comptaible with being split that are also shared
		std::get<9>(lastElement->second) = numSharedOrSplit;	// Number of split + shared clusters
		std::get<10>(lastElement->second) = numSharedOrSplitPixels;	// Number of pixel clusters that are either split or shared
		std::get<11>(lastElement->second) = numShared;	// Number of shared hits on track
	}
}

void Trk::TrkObserverTool::rejectTrack(const Trk::Track& track, int rejectPlace) const {
	auto it_range = m_observedTrkMap->equal_range(&track);		// because same address is used multiple times we have to search by range

	if (it_range.first == it_range.second) {
		ATH_MSG_WARNING("rejectTrack: trackId << " << track << " not found in observedTrkMap");
	}
	else {	// range found
		// get last element from range, becaus it is the most recent added tracks (with the same address)
		auto lastElement = it_range.second;		//is the upper bound
		lastElement--;	// to really get the last Element
		std::get<2>(lastElement->second) = rejectPlace;	// place where it was rejected
	}
}

void Trk::TrkObserverTool::addSubTrack(const Trk::Track& track, const Trk::Track& parentTrack) {
	Trk::Track* copiedTrack = new Trk::Track(track);		// deep copy of the track (because some subtracks get deleted), information has to be available later 
	m_savedTracks->push_back(copiedTrack);		// push into collection, that new (sub)track gets saved
	
	double score = -1;
	Trk::Track* uniqueParentId = 0;

	auto it_range = m_observedTrkMap->equal_range(&parentTrack);		// because same address is used multiple times we have to search by range

	if (it_range.first == it_range.second) {
		ATH_MSG_WARNING("addSubTrack: trackId << " << parentTrack << " not found in observedTrkMap");
	}
	else {	// range found
		// get last element from range, becaus it is the most recent added tracks (with the same address)
		auto lastElement = it_range.second;		//is the upper bound
		lastElement--;	// to really get the last Element

		score = std::get<1>(lastElement->second);	// get score from parent element
		uniqueParentId = std::get<3>(lastElement->second);	// get score from parent element
	}

	m_observedTrkMap->insert( std::make_pair(&track, std::make_tuple(&parentTrack, score, 0, copiedTrack, uniqueParentId, -2, -2, -2, -2, -2, -2, -2)) );		// add to map (-2 means not filled yet)
}

void Trk::TrkObserverTool::saveTracksToxAOD() {
	ATH_MSG_INFO ("Saving "<<m_savedTracks->size() << " observed track candidates to xAOD");

	xAOD::TrackParticleContainer* observedTracks = new xAOD::TrackParticleContainer();
	xAOD::TrackParticleAuxContainer* observedTracksAux = new xAOD::TrackParticleAuxContainer();

	StatusCode scObsTrack;
	scObsTrack =  evtStore()->record(observedTracksAux, m_observedTrksXaodName+"Aux.") ;

	if (scObsTrack.isFailure()) 
		ATH_MSG_WARNING("ERROR record " << m_observedTrksXaodName << "Aux."); 
	else
		ATH_MSG_DEBUG( "Recorded TrackParticles with key: " << m_observedTrksXaodName << "Aux." );

	observedTracks->setStore(observedTracksAux);

	scObsTrack =  evtStore()->record(observedTracks, m_observedTrksXaodName) ;
	if (scObsTrack.isFailure()) 
		ATH_MSG_WARNING("ERROR record " << m_observedTrksXaodName); 
	else
		ATH_MSG_DEBUG( "Recorded TrackParticles with key: " << m_observedTrksXaodName);


	// Create the xAOD objects:		
	for( const auto& itr : *m_savedTracks) {		// looping through tracks
	
		// Create the xAOD object:
		if (!itr) {
			ATH_MSG_WARNING("WTaF? Empty element in container!");
			continue;
		}
		//ATH_MSG_DEBUG ("Processing track candidate "<<itr);

		xAOD::TrackParticle *newParticle = createParticle(*observedTracks, *m_savedTracks, *itr);
		if(!newParticle){
			ATH_MSG_WARNING("Failed to create a TrackParticle");
			continue;
		}

		bool found = false;
		auto itrMap = m_observedTrkMap->begin();
		auto itrEnd = m_observedTrkMap->end();
		
		while (itrMap != itrEnd && !found) {
			if (std::get<3>(itrMap->second) == itr)		// search for the unique Id in the map
				found = true;
			else
				++itrMap;
		}

		if (found) {		// itrMap is on now the found element
			Trk::Track *tmpTrackId = std::get<3>(itrMap->second);
			newParticle->auxdata<long int>("Id") = reinterpret_cast<long int>(tmpTrackId);		//unique ID
			Trk::Track *tmpTrackParentId = std::get<4>(itrMap->second);
			newParticle->auxdata<long int>("parentId") = reinterpret_cast<long int>(tmpTrackParentId);	//parentId (unique) from map
			newParticle->auxdata<double>("score") = std::get<1>(itrMap->second);	// score from map
			newParticle->auxdata<int>("rejectPlace") = std::get<2>(itrMap->second);	// rejection Location from map

			newParticle->auxdata<int>("numPixelHoles") = std::get<5>(itrMap->second);
			newParticle->auxdata<int>("numSCTHoles") = std::get<6>(itrMap->second);
			newParticle->auxdata<int>("numSplitSharedPixel") = std::get<7>(itrMap->second);	// Number of Pixel clusters comptaible with being split that are also shared
			newParticle->auxdata<int>("numSplitSharedSCT") = std::get<8>(itrMap->second);	// Number of SCT clusters comptaible with being split that are also shared
			newParticle->auxdata<int>("numSharedOrSplit") = std::get<9>(itrMap->second);	// Number of split + shared clusters
			newParticle->auxdata<int>("numSharedOrSplitPixels") = std::get<10>(itrMap->second);	// Number of pixel clusters that are either split or shared
			newParticle->auxdata<int>("numShared") = std::get<11>(itrMap->second);				// Number of shared hits on track

			// delete element from map, after saved to xaod (=decoration). this should speed up the search a little bit
			m_observedTrkMap->erase(itrMap);
		}
		else {		// not found
			ATH_MSG_WARNING("unique Trk-ID << " << itr << " NOT found in observedTrkMap");
			newParticle->auxdata<long int>("Id") = -1;
			newParticle->auxdata<long int>("parentId") = -1;		
			newParticle->auxdata<double>("score") = -1;
			newParticle->auxdata<int>("rejectPlace") = -1;

			newParticle->auxdata<int>("numPixelHoles") = -1;
			newParticle->auxdata<int>("numSCTHoles") = -1;
			newParticle->auxdata<int>("numSplitSharedPixel") = -1;
			newParticle->auxdata<int>("numSplitSharedSCT") = -1;
			newParticle->auxdata<int>("numSharedOrSplit") = -1;
			newParticle->auxdata<int>("numSharedOrSplitPixels") = -1;
			newParticle->auxdata<int>("numShared") = -1;
		}
	}
} 

xAOD::TrackParticle* Trk::TrkObserverTool::createParticle( xAOD::TrackParticleContainer& xaod, 
    const TrackCollection& container, 
    const Trk::Track& tp)
{
    // create the xAOD::TrackParticle, the pointer is added to the container in the function
    ElementLink<TrackCollection> trackLink(&tp,container);
    return m_particleCreator->createParticle(trackLink,&xaod);		
}
