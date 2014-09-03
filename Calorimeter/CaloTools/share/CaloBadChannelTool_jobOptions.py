#
# jobOptions for the instance CaloBadChannelTool of ICaloBadChannelTool 
# 
theApp.Dlls += [ "CaloTools" ]

ToolSvc = Service( "ToolSvc" )

if 'GlobalFlags.DetGeo' in dir():
    if( GlobalFlags.DetGeo.is_ctbh8() ) : 
        ToolSvc.CaloBadChannelTool.geometry='H8'
    else :
        ToolSvc.CaloBadChannelTool.geometry='Atlas'

else :
    ToolSvc.CaloBadChannelTool.geometry='Atlas'
