/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuonCombinedTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IRECMUONCOMBINEDTOOL_H
#define IRECMUONCOMBINEDTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateToTagMap.h"
#include "TrkTrack/TrackCollection.h"
#include <vector>

namespace MuonCombined {

  static const InterfaceID IID_IMuonCombinedTool("MuonCombined::IMuonCombinedTool", 1, 0);

  /** @class IMuonCombinedTool
      @brief interface for tools building combined muons from ID and Muon candidates
 
      @author Niels van Eldik
   */

  class IMuonCombinedTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;


    /**IMuonCombinedTool interface: build combined muons from ID and MS candidates */    
    virtual void combine( const MuonCandidateCollection& muonCandidates,  const InDetCandidateCollection& inDetCandidates, std::vector<InDetCandidateToTagMap*> tagMaps,
			  TrackCollection* combTracks, TrackCollection* METracks) const = 0;

    virtual void cleanUp() const = 0;

  };

  inline const InterfaceID& IMuonCombinedTool::interfaceID()
    { 
      return IID_IMuonCombinedTool; 
    }

} // end of namespace

#endif 
