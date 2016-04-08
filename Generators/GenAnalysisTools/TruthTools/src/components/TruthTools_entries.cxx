// $Id: TruthTools_entries.cxx,v 1.2 2008-05-29 20:08:41 jkatzy Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TruthTools/TruthSelector.h"

DECLARE_TOOL_FACTORY	( TruthSelector )

DECLARE_FACTORY_ENTRIES( TruthTools ) 
{
    DECLARE_TOOL	( TruthSelector )
}

