/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_CANDIDATENTUPLE_H
#define MUGIRL_CANDIDATENTUPLE_H

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"

#include "MuGirlCandidate/MuGirlNS.h"

#include "MuonLayerHough/MuonLayerHough.h"

#include <vector>

class INTupleSvc;

namespace Muon {
  class MuonIdHelperTool;
}

namespace MuGirlNS
{
class Candidate;

class CandidateNtuple
{
public:
    CandidateNtuple(MsgStream& log, Muon::MuonIdHelperTool& helper);
    StatusCode book(NTuple::Tuple* pNTuple);
    StatusCode fillEventInfo(unsigned nRunNumber, unsigned nEventNumber);
    StatusCode fillCandidate(int iTrack, Candidate* pCand);
    // the Hough Data fill must be called after the fillCandidate, because it tries to connect the Hough Hits to the MuGirl hits
    StatusCode fillHoughData(Candidate* pCand, std::vector<MuonHough::MuonLayerHough::Maximum*> hough_data[16][4][2]);

private:
    void connectHitToHough(Candidate* pcand, Identifier id, int hough_maximum, int hough_hit);

    MsgStream& m_log;

    Muon::MuonIdHelperTool& m_idHelperTool;

    int m_maxChambers;                          /**< Maximum number of chambers                 = 200 */
    int m_maxIsect;                             /**< Maximum number of intersections            = 300 */
    int m_maxHits;                              /**< Maximum number of hits                     = 300 */
    int m_maxSeg;                               /**< Maximum number of segments                 = 100 */
    int m_maxHitSegMaps;                        /**< Maximum number of hit/segment associations = 300 */
    int m_maxMaxima;                            /**< Maximum number of Hough tranform maxima    = 300 */
    int m_maxHoughHits;                         /**< Maximum number of Hough hits               = 300 */

    // Run/Event/Candidate identification
    NTuple::Item<int>    m_RunNumber;          /**< The run number */
    NTuple::Item<int>    m_EventNumber;        /**< The event number */
    NTuple::Item<int>    m_CandidateNumber;    /**< The candidate index */

