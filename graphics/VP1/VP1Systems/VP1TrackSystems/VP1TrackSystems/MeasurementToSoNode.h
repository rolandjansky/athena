/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_VP1TRACKSYSTEMS_MEASUREMENTTOSONODE_H
#define TRK_VP1TRACKSYSTEMS_MEASUREMENTTOSONODE_H
#include <vector>
#include <map>
#include <string>
#include "Identifier/Identifier.h"
#include "VP1Utils/HitsSoNodeManager.h"
#include "VP1TrackSystems/TrkObjToString.h"

class SoNode;
class SoCube;
class SoTubs;
class SoSeparator;
class SoTransform;
class SoMaterial;
class CscIdHelper;
class RpcIdHelper;
class TgcIdHelper;
class MdtIdHelper;

namespace Muon {
    class CscPrepData;
    class RpcPrepData;
    class TgcPrepData;
    class MdtPrepData;
}

namespace InDet {
    class PixelCluster;
    class SCT_Cluster;
    class TRT_DriftCircle;
}

namespace InDetDD {
    class SiDetectorElement;
}

namespace MuonGM {
    class CscReadoutElement;
    class RpcReadoutElement;
    class TgcReadoutElement;
    class MdtReadoutElement;
}

namespace Trk {
//    class LocalPosition;
    class TrkDetElementBase;
    class PrepRawData;
    class RIO_OnTrack;
    class CompetingRIOsOnTrack;
    class PseudoMeasurementOnTrack;
    class MeasurementBase;

    class MeasurementToSoNode {
    public:

        enum ClusterDrawMode { SingleStrip=0, MultipleStrips=1, Errors=2 };
        enum DriftTubeDrawMode { ShortLength=0, FullLength=1 };

        MeasurementToSoNode();

        void initialize(    const CscIdHelper* cscIdHelper,
                            const RpcIdHelper* rpcIdHelper,
                            const TgcIdHelper* tgcIdHelper,
                            const MdtIdHelper* mdtIdHelper);

        /** This is the most general and most complete method - it returns a separator with nodes and transform. The material is not
        added here as it should be at the top of the tree (in general) */
        std::pair<SoSeparator*,  std::vector<SoNode*> >              \
                                createMeasSep(  const Trk::MeasurementBase& meas);

        /** Returns a vector of SoNodes to represent the RIO_OnTrack. The cluster mode determines how the clusters are drawn -
        i.e. as single central strip or the strips contributing to the cluster*/
        std::vector<SoNode*>    toSoNodes(      const Trk::RIO_OnTrack& prd);

        std::vector<SoNode*>    toSoNodes(      const Trk::CompetingRIOsOnTrack& crot);

        std::vector<SoNode*>    toSoNodes(      const Trk::PseudoMeasurementOnTrack& crot);

        // create basic shapes
        SoNode*                 createDriftTubeMarker(  double halfLength,  double radius);
        SoNode*                 createProjectedDriftTubeMarker(  double strawLength, double radius  );

        // SoNode*                 createPointMarker();
        SoNode*                 createLineMarker(double halfLength, bool isTube=false );

        /** Generic routine which creates SoNode from passed detector element, plus measurement type*/
        SoNode*                 toSoNode(const Trk::TrkDetElementBase* baseDetEl, Identifier id, TrkObjToString::MeasurementType prdType, Amg::Vector2D locPos);

        /** handles both drift circle types*/
        SoNode*                 toSoNodeDriftTube(const Trk::TrkDetElementBase* detEl, Identifier id, TrkObjToString::MeasurementType detType, double radius);

        //specific types
        SoNode*                 toSoNode(const InDetDD::SiDetectorElement* detElement, Identifier id, TrkObjToString::MeasurementType detType, Amg::Vector2D& locPos);
        SoNode*                 toSoNode(const MuonGM::CscReadoutElement* detElement, Identifier id);
        SoNode*                 toSoNode(const MuonGM::RpcReadoutElement* detElement, Identifier id);
        SoNode*                 toSoNode(const MuonGM::TgcReadoutElement* detElement, Identifier id);

        void                    setClusterDrawMode    (ClusterDrawMode mode);
        ClusterDrawMode         clusterDrawMode       ();
        void                    setDriftTubeDrawMode  (DriftTubeDrawMode mode);
        void                    setDriftTubeLength  (float length);
        DriftTubeDrawMode       driftTubeDrawMode  ();
        void                    setSimpleView(      TrkObjToString::MeasurementType type, bool value );
        void                    setSimpleViewAll(   bool value );
        bool                    isSimpleView(       TrkObjToString::MeasurementType type );

        void                    setTRTProjection( bool );
        bool                    isTRTProjection() const;

        /** Uses the local coords & the surface of the passed PRD to create a transform for the strip*/
        // SoTransform*            createTransform(    const Trk::PrepRawData& prd,
        //                                             MeasurementType detType);

        /** Uses the local coords & the surface of the passed Trk::MeasurementBase to create a transform for the strip*/
        SoTransform*            createTransform(    const Trk::MeasurementBase& rot,
                                                    TrkObjToString::MeasurementType detType);

    private:
        std::vector<bool>       m_simpleView;
        bool                    m_trtProjection;
        const CscIdHelper*      m_cscIdHelper;
        const RpcIdHelper*      m_rpcIdHelper;
        const TgcIdHelper*      m_tgcIdHelper;
        const MdtIdHelper*      m_mdtIdHelper;
        double                  m_stripThickness; //!< Depth/Thickness of strips in z (local coords)
        std::vector< SoMaterial* > m_materials;

        DriftTubeDrawMode       m_dtDrawMode;
        ClusterDrawMode         m_clusDrawMode;
        float m_dtLength; //!< Only used when in ShortMode
        
        HitsSoNodeManager       m_nodeManager;
        TrkObjToString          m_objToType;
    };
}

inline void
Trk::MeasurementToSoNode::setClusterDrawMode(Trk::MeasurementToSoNode::ClusterDrawMode mode)
{
    m_clusDrawMode=mode;
}

inline void
Trk::MeasurementToSoNode::setDriftTubeDrawMode(Trk::MeasurementToSoNode::DriftTubeDrawMode mode)
{
    m_dtDrawMode=mode;
}

inline void
Trk::MeasurementToSoNode::setDriftTubeLength  (float length)
{
    m_dtLength=length;
}

inline Trk::MeasurementToSoNode::ClusterDrawMode
Trk::MeasurementToSoNode::clusterDrawMode()
{
    return m_clusDrawMode;
}

inline Trk::MeasurementToSoNode::DriftTubeDrawMode
Trk::MeasurementToSoNode::driftTubeDrawMode()
{
    return m_dtDrawMode;
}

inline void
Trk::MeasurementToSoNode::setSimpleView(TrkObjToString::MeasurementType type, bool value)
{
    m_simpleView[type]=value;
}


inline void
Trk::MeasurementToSoNode::setTRTProjection( bool b )
{
  m_trtProjection = b;
}

inline bool
Trk::MeasurementToSoNode::isTRTProjection() const
{
  return m_trtProjection;
}

inline void
Trk::MeasurementToSoNode::setSimpleViewAll( bool value)
{
    unsigned int end = static_cast<TrkObjToString::MeasurementType>(TrkObjToString::Unknown);
    for (unsigned int i=0; i<end; ++i) m_simpleView[i]=value;
}

inline bool
Trk::MeasurementToSoNode::isSimpleView(TrkObjToString::MeasurementType type)
{
    return m_simpleView[type];
}


#endif

