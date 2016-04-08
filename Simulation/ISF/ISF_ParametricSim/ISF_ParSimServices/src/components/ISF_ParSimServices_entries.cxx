#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ISF_ParSimServices/ParametricSimSvc.h"

DECLARE_NAMESPACE_SERVICE_FACTORY( iParSim , ParametricSimSvc )

DECLARE_FACTORY_ENTRIES( ISF_FatrasServices ) {
  DECLARE_NAMESPACE_SERVICE( iParSim ,  ParametricSimSvc )
}   
