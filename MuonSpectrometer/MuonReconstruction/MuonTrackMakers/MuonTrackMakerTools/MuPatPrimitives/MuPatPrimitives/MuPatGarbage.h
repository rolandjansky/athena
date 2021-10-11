/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUPATGARBAGE_H
#define MUPATGARBAGE_H

#include <memory>
#include <set>

#include "MuPatPrimitives/MuPatHit.h"
#include "MuPatPrimitives/MuPatSegment.h"
#include "TrkMeasurementBase/MeasurementBase.h"
namespace Muon {
    /// Simple helbper class to take care of the deletion of memory to be trashed
    class GarbageContainer final {
    public:
        GarbageContainer() = default;
        ~GarbageContainer() = default;

        void push_back(const MuPatSegment* segment);
        void push_back(const MuonSegment* segment);
        void push_back(const MuPatHit* hit);
        void push_back(const Trk::MeasurementBase* meas);
        void push_back(const Trk::Perigee* perigee);

        void push_back(std::unique_ptr<const MuPatSegment> segment);
        void push_back(std::unique_ptr<const MuonSegment> segment);
        void push_back(std::unique_ptr<const MuPatHit> hit);
        void push_back(std::unique_ptr<const Trk::MeasurementBase> meas);
        void push_back(std::unique_ptr<const Trk::Perigee> perigee);

    private:
        template <class T> void push_back(const T* element, std::set<std::unique_ptr<const T>>& container) {
            typename std::set<std::unique_ptr<const T>>::const_iterator itr = std::find_if(
                container.begin(), container.end(), [element](const std::unique_ptr<const T>& holder) { return holder.get() == element; });
            if (itr == container.end()) { container.emplace(element); }
        }
        template <class T> void push_back(std::unique_ptr<const T>& element, std::set<std::unique_ptr<const T>>& container) {
            typename std::set<std::unique_ptr<const T>>::const_iterator itr =
                std::find_if(container.begin(), container.end(),
                             [&element](const std::unique_ptr<const T>& holder) { return holder.get() == element.get(); });
            if (itr == container.end()) {
                container.emplace(std::move(element));
            }
            /// actually this should never happen
            else
                element.release();
        }

        std::set<std::unique_ptr<const MuPatSegment>> m_pat_segments;
        std::set<std::unique_ptr<const MuonSegment>> m_muon_segments;
        std::set<std::unique_ptr<const MuPatHit>> m_hits;
        std::set<std::unique_ptr<const Trk::MeasurementBase>> m_measurements;
        std::set<std::unique_ptr<const Trk::Perigee>> m_perigees;
    };

}  // namespace Muon

#endif
