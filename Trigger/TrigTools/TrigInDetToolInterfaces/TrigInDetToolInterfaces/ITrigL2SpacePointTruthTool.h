/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIGL2SPTRUTHTOOL_H__
#define __ITRIGL2SPTRUTHTOOL_H__

#include "GaudiKernel/IAlgTool.h"
#include <vector>

class TrigSiSpacePoint;

static const InterfaceID IID_ITrigL2SpacePointTruthTool("ITrigL2SpacePointTruthTool",1,0);


  /** @class ITrigL2SpacePointTruthTool

      provides the abstract interface for the helper tool used by TrigL2SpacePointTruthTool
      which associates a TrigSiSpacePoint with MC truth particle barcode(s)

      @author D.Emeliyanov <http://consult.cern.ch/xwho>
  */

  class ITrigL2SpacePointTruthTool : virtual public IAlgTool { 

  public:
    /** other standard AlgTool methods */

    static const InterfaceID& interfaceID ()   //!< the Tool's interface
      {  return IID_ITrigL2SpacePointTruthTool; }  	

    virtual void getBarCodes(std::vector<TrigSiSpacePoint*>&, std::vector<int>&) = 0;
    virtual void getBarCodes(std::vector<TrigSiSpacePointBase>&, std::vector<int>&) = 0;
  };

#endif
