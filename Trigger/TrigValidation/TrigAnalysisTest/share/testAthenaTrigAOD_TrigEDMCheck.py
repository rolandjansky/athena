
#PoolAODInput=['/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/data130003002/AOD.015446._00012.pool.root.2']

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

rec.readAOD=True

# different input files for ATN and RTT tests
if not acf.EvtMax.is_locked():
    acf.EvtMax=10
if not ('OutputLevel' in dir()):
    rec.OutputLevel=INFO

# look for backup files if RDOtoAOD fails
#if not ('fileList' in dir()):
#added for RTT-chainstore conmpatibility
if not ('fileList' in dir()) and not ('RunningRTT' in dir()):
    #default = "../AthenaTrigRDOtoAOD/AOD.pool.root"
    #summary = "../AthenaTrigRDOtoAOD/summary.log"
    #just for now take the DC14 as default input
    #
    default = "../AthenaTrigRDOtoAOD/AOD.pool.root"
    summary = "../AthenaTrigRDOtoAOD/summary.log"
    RDOtoAODokay = False
    from os.path import exists
    if exists(summary):
        fsummary = open(summary)
        if  "ATHENA_BAD_EXIT" in fsummary.read():
            printfunc ("WARNING: RDOtoAOD step bad, searching alternatives")
        else:
           if exists(default):
               printfunc ("INFO: ouptut from RDOtoAOD step exists: ",default )
               RDOtoAODokay = True
    if RDOtoAODokay:
        fileList = [default]
    else:
        printfunc ("WARNING: default AOD file doesn't exist or is bad, searching alternatives")
        import glob
        pattern="../AthenaTrig*toESDAOD*/AOD*.pool.root"
        files=glob.glob(pattern)
        if (len(files)==0):
            log.fatal(" ERROR FATAL No input AOD file could be found matching '" + pattern + "'")
            exit(-1)
        elif (len(files)>1):
            log.warning("Multiple input AOD files found, taking last one: " + files[-1])
            printfunc ("All matches to pattern '" + pattern + "': ", files)
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


#doTrigger=False
rec.doTrigger=False
from RecExConfig.RecAlgsFlags  import recAlgs
recAlgs.doTrigger=False


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


####NOTE this still needs to updated for the DC14 menu!!! check what the TrigEDMChecker supports!!! rm L2 etc .. 
from TrigValAlgs.TrigValAlgsConfig import TrigEDMChecker
TrigEDMChecker = TrigEDMChecker("TrigEDMChecker")

from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )

from TrigEDMConfig.TriggerEDM import EDMLibraries
ToolSvc.TrigDecisionTool.Navigation.Dlls = EDMLibraries

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += TrigEDMChecker
TrigEDMChecker.doDumpAll = False



include("TriggerTest/TriggerTestCommon.py")


# get the name auditor
#theAuditorSvc = AuditorSvc()
#theAuditorSvc.Auditors  += [ "NameAuditor"]


