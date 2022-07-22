"""ComponentAccumulator configuration for MT pileup digitization

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from Digitization.RunDependentConfigNew import (
    maxNevtsPerXing,
    LumiProfileSvcCfg,
    NoProfileSvcCfg,
)
from Digitization.PileUpConfigNew import (
    LowPtMinBiasEventSelectorCfg,
    HighPtMinBiasEventSelectorCfg,
    CavernEventSelectorCfg,
    BeamGasEventSelectorCfg,
    BeamHaloEventSelectorCfg,
    StepArrayBMCfg,
    FixedArrayBMCfg,
    ArrayBMCfg,
)

from enum import Enum


class PUBkgKind(Enum):
    LOWPT = 1
    HIGHPT = 2
    CAVERN = 3
    BEAMGAS = 4
    BEAMHALO = 5


def BatchedMinbiasSvcCfg(flags, name="LowPtMinbiasSvc", kind=PUBkgKind.LOWPT, **kwargs):
    flags.dump()
    acc = ComponentAccumulator()
    skip = flags.Exec.SkipEvents
    n_evt = flags.Exec.MaxEvents
    n_bc = (
        flags.Digitization.PU.FinalBunchCrossing
        - flags.Digitization.PU.InitialBunchCrossing
        + 1
    )

    if kind == PUBkgKind.LOWPT:
        acc.merge(LowPtMinBiasEventSelectorCfg(flags))
        kwargs.setdefault("OnDemandMB", False)
        kwargs.setdefault("MBBatchSize", 10000)
        # kwargs.setdefault("MBBatchSize", 1.3 * flags.Digitization.PU.NumberOfLowPtMinBias * n_bc)
        kwargs.setdefault("NSimultaneousBatches", 1)
        kwargs.setdefault("HSBatchSize", flags.Concurrency.NumConcurrentEvents)
        evt_per_batch = kwargs["HSBatchSize"]
        actualNHSEventsPerBatch = (
            (skip // evt_per_batch) * [0]
            + (skip % evt_per_batch != 0) * [skip % evt_per_batch]
            + (n_evt // evt_per_batch) * [evt_per_batch]
            + (n_evt % evt_per_batch != 0) * [n_evt % evt_per_batch]
        )
        kwargs["actualNHSEventsPerBatch"] = actualNHSEventsPerBatch
        kwargs.setdefault(
            "BkgEventSelector", acc.getService("LowPtMinBiasEventSelector")
        )
    elif kind == PUBkgKind.HIGHPT:
        acc.merge(HighPtMinBiasEventSelectorCfg(flags))
        kwargs.setdefault("OnDemandMB", False)
        # load enough events that the probability of running out for any given event is no more than 1e-5
        kwargs.setdefault(
            "MBBatchSize", 8 * flags.Digitization.PU.NumberOfHighPtMinBias * n_bc
        )
        kwargs.setdefault("NSimultaneousBatches", flags.Concurrency.NumConcurrentEvents)
        kwargs.setdefault("HSBatchSize", 1)
        evt_per_batch = kwargs["HSBatchSize"]
        actualNHSEventsPerBatch = (
            (skip // evt_per_batch) * [0]
            + (skip % evt_per_batch != 0) * [skip % evt_per_batch]
            + (n_evt // evt_per_batch) * [evt_per_batch]
            + (n_evt % evt_per_batch != 0) * [n_evt % evt_per_batch]
        )
        kwargs["actualNHSEventsPerBatch"] = actualNHSEventsPerBatch
        kwargs.setdefault(
            "BkgEventSelector", acc.getService("HighPtMinBiasEventSelector")
        )
    elif kind == PUBkgKind.CAVERN:
        acc.merge(CavernEventSelectorCfg(flags))
        kwargs.setdefault("OnDemandMB", False)
        kwargs.setdefault("MBBatchSize", flags.Digitization.PU.NumberOfCavern * n_bc)
        kwargs.setdefault("NSimultaneousBatches", flags.Concurrency.NumConcurrentEvents)
        kwargs.setdefault("HSBatchSize", 1)
        evt_per_batch = kwargs["HSBatchSize"]
        actualNHSEventsPerBatch = (
            (skip // evt_per_batch) * [0]
            + (skip % evt_per_batch != 0) * [skip % evt_per_batch]
            + (n_evt // evt_per_batch) * [evt_per_batch]
            + (n_evt % evt_per_batch != 0) * [n_evt % evt_per_batch]
        )
        kwargs["actualNHSEventsPerBatch"] = actualNHSEventsPerBatch
        kwargs.setdefault("BkgEventSelector", acc.getService("CavernEventSelector"))
    elif kind == PUBkgKind.BEAMGAS:
        acc.merge(BeamGasEventSelectorCfg(flags))
        kwargs.setdefault("OnDemandMB", False)
        kwargs.setdefault(
            "MBBatchSize", 8 * flags.Digitization.PU.NumberOfBeamGas * n_bc
        )
        kwargs.setdefault("NSimultaneousBatches", flags.Concurrency.NumConcurrentEvents)
        kwargs.setdefault("HSBatchSize", 1)
        evt_per_batch = kwargs["HSBatchSize"]
        actualNHSEventsPerBatch = (
            (skip // evt_per_batch) * [0]
            + (skip % evt_per_batch != 0) * [skip % evt_per_batch]
            + (n_evt // evt_per_batch) * [evt_per_batch]
            + (n_evt % evt_per_batch != 0) * [n_evt % evt_per_batch]
        )
        kwargs["actualNHSEventsPerBatch"] = actualNHSEventsPerBatch
        kwargs.setdefault("BkgEventSelector", acc.getService("BeamGasEventSelector"))
    elif kind == PUBkgKind.BEAMHALO:
        acc.merge(BeamHaloEventSelectorCfg(flags))
        kwargs.setdefault("OnDemandMB", False)
        kwargs.setdefault(
            "MBBatchSize", 8 * flags.Digitization.PU.NumberOfBeamHalo * n_bc
        )
        kwargs.setdefault("NSimultaneousBatches", flags.Concurrency.NumConcurrentEvents)
        kwargs.setdefault("HSBatchSize", 1)
        evt_per_batch = kwargs["HSBatchSize"]
        actualNHSEventsPerBatch = (
            (skip // evt_per_batch) * [0]
            + (skip % evt_per_batch != 0) * [skip % evt_per_batch]
            + (n_evt // evt_per_batch) * [evt_per_batch]
            + (n_evt % evt_per_batch != 0) * [n_evt % evt_per_batch]
        )
        kwargs["actualNHSEventsPerBatch"] = actualNHSEventsPerBatch
        kwargs.setdefault("BkgEventSelector", acc.getService("BeamHaloEventSelector"))

    acc.addService(CompFactory.BatchedMinbiasSvc(name, **kwargs), primary=True)
    return acc


def PileUpMTAlgCfg(flags, **kwargs):
    kwargs.setdefault("Cardinality", flags.Concurrency.NumThreads)
    acc = ComponentAccumulator()
    # acc = BeamSpotFixerAlgCfg(flags)  # Needed currently for running on 21.0 HITS

    assert (
        flags.Digitization.DoXingByXingPileUp
    ), "PileUpMTAlg only supports XingByXing pile-up!"
    # Bunch Structure
    if flags.Digitization.PU.BeamIntensityPattern:
        if flags.Digitization.PU.SignalPatternForSteppingCache:
            # Simulate Bunch Structure with events sliding backwards on a conveyor belt
            acc.merge(StepArrayBMCfg(flags))
            kwargs.setdefault("BeamIntSvc", acc.getService("StepArrayBM"))
        elif flags.Digitization.PU.FixedT0BunchCrossing:
            # Simulate Bunch Structure using a fixed point for the central bunch crossing
            acc.merge(FixedArrayBMCfg(flags))
            kwargs.setdefault("BeamIntSvc", acc.getService("FixedArrayBM"))
        else:
            # Simulate Bunch Structure and allow the central bunch crossing to vary
            acc.merge(ArrayBMCfg(flags))
            kwargs.setdefault("BeamIntSvc", acc.getService("ArrayBM"))

    # define inputs
    assert not flags.Input.SecondaryFiles, (
        "Found ConfigFlags.Input.SecondaryFiles = %r; "
        "double event selection is not supported "
        "by PileUpMTAlg" % (not flags.Input.SecondaryFiles)
    )
    acc.merge(PoolReadCfg(flags))
    # add minbias service(s)
    if flags.Digitization.PU.LowPtMinBiasInputCols:
        svc = acc.getPrimaryAndMerge(
            BatchedMinbiasSvcCfg(flags, name="LowPtMinBiasSvc", kind=PUBkgKind.LOWPT)
        )
        kwargs.setdefault("LowPtMinbiasSvc", svc)
        kwargs.setdefault(
            "FracLowPt",
            flags.Digitization.PU.NumberOfLowPtMinBias
            / flags.Digitization.PU.NumberOfCollisions,
        )
    if flags.Digitization.PU.HighPtMinBiasInputCols:
        svc = acc.getPrimaryAndMerge(
            BatchedMinbiasSvcCfg(flags, name="HighPtMinBiasSvc", kind=PUBkgKind.HIGHPT)
        )
        kwargs.setdefault("HighPtMinbiasSvc", svc)
        kwargs.setdefault(
            "FracHighPt",
            flags.Digitization.PU.NumberOfHighPtMinBias
            / flags.Digitization.PU.NumberOfCollisions,
        )
    if flags.Digitization.PU.CavernInputCols:
        svc = acc.getPrimaryAndMerge(
            BatchedMinbiasSvcCfg(flags, name="CavernMinBiasSvc", kind=PUBkgKind.CAVERN)
        )
        kwargs.setdefault("CavernMinbiasSvc", svc)
        kwargs.setdefault("NumCavern", flags.Digitization.PU.NumberOfCavern)
    if flags.Digitization.PU.BeamGasInputCols:
        svc = acc.getPrimaryAndMerge(
            BatchedMinbiasSvcCfg(
                flags, name="BeamGasMinBiasSvc", kind=PUBkgKind.BEAMGAS
            )
        )
        kwargs.setdefault("BeamGasMinbiasSvc", svc)
        kwargs.setdefault("NumBeamGas", flags.Digitization.PU.NumberOfBeamGas)
    if flags.Digitization.PU.BeamHaloInputCols:
        svc = acc.getPrimaryAndMerge(
            BatchedMinbiasSvcCfg(
                flags, name="BeamHaloMinBiasSvc", kind=PUBkgKind.BEAMHALO
            )
        )
        kwargs.setdefault("BeamHaloMinbiasSvc", svc)
        kwargs.setdefault("NumBeamHalo", flags.Digitization.PU.NumberOfBeamHalo)

    kwargs.setdefault("BCSpacing", flags.Digitization.PU.BunchSpacing)
    kwargs.setdefault("EarliestDeltaBC", flags.Digitization.PU.InitialBunchCrossing)
    kwargs.setdefault("LatestDeltaBC", flags.Digitization.PU.FinalBunchCrossing)
    if flags.Input.RunAndLumiOverrideList:
        acc.merge(LumiProfileSvcCfg(flags))
        kwargs.setdefault("BeamLumiSvc", acc.getService("LumiProfileSvc"))
        kwargs.setdefault("AverageMu", maxNevtsPerXing(flags))
    else:
        acc.merge(NoProfileSvcCfg(flags))
        kwargs.setdefault("BeamLumiSvc", acc.getService("NoProfileSvc"))
        kwargs.setdefault("AverageMu", flags.Digitization.PU.NumberOfCollisions)
    presampling = flags.Common.ProductionStep == ProductionStep.PileUpPresampling
    if presampling:
        kwargs.setdefault("EventInfoKey", flags.Overlay.BkgPrefix + "EventInfo")
    else:
        kwargs.setdefault("EventInfoKey", "EventInfo")

    acc.addEventAlgo(
        CompFactory.PileUpMTAlg(flags.Digitization.DigiSteeringConf, **kwargs)
    )

    # write PileUpEventInfo
    if flags.Output.doWriteRDO:
        from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg

        acc.merge(
            OutputStreamCfg(
                flags,
                "RDO",
                ItemList=[
                    "xAOD::EventInfoContainer#PileUpEventInfo",
                    "xAOD::EventInfoAuxContainer#PileUpEventInfo*",
                ],
            )
        )

    return acc
