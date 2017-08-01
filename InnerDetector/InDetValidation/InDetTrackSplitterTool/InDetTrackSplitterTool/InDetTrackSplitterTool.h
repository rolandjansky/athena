/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETINDETTRACKSPLITTERTOOL_H
#define INDETINDETTRACKSPLITTERTOOL_H

/**
  InDetTrackSplitterTool.h
  AUTHORS: John Alison
*/

#include "InDetTrackSplitterTool/IInDetTrackSplitterTool.h"

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"


class TRT_ID;
class SCT_ID;

class IInDetTrackSplitterTool;

namespace Trk {
  class ITrackFitter ;
  class ITrackSelectorTool;
  class RIO_OnTrack;
  class MeasurementBase;
  class MeasurementBaseComparisonFunction;
  class PseudoMeasurementOnTrack;
  class StraightLineSurface;
}


namespace InDet{

  /** @class InDetTrackSplitterTool 
      
  This tool is used for splitting tracks in different segments and returning fitted tracks that
  have the corrisponding hits associated with them.
  
  Currently it is for seen to split tracks in:
  
     - Upper and Lower tracks (for cosmics)
    
     - Tracks using alternating hits. (odd and even)
  
     - Strip a combined (Si+TRT) track of its Si hits
        refit remianing TRT-Only hits with theta and z0
  constraint from input track.

     - Strip a combined (Si+TRT) track of its TRT hits
        refit remianing Si-Only hits with qOverP
  constraint from input track.

  Usages:
  1) If you have a single track and want to split in to upper and lower do:

     m_trackSplitterTool->splitInUpperLowerTrack(inputtrack);
                        ->splitInOddEvenHitsTrack(inputtrack);
  
     this call return a std::pair containing the split tracks
  
  3) If you have a single track and want to strip ether Si or TRT hits do:
     
     m_trackSplitterTool->stripTrack(inputtrack,true); //For removing the Si Hits
     m_trackSplitterTool->stripTrack(inputtrack,false);//For removing the TRT Hits

  2) If you have a track collection and want to create track collections with upper and lower tracks do
  
     m_trackSplitterTool->splitTracks(inputTracks);
  
  then retrieve the collection from storeGate with the names m_outputUpperTracksName, m_outputLowerTracksName
    
  @author  John Alison*/  

  class InDetTrackSplitterTool : public AthAlgTool, virtual public IInDetTrackSplitterTool
    {
      
    public:
      /** default constructor */
      InDetTrackSplitterTool(std::string const&, std::string const&,IInterface const *);
      
      /** default destructor */
      virtual ~InDetTrackSplitterTool();

      /** standard Athena-Algorithm methods */
      virtual StatusCode initialize();
      virtual StatusCode finalize();
      
      /** Takes a trackCollection, splits them according to upper and lower parts and fills
    two track collections, one with upper tracks and one with lower tracks
         which are accessed via getUpperTracks and getLowerTracks 
    
   Using this method requires track to pass trackIsCandidate 
   (for the moment this is just a d0 cut requiring the track went through TRT cavity*/
      virtual void splitTracks(TrackCollection const* inputTracks) const;
      
      /** Splits a single input track into upper and lower parts (based on global y) 
    returns a pair of track the first being the upper*/
      virtual std::pair<Trk::Track*, Trk::Track*> splitInUpperLowerTrack(Trk::Track const& input, bool siliconHitsOnly = false) const;

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
      virtual Trk::Track* stripTrack(Trk::Track const& input, bool removeSilicon = true, bool applyConstraint = true) const;
      
      /** Splits a single input track into odd and even parts (with logic to aviod splitting SCT space points)
       */
      virtual std::pair<Trk::Track*, Trk::Track*> splitInOddEvenHitsTrack(Trk::Track const& input) const;

      /** Return the names of the track collections stored in storeGate
       */
      std::string const inline UpperTracksKey() const {return m_outputUpperTracksName.key();}
      std::string const inline LowerTracksKey() const {return m_outputLowerTracksName.key();}

    private:
      
      /** Track selection applied in the splitTracks method
    (for the moment this is just a d0 cut requiring the track went through TRT cavity
      */ 
      bool trackIsCandidate(Trk::Track const& inputTrack) const;
      
      /** Return a vector of the SCT hits on track
       */
      std::vector<Trk::MeasurementBase const*> getSCTHits(Trk::Track const& input) const;

      /** Logic to check if there is another SCT hit associated with the input hit, which forms a space point
       */
      std::vector<Trk::MeasurementBase const*>::iterator findSCTHitsFromSameSpacePoint(Trk::MeasurementBase const* sctHit, std::vector<Trk::MeasurementBase const*>& listOfSCTHits) const;

      /** Logic to check if the track is constrained given the number of various types of hits
       */
      bool isConstrained(unsigned int nPixelHits, unsigned int nSCTHits, unsigned int nTRTHits, unsigned int nPseudomeasurements) const;
      
      /** Strip the Si hits, fit the remaining with a theta, z0 constraint.
       */
      Trk::Track* stripSiFromTrack(Trk::Track const& input) const;
      
      /** Make the theta and z0 constraint 
       */
      Trk::PseudoMeasurementOnTrack const* makeThetaZ0Constraint(Trk::Perigee const* originialPerigee) const;

      /** Strip the TRT hits, fit the remaining with a qOverP constraint
       */
      Trk::Track* stripTRTFromTrack(Trk::Track const& input, bool applyConstraint = true) const;
      
      /** Make the qOverP constraint
       */
      Trk::PseudoMeasurementOnTrack const* makePConstraint(Trk::Perigee const* originialPerigee
                 ,Trk::StraightLineSurface const* trtSurf) const;

      /** Output track collection names recorded in storgate 
       */
      SG::WriteHandleKey<TrackCollection> m_outputUpperTracksName;
      SG::WriteHandleKey<TrackCollection> m_outputLowerTracksName;
      
      /** Helper Functions */
      ToolHandle<Trk::ITrackFitter>  m_trkfitter;
      TRT_ID const* m_trtid ;
      SCT_ID const* m_sctid ;
      
      /** Do we use the material on the input track
       */
      bool m_keepmaterial;
      
    };
}


#endif 
