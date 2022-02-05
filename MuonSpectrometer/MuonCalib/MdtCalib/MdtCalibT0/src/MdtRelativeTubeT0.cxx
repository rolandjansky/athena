/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibT0/MdtRelativeTubeT0.h"

// MuonCalibIdentifier
#include "MuonCalibIdentifier/MuonFixedId.h"

// MuonCalibEventBase
#include "MuonCalibEventBase/MdtCalibHitBase.h"

namespace MuonCalib {

    inline unsigned int get_group_id(const MuonFixedId &id, const MdtRelativeTubeT0::TubeGroup &grp);

    void MdtRelativeTubeT0::AddHit(const MdtCalibHitBase &hit) {
        const MuonFixedId &id = hit.identify();
        if (m_tube_t0.find(id) == m_tube_t0.end()) {
            if (m_relative_offset.size()) m_relative_offset.clear();
            m_tube_t0[id] = hit.tubeT0();
        }
    }

    double MdtRelativeTubeT0 ::GetRelativeOffset(const MuonFixedId &id, TubeGroup grp) {
        if (grp == UNKNOWN) return 0;
        std::map<TubeGroup, std::map<MuonFixedId, double> >::const_iterator it = m_relative_offset.find(grp);
        if (it == m_relative_offset.end()) {
            calculate_relative_t0s(grp);
            return GetRelativeOffset(id, grp);
        }
        std::map<MuonFixedId, double>::const_iterator it2 = it->second.find(id);
        if (it2 == it->second.end()) { return 0; }
        return it2->second;
    }

    inline void MdtRelativeTubeT0 ::calculate_relative_t0s(const TubeGroup &grp) {
        // calculate mean t0 per group
        std::map<unsigned int, std::pair<double, int> > mean_t0;
        for (std::map<MuonFixedId, double>::const_iterator it = m_tube_t0.begin(); it != m_tube_t0.end(); it++) {
            unsigned int grp_id(get_group_id(it->first, grp));
            std::map<unsigned int, std::pair<double, int> >::iterator it2 = mean_t0.find(grp_id);
            if (it2 == mean_t0.end()) {
                mean_t0[grp_id] = std::pair<double, int>(it->second, 1.);
            } else {
                it2->second.first += it->second;
                it2->second.second++;
            }
        }
        for (std::map<unsigned int, std::pair<double, int> >::iterator it = mean_t0.begin(); it != mean_t0.end(); it++) {
            it->second.first /= it->second.second;
        }

        // calculate tube offsets
        std::map<MuonFixedId, double> &offsets(m_relative_offset[grp]);
        offsets.clear();
        for (std::map<MuonFixedId, double>::const_iterator it = m_tube_t0.begin(); it != m_tube_t0.end(); it++) {
            unsigned int grp_id(get_group_id(it->first, grp));
            offsets[it->first] = it->second - mean_t0[grp_id].first;
        }
    }

    inline unsigned int get_group_id(const MuonFixedId &id, const MdtRelativeTubeT0::TubeGroup &grp) {
        switch (grp) {
            case MdtRelativeTubeT0::CHAMBER: return id.mdtChamberId().getIdInt(); break;
            case MdtRelativeTubeT0::MULTILAYER: return id.mdtMultilayerId().getIdInt(); break;
            case MdtRelativeTubeT0::LAYER: return 4 * (id.mdtMultilayer() - 1) + (id.mdtTubeLayer() - 1); break;
            case MdtRelativeTubeT0::MEZZ_CARD: return id.mdtMezzanine(); break;
            case MdtRelativeTubeT0::UNKNOWN: return 0;
        }
        return 0;
    }

}  // namespace MuonCalib
