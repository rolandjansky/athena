#include"GaudiKernel/DeclareFactoryEntries.h"
#include "TrigTrkValTools/RoIPropertyNtupleTool.h"
#include "TrigTrkValTools/RoIToTrackLinkNtupleTool.h"
#include "TrigTrkValTools/TrigL2ValidationNtupleTool.h"

// using namespace Trk;
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, RoIPropertyNtupleTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trk, RoIToTrackLinkNtupleTool )
DECLARE_TOOL_FACTORY( TrigL2ValidationNtupleTool )

DECLARE_FACTORY_ENTRIES(TrigTrkValTools)
{ // in this package name these methods are registered
  DECLARE_NAMESPACE_TOOL(Trk, RoIPropertyNtupleTool )
  DECLARE_NAMESPACE_TOOL(Trk, RoIToTrackLinkNtupleTool )
  DECLARE_TOOL( TrigL2ValidationNtupleTool )
}

