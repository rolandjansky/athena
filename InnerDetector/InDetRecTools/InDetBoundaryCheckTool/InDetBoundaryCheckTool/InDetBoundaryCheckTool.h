/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ATHENA_INNERDETECTOR_TOOLS_BOUNDARYCHECKTOOL
#define ATHENA_INNERDETECTOR_TOOLS_BOUNDARYCHECKTOOL

#include "AthenaBaseComps/AthAlgTool.h"

#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetRecToolInterfaces/IInDetTestPixelLayerTool.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkToolInterfaces/IBoundaryCheckTool.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"

namespace InDet {
    class InDetBoundaryCheckTool : public AthAlgTool, virtual public Trk::IBoundaryCheckTool {
        public:
            InDetBoundaryCheckTool(
                const std::string &,
                const std::string &,
                const IInterface *
            );

            virtual StatusCode initialize() override;
            virtual StatusCode finalize() override;

            virtual Trk::BoundaryCheckResult boundaryCheck(
                const Trk::TrackParameters &
            ) const override;
        private:
            bool isAlivePixel(
                const InDetDD::SiDetectorElement &element,
                const Trk::TrackParameters &parameters
            ) const;

            bool isAliveSCT(
                const InDetDD::SiDetectorElement &element,
                const Trk::TrackParameters &parameters
            ) const;

            bool isBadSCTChipStrip(
                const Identifier &,
                const Trk::TrackParameters &,
                const InDetDD::SiDetectorElement &
            ) const;

            Trk::BoundaryCheckResult boundaryCheckSiElement(
                const InDetDD::SiDetectorElement &,
                const Trk::TrackParameters &
            ) const;

            ServiceHandle<IGeoModelSvc> m_geoModelSvc;

            ToolHandle<IInDetConditionsTool> m_sctCondSummaryTool{
                this,
                "SctSummaryTool",
                "SCT_ConditionsSummaryTool/InDetSCT_ConditionsSummaryTool",
                "Tool to retrieve SCT Conditions summary"
            };
            ToolHandle<IInDetTestPixelLayerTool> m_pixelLayerTool;

            const AtlasDetectorID *m_atlasId;

            /** eta and phi tolerances **/
            Gaudi::Property<double> m_etaTol{this, "ToleranceEta", 3.0};
            Gaudi::Property<double> m_phiTol{this, "TolerancePhi", 3.0};

            /** Control usage of pixel and SCT info */
            Gaudi::Property<bool> m_usePixel{this, "UsePixel", true};
            Gaudi::Property<bool> m_useSCT{this, "UseSCT", true};

            /** Control check of bad SCT chip (should be false for ITk Strip) */
            Gaudi::Property<bool> m_checkBadSCT{this, "CheckBadSCT", true};
    };
}

#endif
