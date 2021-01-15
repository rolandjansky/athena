// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef HTTLOGICALEVENTOUTPUTHEADER_H
#define HTTLOGICALEVENTOUTPUTHEADER_H

#include <TObject.h>

#include <vector>
#include <iostream>
#include <sstream>

#include "TrigHTTObjects/HTTTruthTrack.h"
#include "TrigHTTObjects/HTTOfflineTrack.h"
#include "TrigHTTObjects/HTTTrack.h"

class HTTLogicalEventOutputHeader: public TObject
{
    public:

        HTTLogicalEventOutputHeader() {};
        virtual ~HTTLogicalEventOutputHeader();

        void reset(); //reset per event variables

        // Truth Tracks
        const std::vector<HTTTruthTrack>& getTruthTracks()  const { return m_TruthTrack; }
        int  nTruthTracks()  const { return m_TruthTrack.size(); }  
        void addTruthTrack(HTTTruthTrack t) { m_TruthTrack.push_back(t); };

        // Offline Tracks
        const std::vector<HTTOfflineTrack>&  getOfflineTracks()  const { return m_OfflineTrack; }
        int  nOfflineTracks()  const { return m_OfflineTrack.size(); }
        void addOfflineTrack(HTTOfflineTrack t) { m_OfflineTrack.push_back(t); };

        // HTT Tracks
        const std::vector<HTTTrack>&  getHTTTracks()  const { return m_HTTTrack; }
        int  nHTTTracks()  const { return m_HTTTrack.size(); }
        void addHTTTrack(HTTTrack t) { m_HTTTrack.push_back(t); };

    private:

        std::vector<HTTTruthTrack>   m_TruthTrack;
        std::vector<HTTOfflineTrack> m_OfflineTrack;
        std::vector<HTTTrack>        m_HTTTrack;

        ClassDef(HTTLogicalEventOutputHeader, 1)
};

std::ostream& operator<<(std::ostream&, const HTTLogicalEventOutputHeader&);

#endif // HTTEVENTOUTPUTHEADER_H
