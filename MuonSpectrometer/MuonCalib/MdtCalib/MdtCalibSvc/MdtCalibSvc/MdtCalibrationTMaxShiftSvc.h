/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTCALIBSVC_MDTCALIBRATIONTMAXSHIFTSVC_H
#define MDTCALIBSVC_MDTCALIBRATIONTMAXSHIFTSVC_H

/*
 *
 * Author Andreas Hoenle (der.andi@cern.ch)
 *
 */

#include <map>

#include "AthenaBaseComps/AthService.h"

#include "MdtCalibSvc/MdtCalibrationShiftMapBase.h"

class Identifier;

/*
   @class MdtCalibrationTMaxShiftSvc
   Provides a per-tube shifting of the TMax value.
   @author Andreas Hoenle
*/
class MdtCalibrationTMaxShiftSvc : virtual public MdtCalibrationShiftMapBase {
public:
  /* constructor */
  MdtCalibrationTMaxShiftSvc(const std::string &name, ISvcLocator* pSvcLocator);

  /* destructor */
  ~MdtCalibrationTMaxShiftSvc();

  /*
   * initalization of map cannot happen before first event
   * special function required
   */
  StatusCode initializeMap() override;

  StatusCode setTUpper(float tUpper);
  float getTUpper() { return m_tUpper; }

private:
  float m_tUpper;
};

#endif
