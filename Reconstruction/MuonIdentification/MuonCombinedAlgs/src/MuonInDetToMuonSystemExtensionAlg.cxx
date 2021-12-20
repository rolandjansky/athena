/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonInDetToMuonSystemExtensionAlg.h"
#include "AthContainers/ConstDataVector.h"
#include "MuonDetDescrUtils/MuonSectorMapping.h"
#include "MuonLayerEvent/MuonSystemExtension.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "MuonSegment/MuonSegment.h"
namespace {
    constexpr unsigned int num_sectors = 16;
    using RegionIndex = Muon::MuonStationIndex::DetectorRegionIndex;
    using LayerIndex = Muon::MuonStationIndex::LayerIndex;   
}  // namespace

MuonInDetToMuonSystemExtensionAlg::MuonInDetToMuonSystemExtensionAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode MuonInDetToMuonSystemExtensionAlg::initialize() {
    
    ATH_CHECK(m_muonSystemExtensionTool.retrieve());
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_edmHelperSvc.retrieve());
    ATH_CHECK(m_combTagMap.initialize());
    ATH_CHECK(m_inputCandidate.initialize());
    ATH_CHECK(m_bulkInDetCandKey.initialize());
    ATH_CHECK(m_stauInDetCandKey.initialize(!m_stauInDetCandKey.empty()));
    if (m_restrictExtension) { 
        ATH_MSG_INFO("Use the Hough seeds to determine the sectors in the MS worth for being extrapolated to");
    }
    ATH_CHECK(m_houghDataPerSectorVecKey.initialize(m_restrictExtension)); 
   
    return StatusCode::SUCCESS;
}

