/**
 * @file AthenaPoolTest_entries.cxx
 *
 * @brief Declaration of component entry points
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: AthenaPoolTest_entries.cxx,v 1.25 2007-12-05 15:47:15 emoyse Exp $
 *
 */

#include "GaudiKernel/DeclareFactoryEntries.h"
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

DECLARE_FACTORY_ENTRIES(AthenaPoolTest) {
  DECLARE_ALGORITHM(AthenaPoolTestDataWriter)
  DECLARE_ALGORITHM(AthenaPoolTestDataReader)
  DECLARE_ALGORITHM(AthenaPoolTestAttrWriter)
  DECLARE_ALGORITHM(AthenaPoolTestAttrReader)
  // DECLARE_ALGORITHM(CaloClusterFakeWriter)
  // DECLARE_ALGORITHM(CaloClusterFakeReader)
  DECLARE_ALGORITHM(InDetRawDataFakeWriter)
  DECLARE_ALGORITHM(InDetRawDataFakeReader)
  DECLARE_ALGORITHM(LArCellContFakeWriter)
  DECLARE_ALGORITHM(LArCellContFakeReader)
  DECLARE_ALGORITHM(TrkTrackFakeWriter)
  DECLARE_ALGORITHM(TrkTrackFakeReader)
  DECLARE_ALGORITHM(EventInfoWriter)
  DECLARE_ALGORITHM(EventInfoReader)
  DECLARE_ALGORITHM(PileUpEventInfoWriter)
  DECLARE_ALGORITHM(PileUpEventInfoReader)
}
