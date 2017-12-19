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

DECLARE_COMPONENT( Athena::NtupleCnvSvc )
//DECLARE_CONVERTER_FACTORY( Athena::LeafCnv )

//DECLARE_CONVERTER_FACTORY( Athena::RootCnv )
DECLARE_COMPONENT( Athena::RootCnvSvc )
DECLARE_COMPONENT( Athena::RootSvc )
DECLARE_COMPONENT( Athena::RootOutputStreamTool )

DECLARE_COMPONENT( Athena::xAODCnvSvc )

DECLARE_COMPONENT( Athena::RootNtupleEventSelector )
DECLARE_COMPONENT( Athena::xAODEventSelector )
DECLARE_COMPONENT( Athena::RootNtupleOutputStream )
DECLARE_COMPONENT( Athena::RootNtupleOutputMetadataTool )

DECLARE_COMPONENT( Athena::RootAsciiDumperAlg )
DECLARE_COMPONENT( Athena::RootAsciiDumperAlgHandle )


DECLARE_COMPONENT( IoSvc )

