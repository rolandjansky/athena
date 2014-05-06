// $Id: D3PDMakerReader_entries.cxx 348546 2011-03-01 15:09:56Z krasznaa $

// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "../ReaderAlg.h"
#include "../MultiReaderAlg.h"
#include "../RootReaderD3PDSvc.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( D3PD, ReaderAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( D3PD, MultiReaderAlg )
DECLARE_NAMESPACE_SERVICE_FACTORY( D3PD, RootReaderD3PDSvc )

DECLARE_FACTORY_ENTRIES( D3PDMakerReader ) {

   DECLARE_NAMESPACE_ALGORITHM( D3PD, ReaderAlg )
   DECLARE_NAMESPACE_ALGORITHM( D3PD, MultiReaderAlg )
   DECLARE_NAMESPACE_SERVICE( D3PD, RootReaderD3PDSvc )

}
