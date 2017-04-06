###############################################################
#
# Job options file to convert between text file and POOL file.
# Specially configured and prepared to translate from the new to the old DB
#
#==============================================================

config = 'TxtToPool'

if 'FileInSi' in dir():
    textFileInSi = FileInSi
else:
    textFileInSi = "SiInFile.txt"

if 'FileInTRT' in dir():
    textFileInTRT  = FileInTRT
else:
    textFileInTRT = "TRTInFile.txt"

#-- input/output root files
outputFile     = "OldFolderScheme.root"
tagSi          = "InDetAlign_test"
tagTRT         = "TRTAlign_test"

#-- text files (input and output)
textFileOutSi  = "SiOutFile.txt"
textFileOutTRT =      "TRTOutFile.txt"             

doSilicon      = True
doTRT          = True

print " <poolToTxt> config mode = ", config
print " <poolToTxt> doSilicon   = ", doSilicon
print " <poolToTxt> doTRT       = ", doTRT

if (doSilicon):
    if (config == 'TxtToPool'):
        if os.path.isfile(textFileInSi):
            print " <poolToTxt> textFileInSi = ", textFileInSi, "   output = ",textFileOutSi
        else:
            print " <poolToTxt> ** ERROR ** input file:", textFileInSi, " does not exist "
if (doTRT):
    if os.path.isfile(textFileInTRT):
        print " <poolToTxt> textFileInTRT = ", textFileInTRT, "   output = ",textFileOutTRT
    else:
        print " <poolToTxt> ** ERROR ** input file:", textFileInTRT, " does not exist "
    
#==============================================================
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.useDynamicAlignFolders.set_Value_and_Lock             (False)   # We want the old structure loaded!!!

# These will be overwritten if config is set
readTextFile  = True
writeTextFile = False
readPool      = False
writeDB       = True

# To extract constants from the database select the appropriate database tag.
from AthenaCommon.GlobalFlags import globalflags
#globalflags.DataSource='geant4'   #geant4 for MC
globalflags.DataSource.set_Value_and_Lock('data')
#from RecExConfig.RecFlags import rec
#rec.projectName.set_Value_and_Lock('data15_cos')


from IOVDbSvc.CondDB import conddb
#conddb.setGlobalTag("OFLCOND-RUN12-SDR-28") # latest and greates
conddb.setGlobalTag("CONDBR2-BLKPA-2016-16") # latest and greates

# The geometry version does not really matter. Any will do.
#conddb.addOverride('/Indet/Align','InDetAlign_R2_Initial_fixed')
#conddb.addOverride('/TRT/Align','TRTAlign-RUN2-BLK-UPD4-09')

globalflags.DetDescrVersion = 'ATLAS-R2-2015-03-01-00'

# To override the run number set this variable. Only has an affect for
# DBToTxt and DBToPool options.
runNumber = 280806

#==============================================================

# You shouldn't need to change anything below here.

if ('config' not in dir()):
    config = ''
if (config == 'PoolToTxt') :
    readTextFile = False
    writeTextFile = True
    readPool  = True
    writeDB   = True
if (config == 'TxtToPool') :
    readTextFile = True
    writeTextFile = False
    readPool  = False
    writeDB   = True
if (config == 'PoolToPool') :
    readTextFile = False
    writeTextFile = False
    readPool  = True
    writeDB   = True
if (config == 'DBToTxt') :
    readTextFile = False
    writeTextFile = True
    readPool  = False
    writeDB   = False
if (config == 'DBToPool') :
    readTextFile = False
    writeTextFile = False
    readPool  = False
    writeDB   = True


# Setup geometry
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()

from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion

# Its normally OK to read in some database so this can be left True.
# In case you want to force it not to read in anything you can set this to False.
readDB = True
if not readDB :
    conddb.blockFolder("/Indet/Align")
    conddb.blockFolder("/TRT/Align")
#    conddb.blockFolder("/TRT/Calib/DX")

if readPool :
    conddb.blockFolder("/Indet/Align")
    conddb.blockFolder("/TRT/Align")
