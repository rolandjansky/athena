///////////////////////// -*- C++ -*- /////////////////////////////

/*
	Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// TrkObserverTool.cxx 
// Implementation file for class TrkObserverTool
// Authors: C.Rauchegger<christoph.rauchegger@cern.ch> and 
// 			S.Rettie<sebastien.rettie@cern.ch>

// Description:
//	This tool monitors a track candidate through the whole 
//	ambiguity solving process
//		(AmbiguityProcessorBase.cxx,
//		 DenseEnvironmentsAmbiguityProcessorTool.cxx,
//		 DenseEnvironmentsAmbiguityScoreProcessorTool.cxx,
//		 InDetDenseEnvAmbiTrackSelectionTool.cxx).
//	Every change (score, subtrack created, track rejected) is 
//	recorded and in the end an xAOD output file is created.

//	The output file contains all the tracks which occur during 
//	the ambiguity solving (input tracks, final tracks, and only
//	temporary tracks as well). The tracks in the file contain 
//	all information the ambiguity solver has (pixel hits, SCT 
//	hits, split hits, etc.) and additional information by the
//	TrkObserverTool (uniqueId, parentId, score, rejection location).

//	Every track has ONLY one "direct subtrack". But this subtrack
//	can have another subtrack, ...

// Each track is uniquely identified using a thread-safe
// std::atomic<std::uint32_t> (defined in AmbiguityProcessorUtility.cxx),
// which serves as Id for the tool. If a track has a parent, the unique 
// Id of the parent is also saved by the tool. As the ambiguity processor
// tools delete tracks, all tracks (including temporary tracks) are
// saved to the tool's cache entry, i.e. an ObservedTrackMap object.

// Two instances of the TrkObserverTool must be instantiated in order
// to avoid data handle conflicts:
// - Instance that does not initialize the data handles and 
//	 is used throughout the ambiguity solving
// - Instance that initializes the data handles and is only used
// 	 to write out the observed tracks after the ambiguity solving
//	 is done

// Package includes
#include "TrkValTools/TrkObserverTool.h"
#include <sstream> 

// FrameWork includes
#include "GaudiKernel/IToolSvc.h"
#include "TrkTrack/TrackCollection.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructor
////////////////
Trk::TrkObserverTool::TrkObserverTool(const std::string& type, const std::string& name, const IInterface* parent)
	: AthAlgTool(type, name, parent){
			declareInterface< ITrkObserverTool >(this);
			declareProperty("ObsTrackCollection", m_savedTracksWriteKey);
			declareProperty("ObsTrackCollectionMap", m_savedTracksMapWriteKey);
}

// Destructor
///////////////
Trk::TrkObserverTool::~TrkObserverTool() {}

// Athena algtool's Hooks
////////////////////////////
StatusCode Trk::TrkObserverTool::initialize() {
	ATH_MSG_INFO("Initializing TrkObserverTool with name: " << name());
	ATH_MSG_INFO("\tm_savedTracksWriteKey: " << m_savedTracksWriteKey.key());
	ATH_MSG_INFO("\tm_savedTracksMapWriteKey: " << m_savedTracksMapWriteKey.key());
	ATH_CHECK(AthAlgTool::initialize());
	ATH_CHECK(m_savedTracksWriteKey.initialize(!m_savedTracksWriteKey.key().empty()));
	ATH_CHECK(m_savedTracksMapWriteKey.initialize(!m_savedTracksMapWriteKey.key().empty()));
	ATH_MSG_INFO("Initialized TrkObserverTool");
	return StatusCode::SUCCESS;
}

StatusCode Trk::TrkObserverTool::finalize() {
	ATH_MSG_INFO("Finalizing " << name() << "...");
	return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////
void Trk::TrkObserverTool::newEvent(CacheEntry* ent) const {
	ATH_MSG_DEBUG("Starting new event");
	if (ent->m_observedTrkMap) {
		ent->m_observedTrkMap->clear();
		delete ent->m_observedTrkMap;
	}
	ent->m_observedTrkMap = new ObservedTrackMap;
}

void Trk::TrkObserverTool::updateTrackMap(int uid, double score, xAOD::RejectionStep rejectStep, xAOD::RejectionReason rejectReason) const {

	ATH_MSG_DEBUG("updateTrackMap: track "<<uid);
	// get event context and map from cache
	const EventContext& ctx{Gaudi::Hive::currentContext()};
	std::lock_guard<std::mutex> lock{m_mutex};
	ObservedTrackMap* trk_map = getTrackMap(ctx);
	// find track and update score
	if ( trk_map->find(uid) == trk_map->end() ) {
		// not found
		ATH_MSG_WARNING("updateTrackMap: track << " << uid << " not found in observedTrkMap");
	}
	else {
		// found
		std::get<xAOD::ObserverToolIndex::score>(trk_map->at(uid)) = score;
		std::get<xAOD::ObserverToolIndex::rejectStep>(trk_map->at(uid)) = rejectStep;
		std::get<xAOD::ObserverToolIndex::rejectReason>(trk_map->at(uid)) = rejectReason;
		// Keep track of cumulative rejection steps/reasons
		std::get<xAOD::ObserverToolIndex::rejectStep_full>(trk_map->at(uid)).push_back(rejectStep);
		std::get<xAOD::ObserverToolIndex::rejectReason_full>(trk_map->at(uid)).push_back(rejectReason);
		ATH_MSG_DEBUG("updateTrackMap: track "<<uid<<" with score, rejectStep, rejectReason: "<<score<<", "<<rejectStep<<", "<<rejectReason);
	}
}

void Trk::TrkObserverTool::updateScore(int uid, double score) const {

	ATH_MSG_DEBUG("updateScore: track "<<uid);
	// get event context and map from cache
	const EventContext& ctx{Gaudi::Hive::currentContext()};
	std::lock_guard<std::mutex> lock{m_mutex};
	ObservedTrackMap* trk_map = getTrackMap(ctx);
	// find track and update score
	if ( trk_map->find(uid) == trk_map->end() ) {
		// not found
		ATH_MSG_WARNING("updateScore: track << " << uid << " not found in observedTrkMap");
	}
	else {
		// found
		std::get<xAOD::ObserverToolIndex::score>(trk_map->at(uid)) = score;
		ATH_MSG_DEBUG("updateScore: track "<<uid<<" with score "<<score);
	}
}

void Trk::TrkObserverTool::rejectTrack(int uid, xAOD::RejectionStep rejectStep, xAOD::RejectionReason rejectReason) const {

	ATH_MSG_DEBUG("rejectTrack: track "<<uid);
	// get event context and map from cache
	const EventContext& ctx{Gaudi::Hive::currentContext()};
	std::lock_guard<std::mutex> lock{m_mutex};
	ObservedTrackMap* trk_map = getTrackMap(ctx);
	// find track and update rejection location
	if ( trk_map->find(uid) == trk_map->end() ) {
		// not found
		ATH_MSG_WARNING("rejectTrack: track "<<uid<<" not found in observedTrkMap");
	}
	else {
		// found
		std::get<xAOD::ObserverToolIndex::rejectStep>(trk_map->at(uid)) = rejectStep;
		std::get<xAOD::ObserverToolIndex::rejectReason>(trk_map->at(uid)) = rejectReason;
		// Keep track of cumulative rejection steps/reasons
		std::get<xAOD::ObserverToolIndex::rejectStep_full>(trk_map->at(uid)).push_back(rejectStep);
		std::get<xAOD::ObserverToolIndex::rejectReason_full>(trk_map->at(uid)).push_back(rejectReason);
		ATH_MSG_DEBUG("rejectTrack: track "<<uid<<" with rejection in "<<dumpRejection(rejectStep, rejectReason));
	}
}

void Trk::TrkObserverTool::addInputTrack(int uid, const Trk::Track& track) const {

	ATH_MSG_DEBUG("addInputTrack: track "<<uid);
	// test to see if we have a new event
	const EventContext& ctx{Gaudi::Hive::currentContext()};
	std::lock_guard<std::mutex> lock{m_mutex};
	CacheEntry* ent{m_cache.get(ctx)};
	if (ent->m_evt!=ctx.evt()) {
		// New event in this slot
		newEvent(ent);
		ent->m_evt = ctx.evt();
	}
	// add input track to cache map
	Trk::Track* copiedTrack = new Trk::Track(track);
	std::vector<xAOD::RejectionStep> v_rejectStep = {xAOD::RejectionStep::solveTracks};
	std::vector<xAOD::RejectionReason> v_rejectReason = {xAOD::RejectionReason::acceptedTrack};
	ent->m_observedTrkMap->insert( std::make_pair(uid, std::make_tuple(copiedTrack, // Id, track
											 -1, // score
											 xAOD::RejectionStep::solveTracks, // rejection step
											 xAOD::RejectionReason::acceptedTrack, // rejection reason
											 0, // unique parentId
											 // holes/shared/split hits information (-2 means not filled yet)
											 -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2.0f, -2.0f, -2,
											 v_rejectStep, v_rejectReason)));
}

void Trk::TrkObserverTool::addSubTrack(int track_uid, int parent_uid, const Trk::Track& track) const {

	ATH_MSG_DEBUG("addSubTrack: subtrack "<<track_uid);
	// get event context and map from cache
	const EventContext& ctx{Gaudi::Hive::currentContext()};
	std::lock_guard<std::mutex> lock{m_mutex};
	ObservedTrackMap* trk_map = getTrackMap(ctx);

	// deep copy of the track (because some subtracks get deleted), information has to be available later
	Trk::Track* copiedTrack = new Trk::Track(track);
	// get score and rejection step from parent element
	double score = -1;
	xAOD::RejectionStep rejectStep = xAOD::RejectionStep::solveTracks;
	if ( trk_map->find(parent_uid) == trk_map->end() ) {
		// not found
		ATH_MSG_WARNING("addSubTrack: parent " << parent_uid << " not found in observedTrkMap");
	}
	else {
		// found
		score = std::get<xAOD::ObserverToolIndex::score>(trk_map->at(parent_uid));
		rejectStep = std::get<xAOD::ObserverToolIndex::rejectStep>(trk_map->at(parent_uid));
		ATH_MSG_DEBUG("addSubTrack: track "<<track_uid<<" with parent "<<parent_uid<<", score "<<score);
	}
	// add subtrack to cache map
	std::vector<xAOD::RejectionStep> v_rejectStep = {rejectStep};
	std::vector<xAOD::RejectionReason> v_rejectReason = {xAOD::RejectionReason::acceptedTrack};
	trk_map->insert( std::make_pair(track_uid, std::make_tuple(copiedTrack, // Id, track
											 score, // score
											 rejectStep, // rejection step
											 xAOD::RejectionReason::acceptedTrack, // rejection reason
											 parent_uid, // unique parentId
											 // holes/shared/split hits information (-2 means not filled yet)
											 -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2.0f, -2.0f, -2,
											 v_rejectStep, v_rejectReason)));
}

ObservedTrackMap* Trk::TrkObserverTool::getTrackMap(const EventContext& ctx) const {

	ATH_MSG_DEBUG("Get track map from cache");
	// get cache
	CacheEntry* ent{m_cache.get(ctx)};
	// sanity check
	if (ent->m_evt!=ctx.evt()) { 
		ATH_MSG_ERROR("Different event context in getTrackMap!!");
	}
	return ent->m_observedTrkMap;
}

int Trk::TrkObserverTool::saveTracksToStore(const EventContext& ctx, const ObservedTrackMap* trk_map) const {

	std::lock_guard<std::mutex> lock{m_mutex};
	// Save tracks and map to store
	ATH_MSG_INFO ("saveTracksToStore: Recording "<<trk_map->size() << " observed track candidates to event store");
	ATH_MSG_DEBUG("\tm_savedTracksWriteKey: "<<m_savedTracksWriteKey.key());
	ATH_MSG_DEBUG("\tm_savedTracksMapWriteKey: "<<m_savedTracksMapWriteKey.key());

	SG::WriteHandle<TrackCollection> wh_tracks{m_savedTracksWriteKey, ctx};
	SG::WriteHandle<ObservedTrackMap> wh_tracksMap{m_savedTracksMapWriteKey, ctx};

	// Tracks write handle
	StatusCode sc = wh_tracks.record(std::make_unique<TrackCollection>());
	if (sc.isFailure()) {
		ATH_MSG_ERROR("saveTracksToStore: Could not record input tracks: "<<m_savedTracksWriteKey.key());
	}
	else {
		ATH_MSG_INFO("saveTracksToStore: Recorded empty container for input tracks to: "<<m_savedTracksWriteKey.key());
	}
	if (!wh_tracks.isValid()) {
		ATH_MSG_DEBUG ("saveTracksToStore: Invalid key: "<<m_savedTracksWriteKey.key());
	}
	else {
		ATH_MSG_DEBUG ("saveTracksToStore: Valid key: "<<m_savedTracksWriteKey.key());
	}

	// Tracks map write handle
	sc = wh_tracksMap.record(std::make_unique<ObservedTrackMap>());
	if (sc.isFailure()) {
		ATH_MSG_ERROR("saveTracksToStore: Could not record tracks map: "<<m_savedTracksMapWriteKey.key());
	}
	else {
		ATH_MSG_INFO("saveTracksToStore: Recorded empty container for tracks map to: "<<m_savedTracksMapWriteKey.key());
	}
	if (!wh_tracksMap.isValid()) {
		ATH_MSG_DEBUG ("saveTracksToStore: Invalid key: "<<m_savedTracksMapWriteKey.key());
	}
	else {
		ATH_MSG_DEBUG ("saveTracksToStore: Valid key: "<<m_savedTracksMapWriteKey.key());
	}

	for (auto& itrMap : *trk_map) {
		ATH_MSG_DEBUG("saveTracksToStore: Writing track with id "<<itrMap.first<<" and rejection reason "<<std::get<xAOD::ObserverToolIndex::rejectReason>(itrMap.second));
		wh_tracks->push_back(std::get<xAOD::ObserverToolIndex::track>(itrMap.second));
		wh_tracksMap->insert(std::make_pair(itrMap.first, itrMap.second));
	}

	ATH_MSG_DEBUG("saveTracksToStore: Pushed "<<wh_tracks->size()<<" observed tracks to store");
	ATH_MSG_DEBUG("saveTracksToStore: Pushed "<<wh_tracksMap->size()<<" pairs to track map in store");
	// Keep track of "good" tracks as a sanity check
	int nFinalTracks = getNFinalTracks(trk_map);
	ATH_MSG_DEBUG ("saveTracksToStore: Number of RejectionReason = acceptedTrack (should equal final tracks): " << nFinalTracks);
	return nFinalTracks;
}

void Trk::TrkObserverTool::updateHolesSharedHits(int uid, int numPixelHoles, int numSCTHoles, int numSplitSharedPixel, int numSplitSharedSCT,
		int numSharedOrSplit, int numSharedOrSplitPixels, int numShared, int isPatternTrack, int totalSiHits, int inROI, int hasIBLHit,
		int hasSharedIBLHit, int hasSharedPixel, int firstPixIsShared, int numPixelDeadSensor, int numSCTDeadSensor, int numPixelHits,
		int numSCTHits, int numUnused, int numTRT_Unused, int numSCT_Unused, int numPseudo, float averageSplit1, float averageSplit2, int numWeightedShared) const {

	ATH_MSG_DEBUG("updateHolesSharedHits: track "<<uid);
	// get event context and map from cache
	const EventContext& ctx{Gaudi::Hive::currentContext()};
	std::lock_guard<std::mutex> lock{m_mutex};
	ObservedTrackMap* trk_map = getTrackMap(ctx);

	// find track and update rejection location
	if ( trk_map->find(uid) == trk_map->end() ) {
		// not found
		ATH_MSG_WARNING("updateHolesSharedHits: track "<<uid<<" not found in observedTrkMap");
	}
	else {
		// found
		std::get<xAOD::ObserverToolIndex::numPixelHoles>(trk_map->at(uid))  = numPixelHoles;	
		std::get<xAOD::ObserverToolIndex::numSCTHoles>(trk_map->at(uid))  = numSCTHoles;	
		std::get<xAOD::ObserverToolIndex::numSplitSharedPixel>(trk_map->at(uid))  = numSplitSharedPixel; // Number of Pixel clusters comptaible with being split that are also shared
		std::get<xAOD::ObserverToolIndex::numSplitSharedSCT>(trk_map->at(uid))  = numSplitSharedSCT; // Number of SCT clusters comptaible with being split that are also shared
		std::get<xAOD::ObserverToolIndex::numSharedOrSplit>(trk_map->at(uid))  = numSharedOrSplit; // Number of split + shared clusters
		std::get<xAOD::ObserverToolIndex::numSharedOrSplitPixels>(trk_map->at(uid)) = numSharedOrSplitPixels; // Number of pixel clusters that are either split or shared
		std::get<xAOD::ObserverToolIndex::numShared>(trk_map->at(uid)) = numShared; // Number of shared hits on track
		std::get<xAOD::ObserverToolIndex::isPatternTrack>(trk_map->at(uid)) = isPatternTrack; // Pattern Track or Fitted track
		std::get<xAOD::ObserverToolIndex::totalSiHits>(trk_map->at(uid)) = totalSiHits; // totalSiHits
		std::get<xAOD::ObserverToolIndex::inROI>(trk_map->at(uid)) = inROI;
		std::get<xAOD::ObserverToolIndex::hasIBLHit>(trk_map->at(uid)) = hasIBLHit;
		std::get<xAOD::ObserverToolIndex::hasSharedIBLHit>(trk_map->at(uid)) = hasSharedIBLHit;
		std::get<xAOD::ObserverToolIndex::hasSharedPixel>(trk_map->at(uid)) = hasSharedPixel;
		std::get<xAOD::ObserverToolIndex::firstPixIsShared>(trk_map->at(uid)) = firstPixIsShared;
		std::get<xAOD::ObserverToolIndex::numPixelDeadSensor>(trk_map->at(uid)) = numPixelDeadSensor;
		std::get<xAOD::ObserverToolIndex::numSCTDeadSensor>(trk_map->at(uid)) = numSCTDeadSensor;
		std::get<xAOD::ObserverToolIndex::numPixelHits>(trk_map->at(uid)) = numPixelHits;
		std::get<xAOD::ObserverToolIndex::numSCTHits>(trk_map->at(uid)) = numSCTHits;
		std::get<xAOD::ObserverToolIndex::numUnused>(trk_map->at(uid)) = numUnused;
		std::get<xAOD::ObserverToolIndex::numTRT_Unused>(trk_map->at(uid)) = numTRT_Unused;
		std::get<xAOD::ObserverToolIndex::numSCT_Unused>(trk_map->at(uid)) = numSCT_Unused;
		std::get<xAOD::ObserverToolIndex::numPseudo>(trk_map->at(uid)) = numPseudo;
		std::get<xAOD::ObserverToolIndex::averageSplit1>(trk_map->at(uid)) = averageSplit1;
		std::get<xAOD::ObserverToolIndex::averageSplit2>(trk_map->at(uid)) = averageSplit2;
		std::get<xAOD::ObserverToolIndex::numWeightedShared>(trk_map->at(uid)) = numWeightedShared;
		ATH_MSG_DEBUG("updateHolesSharedHits: track "<<uid<<" with totalSiHits "<<totalSiHits);
	}
}

void Trk::TrkObserverTool::dumpTrackMap(const ObservedTrackMap* trk_map) const {

	// prints out/dumps all entries in m_observedTrkMap

	std::lock_guard<std::mutex> lock{m_mutex};

	ATH_MSG_INFO ("Dump observedTrkMap (size = " << getNObservedTracks(trk_map) << ")");
	for (auto& itrMap : *trk_map) {
		ATH_MSG_DEBUG("Id: " << itrMap.first);
		ATH_MSG_DEBUG("\tscore:                  " << std::get<xAOD::ObserverToolIndex::score>(itrMap.second));
		ATH_MSG_DEBUG("\trejectStep:             " << std::get<xAOD::ObserverToolIndex::rejectStep>(itrMap.second));
		ATH_MSG_DEBUG("\trejectReason:           " << std::get<xAOD::ObserverToolIndex::rejectReason>(itrMap.second));
		ATH_MSG_DEBUG("\tparentId:               " << std::get<xAOD::ObserverToolIndex::parentId>(itrMap.second));
		ATH_MSG_DEBUG("\tnumPixelHoles:          " << std::get<xAOD::ObserverToolIndex::numPixelHoles>(itrMap.second));
		ATH_MSG_DEBUG("\tnumSCTHoles:            " << std::get<xAOD::ObserverToolIndex::numSCTHoles>(itrMap.second));
		ATH_MSG_DEBUG("\tnumSplitSharedPixel:    " << std::get<xAOD::ObserverToolIndex::numSplitSharedPixel>(itrMap.second));
		ATH_MSG_DEBUG("\tnumSplitSharedSCT:      " << std::get<xAOD::ObserverToolIndex::numSplitSharedSCT>(itrMap.second));
		ATH_MSG_DEBUG("\tnumSharedOrSplit:       " << std::get<xAOD::ObserverToolIndex::numSharedOrSplit>(itrMap.second));
		ATH_MSG_DEBUG("\tnumSharedOrSplitPixels: " << std::get<xAOD::ObserverToolIndex::numSharedOrSplitPixels>(itrMap.second));
		ATH_MSG_DEBUG("\tnumShared:              " << std::get<xAOD::ObserverToolIndex::numShared>(itrMap.second));
		ATH_MSG_DEBUG("\tisPatternTrack:         " << std::get<xAOD::ObserverToolIndex::isPatternTrack>(itrMap.second));
		ATH_MSG_DEBUG("\ttotalSiHits:            " << std::get<xAOD::ObserverToolIndex::totalSiHits>(itrMap.second));
		ATH_MSG_DEBUG("\tinROI:                  " << std::get<xAOD::ObserverToolIndex::inROI>(itrMap.second));
		ATH_MSG_DEBUG("\thasIBLHit:              " << std::get<xAOD::ObserverToolIndex::hasIBLHit>(itrMap.second));
		ATH_MSG_DEBUG("\thasSharedIBLHit:        " << std::get<xAOD::ObserverToolIndex::hasSharedIBLHit>(itrMap.second));
		ATH_MSG_DEBUG("\thasSharedPixel:         " << std::get<xAOD::ObserverToolIndex::hasSharedPixel>(itrMap.second));
		ATH_MSG_DEBUG("\tfirstPixIsShared:       " << std::get<xAOD::ObserverToolIndex::firstPixIsShared>(itrMap.second));
		ATH_MSG_DEBUG("\tnumPixelDeadSensor:     " << std::get<xAOD::ObserverToolIndex::numPixelDeadSensor>(itrMap.second));
		ATH_MSG_DEBUG("\tnumSCTDeadSensor:       " << std::get<xAOD::ObserverToolIndex::numSCTDeadSensor>(itrMap.second));
		ATH_MSG_DEBUG("\tnumPixelHits:           " << std::get<xAOD::ObserverToolIndex::numPixelHits>(itrMap.second));
		ATH_MSG_DEBUG("\tnumSCTHits:             " << std::get<xAOD::ObserverToolIndex::numSCTHits>(itrMap.second));
		ATH_MSG_DEBUG("\tnumUnused:              " << std::get<xAOD::ObserverToolIndex::numUnused>(itrMap.second));
		ATH_MSG_DEBUG("\tnumTRT_Unused:          " << std::get<xAOD::ObserverToolIndex::numTRT_Unused>(itrMap.second));
		ATH_MSG_DEBUG("\tnumSCT_Unused:          " << std::get<xAOD::ObserverToolIndex::numSCT_Unused>(itrMap.second));
		ATH_MSG_DEBUG("\tnumPseudo:              " << std::get<xAOD::ObserverToolIndex::numPseudo>(itrMap.second));
		ATH_MSG_DEBUG("\taverageSplit1:          " << std::get<xAOD::ObserverToolIndex::averageSplit1>(itrMap.second));
		ATH_MSG_DEBUG("\taverageSplit2:          " << std::get<xAOD::ObserverToolIndex::averageSplit2>(itrMap.second));
		ATH_MSG_DEBUG("\tnumWeightedShared:      " << std::get<xAOD::ObserverToolIndex::numWeightedShared>(itrMap.second));
	}
	ATH_MSG_DEBUG("Number of RejectionReason = acceptedTrack (should equal final tracks): " << getNFinalTracks(trk_map));
}

std::string Trk::TrkObserverTool::dumpRejection(xAOD::RejectionStep rejectStep, xAOD::RejectionReason rejectReason) const {
	// Generate rejection description
	std::string rejection_step = "";
	std::string rejection_reason = "";
	std::string rejection_description = "";
	if ( m_rejectStep_descriptions.find(rejectStep) == m_rejectStep_descriptions.end() ) {
		// not found
		rejection_step = "REJECTION STEP DESCRIPTION NOT FOUND: " + std::to_string(rejectStep);
	}
	else {
		// found
		rejection_step = m_rejectStep_descriptions[rejectStep];
	}
	if ( m_rejectReason_descriptions.find(rejectReason) == m_rejectReason_descriptions.end() ) {
		// not found
		rejection_reason = "REJECTION REASON DESCRIPTION NOT FOUND: " + std::to_string(rejectReason);
	}
	else {
		// found
		rejection_reason = m_rejectReason_descriptions[rejectReason];
	}
	rejection_description = rejection_step + " (" + rejection_reason + ")";
	return rejection_description;
}

int Trk::TrkObserverTool::getNFinalTracks(const ObservedTrackMap* trk_map) const {
	// counts the tracks which did not get rejected (this number should equal finalTracks)
	int nFinalTracks = 0;
	for (auto& itrMap : *trk_map) {
		if (std::get<xAOD::ObserverToolIndex::rejectReason>(itrMap.second) == xAOD::RejectionReason::acceptedTrack) nFinalTracks++;
	}
	return nFinalTracks;
}

int Trk::TrkObserverTool::getNObservedTracks(const ObservedTrackMap* trk_map) const {
	// check the number of tracks in the observer tool map
	return trk_map->size();
}

std::map<xAOD::RejectionStep, std::string> Trk::TrkObserverTool::m_rejectStep_descriptions = {
	{xAOD::RejectionStep::addNewTracks, "DenseEnvironmentsAmbiguityScoreProcessorTool::addNewTracks"},
	{xAOD::RejectionStep::refitTrack, "AmbiguityProcessorBase::refitTrack"},
	{xAOD::RejectionStep::addTrack, "AmbiguityProcessorBase::addTrack"},
	{xAOD::RejectionStep::decideWhichHitsToKeep, "InDetDenseEnvAmbiTrackSelectionTool::decideWhichHitsToKeep"},
	{xAOD::RejectionStep::getCleanedOutTrack, "InDetDenseEnvAmbiTrackSelectionTool::getCleanedOutTrack"}
};

std::map<xAOD::RejectionReason, std::string> Trk::TrkObserverTool::m_rejectReason_descriptions = {
	{xAOD::RejectionReason::acceptedTrack, "not actually a rejection; accepted track"},
	// Reason for rejection within DenseEnvironmentsAmbiguityProcessorTool::solveTracks
	{xAOD::RejectionReason::stillBeingProcessed, "not actually a rejection; track still being processed"},
	// Reason for rejection within DenseEnvironmentsAmbiguityScoreProcessorTool::addNewTracks
	{xAOD::RejectionReason::trackScoreZero, "track score is zero"},
	{xAOD::RejectionReason::duplicateTrack, "duplicate track"},
	// Reason for rejection within AmbiguityProcessorBase::refitTrack
	{xAOD::RejectionReason::subtrackCreated, "not actually a rejection; refit successful, new subtrack created"},
	{xAOD::RejectionReason::refitFailed, "refit failed"},
	// Reason for rejection within AmbiguityProcessorBase::addTrack
	{xAOD::RejectionReason::bremRefitFailed, "brem refit failed"},
	{xAOD::RejectionReason::bremRefitSubtrackCreated, "not actually a rejection; brem refit successful, new subtrack created"},
	{xAOD::RejectionReason::bremRefitTrackScoreZero, "brem refit still gave track score zero"},
	{xAOD::RejectionReason::refitTrackScoreZero, "refit track score 0"},
	// Reason for rejection within decideWhichHitsToKeep
	{xAOD::RejectionReason::TSOSRejectedHit, "TSOS of type RejectedHit"},
	{xAOD::RejectionReason::TSOSOutlierShared, "TSOS of type Outlier and shared"},
	{xAOD::RejectionReason::pixelSplitButTooManyShared2Ptc, "pixel split but shared between too many tracks (2 particle cluster)"},
	{xAOD::RejectionReason::pixelSplitButTooManyShared3Ptc, "pixel split but shared between too many tracks (3+ particle cluster)"},
	{xAOD::RejectionReason::tooManySharedRecoverable, "too many shared hits (recoverable shared hits)"},
	{xAOD::RejectionReason::tooManySharedNonRecoverable, "too many shared hits (all other non-recoverable shared hits)"},
	{xAOD::RejectionReason::sharedSCT, "shared SCT hits when we don't really have enough to share"},
	{xAOD::RejectionReason::sharedHitsBadChi2, "shared hits cause a bad chi2 track"},
	{xAOD::RejectionReason::sharedHitsNotEnoughUniqueHits, "shared hits does not leave enough unique hits on accepted track"},
	{xAOD::RejectionReason::firstHitSharedAndPixIBL, "first hit is shared and either IBL or pixel"},
	{xAOD::RejectionReason::firstHitSharedAndExtraShared, "track with first hit shared had additional shared hit"},
	{xAOD::RejectionReason::sharedHitsNotEnoughUniqueSiHits, "shared hits does not leave enough Si hits on accepted track"},
	{xAOD::RejectionReason::sharedIBLSharedWithNoIBLTrack, "shared IBL hit is shared with accepted track that does not have an IBL hit"},
	{xAOD::RejectionReason::sharedPixelSharedWithDifferentIBLTrack, "shared pixel hit is shared with accepted track that has different IBL hit content"},
	{xAOD::RejectionReason::tooManySharedAfterIncreasingShared, "too many shared hits after increasing shared hits"},
	// Reason for rejection within getCleanedOutTrack
	{xAOD::RejectionReason::notEnoughSiHits, "not enough Si hits"},
	{xAOD::RejectionReason::notEnoughTRTHits, "not enough TRT hits"},
	{xAOD::RejectionReason::notEnoughUniqueSiHits, "not enough unique Si hits"},
	{xAOD::RejectionReason::tooFewHits, "too few hits, reject track with shared hits"},
	{xAOD::RejectionReason::failedSubtrackCreation, "failed to create subtrack"},
	{xAOD::RejectionReason::subtrackCreatedWithRecoveredShared, "created subtrack with recovered shared hits"},
	{xAOD::RejectionReason::other, "other"}
};