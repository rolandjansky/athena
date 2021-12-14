#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentFactory import CompFactory
from TrigEgammaMonitoring.TrigEgammaMonitCategoryMT import single_electron_triggers, single_photon_triggers

#
# emulator config class
#
class TrigEgammaEmulationToolConfig:

    #
    # Constructor
    #
    def __init__(self, name, triggerList         = [], 
                             OutputLevel         = 0, 
                             ElectronTriggerList = single_electron_triggers, 
                             PhotonTriggerList   = single_photon_triggers):

        self.name = name
        self.__chains = []
        self.TriggerList = triggerList
        self.OutputLevel = OutputLevel
        self.ElectronTriggerList = ElectronTriggerList
        self.PhotonTriggerList = PhotonTriggerList

        # force cofnig file path
        self.ElectronCBConfigFilePath = None
        self.ElectronDNNConfigFilePath = None
        self.ElectronLHConfigFilePath = None
        self.FastCaloConfigFilePath = None
        self.PhotonCBConfigFilePath = None

        self.__configured_triggers = []


    def core(self):
        self.update()
        return self.__emulator
 

    def update(self):
        for trigger in self.TriggerList:
            if trigger not in self.__configured_triggers:
                hypo = self.setupChain(trigger)
                self.__emulator.HypoTools += [hypo]
                self.__configured_triggers += trigger

    #
    # Configure emulator tool
    #
    def configure(self):

        from TrigEgammaHypo.TrigEgammaFastCaloHypoTool          import createTrigEgammaFastCaloSelectors
        from TrigEgammaHypo.TrigEgammaPrecisionPhotonHypoTool   import createTrigEgammaPrecisionPhotonSelectors
        from TrigEgammaHypo.TrigEgammaPrecisionElectronHypoTool import TrigEgammaPrecisionElectronCBSelectorCfg
        from TrigEgammaHypo.TrigEgammaPrecisionElectronHypoTool import TrigEgammaPrecisionElectronLHSelectorCfg
        from TrigEgammaHypo.TrigEgammaPrecisionElectronHypoTool import TrigEgammaPrecisionElectronDNNSelectorCfg


        # create the emulator tool
        self.__emulator = CompFactory.Trig.TrigEgammaEmulationToolMT( self.name, 
                                                               ElectronTriggerList = self.ElectronTriggerList,
                                                               PhotonTriggerList = self.PhotonTriggerList )
        
        # NOTE: will be adapt next
        self.__emulator.RingerTools              = createTrigEgammaFastCaloSelectors(ConfigFilePath = self.FastCaloConfigFilePath)
        self.__emulator.ElectronLHSelectorTools  = TrigEgammaPrecisionElectronLHSelectorCfg(ConfigFilePath = self.ElectronLHConfigFilePath).getPublicTool()
        self.__emulator.ElectronCBSelectorTools  = TrigEgammaPrecisionElectronCBSelectorCfg(ConfigFilePath = self.ElectronCBConfigFilePath).getPublicTool()
        self.__emulator.ElectronDNNSelectorTools = TrigEgammaPrecisionElectronDNNSelectorCfg(ConfigFilePath = self.ElectronDNNConfigFilePath).getPublicTool()
        self.__emulator.PhotonCBSelectorTools    = createTrigEgammaPrecisionPhotonSelectors(ConfigFilePath = self.PhotonCBConfigFilePath)
    
    
    #
    # Setup chain by name (only single chains)
    #
    def setupChain( self, trigger ):

        import cppyy
        cppyy.load_library('libElectronPhotonSelectorToolsDict')

        from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
        d = dictFromChainName(trigger)

        signature = d['signatures'][0]

        from TrigEgammaHypo.TrigEgammaFastCaloHypoTool          import TrigEgammaFastCaloHypoToolFromDict
        from TrigEgammaHypo.TrigEgammaFastElectronHypoTool      import TrigEgammaFastElectronHypoToolFromDict
        from TrigEgammaHypo.TrigEgammaFastPhotonHypoTool        import TrigEgammaFastPhotonHypoToolFromDict
        from TrigEgammaHypo.TrigEgammaPrecisionCaloHypoTool     import TrigEgammaPrecisionCaloHypoToolFromDict
        from TrigEgammaHypo.TrigEgammaPrecisionElectronHypoTool import TrigEgammaPrecisionElectronHypoToolFromDict
        from TrigEgammaHypo.TrigEgammaPrecisionPhotonHypoTool   import TrigEgammaPrecisionPhotonHypoToolFromDict
        from TrigEgammaHypo.TrigEgammaPrecisionTrackingHypoTool import TrigEgammaPrecisionTrackingHypoToolFromDict

        # Configure L1Calo 
        L1CaloTool = self.setupL1Calo(trigger + "_Step0" , d)

        # Configure HLT
        FastCaloTool = TrigEgammaFastCaloHypoToolFromDict(d, tool = CompFactory.Trig.TrigEgammaEmulationFastCaloHypoTool(trigger+'_Step1') )

        PrecisionCaloTool       = TrigEgammaPrecisionCaloHypoToolFromDict(d, tool = CompFactory.Trig.TrigEgammaEmulationPrecisionCaloHypoTool(trigger+'_Step3'))
        PrecisionTrackingTool   = TrigEgammaPrecisionTrackingHypoToolFromDict(d, tool = CompFactory.Trig.TrigEgammaEmulationPrecisionTrackingHypoTool(trigger+'_Step4'))

        if signature == 'Electron':
            FastTool                = TrigEgammaFastElectronHypoToolFromDict(d, tool = CompFactory.Trig.TrigEgammaEmulationFastElectronHypoTool(trigger+'_Step2'))
            PrecisionTool           = TrigEgammaPrecisionElectronHypoToolFromDict(d, tool = CompFactory.Trig.TrigEgammaEmulationPrecisionElectronHypoTool(trigger+'_Step5'))
        elif signature == 'Photon':
            FastTool                = TrigEgammaFastPhotonHypoToolFromDict(d, tool = CompFactory.Trig.TrigEgammaEmulationFastPhotonHypoTool(trigger+'_Step2'))
            PrecisionTool           = TrigEgammaPrecisionPhotonHypoToolFromDict(d, tool = CompFactory.Trig.TrigEgammaEmulationPrecisionPhotonHypoTool(trigger+'_Step5'))

        chain = CompFactory.Trig.TrigEgammaEmulationChain(
                                    name                    = trigger,
                                    L1Seed                  = L1CaloTool,
                                    Steps                   = [FastCaloTool, FastTool, PrecisionCaloTool, PrecisionTrackingTool, PrecisionTool],
                                    Chain                   = trigger,
                                    Signature               = signature.lower(),
                                    OutputLevel             = self.OutputLevel
        )

        return chain



    #
    # Configure legacy L1
    #
    def setupL1Calo( self, name , info):

        from AthenaConfiguration.ComponentFactory import CompFactory
        import re

        #chainName = info['chainName']
        L1Item = info['chainParts'][0]['L1threshold']
        L1thr = float( re.findall(r'\d+', L1Item)[0] )
        wp = 0 # default

        #                        [Default, Tight , Medium, Loose ]
        HadCoreCutMin          = [ 1.0   ,  1.0  ,  1.0  ,  1.0  ]
        HadCoreCutOff          = [-0.2   , -0,2  , -0.2  , -0.2  ]
        HadCoreSlope           = [ 1/23. ,  1/23.,  1/23.,  1/23.]
        EmIsolCutMin           = [ 2.0   ,  1.0  ,  1.0  ,  1.5  ]
        EmIsolCutOff           = [-1.8   , -2.6  , -2.0  , -1.8  ]
        EmIsolSlope            = [ 1/8.  ,  1/8. ,  1/8. ,  1/8. ]

        # Configure L1
        L1CaloTool = CompFactory.Trig.TrigEgammaEmulationL1CaloHypoTool(
                                    name                   = name,
                                    L1Item                 = L1Item,
                                    L1Thr                  = L1thr, 
                                    HadCoreCutMin          = HadCoreCutMin[wp],
                                    HadCoreCutOff          = HadCoreCutOff[wp],
                                    HadCoreSlope           = HadCoreSlope[wp],
                                    EmIsolCutMin           = EmIsolCutMin[wp],
                                    EmIsolCutOff           = EmIsolCutOff[wp],
                                    EmIsolSlope            = EmIsolSlope[wp],
                                    IsolCutMax             = 50,
                                    OutputLevel            = self.OutputLevel)

        return L1CaloTool








