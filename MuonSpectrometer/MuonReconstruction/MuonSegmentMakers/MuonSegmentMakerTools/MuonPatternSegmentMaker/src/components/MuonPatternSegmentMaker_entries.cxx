#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuonPatternSegmentMaker/MuonPatternSegmentMaker.h"
#include "MuonPatternSegmentMaker/MuonPatternCalibration.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonPatternSegmentMaker )
DECLARE_NAMESPACE_TOOL_FACTORY( Muon, MuonPatternCalibration )

DECLARE_FACTORY_ENTRIES( MuonPatternSegmentMaker )
{
  DECLARE_NAMESPACE_TOOL( Muon, MuonPatternSegmentMaker )
}
DECLARE_FACTORY_ENTRIES( MuonPatternCalibration )
{
  DECLARE_NAMESPACE_TOOL( Muon, MuonPatternCalibration )
}


