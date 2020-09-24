/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBEXTRANTUPLEANALYSIS_MUONCALIBSELECTOR_H
#define MUONCALIBEXTRANTUPLEANALYSIS_MUONCALIBSELECTOR_H

#include "CxxUtils/checker_macros.h"

#include <vector>

namespace MuonCalib {
  
  class MuonCalibExtendedSegment;
  class MuonCalibExtendedTrack;
  class MuonCalibTrackSummary;

  class ATLAS_NOT_THREAD_SAFE MuonCalibSelector {
  public: 
    typedef std::vector<const MuonCalibExtendedSegment*> SegVec;
    typedef SegVec::iterator                             SegIt;
    typedef SegVec::const_iterator                       SegCit;

    typedef std::vector<const MuonCalibExtendedTrack*>   TrackVec;
    typedef TrackVec::iterator                           TrkIt;
    typedef TrackVec::const_iterator                     TrkCit;
    
    struct Selection {
      Selection() : nminMdtHits(3),nminMdtMl1(0),nminMdtMl2(0),nminMdtCh(0),nmaxMdtCh(1000),
	   nminPhiTrigHits(0),nminEtaTrigHits(0),nmaxOutliers(100000),nmaxPseudo(1000),maxChi2Ndof(1e9) {}
      virtual ~Selection() {}

      unsigned int nminMdtHits; //!< minimum number of MDT hits
      unsigned int nminMdtMl1;  //!< minimum number of MDT hits in ml 1
      unsigned int nminMdtMl2;  //!< minimum number of MDT hits in ml 1
      unsigned int nminMdtCh;   //!< minimum number of chambers MDT
      unsigned int nmaxMdtCh;   //!< maximum number of chambers MDT
      unsigned int nminPhiTrigHits;  //!< minimum number of phi trigger hits 
      unsigned int nminEtaTrigHits;  //!< minimum number of eta trigger hits 
      unsigned int nmaxOutliers; //!< maximum number of outliers 
      unsigned int nmaxPseudo; //!< maximum number of pseudo-measurements
      double maxChi2Ndof; //!< maximum chi2/ndof
    };

    struct SegmentSelection : public Selection {
      SegmentSelection() : hasFittedT0(false),hasAssociatedTrack(false),singleML(false),twoML(false),minAdcGoodFraction(0.) {}
      bool hasFittedT0;
      bool hasAssociatedTrack;
      bool singleML;
      bool twoML;
      double minAdcGoodFraction;
    };

    struct TrackSelection : public Selection {
      TrackSelection() : barrelTrack(false),endcapTrackA(false),endcapTrackC(false),maxr0(10000),maxz0(50000) {}
      bool barrelTrack;
      bool endcapTrackA;
      bool endcapTrackC;
      double maxr0; //!< maximum transverse impact parameter wrt I.P.
      double maxz0; //!< maximum longitudinal impact parameter at I.P.
    };

  public:
    // default constructor
    MuonCalibSelector();


    /** select segments satisfying selection. If a selection is passed it will be used, else default */
    SegVec select( const SegVec& segments, const Selection* selection = 0 ) const;

    /** select tracks satisfying selection. If a selection is passed it will be used, else default */
    TrackVec select( const TrackVec& tracks, const Selection* selection = 0 ) const;

    /** check whether segment passes selection. If a selection is passed it will be used, else default */
    bool select( const MuonCalibExtendedSegment& segment, const Selection* selection = 0 ) const;

    /** check whether track passes selection. If a selection is passed it will be used, else default */
    bool select( const MuonCalibExtendedTrack& track, const Selection* selection = 0 ) const;

    /** check whether summary passes selection. If a selection is passed it will be used, else default */
    bool select( const MuonCalibTrackSummary& summary, const Selection* selection = 0 ) const;

  private:
    Selection m_selection;
  };

}
#endif
