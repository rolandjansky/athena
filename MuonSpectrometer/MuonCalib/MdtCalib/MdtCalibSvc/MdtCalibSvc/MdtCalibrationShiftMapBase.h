/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTCALIBSVC_MDTCALIBRATIONSHIFTMAPBASE_H
#define MDTCALIBSVC_MDTCALIBRATIONSHIFTMAPBASE_H

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

#include "MuonMDT_Cabling/MuonMDT_CablingSvc.h"

class Identifier;
class TTree;

/**
   @class MdtCalibrationShiftMapBase
   Provides the base class for the per-tube shifting tools, like
   MdtCalibT0ShiftTool & MdtCalibTMaxShiftTool.
   @author Andreas Hoenle
*/
class MdtCalibrationShiftMapBase : virtual public IInterface, public AthService {
public:
  /* constructor */
  MdtCalibrationShiftMapBase(const std::string &name, ISvcLocator *sl);

  /* destructor */
  virtual ~MdtCalibrationShiftMapBase() = 0;

  /* implements IInterface */
  static const InterfaceID &interfaceID() {
    static InterfaceID s_iID("MdtCalibrationShiftMapBase", 1, 0);
    return s_iID;
  }

  /* implements IInterface */
  virtual StatusCode queryInterface(const InterfaceID &riid, void **ppvIF);

  /* initialization */
  virtual StatusCode initialize();

  /* finalization */
  virtual StatusCode finalize();

  /*
   * initalization of map cannot happen before first event
   * special function required
   */
  virtual StatusCode initializeMap() = 0;

  /* dump the map in binary file, given a path */
  StatusCode dumpMapAsFile();

  /* load the map from a binary file, given a path */
  StatusCode loadMapFromFile();

  /* get shift */
  float getValue(const Identifier& id);

protected:
  /// please don't add any data members here!!
  /// they should be added to Imp to keep the class free from data exposed to clients
  ServiceHandle<MuonMDT_CablingSvc> m_cablingSvc;
  std::map<Identifier, float> m_shiftValues;
  bool m_mapIsInitialized;

  /* Make map configurable in job options */
  std::string m_mapFileName;
  float m_centralValue;
  float m_sigma;
  bool m_forceMapRecreate;


private:
};

#endif
