#include "GaudiKernel/DeclareFactoryEntries.h"
#include "src/PixelRecoDbTool.h"
#include "src/PixelRecoDbTestWriteRead.h"
#include "src/PixelCalibDbTool.h"
#include "src/PixelCalibDbTestWriteRead.h"
#include "src/PixelDCSTool.h"
#include "src/PixelDCSTestReadWrite.h"
#include "src/PixelDistortionsTool.h"
#include "src/PixelDistortionsTestReadWrite.h"
//using namespace PixelCalib;

DECLARE_TOOL_FACTORY( PixelRecoDbTool )
DECLARE_TOOL_FACTORY( PixelCalibDbTool )
DECLARE_ALGORITHM_FACTORY( PixelCalibDbTestWriteRead )
DECLARE_ALGORITHM_FACTORY( PixelRecoDbTestWriteRead )
DECLARE_TOOL_FACTORY( PixelDCSTool )
DECLARE_ALGORITHM_FACTORY( PixelDCSTestReadWrite )
DECLARE_TOOL_FACTORY( PixelDistortionsTool )
DECLARE_ALGORITHM_FACTORY( PixelDistortionsTestReadWrite )

DECLARE_FACTORY_ENTRIES( PixelConditionsTools ) {
  DECLARE_TOOL( PixelRecoDbTool )
  DECLARE_TOOL( PixelCalibDbTool )
  DECLARE_ALGORITHM( PixelCalibDbTestWriteRead ) 
  DECLARE_ALGORITHM( PixelRecoDbTestWriteRead ) 
  DECLARE_TOOL( PixelDCSTool )
  DECLARE_ALGORITHM( PixelDCSTestReadWrite ) 
  DECLARE_TOOL( PixelDistortionsTool )
  DECLARE_ALGORITHM( PixelDistortionsTestReadWrite ) 
}
       
