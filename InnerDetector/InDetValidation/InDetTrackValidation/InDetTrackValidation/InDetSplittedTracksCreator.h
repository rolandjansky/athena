/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// InDetSplittedTracksCreator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDET_INDETSPLITTEDTRACKSCREATOR_H
#define INDET_INDETSPLITTEDTRACKSCREATOR_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkTrack/TrackCollection.h"

namespace InDet {

/** @class InDetSplittedTracksCreator
 
   Validation Algorithm to retrieve, a track collection from cosmics events, split it in upper half and lower half tracks
   and record it to storegate

   @author Manuel.Kayl@cern.ch
*/
  class IInDetTrackSplitterTool;

class InDetSplittedTracksCreator : public AthReentrantAlgorithm {
public:
    /** Standard Athena-Algorithm Constructor */
    InDetSplittedTracksCreator(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~InDetSplittedTracksCreator();

    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode execute_r(const EventContext& ctx) const;
    /** standard Athena-Algorithm method */
    StatusCode          finalize();

private:
    /**  holds the name of the track coll to be used*/
    SG::ReadHandleKey<TrackCollection>     m_TrackCol;
    /**  holds the names the output track collection written to store gate*/
    SG::WriteHandleKey<TrackCollection>    m_OutputTrackCol;
    /** flag for using TRT standalone tracks **/
    //bool                                   m_makeTRTStandaloneTracks;
    /** flag for using si only tracks **/
    bool                                   m_makeSiOnlyTracks;
    /** flag to accept the upper segment **/
    bool                                   m_takeUpperSegment;
    /** flag to accept the lower segment **/
    bool                                   m_takeLowerSegment;
    /** instances of IInDet */
    ToolHandle< InDet::IInDetTrackSplitterTool >  m_trackSplitterTool;

};

} // close of namespace

#endif // INDET_INDETSPLITTEDTRACKSCREATOR_H

