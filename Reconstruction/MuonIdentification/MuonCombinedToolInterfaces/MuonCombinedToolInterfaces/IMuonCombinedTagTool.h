/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuonCombinedTagTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IRECMUONCOMBINEDTAGTOOL_H
#define IRECMUONCOMBINEDTAGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>


namespace MuonCombined {
  class InDetCandidate;
  class MuonCandidate;

  static const InterfaceID IID_IMuonCombinedTagTool("MuonCombined::IMuonCombinedTagTool", 1, 0);

  /** @class IMuonCombinedTagTool
      @brief interface for tools building combined muons from a MuonCandidate and a set of ID tracks
 
      @author Niels van Eldik
   */

  class IMuonCombinedTagTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;


    /**IMuonCombinedTagTool interface: build combined  muons from a muon and a vector of indet candidates */    
    virtual void combine( const MuonCandidate& muonCandidate, const std::vector<InDetCandidate*>& indetCandidates ) = 0;

  };

  inline const InterfaceID& IMuonCombinedTagTool::interfaceID()
    { 
      return IID_IMuonCombinedTagTool; 
    }

} // end of namespace

#endif 
