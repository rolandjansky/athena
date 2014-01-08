/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MB_Track_CaloEntr.h"

MB_Track_CaloEntr::MB_Track_CaloEntr():MB_Track_MboyCyl(){}
MB_Track_CaloEntr::~MB_Track_CaloEntr(){}

void MB_Track_CaloEntr::SetTrackCalo(
      double ThetaOfCrossingPoint,
       double PhiOfCrossingPoint,
        double ThetaOfUnitVector,
	 double PhiOfUnitVector,
	  double InverseMomentum,
      double Cov11, double Cov21, double Cov22,
      double Cov31, double Cov32, double Cov33,
      double Cov41, double Cov42, double Cov43,
      double Cov44, double Cov51, double Cov52,
      double Cov53, double Cov54, double Cov55,
      double XCrossingPoint ,double YCrossingPoint ,double ZCrossingPoint,
      double UXUnitVector ,double UYUnitVector ,double UZUnitVector,
      double Momentum,
      double ZHalfLengthOfCylinder,double RadiusOfCylinder)
{

  SetMboyCyl(
      ThetaOfCrossingPoint,
       PhiOfCrossingPoint,
        ThetaOfUnitVector,
	 PhiOfUnitVector,
	  InverseMomentum,
      Cov11, Cov21, Cov22,
      Cov31, Cov32, Cov33,
      Cov41, Cov42, Cov43,
      Cov44, Cov51, Cov52,
      Cov53, Cov54, Cov55,
      XCrossingPoint ,YCrossingPoint ,ZCrossingPoint,
      UXUnitVector ,UYUnitVector ,UZUnitVector,
      Momentum,
      ZHalfLengthOfCylinder,RadiusOfCylinder)
      ;

}
