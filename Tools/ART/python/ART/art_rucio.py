#!/usr/bin/env python
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
"""Class to interact with RUCIO."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import json
import logging
import os
import re
import tempfile

try:
    import rucio.client
    RUCIO = True
except ImportError:
    RUCIO = False

from art_misc import run_command

MODULE = "art.rucio"


class ArtRucio(object):
    """Class to interact with RUCIO."""

    ART_JOB = 'art-job.json'
    ATHENA_STDOUT = 'athena_stdout.txt'
    JSON = '_EXT0'

    def __init__(self, art_directory, nightly_release, project, platform, nightly_tag):
        """Keep arguments."""
        self.art_directory = art_directory
        self.nightly_release = nightly_release
        self.project = project
        self.platform = platform
        self.nightly_tag = nightly_tag

        self.table = None

    def exit_if_no_rucio(self):
        """Exit if RUCIO is not available."""
        log = logging.getLogger(MODULE)
        if not RUCIO:
            log.critical("RUCIO not available")
            exit(1)

    def get_scope(self, user):
        """Return scope."""
        return '.'.join(('user', user))

    def download(self, did, dst_dir, shell=False):
        """Download did into temp directory."""
        log = logging.getLogger(MODULE)
        self.exit_if_no_rucio()

        # rucio downloads cache properly
        log.debug("DID = %s", did)
        log.debug("Shell = %s", shell)
        env = os.environ.copy()
        if shell:
            cmd = ' '.join((os.path.join(self.art_directory, 'art-download.sh'), did, dst_dir))
            env['PATH'] = '.:' + env['PATH']
        else:
            cmd = ' '.join(('rucio', 'download', '--dir', dst_dir, did))

        (exit_code, out, err, command, start_time, end_time) = run_command(cmd, env=env)
        if (exit_code != 0):
            log.error(err)
        log.info(out)
        return exit_code

    def xrdcp(self, src, dst, force=False, recursive=False, verbose=False):
        """Copy using xrdcp."""
        if src is None or dst is None:
            return 1
        log = logging.getLogger(MODULE)
        cmd = ' '.join(('xrdcp -N', '-f' if force else '', '-r' if recursive else '', '-v' if verbose else '', src, dst))
        log.debug(cmd)
        (exit_code, out, err, command, start_time, end_time) = run_command(cmd, verbose=False)
        if exit_code != 0:
            log.error(err)
        # seems to write empty lines
        # log.info(out)
        return exit_code

    def __parse_outfile(self, outfile):
        """Parse outfile and return tuple (sequence_tag, single_index) or None."""
        #
        # Matching: user.artprod.atlas.master.Athena.x86_64-slc6-gcc62-opt.2018-01-21T2301.284099.MuonRecRTT.6.log.13062437.000001.log.tgz
        #           user.artprod.atlas.master.Athena.x86_64-slc6-gcc62-opt.2018-01-21T2301.284099.MuonRecRTT.6
        #           user.artprod.atlas.master.Athena.x86_64-slc6-gcc62-opt.2018-01-19T2301.283573.TrigAnalysisTest
        #
        PATTERN = r"user\.([^\.]+)\.([^\.]+)\." + self.nightly_release + "\." + self.project + "\." + self.platform + "\." + self.nightly_tag + "\.(.+)"
        match = re.search(PATTERN, outfile)
        if not match:
            return None

        (user, experiment, rest) = match.groups()

        items = rest.split(".")
        sequence_tag = items[0] if len(items) > 0 else -1
        try:
            single_index = int(items[2]) if len(items) > 2 else -1
        except ValueError:
            single_index = -1

        if single_index < 0:
            grid_index = int(items[4]) if len(items) > 4 else -1
        else:
            grid_index = int(items[5]) if len(items) > 5 else -1

        # print outfile, sequence_tag, single_index, grid_index

        return (sequence_tag, single_index, grid_index)

    def get_sequence_tag(self, outfile):
        """Return sequence tag or None."""
        result = self.__parse_outfile(outfile)
        return result[0] if result is not None else None

    def get_single_index(self, outfile):
        """Return single index or -1."""
        result = self.__parse_outfile(outfile)
        return result[1] if result is not None else -1

    def get_grid_index(self, outfile):
        """Return frid index or -1."""
        result = self.__parse_outfile(outfile)
        return result[2] if result is not None else -1

    def get_outfile_name(self, user, package, sequence_tag, test_name=None, nightly_tag=None):
        """Create outfile name based on parameters."""
        nightly_tag = self.nightly_tag if nightly_tag is None else nightly_tag
        outfile = '.'.join(('user', user, 'atlas', self.nightly_release, self.project, self.platform, nightly_tag, sequence_tag, package))
        return outfile if test_name is None else '.'.join((outfile, test_name))

    def get_outfiles(self, user, package, nightly_tag=None):
        """
        Create list of outfiles from parameters.

        example: ['user.artprod.atlas.master.Athena.x86_64-slc6-gcc62-opt.2018-01-21T2301.284099.MuonRecRTT.3']
        """
        log = logging.getLogger(MODULE)
        nightly_tag = self.nightly_tag if nightly_tag is None else nightly_tag

        self.exit_if_no_rucio()
        rucio_client = rucio.client.Client()

        result = []

        # look for "batch" outfile, and take latest (by sequence tag)
        pattern = self.get_outfile_name(user, package, '*', None, nightly_tag)
        outfile = None
        sequence = None
        log.debug("Pattern 1 %s", pattern)
        for out in rucio_client.list_dids(self.get_scope(user), {'name': '.'.join((pattern, 'log'))}):
            sequence_tag = self.get_sequence_tag(out)
            if sequence is None or sequence_tag > sequence:
                outfile = os.path.splitext(out)[0]
                sequence = sequence_tag

        if outfile is not None:
            result.append(outfile)

        # look for "single" outfile, deduce sequence_tag
        pattern = self.get_outfile_name(user, package, '*', '*', nightly_tag)
        outfile = None
        sequence = None
        log.debug("Pattern 2 %s", pattern)
        for out in rucio_client.list_dids(self.get_scope(user), {'name': '.'.join((pattern, 'log'))}):
            sequence_tag = self.get_sequence_tag(out)
            if sequence is None or sequence_tag > sequence:
                outfile = os.path.splitext(out)[0]
                sequence = sequence_tag

        if outfile is not None:
            log.debug("Found %s", outfile)
            sequence_tag = self.get_sequence_tag(outfile)
            if sequence_tag is not None:
                # found sequence_tag, find all 'single' outfiles
                pattern = self.get_outfile_name(user, package, sequence_tag, '*', nightly_tag)
                log.debug("Pattern 3 %s", pattern)
                for out in rucio_client.list_dids(self.get_scope(user), {'name': '.'.join((pattern, 'log'))}):
                    outfile = os.path.splitext(out)[0]
                    log.debug("Adding 'single': %s", outfile)
                    result.append(outfile)

        return result

    def get_table(self, user, package, nightly_tag=None, shell=False, tmp=None):
        """Get full table with grid_index, single_index and test_name for particular package and nightly_tag."""
        log = logging.getLogger(MODULE)

        if self.table is not None:
            return self.table

        self.exit_if_no_rucio()

        tmp = tempfile.gettempdir() if tmp is None else tmp

        table = []

        nightly_tag = self.nightly_tag if nightly_tag is None else nightly_tag

        outfiles = self.get_outfiles(user, package, nightly_tag)

        outfiles_str = [x + ArtRucio.JSON for x in outfiles]
        if outfiles_str:
            outfiles_str = ' '.join(outfiles_str)

            dst_dir = tmp

            log.debug("Shell = %s", shell)
            exit_code = self.download(outfiles_str, dst_dir, shell)
            if exit_code != 0:
                log.error("Failed to execute rucio download %d", exit_code)
                return table

            for outfile in outfiles:
                single_index = self.get_single_index(outfile)

                json_directory = os.path.join(dst_dir, outfile + ArtRucio.JSON)
                if not os.path.isdir(json_directory):
                    log.debug("Adding, single_index: %d, outfile: %s", single_index, outfile)
                    table.append({
                        'single_index': single_index,
                        'grid_index': -1,
                        'file_index': -1,
                        'job_index': -1,
                        'outfile': outfile,
                        'job_name': None
                    })
                    continue

                for json_file in os.listdir(json_directory):
                    if json_file.endswith(".json"):
                        json_path = os.path.join(json_directory, json_file)
                        if os.path.isfile(json_path):
                            with open(json_path) as json_fd:
                                info = json.load(json_fd)
                                job_name = os.path.splitext(info['name'])[0]

                                # Match: user.artprod.13199077.EXT0._000002.art-job.json
                                # Match: user.artprod.13199077.EXT0._000003.art-job.json.4
                                # job_index = 13199077, grid_index = 3, file_index = 4
                                match = re.search(r"user\.([^\.]+)\.(\d+)\.EXT0\._(\d+)\.art-job.json(?:\.(\d+))?", json_file)
                                if match:
                                    job_index = int(match.group(2))
                                    grid_index = int(match.group(3))
                                    file_index = -1 if match.group(4) is None else int(match.group(4))
                                else:
                                    job_index = -1
                                    grid_index = -1
                                    file_index = -1

                                log.debug("Adding, single_index: %d, grid_index: %d, file_index: %d, job_index %d, outfile: %s, job_name: %s", single_index, grid_index, file_index, job_index, outfile, job_name)
                                table.append({
                                    'single_index': single_index,
                                    'grid_index': grid_index,
                                    'file_index': file_index,
                                    'job_index': job_index,
                                    'outfile': outfile,
                                    'job_name': job_name
                                })

        self.table = table
        return table
