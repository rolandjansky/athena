# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.Logging import logging
from TrigEDMConfig.Utils import getEDMVersionFromBS

log=logging.getLogger('TriggerConfigFlags')

def createTriggerFlags():
    flags = AthConfigFlags()    

    # enables L1 simulation
    flags.addFlag('Trigger.doLVL1', lambda prevFlags: prevFlags.Input.isMC)

    # enables L1 topological trigger simulation
    flags.addFlag('Trigger.doL1Topo', True )

    # need proper documentation
    flags.addFlag('Trigger.useRun1CaloEnergyScale', False)

    # enable HLT part of the trigger
    flags.addFlag('Trigger.doHLT', True)

    # changes decoding of L1 so that allways all configured chains are enabled, testing mode
    flags.addFlag("Trigger.L1Decoder.forceEnableAllChains", False)

#    # Enable Run-3 LVL1 simulation and/or decoding
#    flags.addFlag('Trigger.enableL1Phase1', False)

    # Enable Run-3 LVL1 muon simulation and/or decoding
    flags.addFlag('Trigger.enableL1MuonPhase1', False)

    # Enable Run-3 LVL1 calo simulation and/or decoding
    flags.addFlag('Trigger.enableL1CaloPhase1', False)

    # Enable L1Topo simulation to write inputs to txt
    flags.addFlag('Trigger.enableL1TopoDump', False)

    # Enable usage of new L1 menu   
    flags.addFlag('Trigger.readLVL1FromJSON', True)

    # Enable Run-2 L1Calo simulation and/or decoding (possible even if enablePhase1 is True)
    flags.addFlag('Trigger.enableL1CaloLegacy', True)

    # Enable emulation tool for NSW-TGC coincidence in A-side
    flags.addFlag('Trigger.L1MuonSim.EmulateNSWA', False)

    # Enable emulation tool for NSW-TGC coincidence in C-side
    flags.addFlag('Trigger.L1MuonSim.EmulateNSWC', False)

    # Offline CondDB tag for RPC/TGC coincidence window in rerunLVL1 on data
    flags.addFlag('Trigger.L1MuonSim.CondDBOffline', 'OFLCOND-MC16-SDR-RUN2-04')

    # Enable Inner Detector
    flags.addFlag('Trigger.doID', True)

    # Enable muon system
    flags.addFlag('Trigger.doMuon', True)

    # Enable calorimeters
    flags.addFlag('Trigger.doCalo', True)

    # Checks the validity of each Decision Object produced by a HypoAlg, including all of its
    # parents all the way back to the L1 decoder. Potentially CPU expensive.
    # also enables per step decison printouts
    flags.addFlag('Trigger.doRuntimeNaviVal', False)

    # if 1, Run1 decoding version is set; if 2, Run2; if 3, Run 3
    def EDMVersion(flags):
        """Determine Trigger EDM version based on the input file."""
        _log = logging.getLogger('TriggerConfigFlags.EDMVersion')

        default_version = -1     # intentionally invalid default value, ATR-22856

        if flags.Input.Format=="BS":
            _log.debug("Input format is ByteStream")

            if not flags.Input.Files and flags.Common.isOnline():
                _log.info("Online reconstruction, no input file. Return default EDMVersion=%d", default_version)
                return default_version

            version = getEDMVersionFromBS(flags.Input.Files[0])

            return version if version is not None else default_version

        else:
            # POOL files: decide based on HLT output type present in the file
            _log.debug("Input format is POOL -- determine from input file collections")
            collections = flags.Input.Collections
            if "HLTResult_EF" in collections:
                _log.info("Determined EDMVersion to be 1, because HLTResult_EF found in POOL file")
                return 1
            elif "TrigNavigation" in collections:
                _log.info("Determined EDMVersion to be 2, because TrigNavigation found in POOL file")
                return 2
            elif any("HLTNav_Summary" in s for s in collections):
                _log.info("Determined EDMVersion to be 3, because HLTNav_Summary.* found in POOL file")
                return 3

        _log.warning("Could not determine EDM version from the input file. Return default EDMVersion=%d",
                     default_version)
        return default_version

    flags.addFlag('Trigger.EDMVersion', lambda prevFlags: EDMVersion(prevFlags))
    flags.addFlag('Trigger.doEDMVersionConversion', True)
    flags.addFlag('Trigger.doConfigVersionConversion', True)
    # enables additional algorithms colecting MC truth infrmation  (this is only used by IDso maybe we need Trigger.ID.doTruth only?)
    flags.addFlag('Trigger.doTruth', False)

    # True if we have at least one input file, it is a POOL file, it has a metadata store, and the store has xAOD trigger configuration data
    # in either the run-2 or run-3 formats.
    def TrigConfMeta(flags):
        from AthenaConfiguration.AutoConfigFlags import GetFileMD
        md = GetFileMD(flags.Input.Files) if len(flags.Input.Files) > 0 else {}
        return ("metadata_items" in md and any(('TriggerMenu' in key) for key in md["metadata_items"].keys()))

    # Flag to sense if trigger confioguration POOL metadata is available on the job's input
    flags.addFlag('Trigger.InputContainsConfigMetadata', lambda prevFlags: TrigConfMeta(prevFlags))

    # only enable services for analysis and BS -> ESD processing (we need better name)
    flags.addFlag('Trigger.doTriggerConfigOnly', False)

    # Enables collection and export of detailed monitoring data of the HLT execution
    flags.addFlag('Trigger.CostMonitoring.doCostMonitoring', False)
    flags.addFlag('Trigger.CostMonitoring.chain', 'HLT_noalg_CostMonDS_L1All')
    flags.addFlag('Trigger.CostMonitoring.outputCollection', 'HLT_TrigCostContainer')
    flags.addFlag('Trigger.CostMonitoring.monitorAllEvents', False)
    flags.addFlag('Trigger.CostMonitoring.monitorROBs', True)

    # enable muon inputs simulation
    flags.addFlag('Trigger.L1.doMuons', True)

    # version of CTP data, int value up to 4
    flags.addFlag('Trigger.L1.CTPVersion', 4)

    # list of thresholds (not sure if we want to use new flags to generate L1, leaving out for now?)
    
    # partition name used to determine online vs offline BS result writing
    import os
    flags.addFlag('Trigger.Online.partitionName', os.getenv('TDAQ_PARTITION') or '')

    # shortcut to check if job is running in a partition (i.e. partition name is not empty)
    flags.addFlag('Trigger.Online.isPartition', lambda prevFlags: len(prevFlags.Trigger.Online.partitionName)>0)
    
    # write BS output file
    flags.addFlag('Trigger.writeBS', False)

    # Write transient BS before executing HLT algorithms (for running on MC RDO with clients which require BS inputs)
    flags.addFlag('Trigger.doTransientByteStream', False)

    # list of EDM objects to be written to AOD
    flags.addFlag('Trigger.AODEDMSet', 'AODSLIM')

    # list of objects to be written to ESD
    flags.addFlag('Trigger.ESDEDMSet', 'ESD')

    # to allow stroing extra EDM items via preExec
    flags.addFlag('Trigger.ExtraEDMList', [])

    # tag to be used for condutions used by HLT code
    flags.addFlag('Trigger.OnlineCondTag', 'CONDBR2-HLTP-2018-01')

    # geometry version used by HLT online
    flags.addFlag('Trigger.OnlineGeoTag', 'ATLAS-R2-2016-01-00-01')

    # comissionig options
    # one of:  'HltOnly',
    #    'Lvl1Only',
    #    'FullTrigger',
    #    'NoTrigger'
    flags.addFlag('Trigger.dataTakingConditions', 'FullTrigger')
                

    # use or not frontier proxies
    flags.addFlag('Trigger.triggerUseFrontier', False)

    # the configuration source
    # see https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerConfigFlag
    flags.addFlag('Trigger.triggerConfig', 'FILE')

    # name of the trigger menu
    flags.addFlag('Trigger.triggerMenuSetup', 'LS2_v1_BulkMCProd_prescale')

    # modify the slection of chains that are run (default run all), see more in GenerateMenuMT_newJO

    flags.addFlag('Trigger.triggerMenuModifier', ['all'])

    # name of the trigger menu
    flags.addFlag('Trigger.generateMenuDiagnostics', False)

    # version of the menu
    from AthenaCommon.AppMgr import release_metadata
    flags.addFlag('Trigger.menuVersion',
                  lambda prevFlags:  release_metadata()['release'] )
    
    # generate or not the HLT configuration
    flags.addFlag('Trigger.generateHLTMenu', False)
    
    # HLT XML file name 
    flags.addFlag('Trigger.HLTMenuFile',
                  lambda prevFlags: 'HLTMenu_'+prevFlags.Trigger.triggerMenuSetup+'_' + prevFlags.Trigger.menuVersion + '.xml')

    # generate or not the L1 configuration
    flags.addFlag('Trigger.generateL1Menu', False)
    
    def _deriveL1ConfigName(prevFlags):
        import re
        log = logging.getLogger('TrigConfigSvcCfg')
        pattern = re.compile(r'_v\d+|DC14')
        menuName=prevFlags.Trigger.triggerMenuSetup
        patternPos = pattern.search(menuName)
        if patternPos:
            menuName=menuName[:patternPos.end()]
        else:
            log.info('Can\'t find pattern to shorten menu name, either non-existent in name or not implemented.')
        
        return "LVL1config_"+menuName+"_" + prevFlags.Trigger.menuVersion + ".xml"

    # L1 XML file name 
    flags.addFlag('Trigger.LVL1ConfigFile', _deriveL1ConfigName)
   
    # L1 Json file name 
    flags.addFlag('Trigger.L1MenuFile',
                  lambda prevFlags: 'L1Menu_'+prevFlags.Trigger.triggerMenuSetup+'_' + prevFlags.Trigger.menuVersion + '.json')
    

    # trigger reconstruction 

    # enables the correction for pileup in cell energy calibration (should it be moved to some place where other calo flags are defined?)
    flags.addFlag('Trigger.calo.doOffsetCorrection', True )

    # Particle ID tune
    flags.addFlag('Trigger.egamma.pidVersion', 'ElectronPhotonSelectorTools/trigger/rel21_20170214/')

    # cluster correction version, allowed value is: None or v12phiflip_noecorrnogap
    flags.addFlag('Trigger.egamma.clusterCorrectionVersion', 'v12phiflip_noecorrnogap')

    # tune of MVA
    flags.addFlag('Trigger.egamma.calibMVAVersion', 'egammaMVACalib/online/v6')

    # muons
    flags.addFlag('Trigger.muon.doEFRoIDrivenAccess', False)

    # muon offline reco flags varaint for trigger
    def __muonSA():
        from MuonConfig.MuonConfigFlags import createMuonConfigFlags
        muonflags = createMuonConfigFlags()
        muonflags.Muon.useTGCPriorNextBC=True
        muonflags.Muon.MuonTrigger=True
        muonflags.Muon.SAMuonTrigger=True
        return muonflags 

    def __muon():
        from MuonConfig.MuonConfigFlags import createMuonConfigFlags
        muonflags = createMuonConfigFlags()
        muonflags.Muon.useTGCPriorNextBC=True
        muonflags.Muon.MuonTrigger=True

        return muonflags 

    def __muonCombined():
        from MuonCombinedConfig.MuonCombinedConfigFlags import createMuonCombinedConfigFlags
        muonflags = createMuonCombinedConfigFlags()
        muonflags.MuonCombined.doCaloTrkMuId = False
        muonflags.MuonCombined.doSiAssocForwardMuons = False
        muonflags.MuonCombined.doStatisticalCombination = False
        muonflags.MuonCombined.doMuGirl = False
        return muonflags


    flags.addFlagsCategory('Trigger.Offline.SA', __muonSA, prefix=True)
    flags.addFlagsCategory('Trigger.Offline', __muon, prefix=True)
    flags.addFlagsCategory('Trigger.Offline.Combined', __muonCombined, prefix=True)

    from TrigTauRec.TrigTauConfigFlags import createTrigTauConfigFlags
    flags.addFlagsCategory('Trigger.Offline.Tau', createTrigTauConfigFlags)
    #TODO come back and use systematically the same 

    from TrigInDetConfig.TrigTrackingCutFlags import createTrigTrackingFlags
    flags.addFlagsCategory( 'Trigger.InDetTracking', createTrigTrackingFlags )

    # NB: Longer term it may be worth moving these into a PF set of config flags, but right now the only ones that exist do not seem to be used in the HLT.
    # When we use component accumulators for this in the HLT maybe we should revisit this
    # PFO-muon removal option for the full-scan hadronic signatures.
    # Options are:
    #   "None": Do no PFO-muon removal
    #   "Calo": Use the calo-tagging tools from the muon slice
    #   "Iso" : Use the mainly isolation-based selections based on the MET associator package
    flags.addFlag("Trigger.FSHad.PFOMuonRemoval", "Calo")

    # the minimum pT threshold to use for the muon removal
    flags.addFlag("Trigger.FSHad.PFOMuonRemovalMinPt", 10 * GeV)

    # Switch on AMVF vertice and priority TTVA for jet slice
    flags.addFlag("Trigger.Jet.doAMVFPriorityTTVA", False)
    flags.addFlag("Trigger.Jet.doMC16_EOverP", False)

    return flags
    # for reference, this flags are skipped as never used or never set in fact, or set identical to de default or used in a very old JO:
    # configForStartup
    # the flags related to trigger DB are redundant of triggerConfig - need to decide if they are needed in this form
    # also not defined the Prescale sets yet
    # in signatures
    # egamma: ringerVersion - not used
    # muon: doMuonCalibrationStream - not used
    # tau: doTrackingApproach - not used


    
import unittest
class __UseOfOfflineRecoFlagsTest(unittest.TestCase):
    def runTest(self):
        """... Check if offline reco flags can be added to trigger"""
        from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
        flags.Trigger.Offline.Muon.doMDTs=False
        flags.Muon.doMDTs=True
        self.assertEqual(flags.Trigger.Offline.Muon.doMDTs, False, " dependent flag setting does not work")
        self.assertEqual(flags.Muon.doMDTs, True, " dependent flag setting does not work")

        newflags = flags.cloneAndReplace('Muon', 'Trigger.Offline.Muon')

        self.assertEqual(flags.Muon.doMDTs, True, " dependent flag setting does not work")
        self.assertEqual(newflags.Muon.doMDTs, False, " dependent flag setting does not work")
        newflags.dump()

if __name__ == "__main__":
    suite = unittest.TestSuite()
    suite.addTest(__UseOfOfflineRecoFlagsTest())
    runner = unittest.TextTestRunner(failfast=False)
    runner.run(suite)
