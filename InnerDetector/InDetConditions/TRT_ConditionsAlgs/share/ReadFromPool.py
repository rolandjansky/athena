#
# jobOptions to read TRT conditions from a POOL root file
# use as include in a RecExample

#load relevant libraries
if not "TRT_ConditionsAlgs" in theApp.Dlls:
    theApp.Dlls += [ "TRT_ConditionsAlgs" ]
# include the converters
include ( "InDetConditionsAthenaPool/InDetConditionsAthenaPool_joboptions.py" )
include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
#include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
#theApp.Dlls += [ "EventSelectorAthenaPool" ]
 
#ProxyProviderSvc = Service( "ProxyProviderSvc" )
#ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]

#CondProxyProvider = Service( "CondProxyProvider" )
#CondProxyProvider.InputCollections = ["PhotonCalibration.root", "Alignment_2102540_2102999.root"]
#CondProxyProvider.InputCollections = ["FinalCalibration.root", "Alignment_2102464_2102539.root"]
#CondProxyProvider.InputCollections = ["FinalCalibration.root", "Alignment_2102411_2102463.root"]
#CondProxyProvider.InputCollections = ["FinalCalibration.root", "Alignment_2102372_2102410.root"]
#CondProxyProvider.InputCollections = ["LatestCalibration.root", "Alignment_2102312_2102371.root"]
#CondProxyProvider.InputCollections = ["LatestCalibration.root"]
#CondProxyProvider.InputCollections = ["FinalCalibration.root", "Alignment_2102298_2102311.root"]
#CondProxyProvider.InputCollections = ["FinalCalibration.root", "Alignment_2102226_2102297.root"]
#CondProxyProvider.InputCollections = ["FinalCalibration.root", "Alignment_2102182_2102225.root"]
#CondProxyProvider.InputCollections = ["FinalCalibration.root", "Alignment_2102151_2102181.root"]
#CondProxyProvider.InputCollections = ["FinalCalibration.root", "Alignment_2102121_2102150.root"]
#CondProxyProvider.InputCollections = ["FinalCalibration.root", "Alignment_2100000_2102120.root"]
#CondProxyProvider.InputCollection = "FinalCalibration.root"

#IOVSvc = Service("IOVSvc")
#IOVSvc.PreLoadData=True

IOVDbSvc.Folders += ["<dbConnection>impl=cool;techno=sqlite;schema=TrtAlign_07.db;DUMMY:COOLTEST</dbConnection> /TRT/Align<tag>TrtAlign07</tag>"]
#IOVDbSvc.Folders += ["<dbConnection>impl=cool;techno=sqlite;schema=align_trt_2102312_2102371.db;DUMMY:COOLTEST</dbConnection> /TRT/Align<tag>TrtAlign07</tag>"]


IOVDbSvc.Folders += ["<dbConnection>impl=cool;techno=sqlite;schema=TrtCalib_02.db;DUMMY:COOLTEST</dbConnection> /TRT/Calib/DF_B0<tag>TrtDfB0_02</tag>"]
IOVDbSvc.Folders += ["<dbConnection>impl=cool;techno=sqlite;schema=TrtCalib_02.db;DUMMY:COOLTEST</dbConnection> /TRT/Calib/DF_B1<tag>TrtDfB1_02</tag>"]
IOVDbSvc.Folders += ["<dbConnection>impl=cool;techno=sqlite;schema=TrtCalib_02.db;DUMMY:COOLTEST</dbConnection> /TRT/Calib/DF_B2<tag>TrtDfB2_02</tag>"]
IOVDbSvc.Folders += ["<dbConnection>impl=cool;techno=sqlite;schema=TrtCalib_02.db;DUMMY:COOLTEST</dbConnection> /TRT/Calib/T0_B0<tag>TrtT0B0_02</tag>"]
IOVDbSvc.Folders += ["<dbConnection>impl=cool;techno=sqlite;schema=TrtCalib_02.db;DUMMY:COOLTEST</dbConnection> /TRT/Calib/T0_B1<tag>TrtT0B1_02</tag>"]
IOVDbSvc.Folders += ["<dbConnection>impl=cool;techno=sqlite;schema=TrtCalib_02.db;DUMMY:COOLTEST</dbConnection> /TRT/Calib/T0_B2<tag>TrtT0B2_02</tag>"]
IOVDbSvc.Folders += ["<dbConnection>impl=cool;techno=sqlite;schema=TrtCalib_02.db;DUMMY:COOLTEST</dbConnection> /TRT/Calib/RT_B0<tag>TrtRtB0_02</tag>"]
IOVDbSvc.Folders += ["<dbConnection>impl=cool;techno=sqlite;schema=TrtCalib_02.db;DUMMY:COOLTEST</dbConnection> /TRT/Calib/RT_B1<tag>TrtRtB1_02</tag>"]
IOVDbSvc.Folders += ["<dbConnection>impl=cool;techno=sqlite;schema=TrtCalib_02.db;DUMMY:COOLTEST</dbConnection> /TRT/Calib/RT_B2<tag>TrtRtB2_02</tag>"]
IOVDbSvc.Folders += ["<dbConnection>impl=cool;techno=sqlite;schema=TrtCalib_02.db;DUMMY:COOLTEST</dbConnection> /TRT/Calib/Defaults<tag>TrtDe_02</tag>"]

#Explicitly specify the input file catalog
PoolSvc = Service( "PoolSvc" )
#PoolSvc.ReadCatalog += [ "file:PhotonCalibration.xml" ]
#PoolSvc.ReadCatalog += [ "file:LatestCalibration.xml" ]
PoolSvc.ReadCatalog += [ "file:TrtCalib_02.xml" ]
PoolSvc.ReadCatalog += [ "file:TrtAlign_07.xml" ]
#PoolSvc.ReadCatalog += [ "file:Alignment_2102540_2102999.xml" ]
#PoolSvc.ReadCatalog += [ "file:Alignment_2102464_2102539.xml" ]
#PoolSvc.ReadCatalog += [ "file:Alignment_2102411_2102463.xml" ]
#PoolSvc.ReadCatalog += [ "file:Alignment_2102372_2102410.xml" ]
#PoolSvc.ReadCatalog += [ "file:Alignment_2102312_2102371.xml" ]
#PoolSvc.ReadCatalog += [ "file:Alignment_2102298_2102311.xml" ]
#PoolSvc.ReadCatalog += [ "file:Alignment_2102226_2102297.xml" ]
#PoolSvc.ReadCatalog += [ "file:Alignment_2102182_2102225.xml" ]
#PoolSvc.ReadCatalog += [ "file:Alignment_2102151_2102181.xml" ]
#PoolSvc.ReadCatalog += [ "file:Alignment_2102121_2102150.xml" ]
#PoolSvc.ReadCatalog += [ "file:Alignment_2100000_2102120.xml" ]

ToolSvc = Service("ToolSvc")

# This needs to be false for testbeam before 12.0.0
#theApp.TopAlg = [ "TRT_TestRead" ]
ToolSvc.TRT_CondDbTool.useCOOL = True
#ToolSvc.TRT_CondDbTool.OutputLevel = 2
#ToolSvc.TRT_DriftFunctionTool.OutputLevel = 1


#  LocalWords:  PoolSvc
