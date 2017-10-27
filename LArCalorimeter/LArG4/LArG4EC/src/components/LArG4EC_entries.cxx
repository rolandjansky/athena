#include "../CalibrationCalculator.h"
#include "../CryostatCalibrationLArCalculator.h"
#include "../EMECSupportCalibrationCalculator.h"
#include "../LArEndcapPresamplerCalculator.h"
#include "../CryostatCalibrationCalculator.h"
#include "../CryostatCalibrationMixedCalculator.h"
#include "../EnergyCalculator.h"
#include "../PresamplerCalibrationCalculator.h"

DECLARE_COMPONENT( LArG4::EC::CalibrationCalculator )
DECLARE_COMPONENT( LArG4::EMECSupportCalibrationCalculator )
DECLARE_COMPONENT( LArEndcapPresamplerCalculator )
DECLARE_COMPONENT( LArG4::EndcapCryostat::CalibrationLArCalculator )
DECLARE_COMPONENT( LArG4::EndcapCryostat::CalibrationCalculator )
DECLARE_COMPONENT( LArG4::EndcapCryostat::CalibrationMixedCalculator )
DECLARE_COMPONENT( LArG4::EC::EnergyCalculator )
DECLARE_COMPONENT( LArG4::EC::PresamplerCalibrationCalculator )

