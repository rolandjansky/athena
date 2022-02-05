# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from glob import glob
from logging import Logger
from os import environ, path
from pathlib import Path
from typing import List


def get_pwd() -> Path:
    return Path.cwd()


def get_release_setup(logger: Logger, no_setup=False) -> str:
    """Get release setup."""
    if no_setup:
        logger.info("No release information is available when a release is not set-up.\n")
        return ""

    current_nightly = environ["AtlasBuildStamp"]
    release_base = environ["AtlasBuildBranch"]
    release_head = environ["AtlasVersion"]
    platform = environ["LCG_PLATFORM"]
    project = environ["AtlasProject"]
    builds_dir_search_str = f"/cvmfs/atlas-nightlies.cern.ch/repo/sw/{release_base}_{project}_{platform}/[!latest_]*/{project}/{release_head}"
    # finds all directories matching above search pattern, and sorts by modification time
    # suggest to use latest opt over dbg
    sorted_list = sorted(glob(builds_dir_search_str), key=path.getmtime)
    latest_nightly = ""
    for folder in reversed(sorted_list):
        if not glob(f"{folder}/../../{release_base}__{project}*-opt*.log"):
            continue
        latest_nightly = folder.split("/")[-3]
        break

    if current_nightly != latest_nightly:
        logger.info(f"Please be aware that you are not testing your tags in the latest available nightly, which is {latest_nightly}")

    setup = "%s,%s,%s,Athena" % (release_base, platform.replace("-", ","), current_nightly)

    logger.info(f"Your tags will be tested in environment {setup}")

    return setup


def list_changed_packages(logger: Logger, no_setup=False) -> None:
    """List packages that have changed."""
    if no_setup:
        logger.info("The list of changed packages is not available when the relase is not set-up.\n")
        return

    if "WorkDir_DIR" in environ:
        logger.info("Changed packages in your build to be tested:\n")
        file_path = Path(environ["WorkDir_DIR"])
        fname = file_path / "packages.txt"
        with fname.open() as fp:
            lines = fp.readlines()
            last_line = lines[-1].strip() if lines else None
            for line in lines:
                line = line.strip()
                if "#" not in line:
                    if line == last_line:
                        logger.info(f"{line}\n")
                    else:
                        logger.info(line)
    else:
        logger.warning("A release area with locally installed packages has not been setup.")
        logger.warning("quit by executing <CONTROL-C> if this is not your intention, and")
        logger.warning("source <YOUR_CMake_BUILD_AREA>/setup.sh")
        logger.warning("to pickup locally built packages in your environment.\n")
    pass


def warnings_count(file_name: Path) -> List[str]:
    """Run a WARNING helper function."""
    warnings = []
    with file_name.open() as file:
        for line in file:
            if "WARNING" in line and "| WARNING |" not in line:
                warnings.append(line)
    return warnings