    // MF track parameters
    NTuple::Item<float>  m_MFpt;               /**< Original TrigMuonEFTrack: pT */
    NTuple::Item<int>    m_MFcharge;           /**< Original TrigMuonEFTrack: charge */
    NTuple::Item<float>  m_MFradius;           /**< Original TrigMuonEFTrack: radius */
    NTuple::Item<float>  m_MFphi;              /**< Original TrigMuonEFTrack: phi */
    NTuple::Item<int>    m_MFhasCombined;      /**< Original TrigMuonEFTrack: phi */
    NTuple::Item<float>  m_MFdir_phi;          /**< Original TrigMuonEFTrack: dir_phi */
    NTuple::Item<float>  m_MFz;                /**< Original TrigMuonEFTrack: z */
    NTuple::Item<float>  m_MFbeta;             /**< Original TrigMuonEFTrack: beta */
    NTuple::Item<float>  m_MFdir_z;            /**< Original TrigMuonEFTrack: dir_z */
    NTuple::Item<float>  m_MFeta;              /**< Original TrigMuonEFTrack: eta */
    // ID track parameters
    NTuple::Item<float>  m_IDTrkPt;            /**< Original ID Track: pT */
    NTuple::Item<float>  m_IDTrkEta;           /**< Original ID Track: Eta */
    NTuple::Item<float>  m_IDTrkPhi;           /**< Original ID Track: Phi */
    NTuple::Item<float>  m_IDTrkCharge;        /**< Original ID Track: Charge */
    NTuple::Item<float>  m_IDTrkQOverP;        /**< Original ID Track: q/p */
    NTuple::Item<float>  m_IDTrkTheta;         /**< Original ID Track: Theta */
    NTuple::Item<float>  m_IDTrka0;            /**< Original ID Track: a0 */
    NTuple::Item<float>  m_IDTrkz0;            /**< Original ID Track: Z0 */
    NTuple::Item<float>  m_IDTrkChi2;          /**< Original ID Track: chi2*/
    NTuple::Item<float>  m_IDTrkNDofFr;        /**< Original ID Track: Number of Degrees of Freedom*/
    NTuple::Item<float>  m_IDTrkChi2pr;        /**< Original ID Track:chi2 probability of the track fit*/
    NTuple::Item<float>  m_IDTrkCov11;         /**< Original ID Track: CovarianceMatrix Diagonal*/
    NTuple::Item<float>  m_IDTrkCov22;         /**< Original ID Track: CovarianceMatrix Diagonal*/
    NTuple::Item<float>  m_IDTrkCov33;         /**< Original ID Track: CovarianceMatrix Diagonal*/
    NTuple::Item<float>  m_IDTrkCov44;         /**< Original ID Track: CovarianceMatrix Diagonal*/
    NTuple::Item<float>  m_IDTrkCov55;         /**< Original ID Track: CovarianceMatrix Diagonal*/
    // ID track extrapolated parameters
    NTuple::Item<float>  m_muidELoss;          /**< Original ID Track: Muid energy loss estimation */
    NTuple::Item<float>  m_extrIDTrkPt;        /**< Original ID Track extrapolated to muon entry layer: pT */
    NTuple::Item<float>  m_extrIDTrkEta;       /**< Original ID Track extrapoalted to muon entry layer: Eta */
    NTuple::Item<float>  m_extrIDTrkPhi;       /**< Original ID Track extrapoalted to muon entry layer: Phi */
    NTuple::Item<float>  m_extrIDTrkCharge;    /**< Original ID Track extrapoalted to muon entry layer: Charge */
    NTuple::Item<float>  m_extrIDTrkQOverP;    /**< Original ID Track extrapoalted to muon entry layer: q/p */
    NTuple::Item<float>  m_extrIDTrkTheta;     /**< Original ID Track extrapoalted to muon entry layer: Theta */
    NTuple::Item<float>  m_extrIDTrka0;        /**< Original ID Track extrapoalted to muon entry layer: a0 */
    NTuple::Item<float>  m_extrIDTrkz0;        /**< Original ID Track extrapoalted to muon entry layer: Z0 */
    NTuple::Item<float>  m_extrIDTrkChi2;      /**< Original ID Track extrapoalted to muon entry layer: chi2*/
    NTuple::Item<float>  m_extrIDTrkNDofFr;    /**< Original ID Track extrapoalted to muon entry layer: Number of Degrees of Freedom*/
    NTuple::Item<float>  m_extrIDTrkChi2pr;    /**< Original ID Track extrapoalted to muon entry layer:chi2 probability of the track fit*/
    NTuple::Item<float>  m_extrIDTrkCov11;     /**< Original ID Track extrapoalted to muon entry layer: CovarianceMatrix Diagonal*/
    NTuple::Item<float>  m_extrIDTrkCov22;     /**< Original ID Track extrapoalted to muon entry layer: CovarianceMatrix Diagonal*/
    NTuple::Item<float>  m_extrIDTrkCov33;     /**< Original ID Track extrapoalted to muon entry layer: CovarianceMatrix Diagonal*/
    NTuple::Item<float>  m_extrIDTrkCov44;     /**< Original ID Track extrapoalted to muon entry layer: CovarianceMatrix Diagonal*/
    NTuple::Item<float>  m_extrIDTrkCov55;     /**< Original ID Track extrapoalted to muon entry layer: CovarianceMatrix Diagonal*/

    NTuple::Item<float>  m_QOverP;             /**< Corrected q/p (energy loss) */
    NTuple::Item<int>    m_passNN;             /**< ANN result for barrel */
    NTuple::Item<int>    m_isMuonFeature;      /**< Starts from MuonFeature */
    NTuple::Item<int>    m_passStau;           /**< ANN result for barrel */
    NTuple::Item<float>  m_NNBarrel;           /**< ANN result for barrel */
    NTuple::Item<float>  m_NNEndCap;           /**< ANN result for end-cap */
    NTuple::Item<float>  m_InnerAngle;         /**< Angle between ID and combined isect in inner distance */
    NTuple::Item<float>  m_InnerDelta;         /**< Distance between ID and combined isect in inner distance */
    NTuple::Item<float>  m_MiddleAngle;        /**< Angle between ID and combined isect in middle distance */
    NTuple::Item<float>  m_MiddleDelta;        /**< Distance between ID and combined isect in middle distance */
    NTuple::Item<float>  m_Lhr;
    NTuple::Item<int>    m_Isolation;

