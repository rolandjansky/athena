/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_DRIFTFUNCTIONTOOL_H
#define ITRT_DRIFTFUNCTIONTOOL_H

// Include files
#include "GaudiKernel/IAlgTool.h"

class Identifier;

/** @class ITRT_DriftFunctionTool
    Interface to AlgTool TRT_DriftFunctionTool  */

class ITRT_DriftFunctionTool : virtual public IAlgTool {

 public:

  /** Virtual destructor */
  virtual ~ITRT_DriftFunctionTool(){}

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ITRT_DriftFunctionTool, 1, 0);

  virtual bool isValidTime(double drifttime) const = 0;

  virtual bool isTestBeamData() const = 0;

  virtual double rawTime(int tdcvalue) const = 0;

  virtual double driftRadius(double rawtime, Identifier id, double& t0, bool& isOK, unsigned int word=0) const = 0;

  virtual double driftRadius(double drifttime) const = 0;

  virtual double approxDriftTime(double driftradius) const = 0;

  virtual double errorOfDriftRadius(double drifttime, Identifier id, float mu = -10, unsigned int word=0) const = 0;

  virtual double driftTimeToTCorrection(double tot, Identifier id, bool isArgonStraw=false) const = 0;

  virtual double driftTimeHTCorrection(Identifier id, bool isArgonStraw=false) const = 0;


};

#endif // ITRT_DRIFTFUNCTIONTOOL_H
