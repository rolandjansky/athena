/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MOOSEGMENTFINDERS_MUONSEGMENTFILTERALG_H
#define MOOSEGMENTFINDERS_MUONSEGMENTFILTERALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "TrkSegment/Segment.h"
#include "TrkSegment/SegmentCollection.h"

#include "AthContainers/ConstDataVector.h"

///  The muon segment filter alg thins all segments coming from a certain detector technology,
///  or in a certain region of the muon spectrometer, e.g. BI, BIS, Barrel, Middle, etc... 
///  The pointers of the TrackSegements passing the filter are added to a SG::VIEW_ELEMENTS container.
class MuonSegmentFilterAlg : public AthReentrantAlgorithm {
public:
    MuonSegmentFilterAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual ~MuonSegmentFilterAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

private:
    bool keep_segment(const Trk::Segment* segment) const;
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
        this,
        "MuonIdHelperSvc",
        "Muon::MuonIdHelperSvc/MuonIdHelperSvc",
    };
    // the following Trk::SegmentCollection MuonSegments are standard MuonSegments, the MuGirl segments are stored in MuonCreatorAlg.h
    SG::WriteHandleKey<ConstDataVector<Trk::SegmentCollection>> m_outKey{
        this,
        "FilteredCollectionName",
        "FilteredMuonSegments",
        "Output container",
    };
    SG::ReadHandleKey<Trk::SegmentCollection> m_inKey{
        this,
        "SegmentCollectionName",
        "TrackMuonSegments",
        "Input container",
    };

    
  
    using  StIdx = Muon::MuonStationIndex;

    Gaudi::Property<std::vector<int>> m_thin_stations{this, "ThinStations", {StIdx::BI, StIdx::BM, StIdx::BO, StIdx::BE, StIdx::EI}, "Removes sgements in a given Muon station" };
    Gaudi::Property<std::vector<int>> m_thin_layers{this, "ThinLayers", {}, "Removes segments in a given layer of the MuonSpectrometer "};
    Gaudi::Property<std::vector<int>> m_thin_technology{this, "ThinTechnology", {StIdx::STGC, StIdx::MM}, "Removes segments from a given chamber technology"};
    Gaudi::Property<std::vector<int>> m_thin_region{this, "ThinDetRegion", {}, "Removes all segments form Barrel / EndcapA / EndcapC"};
    Gaudi::Property<std::vector<int>> m_thin_chamber_idx{this, "ThinChamberIndex", {}, "Removes all segments from a BIS/BIL/BMS/.."};
    
    /// If no segments are removed from the container then empty the entire container as the 
    /// algoritihms depending on the filtered container will give the same as the vanilla chain
    Gaudi::Property<bool> m_trash_unfiltered{this, "TrashUnFiltered", true };

};

#endif