    // Chamber paramaters
    NTuple::Item<int>    m_NumChambers;        /**< Number of chambers */
    NTuple::Array<int>   m_ChamberID;          /**< Chamber: ID */
    NTuple::Array<int>   m_ChamberStation;     /**< Chamber: Station ID */
    NTuple::Array<int>   m_ChamberRegion;      /**< Chamber: Region (0=Barrel, 1=Endcap) */
    NTuple::Array<int>   m_ChamberDistance;    /**< Chamber: Distance (0=Inner, 1=EE, 2=Middle, 3=Outer) */
    NTuple::Array<int>   m_ChamberTechnology;  /**< Chamber: Technology (0=MDT, 1=RPC, 2=TGC, 3=CSC) */
    NTuple::Array<int>   m_ChamberFineIsect;   /**< Chamber: Index of fine (ID) intersection */
    NTuple::Array<int>   m_ChamberMdtIsect;    /**< Chamber: Index of intersection evaluated from MDT fit from same distance (-1 if not set) */
    NTuple::Array<int>   m_ChamberRpcIsect;    /**< Chamber: Index of intersection evaluated from RPC fit from same distance (-1 if not set) */
    NTuple::Array<int>   m_ChamberTgcIsect;    /**< Chamber: Index of intersection evaluated from TGC fit from same distance (-1 if not set) */
    NTuple::Array<int>   m_ChamberCscIsect;    /**< Chamber: Index of intersection evaluated from Csc fit from same distance (-1 if not set) */
    NTuple::Array<int>   m_ChamberCombIsect;   /**< Chamber: Index of intersection evaluated from combined in same distance (-1 if not set) */
    NTuple::Array<int>   m_ChamberMdtIsect1;   /**< Chamber: Index of intersection evaluated from MDT fit from other distance (-1 if not set) */
    NTuple::Array<int>   m_ChamberRpcIsect1;   /**< Chamber: Index of intersection evaluated from RPC fit from other distance (-1 if not set) */
    NTuple::Array<int>   m_ChamberTgcIsect1;   /**< Chamber: Index of intersection evaluated from TGC fit from other distance (-1 if not set) */
    NTuple::Array<int>   m_ChamberCscIsect1;   /**< Chamber: Index of intersection evaluated from Csc fit from other distance (-1 if not set) */
    NTuple::Array<int>   m_ChamberCombIsect1;  /**< Chamber: Index of intersection evaluated from combined in other distance (-1 if not set) */
    NTuple::Array<float> m_ChamberRZ;          /**< Chamber: Perp (for barrel) or Z (for endcap) of the center of the chamber plane surface */
    NTuple::Array<float> m_ChamberPhi;         /**< Chamber: Phi (for barrel) of the center of the chamber plane surface */

    // Intersection parameters
    NTuple::Item<int>    m_NumIsects;           /**< Number of intersections */
    NTuple::Array<int>   m_IsectType;          /**< Intersection: Type (0=Crude, 1=Fine, 2=Fit, 3=Evaluated, 4=Combined) */
    NTuple::Array<int>   m_IsectOrigTech;      /**< Intersection: Originating Technology (0=MDT, 1=RPC, 2=TGC, 3=CSC, 4=ID, 5=COMBINED) */
    NTuple::Array<int>   m_IsectOrigDist;      /**< Intersection: Originating distance (0=Inner, 1=EE, 2=Middle, 3=Outer) */
    NTuple::Array<int>   m_IsectTargTech;      /**< Intersection: Target Technology (0=MDT, 1=RPC, 2=TGC, 3=CSC, 4=ID, 5=COMBINED) */
    NTuple::Array<int>   m_IsectTargDist;      /**< Intersection: Target distance (0=Inner, 1=EE, 2=Middle, 3=Outer) */
    NTuple::Array<int>   m_IsectRegion;        /**< Intersection: Region (0=Barrel, 1=Endcap) */
    NTuple::Array<int>   m_IsectChamber;       /**< Intersection: Chamber number (-1 if not for chamber) */
    NTuple::Array<float> m_IsectPosEta;        /**< Intersection: Position Eta */
    NTuple::Array<float> m_IsectPosPhi;        /**< Intersection: Position Phi */
    NTuple::Array<float> m_IsectPosR;          /**< Intersection: Position R (Perp) */
    NTuple::Array<float> m_IsectPosZ;          /**< Intersection: Position Z */
    NTuple::Array<float> m_IsectDirEta;        /**< Intersection: Direction Eta */
    NTuple::Array<float> m_IsectDirPhi;        /**< Intersection: Direction Phi */
    NTuple::Array<float> m_IsectDirZ;          /**< Intersection: Direction Z */
    NTuple::Array<int>   m_IsectSegment;       /**< Intersection: Index of corresponding segment (Type=Fit, -1 if not) */

