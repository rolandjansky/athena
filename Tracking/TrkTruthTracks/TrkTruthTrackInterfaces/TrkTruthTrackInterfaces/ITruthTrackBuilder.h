/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITruthTrackBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_TRKTRUTHTRACKINTERFACES_ITRUTHTRACKBUILDER_H
#define TRK_TRKTRUTHTRACKINTERFACES_ITRUTHTRACKBUILDER_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "TrkTruthTrackInterfaces/PRD_TruthTrajectory.h"
#include "TrkSegment/SegmentCollection.h"


namespace Trk {

  class Track;

  static const InterfaceID IID_ITruthTrackBuilder("ITruthTrackBuilder", 1, 0);
   
  /**
   @class ITruthTrackBuilder

   @brief The interface for the truth track finder
       
   @author Andreas.Salzburger -at- cern.ch, Thijs.Cornelissen -at- cern.ch
   */
     
  class ITruthTrackBuilder : virtual public IAlgTool {

     public:     
       /** Virtual destructor */
       virtual ~ITruthTrackBuilder(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_ITruthTrackBuilder; }

       /** return a track from a PrepRawData trajectories, if a segment collection pointer is provided it will also try filling it **/
       virtual Track* createTrack(const PRD_TruthTrajectory& traj, SegmentCollection* segs = 0 ) const = 0;

  };

} // end of namespace

#endif // TRK_TRKTRUTHTRACKINTERFACES_ITRUTHTRACKBUILDER_H
