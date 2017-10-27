#include "../LArHECLocalCalculator.h"
#include "../LocalCalibrationCalculator.h"
#include "LArG4HEC/LocalGeometry.h"
#include "../LArHECWheelCalculator.h"
#include "../LArHECCalibrationWheelCalculator.h"
#include "LArG4HEC/HECGeometry.h"

DECLARE_SERVICE_FACTORY(LArHECLocalCalculator)
DECLARE_SERVICE_FACTORY(LArG4::HEC::LocalCalibrationCalculator)
DECLARE_SERVICE_FACTORY(LArG4::HEC::LocalGeometry)
DECLARE_SERVICE_FACTORY(LArHECWheelCalculator)
DECLARE_SERVICE_FACTORY(LArG4::HEC::LArHECCalibrationWheelCalculator)
DECLARE_SERVICE_FACTORY(LArG4::HEC::HECGeometry)

