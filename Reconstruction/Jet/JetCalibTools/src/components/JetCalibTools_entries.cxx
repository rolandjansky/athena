#include "GaudiKernel/DeclareFactoryEntries.h"

#include "JetCalibTools/JetCalibrationTool.h"

#include "JetCalibTools/CalibrationMethods/JetPileupCorrection.h"
#include "JetCalibTools/CalibrationMethods/ResidualOffsetCorrection.h"
#include "JetCalibTools/CalibrationMethods/BcidOffsetCorrection.h"
#include "JetCalibTools/CalibrationMethods/EtaJESCorrection.h"
#include "JetCalibTools/CalibrationMethods/InsituDataCorrection.h"

DECLARE_TOOL_FACTORY( JetCalibrationTool )
DECLARE_TOOL_FACTORY( JetPileupCorrection )
DECLARE_TOOL_FACTORY( ResidualOffsetCorrection )
DECLARE_TOOL_FACTORY( BcidOffsetCorrection )
DECLARE_TOOL_FACTORY( EtaJESCorrection )
DECLARE_TOOL_FACTORY( InsituDataCorrection )

DECLARE_FACTORY_ENTRIES( JetCalibTools )
{
  DECLARE_TOOL( JetCalibrationTool );
  DECLARE_TOOL( JetPileupCorrection );
  DECLARE_TOOL( ResidualOffsetCorrection );
  DECLARE_TOOL( BcidOffsetCorrection );
  DECLARE_TOOL( EtaJESCorrection );
  DECLARE_TOOL( InsituDataCorrection );
}
