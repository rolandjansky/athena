/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/DeclareFactoryEntries.h"

#include "AFP_ByteStream2RawCnv/AFP_ByteStream2RawCnv.h"
DECLARE_TOOL_FACTORY( AFP_ByteStream2RawCnv )

#include "AFP_ByteStream2RawCnv/AFP_RawDataProviderTool.h"
DECLARE_TOOL_FACTORY( AFP_RawDataProviderTool )

#include "AFP_ByteStream2RawCnv/AFP_RawDataProvider.h"
DECLARE_ALGORITHM_FACTORY( AFP_RawDataProvider )

DECLARE_FACTORY_ENTRIES( AFP_ByteStream2RawCnv ) 
{
  DECLARE_ALGORITHM( AFP_RawDataProvider );
  DECLARE_TOOL( AFP_RawDataProviderTool );
  DECLARE_TOOL( AFP_ByteStream2RawCnv );
}
