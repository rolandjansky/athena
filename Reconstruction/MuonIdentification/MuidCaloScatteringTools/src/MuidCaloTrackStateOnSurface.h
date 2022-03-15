/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
/**@class  MuidCaloTrackStateOnSurface
  AlgTool to allocate 3 scattering centres as TrackStateOnSurface objects
  to represent the Coulomb scattering and energy deposit between the InDet
  and MuonSpectrometer entrance.

  This tool retrieves the appropriate parametrized scattering centres and
  hybrid (measured + parametrized) energy deposit evaluation
  using the IMuidCaloMaterialParam and IMuidCaloEnergy interfaces

   @author Alan.Poppleton@cern.ch
*/
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDCALOSCATTERINGTOOLS_MUIDCALOTRACKSTATEONSURFACE_H
#define MUIDCALOSCATTERINGTOOLS_MUIDCALOTRACKSTATEONSURFACE_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgTool.h"
// For magneticfield
#include <atomic>

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MuidInterfaces/IMuidCaloEnergy.h"
#include "MuidInterfaces/IMuidCaloMaterialParam.h"
#include "MuidInterfaces/IMuidCaloTrackStateOnSurface.h"
#include "TrkExInterfaces/IPropagator.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace Trk {
    class MagneticFieldProperties;
}  // namespace Trk

namespace Rec {

    class MuidCaloTrackStateOnSurface : public AthAlgTool, virtual public IMuidCaloTrackStateOnSurface {
    public:
        MuidCaloTrackStateOnSurface(const std::string& type, const std::string& name, const IInterface* parent);
        virtual ~MuidCaloTrackStateOnSurface() = default;  // destructor

        StatusCode initialize() override;
        StatusCode finalize() override;

        /**IMuidCaloTrackStateOnSurface interface:
           to get the 3 scattering and energy deposit TSOS'es representing the calorimeter.
           The input TrackParameters may be anywhere along the track. */
        std::vector<std::unique_ptr<const Trk::TrackStateOnSurface>> caloTSOS(const EventContext& ctx,
                                                                              const Trk::TrackParameters& parameters) const override;

        /**IMuidCaloTrackStateOnSurface interface:
           to get individually the scattering TSOS'es representing the calorimeter.
           The input TrackParameters may be anywhere along the track. */
        std::unique_ptr<Trk::TrackStateOnSurface> innerTSOS(const EventContext& ctx, const Trk::TrackParameters& parameters) const override;
        std::unique_ptr<Trk::TrackStateOnSurface> outerTSOS(const EventContext& ctx, const Trk::TrackParameters& parameters) const override;

        /**IMuidCaloTrackStateOnSurface interface:
           to get the energy deposit TSOS representing the calorimeter.
           The input TrackParameters may be anywhere along the track, but when the inner
           or outer parameters are provided they must be expressed at the appropriate surface.
           The return TSOS surface is at the material midpoint. */
        std::unique_ptr<Trk::TrackStateOnSurface> middleTSOS(const EventContext& ctx, const Trk::TrackParameters& middleParameters,
                                                             const Trk::TrackParameters* innerParameters,
                                                             const Trk::TrackParameters* outerParameters) const override;

    private:
        // private methods
        enum SurfaceLayer { Inner, Middle, Outer };
        const Trk::Surface* getCaloSurface(const double eta, const short layer) const;

        std::unique_ptr<const Trk::TrackParameters> getExtrapolatedParameters(const EventContext& ctx,
                                                                              const Trk::TrackParameters& parameters,
                                                                              const short layer) const;

        bool useEtaPhiFromDirection(const Trk::TrackParameters& parameters) const;

        // helpers, managers, tools
        ToolHandle<Rec::IMuidCaloEnergy> m_caloEnergyDeposit{
            this,
            "CaloEnergyDeposit",
            "Rec::MuidCaloEnergyTool/MuidCaloEnergyTool",
        };
        ToolHandle<Rec::IMuidCaloEnergy> m_caloEnergyParam{
            this,
            "CaloEnergyParam",
            "Rec::MuidCaloEnergyTool/MuidCaloEnergyToolParam",
        };
        ToolHandle<Rec::IMuidCaloMaterialParam> m_caloMaterialParam{
            this,
            "CaloMaterialParam",
            "Rec::MuidCaloMaterialParam/MuidCaloMaterialParam",
        };
        ToolHandle<Trk::IPropagator> m_propagator{
            this,
            "Propagator",
            "Trk::IntersectorWrapper/IntersectorWrapper",
        };

        Trk::MagneticFieldProperties m_magFieldProperties{Trk::FullField};

        // Read handle for conditions object to get the field cache
        SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey{this, "AtlasFieldCacheCondObj", "fieldCondObj",
                                                                                  "Name of the Magnetic Field conditions object key"};

        // configurable cut-off values
        double m_minCaloRadius;
        double m_minRemainingEnergy;
        double m_paramPtCut;

        // counters (for finalize)
        mutable std::atomic_int m_count{0};
        mutable std::atomic_int m_countArbitrarySolution{0};
        mutable std::atomic_int m_countCompleteFailure{0};
        mutable std::atomic_int m_countInnerFailure{0};
        mutable std::atomic_int m_countOuterFailure{0};
    };

}  // namespace Rec

#endif  // MUIDCALOSCATTERINGTOOLS_MUIDCALOTRACKSTATEONSURFACE_H
