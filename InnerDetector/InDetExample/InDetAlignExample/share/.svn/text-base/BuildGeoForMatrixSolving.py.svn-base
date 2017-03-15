
import os

loadInDetRec_Options = {"detectorDescription" : ""
                       ,"doReadBS"            : True
		       ,"globalTag"           : ""
	               ,"siAlignmentTag"      : ""
	               ,"trtAlignmentTag"     : ""
		       ,"beamSpotTag"         : ""
		       ,"errorScalingTag"     : ""
	 	       ,"TRTCalibTextFile"    : ""
		       ,"useTRT"              : True
		       ,"inputPoolFiles"      : ""
		       }

# If the variables are defined use thier values.
# If not defined the defaults given above are used
for var in loadInDetRec_Options:
	if var in dir():
		loadInDetRec_Options[var] = eval(var)


# To extract constants from the database select the appropriate database tag.
from AthenaCommon.GlobalFlags import globalflags

if ( 'COMCOND' in loadInDetRec_Options["globalTag"]) :
	globalflags.DataSource	=	'data'
else :
	globalflags.DataSource	= 	'geant4'



if len(loadInDetRec_Options["globalTag"])!=0:
	from AthenaCommon.GlobalFlags import globalflags
	globalflags.ConditionsTag.set_Value_and_Lock(loadInDetRec_Options["globalTag"])

	
if len(loadInDetRec_Options["detectorDescription"])!=0:
	from AthenaCommon.GlobalFlags import globalflags
	globalflags.DetDescrVersion.set_Value_and_Lock(loadInDetRec_Options["detectorDescription"])


from AthenaCommon.GlobalFlags import globalflags
if len(globalflags.ConditionsTag())!=0:
	from IOVDbSvc.CondDB import conddb
	conddb.setGlobalTag(globalflags.ConditionsTag())


globalflags.print_JobProperties()



# Setup geometry
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()


from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

from InDetRecExample.InDetJobProperties import InDetFlags

#include("InDetRecExample/InDetRec_all.py")

#include ("DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )

from IOVDbSvc.CondDB import conddb

# Its normally OK to read in some database so this can be left True.
# In case you want to force it not to read in anything you can set this to False.


#if loadInDetRec_Options["inputPoolFiles"] : 
#	os.system("pool_insertFileToCatalog *.root")


if loadInDetRec_Options["siAlignmentTag"]:
  conddb.addOverride('/Indet/Align',loadInDetRec_Options["siAlignmentTag"])
if loadInDetRec_Options["trtAlignmentTag"]:
  conddb.addOverride('/TRT/Align',loadInDetRec_Options["trtAlignmentTag"])


if loadInDetRec_Options["errorScalingTag"]:
  if ".db" in loadInDetRec_Options["errorScalingTag"]:
    conddb.blockFolder("/Indet/TrkErrorScaling")
    conddb.addFolder("","<dbConnection>sqlite://;schema="+loadInDetRec_Options["errorScalingTag"]+";dbname=OFLP200</dbConnection> /Indet/TrkErrorScaling" + "<tag>IndetTrkErrorScaling_cscasbuilt_00</tag>", force=True)
  else:
    conddb.addOverride('/Indet/TrkErrorScaling',loadInDetRec_Options["errorScalingTag"])

if loadInDetRec_Options["beamSpotTag"]:
  conddb.addOverride('/Indet/Beampos',loadInDetRec_Options["beamSpotTag"])

if loadInDetRec_Options["TRTCalibTextFile"]:
  from AthenaCommon.AppMgr import ToolSvc
  from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
  TRTCalibDBSvc=TRT_CalDbSvc()
  ServiceMgr+=TRTCalibDBSvc

  conddb.blockFolder("/TRT/Calib/RT" )
  conddb.blockFolder("/TRT/Calib/T0" )
  TRTCalibDBSvc.calibTextFile = loadInDetRec_Options["TRTCalibTextFile"]



# Options for IOVRegistrationSvc
# Select the following to delete and recreate the folders. Default is
# NOT to recreate
#import RegistrationServices.IOVRegistrationSvc
#regSvc = svcMgr.IOVRegistrationSvc
#regSvc.RecreateFolders = True

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AppMgr import ToolSvc

