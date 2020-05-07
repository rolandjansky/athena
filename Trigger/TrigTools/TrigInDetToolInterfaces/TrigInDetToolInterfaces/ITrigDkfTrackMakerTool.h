/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIGDKFTRACKMAKERTOOL_H__
#define __ITRIGDKFTRACKMAKERTOOL_H__

#include "GaudiKernel/IAlgTool.h"
#include <vector>

namespace Trk {

    class TrkBaseNode;
    class TrkPlanarSurface;
		class Track;
}

static const InterfaceID IID_ITrigDkfTrackMakerTool("ITrigDkfTrackMakerTool",1,0);


  /** @class ITrigDkfTrackMakerTool

      provides the abstract interface for the helper tool used by TrigInDetTrackFitter
      to convert TrigSiSpacePoint vector into vector of Trk::TrkBaseNode
      objects and vector of associated Trk::TrkPlanarSurface objects.  

      @author D.Emeliyanov <http://consult.cern.ch/xwho>
  */

  class ITrigDkfTrackMakerTool : virtual public IAlgTool { 

  public:
    /** other standard AlgTool methods */

    static const InterfaceID& interfaceID ()   //!< the Tool's interface
      {  return IID_ITrigDkfTrackMakerTool; }  	

    virtual bool createDkfTrack(const Trk::Track&, std::vector<Trk::TrkBaseNode*>&, double) const = 0;
  };

#endif
