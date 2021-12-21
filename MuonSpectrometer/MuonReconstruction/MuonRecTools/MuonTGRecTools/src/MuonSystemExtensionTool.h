/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSYSTEMEXTENSIONTOOL_H
#define MUON_MUONSYSTEMEXTENSIONTOOL_H

#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonDetDescrUtils/MuonSectorMapping.h"
#include "MuonLayerEvent/MuonLayerSurface.h"
#include "MuonRecToolInterfaces/IMuonSystemExtensionTool.h"
#include "MuonStationIndex/MuonStationIndex.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODTracking/TrackParticle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"


namespace Trk {
    class Surface;
}

namespace Muon {

    class MuonSystemExtension;

    class MuonSystemExtensionTool : virtual public IMuonSystemExtensionTool, public AthAlgTool {
    public:
        typedef std::vector<MuonLayerSurface> SurfaceVec;

        /** Default AlgTool functions */
        MuonSystemExtensionTool(const std::string& type, const std::string& name, const IInterface* parent);
        virtual ~MuonSystemExtensionTool() = default;
        StatusCode initialize() override;

        /** get muon system extension */
        bool muonSystemExtension(const EventContext& ctx, SystemExtensionCache& cache) const override;

        bool muonLayerInterSections(const EventContext& ctx, 
                                    const MuonCombined::TagBase& cmb_tag,
                                    SystemExtensionCache& cache) const override;                                   
       
    private:
        /** initialize geometry */
        bool initializeGeometry();
        bool initializeGeometryBarrel(int sector, const Amg::AngleAxis3D& sectorRotation);
        bool initializeGeometryEndcap(int sector, MuonStationIndex::DetectorRegionIndex regionIndex,
                                      const Amg::AngleAxis3D& sectorRotation);

        /** get surfaces to be intersected for a given start parameters */
        SurfaceVec getSurfacesForIntersection(const Trk::TrackParameters& muonEntryPars, const SystemExtensionCache& cache) const;

        MuonSystemExtension::Intersection makeInterSection(std::shared_ptr<const Trk::TrackParameters> pars, const MuonLayerSurface& surf) const;

        
        ToolHandle<Trk::IParticleCaloExtensionTool> m_caloExtensionTool{
            this,
            "ParticleCaloExtensionTool",
            "Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool",
        };
        ToolHandle<Trk::IExtrapolator> m_extrapolator{
            this,
            "Extrapolator",
            "Trk::Extrapolator/AtlasExtrapolator",
        };
        
        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

      
        /** reference surfaces per region and sector */
        std::vector<std::vector<SurfaceVec> > m_referenceSurfaces;

        /** sector mapping helper */
        MuonSectorMapping m_sectorMapping;
    };
}  // namespace Muon

#endif
