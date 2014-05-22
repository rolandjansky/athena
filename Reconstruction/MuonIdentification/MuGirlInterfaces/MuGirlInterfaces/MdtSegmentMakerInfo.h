/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLNSMDTSEGMENTMAKERINFO_H
#define MUGIRLNSMDTSEGMENTMAKERINFO_H

namespace Muon
{
class MdtDriftCircleOnTrack;
}

namespace Trk
{
class TrackRoad;
}

namespace MuGirlNS
{
struct MdtSegmentMakerInfo
{
    typedef std::vector<const Muon::MdtDriftCircleOnTrack*> MDCOTList;
    typedef std::vector<MDCOTList> MDCOTLists;

    const Trk::TrackRoad* pRoad;
    MDCOTLists mdts;

    MdtSegmentMakerInfo(const Trk::TrackRoad* pRoad, const MDCOTLists& in_mdts) : pRoad(pRoad)
    {
        for (MDCOTLists::const_iterator it = in_mdts.begin(); it != in_mdts.end(); it++)
        {
            MDCOTList v;
            v.insert(v.end(), it->begin(), it->end());
            mdts.push_back(v);
        }
    }
};

typedef std::vector<MdtSegmentMakerInfo*> MdtSegmentMakerInfoList;
}

#endif // MUGIRLNSMDTSEGMENTMAKERINFO_H
