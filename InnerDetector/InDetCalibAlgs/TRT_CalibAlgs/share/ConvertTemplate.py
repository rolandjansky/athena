import os,sys,time,glob,fnmatch

def txt2pooloptionsDATA(calibconstants,outputfile,rttag,t0tag,IOV): 
    print "Reading from txt2pooloptionsDATA"
    ostring="""

#New way to create cool and pool file, easier and more stable:

from AthenaCommon.GlobalFlags import GlobalFlags

GlobalFlags.DetGeo.set_atlas()

#GlobalFlags.DataSource.set_geant4()
GlobalFlags.DataSource.set_data()



from IOVDbSvc.CondDB import conddb
include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )
GlobalFlags.DetDescrVersion ="ATLAS-CSC-02-01-00"


from AthenaCommon.DetFlags import DetFlags
DetFlags.TRT_setOn()
DetFlags.detdescr.TRT_setOn()

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

include("RegistrationServices/RegistrationServices_jobOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

svcMgr.IOVDbSvc.forceRunNumber=%d

from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import AthenaPoolOutputStreamTool
TRTCondStream=AthenaPoolOutputStreamTool(name="CondStream1",OutputFile="trtcalibout.pool.root")
ToolSvc += TRTCondStream
print TRTCondStream

conddb.blockFolder("/TRT/Calib/RT" )
conddb.blockFolder("/TRT/Calib/T0" )

from AthenaCommon.AppMgr import ServiceMgr

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRTCalDbSvc
TRTCalSvc=TRTCalDbSvc(name="TRTCalDbSvc",StreamTool=TRTCondStream)
TRTCalSvc.calibTextFile = "%s"
ServiceMgr += TRTCalSvc
print TRTCalSvc

from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTCondWrite
TRTCondWrite=TRTCondWrite( name = "TRTCondWrite",
                               CalibInputFile= "%s")

topSequence+=TRTCondWrite
                             
outputFile = "%s"
objectList = [ "TRTCond::RtRelationMultChanContainer#/TRT/Calib/RT","TRTCond::StrawT0MultChanContainer#/TRT/Calib/T0"]
tagList = ["%s",
           "%s"]
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg(outputFile=outputFile)
myOCA.ObjectList=objectList
myOCA.IOVTagList=tagList
myOCA.WriteIOV=True

myOCA.Run1=%d
myOCA.LB1=%d
myOCA.Run2=%d
myOCA.LB2=%d

topSequence+=myOCA

""" % (IOV[0],calibconstants,calibconstants,outputfile,rttag,t0tag,IOV[0],IOV[1],IOV[2],IOV[3])
    return ostring


def txt2pooloptionsMC(calibconstants,outputfile,rttag,t0tag,IOV):
    print "Reading from txt2pooloptionsRAW"
    ostring="""

#New way to create cool and pool file, easier and more stable:

from AthenaCommon.GlobalFlags import GlobalFlags

GlobalFlags.DetGeo.set_atlas()

GlobalFlags.DataSource.set_geant4()
#GlobalFlags.DataSource.set_data()



from IOVDbSvc.CondDB import conddb
include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )
GlobalFlags.DetDescrVersion ="ATLAS-CSC-02-01-00"


from AthenaCommon.DetFlags import DetFlags
DetFlags.TRT_setOn()
DetFlags.detdescr.TRT_setOn()

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

include("RegistrationServices/RegistrationServices_jobOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

svcMgr.IOVDbSvc.forceRunNumber=%d

from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import AthenaPoolOutputStreamTool
TRTCondStream=AthenaPoolOutputStreamTool(name="CondStream1",OutputFile="trtcalibout.pool.root")
ToolSvc += TRTCondStream
print TRTCondStream

conddb.blockFolder("/TRT/Calib/RT" )
conddb.blockFolder("/TRT/Calib/T0" )

from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRTCalDbSvc
TRTCalSvc=TRTCalDbSvc(name="TRTCalDbSvc",StreamTool=TRTCondStream)
TRTCalSvc.calibTextFile = "%s"
ServiceMgr+=TRTCalSvc
print TRTCalSvc

from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTCondWrite
TRTCondWrite=TRTCondWrite( name = "TRTCondWrite",
                               CalibInputFile= "%s")

topSequence+=TRTCondWrite

outputFile = "%s"
objectList = [ "TRTCond::RtRelationMultChanContainer#/TRT/Calib/RT","TRTCond::StrawT0MultChanContainer#/TRT/Calib/T0"]
tagList = ["%s",
           "%s"]
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg(outputFile=outputFile)
myOCA.ObjectList=objectList
myOCA.IOVTagList=tagList
myOCA.WriteIOV=True

myOCA.Run1=%d
myOCA.LB1=%d
myOCA.Run2=%d
myOCA.LB2=%d

topSequence+=myOCA

""" % (IOV[0],calibconstants,calibconstants,outputfile,rttag,t0tag,IOV[0],IOV[1],IOV[2],IOV[3])
    return ostring

