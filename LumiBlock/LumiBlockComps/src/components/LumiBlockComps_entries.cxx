#include "LumiBlockComps/CreateLumiBlockCollectionFromFile.h"
//#include "LumiBlockComps/CreateAANTFromLumiBlockCollection.h"
#include "LumiBlockComps/LumiBlockMetaDataTool.h"

#ifndef XAOD_ANALYSIS
//full-athena-only components
#include "LumiBlockComps/LumiBlockMuTool.h"
#include "LumiBlockComps/LuminosityTool.h"
#include "LumiBlockComps/TrigLivefractionTool.h"
#include "LumiBlockComps/LumiCalcSvc.h"
#include "LumiBlockComps/LumiBlockMuWriter.h"
#include "LumiBlockComps/LumiBlockTester.h"
#endif

DECLARE_COMPONENT( CreateLumiBlockCollectionFromFile )
//DECLARE_COMPONENT( CreateAANTFromLumiBlockCollection )
#ifndef XAOD_ANALYSIS
DECLARE_COMPONENT( LumiBlockMuWriter )
DECLARE_COMPONENT( LumiBlockTester )
DECLARE_COMPONENT( LumiBlockMuTool )
DECLARE_COMPONENT( LuminosityTool )
DECLARE_COMPONENT( TrigLivefractionTool )
DECLARE_COMPONENT( LumiCalcSvc )
#endif
DECLARE_COMPONENT( LumiBlockMetaDataTool )

#include "../xAOD2NtupLumiSvc.h"
DECLARE_COMPONENT( xAOD2NtupLumiSvc )

