#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigTrkValAlgs/TrigTrkValidationNtupleWriter.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trk, TrigTrkValidationNtupleWriter  )


DECLARE_FACTORY_ENTRIES( TrigTrkValAlgs )
{
    DECLARE_NAMESPACE_ALGORITHM( Trk, TrigTrkValidationNtupleWriter )
}
