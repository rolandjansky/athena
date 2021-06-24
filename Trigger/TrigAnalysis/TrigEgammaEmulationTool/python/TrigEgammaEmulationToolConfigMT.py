#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#


#
# Configure legacy L1
#
def createL1Calo( name , info, OutputLevel=0 ):
    
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaCommon.SystemOfUnits import GeV
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
                                L1Thr                  = L1thr * GeV,
                                HadCoreCutMin          = HadCoreCutMin[wp],
                                HadCoreCutOff          = HadCoreCutOff[wp],
                                HadCoreSlope           = HadCoreSlope[wp],
                                EmIsolCutMin           = EmIsolCutMin[wp],
                                EmIsolCutOff           = EmIsolCutOff[wp],
                                EmIsolSlope            = EmIsolSlope[wp],
                                IsolCutMax             = 50,
                                OutputLevel            = OutputLevel)

    return L1CaloTool

#
# Setup chain by name (only single chains)
#
def setupChain( trigger , OutputLevel=0):

    from AthenaConfiguration.ComponentFactory import CompFactory

    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
    d = dictFromChainName(trigger)

    signature = d['signatures'][0]
    from pprint import pprint
    pprint(d)

    from TrigEgammaHypo.TrigEgammaFastCaloHypoTool          import TrigEgammaFastCaloHypoToolFromDict
    from TrigEgammaHypo.TrigEgammaFastElectronHypoTool      import TrigEgammaFastElectronHypoToolFromDict
    from TrigEgammaHypo.TrigEgammaFastPhotonHypoTool        import TrigEgammaFastPhotonHypoToolFromDict
    from TrigEgammaHypo.TrigEgammaPrecisionCaloHypoTool     import TrigEgammaPrecisionCaloHypoToolFromDict
    from TrigEgammaHypo.TrigEgammaPrecisionElectronHypoTool import TrigEgammaPrecisionElectronHypoToolFromDict
    from TrigEgammaHypo.TrigEgammaPrecisionPhotonHypoTool   import TrigEgammaPrecisionPhotonHypoToolFromDict
    from TrigEgammaHypo.TrigEgammaPrecisionTrackingHypoTool import TrigEgammaPrecisionTrackingHypoToolFromDict

    # Configure L1Calo 
    L1CaloTool = createL1Calo(trigger + "_Step0" , d)

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
                                OutputLevel             = OutputLevel
    )

    return chain



def createEmulator( name, TriggerList=[], ElectronTriggerList=[], PhotonTriggerList=[]):

    #from TriggerMenuMT.HLTMenuConfig.Egamma.EgammaDefs import createTrigEgammaPrecisionElectronLHSelectors
    from AthenaConfiguration.ComponentFactory import CompFactory
    hypos = []
    # setup all chains
    for trigger in TriggerList:
        chain = setupChain(trigger)
        hypos.append(chain)


    #
    # create emulator
    #
    emulator = CompFactory.Trig.TrigEgammaEmulationToolMT(name, 
                                        #ElectronLHTools = createTrigEgammaPrecisionElectronLHSelectors(),
                                        #PhotonIsEMTools = createTrigEgammaPrecisionPhotonSelectors(),
                                        ElectronTriggerList = ElectronTriggerList,
                                        PhotonTriggerList = PhotonTriggerList,
                                        HypoTools = hypos,
                                        )

    return emulator



if __name__=='__main__':

    triggerList = [ "HLT_g20_loose_L1EM15",
                    "HLT_e28_lhtight_ivarloose_L1EM22VHI"]

    emulator = createEmulator( "EmulatorTool", triggerList )