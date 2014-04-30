/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ITRKALIGNMENTDEVIATIONTOOL_H
#define ITRKALIGNMENTDEVIATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h" 
#include <vector>

static const InterfaceID IID_ITrkAlignmentDeviationTool("Trk::ITrkAlignmentDeviationTool", 1, 0);

//class HepMatrix;

namespace Trk {

  class Track; 
  class MeasurementBase;

/** @brief Interface for tool to add alignmenties to a global chi2 fit

    @author Niels.van.Eldik@cern.ch */
  class ITrkAlignmentDeviationTool : virtual public IAlgTool
  {
    public:

      static const InterfaceID& interfaceID( ) ;

      /** calculate the number of degrees of freedom, covariance matrix and the required Jacobians needed to add the alignment uncertainties 
	  in a global chi2 fit and  based on a given trajectory */
      virtual bool initTrajectory( const Trk::Track& track, int& dim, HepMatrix& cov, std::vector<const HepMatrix*>& jacobians ) const = 0;

      /** update the list of rots based on a set of fitted alignment deviations */
      virtual bool updateROTs( std::vector< const MeasurementBase* >& mbs, std::vector<double>& alignmentDeviations ) const = 0;
  };

}

inline const InterfaceID& Trk::ITrkAlignmentDeviationTool::interfaceID()
{
  return IID_ITrkAlignmentDeviationTool;
}

#endif 
