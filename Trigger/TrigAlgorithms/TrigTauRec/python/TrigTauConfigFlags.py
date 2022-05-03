# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from tauRec.TauConfigFlags import createTauConfigFlags

def createTrigTauConfigFlags():
    flags = AthConfigFlags()
    flags.join(createTauConfigFlags(), prefix='Trigger.Offline')
    flags.Trigger.Offline.Tau.CalibrateLCConfig = 'TES2016_LC_online_inc.root'
    flags.Trigger.Offline.Tau.MvaTESConfig = 'OnlineMvaTES_BRT_v1.weights.root'
    flags.Trigger.Offline.Tau.tauRecToolsCVMFSPath = 'TrigTauRec/00-11-02'
    # this flag only exists in the trigger, but 'cloneAndReplace' in 'addFlagsCategory' assume a 'Trigger.Offline.Tau' structure
    flags.addFlag("Trigger.Offline.Tau.FTFTauCoreBDTConfig", "FTF_tauCore_BDT_v0.root")

    return flags


if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW

    ConfigFlags.lock()
    ConfigFlags.Tau.CalibrateLCConfig
    ConfigFlags.Trigger.doLVL1
    ConfigFlags.dump("Tau|Trigger")

#    print( ConfigFlags.Trigger.Tau.CalibrateLCConfig )
    assert ConfigFlags.Tau.CalibrateLCConfig != ConfigFlags.Trigger.Offline.Tau.CalibrateLCConfig, "No difference between trigger customization"
    ConfigFlags.dump("Tau|Trigger")
