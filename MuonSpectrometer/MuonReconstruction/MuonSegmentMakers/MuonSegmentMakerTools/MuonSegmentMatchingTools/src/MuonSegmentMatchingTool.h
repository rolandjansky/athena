/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENTMATCHINGTOOL_H
#define MUON_MUONSEGMENTMATCHINGTOOL_H

#include "MuonSegmentMakerToolInterfaces/IMuonSegmentMatchingTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentInOverlapResolvingTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentPairMatchingTool.h"
#include "TrkGeometry/MagneticFieldProperties.h"

#include <atomic>

namespace Muon {
  
  class MuonSegment;

  /**
     @brief tool to match segments using a curved trajectory. The tool decides whether the matching should be 
     performed with curvature or with a straight line. 
     Cases in which a straight line will be used:
     - no magnetic field in the muon spectrometer
     - endcap middle/outer combination
     - small/large overlap in one station layer
  */
  class MuonSegmentMatchingTool : virtual public IMuonSegmentMatchingTool, public AthAlgTool {
  public:
    /** @brief constructor */
    MuonSegmentMatchingTool(const std::string&,const std::string&,const IInterface*);

    /** @brief destructor */
    virtual ~MuonSegmentMatchingTool()=default;
    
    /** @brief AlgTool initilize */
    StatusCode initialize();
    
    /** @brief AlgTool finalize */
    StatusCode finalize();
    
    /** @brief match two segments */
    bool match( const MuonSegment& seg1, const MuonSegment& seg2 ) const;

  private:
    /** @brief perform straight line matching using SL extrapolation */
    bool straightLineMatch( const MuonSegment& seg1, const MuonSegment& seg2 ) const;

    /** @brief perform curved matching */
    bool curvedMatch( const MuonSegment& seg1, const MuonSegment& seg2 ) const;

    /** @brief perform overlap matching */
    bool overlapMatch( const MuonSegment& seg1, const MuonSegment& seg2 ) const;

    /** @brief setup field */
    bool initializeField() const;

    /** @brief check whether the two segments have a stereo angle */
    bool hasStereoAngle( const Identifier& id1, const Identifier& id2 ) const;
    
    /** @brief check whether we should perform a straight line match */
    bool isSLMatch( const Identifier& chid1, const Identifier& chid2 ) const;
        
    /** @brief Suppress noise from cavern background/pile up using basic cuts  */
    bool suppressNoise( const MuonSegment& seg1, const MuonSegment& seg2, const bool& useTightCuts ) const;
    
    /** @brief Suppress noise from cavern background/pile up using basic cuts in phi */
    bool suppressNoisePhi( const MuonSegment& seg1, const MuonSegment& seg2, const bool& useTightCuts ) const;

    /** @brief extrapolate segment in middle or outer endcap station to inner layer assuming the particle came from the IP */
    void simpleEndcapExtrapolate(double x_segment, double y_segment, double z_segment, double theta_segment, double z_extrapolated, 
				 double& r_expected, double& theta_expected, double& rhoInv) const;

    
    /** @brief match an endcap middle or outer segment with an inner segment using a simple analytic extrapolation model */
    bool endcapExtrapolationMatch( const MuonSegment& seg1, const MuonSegment& seg2, bool useTightCuts ) const;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ServiceHandle<Muon::IMuonEDMHelperSvc>               m_edmHelperSvc {this, "edmHelper", 
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
      "Handle to the service providing the IMuonEDMHelperSvc interface" };           //!< EDM Helper tool
    ToolHandle<Muon::MuonEDMPrinterTool>                 m_printer;              //!< EDM printer tool
    ToolHandle<Muon::IMuonSegmentInOverlapResolvingTool> m_overlapResolvingTool; //!< matching tool to handle the overlaps
    ToolHandle<Muon::IMuonSegmentPairMatchingTool>       m_pairMatchingTool;     //!< matching tool to handle the pairs of segments

    Gaudi::Property<bool> m_toroidOn { this, "ToroidOn", true, "Status of toroidal B-Field" };

    mutable std::atomic_uint m_straightLineMatches;
    mutable std::atomic_uint m_straightLineMatchesGood;
    mutable std::atomic_uint m_overlapMatches;
    mutable std::atomic_uint m_overlapMatchesGood;
    mutable std::atomic_uint m_curvedMatches;
    mutable std::atomic_uint m_curvedMatchesGood;

    bool m_isCosmics;
    bool m_doOverlapMatch;
    bool m_doStraightLineMatch;
    bool m_doCurvedMatch;
    bool m_thetaMatch;
    bool m_phiMatch;
    bool m_useLocalAngles;
 
    // cuts for straight line match
    double m_straightLineMatchAngleCut;
    double m_straightLineMatchPositionCut;

    // cuts for sl overlap match
    double m_overlapMatchAngleDPhiCut; //!< cut of the angular difference between phi from phi match and phi from positions
    double m_overlapMatchAngleDYZCut; //!< cut of the angular difference between phi from phi match and phi from positions
    double m_overlapMatchPositionCut;  //!< cut on the distance of best position from the chamber bounds
    double m_overlapMatchPositionResidualCut; //!< cut on the position residual for the best position match
    double m_overlapMatchPhiHitPullCut; //!< cut on the average pull of the phi hits with the new segment parameters

    // cuts for matching segments from different stations
    double m_angleABCut;
    double m_maxDistSegments; //!< cut on the maximum distance between the segments
    double m_minDistSegmentsCosmics; //!< cut on the minimum distance between the segments, if the distance is larger than the cut the segments are always matched (for cosmics)
    double m_matchingbibm_lphisec;
    double m_matchingbibo_lphisec;
    double m_matchingbmbo_lphisec;
    double m_matchingeiem_lphisec;
    double m_matchingeieo_lphisec;
    double m_matchingemeo_lphisec;
    double m_matchingbibm_sphisec;
    double m_matchingbibo_sphisec;
    double m_matchingbmbo_sphisec;
    double m_matchingeiem_sphisec;
    double m_matchingeieo_sphisec;
    double m_matchingemeo_sphisec;
    
    double m_matchingbee_sphisec;
    
    bool   m_onlySameSectorIfTight;  //!< reject all segments in different sectors if in tight matching
    bool   m_useTightCuts;  //!< only apply tight selection for busy combinations
    bool   m_dumpAngles;    //!< dump matching angle info to screen

    bool   m_useEndcapExtrapolationMatching;
    double m_drExtrapRMS;
    double m_drExtrapAlignmentOffset;
    double m_dthetaExtrapRMS;
  };

}

#endif
