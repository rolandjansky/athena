
from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

if not ('fileList' in dir()):
     fileList=['/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/validation-data/valid1.105200.T1_McAtNlo_Jimmy.recon.AOD.e380_s764_r1295/AOD.134900._000001.pool.root.1']

# allow this to work in RTT
if not fileList == []:
    acf.FilesInput=fileList

rec.readAOD=True

# different input files for ATN and RTT tests
if not acf.EvtMax.is_locked():
    acf.EvtMax=2000
if not ('OutputLevel' in dir()):
    rec.OutputLevel=INFO
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

# abort when there is an unchecked status code
StatusCodeSvc.AbortOnError=False


####NOTE this still needs to updated for the DC14 menu!!! check what the TrigEDMChecker supports!!! rm L2 etc .. 
from TrigValAlgs.TrigValAlgsConfig import TrigEDMChecker
TrigEDMChecker = TrigEDMChecker("TrigEDMChecker")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += TrigEDMChecker
TrigEDMChecker.doDumpAll = True

include("TriggerTest/TriggerTestCommon.py")