StatusCode MuonInDetToMuonSystemExtensionAlg::execute(const EventContext& ctx) const {
    InDetCandidateCache output_cache{};

    SG::ReadHandle<InDetCandidateCollection> input_container{m_inputCandidate, ctx};
    if (!input_container.isValid()){
        ATH_MSG_FATAL("Failed to retrieve "<<m_inputCandidate.fullKey());
        return StatusCode::FAILURE;
    }
    output_cache.input_candidates = input_container.cptr();
    ATH_CHECK(selectCandidates(ctx,output_cache));    
    
    if (m_restrictExtension) { ATH_CHECK(findHittedSectors(ctx, output_cache)); }

    
    ATH_MSG_DEBUG("Find the inner detector candidates to be used for MuGirl / Segment tagging");
    ATH_CHECK(create(ctx,output_cache));
    SG::WriteHandle<InDetCandidateCollection> indetCandidateCollection(m_bulkInDetCandKey, ctx);
    ATH_CHECK(indetCandidateCollection.record(std::move(output_cache.outputContainer)));

    ATH_CHECK(createStaus(ctx, indetCandidateCollection.ptr(), output_cache.tag_map));

    return StatusCode::SUCCESS;
}
StatusCode MuonInDetToMuonSystemExtensionAlg::selectCandidates(const EventContext& ctx, InDetCandidateCache& out_cache) const{
    
    SG::ReadHandle<MuonCombined::InDetCandidateToTagMap> tag_map{m_combTagMap, ctx};
    if(!tag_map.isValid()) {
        ATH_MSG_FATAL("Failed to retrieve the tag map from "<<tag_map.fullKey());
        return StatusCode::FAILURE;
    }   
    if (tag_map->empty()) {
        ATH_MSG_DEBUG("No combined candidates in this event");
        return StatusCode::SUCCESS;
    }

    for (const auto& combined_tags : *tag_map) { 
        if (combined_tags.second->author() != xAOD::Muon::MuidCo) {
            ATH_MSG_WARNING("Found a non MuidCo tag.... Please check "<<combined_tags.second->toString());
            continue;
        }
        const MuonCombined::CombinedFitTag* cmb_tag = dynamic_cast<const MuonCombined::CombinedFitTag*>(combined_tags.second.get());
        MuidCoCache cache{combined_tags.first, cmb_tag};
        /// Need to remember to put the segment retrieval here somehow

        out_cache.tag_map.push_back(std::move(cache));
        out_cache.excluded_trks.insert(combined_tags.first);       
    }
    return StatusCode::SUCCESS;
}
StatusCode MuonInDetToMuonSystemExtensionAlg::create(const EventContext& ctx, InDetCandidateCache& out_cache) const {    
    
    //// Loop over the tag candidates
    for (const MuonCombined::InDetCandidate* candidate : *out_cache.input_candidates) {
        
        if (out_cache.excluded_trks.count(candidate)) {
            ATH_MSG_VERBOSE("Inner detector track "<<candidate->toString()<<" has already been used by MuidCo");
            continue;
        }
        /// We want these tracks to be extrapolated through the spectrometer reject those no meeting
        /// the basic requirements
        if (candidate->indetTrackParticle().pt() < m_extThreshold ||
            (!m_extendSAF && candidate->isSiliconAssociated()) ||
            (!m_extendBulk && !candidate->isSiliconAssociated()) ) {
            ATH_MSG_VERBOSE("Inner detector track "<<candidate->toString()<<" is too soft");
            continue;
        }

        /// Prepare the system extension
        Muon::IMuonSystemExtensionTool::SystemExtensionCache cache;
        cache.candidate = std::make_unique<MuonCombined::InDetCandidate>(candidate->indetTrackParticleLink());
        cache.candidate->setSiliconAssociated(candidate->isSiliconAssociated());
        cache.candidate->setExtension(candidate->getCaloExtension());
        cache.useHitSectors = m_restrictExtension;
        cache.sectorsWithHits = &out_cache.hitted_sectors;
        cache.createSystemExtension = true;
        cache.requireSystemExtension = true; 

        const Muon::MuonSystemExtension* extension = candidate->getExtension(); 
        /// The candidate already has somehow a system extension --> it's just a matter of copying it 
        if (extension) {
            /// The system extension does not own the MuonEntryLayerIntersection()-> it's fine
            std::vector<Muon::MuonSystemExtension::Intersection> intersects = extension->layerIntersections();
            cache.candidate->setExtension(std::make_unique<Muon::MuonSystemExtension>(&extension->muonEntryLayerIntersection(), 
                                            std::move(intersects)));
        } else if (!m_muonSystemExtensionTool->muonSystemExtension(ctx, cache)) {
            ATH_MSG_VERBOSE("Extension failed");
            continue;
        }
        out_cache.outputContainer->push_back(std::move(cache.candidate));
    }
    if (msgLevel(MSG::DEBUG))  {
        std::stringstream sstr;
        for (const MuonCombined::InDetCandidate*  extended :  *out_cache.outputContainer) {
            sstr<<"   * "<<extended->toString()<<std::endl;
        }
        if(!out_cache.excluded_trks.empty()) {
            sstr<<std::endl<<" ======== The following tracks were already successfully combined  ======== "<<std::endl;
            for (const MuonCombined::InDetCandidate* combined : out_cache.excluded_trks) {
                sstr<<"   = "<<combined->toString()<<std::endl;
            }
        }
        if (out_cache.excluded_trks.size() + out_cache.outputContainer->size() != out_cache.input_candidates->size()) {
            sstr<<std::endl<<" ++++++++ The following candidates were not extrapolated  ++++++++  "<<std::endl;           
            const InDetCandidateCollection::const_iterator begin = out_cache.outputContainer->begin();
            const InDetCandidateCollection::const_iterator end = out_cache.outputContainer->end();
            
            for (const MuonCombined::InDetCandidate* rejected : *out_cache.input_candidates) {
                if (out_cache.excluded_trks.count(rejected) || 
                    std::find_if(begin, end, [rejected](const MuonCombined::InDetCandidate* extended) {
                            return (*rejected) == (*extended); 
                } )!= end) continue;
                sstr<<"   + "<<rejected->toString()<<std::endl;
            }
        }        
        ATH_MSG_DEBUG("Extrapolated "<<out_cache.outputContainer->size()<<" out of "
                        <<out_cache.input_candidates->size()<<" tracks through the spectrometer" <<std::endl<<std::endl<<sstr.str());    
    }
    return StatusCode::SUCCESS;
}
    


