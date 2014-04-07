#include "GaudiKernel/DeclareFactoryEntries.h"

#include "MuonGeoModelTest/MuonGMCheck.h"
#include "MuonGeoModelTest/MuonGMTestOnPrd.h"
#include "MuonGeoModelTest/MuonHitRelocation.h"

DECLARE_ALGORITHM_FACTORY( MuonGMCheck )
DECLARE_ALGORITHM_FACTORY( MuonGMTestOnPrd )
DECLARE_ALGORITHM_FACTORY( MuonHitRelocation )

DECLARE_FACTORY_ENTRIES(MuonGeoModelTest) {
  DECLARE_ALGORITHM( MuonGMCheck )
  DECLARE_ALGORITHM( MuonGMTestOnPrd )
  DECLARE_ALGORITHM( MuonHitRelocation )
}
