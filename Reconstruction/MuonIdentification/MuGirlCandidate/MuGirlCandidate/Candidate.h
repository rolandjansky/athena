/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_CANDIDATE_H
#define MUGIRL_CANDIDATE_H

#include "Identifier/IdentifierHash.h"
#include "MuGirlCandidate/MuGirlNS.h"
#include "MuGirlInterfaces/MdtSegmentMakerInfo.h"
#include "MuGirlInterfaces/ICandidateTool.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/MuonFeature.h"

class TrigMuonEFTrack;
class IRoiDescriptor;
//class MuonFeature;

namespace MuonGM
{
class MuonReadoutElement;
}

namespace Muon
{
class MuonSegment;
}

namespace Trk
{
  class Charged;
  class PerigeeSurface;
  template <int DIM, class Y, class Z> class ParametersT;
  typedef ParametersT<5,Charged,PerigeeSurface> Perigee;
  class TrackSurfaceIntersection;
}

//namespace xAOD
//{
//class TrackParticle;
//}

namespace MuGirlNS
{
class CandidateTool;
class CandidateSummary;
struct MdtSegmentMakerInfo;
class SegmentManager;

class Candidate
{
public:
    Candidate(CandidateTool* pMuGirl);
    virtual ~Candidate();

    virtual void clear();
    //void clearMuonSeg();
    const  Trk::TrackParameters* IDPerigee() const;
    const  TrigMuonEFTrack* getMuonEFTrack() const;
    const  MuonFeature* getMuonFeature() const;
    const  xAOD::TrackParticle* IDTrk() const;
    const  xAOD::TrackParticle* RefittedTrack() const;
    const  xAOD::TrackParticle* MSRefittedTrack() const;
    const  Trk::Track* TrkRefittedTrack() const;
    double qOverP() const;
    double muidELoss() const;
    double annBarrel() const;
    double annEndCap() const;
    double LHR() const;
    int Isolation() const;
    void setIDPerigee(const Trk::TrackParameters* pPerigee);
    void setIDTrk(const xAOD::TrackParticle* pTrackParticle);
    void setMuonEFTrack(const TrigMuonEFTrack* muonEFTrack);
    void setMuonBetaRefitTrack(const xAOD::TrackParticle* muonBetaRefitTrack);
    void setMuonBetaRefitTrack(const Trk::Track* muonBetaRefitTrack);
    void setMuonFeature(const MuonFeature* muonFeature);
    void setRefittedTrack(const xAOD::TrackParticle* pRefittedTrack);
    void setRefittedTrack(const Trk::Track* pRefittedTrack);
    void setRefittedTrackToSummary(const xAOD::TrackParticle* pRefittedTrack, const Trk::Track* pTrkRefittedTrack, CandidateSummary* Summary);
    void setMSTrackToSummary(std::unique_ptr<const xAOD::TrackParticle> pRefittedTrack,
                             std::unique_ptr<const Trk::Track> pTrkrefittedTrack,
                             CandidateSummary* Summary);

