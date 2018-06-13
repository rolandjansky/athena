/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTCALIBSVC_MDTCALIBRATIONT0SHIFTSVC_H
#define MDTCALIBSVC_MDTCALIBRATIONT0SHIFTSVC_H

/*
 *
 * Author Andreas Hoenle (der.andi@cern.ch)
 *
 */

#include <map>

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MdtCalibSvc/MdtCalibrationShiftMapBase.h"

class Identifier;
class TTree;

/**
   @class MdtCalibrationT0ShiftSvc
   Provides a per-tube smearing of the T0 value.
   @author Andreas Hoenle
*/
class MdtCalibrationT0ShiftSvc : public MdtCalibrationShiftMapBase {
public:
  /** constructor */
  MdtCalibrationT0ShiftSvc(const std::string &name, ISvcLocator *sl);

  /** destructor */
  ~MdtCalibrationT0ShiftSvc();

  /** implements IInterface */
  static const InterfaceID &interfaceID() {
    static InterfaceID s_iID("MdtCalibrationT0ShiftSvc", 1, 0);
    return s_iID;
  }

  /* implements IInterface */
  virtual StatusCode queryInterface(const InterfaceID &riid, void **ppvIF);

  /*
   * initalization of map cannot happen before first event
   * special function required
   */
  StatusCode initializeMap();

private:
};

#endif
