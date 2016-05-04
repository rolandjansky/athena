#fileList=['/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/validation-data/valid1.105200.T1_McAtNlo_Jimmy.recon.AOD.e380_s764_r1295/AOD.134900._000001.pool.root.1']

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

rec.readAOD=True

# different input files for ATN and RTT tests
if not acf.EvtMax.is_locked():
    acf.EvtMax=2000
if not ('OutputLevel' in dir()):
    rec.OutputLevel=INFO

# find input file using a glob in case the previous test is broken
# logic must be identical to what is used in rerunAOD
#if not ('fileList' in dir()):
#added for RTT-chainstore conmpatibility
if not ('fileList' in dir()) and not ('RunningRTT' in dir()):
    #just for now take the DC14 as default input
    #
    default = "../AthenaTrigRDOtoAOD/AOD.pool.root"
    summary = "../AthenaTrigRDOtoAOD/summary.log"
    RDOtoAODokay = False
    from os.path import exists
    if exists(summary):
        fsummary = open(summary)
        if  "ATHENA_BAD_EXIT" in fsummary.read():
            print "WARNING: RDOtoAOD step bad, searching alternatives"
        else:
           if exists(default):
               print "INFO: ouptut from RDOtoAOD step exists: ",default 
               RDOtoAODokay = True
    if RDOtoAODokay:
        fileList = [default]
    else:
        print "WARNING: default AOD file doesn't exist or is bad, searching alternatives"
        import glob
        pattern="../AthenaTrig*toESDAOD*/AOD*.pool.root"
        files=glob.glob(pattern)
        if (len(files)==0):
            log.fatal(" ERROR FATAL No input AOD file could be found matching '" + pattern + "'")
            exit(-1)
        elif (len(files)>1):
            log.warning("Multiple input AOD files found, taking last one: " + files[-1])
            print "All matches to pattern '" + pattern + "': ", files
            fileList=[files[-1]]
        else:
            log.info("Using input AOD file " + files[0])
            fileList=files

#added for RTT-chainstore conmpatibility
if not ('RunningRTT' in dir()):
    acf.FilesInput=fileList
#acf.FilesInput=fileList

###############################

rec.doCBNT=False

from RecExConfig.RecFlags import rec
rec.doTrigger=True
from RecExConfig.RecAlgsFlags  import recAlgs
recAlgs.doTrigger=True
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doTriggerConfigOnly=True


rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False
rec.doDPD=False 
rec.doESD=False 
doTAG=False

#rec.doTruth=True


#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

# abort when there is an unchecked status code
StatusCodeSvc.AbortOnError=False



from TrigValAlgs.TrigValAlgsConfig import TrigDecisionChecker

TrigDecChecker = TrigDecisionChecker("TrigDecChecker")
TrigDecChecker.WriteEventDecision=False
TrigDecChecker.MonitoringBlock=100
TrigDecChecker.MonitoredChains=[]
#TrigDecChecker.OutputLevel=DEBUG

#from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
#tdt = Trig__TrigDecisionTool("TrigDecisionTool")
#ToolSvc += tdt

#from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DetGeo.set_atlas()

from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )

from TrigEDMConfig.TriggerEDM import EDMLibraries
ToolSvc.TrigDecisionTool.Navigation.Dlls = EDMLibraries

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += TrigDecChecker


include("TriggerTest/TriggerTestCommon.py")



