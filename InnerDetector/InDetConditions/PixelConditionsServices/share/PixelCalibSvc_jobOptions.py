include.block("PixelConditionsServices/PixelCalibSvc_jobOptions.py")
# load library
#theApp.Dlls += [ "PixelConditionsServices" ]

#from AthenaCommon.AppMgr import ToolSvc

# configure PixelCalibSvc
from PixelConditionsServices.PixelConditionsServicesConf import PixelCalibSvc
ServiceMgr +=PixelCalibSvc()
PixelCalibSvc = ServiceMgr.PixelCalibSvc

# configure PixelCalibDbTool 
#from PixelConditionsTools.PixelConditionsToolsConf import PixelCalibDbTool
#ToolSvc +=PixelCalibDbTool()

#theApp.ExtSvc += [ "PixelCalibSvc" ]
#PixelCalibSvc = Service( "PixelCalibSvc" )

#theApp.Dlls += [ "PixelConditionsTools" ]
#theApp.Dlls +=["DetDescrCondTools","DetDescrCondExample"]
#if 'InDetCabling' not in theApp.Dlls:
#    theApp.Dlls += [ "InDetCabling" ]

# setup for ToolSvc
#ToolSvc = Service( "ToolSvc" )
#ToolSvc.ConditionsAlgStream.OutputFile="dummy.root"
# this example is for a local SQlite file mysqlfile.db, COOL DB named COOLTEST
#IOVDbSvc.dbConnection="impl=cool;techno=oracle;ATLAS_COOLPROD:COMP130:ATLAS_COOL_READER:COOLRED4PRD"
#IOVDbSvc.Folders+=[CondDBCool.PIXEL + "/PIXEL/PixCalib" + "<tag>PixCalib-Assembly-01</tag>"]
#IOVDbSvc.GlobalTag ="PixCalib-Assembly-00"
#IOVDbSvc.OutputLevel = DEBUG
from IOVDbSvc.CondDB import conddb
conddb.addFolder("PIXEL_OFL","/PIXEL/PixCalib")

# setup PixelCalibDbTool in ToolSvc
#PixelCalibDbTool = Service("ToolSvc.PixelCalibDbTool")
#ToolSvc.PixelCalibDbTool.CalibFolder ="/PIXEL/PixCalib"
#ToolSvc.PixelCalibDbTool.CalibLocation ="PixCalibKey"
#ToolSvc.PixelCalibDbTool.WriteDB =False
