#==============================================================
#
# Job options file
#
# @file TrigT1CTMonitoringJobOptions.py
#
# @brief Monitoring for LVL1 Central Trigger bytestream.
#
# @author David Berge <berge@cern.ch>
#
#==============================================================
#==============================================================
# Merge of both TrigT1CTMonitoringJobOptions_forRecExCommission and TrigT1CTMonitoringJobOptions

## get a handle to the ServiceManager
#-----------COMMON CODE---------------------

from AthenaCommon.Logging import logging
ctpmonlog = logging.getLogger("CTPMonitoringSetup")

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec

LHCFillStateAvailable=False
UsedFillStateCoolFolderName="/LHC/DCS/FILLSTATE"
if LHCFillStateAvailable:
    UsedFillStateCoolFolderName="Unavailable"

# add CTP and MUCTPI data to ByteStreamAddressProviderSvc
if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
    from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc 
    svcMgr += ByteStreamAddressProviderSvc()
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
    "MuCTPI_RDO/MUCTPI_RDO",
    "CTP_RDO/CTP_RDO",
    "MuCTPI_RIO/MUCTPI_RIO",
    "CTP_RIO/CTP_RIO"
    ]

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#---------------------------------------------------------------
# Central-Trigger Monitoring
#---------------------------------------------------------------
from TrigT1CTMonitoring.TrigT1CTMonitoringConf import TrigT1CTMonitoring__BSMonitoring as BSMon
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager

#-----------ONLINE CODE---------------------
if athenaCommonFlags.isOnline():
    # fwinkl 09/2021: removed online-specific code as it was certainly broken and likely unused
    ctpmonlog.error("Setting up CTP/MUCTPI BS monitoring for online currently not supported")

#-----------OFFLINE CODE---------------------
else:
    from PyUtils.MetaReaderPeeker import metadata
    isMC = 'IS_SIMULATION' in metadata['eventTypes']
    ctpmonlog.info("Setting up CTP/MUCTPI BS monitoring for offline (MC=%s)", isMC)

    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
    cfg = TriggerConfigGetter()

    ## add pre algorithms for rerunning CTP simulation
    if 'IS_SIMULATION' not in metadata['eventTypes']:
        #svcMgr.DSConfigSvc.readLVL1Thr=True
        #svcMgr.DSConfigSvc.readLVL1BG=True

        # Wrap everything in a sequence which will force algs to execute in order, even in MT mode
        from AthenaCommon.AlgSequence import AthSequencer
        CTPMonSeq=AthSequencer('CTPMonSeq')

        from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Config import L1MuctpiPhase1_on_Data
        CTPMonSeq += L1MuctpiPhase1_on_Data()

        from TrigT1CTMonitoring.TrigT1CTMonitoringConf import TrigT1CTMonitoring__DeriveSimulationInputs as DeriveSimulationInputs
        CTPMonSeq += DeriveSimulationInputs(do_MuCTPI_input=True,
                                            do_L1Calo_sim=False)

        from TrigT1CTP.TrigT1CTPConfig import CTPSimulationOnData
        CTPMonSeq += CTPSimulationOnData("CTPSimulation")

        # configure simulation histogram output directory
        from AthenaMonitoring.DQMonFlags import DQMonFlags
        histbase = "/" + DQMonFlags.monManFileKey() + "/"
        if DQMonFlags.monManRun():
            from RecExConfig.AutoConfiguration import GetRunNumber
            histbase += "run_%i/L1" % GetRunNumber()
        else:
            histbase += "L1Simulation"
        try:
            CTPMonSeq.CTPSimulation.HistPath = histbase
        except AttributeError as ex:
            print (ex," ignore for now")
            import traceback
            traceback.print_exc()

    ## add an AthenaMonManager algorithm to the list of algorithms to be run
    monMan = AthenaMonManager( name="CTMonManager",
                            FileKey             = DQMonFlags.monManFileKey(),
                            Environment         = DQMonFlags.monManEnvironment(),
                            ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                            DataType            = DQMonFlags.monManDataType() )
    CTPMonSeq += monMan

    # check if global muons are on
    if not rec.doMuon:
        if 'IS_SIMULATION' not in metadata['eventTypes']:
            CTBSMonTool = BSMon(ProcessRoIBResult = False,
                                InclusiveTriggerThresholds = False,
                                ProcessMuctpiData = False,
                                ProcessMuctpiDataRIO = False,
                                CompareRerun = True,
                                FillStateCoolFolderName=UsedFillStateCoolFolderName)
        else:
            CTBSMonTool = BSMon(ProcessRoIBResult = False,
                                InclusiveTriggerThresholds = False,
                                ProcessMuctpiData = False,
                                ProcessMuctpiDataRIO = False,
                                RunOnESD = True,
                                CompareRerun = False,
                                FillStateCoolFolderName=UsedFillStateCoolFolderName)
    else:
        if 'IS_SIMULATION' not in metadata['eventTypes']:
            CTBSMonTool = BSMon(ProcessRoIBResult = True,
                                ProcessMuctpiData = True,
                                ProcessMuctpiDataRIO = True,
                                CompareRerun = True)
        else:
            CTBSMonTool = BSMon(ProcessRoIBResult = True,
                                ProcessMuctpiData = True,
                                ProcessMuctpiDataRIO = False,
                                RunOnESD = True,
                                CompareRerun = False,
                                FillStateCoolFolderName=UsedFillStateCoolFolderName)

    processByteStream = True


    if 'IS_SIMULATION' not in metadata['eventTypes']:
        from IOVDbSvc.CondDB import conddb
        conddb.addFolder('TRIGGER', '/TRIGGER/LUMI/LBLB') 
        if LHCFillStateAvailable:
            conddb.addFolder('DCS_OFL', "/LHC/DCS/FILLSTATE")
        conddb.addFolder('TDAQ', '/TDAQ/RunCtrl/DataTakingMode', className='AthenaAttributeList')
        conddb.addFolder('TRIGGER', "/TRIGGER/LVL1/RFPhase")
        conddb.addFolder('TRIGGER', '/TRIGGER/LVL1/CTPCoreInputMapping')

    monMan.AthenaMonTools += [ CTBSMonTool ]

    CTPMonSeq.Sequential=True
    topSequence += CTPMonSeq


del ctpmonlog
