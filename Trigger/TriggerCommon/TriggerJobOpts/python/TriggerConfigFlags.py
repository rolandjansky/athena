# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaCommon.Logging import logging
log=logging.getLogger('TriggerConfigFlags')

def createTriggerFlags():
    flags = AthConfigFlags()    

    # enables L1 simulation
    flags.addFlag('Trigger.doLVL1', lambda prevFlags: prevFlags.Input.isMC)

    # enables L1 topological trigger simulation
    flags.addFlag('Trigger.doL1Topo', True )

    # writes additional info from Topo simulation
    flags.addFlag('Trigger.writeL1TopoValData', True )
    
    # need proper documentation
    flags.addFlag('Trigger.useL1CaloCalibration', False)

    # need proper documentation
    flags.addFlag('Trigger.useRun1CaloEnergyScale', False)

    # enable HLT part of the trigger
    flags.addFlag('Trigger.doHLT', True)

    # changes decoding of L1 so that allways all configured chains are enabled, testing mode
    flags.addFlag("Trigger.L1Decoder.forceEnableAllChains", False)

    # Enable Run-3 LVL1 simulation and/or decoding
    flags.addFlag('Trigger.enableL1Phase1', False)

    # Enable usage of new L1 menu   
    flags.addFlag('Trigger.readLVL1FromJSON', True)

    # Enable Run-2 L1Calo simulation and/or decoding (possible even if enablePhase1 is True)
    flags.addFlag('Trigger.enableL1CaloLegacy', True)

    # Enable Inner Detector
    flags.addFlag('Trigger.doID', True)

    # Enable muon system
    flags.addFlag('Trigger.doMuon', True)

    # Enable calorimeters
    flags.addFlag('Trigger.doCalo', True)

    # Checks the validity of each Decision Object produced by a HypoAlg, including all of its
    # parents all the way back to the L1 decoder. Potentially CPU expensive.
    flags.addFlag('Trigger.doRuntimeNaviVal', False)

    # if 1, Run1 decoding version is set; if 2, Run2; if 3, Run 3
    def EDMVersion(flags):
        '''
        Determine Trigger EDM version based on the input file. For ByteStream, Run-3 EDM is indicated
        by HLT ROD version > 1.0, for both Runs 1 and 2 the HLT ROD version was 0.0 and the run number
        is used to disambiguate between them. For POOL inputs, the EDM version is determined based on
        finding a characteristic HLT navigation collection in the file.
        '''
        _log = logging.getLogger('TriggerConfigFlags.EDMVersion')
        _log.debug("Attempting to determine EDMVersion")
        default_version = 3
        if flags.Input.Format=="BS":
            _log.debug("Input format is ByteStream")
            inputFileName = flags.Input.Files[0]
            if not inputFileName and flags.Common.isOnline():
                _log.info("Online reconstruction, no input file. Return default EDMVersion=%d", default_version)
                return default_version

            _log.debug("Checking ROD version")
            import eformat
            from libpyeformat_helper import SubDetector
            bs = eformat.istream(inputFileName)

            rodVersionM = -1
            rodVersionL = -1
            # Find the first HLT ROBFragment in the first event
            for robf in bs[0]:
                if robf.rob_source_id().subdetector_id()==SubDetector.TDAQ_HLT:
                    rodVersionM = robf.rod_minor_version() >> 8
                    rodVersionL = robf.rod_minor_version() & 0xFF
                    _log.debug("HLT ROD minor version from input file is %d.%d", rodVersionM, rodVersionL)
                    break

            # Case 1: failed to read ROD version
            if rodVersionM < 0 or rodVersionL < 0:
                _log.warning("Cannot determine HLT ROD version from input file, falling back to run-number-based decision")
            # Case 2: ROD version indicating Run 3
            elif rodVersionM >= 1:
                _log.info("Determined EDMVersion to be 3, because running on BS file with HLT ROD version %d.%d",
                          rodVersionM, rodVersionL)
                return 3
            # Case 3: ROD version indicating Run 1 or 2 - use run number to disambiguate
            runNumber = flags.Input.RunNumber[0]
            _log.debug("Read run number %s", runNumber)

            boundary_run12 = 230000
            boundary_run23 = 368000

            if not runNumber or runNumber <= 0:
                _log.warning("Cannot determine EDM version because run number %s is invalid. "
                             "Return default EDMVersion=%d", runNumber, default_version)
                return default_version
            elif runNumber < boundary_run12:
                # Run-1 data
                _log.info("Determined EDMVersion to be 1 based on BS file run number (runNumber < %d)",
                          boundary_run12)
                return 1
            elif runNumber < boundary_run23:
                # Run-2 data
                _log.info("Determined EDMVersion to be 2 based on BS file run number (%d < runNumber < %d)",
                          boundary_run12, boundary_run23)
                return 2
            else:
                # Run-3 data
                _log.info("Determined EDMVersion to be 3 based on BS file run number (runNumber > %d)",
                          boundary_run23)
                return 3
        else:
            # POOL files: decide based on HLT output type present in the file
            _log.debug("EDMVersion: Input format is POOL -- determine from input file collections")
            if "HLTResult_EF" in flags.Input.Collections:
                _log.info("Determined EDMVersion to be 1, because HLTResult_EF found in POOL file")
                return 1
            elif "TrigNavigation" in flags.Input.Collections:
                _log.info("Determined EDMVersion to be 2, because TrigNavigation found in POOL file")
                return 2
            elif "HLTNav_Summary" in flags.Input.Collections:
                _log.info("Determined EDMVersion to be 3, because HLTNav_Summary found in POOL file")
                return 3

        _log.warning("Could not determine EDM version from the input file. Return default EDMVersion=%d",
                     default_version)
        return default_version

    flags.addFlag('Trigger.EDMVersion', lambda prevFlags: EDMVersion(prevFlags))
                     
    # enables additional algorithms colecting MC truth infrmation  (this is only used by IDso maybe we need Trigger.ID.doTruth only?)
    flags.addFlag('Trigger.doTruth', False)

    # only enable services for analysis and BS -> ESD processing (we need better name)
    flags.addFlag('Trigger.doTriggerConfigOnly', False)

    # Enables collection and export of detailed monitoring data of the HLT execution
    flags.addFlag('Trigger.CostMonitoring.doCostMonitoring', False)
    flags.addFlag('Trigger.CostMonitoring.chain', 'HLT_costmonitor_CostMonDS_L1All')
    flags.addFlag('Trigger.CostMonitoring.outputCollection', 'HLT_TrigCostContainer')
    flags.addFlag('Trigger.CostMonitoring.monitorAllEvents', False)
    flags.addFlag('Trigger.CostMonitoring.monitorROBs', False)


    # enable Bcm inputs simulation
    flags.addFlag('Trigger.L1.doBcm', True)

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

    # configuration tune for various years of Run2 (why string?)
    flags.addFlag('Trigger.run2Config', '2018')

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
    
    # L1 topo XML file name
    def _deriveTopoConfigName(prevFlags):
        import re
        menuSetup = prevFlags.Trigger.triggerMenuSetup
        m = re.match(r'(.*v\d).*', menuSetup)
        if m:
            menuSetup = m.groups()[0]
        return "L1Topoconfig_" + menuSetup + "_" + prevFlags.Trigger.menuVersion + ".xml"
    flags.addFlag('Trigger.LVL1TopoConfigFile', _deriveTopoConfigName)

    
    # trigger reconstruction 

    # enables the correction for pileup in cell energy calibration (should it be moved to some place where other calo flags are defined?)
    flags.addFlag('Trigger.calo.doOffsetCorrection', True )

    # helper to switch between versions
    def __tunes(default, ver2016, ver2017):
        from collections import defaultdict
        return lambda year:  defaultdict( lambda: default,
                                           (('2016', ver2016),
                                            ('2017', ver2017)))[year]

                                           
    # Particle ID tune
    flags.addFlag('Trigger.egamma.pidVersion',
                lambda prevFlags:
                __tunes(default='ElectronPhotonSelectorTools/trigger/rel21_mc16a/',
                        ver2016='ElectronPhotonSelectorTools/trigger/rel21_mc16a/',
                        ver2017='ElectronPhotonSelectorTools/trigger/rel21_20170214/')( prevFlags.Trigger.run2Config )
    )

    # cluster correction version, allowed value is: None or v12phiflip_noecorrnogap
    flags.addFlag('Trigger.egamma.clusterCorrectionVersion',
                lambda prevFlags:
                __tunes(default=None,
                        ver2016=None,
                        ver2017='v12phiflip_noecorrnogap')( prevFlags.Trigger.run2Config )
    )
    # tune of MVA
    flags.addFlag('Trigger.egamma.calibMVAVersion',
                  lambda prevFlags:
                  __tunes(default='egammaMVACalib/online/v3',
                          ver2016='egammaMVACalib/online/v3',
                          ver2017='egammaMVACalib/online/v6')( prevFlags.Trigger.run2Config )
    )

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

    from TrigInDetConfig.TrigTrackingCutFlags import createTrigTrackingFlags
    flags.addFlagsCategory( 'Trigger.InDetTracking', createTrigTrackingFlags )

    return flags
    # for reference, this flags are skipped as never used or never set in fact, or set identical to de default or used in a very old JO:
    # fakeLVL1, useCaloTTL
    # doCosmicSim - old JO
    # doMergedHLTResult - not needed now
    # doAlwaysUnpackDSResult - never set
    # doTrt - fast TRT or trigger - never used
    # doLucid - not sure if ever set to False, identical rec flag exists
    # doZdc - never set to anything different than default
    # configForStartup
    # the flags related to trigger DB are redundant of triggerConfig - need to decide if they are needed in this form
    # also not defined the Prescale sets yet
    # in signatures
    # egamma: ringerVersion - not used
    # muon: doMuonCalibrationStream - not used
    # tau: doTrackingApproach - not used


    
import unittest
class __YearDependentFlagTest(unittest.TestCase):    
    def runTest(self):
        """... Check if year dependent flags propagate the info correctly"""
        from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
        flags.Trigger.run2Config='2017'
        self.assertEqual(flags.Trigger.egamma.clusterCorrectionVersion, "v12phiflip_noecorrnogap", " dependent flag setting does not work")
        flags.dump()

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
    suite.addTest(__YearDependentFlagTest())
    suite.addTest(__UseOfOfflineRecoFlagsTest())
    runner = unittest.TextTestRunner(failfast=False)
    runner.run(suite)
