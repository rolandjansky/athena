# include file fragment for ReadTopTrans algorithm
# access COOL conditions database for TOPTRANS object
# algorithm demonstrates access through execute, callback and BeginRun incident
#
include( "IOVDbSvc/IOVDbSvc_jobOptions.py" )
IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.dbConnection = "impl=cool;techno=oracle;devdb10:TESTCOOL:ATLAS_COOLTEST:pwd4cool"


IOVDbSvc.Folders+= ["/GLOBAL/GEOM/TOPTRANS" ]
IOVDbSvc.OutputLevel=2

include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )

theApp.Dlls+=["DetDescrCondExample"]
theApp.TopAlg+= [ "ReadTopTrans" ]
