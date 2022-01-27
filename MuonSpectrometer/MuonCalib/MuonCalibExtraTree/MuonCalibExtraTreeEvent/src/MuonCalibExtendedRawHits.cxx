/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibExtendedRawHits.h"

#include <sstream>

#include "MuonCalibEventBase/MuonCalibPattern.h"
#include "MuonCalibEventBase/MuonCalibRawCscHit.h"
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MuonCalibRawRpcHit.h"
#include "MuonCalibEventBase/MuonCalibRawTgcHit.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedSegment.h"
#include "MuonCalibExtraTreeEvent/MuonCalibExtendedTrack.h"
#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"
#include "MuonCalibExtraUtils/MuonFixedIdManipulator.h"
#include "MuonCalibExtraUtils/MuonFixedIdPrinter.h"

namespace MuonCalib {

    MuonCalibExtendedRawHits::MuonCalibExtendedRawHits() : m_adcThreshold(50.) {}

    MuonCalibExtendedRawHits::MuonCalibExtendedRawHits(const MuonCalibRawHitCollection& rawHits,
                                                       const std::vector<ExtendedSegPtr>& segments,
                                                       const std::vector<ExtendedTrkPtr>& tracks) :
        m_adcThreshold(70.) {
        MuonFixedIdManipulator idManip;
        // loop over MDT raw hits and
        for (const MdtCalibRawHitPtr& rawHit : rawHits.MdtContainer()) {
            const MuonFixedId id = rawHit->identify();
            // create ChamberId + StationIndex
            MuonFixedId chId = idManip.chamberIdentifier(id);
            MuonFixedIdManipulator::StationIndex stationIndex = idManip.stationLayerIndex(id);

            MdtRegionInfo& chInfo = m_mdtChamberIdMap[chId];
            ExtendedChannelInfo& channelChInfo = chInfo.hits[id];
            MdtRegionInfo& stInfo = m_mdtChamberIndexMap[stationIndex];
            ExtendedChannelInfo& channelStInfo = stInfo.hits[id];

            channelChInfo.hits.emplace_back(rawHit);
            channelStInfo.hits.emplace_back(rawHit);

            const bool above_thresh = {rawHit->adcCount() > m_adcThreshold};
            chInfo.nhitsAboveAdcThreshold += above_thresh;
            stInfo.nhitsAboveAdcThreshold += above_thresh;

            // only increase the hit counters once
            bool isFirst = true;
            // loop over tracks and check whether hit is 'on-track'
            for (const ExtendedTrkPtr& trk : tracks) {
                // look for id in map
                const IdHitMap& idHitMap = trk->idHitMap();
                IdHitMap::const_iterator pos = idHitMap.find(id);
                if (pos == idHitMap.end()) continue;

                // if we get here the hit was found, now check that it is a hit on track
                const MuonCalibHit_E& hit = *pos->second;
                if (hit.type() < 4) {
                    if (isFirst) {
                        ++chInfo.nhitsOnTrack;
                        ++stInfo.nhitsOnTrack;
                        isFirst = false;
                    }
                    trk->rawHitAssociationMap().addMdtRawHit(rawHit);
                    channelChInfo.associatedTracks.emplace_back(trk);
                    channelStInfo.associatedTracks.emplace_back(trk);
                }
            }

            // only increase the hit counters once
            isFirst = true;
            // loop over segments and check whether hit is 'on-segment'
            for (const ExtendedSegPtr& seg : segments) {
                // look for id in segment
                if (!seg->idSet().count(id)) continue;

                if (isFirst) {
                    ++chInfo.nhitsOnSegment;
                    ++stInfo.nhitsOnSegment;
                    isFirst = false;
                }
                seg->rawHitAssociationMap().addMdtRawHit(rawHit);
                channelChInfo.associatedSegments.emplace_back(seg);
                channelStInfo.associatedSegments.emplace_back(seg);
            }
        }
    }

    std::string MuonCalibExtendedRawHits::dumpMdtHits() const {
        std::ostringstream sout;
        MuonFixedIdPrinter printer{};

        sout << " number of MDT chambers with hits: " << m_mdtChamberIdMap.size() << std::endl;
        for (const auto& chIt : m_mdtChamberIdMap) {
            if (chIt.second.nhitsAboveAdcThreshold < 3) continue;
            const MuonFixedId& chId = chIt.first;
            sout << "    " << std::setw(20) << printer.chamberIdentifier(chId) << "    nhits " << std::setw(3) << chIt.second.hits.size()
                 << "    above ADC cut " << std::setw(3) << chIt.second.nhitsAboveAdcThreshold << "    on segment " << std::setw(3)
                 << chIt.second.nhitsOnSegment << "    on track " << std::setw(3) << chIt.second.nhitsOnTrack;

            // look at next iterator to decide whether to add endl or not
            sout << std::endl;
        }
        return sout.str();
    }
}  // namespace MuonCalib
