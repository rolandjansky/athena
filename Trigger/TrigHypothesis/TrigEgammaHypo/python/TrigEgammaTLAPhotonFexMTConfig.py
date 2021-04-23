from GaudiKernel.Constants import WARNING


# verify if this needs to be in the same directory of the FexMT.cxx, in TrigHypothesis/Egamma/python corrispondente
def getConfiguredTLAPhotonSelector(photonPtThreshold=35000, maxNPhotons=-1, inputPhotonsKey="HLT_FastPhotons", TLAPhotonsKey="HLT_Photons_TLA", outputLevel=WARNING):

    # we need to change names to  'TrigEgammaTLAPhotonFexMT', it doesn't make sense to keep it as it is

    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
    from TrigEDMConfig.TriggerEDMRun3 import recordable

    TrigEgammaTLAPhotonFexMT = conf2toConfigurable(CompFactory.TrigEgammaTLAPhotonFexMT() )

    TrigEgammaTLAPhotonFexMT.photonPtThreshold = photonPtThreshold
    TrigEgammaTLAPhotonFexMT.maxNPhotons = maxNPhotons
    TrigEgammaTLAPhotonFexMT.TLAInputPhotonContainer = inputPhotonsKey

    #makes sure that the name of the output collection is acceptable by Run3 stadards (i.e. is contained in TriggerHLTListRun3 defined in
    # TrigEDMConfig/python/TriggerEDMRun3.py
    # we should add lines to include "HLT_FastPhotons_TLA", otherwise we get an error
    outputPhotonsValidName = recordable(TLAPhotonsKey)
    TrigEgammaTLAPhotonFexMT.TLAOutputPhotonContainer = outputPhotonsValidName
    TrigEgammaTLAPhotonFexMT.OutputLevel = outputLevel

    return TrigEgammaTLAPhotonFexMT
