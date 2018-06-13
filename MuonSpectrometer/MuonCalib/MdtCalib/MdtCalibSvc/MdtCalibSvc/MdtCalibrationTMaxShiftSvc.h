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
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MdtCalibSvc/MdtCalibrationShiftMapBase.h"

class Identifier;

/**
   @class MdtCalibrationTMaxShiftSvc
   Provides a per-tube shifting of the TMax value.
   @author Andreas Hoenle
*/
class MdtCalibrationTMaxShiftSvc : public MdtCalibrationShiftMapBase {
public:
  /** constructor */
  MdtCalibrationTMaxShiftSvc(const std::string &name, ISvcLocator *sl);

  /** destructor */
  virtual ~MdtCalibrationTMaxShiftSvc();

  /** implements IInterface */
  static const InterfaceID &interfaceID() {
    static InterfaceID s_iID("MdtCalibrationTMaxShiftSvc", 1, 0);
    return s_iID;
  }

  /* implements IInterface */
  virtual StatusCode queryInterface(const InterfaceID &riid, void **ppvIF);

  /*
   * initalization of map cannot happen before first event
   * special function required
   */
  StatusCode initializeMap();

  StatusCode setTUpper(float tUpper);
  float getTUpper() { return m_tUpper; }

private:
  float m_tUpper;
};

#endif
