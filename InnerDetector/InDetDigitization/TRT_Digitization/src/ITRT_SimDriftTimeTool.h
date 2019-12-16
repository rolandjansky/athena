/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 ////////////////////////////////////
//                                  //
//  Class: ITRT_SimDriftTimeTool    //
//                                  //
//  Author: Thomas Kittelmann       //
//                                  //
//  First Version: January 2006     //
//                                  //
 ////////////////////////////////////

#ifndef ITRT_SIMDRIFTTIMETOOL_H
#define ITRT_SIMDRIFTTIMETOOL_H

#include "GaudiKernel/IAlgTool.h"

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_ITRT_SimDriftTimeTool("ITRT_SimDriftTimeTool", 1 , 0);

//
// @class ITRT_SimDriftTimeTool ITRT_SimDriftTimeTool.h
//  Give an AlgTool interface to Drift Times for digitization
//
//  @author Thomas Kittelmann <kittel@nbi.dk>
//

class ITRT_SimDriftTimeTool : virtual public IAlgTool {

public:

  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITRT_SimDriftTimeTool; }

  //The drifttime depends on the external (solenoidal) field (and is
  //possibly different for barrel/endcap).

  //This is the average drift time for a certain distance
  virtual double getAverageDriftTime( const double& dist,
                                      const double& effectivefield_squared,
                                      int strawGasType = 0) const = 0;

  // NB: The passed field value should be the magnitude of the field
  // component that is orthogonal to the local straw E-field.
  // It is assumed that the field in the barrel is parallel to the
  // wires and that the field in the endcap is orthogonal to them.
  // Magnetic field mapping provides details.
};

#endif
