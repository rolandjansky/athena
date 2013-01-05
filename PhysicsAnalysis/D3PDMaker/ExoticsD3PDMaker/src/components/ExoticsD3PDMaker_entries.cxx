#include "../ZPrimeElectronSelector.h"
#include "../ZPrimeMuonSelector.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(D3PD, ZPrimeElectronSelector )
DECLARE_NAMESPACE_ALGORITHM_FACTORY(D3PD, ZPrimeMuonSelector )
DECLARE_FACTORY_ENTRIES(ExoticsD3PDMaker) {
DECLARE_NAMESPACE_ALGORITHM(D3PD, ZPrimeElectronSelector )
DECLARE_NAMESPACE_ALGORITHM(D3PD, ZPrimeMuonSelector )
}
