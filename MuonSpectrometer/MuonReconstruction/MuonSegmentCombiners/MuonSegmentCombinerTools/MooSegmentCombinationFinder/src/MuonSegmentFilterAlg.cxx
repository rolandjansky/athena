/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentFilterAlg.h"
#include "TrkSurfaces/Surface.h"
MuonSegmentFilterAlg::MuonSegmentFilterAlg(const std::string& name, ISvcLocator* pSvcLocator) : AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode MuonSegmentFilterAlg::initialize() {
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_outKey.initialize());

    ATH_CHECK(m_inKey.initialize());

    return StatusCode::SUCCESS;
}

StatusCode MuonSegmentFilterAlg::execute(const EventContext& ctx) const {
    
    SG::ReadHandle<Trk::SegmentCollection> segment_container(m_inKey, ctx);
    if (!segment_container.isValid()){
        ATH_MSG_FATAL("Failed to retrieve segment container "<<m_inKey.fullKey());
    }

    
    SG::WriteHandle<ConstDataVector<Trk::SegmentCollection>> out_handle{m_outKey, ctx};
    ATH_CHECK(out_handle.record(std::make_unique<ConstDataVector<Trk::SegmentCollection>>(SG::VIEW_ELEMENTS)));
    for (const Trk::Segment* seg : *segment_container){
        if (keep_segment(seg)) out_handle->push_back(seg);
    }
    if (m_trash_unfiltered && !segment_container->empty() && out_handle->size() == segment_container->size()){
        ATH_MSG_DEBUG("The input and output container are the same. Clear output container");
        out_handle->clear();
    }

    

    return StatusCode::SUCCESS;
}
bool MuonSegmentFilterAlg::keep_segment(const Trk::Segment* segment) const {
    for (const Trk::MeasurementBase* meas : segment->containedMeasurements()){
         /// First find the identifier
         const Identifier id = meas->associatedSurface().associatedDetectorElementIdentifier();        
         if (!m_thin_stations.empty()){
              const StIdx::StIndex  stationIndex = m_idHelperSvc->stationIndex(id);
              if (std::find_if(m_thin_stations.begin(), m_thin_stations.end(), [stationIndex](const int idx){
                    return idx == stationIndex;
              }) != m_thin_stations.end() ) return false;
         }
         if (!m_thin_layers.empty()){
             const StIdx::LayerIndex  layerIndex = m_idHelperSvc->layerIndex(id);
             if (std::find_if(m_thin_layers.begin(),m_thin_layers.end(),[layerIndex](const int idx)
                        {return idx == layerIndex;})!= m_thin_layers.end()) return false;
         }
         if (!m_thin_technology.empty()){
             const StIdx::TechnologyIndex technologyIndex = m_idHelperSvc->technologyIndex(id);
             if (std::find_if(m_thin_technology.begin(),m_thin_technology.end(),[technologyIndex](const int idx ){
                return idx == technologyIndex;
             })!= m_thin_technology.end()) return false;
         }
         if (!m_thin_region.empty()){
             const StIdx::DetectorRegionIndex regionIndex = m_idHelperSvc->regionIndex(id);
             if (std::find_if(m_thin_region.begin(),m_thin_region.end(),[regionIndex](const int idx){
                    return regionIndex == idx;
             }) != m_thin_region.end() ) return false;
         }
         ///
         if (!m_thin_chamber_idx.empty()){
             const StIdx::ChIndex  chamberIdx = m_idHelperSvc->chamberIndex(id);
             if (std::find_if(m_thin_chamber_idx.begin(),m_thin_chamber_idx.end(),[chamberIdx](const int idx){
                    return idx == chamberIdx;}) != m_thin_chamber_idx.end()) return false;
         }        
    }
    return true;    
}