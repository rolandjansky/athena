#############################################
#
# Authors : Sam Meehan <samuel.meehan@cern.ch>   modified the original file from Geraldine
#           Geraldine Conti <geraldine.conti@cern.ch> 
#
# Example running : athena -c "OUTFILENAME='PixMapOverlay_Run2.db';RUN1=222222;LB1=0;RUN2=222345;LB2=0;" PixMapOverlay_Run2_sam.py
#
#############################################

#get the arguments from the command line
outfilename   = vars().get('OUTFILENAME', 'PixMapOverlay_Run2.db')
database      = vars().get('DATABASE', 'OFLP200')                     #OFLP200 or CONDBR2
tagname       = vars().get('TAGNAME', 'PixMapOverlay-DEFAULTTAG')
run1          = vars().get('RUN1', 0)
lb1           = vars().get('LB1', 0)
run2          = vars().get('RUN2', 2147483647)
lb2           = vars().get('LB2', 4294967295)
maskmodules   = vars().get('MASKMODULES','')
maskfrontends = vars().get('MASKFRONTENDS','')

#reformat the modules
#such that they are in the format that 
#SpecialPixelMapService wants
tempmaskmodules = maskmodules 
maskmodules = [] 
if "," in tempmaskmodules:
    for i_module in tempmaskmodules.split(","):
        maskmodules.append(int(i_module))

import logging
logger = logging.getLogger( 'PixelCalibAlgs' )

#reformat the frontends to mask
#such that they are in the format that 
#SpecialPixelMapService wants
tempmaskfrontends = maskfrontends 
maskfrontends = {}
if "|" in tempmaskfrontends or tempmaskfrontends.count(":")==1:
    for module in tempmaskfrontends.split("|"):

        logger.info("module: ",module)

        moduleID = str(module.split(":")[0].strip())

        logger.info("moduleID: ",moduleID)

        frontend_list = []    
        temp_felist   = module.split(":")[1].replace("[","").replace("]","").strip().split(",")
        logger.info(temp_felist)
        for fe in temp_felist:
            logger.info("MaskingFE: ",fe)
            frontend_list.append(int(fe))

        maskfrontends[moduleID] = frontend_list


logger.info("============================")
logger.info("USER CONFIGURABLE ARGUMENTS:")
logger.info("outfilename  : ",outfilename)
logger.info("database     : ",database)
logger.info("tagname      : ",tagname)
logger.info("run1         : ",run1       )
logger.info("lb1          : ",lb1        )
logger.info("run2         : ",run2       )
logger.info("lb2          : ",lb2       )
logger.info("maskmodules  : ",maskmodules)
logger.info("maskfrontends: ",maskfrontends)
logger.info("============================")

##############################################
#real jobOptions start here
##############################################
import AthenaCommon.AtlasUnixStandardJob

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'geant4'
globalflags.InputFormat = 'pool'
globalflags.print_JobProperties()
globalflags.DetDescrVersion = "ATLAS-R2-2015-01-01-00" 


from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.pixel_setOn()
DetFlags.Print()

from IOVDbSvc.CondDB import conddb
#Fares: change for data map:
conddb.setGlobalTag('OFLCOND-RUN12-SDR-22')#'OFLCOND-MC12-SDR-05')
#conddb.setGlobalTag('CONDBR2-BLKPA-2015-11')#'OFLCOND-MC12-SDR-05')

conddb.iovdbsvc.dbConnection = "sqlite://;schema="+outfilename+";dbname="+database

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

from PixelCalibAlgs.PixelCalibAlgsConf import PixMapOverlayWriter

job += PixMapOverlayWriter()
job.PixMapOverlayWriter.ModuleStatus = 1

from AthenaServices.AthenaServicesConf import AthenaOutputStream

job += AthenaOutputStream( "Stream1" )

from PixelConditionsServices.PixelConditionsServicesConf import SpecialPixelMapSvc

ServiceMgr += SpecialPixelMapSvc()
SpecialPixelMapSvc = ServiceMgr.SpecialPixelMapSvc

SpecialPixelMapSvc.RegisterCallback = False    
SpecialPixelMapSvc.OutputFolder = "/PIXEL/PixMapOverlay"

### configure OutputConditionsAlg

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg

OutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg","PixMapOverlay.root")

OutputConditionsAlg.ObjectList=[ "CondAttrListCollection#" + SpecialPixelMapSvc.OutputFolder ]

OutputConditionsAlg.WriteIOV=True

OutputConditionsAlg.Run1=run1
OutputConditionsAlg.LB1=lb1
OutputConditionsAlg.Run2=run2
OutputConditionsAlg.LB2=lb2

OutputConditionsAlg.IOVTagList=[tagname]


# configure IOVRegistrationSvc
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc

ServiceMgr += IOVRegistrationSvc()
regSvc = ServiceMgr.IOVRegistrationSvc

regSvc.OverrideNames = [ "ModuleSpecialPixelMap_Clob" ]
regSvc.OverrideTypes = [ "String4k" ]


### configure the event selector
from GaudiCoreSvc.GaudiCoreSvcConf import EventSelector

ServiceMgr += EventSelector()

EventSelector.RunNumber         = 0
EventSelector.EventsPerRun      = 5
EventSelector.FirstEvent        = 1
EventSelector.InitialTimeStamp  = 0
EventSelector.TimeStampInterval = 5
theApp.EvtMax                   = 1


### configure the message service
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel      = 3
MessageSvc.debugLimit       = 100000
MessageSvc.infoLimit        = 100000
MessageSvc.errorLimit       = 1000
job.PixMapOverlayWriter.ModuleIDFragments = [ ]

##############################
# Module Masking to be input as list of integers of module Hash ID's
##############################
# hardcoded example
# job.PixMapOverlayWriter.Modules = [
#     52187136
# ]
job.PixMapOverlayWriter.Modules = maskmodules

##############################
# Frontend Masking to be input as dictionary with keys and entries as
# keys = module Hash ID [string]
# entries = list of front ends [list of integers] 0-15
##############################
# hardcoded example
# job.PixMapOverlayWriter.Frontends = {
#     "21254144": [8]}
job.PixMapOverlayWriter.Frontends = maskfrontends



