// $Id$
/**
 * @file D3PDMakerRoot/src/components/D3PDMakerRoot_entries.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief List Gaudi components.
 */


#include "../RootD3PDSvc.h"
#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_NAMESPACE_SERVICE_FACTORY   (D3PD, RootD3PDSvc)


DECLARE_FACTORY_ENTRIES(D3PDMakerRoot) {
  DECLARE_NAMESPACE_SERVICE   (D3PD, RootD3PDSvc)
}
