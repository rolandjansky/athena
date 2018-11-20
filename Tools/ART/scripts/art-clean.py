#!/usr/bin/env python
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
"""
ART  - ATLAS Release Tester - Clean.

Usage:
  art-clean.py [-v -q --base-dir=<base_dir> --delete --days=<days> --eos --config=<file> --release --package=<package>] [<nightly_release> <project> [<platform>]]

Options:
  --base-dir=<base_dir>      Start search from basedir [default: /eos/atlas/atlascerngroupdisk/data-art/grid-output]
  --config=<file>            art-configuration to retrieve packages and days [default: art-configuration.yml]
  --delete                   Actually delete the directories to be cleaned
  --days=<days>              Number of nighlies to keep [default: 7]
  --eos                      Use eos commands
  -h --help                  Show this screen
  --package=<package>        Package to be cleaned up, no configuration
  --release                  Clean up full release, no configuration
  -q --quiet                 Show less information, only warnings and errors
  -v --verbose               Show more information, debug level
  --version                  Show version

Arguments:
  nightly_release            Name of the nightly release (e.g. 21.0)
  project                    Project to clean (e.g. Athena)
  platform                   Platform to clean [default: x86_64-slc6-gcc62-opt]

Environment:
  AtlasBuildBranch          Name of the nightly release (e.g. 21.0)
  AtlasProject              Name of the project (e.g. Athena)
  <AtlasProject>_PLATFORM   Platform (e.g. x86_64-slc6-gcc62-opt)
"""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import datetime
import logging
import os
import re
import shutil
import sys

from ART.docopt import docopt
from ART.art_misc import get_atlas_env, run_command
from ART.art_configuration import ArtConfiguration

MODULE = "art.clean"


class ArtClean(object):
    """Class to cleanup eos area."""

    EOS_MGM_URL = 'root://eosatlas.cern.ch'

    def __init__(self, arguments):
        """Clean when more than 'days' old."""
        log = logging.getLogger(MODULE)
        self.verbose = arguments['--verbose']
        self.eos = arguments['--eos']
        default_days = int(arguments['--days'])
        self.delete = arguments['--delete']
        nightly_release = arguments['<nightly_release>']
        project = arguments['<project>']
        platform = arguments['<platform>']
        base_dir = arguments['--base-dir']

        # Check for missing arguments
        if nightly_release is None:
            (nightly_release, project, platform, dummy) = get_atlas_env()

        if platform is None:
            platform = 'x86_64-slc6-gcc62-opt'
            log.info("Defaulting to platform %s", platform)

        if self.eos:
            # Test if we have access to kerberos
            (code, out, err, command, start_time, end_time) = run_command('klist', verbose=self.verbose)
            if code != 0:
                log.critical("%s", err)
                exit(1)

        package = arguments['--package']
        if package is not None:
            self.clean_release(os.path.join(base_dir, nightly_release, project, platform), default_days, package)
            return

        if arguments['--release']:
            self.clean_release(os.path.join(base_dir, nightly_release, project, platform), default_days)
            return

        config_file = arguments['--config']
        config = ArtConfiguration(config_file)
        for package in config.packages():
            copy = config.get(nightly_release, project, platform, package, 'copy', False)
            if copy:
                days = config.get(nightly_release, project, platform, package, 'days', default_days)
                self.clean_release(os.path.join(base_dir, nightly_release, project, platform), days, package)
        return

    def clean_release(self, release, days, package=None):
        """Clean a release dir."""
        log = logging.getLogger(MODULE)
        log.debug("Starting to clean up release, keeping %d days, for %s in %s", days, "All" if package is None else package, release)
        now = datetime.datetime.now()
        count = 0
        date = '1970-01-01'
        for entry in reversed(self.listdirs(release)):
            # Matches 2018-12-05T0345
            match = re.match(r"(\d{4}-\d{2}-\d{2})T\d{4}", entry)
            if match:
                tag = os.path.join(release, entry)
                # package_dir = tag for full release cleanup
                package_dir = tag if package is None else os.path.join(tag, package)

                # only count entries on different dates and where package actually exist
                if self.isdir(package_dir) and match.group(1) != date:
                    count += 1
                    date = match.group(1)

                if count > days:
                    # compare times
                    dir_time = datetime.datetime.strptime(match.group(0), '%Y-%m-%dT%H%M')
                    time_diff = now - dir_time
                    if time_diff.days > days:
                        log.info("- Nightly tag %d days old, removing %s for package %s", time_diff.days, package_dir, package if package is not None else 'All')
                        self.remove_dir(package_dir)
                        if package is not None and self.isempty(tag):
                            log.info("- Nightly tag contains no more packages, removing %s for package %s", tag, package if package is not None else 'All')
                            self.remove_dir(tag)
                    else:
                        log.debug("- Tag within %d days, keeping %s for package %s", days, tag, package if package is not None else 'All')
                else:
                    log.debug("- Tag within %d entries, keeping %s for package %s", days, tag, package if package is not None else 'All')

        if package is not None and count == 0:
            log.debug("No package %s found in any available nightly tag for %s", package, release)

    def listdirs(self, directory):
        """Return list of directories in directory."""
        dirs = []
        if self.isdir(directory):
            for entry in self.listdir(directory):
                path = os.path.join(directory, entry)
                if self.isdir(path):
                    dirs.append(entry)
        return dirs

    def isempty(self, directory):
        """Return true if directory is empty."""
        if self.isdir(directory):
            for entry in self.listdir(directory):
                return False
            return True

    def listdir(self, directory):
        """Return list of entries in directory."""
        log = logging.getLogger(MODULE)
        if self.eos:
            (code, out, err, command, start_time, end_time) = run_command('eos ' + ArtClean.EOS_MGM_URL + ' ls ' + directory, verbose=self.verbose)
            if code == 0:
                return out.splitlines()
            log.info("eos listdir %d %s", code, err)
            return []
        else:
            return os.listdir(directory)

    def isdir(self, path):
        """Return true is path is directory."""
        log = logging.getLogger(MODULE)
        if self.eos:
            (code, out, err, command, start_time, end_time) = run_command('eos ' + ArtClean.EOS_MGM_URL + ' stat -d ' + path, verbose=self.verbose)
            if code == 0:
                return True
            log.debug("eos isdir(%s) %d %s", path, code, err)
            return False
        else:
            return os.path.isdir(path)

    def remove_dir(self, directory):
        """Remove directory and all below."""
        log = logging.getLogger(MODULE)
        if not self.isdir(directory):
            return

        if self.delete:
            if self.eos:
                (code, out, err, command, start_time, end_time) = run_command('eos ' + ArtClean.EOS_MGM_URL + ' rm -r ' + directory, verbose=self.verbose)
                if code == 0:
                    return True
                log.info("eos rm -r (%s) %d %s", directory, code, err)
            else:
                shutil.rmtree(directory)


if __name__ == '__main__':
    if sys.version_info < (2, 7, 0):
        sys.stderr.write("You need python 2.7 or later to run this script\n")
        exit(1)

    # NOTE: import should be here, to keep the order of the decorators (module first, art last and unused)
    from art import __version__

    logging.basicConfig()
    log = logging.getLogger('art')

    arguments = docopt(__doc__, version=os.path.splitext(os.path.basename(__file__))[0] + ' ' + __version__)
    level = logging.DEBUG if arguments['--verbose'] else logging.WARN if arguments['--quiet'] else logging.INFO
    log.setLevel(level)
    ArtClean(arguments)
