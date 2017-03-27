/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "G4AtlasTests/G4TestAlg.h"
#include "SiHitsTestTool.h"
#include "TrtHitsTestTool.h"
#include "LArHitsTestTool.h"
#include "EMBHitsTestTool.h"
#include "EMECHitsTestTool.h"
#include "FCALHitsTestTool.h"
#include "HECHitsTestTool.h"
#include "TileHitsTestTool.h"
#include "MDTHitsTestTool.h"
#include "RPCHitsTestTool.h"
#include "CSCHitsTestTool.h"
#include "TGCHitsTestTool.h"
#include "MMHitsTestTool.h"
#include "sTGCHitsTestTool.h"
#include "ALFA_SimHitsTestTool.h"
#include "ZDCHitsTestTool.h"
#include "TruthTestTool.h"
#include "LayerTestTool.h"
#include "CaloCalibrationHitsTestTool.h"
#include "SteppingValidationTool.h"
#include "LucidHitsTestTool.h"
#include "CalibHitValidate.h"

DECLARE_FACTORY_ENTRIES(G4AtlasTests)
{
  DECLARE_ALGTOOL( TruthTestTool )
  DECLARE_ALGTOOL( SiHitsTestTool )
  DECLARE_ALGTOOL( TrtHitsTestTool )
  DECLARE_ALGTOOL( LArHitsTestTool )
  DECLARE_ALGTOOL( EMBHitsTestTool )
  DECLARE_ALGTOOL( EMECHitsTestTool )
  DECLARE_ALGTOOL( FCALHitsTestTool )
  DECLARE_ALGTOOL( HECHitsTestTool )
  DECLARE_ALGTOOL( TileHitsTestTool )
  DECLARE_ALGTOOL( MDTHitsTestTool )
  DECLARE_ALGTOOL( MMHitsTestTool )
  DECLARE_ALGTOOL( sTGCHitsTestTool )
  DECLARE_ALGTOOL( RPCHitsTestTool )
  DECLARE_ALGTOOL( CSCHitsTestTool )
  DECLARE_ALGTOOL( TGCHitsTestTool )
  DECLARE_ALGTOOL( ALFA_SimHitsTestTool )
  DECLARE_ALGTOOL( ZDCHitsTestTool )
  DECLARE_ALGTOOL( CaloCalibrationHitsTestTool )
  DECLARE_ALGTOOL( LayerTestTool )
  DECLARE_ALGTOOL( G4UA::SteppingValidationTool )
  DECLARE_ALGTOOL( LucidHitsTestTool )
  DECLARE_ALGORITHM( G4TestAlg )
  DECLARE_ALGORITHM( CalibHitValidate )
}

DECLARE_TOOL_FACTORY( TruthTestTool )
DECLARE_TOOL_FACTORY( SiHitsTestTool )
DECLARE_TOOL_FACTORY( TrtHitsTestTool )
DECLARE_TOOL_FACTORY( LArHitsTestTool )
DECLARE_TOOL_FACTORY( EMBHitsTestTool )
DECLARE_TOOL_FACTORY( EMECHitsTestTool )
DECLARE_TOOL_FACTORY( FCALHitsTestTool )
DECLARE_TOOL_FACTORY( HECHitsTestTool )
DECLARE_TOOL_FACTORY( TileHitsTestTool )
DECLARE_TOOL_FACTORY( MDTHitsTestTool )
DECLARE_TOOL_FACTORY( RPCHitsTestTool )
DECLARE_TOOL_FACTORY( CSCHitsTestTool )
DECLARE_TOOL_FACTORY( TGCHitsTestTool )
DECLARE_TOOL_FACTORY( MMHitsTestTool )
DECLARE_TOOL_FACTORY( sTGCHitsTestTool )
DECLARE_TOOL_FACTORY( ALFA_SimHitsTestTool )
DECLARE_TOOL_FACTORY( ZDCHitsTestTool )
DECLARE_TOOL_FACTORY( CaloCalibrationHitsTestTool )
DECLARE_TOOL_FACTORY( LayerTestTool )
DECLARE_TOOL_FACTORY( G4UA::SteppingValidationTool )
DECLARE_ALGORITHM_FACTORY( G4TestAlg )
DECLARE_ALGORITHM_FACTORY( CalibHitValidate )
DECLARE_TOOL_FACTORY( LucidHitsTestTool )
