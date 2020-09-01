/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGMEASASSOCALG_MUONTGMEASASSOCALG_H
#define MUONTGMEASASSOCALG_MUONTGMEASASSOCALG_H

#include <fstream>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonTGRecTools/IMuonTGMeasTool.h"
#include "MuonTGRecTools/MuonTGHits.h"
#include "MuonTGRecTools/MuonTGSegments.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkTrack/Track.h"

namespace Muon {

/** @class MuonTGMeasAssocAlg

    associates hits/segments with active layers from the (muon) tracking geometry

    @author Sarka.Todorova@cern.ch
    */

class MuonTGMeasAssocAlg : public AthAlgorithm {

  public:
    typedef Trk::Track                                                           Track;
    typedef std::pair<const Trk::Layer*, std::vector<Identifier>*>               PairOfLayerID;
    typedef std::pair<const Trk::Layer*, std::vector<const Trk::PrepRawData*>*>  PairOfLayerPrd;
    typedef std::pair<const Trk::Layer*, std::vector<const Muon::MdtPrepData*>*> PairOfLayerMdtPrd;
    typedef std::pair<const Trk::Layer*, std::vector<const Muon::RpcPrepData*>*> PairOfLayerRpcPrd;
    typedef std::pair<const Trk::Layer*, std::vector<const Muon::CscPrepData*>*> PairOfLayerCscPrd;
    typedef std::pair<const Trk::Layer*, std::vector<const Muon::TgcPrepData*>*> PairOfLayerTgcPrd;

    MuonTGMeasAssocAlg(const std::string& name, ISvcLocator* pSvcLocator);

    ~MuonTGMeasAssocAlg() = default;

    StatusCode initialize();
    StatusCode execute();

    StatusCode retrieveMeasurements(const MuonGM::MuonDetectorManager* MuonDetMgr);
    StatusCode storeMeasurements();
    StatusCode storeSegments();

  private:
    std::vector<PairOfLayerPrd*>* createMdtHitCollectionLayers(const MuonGM::MuonDetectorManager* MuonDetMgr) const;
    std::vector<PairOfLayerPrd*>* createRpcHitCollectionLayers(const MuonGM::MuonDetectorManager* MuonDetMgr) const;
    std::vector<PairOfLayerPrd*>* createCscHitCollectionLayers(const MuonGM::MuonDetectorManager* MuonDetMgr) const;
    std::vector<PairOfLayerPrd*>* createTgcHitCollectionLayers(const MuonGM::MuonDetectorManager* MuonDetMgr) const;
    StatusCode                    createStationHitCollection() const;
    StatusCode                    createStationSegmentCollection(const MuonGM::MuonDetectorManager* MuonDetMgr) const;
    void createStationMap(const Trk::TrackingVolume* vol, const MuonGM::MuonDetectorManager* MuonDetMgr) const;
    void misAlignStations() const;
    void reAlignStations() const;
    const Trk::Layer* associatedLayer(int techn, Identifier id, const MuonGM::MuonDetectorManager* MuonDetMgr) const;
    const Trk::Layer* associatedLayer(const Trk::TrackingVolume* trVol, Identifier id) const;
    Identifier        getStationId(Identifier id) const;

    // --- job options
    bool m_mdtIn;
    bool m_rpcIn;
    bool m_cscIn;
    bool m_tgcIn;
    bool m_segmentsIn;
    bool m_writeTgHits;
    bool m_writeTgSegments;
    bool m_reAlign;
    bool m_misAlign;
    bool m_allowGeomAssoc;

    mutable const Trk::TrackingGeometry* m_trackingGeometry;
    std::string                          m_trackingGeometryName;

    ToolHandle<Muon::IMuonTGMeasTool> m_muonTgTool{
        this,
        "MuonTGMeasurementTool",
        "Muon::MuonTGMeasurementTool/MuonTGMeasurementTool",
    };
    ToolHandle<Trk::IExtrapolator> m_extrapolator{
        this,
        "Extrapolator",
        "Trk::Extrapolator/Extrapolator",
    };

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc",
                                                        "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey{
        this, "DetectorManagerKey", "MuonDetectorManager", "Key of input MuonDetectorManager condition data"};

    std::string m_inputSegmentCollectionMoore;
    std::string m_inputSegmentCollectionMoMu;
    std::string m_inputSegmentCollectionMBoy;
    std::string m_inputMdtPrdCollection;
    std::string m_inputRpcPrdCollection;
    std::string m_inputTgcPrdCollection;
    std::string m_inputCscPrdCollection;

    // -- algorithm members
    ActiveStoreSvc*                       m_activeStore;
    mutable std::vector<PairOfLayerPrd*>* m_mdtHits;
    mutable std::vector<PairOfLayerPrd*>* m_rpcHits;
    mutable std::vector<PairOfLayerPrd*>* m_cscHits;
    mutable std::vector<PairOfLayerPrd*>* m_tgcHits;
    mutable MuonTGHits*                   m_allHits;
    mutable MuonTGSegments*               m_allSegments;
    mutable std::map<Identifier, std::pair<const MuonGM::MuonStation*, const Trk::DetachedTrackingVolume*> >
        m_stationMap;
    mutable std::vector<std::pair<Identifier, Amg::Transform3D> >
        m_misAlignDiff;  // vector holding required misalignment
};

inline StatusCode
MuonTGMeasAssocAlg::retrieveMeasurements(const MuonGM::MuonDetectorManager* MuonDetMgr)
{
    // Get the messaging service, print where you are
    ATH_MSG_DEBUG("MuonTGMeasAssocAlg::retrieveMeasurements()");
    //
    // if (m_allHits) delete m_allHits;
    if (m_mdtHits) delete m_mdtHits;
    if (m_rpcHits) delete m_rpcHits;
    if (m_cscHits) delete m_cscHits;
    if (m_tgcHits) delete m_tgcHits;

    if (m_mdtIn) m_mdtHits = createMdtHitCollectionLayers(MuonDetMgr);
    if (m_rpcIn) m_rpcHits = createRpcHitCollectionLayers(MuonDetMgr);
    if (m_cscIn) m_cscHits = createCscHitCollectionLayers(MuonDetMgr);
    if (m_tgcIn) m_tgcHits = createTgcHitCollectionLayers(MuonDetMgr);
    ATH_CHECK(createStationHitCollection());

    ATH_MSG_DEBUG("MuonTGMeasAssocAlg::collected hits from " << m_allHits->size() << " stations");

    if (m_allHits->size()) return StatusCode::SUCCESS;

    return StatusCode::FAILURE;
}

}  // namespace Muon

#endif  // MUONTGMEASASSOCALG_MUONTGMEASASSOCALG_H
