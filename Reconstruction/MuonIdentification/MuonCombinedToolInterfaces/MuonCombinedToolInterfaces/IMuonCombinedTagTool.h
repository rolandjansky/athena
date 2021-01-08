/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuonCombinedTagTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IRECMUONCOMBINEDTAGTOOL_H
#define IRECMUONCOMBINEDTAGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/TrackCollection.h"
#include <vector>


namespace MuonCombined {
  class InDetCandidate;
  class MuonCandidate;
  class InDetCandidateToTagMap;

  static const InterfaceID IID_IMuonCombinedTagTool("MuonCombined::IMuonCombinedTagTool", 1, 0);

  /** @class IMuonCombinedTagTool
      @brief interface for tools building combined muons from a MuonCandidate and a set of ID tracks
 
      @author Niels van Eldik
   */

  class IMuonCombinedTagTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;


    /**IMuonCombinedTagTool interface: build combined  muons from a muon and a vector of indet candidates */    
    virtual void combine( const MuonCandidate& muonCandidate, const std::vector<const InDetCandidate*>& indetCandidates, InDetCandidateToTagMap& tagMap,
			  TrackCollection* combTracks, TrackCollection* METracks) const = 0;


    virtual void cleanUp() const = 0;
  };

  inline const InterfaceID& IMuonCombinedTagTool::interfaceID()
    { 
      return IID_IMuonCombinedTagTool; 
    }

} // end of namespace

#endif 
