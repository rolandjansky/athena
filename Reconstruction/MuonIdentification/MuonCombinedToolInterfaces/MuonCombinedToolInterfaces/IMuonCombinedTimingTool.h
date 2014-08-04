/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuonCombinedTimingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IRECMUONCOMBINEDTIMINGTOOL_H
#define IRECMUONCOMBINEDTIMINGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODMuon/Muon.h"
#include <vector>

namespace Trk{ class SpaceTimePoint; }

namespace Rec 
{


  static const InterfaceID IID_IMuonCombinedTimingTool("Rec::IMuonCombinedTimingTool", 1, 0);

  /** @class IMuonCombinedTimingTool
      @brief interface providing a vector of time-space points from the various detector
             measurements crossed by a muon.

      @author Wolfgang Liebig
   */

  class IMuonCombinedTimingTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    /** @brief the main method providing the vector of space-time points
      */
    virtual std::vector<Trk::SpaceTimePoint*> timeMeasurements(const xAOD::Muon& muon) const = 0;

  };

  inline const InterfaceID& Rec::IMuonCombinedTimingTool::interfaceID()
    { 
      return IID_IMuonCombinedTimingTool; 
    }

} // end of namespace

#endif 
