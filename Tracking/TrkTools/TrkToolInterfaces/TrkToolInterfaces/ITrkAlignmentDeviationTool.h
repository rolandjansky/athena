/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRKALIGNMENTDEVIATIONTOOL_H
#define ITRKALIGNMENTDEVIATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h" 
#include <vector>

static const InterfaceID IID_ITrkAlignmentDeviationTool("Trk::ITrkAlignmentDeviationTool", 1, 0);

namespace Trk {

  class Track; 
  class MeasurementBase;
  class AlignmentDeviation;

/** @brief Interface for tool to add alignmenties to a global chi2 fit

    @author Niels.van.Eldik@cern.ch */
  class ITrkAlignmentDeviationTool : virtual public IAlgTool
  {
    public:

      static const InterfaceID& interfaceID( ) ;

      /**
       * Compute alignment deviations, given a track as input
       */
      virtual void makeAlignmentDeviations (const Trk::Track& track, std::vector<Trk::AlignmentDeviation*>& deviations) const = 0;
  };

}

inline const InterfaceID& Trk::ITrkAlignmentDeviationTool::interfaceID()
{
  return IID_ITrkAlignmentDeviationTool;
}

#endif 
