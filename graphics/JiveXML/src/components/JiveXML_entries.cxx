#include "GaudiKernel/DeclareFactoryEntries.h"
#include "JiveXML/AlgoJiveXML.h"
#include "JiveXML/StreamToFileTool.h"
#include "JiveXML/StreamToServerTool.h"
#include "JiveXML/ONCRPCServerSvc.h"
#include "JiveXML/ExternalONCRPCServerSvc.h"
#ifndef __APPLE__
#include "JiveXML/XMLFormatTool.h"
#endif

DECLARE_NAMESPACE_ALGORITHM_FACTORY( JiveXML , AlgoJiveXML )
DECLARE_NAMESPACE_SERVICE_FACTORY( JiveXML, ONCRPCServerSvc )
DECLARE_NAMESPACE_SERVICE_FACTORY( JiveXML, ExternalONCRPCServerSvc )
DECLARE_NAMESPACE_TOOL_FACTORY( JiveXML , StreamToFileTool )
DECLARE_NAMESPACE_TOOL_FACTORY( JiveXML , StreamToServerTool )
#ifndef __APPLE__
DECLARE_NAMESPACE_TOOL_FACTORY( JiveXML , XMLFormatTool )
#endif

DECLARE_FACTORY_ENTRIES( JiveXML ) {
  DECLARE_NAMESPACE_ALGORITHM( JiveXML , AlgoJiveXML)
  DECLARE_NAMESPACE_SERVICE( JiveXML, ONCRPCServerSvc )
  DECLARE_NAMESPACE_SERVICE( JiveXML, ExternalONCRPCServerSvc )
  DECLARE_NAMESPACE_TOOL ( JiveXML , StreamToFileTool )
  DECLARE_NAMESPACE_TOOL ( JiveXML , StreamToServerTool )
#ifndef __APPLE__
  DECLARE_NAMESPACE_TOOL ( JiveXML , XMLFormatTool )
#endif
}

