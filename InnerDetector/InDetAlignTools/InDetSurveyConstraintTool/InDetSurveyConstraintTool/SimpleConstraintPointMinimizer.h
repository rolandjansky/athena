/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ------------------------------------------------------------------------------
//  Description: class SimpleConstraintPointMinimizer.  Simple implementation
//  of the constraint point minimizer
//
//  Authors: Tobias Golling, 10/27/2005
// ------------------------------------------------------------------------------
#ifndef SIMPLECONSTRAINTPOINTMINIMIZER_HH
#define SIMPLECONSTRAINTPOINTMINIMIZER_HH

#include "InDetSurveyConstraintTool/SurveyConstraintPoint.h"
#include <vector>

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

class SimpleConstraintPointMinimizer {
 public:
  // describe the maximum distance from the origin to include in the
  // minimization
  SimpleConstraintPointMinimizer(double proximity);
  ~SimpleConstraintPointMinimizer();
  // minimize via a trivial algorithm which weights all points and dimensions
  // equally
  double findMinimum(const std::vector<SurveyConstraintPoint>& points,
		     Amg::Vector3D& aRotat, Amg::Vector3D& translate);
  double  secondMinimum(
			const std::vector<SurveyConstraintPoint>& points,
			Amg::Vector3D& aRotat1,  Amg::Vector3D& translate1,
			Amg::Vector3D& aRotat2,  Amg::Vector3D& translate2);
 private:
  double _proximity;
};

#endif
