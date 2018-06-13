#include "GaudiKernel/DeclareFactoryEntries.h"

#include "MdtCalibSvc/MdtCalibrationSvc.h"
#include "MdtCalibSvc/MdtCalibrationRegionSvc.h"
#include "MdtCalibSvc/MdtCalibrationDbSvc.h"
#include "MdtCalibSvc/MdtCalibrationT0ShiftSvc.h"
#include "MdtCalibSvc/MdtCalibrationTMaxShiftSvc.h"

DECLARE_SERVICE_FACTORY ( MdtCalibrationSvc )
DECLARE_SERVICE_FACTORY ( MdtCalibrationRegionSvc )
DECLARE_SERVICE_FACTORY ( MdtCalibrationDbSvc )
DECLARE_SERVICE_FACTORY ( MdtCalibrationT0ShiftSvc )
DECLARE_SERVICE_FACTORY ( MdtCalibrationTMaxShiftSvc )

DECLARE_FACTORY_ENTRIES(MdtCalibSvc) {
  DECLARE_SERVICE(MdtCalibrationSvc)
  DECLARE_SERVICE(MdtCalibrationRegionSvc)
  DECLARE_SERVICE(MdtCalibrationDbSvc)
  DECLARE_SERVICE(MdtCalibrationT0ShiftSvc)
  DECLARE_SERVICE(MdtCalibrationTMaxShiftSvc)
}
