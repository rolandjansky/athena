#include "GaudiKernel/DeclareFactoryEntries.h"

#include "MuonMDT_Cabling/MuonMDT_CablingSvc.h"
#include "MuonMDT_Cabling/MdtTestCabling.h"

DECLARE_SERVICE_FACTORY(MuonMDT_CablingSvc)
DECLARE_ALGORITHM_FACTORY(MdtTestCabling)

DECLARE_FACTORY_ENTRIES(MuonMDT_Cabling) {

  DECLARE_SERVICE(MuonMDT_CablingSvc)
  DECLARE_ALGORITHM(MdtTestCabling)

}
