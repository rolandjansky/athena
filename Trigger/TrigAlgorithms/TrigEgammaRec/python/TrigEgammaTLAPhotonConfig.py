#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

def getConfiguredTLAPhotonSelector(photonPtThreshold=10000, maxNPhotons=-1, inputPhotonsKey="HLT_egamma_Photons", TLAPhotonsKey="HLT_egamma_Photons_TLA"):



    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
    from TrigEDMConfig.TriggerEDMRun3 import recordable
    TrigEgammaTLAPhotonFex = conf2toConfigurable(CompFactory.TrigEgammaTLAPhotonReAlgo() )

    TrigEgammaTLAPhotonFex.photonPtThreshold = photonPtThreshold
    TrigEgammaTLAPhotonFex.maxNPhotons = maxNPhotons
    TrigEgammaTLAPhotonFex.TLAInputPhotonContainer = inputPhotonsKey
    

    #makes sure that the name of the output collection is acceptable by Run3 stadards (i.e. is contained in TriggerHLTListRun3 defined in
    # TrigEDMConfig/python/TriggerEDMRun3.py
    outputPhotonsValidName = recordable(TLAPhotonsKey)
    
    TrigEgammaTLAPhotonFex.TLAOutputPhotonContainer = outputPhotonsValidName
    

    return TrigEgammaTLAPhotonFex
