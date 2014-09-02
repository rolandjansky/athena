#Transform skeleton for vertex-based beamspot method

#define variables at the top
from AthenaCommon.Logging import logging
recoLog = logging.getLogger('beamspot Vertex determination')
recoLog.info( '****************** STARTING Beamspot Determination  MAKING *****************' )

#For beamspot InDetExample
jobConfig = {}

if not 'runArgs' in dir():
    from PyJobTransformsCore.runargs import RunArguments
    runArgs = RunArguments()

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
import AthenaPoolCnvSvc.ReadAthenaPool
print runArgs

if hasattr(runArgs,"InputDESDM_BEAMSPOTFile"):
    #athenaCommonFlags.PoolDPDInput.set_Value_and_Lock( runArgs.inputDPDFile )
    #ServiceMgr.EventSelector.InputCollections = runArgs.inputDPDFile
    athenaCommonFlags.FilesInput=runArgs.InputDESDM_BEAMSPOTFile  #  for filePeeking
    jobConfig['inputfiles'] = runArgs.InputDESDM_BEAMSPOTFile
    
if hasattr(runArgs,"beamspotFile"):
    jobConfig['histfile'] = runArgs.beamspotFile
else:
    jobConfig['histfile']  = "beamspot.root"
   
if hasattr(runArgs,"monitoringFile"):
    jobConfig['monfile'] = runArgs.monitoringFile
else:
    jobConfig['monfile']  = "monitoring.root"

if hasattr(runArgs,"beamspotDb"):
    jobConfig['outputfile'] = runArgs.beamspotDb
else:
    jobConfig['outputfile'] = "beamspot.db"


if hasattr(runArgs,"maxEvents"):
    #One of these must work ... ?
    athenaCommonFlags.EvtMax.set_Value_and_Lock( runArgs.maxEvents )
    theApp.EvtMax = runArgs.maxEvents
    jobConfig['evtmax'] = runArgs.maxEvents
else:
    theApp.EvtMax = -1

# Extra inputs from the command line for the jobConfig settings
if hasattr(runArgs,"jobConfig"):
    if len(runArgs.jobConfig) > 0:
        print "JobConfig Modifications"
        print runArgs.jobConfig
        for i in runArgs.jobConfig:
          exec(i)
          
# Filepeeker to set up truth (and more in future)
from RecExConfig.InputFilePeeker import inputFileSummary 
isMC = False if inputFileSummary['evt_type'][0] == 'IS_DATA'  else True

jobConfig['autoconfparams'] = 'DetDescrVersion'

if isMC:
    jobConfig["DataSource"]="geant4"
    jobConfig["doTruth"]=True

if hasattr(runArgs,"topOptions"):
    if len(runArgs.topOptions) >0 :
        print "Including: JobOption", runArgs.topOptions
        for i in runArgs.topOptions:
            include(i)
    else:
        print "No jobOption specified"
else:
    print "No jobOption specified"


#Run any last options
if hasattr(runArgs,"postExec"):
    if len(runArgs.postExec) > 0:
        print "PostExec Modifications"
        print runArgs.postExec
        for i in runArgs.postExec:
            exec(i)
                                      

print "JobConfig at end of BeamSpotVertex.py"
print jobConfig

