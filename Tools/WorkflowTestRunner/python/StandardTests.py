# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from typing import List

from .Checks import FrozenTier0PolicyCheck
from .Inputs import input_EVNT, input_HITS, \
    input_HITS_MC_overlay, input_RDO_BKG, \
    input_HITS_data_overlay, input_BS_SKIM, \
    input_HITS_minbias_low, input_HITS_minbias_high, input_HITS_neutrino
from .Test import TestSetup, WorkflowRun, WorkflowTest, WorkflowType


class QTest(WorkflowTest):
    """General workflow q-test."""

    def __init__(self, ID: str, run: WorkflowRun, type: WorkflowType, steps: List[str], setup: TestSetup, extra_args: str = '') -> None:
        if "maxEvents" not in extra_args:
            if type == WorkflowType.MCPileUpReco or run == WorkflowRun.Run4:
                extra_args += " --maxEvents 5"
            else:
                extra_args += " --maxEvents 20"

        if "input" not in extra_args and type == WorkflowType.MCPileUpReco:
            extra_args += f" --inputHITSFile {input_HITS[run]} --inputRDO_BKGFile ../run_d*/myRDO.pool.root"

        self.command = \
            (f"ATHENA_CORE_NUMBER=1 Reco_tf.py --multithreaded --AMIConfig {ID}"
             f" --imf False {extra_args}")

        self.output_checks = []
        # TODO: disable RDO comparison for now
        # if type == WorkflowType.MCReco:
        #     self.output_checks.append(FrozenTier0PolicyCheck(setup, "RDO", 10))
        self.output_checks.append(FrozenTier0PolicyCheck(setup, "ESD", 10))
        self.output_checks.append(FrozenTier0PolicyCheck(setup, "AOD", 20))

        super().__init__(ID, run, type, steps, setup)


class SimulationTest(WorkflowTest):
    """Simulation workflow test."""

    def __init__(self, ID: str, run: WorkflowRun, type: WorkflowType, steps: List[str], setup: TestSetup, extra_args: str = '') -> None:
        if "maxEvents" not in extra_args:
            extra_args += " --maxEvents 20"

        self.command = \
            (f"Sim_tf.py --AMIConfig {ID}"
             f" --inputEVNTFile {input_EVNT[run]} --outputHITSFile myHITS.pool.root"
             f" --imf False {extra_args}")

        self.output_checks = [
            FrozenTier0PolicyCheck(setup, "HITS", 10)
        ]

        super().__init__(ID, run, type, steps, setup)


class OverlayTest(WorkflowTest):
    """MC overlay workflow test."""

    def __init__(self, ID: str, run: WorkflowRun, type: WorkflowType, steps: List[str], setup: TestSetup, extra_args: str = '') -> None:
        if "maxEvents" not in extra_args:
            extra_args += " --maxEvents 10"

        self.command = \
            (f"Overlay_tf.py --AMIConfig {ID}"
             f" --inputHITSFile {input_HITS_MC_overlay[run]} --inputRDO_BKGFile {input_RDO_BKG[run]} --outputRDOFile myRDO.pool.root"
             f" --imf False --athenaopts=\"--pmon=sdmonfp\" {extra_args}")

        # skip performance checks for now due to CA
        self.skip_performance_checks = True

        self.output_checks = [
            FrozenTier0PolicyCheck(setup, "RDO", 10)
        ]

        super().__init__(ID, run, type, steps, setup)


class DataOverlayTest(WorkflowTest):
    """Data overlay workflow test."""

    def __init__(self, ID: str, run: WorkflowRun, type: WorkflowType, steps: List[str], setup: TestSetup, extra_args: str = '') -> None:
        if "maxEvents" not in extra_args:
            extra_args += " --maxEvents 10"

        self.command = \
            (f"Overlay_tf.py --AMIConfig {ID}"
             f" --inputHITSFile {input_HITS_data_overlay[run]} --inputBS_SKIMFile {input_BS_SKIM[run]} --outputRDOFile myRDO.pool.root"
             f" --imf False --athenaopts=\"--pmon=sdmonfp\" {extra_args}")

        self.output_checks = [
            FrozenTier0PolicyCheck(setup, "RDO", 10)
        ]

        super().__init__(ID, run, type, steps, setup)


class PileUpTest(WorkflowTest):
    """Digitization with pile-up workflow test."""

    def __init__(self, ID: str, run: WorkflowRun, type: WorkflowType, steps: List[str], setup: TestSetup, extra_args: str = '') -> None:
        if "maxEvents" not in extra_args:
            extra_args += " --maxEvents 5"

        self.command = \
            (f"Digi_tf.py --AMIConfig {ID} --jobNumber 1 --digiSeedOffset1 1 --digiSeedOffset2 1"
             f" --inputHITSFile {input_HITS_neutrino[run]} --inputHighPtMinbiasHitsFile {input_HITS_minbias_high[run]} --inputLowPtMinbiasHitsFile {input_HITS_minbias_low[run]} --outputRDOFile myRDO.pool.root"
             f" --imf False --athenaopts=\"--pmon=sdmonfp\" {extra_args}")

        self.output_checks = [
            FrozenTier0PolicyCheck(setup, "RDO", 5)
        ]

        super().__init__(ID, run, type, steps, setup)
