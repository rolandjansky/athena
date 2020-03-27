/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef INSWCalibTool_h
#define INSWCalibTool_h

#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include <cmath>
#include <vector>

static const InterfaceID IID_INSWCalibTool("Muon::INSWCalibTool",1,0);

namespace Muon {

  class MM_RawData;

  class INSWCalibTool : virtual public IAlgTool {
    
  public:  // static methods

    static const InterfaceID& interfaceID()  {return IID_INSWCalibTool;}

  public:  // interface methods

    virtual StatusCode calibrate(const Muon::MM_RawData* mmRawData, const Amg::Vector3D& globalPos, double& dist_drift, double& distRes_drift, double& calib_charge) = 0;

  };
  
}


#endif
