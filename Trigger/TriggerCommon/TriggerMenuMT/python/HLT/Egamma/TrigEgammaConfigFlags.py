# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createTrigEgammaConfigFlags():
    flags = AthConfigFlags()

    # Particle ID tune
    flags.addFlag('Trigger.egamma.electronPidVersion'          , 'ElectronPhotonSelectorTools/trigger/rel22_20210611/')
    flags.addFlag('Trigger.egamma.electronNoPixPidVersion'     , 'ElectronPhotonSelectorTools/trigger/rel22_20210611/')
    flags.addFlag('Trigger.egamma.electronNoGSFPidVersion'     , 'ElectronPhotonSelectorTools/trigger/rel22_20210611/')
    flags.addFlag('Trigger.egamma.electronNoGSFNoPixPidVersion', 'ElectronPhotonSelectorTools/trigger/rel22_20210611/')
    flags.addFlag('Trigger.egamma.electronHIPidVersion'        , 'ElectronPhotonSelectorTools/trigger/rel22_20210611/')
    flags.addFlag('Trigger.egamma.photonPidVersion'            , 'ElectronPhotonSelectorTools/trigger/rel22_20210611/')
    flags.addFlag('Trigger.egamma.dnnVersion'                  , 'ElectronPhotonSelectorTools/trigger/rel21_20220421/')
    flags.addFlag('Trigger.egamma.ringerVersion'               , 'RingerSelectorTools/TrigL2_20210702_r4/')

    # cluster correction version, allowed value is: None or v12phiflip_noecorrnogap
    flags.addFlag('Trigger.egamma.clusterCorrectionVersion', 'v12phiflip_noecorrnogap')

    # tune of MVA
    flags.addFlag('Trigger.egamma.Calib.precCaloMVAVersion'  , 'egammaMVACalib/online/v7')
    flags.addFlag('Trigger.egamma.Calib.precEgammaMVAVersion', 'egammaMVACalib/online/v7')

    return flags


if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW

    ConfigFlags.lock()
    ConfigFlags.dump("Egamma|Trigger")
