# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#-- set flags ------------------------------------------------------------------------------
log = logging.getLogger( "TrigHLTMonCommon_jobOptions.py" )

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

from RecExConfig.RecFlags import rec
from RecExConfig.AutoConfiguration import GetProjectName,GetRunNumber,ConfigureTriggerStream,ConfigureSimulationOrRealData,ConfigureBeamType,ConfigureFieldAndGeo,ConfigureConditionsTag

from AthenaCommon.BeamFlags import jobproperties
runNb=GetRunNumber()
projectName=GetProjectName()
rec.projectName=projectName
ConfigureFieldAndGeo()
ConfigureTriggerStream()
ConfigureSimulationOrRealData()
ConfigureBeamType()
ConfigureConditionsTag()

data_type=globalflags.InputFormat.get_Value()
beam_type=jobproperties.Beam.beamType.get_Value()
#evt_type=inputFileSummary['evt_type']
log.info("Run Number %s",runNb)
log.info("Project name %s",projectName)
log.info("DetDescription %s", globalflags.DetDescrVersion())
log.info("File type %s",data_type)
log.info("Beam type %s",beam_type)
log.info("Trigger stream %s",rec.triggerStream())
#log.info("Event type %s",evt_type)

# enable PerfMon
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doFastMon = True

#set up DQMonFlags required for HLTMonitoring
from AthenaMonitoring.DQMonFlags import DQMonFlags
if globalflags.DataSource.get_Value() == 'geant4':
    DQMonFlags.monManDataType = 'monteCarlo'
elif (rec.doHeavyIon() or rec.doHIP()):
    # requires something from InputFileSummary rather than rec flag
    DQMonFlags.monManDataType = 'heavyioncollisions'
    DQMonFlags.doHIMon = True
elif jobproperties.Beam.beamType()   == 'cosmics':
    DQMonFlags.monManDataType = 'cosmics'
elif jobproperties.Beam.beamType() == 'collisions':
    DQMonFlags.monManDataType = 'collisions'
elif jobproperties.Beam.beamType() == 'singlebeam':
    ## There is no singlebeam in AthenaMonitoring
    DQMonFlags.monManDataType = 'collisions'
else:
    log.warning("invalid jobproperties.Beam.beamType(): %s, using default (%s)", 
            jobproperties.Beam.beamType(), DQMonFlags.monManDataType)
                             
if data_type == 'bytestream':
    DQMonFlags.monManEnvironment = 'tier0Raw'
if data_type == 'pool':
    DQMonFlags.monManEnvironment = 'tier0'

DQMonFlags.monManStream=rec.triggerStream()
DQMonFlags.monManRun=runNb

log.info('DQMonFlags %s',DQMonFlags)
log.info('RecFlags %s', rec)

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

if not 'HLTMonFlags' in dir():
    from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags

if HLTMonFlags.doBphys:
    #extra parts of the geometry needed for the extrapolation
    include("RecExCond/RecExCommon_DetFlags.py")
    DetFlags.detdescr.Forward_setOff()   #prevent scheduling of alfa conditions 
    include("RecExCond/AllDet_detDescr.py")
    
#-- set up job ------------------------------------------------------------------------------
if data_type == 'bytestream':
    log.info('BS to HIST Monitoring')
    
    # Only doGeneral monitoring from RAW
    HLTMonFlags.set_All_Off()
    HLTMonFlags.doGeneral.set_Value_and_Lock(True)
    #-- set up BS reading ------------------------------------------------------------------------------

    from ByteStreamCnvSvc import ReadByteStream
    svcMgr.ByteStreamInputSvc.FullFileName = athenaCommonFlags.BSRDOInput()
    if hasattr(runArgs,"skipEvents"):
        svcMgr.EventSelector.SkipEvents=runArgs.skipEvents

    if hasattr(runArgs,"maxEvents"):
            theApp.EvtMax=runArgs.maxEvents
    #-- get Trigger Configuration ------------------------------------------------------------------------------

    from TriggerJobOpts.T0TriggerGetter import T0TriggerGetter
    gettrigger = T0TriggerGetter()
    
    from TriggerJobOpts.HLTTriggerResultGetter import ByteStreamUnpackGetter
    bsu=ByteStreamUnpackGetter()

    theByteStreamAddressProviderSvc = svcMgr.ByteStreamAddressProviderSvc
    theByteStreamAddressProviderSvc.TypeNames += ['HLT::HLTResult/HLTResult_HLT',
        "ROIB::RoIBResult/RoIBResult",
        "MuCTPI_RDO/MUCTPI_RDO",
        "CTP_RDO/CTP_RDO",
        ]

