# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "Tool configuration to instantiate MCTruthClassifier with default configurations."

#---------------------------------------
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
from TrackToCalo.TrackToCaloConfig import ParticleCaloExtensionToolCfg

def MCTruthClassifierCfg(flags, **kwargs):
    """ 
    This is the default configuration allowing all options.
    By default, it does not do calo truth matching.
    """
    kwargs.setdefault("ParticleCaloExtensionTool","")
    return MCTruthClassifierCaloTruthMatchCfg(flags, **kwargs)

def MCTruthClassifierCaloTruthMatchCfg(flags, **kwargs):
    """ 
    This is the default configuration allowing all options.
    By default, it does calo truth matching.
    """

    acc=ComponentAccumulator()

    if "ParticleCaloExtensionTool" not in kwargs:
        extAcc = ParticleCaloExtensionToolCfg(flags)
        kwargs["ParticleCaloExtensionTool"] = extAcc.popPrivateTools()
        acc.merge(extAcc)

    kwargs.setdefault("barcodeG4Shift", flags.Sim.SimBarcodeOffset + 1)
    acc.setPrivateTools(MCTruthClassifier(**kwargs))
    return acc

##########################################################
# The functions below are for the old style and should be
# condsidered deprecated
##########################################################

def firstSimCreatedBarcode():
    """DEPRECATED!!! Return the simulation barcode offset for G4 particles from metadata

    In the new configuration scheme use the Sim.SimBarcodeOffset flag instead
    """

    from AthenaCommon.Logging import logging
    mlog = logging.getLogger("firstSimCreatedBarcode")
    mlog.info("This function should not be used in the new configuration scheme. Use Sim.SimBarcodeOffset flag insted")

    offset = 200e3

    # Don't try to run the input peeker for a generator job; it will fail
    # without a proper input file.
    # Is there a better way of doing this test?
    from AthenaCommon.AppMgr import theApp
    if getattr (theApp, 'EvtSel', None) == 'McEventSelector/EventSelector':
        mlog.info('Generator job: leaving SimBarcodeOffset at 200k')

    else:
        from RecExConfig.InputFilePeeker import inputFileSummary
        try:
            offset = int(inputFileSummary['metadata']['/Simulation/Parameters']['SimBarcodeOffset'])
        except Exception:
            mlog.info('Could not retrieve SimBarcodeOffset from /Simulation/Parameters, leaving at 200k')
    return int(offset + 1)


def getMCTruthClassifier(name="MCTruthClassifier", **kwargs):
    from AthenaCommon import CfgMgr
    kwargs.setdefault("barcodeG4Shift", firstSimCreatedBarcode())
    return CfgMgr.MCTruthClassifier(name, **kwargs)

def getJetMCTruthClassifier(name="JetMCTruthClassifier", **kwargs):
    kwargs.setdefault("ParticleCaloExtensionTool","")
    return getMCTruthClassifier(name, **kwargs)

##########################################################

if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Logging import log, logging
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    from AthenaConfiguration.ComponentAccumulator import printProperties
    Configurable.configurableRun3Behavior = 1
    log.setLevel(DEBUG)

    ConfigFlags.Input.isMC = True
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q221/21.0/myRDO.pool.root"]
    ConfigFlags.lock()

    mlog = logging.getLogger("MCTruthClassifierConfigTest")

    cfg = ComponentAccumulator()

    mlog.info("Configuring standard MCTruthClassifier")
    acc1 = MCTruthClassifierCfg(ConfigFlags)
    printProperties(mlog, acc1.popPrivateTools(), 1)

    mlog.info("Configuring MCTruthClassifier with calo truth matching")
    acc2 = MCTruthClassifierCaloTruthMatchCfg(ConfigFlags)
    printProperties(mlog, acc2.popPrivateTools(), 1)

    f = open("mctruthclassifer.pkl", "w")
    cfg.store(f)
    f.close()
