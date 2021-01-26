/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTCALIBSVC_MDTCALIBRATIONT0SHIFTTOOL_H
#define MDTCALIBSVC_MDTCALIBRATIONT0SHIFTTOOL_H

#include "MdtCalibSvc/MdtCalibrationShiftMapBase.h"

/**
   @class MdtCalibrationT0ShiftTool
   Provides a per-tube smearing of the T0 value.
   @author Andreas Hoenle
*/
class MdtCalibrationT0ShiftTool : virtual public MdtCalibrationShiftMapBase {
public:
  /** constructor */
  MdtCalibrationT0ShiftTool(const std::string& type, const std::string& name, const IInterface* parent);

  /** destructor */
  ~MdtCalibrationT0ShiftTool()=default;

  /*
   * initalization of map cannot happen before first event
   * special function required
   */
  StatusCode initializeMap() override;

private:
};

#endif
