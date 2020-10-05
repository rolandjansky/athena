###########################################################################
#
# TopOptions to copy a complete set of conditions for a given run to 
# one POOL file and one sqlite file
#
# Walter.Lampl@cern.ch
#
###########################################################################

import AthenaCommon.AtlasUnixGeneratorJob #use MC event selector

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

include ("LArCalibProcessing/LArCalib_Flags.py")

if not 'RunNumber' in dir():
  RunNumber=76702

if not 'OutputFile' in dir():
  OutputFile="LArConditions_"+str(RunNumber)+".pool.root"


WriteIOV=True


from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('commis')
globalflags.Luminosity.set_Value_and_Lock('zero')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock('bytestream')

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-GEO-03-00-00"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()

#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

#Get identifier mapping (needed by LArConditionsContainer)
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )

theApp.EvtMax = 1
svcMgr.EventSelector.RunNumber = RunNumber


# Add input folders:
from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.OFCShapeFolder="5samples"
include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py" )
conddb.setGlobalTag("COMCOND-ES1C-001-00")
# Output: 

outObjects= ["LArPedestalComplete#Pedestal#/LAR/ElecCalibOfl/Pedestals/Pedestal",
             "LArRampComplete#LArRamp#/LAR/ElecCalibOfl/Ramps/RampLinea",
	     "LArMphysOverMcalComplete#LArMphysOverMcal#/LAR/ElecCalibOfl/MphysOverMcal/RTM"
	     "LAruA2MeVComplete#LAruA2MeV#/LAR/ElecCalibOfl/uA2MeV/Symmetry",
	     "LArDAC2uAMC#LArDAC2uA#/LAR/ElecCalibOfl/DAC2uA/Symmetry",
             "LArOFCComplete#LArOFC#/LAR/ElecCalibOfl/OFC/PhysWave/RTM/"+larCondFlags.OFCShapeFolder() ]

outTags=[]
for obj in outObjects:
    folder=obj[obj.rfind('#')+1:]
    print folder
    tag=LArCalibFolderTag(folder)
    outTags+=[tag]


from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
theOutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg",OutputFile,
					   outObjects,outTags,WriteIOV)

theOutputConditionsAlg.Run1 = RunNumber
theOutputConditionsAlg.Run2 = RunNumber

svcMgr.IOVDbSvc.dbConnection  = LArCalib_Flags.OutputDB
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = True

svcMgr.DetectorStore.Dump=True
