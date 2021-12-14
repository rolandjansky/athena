/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSYSTEMEXTENSION_H
#define MUON_MUONSYSTEMEXTENSION_H

#include <vector>

#include "MuonLayerEvent/MuonLayerSurface.h"
#include "TrkParameters/TrackParameters.h"

namespace Muon {

    /**
        Tracking class to hold the extrapolation from a particle from the calo entry to the end of muon system
     */
    class MuonSystemExtension {
    public:
        /** data per intersection */
        struct Intersection {
            Intersection()= default;
            Intersection(std::shared_ptr<const Trk::TrackParameters> trackParameters_, const MuonLayerSurface& layerSurface_) :
                trackParameters(trackParameters_), layerSurface(layerSurface_) {}

            std::shared_ptr<const Trk::TrackParameters> trackParameters;
            MuonLayerSurface layerSurface;
        };

        /** constructor taking result of the intersection as arguments.
            The muonEntry pointer should be non-zero
            The class takes ownership of all inputs.
         */
        MuonSystemExtension(const Trk::TrackParameters* muonEntry, std::vector<Intersection>&& intersections);

        /** destructor */
        ~MuonSystemExtension();

        /** access to intersection with the muon entry layer */
        const Trk::TrackParameters& muonEntryLayerIntersection() const;

        /** access to the intersections with the layers.
            The intersections are stored as curvilinear parameters,
            the layers are identified by their cIdentifier() */
        const std::vector<Intersection>& layerIntersections() const;

    private:
        /** no copying nor assignment allow, use shared pointers instead */
        MuonSystemExtension& operator=(const MuonSystemExtension&) = delete;
        MuonSystemExtension(const MuonSystemExtension&) = delete;

        /** no default constructor */
        MuonSystemExtension() = delete;

        /** data members */
       const Trk::TrackParameters* m_muonEntryLayerIntersection{nullptr};  /// parameters at the muon entrance
        std::vector<Intersection> m_layerIntersections;                            /// parameters at the different layers
    };

    inline const Trk::TrackParameters& MuonSystemExtension::muonEntryLayerIntersection() const { return *m_muonEntryLayerIntersection; }

    inline const std::vector<MuonSystemExtension::Intersection>& MuonSystemExtension::layerIntersections() const {
        return m_layerIntersections;
    }

}  // namespace Muon

#endif
