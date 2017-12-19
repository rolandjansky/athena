#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""Class to interact with RUCIO."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import atexit
import json
import logging
import os
import re
import shutil
import tarfile
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
    JOB_TAR = 'job.tar'
    JSON = '_EXT0'
    LOG = '.log'
    LOG_TGZ = 'log.tgz'
    OUTPUT = '_EXT1'

    def __init__(self):
        """Keep arguments."""
        pass

    def exit_if_no_rucio(self):
        """Exit if RUCIO is not available."""
        log = logging.getLogger(MODULE)
        if not RUCIO:
            log.critical("RUCIO not available")
            exit(1)

    def get_scope(self, user):
        """Return scope."""
        return '.'.join(('user', user))

    def download(self, did, dst_dir):
        """Download did into temp directory."""
        log = logging.getLogger(MODULE)
        self.exit_if_no_rucio()

        # rucio downloads cache properly
        (exit_code, out, err) = run_command("rucio download --dir " + dst_dir + " " + did)
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
        (exit_code, out, err) = run_command(cmd, verbose=False)
        if exit_code != 0:
            log.error(err)
        # seems to write empty lines
        # log.info(out)
        return exit_code

    def get_outfile(self, user, package, nightly_release, project, platform, nightly_tag, sequence_tag='*', test_name=None):
        """Create outfile from parameters."""
        if nightly_tag is None:
            nightly_tag = self.nightly_tag

        outfile = '.'.join(('user', user, 'atlas', nightly_release, project, platform, nightly_tag, sequence_tag, package))
        if sequence_tag == '*':
            self.exit_if_no_rucio()
            rucio_client = rucio.client.Client()
            for out in rucio_client.list_dids(self.get_scope(user), {'name': '.'.join((outfile, 'log'))}):
                outfile = os.path.splitext(out)[0]
        return outfile if test_name is None else '.'.join((outfile, test_name))

    # private
    def get_rucio_map(self, user, outfile):
        """Return map of entries by grid_index into { source, rucio_name }."""
        log = logging.getLogger(MODULE)
        log.debug("Looking for %s", outfile)
        self.exit_if_no_rucio()

        CERN = 'CERN-PROD_SCRATCHDISK'

        LOG_PATTERN = r"\.(\d{6})\.log\.tgz"
        JSON_PATTERN = r"\._(\d{6})\.art-job\.json"
        OUTPUT_PATTERN = r"\._(\d{6})\.tar"
        table = {}
        rucio_client = rucio.client.Client()
        for rep in rucio_client.list_replicas([{'scope': self.get_scope(user), 'name': outfile}], schemes=['root']):
            source = None
            rucio_name = None
            log.debug("Found in %s", rep['states'].keys())
            # first look at CERN
            if CERN in rep['states'].keys() and rep['states'][CERN] == 'AVAILABLE':
                source = CERN
                rucio_name = rep['rses'][CERN][0]
            else:
                for rse in rep['states'].keys():
                    if rep['states'][rse] == 'AVAILABLE' and len(rep['rses'][rse]) >= 1:
                        source = rse
                        rucio_name = rep['rses'][rse][0]
                        break

            # maybe not found at all
            if rucio_name is not None:
                log.debug("Found rucio name %s in %s", rucio_name, source)
                pattern = JSON_PATTERN if outfile.endswith(ArtRucio.JSON) else LOG_PATTERN if outfile.endswith(ArtRucio.LOG) else OUTPUT_PATTERN
                match = re.search(pattern, rucio_name)
                if match:
                    number = int(match.group(1))
                else:
                    log.warning("%s does not contain test number using pattern %s skipped...", rucio_name, pattern)
                    continue

                table[number] = {'source': source, 'rucio_name': rucio_name}

        if not table:
            log.warning("Outfile %s not found or empty", outfile)
        return table

    def get_index_map(self, user, sequence_tag, package, nightly_release, project, platform, nightly_tag):
        """Return grid map of job_name to index."""
        outfile = self.get_outfile(user, package, nightly_release, project, platform, nightly_tag, sequence_tag)

        # if outfile in self.index_map_cache:
        #    return self.index_map_cache[outfile]

        result = {}
        for index in self.get_indices(user, outfile + ArtRucio.LOG):
            test_name = self.get_job_name(user, index, package, sequence_tag, nightly_release, project, platform, nightly_tag)
            if test_name is None:
                # log.warning("JSON Lookup failed for test %s", rucio_log_name if rucio_name is None else rucio_name)
                continue

            result[test_name] = int(index)

        # self.index_map_cache[outfile] = result
        return result

    def get_rucio_name(self, user, outfile, index):
        """Return rucio name for given outfile and index."""
        rucio_map = self.get_rucio_map(user, outfile)
        return rucio_map[index]['rucio_name'] if index in rucio_map else None

    def get_indices(self, user, outfile):
        """Return list of indices."""
        return self.get_rucio_map(user, outfile).keys()

    def get_job_name(self, user, index, package, sequence_tag, nightly_release, project, platform, nightly_tag):
        """
        Return job name for index.

        job_name is without .sh or .py
        """
        log = logging.getLogger(MODULE)
        self.exit_if_no_rucio()

        outfile = self.get_outfile(user, package, nightly_release, project, platform, nightly_tag, sequence_tag)
        log.debug("outfile %s", outfile)

        tmp_dir = tempfile.mkdtemp()
        atexit.register(shutil.rmtree, tmp_dir)

        tmp_json = os.path.join(tmp_dir, ArtRucio.ART_JOB)
        rucio_name = self.get_rucio_name(user, outfile + ArtRucio.JSON, index)
        if self.xrdcp(rucio_name, tmp_json, force=True) == 0:
            log.debug("copied json %s", rucio_name)
            with open(tmp_json) as json_file:
                info = json.load(json_file)
                job_name = os.path.splitext(info['name'])[0]
                return job_name

        tmp_log = os.path.join(tmp_dir, ArtRucio.LOG_TGZ)
        rucio_log_name = self.get_rucio_name(user, outfile + ArtRucio.LOG, index)
        if self.xrdcp(rucio_log_name, tmp_log, force=True) == 0:
            log.debug("copied log %s %s", rucio_log_name, tmp_log)
            tar = tarfile.open(tmp_log)
            for name in tar.getnames():
                if ArtRucio.ATHENA_STDOUT in name:
                    log.debug("Found %s", ArtRucio.ATHENA_STDOUT)
                    info = tar.extractfile(name).read()
                    # try art-job-name
                    match = re.search(r"art-job-name:\s(\S+)", info)
                    if match:
                        log.debug("Found 'art-job-name'")
                        return os.path.splitext(match.group(1))[0]

                    # try Job Name
                    match = re.search(r"Job Name:\s(\S+)", info)
                    if match:
                        log.debug("Found 'Job Name:'")
                        return os.path.splitext(match.group(1))[0]

        log.debug("Cannot retrieve job_name from art-job.json or logfile")
        return None

    def get_index(self, user, sequence_tag, package, job_name, nightly_release, project, platform, nightly_tag):
        """Return index for job_name."""
        index_map = self.get_index_map(user, sequence_tag, package, nightly_release, project, platform, nightly_tag)
        return index_map[job_name] if job_name in index_map else -1