    StatusCode crudeExtrapolate();
    void computeCombinedIntersection(DistanceType eDist,
                                     RegionType eReg);
    void selectChambers();
    void collectHits(TechnologyType eTech,
                     DistanceType eDist,
                     RegionType eReg,
                     TechnologyType eOrigTech);
    virtual void buildSegments(TechnologyType eTech,
                               DistanceType eDist,
                               RegionType eReg,
                               TechnologyType eOrigTech);
    void computeChamberIntersections(TechnologyType eOrigTech,
                                     DistanceType eOrigDist,
                                     DistanceType eTargDist,
                                     RegionType eReg);
    const Trk::TrackSurfaceIntersection*
    extrapolatePlane(const Trk::TrackSurfaceIntersection* pBaseTrkIsect,
                     const MuonGM::MuonReadoutElement* pReadoutElement);
    //void truthAssociation(const Rec::TrackParticle* pTrackParticle);
    Intersection* addIntersection(IntersectionType eType,
                                  const Trk::TrackSurfaceIntersection* pTrkIsect,
                                  TechnologyType eOrigTech,
                                  DistanceType eOrigDist,
                                  TechnologyType eTargTech,
                                  DistanceType eTargDist,
                                  RegionType eReg,
                                  Chamber* pChamber);
    Intersection* cellIntersection(TechnologyType eTech,
                                   DistanceType eDist,
                                   RegionType eReg);
    const Intersection* cellIntersection(TechnologyType eTech,
                                         DistanceType eDist,
                                         RegionType eReg) const;
    void setCellIntersection(TechnologyType eTech,
                             DistanceType eDist,
                             RegionType eReg,
                             Intersection* pIsect);
    void setAnn(double ann_Barrel, double ann_EndCap);
    void setLHR(double lhr);
    void setIsolation(int isolation);
    void setPassANN(bool pass);
    void setCombinedMF(bool hasCombined);
    void setPassdoStau(bool pass);
    void addChamber(Chamber* pChamber);
    Segment* addSegment(SegmentType eType,
                        const Muon::MuonSegment* pMuonSegment,
                        TechnologyType eTech,
                        DistanceType eDist,
                        RegionType eReg,
                        Station* pStation);
    bool hasCombinedMF() const;
    bool isMF() const;
    bool isEF() const;
    bool isExtrapolated() const;
    void addMDTSegmenttoMuonSegments(const Muon::MuonSegment* pMuonSegment);
    void addT0Segments(const Muon::MuonSegment* pMuonSegment);
    bool passAnn() const;
    bool passStau() const;
    double roadDeltaPhi(double pt) const;
    double roadDeltaEta(double pt) const;
    double roadDeltaEta(TechnologyType eTech,
                        DistanceType eDist,
                        RegionType eReg,
                        double dEta, double pt);
    const IntersectionList& intersections() const;
    const ChamberList& chambers() const;
    const ChamberList& cellChambers(TechnologyType eTech,
                                    DistanceType eDist,
                                    RegionType eReg) const;
    const HitList& hits() const;
    const SegmentList& segments() const;
    const MuonSegmentList& muonSegments() const;
    void markHits(ChamberList& chambers, Segment* pSegment);
    void computeAngles();
    double innerAngle() const;
    double innerDelta() const;
    double middleAngle() const;
    double middleDelta() const;
    //StatusCode fillSummary(CandidateSummary* pSummary,const Trk::MeasuredPerigee* pPerigee);
    StatusCode fillSummary(CandidateSummary* pSummary);
    StatusCode fillSummaryFromBetaRefit(CandidateSummary* pSummary);


    bool firstIsBest( const Muon::MuonSegment& seg1, const Muon::MuonSegment& seg2 ) const;
    std::pair<int,int> hitsInMultilayer( const Muon::MuonSegment& segment ) const;

    bool saveMdtSegmentMakerInfo() const
    {
        return m_bSaveMdtSegmentMakerInfo;
    }
    void setSaveMdtSegmentMakerInfo(bool bSaveMdtSegmentMakerInfo = true)
    {
        m_bSaveMdtSegmentMakerInfo = bSaveMdtSegmentMakerInfo;
    }
    const MdtSegmentMakerInfoList& getMdtSegmentMakerInfo() const
    {
        return m_mdtSegmentMakerInfo;
    }
    MdtSegmentMakerInfoList& getMdtSegmentMakerInfo()
    {
        return m_mdtSegmentMakerInfo;
    }
    const RIO_OnTrackLists& getRpcHitsInSegments();
    const RIO_OnTrackLists& getTgcHitsInSegments();
    std::string cellToString(TechnologyType eTech,
                             DistanceType eDist,
                             RegionType eReg) const;
    const Chamber* findFirstValidChamber(TechnologyType eTech,
                                         DistanceType eDist,
                                         RegionType eReg) const;
    void setSegmentManager(SegmentManager* pSegmentManager)
    {
        m_pSegmentManager = pSegmentManager;
    }
    void setT0Map(std::map<Identifier,const Muon::MuonSegment* >* map)
    {
         m_pT0map = map;
    }
    std::map<Identifier,const Muon::MuonSegment* >* getT0Map()
    {
        return m_pT0map;
    }

