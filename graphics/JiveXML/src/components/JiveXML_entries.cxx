#include "JiveXML/AlgoJiveXML.h"
#include "JiveXML/StreamToFileTool.h"
#include "JiveXML/StreamToServerTool.h"
#include "JiveXML/ONCRPCServerSvc.h"
#include "JiveXML/ExternalONCRPCServerSvc.h"
#ifndef __APPLE__
#include "JiveXML/XMLFormatTool.h"
#endif

DECLARE_COMPONENT( JiveXML::AlgoJiveXML )
DECLARE_COMPONENT( JiveXML::ONCRPCServerSvc )
DECLARE_COMPONENT( JiveXML::ExternalONCRPCServerSvc )
DECLARE_COMPONENT( JiveXML::StreamToFileTool )
DECLARE_COMPONENT( JiveXML::StreamToServerTool )
#ifndef __APPLE__
DECLARE_COMPONENT( JiveXML::XMLFormatTool )
#endif

