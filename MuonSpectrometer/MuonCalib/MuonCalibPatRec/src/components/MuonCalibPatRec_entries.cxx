#include "GaudiKernel/DeclareFactoryEntries.h"

#include "MuonCalibPatRec/MuonSegmentToCalibSegment.h"

using namespace MuonCalib;

DECLARE_ALGORITHM_FACTORY ( MuonSegmentToCalibSegment )

DECLARE_FACTORY_ENTRIES(MuonCalibPatRec) {
  DECLARE_ALGORITHM(MuonSegmentToCalibSegment)
}
