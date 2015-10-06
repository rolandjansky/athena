/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 ////////////////////////////////////
//                                  //
//  Class: ITRT_DriftTimeData       //
//                                  //
//  Author: Thomas Kittelmann       //
//                                  //
//  First Version: January 2006     //
//                                  //
 ////////////////////////////////////

#ifndef ITRT_DRIFTTIMEDATA_H
#define ITRT_DRIFTTIMEDATA_H

class ITRT_DriftTimeData {

public:

  virtual double MaxTabulatedField() const = 0;
  virtual double DriftTimeAtNoField(const double& distance) const = 0;
  virtual double DriftTimeAtMaxField(const double& distance) const = 0;

  virtual ~ITRT_DriftTimeData() {};

};

#endif
