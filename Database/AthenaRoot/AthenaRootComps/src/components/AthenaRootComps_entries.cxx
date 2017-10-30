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