    //Segment parameters
    NTuple::Item<int>    m_NumSegs;             /**< Number of segments */
    NTuple::Array<int>   m_SegType;            /**< Segment: Type (0=Hough, 1=Linear) */
    NTuple::Array<int>   m_SegDistance;        /**< Segment: Distance (0=Inner, 1=EE, 2=Middle, 3=Outer)*/
    NTuple::Array<int>   m_SegRegion;          /**< Segment: Region (0=Barrel, 1=Endcap)*/
    NTuple::Array<int>   m_SegStation;         /**< Segment: Station ID (-1 if not for station)*/
    NTuple::Array<int>   m_SegHits;            /**< Segment: Number of Hits*/
    NTuple::Array<float> m_SegPosEta;          /**< Segment: Position Eta*/
    NTuple::Array<float> m_SegPosPhi;          /**< Segment: Position Phi*/
    NTuple::Array<float> m_SegPosR;            /**< Segment: Position R (Perp)*/
    NTuple::Array<float> m_SegPosZ;            /**< Segment: Position Z*/
    NTuple::Array<float> m_SegDirEta;          /**< Segment: Direction Eta */
    NTuple::Array<float> m_SegDirPhi;          /**< Segment: Direction Phi */
    NTuple::Array<float> m_SegDirZ;            /**< Segment: Direction Z */
    NTuple::Array<float> m_SegChi2;            /**< Segment: Chi2*/
    NTuple::Array<int>   m_SegNdf;             /**< Segment: Degrees of freedom */
    NTuple::Array<float> m_SegProb;            /**< Segment: Probability */
    NTuple::Array<int>   m_SegIsect;           /**< Segment: Index of corresponding Fit intersection (-1 if not set) */

