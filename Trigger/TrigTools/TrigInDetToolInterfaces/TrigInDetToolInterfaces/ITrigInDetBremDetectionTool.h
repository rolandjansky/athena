/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIGINDETBREMDETECTIONTOOL_H__
#define __ITRIGINDETBREMDETECTIONTOOL_H__

#include "GaudiKernel/IAlgTool.h"

namespace Trk {

    class TrkBaseNode;
    class TrkTrackState; 
    class TrkPlanarSurface;  
}

static const InterfaceID IID_ITrigInDetBremDetectionTool("ITrigInDetBremDetectionTool",1,0);


  /** @class ITrigInDetBremDetectionTool

      provides the abstract interface for the input estimation algorithm used to apply 
      bremsstrahlung corrections. 

      @author D.Emeliyanov <http://consult.cern.ch/xwho>
  */

  class ITrigInDetBremDetectionTool : virtual public IAlgTool { 

  public:
    /** other standard AlgTool methods */
    //    virtual ~ITrigInDetBremDetectionTool() {};                //!< virtual destructor

    static const InterfaceID& interfaceID ()   //!< the Tool's interface
      {  return IID_ITrigInDetBremDetectionTool; }  	

    virtual void reset() = 0;
    virtual bool addNewPoint(Trk::TrkTrackState*,Trk::TrkBaseNode*, Trk::TrkPlanarSurface*, double a[5][5], double) = 0;
    virtual bool solve(int) = 0;
    virtual void modifySurfaces(int) = 0;
    virtual void report(int) = 0;
  };

#endif