if data_type == 'pool':
    log.info('AOD to HIST Monitoring')
    import AthenaPoolCnvSvc.ReadAthenaPool 
    svcMgr.EventSelector.InputCollections=athenaCommonFlags.PoolAODInput()
    
    if hasattr(runArgs,"skipEvents"):
        svcMgr.EventSelector.SkipEvents=runArgs.skipEvents

    if hasattr(runArgs,"maxEvents"):
            theApp.EvtMax=runArgs.maxEvents
    
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
    from TrigEDMConfig.TriggerEDM import EDMLibraries
    ToolSvc.TrigDecisionTool.Navigation.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]
    
    if hasattr(runArgs,"useDB"):
        if runArgs.useDB:
            from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
            cfg =  TriggerConfigGetter()
            ToolSvc.TrigDecisionTool.TrigConfigSvc = "Trig::TrigConfigSvc/TrigConfigSvc"
    else:
        log.info("Configure TrigConfigSvc by default")
        from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
        cfg =  TriggerConfigGetter()
        ToolSvc.TrigDecisionTool.TrigConfigSvc = "Trig::TrigConfigSvc/TrigConfigSvc"
    
    # enable slices for monitoring 
    # otherwise enable slices via monFlags 
    HLTMonFlags.doMonTier0 = True
    if not hasattr(runArgs,"monFlags"):
        HLTMonFlags.doGeneral = True
        HLTMonFlags.doEgamma  = True
        HLTMonFlags.doMET     = True
        HLTMonFlags.doJet     = True
        HLTMonFlags.doBjet    = True
        HLTMonFlags.doTau     = True
        HLTMonFlags.doMuon    = True
        HLTMonFlags.doIDtrk   = True
        HLTMonFlags.doIDJpsiMon = True
        HLTMonFlags.doCalo    = True
        HLTMonFlags.doBphys   = False
        HLTMonFlags.doMinBias = False
   
    #-- setup DQTDataFlow ------------------------------------------------------------------------------
    from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
    topSequence += AthenaMonManager( "GlobalMonManager" )
    ManagedAthenaGlobalMon = topSequence.GlobalMonManager
    ManagedAthenaGlobalMon.FileKey             = "GLOBAL"
    ManagedAthenaGlobalMon.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    ManagedAthenaGlobalMon.DataType            = DQMonFlags.monManDataType()
    ManagedAthenaGlobalMon.Environment         = DQMonFlags.monManEnvironment()

    from DataQualityTools.DataQualityToolsConf import DQTDataFlowMonTool
    import os
    if 'AtlasProject' in os.environ and 'AtlasVersion' in os.environ:
        releaseString = '%s-%s' % (os.environ['AtlasProject'],
                                    os.environ['AtlasVersion'])
    else:
        releaseString = 'Unknown'

    DQTDataFlowMon = DQTDataFlowMonTool(name = 'DQTDataFlowMon', 
            histoPathBase = '/GLOBAL/DQTDataFlow',                                     
            releaseString = releaseString)                                   
    ToolSvc += DQTDataFlowMon 
    ManagedAthenaGlobalMon.AthenaMonTools += [ DQTDataFlowMon ]

#-- set up output histogram file ------------------------------------------------------------------------------
log.info('HLTFlags %s', HLTMonFlags)

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output += ["GLOBAL DATAFILE='" + DQMonFlags.histogramFile() + "' OPT='RECREATE'"]

include( "TrigHLTMonitoring/HLTMonitoring_topOptions.py" )

if hasattr(runArgs,"useDB"):
    if runArgs.useDB:
        pass
    elif not runArgs.useDB:
        log.info("Update tools to use xAODConfigTool")
        for tool in ToolSvc.getAllChildren(): 
            tool_prop = tool.getDefaultProperties()
            for prop,value in tool_prop.iteritems():
                if prop == "TrigConfigTool":
                    log.info("Set TrigConfigTool %s",tool.getName())
                    tool.TrigConfigTool="TrigConf::xAODConfigTool"

