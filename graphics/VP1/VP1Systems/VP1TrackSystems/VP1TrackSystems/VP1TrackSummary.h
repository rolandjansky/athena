/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1TrackSummary                     //
//                                                            //
//  Description: A few basic helpers providing track summary  //
//               information.                                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: November 2009                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1TRACKSUMMARY_H
#define VP1TRACKSUMMARY_H
#include <set>

namespace Trk { class Track; class Segment; class MeasurementBase; class TrkDetElementBase;}
class AtlasDetectorID;

class VP1TrackSummary {
public:

  static bool countHits( const Trk::Track*,
       unsigned& nPixelHits,
       unsigned& nSCTHits,
       unsigned& nTRTHits,
       unsigned& nhits_muon_phi, 
       unsigned& nhits_rpc, 
       unsigned& nhits_mdt, 
       unsigned& nhits_tgc, 
       unsigned& nhits_csc
       );
       
   static bool countHits( const Trk::Segment*,
        unsigned& nPixelHits,
        unsigned& nSCTHits,
        unsigned& nTRTHits,
        unsigned& nhits_muon_phi, 
        unsigned& nhits_rpc, 
        unsigned& nhits_mdt, 
        unsigned& nhits_tgc, 
        unsigned& nhits_csc
        );
        
    static void addCounts( std::set<const Trk::TrkDetElementBase*>& detelems, const Trk::MeasurementBase* meas,
        const AtlasDetectorID * idhelper,
        unsigned& nPixelHits,
        unsigned& nSCTHits,
        unsigned& nTRTHits,
        unsigned& nhits_muon_phi, 
        unsigned& nhits_rpc, 
        unsigned& nhits_mdt, 
        unsigned& nhits_tgc, 
        unsigned& nhits_csc
        );
        
private:
  VP1TrackSummary();
  ~VP1TrackSummary();

};

#endif