StatusCode MuonInDetToMuonSystemExtensionAlg::findHittedSectors(const EventContext& ctx,
                                                                InDetCandidateCache& output_cache) const {
    SG::ReadHandle<Muon::HoughDataPerSectorVec> readHandle{m_houghDataPerSectorVecKey, ctx};
    if (!readHandle.isValid()) {
        ATH_MSG_FATAL("Failed to retrieve the prep data container " << m_houghDataPerSectorVecKey.fullKey());
        return StatusCode::FAILURE;
    }
    const std::vector<Muon::HoughDataPerSec>& hough_data = readHandle->vec;
    /// Helper function to cound whether all three stations are actually complete
    auto count_finished = [&output_cache]() -> unsigned int {
        unsigned int n{0};
        for (const auto& sector_hits : output_cache.hitted_sectors) { n += sector_hits.second.size() >= num_sectors; }
        return n;
    };
    /// Helper
    static const Muon::MuonSectorMapping sector_mapping;
    for (const Muon::HoughDataPerSec& sector_data : hough_data) {
        /// The phi-hit vector has a size of 3 representing
        /// the forward-backward and barrel sections
        for (int det_region = 0; det_region < Muon::MuonStationIndex::DetectorRegionIndexMax; ++det_region) {
            const RegionIndex region_index = static_cast<RegionIndex>(det_region);
            for (int layer = 0; layer < Muon::MuonStationIndex::LayerIndexMax; ++layer) {
                const LayerIndex layer_index = static_cast<LayerIndex>(layer);
                const unsigned int hash = Muon::MuonStationIndex::sectorLayerHash(region_index, layer_index);
                const Muon::HoughDataPerSec::MaximumVec& eta_hits = sector_data.maxVec[hash];
                for (const std::shared_ptr<MuonHough::MuonLayerHough::Maximum>& maxima : eta_hits) {
                    if (m_houghMin > maxima->hits.size())  continue;
                    ATH_MSG_VERBOSE("Hough maximum in " <<Muon::MuonStationIndex::regionName(region_index)<<
                                    ", "<<Muon::MuonStationIndex::layerName(layer_index)<<", hits: "<<maxima->hits.size());
                    for (const std::shared_ptr<MuonHough::Hit>& hit : maxima->hits) {
                        const Trk::PrepRawData* prep_data{hit->prd};
                        if (!prep_data && hit->tgc) { prep_data = hit->tgc->phiCluster.hitList.front(); }
                        if (!prep_data) continue;
                        const Amg::Vector3D glob_pos = prep_data->detectorElement()->center(prep_data->identify());
                        output_cache.hitted_sectors[region_index].insert(sector_mapping.getSector(glob_pos.phi()));
                    }
                }
                if (count_finished() >= RegionIndex::DetectorRegionIndexMax) {
                    ATH_MSG_VERBOSE("The MS is filled up with Hough seeds. We do not need to search for them any longer");
                    return StatusCode::SUCCESS;
                }            
            }
        }
    }
    return StatusCode::SUCCESS;
}

StatusCode MuonInDetToMuonSystemExtensionAlg::createStaus(const EventContext& ctx, 
                           const InDetCandidateCollection* ext_candidates,
                           const MuidCoVector& tag_map) const {

    if (m_stauInDetCandKey.empty()) {
        ATH_MSG_VERBOSE("No candidates for stau reconstruction will be written");
        return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Now find the candidates to be considered for MuGirlStau");
    InDetCandidateCache stau_cache{};
    stau_cache.input_candidates = ext_candidates;

    /// This will copy the existing candidates to the new container without calling the system extension tool
    ATH_CHECK(create(ctx,stau_cache));
    /// For the staus it is quite unclear what is the overlap between MuidCo and MuGirlStau. 
    /// So in order to be on the safe side we add the MuidCo candidate tracks as well, but 
    /// we also try to avoid the extrapolator like a cat does for a bubble bath. By taking
    /// the perigee parameters of the associated muon segments, we have a much much shorter path 
    /// to extrapolate the track to the MuonLayerInterSection surfaces.  
    for (const MuidCoCache& idMuidCo : tag_map) {      
        if (idMuidCo.segments.empty()) {
            ATH_MSG_WARNING("No segments were associated with muidCo tag ");
            continue;
        }
        std::unique_ptr<MuonCombined::InDetCandidate> candidate = std::make_unique<MuonCombined::InDetCandidate>(idMuidCo.id_trk->indetTrackParticleLink());
        candidate->setSiliconAssociated(idMuidCo.id_trk->isSiliconAssociated());
        candidate->setExtension(idMuidCo.id_trk->getCaloExtension());
        const xAOD::TrackParticle& id_trk = candidate->indetTrackParticle();
        std::vector<Muon::MuonSystemExtension::Intersection> intersections;
        for (const Muon::MuonSegment* seg : idMuidCo.segments) {
            double momentum{0.};
            std::unique_ptr<const Trk::TrackParameters> pars{m_edmHelperSvc->createTrackParameters(*seg, momentum, id_trk.charge())};
            Muon::MuonSystemExtension::Intersection intersect = m_muonSystemExtensionTool->getInterSection(ctx, *pars);
            if (intersect.trackParameters) intersections.push_back(std::move(intersect));           
        }
        if (intersections.empty()) continue;
        candidate->setExtension(std::make_unique<Muon::MuonSystemExtension>(candidate->getCaloExtension()->muonEntryLayerIntersection(), 
                                            std::move(intersections)));  
        stau_cache.outputContainer->push_back(std::move(candidate));
    }
    
    SG::WriteHandle<InDetCandidateCollection> indetCandidateCollection(m_stauInDetCandKey, ctx);
    ATH_CHECK(indetCandidateCollection.record(std::move(stau_cache.outputContainer)));
    return StatusCode::SUCCESS;
}

    