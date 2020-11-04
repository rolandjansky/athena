#include "GaudiKernel/DeclareFactoryEntries.h"
// Atlas AlgTools
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"
#include "InDetTrackingGeometryXML/GMXReaderSvc.h"
#include "InDetTrackingGeometryXML/HGTD_IdDictSvc.h"
#include "InDetTrackingGeometryXML/LayerProviderXML.h"
#include "InDetTrackingGeometryXML/EndcapBuilderXML.h"
#include "InDetTrackingGeometryXML/BarrelBuilderXML.h"
#include "InDetTrackingGeometryXML/StaveBuilderXML.h"
#include "InDetTrackingGeometryXML/SiModuleProvider.h"

using namespace InDet;

DECLARE_SERVICE_FACTORY( XMLReaderSvc )
DECLARE_SERVICE_FACTORY( GMXReaderSvc )
DECLARE_SERVICE_FACTORY( HGTD_IdDictSvc )
DECLARE_TOOL_FACTORY( LayerProviderXML )
DECLARE_TOOL_FACTORY( EndcapBuilderXML )
DECLARE_TOOL_FACTORY( BarrelBuilderXML )
DECLARE_TOOL_FACTORY( StaveBuilderXML )
DECLARE_TOOL_FACTORY( SiModuleProvider )

DECLARE_FACTORY_ENTRIES( InDetTrackingGeometryXML )
{
    DECLARE_SERVICE( XMLReaderSvc )
    DECLARE_SERVICE( GMXReaderSvc )
    DECLARE_SERVICE( HGTD_IdDictSvc )
    DECLARE_TOOL( LayerProviderXML )
    DECLARE_TOOL( EndcapBuilderXML )
    DECLARE_TOOL( BarrelBuilderXML )
    DECLARE_TOOL( StaveBuilderXML )
    DECLARE_TOOL( SiModuleProvider )
}


