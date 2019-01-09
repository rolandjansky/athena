import logging
log = logging.getLogger( "testAthenaTrigAOD_TrigEgammaMonitoring.py")
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)
from glob import glob

if ('FILE' in dir()):
    #athenaCommonFlags.PoolAODInput.set_Value_and_Lock( runArgs.inputAODFile )
    acf.PoolAODInput=[FILE]
elif ('inputDir' in dir()):
    inputFiles = glob(inputDir+'*')
    acf.PoolAODInput=inputFiles
elif 'RTT' in dir():
    rttfile='root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/rtt/prod/rtt/'+RTT+'/x86_64-slc6-gcc49-opt/offline/TrigEgammaValidation/RDOtoAOD_MC_transform_Zee_25ns_pileup/AOD.Zee.25ns.pileup.pool.root'
    acf.PoolAODInput=[rttfile]
if ('NOV' in dir()):
    log.info('Setting Max events to ' +  str(NOV))
    acf.EvtMax=NOV
    acf.EvtMax.set_Value_and_Lock(NOV)
    theApp.EvtMax = NOV
else:
    acf.EvtMax=-1

#if not acf.EvtMax.is_locked():
#    acf.EvtMax=-1

from RecExConfig.RecFlags import rec

rec.doCBNT=False
rec.readESD=False
rec.doWriteESD=False 
rec.doWriteAOD=False 
rec.doAOD=False
rec.doDPD=False 
rec.doWriteTAG=False
rec.doInDet=False
rec.readAOD=True


from RecExConfig.AutoConfiguration import GetProjectName,GetRunNumber,ConfigureTriggerStream,ConfigureSimulationOrRealData,ConfigureBeamType

from AthenaCommon.BeamFlags import jobproperties 
runNb=GetRunNumber() 
projectName=GetProjectName() 
ConfigureTriggerStream() 
ConfigureSimulationOrRealData() 
ConfigureBeamType()

data_type=globalflags.InputFormat.get_Value()
beam_type=jobproperties.Beam.beamType.get_Value()
log.info("Run Number %s",runNb)
log.info("Project name %s",projectName)
log.info("File type %s",data_type)
log.info("Beam type %s",beam_type)
log.info("Trigger stream %s",rec.triggerStream())

from AthenaMonitoring.DQMonFlags import DQMonFlags
if globalflags.DataSource.get_Value() == 'geant4':
    DQMonFlags.monManDataType = 'monteCarlo'
elif (rec.doHeavyIon() or rec.doHIP()):
    DQMonFlags.monManDataType = 'heavyioncollisions'
    DQMonFlags.doHIMon = True
elif jobproperties.Beam.beamType()   == 'cosmics':
    DQMonFlags.monManDataType = 'cosmics'
elif jobproperties.Beam.beamType() == 'collisions':
    DQMonFlags.monManDataType = 'collisions'
elif jobproperties.Beam.beamType() == 'singlebeam':
    DQMonFlags.monManDataType = 'collisions'
else:
    log.warning("invalid jobproperties.Beam.beamType(): %s, using default (%s)",             
            jobproperties.Beam.beamType(), DQMonFlags.monManDataType)
DQMonFlags.monManEnvironment = 'tier0'
DQMonFlags.monManStream=rec.triggerStream()
if ('histogramFile' in dir()):
    DQMonFlags.histogramFile = histogramFile
    log.info('Setting output file to %s' %DQMonFlags.histogramFile)

log.info('DQMonFlags %s',DQMonFlags) 
log.info('RecFlags %s', rec)

if not 'HLTMonFlags' in dir():
    from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags

algseq = CfgMgr.AthSequencer("AthAlgSeq")
import AthenaPoolCnvSvc.ReadAthenaPool 

svcMgr.EventSelector.InputCollections=acf.PoolAODInput()

from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool     
from AthenaCommon.AppMgr import ToolSvc     
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )     
from TrigEDMConfig.TriggerEDM import EDMLibraries     
ToolSvc.TrigDecisionTool.Navigation.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]
ToolSvc.TrigDecisionTool.Navigation.ReadonlyHolders=True


from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
algseq += CfgMgr.AthenaMonManager( "HLTMonManager")
HLTMonManager = algseq.HLTMonManager

from TrigEgammaMonitoring.TrigEgammaMonitoringConfig import TrigEgammaMonitoringTool
if ('derivation' in dir()):
    HLTMonManager.AthenaMonTools += TrigEgammaMonitoringTool(derivation=True)
elif ('emulation' in dir()):
    HLTMonManager.AthenaMonTools += TrigEgammaMonitoringTool(emulation=True)
else:
    HLTMonManager.AthenaMonTools += TrigEgammaMonitoringTool()

from GaudiSvc.GaudiSvcConf import THistSvc 
svcMgr += THistSvc() 
svcMgr.THistSvc.Output += ["GLOBAL DATAFILE='" + DQMonFlags.histogramFile() + "' OPT='RECREATE'"] 
HLTMonManager.FileKey = "GLOBAL"

if 'outputLevel' in dir():
    for tool in ToolSvc.getAllChildren():         
        # Set DEBUG output level for ALL HLTEgamma* tools
        Tool_Name = tool.getName()
        if 'HLTEgamma' in Tool_Name or 'Isolation' in Tool_Name or 'Track' in Tool_Name:
            log.info('Setting tool %s.OutputLevel=%s'%(Tool_Name,str(outputLevel)))
            tool.OutputLevel = outputLevel
        tool_prop = tool.getDefaultProperties()         
        for prop,value in tool_prop.iteritems():             
            if prop == "TrigConfigTool":                 
                log.info("Set TrigConfigTool %s",tool.getName())                 
                tool.TrigConfigTool="TrigConf::xAODConfigTool"

# main jobOption
#include ("RecExCommon/RecExCommon_topOptions.py")
#
#if ('derivation' in dir()):
    
MessageSvc.debugLimit = 20000000
