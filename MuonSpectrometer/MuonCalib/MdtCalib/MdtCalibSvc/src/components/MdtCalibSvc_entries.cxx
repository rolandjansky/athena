#include "GaudiKernel/DeclareFactoryEntries.h"

#include "MdtCalibSvc/MdtCalibrationSvc.h"
#include "MdtCalibSvc/MdtCalibrationRegionSvc.h"
#include "MdtCalibSvc/MdtCalibrationDbSvc.h"

DECLARE_SERVICE_FACTORY ( MdtCalibrationSvc )
DECLARE_SERVICE_FACTORY ( MdtCalibrationRegionSvc )
DECLARE_SERVICE_FACTORY ( MdtCalibrationDbSvc )

DECLARE_FACTORY_ENTRIES(MdtCalibSvc) {
  DECLARE_SERVICE(MdtCalibrationSvc)
  DECLARE_SERVICE(MdtCalibrationRegionSvc)
  DECLARE_SERVICE(MdtCalibrationDbSvc)
}
