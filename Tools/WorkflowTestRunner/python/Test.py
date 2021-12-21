# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from enum import Enum
from logging import Logger
from os import environ
from pathlib import Path
from typing import List
from uuid import uuid4
import subprocess

from .Helpers import get_pwd, get_release_setup, list_changed_packages


class TestSetup:
    """Test setup."""

    def __init__(self, logger: Logger) -> None:
        self.logger = logger
        self.pwd = get_pwd()
        self.reference_run_path = Path("/tmp")
        self.diff_rules_path = Path()
        self.unique_ID = str(uuid4())
        self.disable_release_setup = False
        self.validation_only = False
        self.checks_only = False
        self.release_reference = ""
        self.release_validation = ""
        self.release_ID = "master"  # The following is not flexible enough, can probably be hardcoded: environ["AtlasVersion"][0:4]
        self.parallel_execution = False
        self.disable_output_checks = False
        self.custom_threads = None

    def setup_release(self, reference=None, validation=None) -> None:
        if reference and validation:
            self.release_reference = reference
            self.release_validation = validation
            self.logger.info(f"WARNING: You have specified a dedicated release as reference {reference} and as validation {validation} release.")
            self.logger.info("Your local setup area will not be considered!!!")
            self.logger.info("this option is mainly designed for comparing release versions!!")
        else:
            self.release_reference = get_release_setup(self.logger, self.disable_release_setup)
            self.release_validation = self.release_reference
            try:
                list_changed_packages(self.logger, self.disable_release_setup)
            except Exception:
                self.logger.warning("Cannot list changed packages...\n")


class WorkflowRun(Enum):
    Run2 = "Run2"
    Run3 = "Run3"
    Run4 = "Run4"

    def __str__(self):
        return self.name


class WorkflowType(Enum):
    FullSim = "FullSim"
    AF3 = "AF3"
    MCOverlay = "MCOverlay"
    DataOverlay = "DataOverlay"
    MCReco = "MCReco"
    MCPileUpReco = "MCPileUpReco"
    DataReco = "DataReco"
    PileUpPresampling = "PileUpPresampling"

    def __str__(self):
        return self.name


class WorkflowCheck:
    """Workflow check base class."""

    def __init__(self, setup: TestSetup) -> None:
        self.setup = setup
        self.logger = setup.logger


class WorkflowTest:
    """Workflow test base class."""

    def __init__(self, ID: str, run: WorkflowRun, type: WorkflowType, steps: List[str], setup: TestSetup) -> None:
        if not hasattr(self, "ID"):
            self.ID = ID

        if not hasattr(self, "tag"):
            self.tag = ID

        if not hasattr(self, "steps"):
            self.steps = steps

        if not self.command:
            raise NotImplementedError("Command needs to be defined")

        if not hasattr(self, "output_checks"):
            self.output_checks = []

        if not hasattr(self, "digest_checks"):
            self.digest_checks = []

        if not hasattr(self, "skip_performance_checks"):
            self.skip_performance_checks = False

        self.run = run
        self.type = type
        self.setup = setup
        self.logger = setup.logger
        self.validation_path: Path = Path(f"run_{self.ID}")
        self.reference_path: Path = self.setup.reference_run_path / self.validation_path

    def run_reference(self) -> None:
        self.logger.info(f"Running reference in rel {self.setup.release_reference}")
        self.logger.info(f"\"{self.command}\"")

        self.reference_path.mkdir(parents=True, exist_ok=True)

        cmd = (f"cd {self.reference_path};"
               f"source $AtlasSetup/scripts/asetup.sh {self.setup.release_reference} >& /dev/null;")
        cmd += f"{self.command} > {self.ID}.log 2>&1"

        subprocess.call(cmd, shell=True)

        self.logger.info(f"Finished clean in rel {self.setup.release_reference}")
        self.logger.info(f"\"{self.command}\"")

    def run_validation(self) -> None:
        self.logger.info(f"Running validation in rel {self.setup.release_validation}")
        self.logger.info(f"\"{self.command}\"")

        self.validation_path.mkdir(parents=True, exist_ok=True)

        cmd = f"cd {self.setup.pwd};"
        if self.setup.disable_release_setup:
            pass
        elif "WorkDir_DIR" in environ:
            cmake_build_dir = environ["WorkDir_DIR"]
            cmd += (f"source $AtlasSetup/scripts/asetup.sh {self.setup.release_validation} >& /dev/null;"
                    f"source {cmake_build_dir}/setup.sh;")
        else:
            cmd += f"source $AtlasSetup/scripts/asetup.sh {self.setup.release_validation} >& /dev/null;"
        cmd += f"cd run_{self.ID};"
        cmd += f"{self.command} > {self.ID}.log 2>&1"

        subprocess.call(cmd, shell=True)

        self.logger.info(f"Finished validation in rel {self.setup.release_validation}")
        self.logger.info(f"\"{self.command}\"")

    def run_checks(self, main_check: WorkflowCheck, fpe_check: WorkflowCheck, performance_checks: List[WorkflowCheck]) -> bool:
        self.logger.info("-----------------------------------------------------")
        self.logger.info(f"----------- Post-processing of {self.ID} Test -----------")
        result = True

        # HAZ: Open question -- is there a cleaner way to do this?
        # HAZ: adding a decorator to `logging` would be nicest (require 0 errors)...
        if not main_check.run(self):
            return False

        # FPE check
        if fpe_check:
            result = fpe_check.run(self) and result

        # digest checks
        for check in self.digest_checks:
            result = check.run(self) and result

        # output checks
        if not self.setup.disable_output_checks:
            for check in self.output_checks:
                result = check.run(self) and result

        if self.setup.validation_only or self.skip_performance_checks:
            return result  # Performance checks against static references not possible

        # performance checks
        for check in performance_checks:
            result = check.run(self) and result

        return result
