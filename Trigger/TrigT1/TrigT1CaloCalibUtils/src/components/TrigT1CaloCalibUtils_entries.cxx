
#include "TrigT1CaloCalibUtils/L1CaloHVCorrectionsForDB.h"
#include "TrigT1CaloCalibUtils/L1CaloHVDummyContainers.h"
#include "TrigT1CaloCalibUtils/L1CaloLinearCalibration.h"
#include "TrigT1CaloCalibUtils/L1CaloPedestalMaker.h"
#include "TrigT1CaloCalibUtils/L1CaloPedestalGenerator.h"
#include "TrigT1CaloCalibUtils/L1CaloRampMaker.h"
#include "TrigT1CaloCalibUtils/L1CaloTriggerTowerSelector.h"
#include "TrigT1CaloCalibUtils/L1CaloPprPhos4ShapeMaker.h"
#include "TrigT1CaloCalibUtils/L1CaloPprMonitoring.h"

DECLARE_COMPONENT( L1CaloHVCorrectionsForDB )
DECLARE_COMPONENT( L1CaloHVDummyContainers )
DECLARE_COMPONENT( L1CaloLinearCalibration )
DECLARE_COMPONENT( L1CaloPedestalMaker )
DECLARE_COMPONENT( L1CaloPedestalGenerator )
DECLARE_COMPONENT( L1CaloRampMaker )
DECLARE_COMPONENT( L1CaloTriggerTowerSelector )
DECLARE_COMPONENT( L1CaloPprPhos4ShapeMaker )
DECLARE_COMPONENT( L1CaloPprMonitoring )

#include "../L1CaloDumpRampData.h"
DECLARE_COMPONENT( L1CaloDumpRampData )

