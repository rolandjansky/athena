###############################################################
#
# Skeleton top job options for Physics Validation
#
#==============================================================


# ==============================================================================
# Load your input file that you want to process
# ==============================================================================
include("RecJobTransforms/CommonRecoSkeletonJobOptions.py")

from AthenaCommon.Logging import logging
skelLog = logging.getLogger('PhysicsValidaiton')
skelLog.info( '****************** Starting Physics Validation *****************' )

from RecExConfig.RecFlags import rec
rec.readESD = False
rec.readAOD = False
rec.readRDO = False
rec.doDPD = False
rec.doHist = False
rec.doMonitoring = False
rec.doWriteAOD = False
rec.doWriteTAG = False

# Setup monitoring framework once
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
monMan = AthenaMonManager( "PhysValMonManager" )
monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "altprod"
monMan.ManualRunLBSetup    = True
monMan.Run                 = 1
monMan.LumiBlock           = 1

# Only for now due to xAOD issues
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += monMan

from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
svcMgr.THistSvc.Output += ["PhysVal DATAFILE='" + runArgs.outputNTUP_PHYSVALFile + "' OPT='RECREATE'"]
monMan.FileKey = "PhysVal" 



if hasattr(runArgs,"inputESDFile"):
    rec.readESD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolESDInput.set_Value_and_Lock( runArgs.inputESDFile )
    rec.readESD = True
    rec.readAOD = False
elif hasattr(runArgs,"inputAODFile"):
    rec.readAOD.set_Value_and_Lock( True )
    athenaCommonFlags.PoolAODInput.set_Value_and_Lock( runArgs.inputAODFile )
    rec.readESD = False
    rec.readAOD = True
else:
    raise RuntimeError('No input file argument given (ESD or AOD input required)')

# Validation dictionary with default run setting:
validationDict = {
                  'Btag': False,
                  'Egamma': False,
                  'Electron': False,
                  'Example': True,
                  'Exotics': False,
                  'HSG6': False,
                  'InDet': False,
                  'Jet': False,
                  'MET': False,
                  'Muon': False,
                  'PFlow': False,
                  'Photon': False,
                  'PrimaryTracking': False,
                  'SMZMet': False,
                  'SMZee': False,
                  'SUSY': False,
                  'SecondaryTracking': False,
                  'Tau': False,
                  'Top': False,
                  }

# Switch on/off various validation routines:
if hasattr(runArgs,"validationFlags"):
    for flag in runArgs.validationFlags:
        onoff = flag[:2]
        validationType = flag[2:]
        # Note that we can add new validation types on the fly 
        if onoff == 'do':
            validationDict[validationType] = True
        elif onoff == 'no':
            validationDict[validationType] = False
        else:
            skelLog.warning("Ignored unrecognised validation control string for {0}: {1}".format(validationType, flag))
            
skelLog.info("Validation switches are set to: {0}".format(validationDict))

# Schedule individual validations
from PyJobTransforms.trfUtils import findFile
for validationType, enabled in validationDict.iteritems():
    if enabled:
        JOFile = 'PhysValMonitoring/PhysVal{0}_jobOptions.py'.format(validationType)
        if findFile(os.environ['JOBOPTSEARCHPATH'], JOFile):
            rec.UserAlgs += [JOFile]
        else:
            skelLog.warning("Job options file for {0} ({1}) was not found in JOBOPTSEARCHPATH!".format(validationType, JOFile))

## Pre-exec
if hasattr(runArgs,"preExec"):
    skelLog.info("transform pre-exec")
    for cmd in runArgs.preExec:
        skelLog.info(cmd)
        exec(cmd)

## Pre-include
if hasattr(runArgs,"preInclude"): 
    for fragment in runArgs.preInclude:
        include(fragment)
    
# Now, include the master top options from RecExCommon.
include ("RecExCommon/RecExCommon_topOptions.py")


## Post-include
if hasattr(runArgs,"postInclude"): 
    for fragment in runArgs.postInclude:
        include(fragment)

## Post-exec
if hasattr(runArgs,"postExec"):
    skelLog.info("transform post-exec")
    for cmd in runArgs.postExec:
        skelLog.info(cmd)
        exec(cmd)


