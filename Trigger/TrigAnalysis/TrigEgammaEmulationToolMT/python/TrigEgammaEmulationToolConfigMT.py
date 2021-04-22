#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#



#
# Setup chain by name (only single chains)
#
def setupChain( trigger , OutputLevel=0):

    from TriggerMenuMT.HLTMenuConfig.Menu.DictFromChainName import dictFromChainName
    info = dictFromChainName(trigger)

    signature = info['signatures'][0]

    from pprint import pprint
    pprint(info)


    # Configure L1Calo 
    from TrigEgammaEmulationToolMT.TrigEgammaEmulationL1CaloHypoConfig import createL1Calo
    L1CaloTool = createL1Calo(trigger + "_Step0" , info)

    from TrigEgammaEmulationToolMT.TrigEgammaEmulationFastCaloHypoConfig import createFastCalo
    FastCaloTool = createFastCalo(trigger+"_Step1", info)

    if signature == 'Electron':
        # Configure Precision Electron
        from TrigEgammaEmulationToolMT.TrigEgammaEmulationPrecisionElectronHypoConfig import createPrecisionElectron
        PrecisionTool = createPrecisionElectron(trigger + "_Step4", info)

    elif signature == 'Photon':
        # Configure Precision Photon
        from TrigEgammaEmulationToolMT.TrigEgammaEmulationPrecisionPhotonHypoConfig import createPrecisionPhoton
        PrecisionTool = createPrecisionPhoton(trigger+"_Step4", info)




    from TrigEgammaEmulationToolMT.TrigEgammaEmulationToolMTConf import Trig__TrigEgammaEmulationChain
    chain = Trig__TrigEgammaEmulationChain(
                                name                = trigger,
                                L1CaloTool          = L1CaloTool,
                                FastCaloTool        = FastCaloTool, 
                                FastTool            = "",
                                PrecisionCaloTool   = "",
                                PrecisionTool       = PrecisionTool,
                                Chain               = trigger,
                                L1Item              = "",
                                Signature           = "electron",
                                OutputLevel         = OutputLevel
    )

    return chain



def createEmulator( name, triggerList ):

    from TrigEgammaEmulationToolMT.TrigEgammaEmulationToolMTConf import Trig__TrigEgammaEmulationToolMT
    #from TriggerMenuMT.HLTMenuConfig.Egamma.EgammaDefs import createTrigEgammaPrecisionPhotonSelectors
    #from TriggerMenuMT.HLTMenuConfig.Egamma.EgammaDefs import createTrigEgammaPrecisionElectronLHSelectors
    
    hypos = []

    # setup all chains
    for trigger in triggerList:
        chain = setupChain(trigger)
        hypos.append(chain)

   
    #
    # create emulator
    #
    emulator = Trig__TrigEgammaEmulationToolMT(name, 
                                        #ElectronLHTools = createTrigEgammaPrecisionElectronLHSelectors(),
                                        #PhotonIsEMTools = createTrigEgammaPrecisionPhotonSelectors(),
                                        ElectronTriggerList = [],
                                        PhotonTriggerList = [],
                                        HypoTools = hypos,
                                        )

    return emulator



if __name__=='__main__':

    triggerList = [ "HLT_g20_loose_L1EM15",
                    "HLT_e28_lhtight_ivarloose_L1EM22VHI"]

    emulator = createEmulator( "EmulatorTool", triggerList )