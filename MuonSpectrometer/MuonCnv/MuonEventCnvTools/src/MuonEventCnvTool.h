/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTCNVTOOL_H
#define MUONEVENTCNVTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TrkEventCnvTools/ITrkEventCnvTool.h"


namespace Trk {
    class RIO_OnTrack;   
}  // namespace Trk

namespace Muon {

    /**Tool used in the persisency convertors to help rebuild EDM objects*/
    class MuonEventCnvTool : public extends<AthAlgTool, Trk::ITrkEventCnvTool> {
    public:
        enum MuonConcreteType { MDT, RPC, CSC, TGC, MM, STGC, TypeUnknown };

        MuonEventCnvTool(const std::string&, const std::string&, const IInterface*);

        virtual StatusCode initialize() override;

        /** check that the RoT is correctly filled*/
        virtual void checkRoT(const Trk::RIO_OnTrack& rioOnTrack) const override;

        virtual std::pair<const Trk::TrkDetElementBase*, const Trk::PrepRawData*> getLinks(Trk::RIO_OnTrack& rioOnTrack) const override;

        /** @copydoc Trk::ITrkEventCnvTool::prepareRIO_OnTrack( Trk::RIO_OnTrack* rot)*/
        virtual void prepareRIO_OnTrack(Trk::RIO_OnTrack* rot) const override;

        /** Similar, but just return the EL components rather then
            changing ROT. */
        virtual void prepareRIO_OnTrackLink(const Trk::RIO_OnTrack* rot, ELKey_t& key, ELIndex_t& index) const override;

        /** Take the passed RoT and recreate it (i.e. fill missing pointers etc)*/
        virtual void recreateRIO_OnTrack(Trk::RIO_OnTrack* RoT) const override;

        /** Return the detectorElement associated with this Identifier*/
        virtual const Trk::TrkDetElementBase* getDetectorElement(const Identifier& id, const IdentifierHash& idHash) const override;

        /** Return the detectorElement associated with this Identifier*/
        virtual const Trk::TrkDetElementBase* getDetectorElement(const Identifier& id) const override;

    private:
        /* Retrieves the PrepRawData container from StorGate and returns the corresponding element with the same identifier */
        const Trk::PrepRawData* getLink(const Identifier& id, const IdentifierHash& idHash) const;

        template <class CONT>
        const Trk::PrepRawData* getLink(const Identifier& id, const IdentifierHash& idHash, const SG::ReadHandleKey<CONT>& handle,
                                        const EventContext& ctx) const;

    private:
        ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

        SG::ReadHandleKey<RpcPrepDataContainer> m_rpcPrdKey{this, "RpcClusterContainer", "RPC_Measurements", "Location for RPC PRDs"};
        SG::ReadHandleKey<CscPrepDataContainer> m_cscPrdKey{this, "CscClusterContainer", "CSC_Clusters", "Location for CSC PRDs"};
        SG::ReadHandleKey<TgcPrepDataContainer> m_tgcPrdKey{this, "TgcClusterContainer", "TGC_MeasurementsAllBCs", "Location for TGC PRDs"};
        SG::ReadHandleKey<MdtPrepDataContainer> m_mdtPrdKey{this, "MdtClusterContainer", "MDT_DriftCircles", "Location for MDT PRDs"};
        SG::ReadHandleKey<MMPrepDataContainer> m_mmPrdKey{this, "MM_ClusterContainer", "MM_Measurements", "Location for MM PRDs"};
        SG::ReadHandleKey<sTgcPrepDataContainer> m_stgcPrdKey{this, "STGC_ClusterContainer", "STGC_Measurements", "Location for sTGC PRDs"};

        const MuonGM::MuonDetectorManager* m_muonDetMgr{nullptr};
        SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey{this, "DetectorManagerKey", "MuonDetectorManager",
                                                                                "Key of input MuonDetectorManager condition data"};

        /** If true, search for PRDs manually - i.e. do not rely on ElementLink working
        @warning This functionality is left in primarily for debugging - will probably be removed in near future.*/
        Gaudi::Property<bool> m_manuallyFindPRDs{this, "FindPRDsManually", false,
                                                 "If true, search for PRDs manually - i.e. do not rely on ElementLink working"};
        Gaudi::Property<bool> m_fixTGCs{this, "FixTGCs", false, "If true, try to fix broken EL, e.g. ATLASRECTS-5151"};
        
        Gaudi::Property<bool> m_useAlignedGeo{this, "UseAlignedGeometry", true, "If true the DetectorManager is retrieved from the conditions store instead of the DetectorStore"};
    };

}  // namespace Muon
#endif  // MOORETOTRACKTOOL_H
