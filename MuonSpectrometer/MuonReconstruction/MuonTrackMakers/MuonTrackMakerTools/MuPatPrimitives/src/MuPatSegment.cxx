/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuPatPrimitives/MuPatSegment.h"

#include <iostream>
#define DUMP_MCTB_OBJECT_POINTERS
namespace Muon {

    // Static members
    unsigned int MuPatSegment::s_maxNumberOfInstantiations = 0;
    unsigned int MuPatSegment::s_numberOfInstantiations = 0;
    unsigned int MuPatSegment::s_numberOfCopies = 0;

    MuPatSegment::MuPatSegment() {
#ifdef MCTB_OBJECT_COUNTERS
        addInstance();
#endif
#ifdef MCTB_OBJECT_POINTERS
        std::cout << " new MuPatSegment() " << this << " , instances " << s_numberOfInstantiations << std::endl;
#endif
    }

    MuPatSegment::~MuPatSegment() {
#ifdef MCTB_OBJECT_COUNTERS
        removeInstance();
#endif
#ifdef MCTB_OBJECT_POINTERS
        std::cout << " dtor MuPatSegment() " << this << " , instances " << s_numberOfInstantiations << std::endl;
#endif
    }

    MuPatSegment::MuPatSegment(const MuPatSegment& info) : MuPatCandidateBase(info) {
        quality = info.quality;
        segQuality = info.segQuality;
        segment = info.segment;
        segPars = info.segPars;
        segmentIndex = info.segmentIndex;
        usedInFit = info.usedInFit;
        chid = info.chid;
        name = info.name;
        chIndex = info.chIndex;
        stIndex = info.stIndex;
        isEndcap = info.isEndcap;
        isMdt = info.isMdt;
#ifdef MCTB_OBJECT_COUNTERS
        addInstance();
        ++s_numberOfCopies;
#endif
#ifdef MCTB_OBJECT_POINTERS
        std::cout << " ctor MuPatSegment " << this << " , instances " << s_numberOfInstantiations << std::endl;
#endif
    }

    MuPatSegment& MuPatSegment::operator=(const MuPatSegment& info) {
        if (&info != this) {
            MuPatCandidateBase::operator=(info);
            quality = info.quality;
            segQuality = info.segQuality;
            segment = info.segment;
            segPars = info.segPars;
            segmentIndex = info.segmentIndex;
            usedInFit = info.usedInFit;
            chid = info.chid;
            name = info.name;
            chIndex = info.chIndex;
            stIndex = info.stIndex;
            isEndcap = info.isEndcap;
            isMdt = info.isMdt;

#ifdef MCTB_OBJECT_COUNTERS
            ++s_numberOfCopies;
#endif
        }
        return *this;
    }

    void MuPatSegment::addTrack(MuPatTrack* track) {
#ifdef MCTB_OBJECT_POINTERS
        std::cout << " addTrack " << this << " track " << track;
        if (m_associatedTracks.count(track)) std::cout << " DOUBLE assocation!! " << std::endl;
        std::cout << std::endl;
#endif
        m_associatedTracks.insert(track);
    }

    void MuPatSegment::removeTrack(MuPatTrack* track) {
        // look up track
        std::set<MuPatTrack*>::iterator pos = m_associatedTracks.find(track);
        if (pos != m_associatedTracks.end()) {
            // if found remove it from list
            m_associatedTracks.erase(pos);
#ifdef MCTB_OBJECT_POINTERS
            std::cout << " removeTrack " << this << " track " << track << std::endl;
#endif
        } else {
#ifdef MCTB_OBJECT_POINTERS
            std::cout << " removeTrack: TRACK NOT FOUND " << this << " track " << track << std::endl;
#endif
        }
    }

}  // namespace Muon
