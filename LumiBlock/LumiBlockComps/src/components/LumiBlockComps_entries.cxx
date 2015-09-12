#include "GaudiKernel/DeclareFactoryEntries.h"
#include "LumiBlockComps/CreateLumiBlockCollectionFromFile.h"
//#include "LumiBlockComps/CreateAANTFromLumiBlockCollection.h"
#include "LumiBlockComps/LumiBlockMetaDataTool.h"
#include "LumiBlockComps/LumiBlockMuTool.h"
#include "LumiBlockComps/LuminosityTool.h"
#include "LumiBlockComps/TrigLivefractionTool.h"
#include "LumiBlockComps/LumiCalcSvc.h"
#include "LumiBlockComps/LumiBlockMuWriter.h"
#include "LumiBlockComps/LumiBlockTester.h"

DECLARE_ALGORITHM_FACTORY( CreateLumiBlockCollectionFromFile )
//DECLARE_ALGORITHM_FACTORY( CreateAANTFromLumiBlockCollection )
DECLARE_ALGORITHM_FACTORY( LumiBlockMuWriter )
DECLARE_ALGORITHM_FACTORY( LumiBlockTester )
DECLARE_TOOL_FACTORY( LumiBlockMetaDataTool )
DECLARE_TOOL_FACTORY( LumiBlockMuTool )
DECLARE_TOOL_FACTORY( LuminosityTool )
DECLARE_TOOL_FACTORY( TrigLivefractionTool )
DECLARE_SERVICE_FACTORY( LumiCalcSvc )

DECLARE_FACTORY_ENTRIES(LumiBlockComps) {
  DECLARE_ALGORITHM( CreateLumiBlockCollectionFromFile );
  //  DECLARE_ALGORITHM( CreateAANTFromLumiBlockCollection );
  DECLARE_ALGORITHM( LumiBlockMuWriter );
  DECLARE_ALGORITHM( LumiBlockTester );
  DECLARE_ALGTOOL( LumiBlockMetaDataTool);
  DECLARE_ALGTOOL( LumiBlockMuTool);
  DECLARE_ALGTOOL( LuminosityTool);
  DECLARE_ALGTOOL( TrigLivefractionTool);
  DECLARE_SERVICE( LumiCalcSvc );
}

