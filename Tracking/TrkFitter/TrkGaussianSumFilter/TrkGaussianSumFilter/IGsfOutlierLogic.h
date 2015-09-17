/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* *******************************************************************************
			IGsfOutlierLogic.h  -  description
			---------------------------------
created              : Thursday 8th January 2009
author               : amorley
email                : Anthony.Morley@cern.ch
decription           : Abstract interface for outlier logic for Gaussian Sum Filter
********************************************************************************** */

#ifndef TrkIGsfOutlierLogic_H
#define TrkIGsfOutlierLogic_H

#include "TrkFitterUtils/FitterTypes.h"

#include "GaudiKernel/IAlgTool.h"

namespace Trk {

class FitQuality;

static const InterfaceID InterfaceID_GsfOutlierLogic ("GsfOutlierLogic", 1, 0);

class IGsfOutlierLogic : virtual public IAlgTool {

 public:

  /** Virtual destructor */
  virtual ~IGsfOutlierLogic() {};

  /** Interface ID method */
  static const InterfaceID& interfaceID() { return InterfaceID_GsfOutlierLogic; };

  virtual const FitQuality* fitQuality( const SmoothedTrajectory& ) const = 0;
 
};

} // end Trk namespace

#endif
