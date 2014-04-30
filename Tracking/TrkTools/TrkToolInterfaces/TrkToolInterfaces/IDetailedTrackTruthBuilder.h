/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDETAILEDTRACKTRUTHBUILDER_H
#define IDETAILEDTRACKTRUTHBUILDER_H

#include <vector>
#include "GaudiKernel/IAlgTool.h"

#include "TrkTrack/TrackCollection.h" /* forward declaring the typedef would be ugly.. */

// Forard declarations of types.
class PRD_MultiTruthCollection;
class DetailedTrackTruthCollection;

namespace Trk {
  
  static const InterfaceID IID_IDetailedTrackTruthBuilder("Trk::IDetailedTrackTruthBuilder",1,0);

  /** @brief Provides interface for tool to return a "detailed" track truth map.

      @author Andrei Gaponenko <agaponenko@lbl.gov>  */
  class IDetailedTrackTruthBuilder : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID() { return IID_IDetailedTrackTruthBuilder; }

    /** The main tool method.  Fills a pre-existing
     * DetailedTrackTruthCollection pointed to by output with data for
     * the tracks, using the prdTruth collections.  pdrTruth is
     * allowed to contain null pointers.  Each non-null pointer must
     * correspond to collection for a single subdetector.
     *
     * This method does not impose a memory management model on the
     * caller. (As e.g. returning an object on the heap would do.)
     * The caller can use it to work with stack or heap objects. 
     * Call with output==0 is a no-op. (But output should not be garbage, it must be initialized.)
     */
    virtual void buildDetailedTrackTruth(DetailedTrackTruthCollection *output,
					 const TrackCollection& tracks,
					 const std::vector<const PRD_MultiTruthCollection*>& prdTruth) = 0;
  };
  
} // namespace Trk

#endif/*IDETAILEDTRACKTRUTHBUILDER_H*/
