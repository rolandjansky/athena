#include "GaudiKernel/DeclareFactoryEntries.h"

#ifdef XAOD_ANALYSIS



#include "DiTauRec/HadElBuilder.h"
#include "DiTauRec/HadMuBuilder.h"

//using namespace DiTauRec;

DECLARE_NAMESPACE_TOOL_FACTORY(DiTauRec, HadElBuilder)
DECLARE_NAMESPACE_TOOL_FACTORY(DiTauRec, HadMuBuilder)
//DECLARE_TOOL_FACTORY(HadElBuilder)
//DECLARE_TOOL_FACTORY(HadMuBuilder)

DECLARE_FACTORY_ENTRIES( DiTauRec ) 
{
  DECLARE_NAMESPACE_TOOL( DiTauRec, HadElBuilder );
  DECLARE_NAMESPACE_TOOL( DiTauRec, HadMuBuilder );
}

#endif
