# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#==============================================================
# Job options file for the AFP_Digitization package
#==============================================================

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from Digitization.PileUpMergeSvcConfigNew import PileUpMergeSvcCfg
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from SGComps.SGInputLoaderConfig import SGInputLoaderCfg


# The earliest bunch crossing time for which interactions will be sent to the AFP Digitization code.
def AFP_FirstXing(flags):
    return 0 #Assume AFP is only sensitive to the current bunch crossing, for the moment


# The latest bunch crossing time for which interactions will be sent to the AFP Digitization code.
def AFP_LastXing(flags):
    return 0 #Assume AFP is only sensitive to the current bunch crossing, for the moment


def AFP_SIDPUXinfFolder(flags, name="AFP_SIDXinfFolder", **kwargs):
    """Return a PileUpXingFoldertool for AFP SID"""
    # bunch crossing range in ns
    kwargs.setdefault("FirstXing", AFP_FirstXing(flags))
    kwargs.setdefault("LastXing", AFP_LastXing(flags))
    kwargs.setdefault("ItemList", ["AFP_SIDSimHitCollection#AFP_SIDSimHitCollection"])
    PileUpXingFolder = CompFactory.PileUpXingFolder
    return PileUpXingFolder(name, **kwargs)


def AFP_TDPUXinfFolder(flags, name="AFP_TDXinfFolder", **kwargs):
    """Return a PileUpXingFoldertool for AFP TD"""
    # bunch crossing range in ns
    kwargs.setdefault("FirstXing", AFP_FirstXing(flags))
    kwargs.setdefault("LastXing", AFP_LastXing(flags))
    kwargs.setdefault("ItemList", ["AFP_TDSimHitCollection#AFP_TDSimHitCollection"])
    PileUpXingFolder = CompFactory.PileUpXingFolder
    return PileUpXingFolder(name, **kwargs)


def AFP_DigitizationToolCfg(flags, **kwargs):
    """Return ComponentAccumulator with AFP digitization tool."""
    acc = ComponentAccumulator()
    
    if flags.Digitization.PileUp:
        intervals = []
        if flags.Digitization.DoXingByXingPileUp:
            kwargs.setdefault("FirstXing", AFP_FirstXing(flags))
            kwargs.setdefault("LastXing", AFP_LastXing(flags))
        else:
            intervals += [acc.popToolsAndMerge(AFP_SIDPUXinfFolder(flags))]
            intervals += [acc.popToolsAndMerge(AFP_TDPUXinfFolder(flags))]
        kwargs.setdefault("mergeSvc", acc.getPrimaryAndMerge(PileUpMergeSvcCfg(flags, Intervals=intervals)).name)
        kwargs.setdefault("OnlyUseContainerName", True)
    else:
        kwargs.setdefault("mergeSvc", '')
        kwargs.setdefault("OnlyUseContainerName", False)
    
    from RngComps.RandomServices import AthRNGSvcCfg
    kwargs.setdefault("RndmSvc", acc.getPrimaryAndMerge(AthRNGSvcCfg(flags)).name)
    
    AFP_PileUpTool = CompFactory.AFP_PileUpTool("AFP_PileUpTool", **kwargs)
    AFP_DigiTop = CompFactory.AFP_DigiTop("AFP_DigiTop",DigitizationTool=AFP_PileUpTool)
    acc.addEventAlgo(AFP_DigiTop)
    
    return acc


def AFP_DigitizationOutputCfg(flags):
    """Return ComponentAccumulator with Output for AFP. Not standalone."""
    acc = ComponentAccumulator()
    if flags.Output.doWriteRDO:
        acc.merge(OutputStreamCfg(flags,"RDO",["AFP_TDDigiCollection#AFP_TDDigiCollection", 
                                               "AFP_SiDigiCollection#AFP_SiDigiCollection", 
                                               "xAOD::AFPSiHitContainer#AFPSiHitContainer", "xAOD::AFPSiHitAuxContainer#AFPSiHitContainerAux.",
                                               "xAOD::AFPToFHitContainer#AFPToFHitContainer", "xAOD::AFPToFHitAuxContainer#AFPToFHitContainerAux."]))
    
    acc.merge(SGInputLoaderCfg(flags,Load=[('AFP_TDSimHitCollection','StoreGateSvc+AFP_TDSimHitCollection'), 
                                           ('AFP_SIDSimHitCollection','StoreGateSvc+AFP_SIDSimHitCollection')] ) )
    
    return acc


def AFP_DigitizationCfg(flags, **kwargs):
    """Return ComponentAccumulator for AFP digitization and Output"""
    acc = AFP_DigitizationToolCfg(flags, **kwargs)
    acc.merge(AFP_DigitizationOutputCfg(flags))
    
    return acc


if __name__ == "__main__":
    """Test AFP ComponentAccumulator Digitization configuration modules, inspired by Digitization/DigitizationConfigNew_test.py"""
    
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from Digitization.DigitizationSteering import DigitizationMainCfg, DigitizationMessageSvcCfg

    # Set up logging and new style config
    log.setLevel(DEBUG)
    Configurable.configurableRun3Behavior = True

    # Configure
    ConfigFlags.Input.Files = ["test.pool.root"]
    ConfigFlags.IOVDb.GlobalTag = "OFLCOND-MC16-SDR-RUN2-09"
    ConfigFlags.Detector.EnableAFP = True
    ConfigFlags.Detector.GeometryAFP = False
    ConfigFlags.GeoModel.Align.Dynamic = False
    ConfigFlags.Concurrency.NumThreads = 1
    ConfigFlags.Concurrency.NumConcurrentEvents=1
    ConfigFlags.Beam.NumberOfCollisions = 0.
    ConfigFlags.Output.RDOFileName = "myRDO.pool.root"

    # TODO: temporary
    ConfigFlags.Digitization.TruthOutput = True

    ConfigFlags.fillFromArgs() # enable unit tests to switch only parts of reco: AFP_Digitization.py Detector.EnableAFP=True
    ConfigFlags.lock()
    ConfigFlags.dump()

    # Construct our accumulator to run
    acc = DigitizationMainCfg(ConfigFlags)
    acc.merge(DigitizationMessageSvcCfg(ConfigFlags))

    # Dump config
    acc.getService("StoreGateSvc").Dump = True
    acc.getService("ConditionStore").Dump = True
    acc.printConfig(withDetails=True)
    ConfigFlags.dump()

    acc.foreach_component("*AFP*").OutputLevel=DEBUG

    # Execute and finish
    status = acc.run(maxEvents=3)
    if status.isFailure():
        import sys
        sys.exit(-1)
