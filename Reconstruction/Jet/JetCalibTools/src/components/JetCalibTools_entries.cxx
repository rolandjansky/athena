#include "JetCalibTools/JetCalibrationTool.h"

#include "JetCalibTools/CalibrationMethods/JetPileupCorrection.h"
#include "JetCalibTools/CalibrationMethods/ResidualOffsetCorrection.h"
#include "JetCalibTools/CalibrationMethods/BcidOffsetCorrection.h"
#include "JetCalibTools/CalibrationMethods/EtaJESCorrection.h"
#include "JetCalibTools/CalibrationMethods/InsituDataCorrection.h"

DECLARE_COMPONENT( JetCalibrationTool )
DECLARE_COMPONENT( JetPileupCorrection )
DECLARE_COMPONENT( ResidualOffsetCorrection )
DECLARE_COMPONENT( BcidOffsetCorrection )
DECLARE_COMPONENT( EtaJESCorrection )
DECLARE_COMPONENT( InsituDataCorrection )

