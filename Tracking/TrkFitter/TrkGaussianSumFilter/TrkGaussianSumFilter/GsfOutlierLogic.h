/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* *******************************************************************************
			GsfOutlierLogic.h  -  description
			---------------------------------
begin                : Tuesday 15th March 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Outlier logic for Gaussian Sum Filter
********************************************************************************** */

#ifndef TrkGsfOutlierLogic_H
#define TrkGsfOutlierLogic_H

#include "TrkGaussianSumFilter/IGsfOutlierLogic.h"

#include "TrkFitterUtils/FitterTypes.h"

#include "AthenaBaseComps/AthAlgTool.h"

namespace Trk {

class FitQuality;

class GsfOutlierLogic : public AthAlgTool, virtual public IGsfOutlierLogic {

 public:

  /** Constructor with AlgTool parameters */
  GsfOutlierLogic( const std::string&, const std::string&, const IInterface* );

  /** Virtual destructor */
  virtual ~GsfOutlierLogic() {};

  /** AlgTool initialise method */
  StatusCode initialize();

  /** AlgTool finalize method */
  StatusCode finalize();

  virtual const FitQuality* fitQuality( const SmoothedTrajectory& ) const;
 
 private:
  int                                m_outputlevel;                      //!< to cache current output level

};

} // end Trk namespace

#endif