    // Hit parameter s
    NTuple::Item<int>    m_NumHits;            /**< Number of hits */
    NTuple::Array<int>   m_HitId;              /**< Hit: Identifier of the Prepdata associated to this hit */
    NTuple::Array<int>   m_HitChamber;         /**< Hit: Index of chamber */
    NTuple::Array<int>   m_HitChamberID;       /**< Hit: Chamber ID */
    NTuple::Array<float> m_HitDeltaEta;        /**< Hit: Eta diff to chamber fine intersection */
    NTuple::Array<float> m_HitDeltaPhi;        /**< Hit: Phi diff to chamber fine intersection */
    NTuple::Array<float> m_HitEta;             /**< Hit: Eta */
    NTuple::Array<float> m_HitPhi;             /**< Hit: Phi */
    NTuple::Array<float> m_HitR;               /**< Hit: R (Perp) */
    NTuple::Array<float> m_HitZ;               /**< Hit: Z */
    NTuple::Array<int>   m_HitIsEta;           /**< Hit: Measures Eta (0=No, 1=Yes) */
    NTuple::Array<int>   m_HitDistance;        /**< Hit: Distance (0=Inner, 1=EE, 2=Middle, 3=Outer) */
    NTuple::Array<int>   m_HitTechnology;      /**< Hit: Technology (0=MDT, 1=RPC, 2=TGC, 3=CSC) */
    NTuple::Array<int>   m_HitRegion;          /**< Hit: Region (0=Barrel, 1=Endcap) */
    NTuple::Array<int>   m_HitStation;         /**< Hit: Station ID */
    NTuple::Array<int>   m_HitFineIsect;       /**< Hit: Chamber fine intersection number */
    NTuple::Array<int>   m_HitMdtIsect;        /**< Hit: Index of intersection evaluated from MDT fit from same distance (-1 if not set) */
    NTuple::Array<int>   m_HitRpcIsect;        /**< Hit: Index of intersection evaluated from RPC fit from same distance (-1 if not set) */
    NTuple::Array<int>   m_HitTgcIsect;        /**< Hit: Index of intersection evaluated from TGC fit from same distance (-1 if not set) */
    NTuple::Array<int>   m_HitCscIsect;        /**< Hit: Index of intersection evaluated from Csc fit from same distance (-1 if not set) */
    NTuple::Array<int>   m_HitCombIsect;       /**< Hit: Index of intersection evaluated from combined in same distance (-1 if not set) */
    NTuple::Array<int>   m_HitMdtIsect1;       /**< Hit: Index of intersection evaluated from MDT fit from other distance (-1 if not set) */
    NTuple::Array<int>   m_HitRpcIsect1;       /**< Hit: Index of intersection evaluated from RPC fit from other distance (-1 if not set) */
    NTuple::Array<int>   m_HitTgcIsect1;       /**< Hit: Index of intersection evaluated from TGC fit from other distance (-1 if not set) */
    NTuple::Array<int>   m_HitCscIsect1;       /**< Hit: Index of intersection evaluated from Csc fit from other distance (-1 if not set) */
    NTuple::Array<int>   m_HitCombIsect1;      /**< Hit: Index of intersection evaluated from combined in other distance (-1 if not set) */
    NTuple::Array<int>   m_HitHough;           /**< Hit: Index of Hough transform maximum to which this hit is associated (-1 if not set) */
    NTuple::Array<int>   m_HitInSegment;       /**< Hit: Participates in a segment (0=No, 1=Yes) */
    NTuple::Array<float> m_HitDriftRadius;     /**< Hit: Dirft radius if MDT hit */
    NTuple::Array<float> m_HitError;           /**< Hit: Error associated with the hit */

    // Hit/Segment association
    NTuple::Item<int>   m_NumHitSegMaps;      /**< Number of Hit/Segment associations */
    NTuple::Array<int>  m_HitSegMapHit;       /**< HitSegMap: Index of hit */
    NTuple::Array<int>  m_HitSegMapSeg;       /**< HitSegMap: Index of segment */

    // Number of Hits and Holes after GlobalFit
    NTuple::Item<int> m_RefittedTrkSummaryNumMdtHits;      /**< Number of MDT hits of the refitted track*/
    NTuple::Item<int> m_RefittedTrkSummaryNumTgcEtaHits;   /**< Number of TGC eta hits of the refitted track*/
    NTuple::Item<int> m_RefittedTrkSummaryNumTgcPhiHits;   /**< Number of TGC phi hits of the refitted track*/
    NTuple::Item<int> m_RefittedTrkSummaryNumCscEtaHits;   /**< Number of Csc eta hits of the refitted track*/
    NTuple::Item<int> m_RefittedTrkSummaryNumCscPhiHits;   /**< Number of Csc phi hits of the refitted track*/
    NTuple::Item<int> m_RefittedTrkSummaryNumRpcEtaHits;   /**< Number of Rpc eta hits of the refitted track*/
    NTuple::Item<int> m_RefittedTrkSummaryNumRpcPhiHits;   /**< Number of Rpc phi hits of the refitted track*/
    NTuple::Item<int> m_RefittedTrkSummaryNumMdtHoles;     /**< Number of MDT holes of the refitted track*/
    NTuple::Item<int> m_RefittedTrkSummaryNumTgcEtaHoles;  /**< Number of TGC eta holes of the refitted track*/
    NTuple::Item<int> m_RefittedTrkSummaryNumTgcPhiHoles;  /**< Number of TGC phi holes of the refitted track*/
    NTuple::Item<int> m_RefittedTrkSummaryNumCscEtaHoles;  /**< Number of Csc eta holes of the refitted track*/
    NTuple::Item<int> m_RefittedTrkSummaryNumCscPhiHoles;  /**< Number of Csc phi holes of the refitted track*/
    NTuple::Item<int> m_RefittedTrkSummaryNumRpcEtaHoles;  /**< Number of Rpc eta holes of the refitted track*/
    NTuple::Item<int> m_RefittedTrkSummaryNumRpcPhiHoles;  /**< Number of Rpc phi holes of the refitted track*/

