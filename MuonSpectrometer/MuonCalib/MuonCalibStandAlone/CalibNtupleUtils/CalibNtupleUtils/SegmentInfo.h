/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SEGMENTINFO_H 
#define SEGMENTINFO_H

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include <iostream>
#include <map>

namespace MuonCalib{

  class MuonCalibSegment;
  class MuonCalibPattern;
  /**
     @class SegmentInfo
     SegmentInfo is used in MultipleAnalysis (MuonSpectrometer/MuonCalibExtraNtuple), as a helper class to store Segment(quality) info on.
  */

  struct SegmentInfo {

    double chi2;         //!< chi2 of the segment
    double dthetaPat;    //!< difference of theta with respect to the MuonCalibPattern
    double dZPat;        //!< difference of Z with respect to the MuonCalibPattern
    double dRPat;        //!< difference of R with respect to the MuonCalibPattern
    double dphiT;        //!< difference of phi with respect to the MuonCalibTruth
    double dthetaT;      //!< difference of theta with respect to the MuonCalibTruth
    double dZT;          //!< difference of Z with respect to the MuonCalibTruth
    double dRT;          //!< difference of R with respect to the MuonCalibTruth

    double segQ();       //!< Quality-variable taking into account Out-of-time hits, empty tubes and deltas, Hits-on-track and wether triggerhits are available 
    double segQ2();      //!< Quality-variable taking into account the difference between the theta-variable with respect to MuonCalibPattern, as well as the segQ variable.

    int nCsc;            //!< nr of CSC hits
    int nHots;           //!< nr of Hits-on-Track: driftcircle compatible with distance to track
    int nHoos;           //!< nr of Out-of-Time hits: driftcircle larger than distance to track
    int nDeltas;         //!< nr of Delta-type of hits: driftcircle smaller than distance to track
    int emptyTubes;      //!< nr of missing hits
    int nch;             //!< nr of stations used to construct segment (overlapping stations)
    int nchT;            //!< nr of stations used to construct segment (overlapping stations) in MuonCalibTruth
    int nTrigEta;        //!< nr of trigger eta hits
    int nTrigPhi;        //!< nr of trigger phi hits
    int trigHitRegion;   //!< 1 if segment should have trigger hits
    int ntruthHits;      //!< nr of MuonCalibXxxTruthHits
    int nWrong;          //!< nr of wrong hits 
    int besttruthHits;   //!< 
    int simMatchOK;      //!< 1 if nHOTS>6 and truthmatch is passed
    std::string station; //!< station of the segment

    int nGoodSegPat;     //!< number of segments with Q<20 within the MuonCalibPattern
    int stationCode;     //!< code for station: id.stationName()*1000+100*id.phi()+id.eta();
  
    int selected;        //!< number of segments selected and matched to truth
    int matchFlag;       //!< (best) segment matched to truth
    int phiPat;          //!< 1 if the Segments contains phi hits
    int regionId;        //!< code to distinguish between station types ('B'/'E', 'I'/'M'/'O', 'S'/'L' etc)

    double theta;        //!< theta of the segment
    double invcurvature; //!< q/p of the segment
    int imeth;           //!< method of extrapolation (barrel ==1, endcap == 0)

    std::map<int,int> barcodeMap;

    const MuonCalibSegment*  seg;
    const MuonCalibPattern* pat;
    
    int trigFlag() const
    {
      if( !phiPat ) return 4;
      if( !trigHitRegion ) return 3;
      if( nTrigPhi && nTrigEta ) return 2;
      if( !nTrigPhi ) return 1;
      return 0;
    }
    
    int missedHits() const
    {
      return emptyTubes + nDeltas + nHoos;
    } 

    std::ostream& print( std::ostream& os) const;

    int barcode() 
    { 
      std::map<int,int>::iterator it = barcodeMap.begin();
      std::map<int,int>::iterator it_end = barcodeMap.end();
      int nhitmax=0;
      int bc=0;
      for(; it!=it_end;it++) {
       if (it->second>nhitmax) {
        nhitmax = it->second;
        bc = it->first;
       }
      }

      if (barcodeMap.size()>1) {
        MsgStream log(Athena::getMessageSvc(),"TimedSegment");
        log<<MSG::WARNING<<"More than one barcode in segment " << barcodeMap.size()<<endmsg;
      }

      return bc;
    }
  };
  typedef std::map<const MuonCalibSegment*,SegmentInfo>  SegInfoMap;
  typedef SegInfoMap::iterator SegInfoIt;

}

std::ostream& operator<<( std::ostream& os, const MuonCalib::SegmentInfo& info);

#endif
