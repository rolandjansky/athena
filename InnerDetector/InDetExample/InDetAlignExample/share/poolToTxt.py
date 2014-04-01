###############################################################
#
# Job options file to convert between text file and POOL file.
#
# For common use cases set the variable config to one of the following.
#   'TxtToPool'
#   'PoolToTxt'
#   'PoolToPool'
#   'DBToTxt'
#   'DBToPool'
#
# Note that the options which write out a pool file also write an sqlite file.
#
# You can also leave config unset and set the following
#
# readTextFile : If True will read alignments from a text file (textFileInSi/TRT)
#                If False alignments will be those read from the conditions
#                database (or a pool file if readPool is True) 
# writeTextFile: If True will write alignments to a text file (textFileOutSi/TRT)
# readPool:      If True will read alignments from pool file specified in
#                inputCollections. NB, readTextFile should be set to False.             
# writeDB:       If True will write sqlite file and pool file. The name
#                of the pool file is given by outputFile.
#
# Default settings are the same as PoolToTxt.
#
#==============================================================

#config = 'TxtToPool'
config = 'PoolToTxt'
#config = 'PoolToPool'
#config = 'DBToTxt'
#config = 'DBToPool'

inputCollections = ["testInputFile/alignment_out_RandomStrawMisalignments.root"]
outputFile = "alignment_out.root"
tagSi  = "IndetAlign_test"
tagTRT = "TRTAlign_test"

textFileInSi  = "alignmentSi_in.txt"
textFileOutSi = "alignmentSi_out.txt"
textFileInTRT  = "NoTRTModuleAlignment.txt"
textFileInTRTStraw  = "CreatedStrawAlignments2.txt"
textFileOutTRT = "alignmentTRT_out.txt"
textFileOutTRTStraw = "alignmentTRTStraw_out.txt"

doSilicon = False
doTRT = True

doPrintDectectorPositions = True

#==============================================================

# These will be overwritten if config is set
readTextFile = False
writeTextFile = True
readPool  = True
writeDB   = False

# To extract constants from the database select the appropriate database tag.
from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource='geant4'
#globalflags.DataSource='data'

from IOVDbSvc.CondDB import conddb
#conddb.setGlobalTag("OFLCOND-CSC-00-00-00")
conddb.setGlobalTag("OFLCOND-SIM-BS7T-00")
#conddb.setGlobalTag("COMCOND-HLTC-000-00")
#conddb.addOverride('/Indet/Align','InDetSi_CSCMisaligned_RDeltaPhi_03')
#conddb.addOverride('/TRT/Align','InDetTRT_CSCMisaligned_RDeltaPhi_03')

# To override the run number set this variable. Only has an affect for
# DBToTxt and DBToPool options.
#runNumber = 12345

#==============================================================

# You shouldn't need to change anything below here.

if ('config' not in dir()):
    config = ''
if (config == 'PoolToTxt') :
    readTextFile = False
    writeTextFile = True
    readPool  = True
    writeDB   = False
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

# The geometry version does not really matter. Any will do.
globalflags.DetDescrVersion = 'ATLAS-GEO-03-00-00'

from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion


from IOVDbSvc.CondDB import conddb

# Its normally OK to read in some database so this can be left True.
# In case you want to force it not to read in anything you can set this to False.
readDB = True
if not readDB :
    conddb.blockFolder("/Indet/Align")
    conddb.blockFolder("/TRT/Align")

if readPool :
    conddb.blockFolder("/Indet/Align")
    conddb.blockFolder("/TRT/Align")
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
    TRTStrawAlign.StrawAlignmentTextOutputFile = textFileOutTRTStraw

    if readTextFile :
        TRTStrawAlign.InputFile=textFileInTRT
        TRTStrawAlign.InputStrawAlignmentFile = textFileInTRTStraw
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
        objectList += [ "TRTCond::StrawDxContainer#/TRT/Calib/DX"]
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
# athena -l DEBUG

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Specify run number
if ('runNumber' in dir()):
    import AthenaCommon.AtlasUnixGeneratorJob
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    svcMgr.EventSelector.RunNumber         = runNumber
    #svcMgr.EventSelector.FirstEvent        = 0
    #svcMgr.EventSelector.FirstLB           = 0


# Number of events to be processed (default is 10)
theApp.EvtMax = 1

if doPrintDectectorPositions:
    include("InDetSimpleVisual/GetDetectorPositions.py")
    job.GetDetectorPositions.DetailLevel = 2

#==============================================================
#
# End of job options file
#
###############################################################
