#include "GaudiKernel/DeclareFactoryEntries.h"

#include "StransverseMassUtils/mT2_332_CalcTool.h"
#include "StransverseMassUtils/mT2Rec.h"
#include "StransverseMassUtils/mTGen_332_CalcTool.h"
#include "StransverseMassUtils/mCTCalcTool.h"
#include "StransverseMassUtils/mCTperpCalcTool.h"
#include "StransverseMassUtils/mT2_330_CalcTool.h"
#include "StransverseMassUtils/mTGen_330_CalcTool.h"
#include "StransverseMassUtils/mCTparCalcTool.h"
#include "StransverseMassUtils/Asymmetric_mT2_332_CalcTool.h"
#include "StransverseMassUtils/Nt2_332_CalcTool.h"
#include "StransverseMassUtils/M2C_332_CalcTool.h"


DECLARE_ALGORITHM_FACTORY( mT2Rec )

DECLARE_TOOL_FACTORY( mT2_332_CalcTool )
DECLARE_TOOL_FACTORY( mTGen_332_CalcTool )
DECLARE_TOOL_FACTORY( mCTCalcTool )
DECLARE_TOOL_FACTORY( mCTperpCalcTool )
DECLARE_TOOL_FACTORY( mT2_330_CalcTool )
DECLARE_TOOL_FACTORY( mTGen_330_CalcTool )
DECLARE_TOOL_FACTORY( mCTparCalcTool )
DECLARE_TOOL_FACTORY( Asymmetric_mT2_332_CalcTool )
DECLARE_TOOL_FACTORY( Nt2_332_CalcTool )
DECLARE_TOOL_FACTORY( M2C_332_CalcTool )

DECLARE_FACTORY_ENTRIES( StransverseMassUtils ){

  DECLARE_ALGORITHM( mT2Rec )

  DECLARE_TOOL( mT2_332_CalcTool )
  DECLARE_TOOL( mTGen_332_CalcTool )
  DECLARE_TOOL( mCTCalcTool )
  DECLARE_TOOL( mCTperpCalcTool )
  DECLARE_TOOL( mT2_330_CalcTool )
  DECLARE_TOOL( mTGen_330_CalcTool )
  DECLARE_TOOL( mCTparCalcTool )
  DECLARE_TOOL( Asymmetric_mT2_332_CalcTool )
  DECLARE_TOOL( Nt2_332_CalcTool )
  DECLARE_TOOL( M2C_332_CalcTool )

}
