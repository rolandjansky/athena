/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IINDETINDETTRACKSPLITTERTOOL_H
#define IINDETINDETTRACKSPLITTERTOOL_H

// Include files
#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

//class Identifier;
namespace InDet{
  
  static const InterfaceID IID_InDetTrackSplitterTool("InDet::InDetTrackSplitterTool", 1, 0);
  
  class IInDetTrackSplitterTool : virtual public IAlgTool {
    
  public:
    /** Destructor */
    virtual ~IInDetTrackSplitterTool(){};
    
    /// Retrieve interface ID
    static InterfaceID const& interfaceID() {return IID_InDetTrackSplitterTool;};
    
    /** Takes a trackCollection, splits them according to upper and lower parts and fills
	two track collections, one with upper tracks and one with lower tracks
	which are accessed via getUpperTracks and getLowerTracks 
	
	Using this method requires track to pass trackIsCandidate 
	(for the moment this is just a d0 cut requiring the track went through TRT cavity*/
    virtual void splitTracks(TrackCollection const* inputTracks) const = 0;

    /** Takes a combined ID track and either
	1) if removeSilicon = true
	   Strips off the Si hits.  
	   Then refits the remaining TRT hits using the original 
	   z0 and eta0 as constratins on the fit.
	2) if removeSilicon = false
	   Strips off the TRT hits.
	   Then refits the remaining Si hits using the original
	   pT and phi as constratins on the fit
    */
    virtual Trk::Track* stripTrack(Trk::Track const& input, bool removeSilicon = true, bool applyConstraint = true) const =0;
    
    /** Splits a single input track into upper and lower parts (based on global y) 
	returns a pair of track the first being the upper*/
    virtual std::pair<Trk::Track*, Trk::Track*> splitInUpperLowerTrack(Trk::Track const& input, bool siliconHitsOnly = false) const = 0;
    
    /** Splits a single input track into odd and even parts (Not yet implemented)
     */
    virtual std::pair<Trk::Track*, Trk::Track*> splitInOddEvenHitsTrack(Trk::Track const& input) const = 0;

    /** Return the names of the track collections stored in storeGate
     */
    virtual std::string const UpperTracksKey() const = 0;
    virtual std::string const LowerTracksKey() const = 0;
  };
}
  
#endif 
  