#    conddb.blockFolder("/TRT/Calib/DX")
    from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr += CondProxyProvider()
    ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
    # set this to the file containing AlignableTransform objects
    ServiceMgr.CondProxyProvider.InputCollections += inputCollections
    ServiceMgr.CondProxyProvider.OutputLevel=DEBUG
    print ServiceMgr.CondProxyProvider

    # this preload causes callbacks for read in objects to be activated,
    # allowing GeoModel to pick up the transforms
    ServiceMgr.IOVSvc.preLoadData=True
    ServiceMgr.IOVSvc.OutputLevel=DEBUG

# Options for IOVRegistrationSvc
# Select the following to delete and recreate the folders. Default is
# NOT to recreate
#import RegistrationServices.IOVRegistrationSvc
#regSvc = svcMgr.IOVRegistrationSvc
#regSvc.RecreateFolders = True

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AppMgr import ToolSvc

if doSilicon:
    from InDetAlignGenAlgs.InDetAlignGenAlgsConf import InDetAlignWrt
    from InDetAlignGenTools.InDetAlignGenToolsConf import InDetAlignDBTool

    InDetDBTool = InDetAlignDBTool()
    ToolSvc += InDetDBTool

    InDetAlignWrt = InDetAlignWrt()

    if readDB or readPool :
        InDetAlignWrt.Create=False
    else :
        InDetAlignWrt.Create=True
    
    # We use OutputConditionsAlg to write pool file and register IOV so set to False.
    InDetAlignWrt.Write=False 
    InDetAlignWrt.WriteIOV=False
    if writeTextFile :
        InDetAlignWrt.Write=True
        InDetAlignWrt.WFile=textFileOutSi
    if readTextFile :
        InDetAlignWrt.RFile=textFileInSi
    # .DispDet=0: none, otherwise creates some random misalignments
    InDetAlignWrt.DispDet=0

    topSequence += InDetAlignWrt

    print InDetAlignWrt
    print InDetDBTool

if doTRT:
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTStrawAlign
    TRTStrawAlign = TRTStrawAlign()
    # We use OutputConditionsAlg to write pool file and register IOV so set to False.
    TRTStrawAlign.Write=False
    TRTStrawAlign.RegisterIOV=False
    TRTStrawAlign.DoModuleAlign=True
    TRTStrawAlign.DoStrawAlign=True
    #if writeTextFile :
    TRTStrawAlign.TextOutputFile=textFileOutTRT
    #TRTStrawAlign.StrawAlignmentTextOutputFile = textFileOutTRTStraw

    if readTextFile :
        TRTStrawAlign.InputFile=textFileInTRT
        #TRTStrawAlign.InputStrawAlignmentFile = textFileInTRTStraw
    topSequence+=TRTStrawAlign
    print TRTStrawAlign

if writeDB:
    objectList = []
    tagList = []
    if doSilicon:
        objectList += [ "AlignableTransformContainer#/Indet/Align"]
        tagList += [tagSi]
    if doTRT:
        objectList += [ "AlignableTransformContainer#/TRT/Align"]
 #       objectList += [ "TRTCond::StrawDxContainer#/TRT/Calib/DX"]
        tagList += [tagTRT]
                            
    ## These lines can be excluded and it will by default connect to SQlite file mycool.db    
    ## If you want a different sqlite db file name or db name edit as appropriate
    #from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    #svcMgr.IOVDbSvc.dbConnection="sqlite://;schema=mycool.db;dbname=OFLP200"

    from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
    myOCA=OutputConditionsAlg(outputFile=outputFile)
    myOCA.ObjectList=objectList
    myOCA.IOVTagList=tagList

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
# To set global output level use
#athena -l DEBUG

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Specify run number
import AthenaCommon.AtlasUnixGeneratorJob
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.EventSelector.RunNumber         = runNumber

# Number of events to be processed (default is 10)
theApp.EvtMax = 1

#==============================================================
#
# End of job options file
#
###############################################################


# Disable StatusCodeSvc 
from AthenaCommon.AppMgr import ServiceMgr as svcMgr 
svcMgr.StatusCodeSvc.SuppressCheck = True
svcMgr.StatusCodeSvc.AbortOnError = False
