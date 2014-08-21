#ServerHost='localhost'

if 'ServerHost' in dir():
    from JiveXML.JiveXMLConf import JiveXML__ExternalONCRPCServerSvc
    ServiceMgr += JiveXML__ExternalONCRPCServerSvc("ONCRPCServerSvc",
                                                    Hostname=ServerHost,
                                                    OutputLevel=DEBUG)
else:
    from JiveXML.JiveXMLConf import JiveXML__ONCRPCServerSvc
    ServiceMgr += JiveXML__ONCRPCServerSvc("ONCRPCServerSvc",
                                            OutputLevel=DEBUG)

from JiveXML.JiveXMLConf import JiveXML__StreamToServerTool
StreamToServerTool = JiveXML__StreamToServerTool(
        OutputLevel=DEBUG,
        ServerService=ServiceMgr.ONCRPCServerSvc,
        StreamName="Test")
ToolSvc += StreamToServerTool
theEventData2XML.StreamTools += [ StreamToServerTool ]
