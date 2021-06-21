#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#



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


    # Configure L1Calo 
    from TrigEgammaEmulationToolMT.TrigEgammaEmulationL1CaloHypoConfig import createL1Calo
    L1CaloTool = createL1Calo(trigger + "_Step0" , d)

    from TrigEgammaEmulationToolMT.TrigEgammaEmulationFastCaloHypoConfig import TrigEgammaFastCaloHypoToolFromDict
    FastCaloTool = TrigEgammaFastCaloHypoToolFromDict(trigger+"_Step1", d)


    if signature == 'Electron':
        
        # Configure Fast Electron
        from TrigEgammaEmulationToolMT.TrigEgammaEmulationFastElectronHypoConfig import TrigEgammaFastElectronHypoToolFromDict
        FastTool = TrigEgammaFastElectronHypoToolFromDict(trigger + "_Step2", d)

        from TrigEgammaEmulationToolMT.TrigEgammaEmulationPrecisionCaloHypoConfig import TrigEgammaPrecisionCaloHypoToolFromDict
        PrecisionCaloTool = TrigEgammaPrecisionCaloHypoToolFromDict(trigger+"_Step3", d)

        from TrigEgammaEmulationToolMT.TrigEgammaEmulationPrecisionTrackingHypoConfig import TrigEgammaPrecisionTrackingHypoToolFromDict
        PrecisionTrackingTool = TrigEgammaPrecisionTrackingHypoToolFromDict(trigger+"_Step4", d)

        # Configure Precision Electron
        from TrigEgammaEmulationToolMT.TrigEgammaEmulationPrecisionElectronHypoConfig import TrigEgammaPrecisionElectronHypoToolFromDict
        PrecisionTool = TrigEgammaPrecisionElectronHypoToolFromDict(trigger + "_Step5", d)

    elif signature == 'Photon':

        # Configure Fast Photon
        from TrigEgammaEmulationToolMT.TrigEgammaEmulationFastPhotonHypoConfig import TrigEgammaFastPhotonHypoToolFromDict
        FastTool = TrigEgammaFastPhotonHypoToolFromDict(trigger + "_Step2", d)

        from TrigEgammaEmulationToolMT.TrigEgammaEmulationPrecisionCaloHypoConfig import TrigEgammaPrecisionCaloHypoToolFromDict
        PrecisionCaloTool = TrigEgammaPrecisionCaloHypoToolFromDict(trigger+"_Step3", d)

        from TrigEgammaEmulationToolMT.TrigEgammaEmulationPrecisionTrackingHypoConfig import TrigEgammaPrecisionTrackingHypoToolFromDict
        PrecisionTrackingTool = TrigEgammaPrecisionTrackingHypoToolFromDict(trigger+"_Step4", d)

        # Configure Precision Photon
        from TrigEgammaEmulationToolMT.TrigEgammaEmulationPrecisionPhotonHypoConfig import TrigEgammaPrecisionPhotonHypoToolFromDict
        PrecisionTool = TrigEgammaPrecisionPhotonHypoToolFromDict(trigger + "_Step5", d)



    chain = CompFactory.Trig.TrigEgammaEmulationChain(
                                name                    = trigger,
                                L1CaloTool              = L1CaloTool, # Step 0
                                FastCaloTool            = FastCaloTool, # Step 1
                                FastTool                = FastTool, # Step 2
                                PrecisionCaloTool       = PrecisionCaloTool, # Step 3
                                PrecisionTrackingTool   = PrecisionTrackingTool, # Step 4
                                PrecisionTool           = PrecisionTool, # Step 5
                                Chain                   = trigger,
                                L1Item                  = "",
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
    emulator = CompFactory.Trig.TrigEgammaEmulationTool(name, 
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