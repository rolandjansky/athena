#include "../CryostatCalibrationCalculator.h"
#include "../CryostatCalibrationMixedCalculator.h"
#include "../LArBarrelCalculator.h"
#include "../LArBarrelPresamplerCalculator.h"
#include "../CryostatCalibrationLArCalculator.h"
#include "../DMCalibrationCalculator.h"
#include "../LArBarrelCalibrationCalculator.h"
#include "../PresamplerCalibrationCalculator.h"
#include "../LArBarrelGeometry.h"
#include "../LArBarrelPresamplerGeometry.h"

DECLARE_SERVICE_FACTORY(LArG4::BarrelCryostat::CalibrationCalculator)
DECLARE_SERVICE_FACTORY(LArG4::BarrelCryostat::CalibrationMixedCalculator)
DECLARE_SERVICE_FACTORY(LArBarrelCalculator)
DECLARE_SERVICE_FACTORY(LArBarrelPresamplerCalculator)
DECLARE_SERVICE_FACTORY(LArG4::BarrelCryostat::CalibrationLArCalculator)
DECLARE_SERVICE_FACTORY(LArG4::DM::CalibrationCalculator)
DECLARE_SERVICE_FACTORY(LArG4::Barrel::CalibrationCalculator)
DECLARE_SERVICE_FACTORY(LArG4::BarrelPresampler::CalibrationCalculator)
DECLARE_SERVICE_FACTORY(LArG4::Barrel::Geometry)
DECLARE_SERVICE_FACTORY(LArG4::BarrelPresampler::Geometry)