def TrigEgammaEmulationToolTestConfig(inputFlags):

    from AthenaConfiguration.ComponentFactory import CompFactory    
    triggerList = [ 
                    "HLT_e17_lhvloose_L1EM15VHI",
                    "HLT_e5_etcut_L1EM3",
                    "HLT_e26_lhtight_L1EM22VHI", 
                    "HLT_g20_loose_L1EM15"
                    ]

    '''Function to configures some algorithms in the monitoring system.'''
    # The following class will make a sequence, configure algorithms, and link
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'TrigEgammaEmulationToolTestCfg')
    
    test = helper.addAlgorithm( CompFactory.TrigEgammaEmulationToolTest("EmulationTest") )
    acc = helper.resobj
    EgammaMatchTool = CompFactory.TrigEgammaMatchingToolMT()
    EgammaMatchTool.DeltaR=0.4
    acc.addPublicTool(EgammaMatchTool)

    emulator = TrigEgammaEmulationToolConfig("EmulatorTool")
    emulator.configure()

    emulator.TriggerList += triggerList
    acc.addPublicTool(emulator.core())   
    test.MatchTool = EgammaMatchTool
    test.EmulatorTool = emulator.core()
    test.ElectronTriggerList = [
                                #"HLT_e5_etcut_L1EM3",
                                "HLT_e17_lhvloose_L1EM15VHI",
                                #"HLT_e26_lhtight_L1EM22VHI",
                                ]
    test.PhotonTriggerList = []

    # include some monitoring histograms
    basepath = '/HLT/EgammaMon'
    for trigger in triggerList:
        cutLabels = ['L1Calo','L2Calo','L2','EFCalo','HLT']
        monGroup = helper.addGroup( test, trigger, basepath+'/EgammaEmulationTest/'+trigger )
        monGroup.defineHistogram("decisions", type='TH1I', path='', title="Event Selection; Cut ; Count",
          xbins=len(cutLabels), xmin=0, xmax=len(cutLabels), xlabels=cutLabels)
        monGroup.defineHistogram("emulations", type='TH1I', path='', title="Event Selection; Cut ; Count",
          xbins=len(cutLabels), xmin=0, xmax=len(cutLabels), xlabels=cutLabels)

    return helper.result()
    


if __name__=='__main__':

    # ATR-11839 to fix the egammaPid import
    from PyUtils.Helpers import ROOT6Setup
    ROOT6Setup()

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    path = '/afs/cern.ch/work/j/jodafons/public/valid_sampleA/AOD_fixCalo.pool.root'
    ConfigFlags.Input.Files = [path]
    ConfigFlags.Input.isMC = True
    ConfigFlags.Output.HISTFileName = 'TrigEgammaMonitorOutput.root'
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))
    testAcc = TrigEgammaEmulationToolTestConfig(ConfigFlags)
    cfg.merge(testAcc)
  
    # If you want to turn on more detailed messages ...
    cfg.printConfig(withDetails=False) # set True for exhaustive info
    cfg.run(-1) #use cfg.run(20) to only run on first 20 events



