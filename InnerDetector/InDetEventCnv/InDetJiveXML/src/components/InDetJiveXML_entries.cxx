#include "InDetJiveXML/SiSpacePointRetriever.h"
#include "InDetJiveXML/TRTRetriever.h"
#include "InDetJiveXML/SiClusterRetriever.h"
#include "InDetJiveXML/PixelClusterRetriever.h"
#include "InDetJiveXML/SCTRDORetriever.h"
#include "InDetJiveXML/PixelRDORetriever.h"
#include "InDetJiveXML/InDetGeoModelTool.h"
#include "InDetJiveXML/BeamSpotRetriever.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace JiveXML;

DECLARE_TOOL_FACTORY(SiSpacePointRetriever)
DECLARE_TOOL_FACTORY(TRTRetriever)
DECLARE_TOOL_FACTORY(SiClusterRetriever)
DECLARE_TOOL_FACTORY(PixelClusterRetriever)
DECLARE_TOOL_FACTORY(SCTRDORetriever)
DECLARE_TOOL_FACTORY(PixelRDORetriever)
DECLARE_TOOL_FACTORY(InDetGeoModelTool)
DECLARE_TOOL_FACTORY(BeamSpotRetriever)

DECLARE_FACTORY_ENTRIES(InDetJiveXML) {
  DECLARE_ALGTOOL(SiSpacePointRetriever)
  DECLARE_ALGTOOL(TRTRetriever)
  DECLARE_ALGTOOL(SiClusterRetriever)
  DECLARE_ALGTOOL(PixelClusterRetriever)
  DECLARE_ALGTOOL(SCTRDORetriever)
  DECLARE_ALGTOOL(PixelRDORetriever)
  DECLARE_ALGTOOL(InDetGeoModelTool)
  DECLARE_ALGTOOL(BeamSpotRetriever)
}
