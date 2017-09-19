#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../NtupleCnvSvc.h"
//#include "../LeafCnv.h"
#include "../RootOutputStreamTool.h"

//#include "../RootCnv.h"
#include "../RootCnvSvc.h"
#include "../RootSvc.h"

#include "../RootNtupleEventSelector.h"
#include "../RootNtupleOutputStream.h"
#include "../RootNtupleOutputMetadataTool.h"

#include "../RootAsciiDumperAlg.h"
#include "../RootAsciiDumperAlgHandle.h"


#include "../IoSvc.h"

#include "../xAODCnvSvc.h"
#include "../xAODCnv.h"
#include "../xAODEventSelector.h"

DECLARE_NAMESPACE_SERVICE_FACTORY(Athena, NtupleCnvSvc)
//DECLARE_NAMESPACE_CONVERTER_FACTORY (Athena, LeafCnv)

//DECLARE_NAMESPACE_CONVERTER_FACTORY (Athena, RootCnv)
DECLARE_NAMESPACE_SERVICE_FACTORY( Athena, RootCnvSvc )
DECLARE_NAMESPACE_SERVICE_FACTORY( Athena, RootSvc )
DECLARE_NAMESPACE_TOOL_FACTORY( Athena, RootOutputStreamTool )

DECLARE_NAMESPACE_SERVICE_FACTORY( Athena, xAODCnvSvc )

DECLARE_NAMESPACE_SERVICE_FACTORY(Athena, RootNtupleEventSelector)
DECLARE_NAMESPACE_SERVICE_FACTORY(Athena, xAODEventSelector)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Athena, RootNtupleOutputStream)
DECLARE_NAMESPACE_TOOL_FACTORY( Athena, RootNtupleOutputMetadataTool )

DECLARE_NAMESPACE_ALGORITHM_FACTORY( Athena, RootAsciiDumperAlg )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Athena, RootAsciiDumperAlgHandle )


DECLARE_SERVICE_FACTORY( IoSvc )

DECLARE_FACTORY_ENTRIES( AthenaRootComps ) {
  DECLARE_NAMESPACE_SERVICE(Athena, NtupleCnvSvc)
  DECLARE_NAMESPACE_CONVERTER(Athena, LeafCnv)
  DECLARE_NAMESPACE_CONVERTER(Athena, xAODCnv)

  DECLARE_NAMESPACE_CONVERTER(Athena, RootCnv)
  DECLARE_NAMESPACE_SERVICE( Athena, RootCnvSvc )
  DECLARE_NAMESPACE_CONVERTER(Athena, RootCnv)
  DECLARE_NAMESPACE_SERVICE( Athena, xAODCnvSvc )
  DECLARE_NAMESPACE_TOOL(Athena, RootOutputStreamTool)

  DECLARE_NAMESPACE_SERVICE(Athena, RootNtupleEventSelector)
  DECLARE_NAMESPACE_SERVICE(Athena, xAODEventSelector)
  DECLARE_NAMESPACE_ALGORITHM(Athena, RootNtupleOutputStream)
  DECLARE_NAMESPACE_TOOL(Athena, RootNtupleOutputMetadataTool)

  DECLARE_NAMESPACE_ALGORITHM(Athena, RootAsciiDumperAlg)
  DECLARE_NAMESPACE_ALGORITHM(Athena, RootAsciiDumperAlgHandle)


  DECLARE_SERVICE( IoSvc )
}
