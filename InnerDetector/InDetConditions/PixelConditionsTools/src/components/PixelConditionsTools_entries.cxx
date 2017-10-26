#include "src/PixelRecoDbTool.h"
#include "src/PixelRecoDbTestWriteRead.h"
#include "src/PixelCalibDbTool.h"
#include "src/PixelCalibDbTestWriteRead.h"
#include "src/PixelDCSTool.h"
#include "src/PixelDCSTestReadWrite.h"
#include "src/PixelDistortionsTool.h"
#include "src/PixelDistortionsTestReadWrite.h"

DECLARE_TOOL_FACTORY( PixelRecoDbTool )
DECLARE_TOOL_FACTORY( PixelCalibDbTool )
DECLARE_ALGORITHM_FACTORY( PixelCalibDbTestWriteRead )
DECLARE_ALGORITHM_FACTORY( PixelRecoDbTestWriteRead )
DECLARE_TOOL_FACTORY( PixelDCSTool )
DECLARE_ALGORITHM_FACTORY( PixelDCSTestReadWrite )
DECLARE_TOOL_FACTORY( PixelDistortionsTool )
DECLARE_ALGORITHM_FACTORY( PixelDistortionsTestReadWrite )

       

