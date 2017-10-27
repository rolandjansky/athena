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

DECLARE_COMPONENT( LArG4::BarrelCryostat::CalibrationCalculator )
DECLARE_COMPONENT( LArG4::BarrelCryostat::CalibrationMixedCalculator )
DECLARE_COMPONENT( LArBarrelCalculator )
DECLARE_COMPONENT( LArBarrelPresamplerCalculator )
DECLARE_COMPONENT( LArG4::BarrelCryostat::CalibrationLArCalculator )
DECLARE_COMPONENT( LArG4::DM::CalibrationCalculator )
DECLARE_COMPONENT( LArG4::Barrel::CalibrationCalculator )
DECLARE_COMPONENT( LArG4::BarrelPresampler::CalibrationCalculator )
DECLARE_COMPONENT( LArG4::Barrel::Geometry )
DECLARE_COMPONENT( LArG4::BarrelPresampler::Geometry )

