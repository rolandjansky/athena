#include "../InDetRawDataFakeWriter.h"
#include "../InDetRawDataFakeReader.h"
#include "../TrkTrackFakeWriter.h"
#include "../TrkTrackFakeReader.h"
#include "../LArCellContFakeWriter.h"
#include "../LArCellContFakeReader.h"
// #include "../CaloClusterFakeWriter.h"
// #include "../CaloClusterFakeReader.h"
#include "../AthenaPoolTestDataWriter.h"
#include "../AthenaPoolTestDataReader.h"
#include "../AthenaPoolTestAttrWriter.h"
#include "../AthenaPoolTestAttrReader.h"
#include "../EventInfoWriter.h"
#include "../EventInfoReader.h"
#include "../PileUpEventInfoWriter.h"
#include "../PileUpEventInfoReader.h"

DECLARE_ALGORITHM_FACTORY(AthenaPoolTestDataWriter)
DECLARE_ALGORITHM_FACTORY(AthenaPoolTestDataReader)
DECLARE_ALGORITHM_FACTORY(AthenaPoolTestAttrWriter)
DECLARE_ALGORITHM_FACTORY(AthenaPoolTestAttrReader)
// DECLARE_ALGORITHM_FACTORY(CaloClusterFakeWriter)
// DECLARE_ALGORITHM_FACTORY(CaloClusterFakeReader)
DECLARE_ALGORITHM_FACTORY(InDetRawDataFakeWriter)
DECLARE_ALGORITHM_FACTORY(InDetRawDataFakeReader)
DECLARE_ALGORITHM_FACTORY(TrkTrackFakeWriter)
DECLARE_ALGORITHM_FACTORY(TrkTrackFakeReader)
DECLARE_ALGORITHM_FACTORY(LArCellContFakeWriter)
DECLARE_ALGORITHM_FACTORY(LArCellContFakeReader)
DECLARE_ALGORITHM_FACTORY(EventInfoWriter)
DECLARE_ALGORITHM_FACTORY(EventInfoReader)
DECLARE_ALGORITHM_FACTORY(PileUpEventInfoWriter)
DECLARE_ALGORITHM_FACTORY(PileUpEventInfoReader)