    Clusters& getClusters(TechnologyType eTech, DistanceType eDist, RegionType eReg)
    {
        return m_cells[eTech][eDist][eReg].clusters;
    }
    Clusters& getClustersOnSegment(TechnologyType eTech, DistanceType eDist, RegionType eReg)
    {
        return m_cells[eTech][eDist][eReg].clustersOnSegment;
    }
    int segmentQuality(const Muon::MuonSegment* pSegment) const;

    bool hasInnerStationSegments();
protected:
    struct Cell
    {
        Intersection*   pIsect;
        ChamberList     chambers;   /**< The list of chambers in the cell */
        Clusters        clusters;   /**< The list of lists of trigger elements */
        Clusters        clustersOnSegment;   /**< The list of lists of trigger elements on segment */

        void clear();
    };

    void updateRoad(Road& road, const Intersection* pIsect);
    void tgcDetHashIDList(const IRoiDescriptor& _roi, std::vector<IdentifierHash> &chambers);
    CandidateTool*                  m_pMuGirl;
    bool                            m_hasCombinedMF;
    const TrigMuonEFTrack*          m_muonEFTrack;   /**< The TrigMuonEFTrack */
    const MuonFeature*              m_muonFeature;
    const Trk::TrackParameters*     m_pIDPerigee;   /**< The ID track perigee parameters */
    double                          m_IDQOverP;     /**< The ID track q/p */
    double                          m_QOverP;       /**< The corrected q/p */
    double                          m_MuidELoss;    /**< The energy loss as estimated by Muid */
    Trk::TrackSurfaceIntersection*  m_pIDTrkIsect;  /**< The ID track original intersection */
    const xAOD::TrackParticle*      m_pIDTrk;
    const xAOD::TrackParticle*      m_pRefittedTrack;
    const xAOD::TrackParticle*      m_pLowBetaTrack;
    const Trk::Track*               m_pTrkRefittedTrack;    /**< Pointer to the MuGirl refitted Track */
    const Trk::Track*               m_pTrkLowBetaTrack;     /**< Pointer to the MuGirl Low Beta refitted Track */
    Cell                            m_cells[ALL_TECHNOLOGIES][DISTANCES][REGIONS];
    IntersectionList                m_iSections;    /**< The global list of intersections */
    ChamberList                     m_chambers;     /**< The global list of chambers */
    HitList                         m_hits;         /**< The global list of hits */
    SegmentList                     m_segments;     /**< The global list of segments */
    MuonSegmentList                 m_muonSegments;
    MuonSegmentList                 m_muonT0Segments;
    double                          m_annBarrel;
    double                          m_annEndCap;
    double                          m_innAngle;
    double                          m_innDelta;
    double                          m_midAngle;
    double                          m_midDelta;
    double                          m_lhr;
    int                             m_isolation;
    bool                            m_bSaveMdtSegmentMakerInfo;
    bool                            m_passAnn;
    bool                            m_passStau;
    MdtSegmentMakerInfoList         m_mdtSegmentMakerInfo;
    RIO_OnTrackLists                m_RIO_OnTrackLists;
    bool                            m_startFromMuonEntry;
    bool                            m_startFromBetaRefit;
    bool                            m_startFromMF;
    bool                            m_startFromEF;
    int                             m_MFstartStation;
    RIO_OnTrackLists                m_RpcRIO_OnTrackLists;
    RIO_OnTrackLists                m_TgcRIO_OnTrackLists;
    SegmentManager*                 m_pSegmentManager;
    std::map<Identifier,const Muon::MuonSegment*>* m_pT0map;

};


inline const Trk::TrackParameters* Candidate::IDPerigee() const
{
    return m_pIDPerigee;
}
inline const MuonFeature* Candidate::getMuonFeature() const
{
    return m_muonFeature;
}
inline const TrigMuonEFTrack* Candidate::getMuonEFTrack() const
{
    return m_muonEFTrack;
}
inline const xAOD::TrackParticle* Candidate::IDTrk() const
{
    return m_pIDTrk;
}
inline const xAOD::TrackParticle* Candidate::RefittedTrack() const
{
    return m_pRefittedTrack;
}
inline double Candidate::qOverP() const
{
    return m_QOverP;
}
inline double Candidate::muidELoss() const {
    return m_MuidELoss;
}
inline double Candidate::annBarrel() const
{
    return m_annBarrel;
}
inline double Candidate::annEndCap() const
{
    return m_annEndCap;
}
inline double Candidate::LHR() const
{
    return m_lhr;
}
inline int Candidate::Isolation() const
{
    return m_isolation;
}
 inline bool Candidate::hasCombinedMF() const
 {
     return m_hasCombinedMF;
 }
 inline bool Candidate::passAnn() const
 {
     return m_passAnn;
 }
 inline bool Candidate::isExtrapolated() const
 {
     return m_startFromMuonEntry;
 }
 inline bool Candidate::isMF() const
 {
     return m_startFromMF;
 }
 inline bool Candidate::isEF() const
 {
     return m_startFromEF;
 }
 inline bool Candidate::passStau() const
 {
     return m_passStau;
 }
inline void Candidate::setAnn(double ann_Barrel, double ann_EndCap)
{
    m_annBarrel = ann_Barrel;
    m_annEndCap = ann_EndCap;
}
inline void Candidate::setCombinedMF(bool hasCombined)
{
    m_hasCombinedMF = hasCombined;
}
inline void Candidate::setPassANN(bool pass)
{
    m_passAnn = pass;
}
inline void Candidate::setPassdoStau(bool pass)
{
    m_passStau = pass;
}
inline void Candidate::setLHR(double lhr)
{
    m_lhr = lhr;
}
inline void Candidate::setIsolation(int isolation)
{
    m_isolation = isolation;
}
inline Intersection* Candidate::cellIntersection(TechnologyType eTech,
        DistanceType eDist,
        RegionType eReg)
{
    return m_cells[eTech][eDist][eReg].pIsect;
}

inline const Intersection* Candidate::cellIntersection(TechnologyType eTech,
        DistanceType eDist,
        RegionType eReg) const
{
    return m_cells[eTech][eDist][eReg].pIsect;
}

inline void Candidate::setCellIntersection(TechnologyType eTech,
        DistanceType eDist,
        RegionType eReg,
        Intersection* pIsect)
{
    m_cells[eTech][eDist][eReg].pIsect = pIsect;
}

inline const ChamberList& Candidate::cellChambers(TechnologyType eTech,
        DistanceType eDist,
        RegionType eReg) const
{
    return m_cells[eTech][eDist][eReg].chambers;
}

inline const IntersectionList& Candidate::intersections() const
{
    return m_iSections;
}

inline const ChamberList& Candidate::chambers() const
{
    return m_chambers;
}

inline const HitList& Candidate::hits() const
{
    return m_hits;
}

inline const SegmentList& Candidate::segments() const
{
    return m_segments;
}
inline const MuonSegmentList& Candidate::muonSegments() const
{
    return m_muonSegments;
}

//     inline const IMuGirlPerformanceTruthTool::TruthTrack* Candidate::truthTrack() const
//     {
//         return m_truthTrackValid ? &m_truthTrack : NULL;
//     }

inline double Candidate::innerAngle() const
{
    return m_innAngle;
}

inline double Candidate::innerDelta() const
{
    return m_innDelta;
}

inline double Candidate::middleAngle() const
{
    return m_midAngle;
}

inline double Candidate::middleDelta() const
{
    return m_midDelta;
}

}

#endif //MUGIRL_CANDIDATE_H
