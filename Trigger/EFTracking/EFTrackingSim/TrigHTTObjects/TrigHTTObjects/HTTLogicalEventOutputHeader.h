// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef TRIGHTTOBJECTS_HTTLOGICALEVENTOUTPUTHEADER_H
#define TRIGHTTOBJECTS_HTTLOGICALEVENTOUTPUTHEADER_H

#include "TrigHTTObjects/HTTDataFlowInfo.h"
#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTTrack.h"
#include "TrigHTTObjects/HTTTypes.h"
#include <TObject.h>

class HTTLogicalEventOutputHeader : public TObject {
public:

    HTTLogicalEventOutputHeader() {};
    virtual ~HTTLogicalEventOutputHeader();

    void reset(); //reset per event variables

    // First Stage HTT Roads
    void getHTTRoads_1st(std::vector<HTTRoad*>& roads_1st) { roads_1st.reserve(m_HTTRoads_1st.size()); for (auto& r : m_HTTRoads_1st) roads_1st.push_back(&r); }
    size_t nHTTRoads_1st() const { return m_HTTRoads_1st.size(); }
    void reserveHTTRoads_1st(size_t size) { m_HTTRoads_1st.reserve(size); }
    void addHTTRoads_1st(std::vector<HTTRoad*> const& roads_1st) { for (auto& r : roads_1st) m_HTTRoads_1st.push_back(*r); }

    // Second Stage HTT Roads
    void getHTTRoads_2nd(std::vector<HTTRoad*>& roads_2nd) { roads_2nd.reserve(m_HTTRoads_2nd.size()); for (auto& r : m_HTTRoads_2nd) roads_2nd.push_back(&r); }
    size_t nHTTRoads_2nd() const { return m_HTTRoads_2nd.size(); }
    void reserveHTTRoads_2nd(size_t size) { m_HTTRoads_2nd.reserve(size); }
    void addHTTRoads_2nd(std::vector<HTTRoad*> const& roads_2nd) { for (auto& r : roads_2nd) m_HTTRoads_2nd.push_back(*r); }

    // First Stage HTT Tracks
    std::vector<HTTTrack> const& getHTTTracks_1st() const { return m_HTTTracks_1st; }
    size_t nHTTTracks_1st() const { return m_HTTTracks_1st.size(); }
    void reserveHTTTracks_1st(size_t size) { m_HTTTracks_1st.reserve(size); }
    void addHTTTracks_1st(std::vector<HTTTrack> const& tracks_1st) { m_HTTTracks_1st = tracks_1st; }

    // Second Stage HTT Tracks
    std::vector<HTTTrack> const& getHTTTracks_2nd() const { return m_HTTTracks_2nd; }
    size_t nHTTTracks_2nd() const { return m_HTTTracks_2nd.size(); }
    void reserveHTTTracks_2nd(size_t size) { m_HTTTracks_2nd.reserve(size); }
    void addHTTTracks_2nd(std::vector<HTTTrack> const& tracks_2nd) { m_HTTTracks_2nd = tracks_2nd; }

    // Data Flow Information
    HTTDataFlowInfo const& getDataFlowInfo() const { return m_dataflowInfo; }
    void setDataFlowInfo(HTTDataFlowInfo const& info) { m_dataflowInfo = info; }

private:

    std::vector<HTTRoad>        m_HTTRoads_1st;
    std::vector<HTTRoad>        m_HTTRoads_2nd;
    std::vector<HTTTrack>       m_HTTTracks_1st;
    std::vector<HTTTrack>       m_HTTTracks_2nd;

    HTTDataFlowInfo             m_dataflowInfo;

    ClassDef(HTTLogicalEventOutputHeader, 4)
};

std::ostream& operator<<(std::ostream& s, HTTLogicalEventOutputHeader const& h);

#endif // HTTEVENTOUTPUTHEADER_H
