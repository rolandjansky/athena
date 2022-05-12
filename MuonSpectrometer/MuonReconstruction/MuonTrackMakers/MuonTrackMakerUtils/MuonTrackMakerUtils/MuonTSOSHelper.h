/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include <memory>

namespace Muon {

    class MuonTSOSHelper {
    public:
        /** clone input, update the type */
        static std::unique_ptr<Trk::TrackStateOnSurface> cloneTSOS(const Trk::TrackStateOnSurface& tsos,
                                                   Trk::TrackStateOnSurface::TrackStateOnSurfaceType type) {
            std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern = tsos.types();
            if (type == Trk::TrackStateOnSurface::Outlier)
                typePattern.set(Trk::TrackStateOnSurface::Measurement);
            else if (type == Trk::TrackStateOnSurface::Measurement)
                typePattern.set(Trk::TrackStateOnSurface::Outlier, false);
            typePattern.set(type);

            return std::make_unique<Trk::TrackStateOnSurface>(tsos.measurementOnTrack() ? tsos.measurementOnTrack()->uniqueClone() : nullptr,
                                                tsos.trackParameters() ? tsos.trackParameters()->uniqueClone() : nullptr,
                                                tsos.fitQualityOnSurface() ? tsos.fitQualityOnSurface()->uniqueClone() : nullptr,
                                                tsos.materialEffectsOnTrack() ? tsos.materialEffectsOnTrack()->uniqueClone() : nullptr,
                                                 typePattern);
        }

        /** clone input, replacing the track parameteres and the measurement base and updating the type */
        static std::unique_ptr<Trk::TrackStateOnSurface> 
        cloneTSOSWithUpdate(const Trk::TrackStateOnSurface& tsos, const Trk::MeasurementBase& meas,
                                                             const Trk::TrackParameters& pars,
                                                             Trk::TrackStateOnSurface::TrackStateOnSurfaceType type) {
            std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern = tsos.types();
            if (type == Trk::TrackStateOnSurface::Outlier)
                typePattern.set(Trk::TrackStateOnSurface::Measurement);
            else if (type == Trk::TrackStateOnSurface::Measurement)
                typePattern.set(Trk::TrackStateOnSurface::Outlier, false);
            typePattern.set(type);

            return std::make_unique<Trk::TrackStateOnSurface>(meas.uniqueClone(), pars.uniqueClone(),
                                                tsos.fitQualityOnSurface() ? tsos.fitQualityOnSurface()->uniqueClone() : nullptr,
                                                tsos.materialEffectsOnTrack() ? tsos.materialEffectsOnTrack()->uniqueClone() : 0, 
                                                typePattern);
        }

        /** create a perigee TSOS, takes ownership of the Perigee */
        static std::unique_ptr<Trk::TrackStateOnSurface> 
        createPerigeeTSOS(std::unique_ptr<const Trk::TrackParameters> perigee) {
            std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
            typePattern.set(Trk::TrackStateOnSurface::Perigee);
            return  std::make_unique<Trk::TrackStateOnSurface>(nullptr, std::move(perigee), nullptr, nullptr, typePattern);
        }
       
        /** create a TSOS with a measurement, takes ownership of the pointers */
        static std::unique_ptr<Trk::TrackStateOnSurface> 
        createMeasTSOS(std::unique_ptr<const Trk::MeasurementBase> meas, 
              std::unique_ptr<const Trk::TrackParameters> pars,
              Trk::TrackStateOnSurface::TrackStateOnSurfaceType type) {
            std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
            if (type == Trk::TrackStateOnSurface::Outlier) typePattern.set(Trk::TrackStateOnSurface::Measurement);
            typePattern.set(type);
            return std::make_unique<Trk::TrackStateOnSurface>(std::move(meas), std::move(pars), nullptr, nullptr, typePattern);
        }
        
      
        /** create a TSOS with a measurement, takes ownership of the pointers */
        static std::unique_ptr<Trk::TrackStateOnSurface> 
        createMeasTSOSWithUpdate(const Trk::TrackStateOnSurface& tsos, std::unique_ptr<const Trk::MeasurementBase> meas,
                                                                  std::unique_ptr<const Trk::TrackParameters> pars,
                                                                  Trk::TrackStateOnSurface::TrackStateOnSurfaceType type) {
            std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern = tsos.types();
            if (type == Trk::TrackStateOnSurface::Outlier)
                typePattern.set(Trk::TrackStateOnSurface::Measurement);
            else if (type == Trk::TrackStateOnSurface::Measurement)
                typePattern.set(Trk::TrackStateOnSurface::Outlier, false);
            typePattern.set(type);
            return std::make_unique<Trk::TrackStateOnSurface>(std::move(meas), std::move(pars), tsos.fitQualityOnSurface() ? tsos.fitQualityOnSurface()->uniqueClone() : 0,
                                                tsos.materialEffectsOnTrack() ? tsos.materialEffectsOnTrack()->uniqueClone() : 0, typePattern);
        }
        
        /** create a hole TSOS, takes ownership of the pointers */
        static std::unique_ptr<Trk::TrackStateOnSurface> createHoleTSOS(std::unique_ptr<const Trk::TrackParameters> pars) {
            std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
            typePattern.set(Trk::TrackStateOnSurface::Hole);
            return std::make_unique<Trk::TrackStateOnSurface>(nullptr, std::move(pars), nullptr, nullptr, typePattern);
        }
    };
}  // namespace Muon