    // Quantities from the Hough transform
    NTuple::Item<int>    m_NumHoughMaxima;      /**< Number of eta maxima found by the Hough trasform */
    NTuple::Array<float> m_MaximumSector;       /**< Maximum: Sector (0 - 15,  0=Large) */
    NTuple::Array<float> m_MaximumDistance;     /**< Maximum: Distance (0=Inner, 1=EE, 2=Middle, 3=Outer) */
    NTuple::Array<float> m_MaximumRegion;       /**< maximum: Region (0=Barrel, 1=Endcap) */
    NTuple::Array<float> m_MaximumValue;        /**< Maximum value, i.e. the number of hit in the bin of the maximum */
    NTuple::Array<float> m_MaximumPosition;     /**< Position of maximum: Z for the barrel, R for the endcap */
    NTuple::Array<float> m_MaximumEta;          /**< Eta position of maximum */
    NTuple::Array<float> m_MaximumTheta;        /**< Theta angle w.r.t. the reference plane, i.e. the medium plane of the MDT */
    NTuple::Array<float> m_MaximumRefPlane;     /**< Reference plane position: R for the barrel, Z for the endcap */
    NTuple::Array<float> m_MaximumLowPos;       /**< Lower width of the maximum */
    NTuple::Array<float> m_MaximumUppPos;       /**< Upper width of the maximum */
    NTuple::Array<int>   m_TriggerConfirmed;    /**< Number of in time trigger hits associated to the maximum */

    NTuple::Item<int>    m_NumHoughHits;        /**< Number of hits associated to the Hough transform maxima */
    NTuple::Array<float> m_HoughHitDeltaEta;    /**< Hough Hit: eta diff to the Hough maximum */
    NTuple::Array<float> m_HoughHitEta;         /**< Hough Hit: Eta , computed from PrepRawData not from RioOnTrack */
    NTuple::Array<float> m_HoughHitPhi;         /**< Hough Hit: Phi , computed from PrepRawData not from RioOnTrack */
    NTuple::Array<float> m_HoughHitR;           /**< Hough Hit: R (Perp) , computed from PrepRawData not from RioOnTrack */
    NTuple::Array<float> m_HoughHitZ;           /**< Hough Hit: Z , computed from PrepRawData not from RioOnTrack */
    NTuple::Array<int>   m_HoughHitIsEta;       /**< Hough Hit: Measures Eta (0=No, 1=Yes) */
    NTuple::Array<int>   m_HoughHitDistance;    /**< Hough Hit: Distance (0=Inner, 1=EE, 2=Middle, 3=Outer) */
    NTuple::Array<int>   m_HoughHitTechnology;  /**< Hough Hit: Technology (0=MDT, 1=RPC, 2=TGC, 3=CSC) */
    NTuple::Array<int>   m_HoughHitRegion;      /**< Hough Hit: Region (0=Barrel, 1=Endcap) */
    NTuple::Array<int>   m_HoughHitSector;      /**< Hough Hit: Sector (0 - 15,  0=Large) */
    NTuple::Array<int>   m_HoughMaximum;        /**< Hough Hit: index of the Hough transform maximum to which this Hit is associated (-1 if not set) */
    NTuple::Array<int>   m_HoughMuGirlHit;      /**< Hough Hit: index of the MuGirl Hit (previous Hit block) retrieved with the intersections (-1 if not set) */

    // NTuple::Item<int> m_MSRefittedTrkSummaryFlag;  /**<Is track an MS track*/

};

}

#endif //MUGIRL_CANDIDATENTUPLE_H
