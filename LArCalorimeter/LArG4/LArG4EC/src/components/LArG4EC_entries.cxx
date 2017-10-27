#include "../CalibrationCalculator.h"
#include "../CryostatCalibrationLArCalculator.h"
#include "../EMECSupportCalibrationCalculator.h"
#include "../LArEndcapPresamplerCalculator.h"
#include "../CryostatCalibrationCalculator.h"
#include "../CryostatCalibrationMixedCalculator.h"
#include "../EnergyCalculator.h"
#include "../PresamplerCalibrationCalculator.h"

DECLARE_SERVICE_FACTORY(LArG4::EC::CalibrationCalculator)
DECLARE_SERVICE_FACTORY(LArG4::EMECSupportCalibrationCalculator)
DECLARE_SERVICE_FACTORY(LArEndcapPresamplerCalculator)
DECLARE_SERVICE_FACTORY(LArG4::EndcapCryostat::CalibrationLArCalculator)
DECLARE_SERVICE_FACTORY(LArG4::EndcapCryostat::CalibrationCalculator)
DECLARE_SERVICE_FACTORY(LArG4::EndcapCryostat::CalibrationMixedCalculator)
DECLARE_SERVICE_FACTORY(LArG4::EC::EnergyCalculator)
DECLARE_SERVICE_FACTORY(LArG4::EC::PresamplerCalibrationCalculator)

