#include "LumiBlockComps/CreateLumiBlockCollectionFromFile.h"
//#include "LumiBlockComps/CreateAANTFromLumiBlockCollection.h"
#include "LumiBlockComps/LumiBlockMetaDataTool.h"

#ifndef XAOD_ANALYSIS
//full-athena-only components
#include "LumiBlockComps/LumiBlockMuTool.h"
#include "LumiBlockComps/LumiCalcSvc.h"
#include "LumiBlockComps/LumiBlockTester.h"
#include "../LBDurationCondAlg.h"
#include "../LuminosityCondAlg.h"
#include "../BunchCrossingCondAlg.h"
#include "../TrigLiveFractionCondAlg.h"

#include "../BunchCrossingCondTest.h"
#endif

DECLARE_COMPONENT( CreateLumiBlockCollectionFromFile )
//DECLARE_COMPONENT( CreateAANTFromLumiBlockCollection )
#ifndef XAOD_ANALYSIS
DECLARE_COMPONENT( LumiBlockTester )
DECLARE_COMPONENT( LumiBlockMuTool )
DECLARE_COMPONENT( LumiCalcSvc )
DECLARE_COMPONENT( LBDurationCondAlg )
DECLARE_COMPONENT( LuminosityCondAlg )
DECLARE_COMPONENT( BunchCrossingCondAlg )
DECLARE_COMPONENT( TrigLiveFractionCondAlg )
DECLARE_COMPONENT ( BunchCrossingCondTest )
#endif
DECLARE_COMPONENT( LumiBlockMetaDataTool )

#include "../xAOD2NtupLumiSvc.h"
DECLARE_COMPONENT( xAOD2NtupLumiSvc )

