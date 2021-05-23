/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuidMaterialEffectsOnTrackProvider.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MuidMaterialEffectsOnTrackProvider_H
#define MuidMaterialEffectsOnTrackProvider_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuidInterfaces/IMuidCaloTrackStateOnSurface.h"
#include "TrkDetDescrInterfaces/IMaterialEffectsOnTrackProvider.h"
#include "TrkExInterfaces/IMultipleScatteringUpdator.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/PlaneSurface.h"

/** @class MuidMaterialEffectsOnTrackProvider

@author thijs.cornelissen@cern.ch
*/
namespace Rec {
    class MuidMaterialEffectsOnTrackProvider : public AthAlgTool, virtual public Trk::IMaterialEffectsOnTrackProvider {
    public:
        /** AlgTool like constructor */
        MuidMaterialEffectsOnTrackProvider(const std::string&, const std::string&, const IInterface*);

        /**Virtual destructor*/
        virtual ~MuidMaterialEffectsOnTrackProvider() = default;

        /** AlgTool initailize method.*/
        StatusCode initialize() override;

        /** return all MaterialLayers associated to this track **/
        virtual std::vector<Trk::MaterialEffectsOnTrack>
        extrapolationSurfacesAndEffects(const Trk::TrackingVolume&,
                                        const Trk::IPropagator&,
                                        const Trk::TrackParameters&,
                                        const Trk::Surface&,
                                        Trk::PropDirection,
                                        Trk::ParticleHypothesis) const override final;

      private:
        ToolHandle<Rec::IMuidCaloTrackStateOnSurface> m_calotsos{
            this,
            "TSOSTool",
            "Rec::MuidCaloTrackStateOnSurface/MuidCaloTrackStateOnSurface",
        };
        ToolHandle<Rec::IMuidCaloTrackStateOnSurface> m_calotsosparam{
            this,
            "TSOSToolParam",
            "",
        };
        ToolHandle<Trk::IMultipleScatteringUpdator> m_scattool{
            this,
            "MultipleScatteringTool",
            "Trk::MultipleScatteringUpdator/AtlasMultipleScatteringUpdator",
        };

        bool m_cosmics;
    };
}  // namespace Rec

#endif  // MuidMaterialEffectsOnTrackProvider_H
