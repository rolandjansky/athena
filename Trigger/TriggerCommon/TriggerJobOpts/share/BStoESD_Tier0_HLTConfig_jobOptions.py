# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
##############################################################
# BStoESD_Tier0_HLTConfig_jobOptions.py
# For DATA reconstruction
##############################################################

# Configuration depends on TriggerFlags.configForStartup():
#   HLTonlineNoL1Thr : Everything is taken from COOL, except LVL1ConfigSvc for thresholds
#   HLToffline       : HLT is ran offline, configuration is read from XML/JSON files
#   HLTonline        : Normal running, everything is taken from COOL

from RecExConfig.RecAlgsFlags import recAlgs
from RecExConfig.RecFlags import rec
from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
from AthenaCommon.AppMgr import ServiceMgr, ToolSvc
from AthenaCommon.Include import include

# First check is HLT psk is ok, if not, turn trigger off.
if tf.configForStartup() != 'HLToffline':
    include( "TriggerJobOpts/TriggerConfigCheckHLTpsk.py" )

if rec.doTrigger():
    
    # configure the HLT config
    if not hasattr( ServiceMgr, "ByteStreamAddressProviderSvc" ):
        from ByteStreamCnvSvcBase. ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
        ServiceMgr += ByteStreamAddressProviderSvc()

    tf.readBS=True # needed in HLTTriggerGetter - do not understand why it is not
    # true by default when globalflags.InputFormat = 'bytestream'
    tf.doLVL1= False # needed to not rerun the trigger
    tf.doEF= False # needed to not rerun the trigger
    tf.doLVL2 = False  # needed to not rerun the trigger
    tf.configurationSourceList = ['ds']

    try:
        from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
        cfg=TriggerConfigGetter()
    except Exception:
        from AthenaCommon.Resilience import treatException
        treatException("Could not run TriggerConfigGetter()")


    #---------------------------------------------------------------------------
    if tf.configForStartup() == "HLTonlineNoL1Thr":

        # Want to use LVL1ConfigSvc for LVL1 thresholds only
        from TrigConfigSvc.TrigConfigSvcConf import TrigConf__LVL1ConfigSvc
        l1 = TrigConf__LVL1ConfigSvc("LVL1ConfigSvc")
        l1.XMLFile = "LVL1config_SingleBeam_v1_7-bit_trigger_types.xml"
        l1.CreateLegacyObjects=True
        ServiceMgr += l1

        tf.inputLVL1configFile = "LVL1config_SingleBeam_v1_7-bit_trigger_types.xml"
        tf.inputLVL1configFile.lock() # this is needed to not be overwritten by TrigT1CTMonitoring

        # The following are using LVL1ConfigSvc (no thresholds involved)
        # They should use COOL and not the xml file
        if not hasattr(ToolSvc,'RecMuCTPIByteStreamTool'):
            from TrigT1ResultByteStream.TrigT1ResultByteStreamConf import RecMuCTPIByteStreamTool
            ToolSvc += RecMuCTPIByteStreamTool("RecMuCTPIByteStreamTool")
        ToolSvc.RecMuCTPIByteStreamTool.LVL1ConfigSvc="TrigConf::TrigConfigSvc/TrigConfigSvc"

        if not hasattr(ToolSvc,'L1EmTauTools'):
            from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1EmTauTools
            ToolSvc += LVL1__L1EmTauTools("L1EmTauTools")
        ToolSvc.L1EmTauTools.LVL1ConfigSvc="TrigConf::TrigConfigSvc/TrigConfigSvc"

        #from TrigT1CTP.TrigT1CTPConf import LVL1CTP__CBNTAA_CTP_RDO
        #a1 = LVL1CTP__CBNTAA_CTP_RDO("CBNTAA_CTP_RDO")
        #a1.LVL1ConfigSvc="TrigConf::TrigConfigSvc/TrigConfigSvc"

    #---------------------------------------------------------------------------    
    elif tf.configForStartup()=="HLToffline": # HLT is ran offline so cannot read from COOL.
        tf.readLVL1configFromXML = True # has to use the .xml file used for reco
        tf.readHLTconfigFromXML = True # has to use the .xml file used for reco
        # You have to set the 2 following files to the .xml files you want.
        # Here are the default files for reprocessing special case with trigger
        tf.inputHLTconfigFile = "HLTMenu.xml" # Has to be set correctly
        tf.inputLVL1configFile = "LVL1Menu.xml" # Has to be set correctly
        tf.inputHLTconfigFile.lock() 
        tf.inputLVL1configFile.lock() # this is needed to not be overwritten by TrigT1CTMonitoring

    #---------------------------------------------------------------------------    
    elif tf.configForStartup()=="HLTonline": # need to talk to clients using LVL1ConfigSvc and add new folders into
        #Just to be sure we use no LVL1 xml file and do not forget any clients
        tf.inputLVL1configFile = "LVL1configDUMMY.xml"
        tf.inputLVL1configFile.lock()
        # do not need thresholds but are using LVL1ConfigSvc
        if not hasattr(ToolSvc,'RecMuCTPIByteStreamTool'):
            from TrigT1ResultByteStream.TrigT1ResultByteStreamConf import RecMuCTPIByteStreamTool
            ToolSvc += RecMuCTPIByteStreamTool("RecMuCTPIByteStreamTool")
        ToolSvc.RecMuCTPIByteStreamTool.LVL1ConfigSvc="TrigConf::TrigConfigSvc/TrigConfigSvc"

        #from TrigT1CTP.TrigT1CTPConf import LVL1CTP__CBNTAA_CTP_RDO
        #a1 = LVL1CTP__CBNTAA_CTP_RDO("CBNTAA_CTP_RDO")
        #a1.LVL1ConfigSvc="TrigConf::TrigConfigSvc/TrigConfigSvc"

        # need thresholds so should be called just if the info is there in COOL
        from AnalysisTriggerAlgs.AnalysisTriggerAlgsConfig import RoIBResultToAOD
        a2 = RoIBResultToAOD("RoIBResultToAOD")
        a2.LVL1ConfigSvc="TrigConf::TrigConfigSvc/TrigConfigSvc"
        a2.L1JEMJetTools.LVL1ConfigSvc="TrigConf::TrigConfigSvc/TrigConfigSvc"
        a2.L1CPMTools.LVL1ConfigSvc="TrigConf::TrigConfigSvc/TrigConfigSvc"

        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__TriggerTowerMaker
        a4 = LVL1__TriggerTowerMaker("TriggerTowerMaker")
        a4.LVL1ConfigSvc="TrigConf::TrigConfigSvc/TrigConfigSvc"

        if not hasattr(ToolSvc,'L1EtTools'):
            from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1EtTools
            ToolSvc +=LVL1__L1EtTools("L1EtTools")
        ToolSvc.L1EtTools.LVL1ConfigSvc="TrigConf::TrigConfigSvc/TrigConfigSvc"

        if not hasattr(ToolSvc,'L1JEPEtSumsTools'):
            from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1JEPEtSumsTools
            ToolSvc += LVL1__L1JEPEtSumsTools("L1JEPEtSumsTools")
        ToolSvc.L1JEPEtSumsTools.LVL1ConfigSvc="TrigConf::TrigConfigSvc/TrigConfigSvc"

        if not hasattr(ToolSvc,'L1JEPHitsTools'):
            from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1JEPHitsTools
            ToolSvc += LVL1__L1JEPHitsTools("L1JEPHitsTools")
        ToolSvc.L1JEPHitsTools.LVL1ConfigSvc="TrigConf::TrigConfigSvc/TrigConfigSvc"

        import TrigT1CaloTools.TrigT1CaloToolsConf as calotools  # noqa: F401

        for toolName in ['L1JetCMXTools', 'L1EnergyCMXTools', 'L1TriggerTowerTool', 'L1CPMTools',
                         'L1CPCMXTools', 'L1EmTauTools', 'L1JEMJetTools', 'L1JetEtTools', 'L1JetTools']:
            if not hasattr(ToolSvc, toolName ):
                ToolSvc += eval('calotools.LVL1__%s( toolName )' % toolName)
            getattr(ToolSvc, toolName).LVL1ConfigSvc="TrigConf::TrigConfigSvc/TrigConfigSvc"

    #---------------------------------------------------------------------------
    if recAlgs.doTrigger():
        try:
            from TriggerJobOpts.T0TriggerGetter import T0TriggerGetter
            triggerGetter = T0TriggerGetter()
        except Exception:
            from AthenaCommon.Resilience import treatException
            treatException("Could not import TriggerJobOpts.TriggerGetter . Switched off !" )
            recAlgs.doTrigger=False
    elif rec.doWriteBS():
        include( "ByteStreamCnvSvc/RDP_ByteStream_jobOptions.py" )
## end of configure the HLT config
