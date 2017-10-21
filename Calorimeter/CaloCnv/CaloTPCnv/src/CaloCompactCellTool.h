//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTPCNV_CALOCOMPACTCELLTOOL_H
#define CALOTPCNV_CALOCOMPACTCELLTOOL_H

/**
    @class CaloCompactCellTool
    @brief helper to convert from CaloCellContainer to CaloCompactCellContainer and back
    @author Sven Menke
*/

#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCompactCellContainer.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <math.h>


class IChronoStatSvc;

class CaloCompactCellTool
{
 public:
  enum KNOWN_VERSIONS {
    VERSION_400 = 400,
    VERSION_500 = 500,
    VERSION_501 = 501,
    VERSION_502 = 502,
    VERSION_503 = 503,
    VERSION_504 = 504,

    // Select the latest version.
    VERSION_LATEST = -1
  };

  CaloCompactCellTool();

  StatusCode getTransient(const CaloCompactCellContainer & theCompactContainer,
			  CaloCellContainer * theCellContainer);
  // fills a CaloCellContainer ; caller has the responsibility
  // of creating and deleting the object again

  StatusCode getPersistent(const CaloCellContainer & theCellContainer,
			   CaloCompactCellContainer * theCompactContainer,
			   int theVersion = VERSION_LATEST);
  // fills a CaloCompactCellContainer ; caller has the responsibility
  // of creating and deleting the object again

 private:

  int getLogCompact(const double & x, const double & log_x0, const double & log_x1, const int & n);

  double unpackLog(const int & log_x, const double & log_x0, const double & log_x1, const int & n);

  int getCubRootCompact(const double & x, const double & cbrt_x1, const int & n);

  double unpackCubRoot(const int & cbrt_x, const double & cbrt_x1, const int & n);

};

inline int CaloCompactCellTool::getLogCompact(const double & x,
				       const double & log_x0,
				       const double & log_x1,
				       const int & n)
{
  int result = 0;
  if ( x > 0 ) {
    double log_x = log(x);
    if ( log_x >= log_x1 ) {
      result = (1<<n)-1;
    }
    else if ( log_x > log_x0 ) {
      int nmax=1<<n;
      result = (int)((log_x - log_x0)/(log_x1 - log_x0)*nmax);
    }
  }
  return result;
}

inline int CaloCompactCellTool::getCubRootCompact(const double & x,
					   const double & cbrt_x1,
					   const int & n){
  int result = 0;
  double cbrt_x=cbrt(x);
  if ( cbrt_x >= cbrt_x1 ) {
      result = (1<<n)-1;
      } else {
      int nmax=1<<n;
      result = (int)(cbrt_x/cbrt_x1*nmax);
      }
  return result;
}

inline double CaloCompactCellTool::unpackLog(const int & log_x,
				      const double & log_x0,
				      const double & log_x1,
				      const int & n){
  double result = 0;
  if ( log_x > 0 ) {
    int nmax=1<<n;
    result = exp((log_x+0.5)*(log_x1-log_x0)/nmax+log_x0);
    }
  return result;
}

inline double CaloCompactCellTool::unpackCubRoot(const int & cbrt_x,
					  const double & cbrt_x1,
					  const int & n){
  double result = 0;
  if ( cbrt_x > 0 ) {
    int nmax=1<<n;
    double r = (cbrt_x+0.5)*cbrt_x1/nmax;
    result =r*r*r;
    }
  return result;
}


#endif

