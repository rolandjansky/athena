/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REC_IMUONSEGMENTTAGGERVALIDATIONTOOL_H
#define REC_IMUONSEGMENTTAGGERVALIDATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "MuonSegmentMakerToolInterfaces/IMuonSegmentHitSummaryTool.h"
#include "MuonSegmentTaggerToolInterfaces/IMuTagMatchingTool.h"


static const InterfaceID IID_IMuonSegmentTaggerValidationTool("Muon::IMuonSegmentTaggerValidationTool",1,0);

namespace Analysis {
  class Muon;
}

namespace Rec {
  class TrackParticle;
}

namespace Muon {
  class MuonSegment;
}

namespace Trk {
  class Track;
}

namespace Rec {

  /** Interface for tools calculating hit count summaries for track */
  class IMuonSegmentTaggerValidationTool : virtual public IAlgTool {      
  public:

    struct Summary {
      /** constructor, takes ownership of track parameters pointer */
      Summary() : trackAtSegement(0),segmentInfo(),t0(-1e9) {}
      
      /** destructor */
      ~Summary() {
	clear();
      }
      
      /** copy constructor */
      Summary( const Summary& sum ) {
	copy(sum);
      }
      
      /** assignment operator */
      Summary& operator=(const Summary& sum){
	if( this!=&sum ){
	  clear();
	  copy(sum);
	}
	return *this;
      }

      /** track extrapolated to the segment */
      const Trk::TrackParameters* trackAtSegement;
      
      /** MuTagSegmentInfo */
      IMuTagMatchingTool::MuTagSegmentInfo segmentInfo;

      /** fitted t0 */
      double t0;

      private:
      void clear() { if( trackAtSegement ) {
	  delete trackAtSegement; 
	  trackAtSegement = 0;
	}
      }
      void copy( const Summary& sum ){
	trackAtSegement = sum.trackAtSegement ? sum.trackAtSegement->clone() : 0;
	segmentInfo = sum.segmentInfo;
	t0 = sum.t0;
      }
    };

  public:
    /** IAlgTool interface */
    static const InterfaceID& interfaceID();

    /**  returns whether the Muon satisfies the new MuTagIMO cuts */
    virtual bool goodMuTagIMOMuon( const Analysis::Muon& muon ) const = 0;

    /** create summary for a Muon, default uses the ID track to calculate the summaries, if useIDTrack == false will use combined track if present */
    virtual std::vector<IMuonSegmentTaggerValidationTool::Summary> create( const Analysis::Muon& muon, bool useIDTrack = true ) const = 0;

    /** create summary for a list of segments and a track */
    virtual std::vector<Summary> create( const Trk::Track& track, const std::vector<const Muon::MuonSegment*>& segments ) const = 0;

    /** create summary for a give segment */
    virtual Summary create( const Muon::MuonSegment& segment, const Trk::TrackParameters& pars ) const = 0;


  };
  
  inline const InterfaceID& IMuonSegmentTaggerValidationTool::interfaceID()
  {
    return IID_IMuonSegmentTaggerValidationTool;
  }


} // end of name space

#endif // IMuonSegmentTaggerValidationTool_H

