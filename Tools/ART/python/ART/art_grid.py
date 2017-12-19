#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""TBD."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import atexit
import datetime
import glob
import json
import logging
import os
import re
import shutil
import sys
import tarfile
import tempfile
import time
import urllib2

try:
    import rucio.client
    RUCIO = True
except ImportError:
    # NOTE: defer logging as level is not set yet
    RUCIO = False

from art_base import ArtBase
from art_header import ArtHeader
from art_misc import mkdir_p, make_executable, run_command

MODULE = "art.grid"


class ArtGrid(ArtBase):
    """TBD."""

    CVMFS_DIRECTORY = '/cvmfs/atlas-nightlies.cern.ch/repo/sw'
    EOS_MGM_URL = 'root://eosatlas.cern.ch/'
    EOS_OUTPUT_DIR = '/eos/atlas/atlascerngroupdisk/data-art/grid-output'

    LOG = '.log'
    JSON = '_EXT0'
    OUTPUT = '_EXT1'

    ARTPROD = 'artprod'
    ART_JOB = 'art-job.json'
    LOG_TGZ = 'log.tgz'
    JOB_TAR = 'job.tar'
    JOB_REPORT = 'jobReport.json'
    JOB_REPORT_ART_KEY = 'art'

    ATHENA_STDOUT = 'athena_stdout.txt'
    RESULT_WAIT_INTERVAL = 300

    def __init__(self, art_directory, nightly_release, project, platform, nightly_tag, script_directory=None, skip_setup=False, submit_directory=None):
        """TBD."""
        super(ArtGrid, self).__init__(art_directory)
        self.nightly_release = nightly_release
        self.project = project
        self.platform = platform
        self.nightly_tag = nightly_tag
        self.script_directory = script_directory
        self.skip_setup = skip_setup
        self.submit_directory = submit_directory
        self.rucio_cache = os.path.join(tempfile.gettempdir(), "rucio-cache")

    def status(self, status):
        """Print status for usage in gitlab-ci."""
        print 'art-status:', status

    def get_script_directory(self):
        """On demand script directory, only to be called if directory exists."""
        if self.script_directory is None:
            self.script_directory = ArtGrid.CVMFS_DIRECTORY
            self.script_directory = os.path.join(self.script_directory, self.nightly_release)  # e.g. 21.0
            self.script_directory = os.path.join(self.script_directory, self.nightly_tag)  # e.g. 2017-10-25T2150
            self.script_directory = os.path.join(self.script_directory, self.project)  # e.g. Athena
            try:
                self.script_directory = os.path.join(self.script_directory, os.listdir(self.script_directory)[0])  # e.g. 21.0.3
                self.script_directory = os.path.join(self.script_directory, os.listdir(self.script_directory)[0])  # InstallArea
            except OSError:
                self.script_directory = os.path.join(self.script_directory, '*', '*')
            self.script_directory = os.path.join(self.script_directory, self.platform)  # x86_64-slc6-gcc62-opt
        return self.script_directory

    def is_script_directory_in_cvmfs(self):
        """Return true if the script directory is in cvmfs."""
        return self.get_script_directory().startswith(ArtGrid.CVMFS_DIRECTORY)

    def exit_if_no_script_directory(self):
        """Exit with ERROR is script directory does not exist."""
        log = logging.getLogger(MODULE)
        if not os.path.isdir(self.get_script_directory()):
            log.critical('Script directory does not exist: %s', self.get_script_directory())
            self.status('error')
            exit(1)

    def copy_art(self, run_dir):
        """Copy all art files to the the run directory. Returns final script directory to be used."""
        log = logging.getLogger(MODULE)
        ART = os.path.join(run_dir, "ART")
        mkdir_p(ART)

        # get the path of the python classes and support scripts
        art_python_directory = os.path.join(self.art_directory, '..', 'python', 'ART')

        shutil.copy(os.path.join(self.art_directory, 'art.py'), run_dir)
        shutil.copy(os.path.join(self.art_directory, 'art-get-input.sh'), run_dir)
        shutil.copy(os.path.join(self.art_directory, 'art-get-tar.sh'), run_dir)
        shutil.copy(os.path.join(self.art_directory, 'art-internal.py'), run_dir)
        shutil.copy(os.path.join(art_python_directory, '__init__.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'art_base.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'art_build.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'art_grid.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'art_header.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'art_misc.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'docopt.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'docopt_dispatch.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'parallelScheduler.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'serialScheduler.py'), ART)

        make_executable(os.path.join(run_dir, 'art.py'))
        make_executable(os.path.join(run_dir, 'art-get-input.sh'))
        make_executable(os.path.join(run_dir, 'art-get-tar.sh'))
        make_executable(os.path.join(run_dir, 'art-internal.py'))

        script_directory = self.get_script_directory()

        # copy a local test directory if needed (only for 'art grid')
        if not self.is_script_directory_in_cvmfs():
            script_directory = os.path.basename(os.path.normpath(self.get_script_directory()))
            target_directory = os.path.join(run_dir, script_directory)
            log.info("Copying script directory for grid submission to %s", target_directory)
            shutil.copytree(self.get_script_directory(), target_directory)

        return script_directory

    def get_jedi_id(self, text):
        """Return Jedi Task Id or 0."""
        match = re.search(r"jediTaskID=(\d+)", text)
        return match.group(1) if match else 0

    def get_nightly_release_short(self):
        """Return  a short version of the nightly release."""
        return re.sub(r"-VAL-.*", "-VAL", self.nightly_release)

    def get_outfile(self, user, package, sequence_tag=0, test_name=None, nightly_tag=None):
        """Create outfile from parameters."""
        log = logging.getLogger(MODULE)

        if nightly_tag is None:
            nightly_tag = self.nightly_tag

        if sequence_tag == 0:
            if not RUCIO:
                log.critical("RUCIO not available")
                exit(1)

            scope = '.'.join(('user', user))
            outfile = '.'.join(('user', user, 'atlas', self.get_nightly_release_short(), self.project, self.platform, nightly_tag, '*', package, 'log'))
            rucio_client = rucio.client.Client()
            for out in rucio_client.list_dids(scope, {'name': outfile}):
                outfile = os.path.splitext(out)[0]
        else:
            outfile = '.'.join(('user', user, 'atlas', self.get_nightly_release_short(), self.project, self.platform, nightly_tag, sequence_tag, package))
        return outfile if test_name is None else '.'.join((outfile, test_name))

    def copy(self, package, dst=None, user=None):
        """Copy output from scratch area to eos area."""
        log = logging.getLogger(MODULE)
        real_user = os.getenv('USER', ArtGrid.ARTPROD)
        user = real_user if user is None else user
        default_dst = ArtGrid.EOS_OUTPUT_DIR if real_user == ArtGrid.ARTPROD else '.'
        dst = default_dst if dst is None else dst

        if package is not None:
            log.info("Copy %s", package)
            outfile = self.get_outfile(user, package)
            log.info("Copying from %s", outfile)

            return self.copy_output(outfile, dst)

        # make sure script directory exist
        self.exit_if_no_script_directory()

        # get the test_*.sh from the test directory
        test_directories = self.get_test_directories(self.get_script_directory())
        if not test_directories:
            log.warning('No tests found in directories ending in "test"')

        # copy results for all packages
        result = 0
        for package, root in test_directories.items():
            number_of_tests = len(self.get_files(root, "grid", "all", self.nightly_release, self.project, self.platform))
            if number_of_tests > 0:
                log.info("Copy %s", package)
                outfile = self.get_outfile(user, package)
                log.info("Copying from %s", outfile)

                result |= self.copy_output(outfile, dst)
        return result

    # Not used yet
    def download(self, did):
        """Download did into temp directory."""
        log = logging.getLogger(MODULE)
        if not RUCIO:
            log.critical("RUCIO not available")
            exit(1)

        # rucio downloads cache properly
        (exit_code, out, err) = run_command("rucio download --dir " + self.rucio_cache + " " + did)
        if (exit_code != 0):
            log.error(err)
        log.info(out)
        return exit_code

    # Not used yet
    def get_job_name(self, user, index, package, sequence_tag, nightly_tag):
        """
        Return job name for index.

        job_name is without .sh or .py
        """
        log = logging.getLogger(MODULE)
        if not RUCIO:
            log.critical("RUCIO not available")
            exit(1)

        outfile = self.get_outfile(user, package, sequence_tag=sequence_tag, nightly_tag=nightly_tag)
        log.debug("outfile %s", outfile)

        container_json = outfile + ArtGrid.JSON
        container_log = outfile + ArtGrid.LOG
        log.info("Downloading json")
        self.download(container_json)
        log.info("Downloading log")
        self.download(container_log)

        index_formatted = index
        indexed_json = os.path.join(container_json, '.'.join((container_json, sequence_tag, index_formatted, ArtGrid.JSON)))
        log.debug("Looking for json")
        if os.path.exists(indexed_json):
            with open(indexed_json) as json_file:
                info = json.load(json_file)
                test_name = os.path.splitext(info['name'])[0]
                return test_name

        indexed_log = os.path.join(container_log, '.'.join((container_log, sequence_tag, index_formatted, ArtGrid.LOG_TGZ)))
        log.debug("Looking for log")
        if os.path.exists(indexed_log):
            tar = tarfile.open(indexed_log)
            for name in tar.getnames():
                if ArtGrid.ATHENA_STDOUT in name:
                    log.debug("Found %s", ArtGrid.ATHENA_STDOUT)
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

        log.error("Cannot retrieve job_name from art-job.json or logfile")
        return None

    def get_test_name(self, rucio_name, rucio_log_name):
        """Return test_name for log rucio_name."""
        log = logging.getLogger(MODULE)
        if not RUCIO:
            log.critical("RUCIO not available")
            exit(1)

        tmp_dir = tempfile.mkdtemp()
        atexit.register(shutil.rmtree, tmp_dir)

        tmp_json = os.path.join(tmp_dir, ArtGrid.ART_JOB)

        if rucio_name is not None:
            (exit_code, out, err) = run_command(' '.join(('xrdcp -N -f ', rucio_name, tmp_json)))
            if exit_code == 0:
                log.debug("copied json %s", rucio_name)
                with open(tmp_json) as json_file:
                    info = json.load(json_file)
                    test_name = os.path.splitext(info['name'])[0]
                    return test_name

        tmp_log = os.path.join(tmp_dir, ArtGrid.LOG_TGZ)

        if rucio_log_name is not None:
            (exit_code, out, err) = run_command(' '.join(('xrdcp -N -f ', rucio_log_name, tmp_log)))
            if exit_code == 0:
                log.debug("copied log %s %s", rucio_log_name, tmp_log)
                tar = tarfile.open(tmp_log)
                for name in tar.getnames():
                    if ArtGrid.ATHENA_STDOUT in name:
                        log.debug("Found %s", ArtGrid.ATHENA_STDOUT)
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

    def copy_output(self, outfile, dst):
        """Copy outfile to dst."""
        log = logging.getLogger(MODULE)
        if not RUCIO:
            log.critical("RUCIO not available")
            exit(1)

        result = 0
        outfile_pattern = r"([^\.]+)\.([^\.]+)\.([^\.]+)\.(.+)\.([^\.]+)\.([^\.]+)\.([^\.]+)\.([^\.]+)\.([^\.\n]+)"
        match = re.search(outfile_pattern, outfile)
        if not match:
            log.error("%s does not match pattern", outfile)
            return 1
        (user_type, user, experiment, nightly_release, project, platform, nightly_tag, sequence_tag, package) = match.groups()
        dst_dir = os.path.join(dst, nightly_release, nightly_tag, project, platform, package)
        log.info(dst_dir)

        scope = '.'.join((user_type, user))

        tmp_dir = tempfile.mkdtemp()
        atexit.register(shutil.rmtree, tmp_dir)

        tmp_json = os.path.join(tmp_dir, ArtGrid.ART_JOB)
        tmp_log = os.path.join(tmp_dir, ArtGrid.LOG_TGZ)
        tmp_tar = os.path.join(tmp_dir, ArtGrid.JOB_TAR)

        jsons = self.get_rucio_map(scope, outfile, ArtGrid.JSON)
        logs = self.get_rucio_map(scope, outfile, ArtGrid.LOG)
        tars = self.get_rucio_map(scope, outfile, ArtGrid.OUTPUT)

        # log.debug(jsons)
        # log.debug(logs)

        for number in tars:
            # get the test name
            rucio_name = jsons[number]['rucio_name'] if number in jsons else None
            rucio_log_name = logs[number]['rucio_name'] if number in logs else None
            test_name = self.get_test_name(rucio_name, rucio_log_name)
            if test_name is None:
                log.error("JSON Lookup Error for test %s", rucio_name)
                result = 1
                continue

            # create tmp test directory
            test_dir = os.path.join(tmp_dir, test_name)
            mkdir_p(test_dir)

            # copy art-job in, ignore error
            run_command(' '.join(('xrdcp -N -f', rucio_name, tmp_json)))
            shutil.copyfile(tmp_json, os.path.join(test_dir, ArtGrid.ART_JOB))

            # copy and unpack log
            log_source = logs[number]['source']
            (exit_code, out, err) = run_command(' '.join(('xrdcp -N -f', rucio_log_name, tmp_log)))
            if exit_code != 0:
                log.error("Log Unpack Error: %d %s %s", exit_code, out, err)
                result = 1
            else:
                tar = tarfile.open(tmp_log)
                for member in tar.getmembers():
                    tar.extract(member, path=test_dir)
                # does not work: tar.extractall()
                tar.close()

            log.info("Copying: %d %s", number, test_name)
            log.info("- json: %s", jsons[number]['source'])
            log.info("- log:  %s", log_source)
            log.info("- tar:  %s", tars[number]['source'])

            # copy results and unpack
            (exit_code, out, err) = run_command(' '.join(('xrdcp -N -f', tars[number]['rucio_name'], tmp_tar)))
            if exit_code != 0:
                log.error("TAR Error: %d %s %s", exit_code, out, err)
                result = 1
            else:
                tar = tarfile.open(tmp_tar)
                tar.extractall(path=test_dir)
                tar.close()

            # copy to eos
            dst_target = os.path.join(dst_dir, test_name)
            log.info("to: %s", dst_target)
            if dst_target.startswith('/eos'):
                mkdir_cmd = 'eos ' + ArtGrid.EOS_MGM_URL + ' mkdir -p'
                xrdcp_target = ArtGrid.EOS_MGM_URL + dst_target
            else:
                mkdir_cmd = 'mkdir -p'
                xrdcp_target = dst_target

            (exit_code, out, err) = run_command(' '.join((mkdir_cmd, dst_target)))
            if exit_code != 0:
                log.error("Mkdir Error: %d %s %s", exit_code, out, err)
                result = 1
            else:
                (exit_code, out, err) = run_command(' '.join(('xrdcp -N -r -v', test_dir, xrdcp_target)))
                if exit_code not in [0, 51, 54]:
                    # 0 all is ok
                    # 51 File exists
                    # 54 is already copied
                    log.error("XRDCP to EOS Error: %d %s %s", exit_code, out, err)
                    result = 1

            # cleanup
            shutil.rmtree(test_dir)

        return result

    def get_rucio_map(self, scope, outfile, extension):
        """Return map of entries by grid_index into { source, rucio_name }."""
        log = logging.getLogger(MODULE)
        if not RUCIO:
            log.critical("RUCIO not available")
            exit(1)

        CERN = 'CERN-PROD_SCRATCHDISK'

        LOG_PATTERN = r"\.(\d{6})\.log\.tgz"
        JSON_PATTERN = r"\._(\d{6})\.art-job\.json"
        OUTPUT_PATTERN = r"\._(\d{6})\.tar"
        table = {}
        rucio_client = rucio.client.Client()
        log.debug("Looking for %s", outfile + extension)
        for rep in rucio_client.list_replicas([{'scope': scope, 'name': outfile + extension}], schemes=['root']):
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
                pattern = JSON_PATTERN if extension == ArtGrid.JSON else LOG_PATTERN if extension == ArtGrid.LOG else OUTPUT_PATTERN
                match = re.search(pattern, rucio_name)
                if match:
                    number = int(match.group(1))
                else:
                    log.warning("%s does not contain test number using pattern %s skipped...", rucio_name, pattern)
                    continue

                table[number] = {'source': source, 'rucio_name': rucio_name}

        if not table:
            log.warning("Outfile %s not found or empty", outfile + extension)
        return table

    def task_package(self, root, package, job_type, sequence_tag, no_action):
        """TBD."""
        log = logging.getLogger(MODULE)
        result = {}
        number_of_tests = len(self.get_files(root, job_type, "all", self.nightly_release, self.project, self.platform))
        if number_of_tests > 0:
            print 'art-package:', package
            self.status('included')
            log.info('root %s', root)
            log.info('Handling %s for %s project %s on %s', package, self.nightly_release, self.project, self.platform)
            log.info("Number of tests: %d", number_of_tests)
            submit_dir = os.path.join(self.submit_directory, package)
            run_dir = os.path.join(submit_dir, "run")

            script_directory = self.copy_art(run_dir)

            result = self.task(script_directory, package, job_type, sequence_tag, no_action)
        return result

    def task_list(self, job_type, sequence_tag, package=None, no_action=False, wait_and_copy=True):
        """TBD."""
        log = logging.getLogger(MODULE)
        # job will be submitted from tmp directory
        self.submit_directory = tempfile.mkdtemp(dir='.')

        # make sure tmp is removed afterwards
        atexit.register(shutil.rmtree, self.submit_directory)

        # make sure script directory exist
        self.exit_if_no_script_directory()

        # get the test_*.sh from the test directory
        test_directories = self.get_test_directories(self.get_script_directory())
        if not test_directories:
            log.warning('No tests found in directories ending in "test"')

        all_results = {}

        if package is None:
            config = None if self.skip_setup else self.get_config()
            excluded_packages = config.get('excluded-packages', []) if config is not None else []

            # submit tasks for all packages
            for package, root in test_directories.items():
                if package in excluded_packages:
                    log.warning("Package %s is excluded", package)
                else:
                    all_results.update(self.task_package(root, package, job_type, sequence_tag, no_action))
        else:
            # Submit single package
            root = test_directories[package]
            all_results.update(self.task_package(root, package, job_type, sequence_tag, no_action))

        # wait for all results
        if wait_and_copy:
            while len(all_results) > 0:
                time.sleep(ArtGrid.RESULT_WAIT_INTERVAL)
                # force a cpy as we are modifying all_results
                for jedi_id in list(all_results):
                    status = self.task_status(jedi_id)
                    if status is not None:
                        log.info("JediID %s finished with status %s", str(jedi_id), status)
                        if status == 'done':
                            package = all_results[jedi_id][0]
                            # FIXME limited
                            if self.nightly_release in ['21.0', '21.0-mc16d'] and package in ['Tier0ChainTests']:
                                log.info("Copy %s to eos area", package)
                                self.copy(package)
                        del all_results[jedi_id]

        return 0

    def task_status(self, jedi_id):
        """
        Wait for job to finish.

        Return final status of a task, or None if not finished
        """
        log = logging.getLogger(MODULE)

        # fake return for simulation
        if jedi_id == 0:
            return "done"

        try:
            r = urllib2.urlopen('https://bigpanda.cern.ch/task/' + str(jedi_id) + '?json=true')
            s = json.load(r)
            status = s['task']['superstatus']
            if status in ["done", "finished", "failed", "aborted", "broken"]:
                log.info("Task: %s %s", str(jedi_id), str(status))
                return status
        except urllib2.HTTPError, e:
            log.error('%s for %s status', str(e.code), str(jedi_id))
        return None

    def task(self, script_directory, package, job_type, sequence_tag, no_action=False):
        """
        Submit a task, consisting of multiple jobs.

        For 'single' jobs each task contains exactly one job.
        Returns a map of JediIds to tuples of (package, test_name)
        """
        log = logging.getLogger(MODULE)
        log.info('Running art task')

        config = None if self.skip_setup else self.get_config()
        grid_options = self.grid_option(config, package, 'grid-exclude-sites', '--excludedSite=')
        grid_options += ' ' + self.grid_option(config, package, 'grid-sites', '--site=')

        # run task from Bash Script as is needed in ATLAS setup
        # FIXME we need to parse the output
        env = os.environ.copy()
        env['PATH'] = '.:' + env['PATH']
        env['ART_GRID_OPTIONS'] = grid_options

        test_directories = self.get_test_directories(self.get_script_directory())
        test_directory = test_directories[package]
        number_of_batch_tests = len(self.get_files(test_directory, job_type, "batch", self.nightly_release, self.project, self.platform))

        MAX_OUTFILE_LEN = 132
        user = env['USER'] if self.skip_setup else ArtGrid.ARTPROD
        outfile = self.get_outfile(user, package, sequence_tag)

        result = {}

        # submit batch tests
        if number_of_batch_tests > 0:
            if len(outfile) > MAX_OUTFILE_LEN:
                log.error("OutFile string length > %d: ", MAX_OUTFILE_LEN, outfile)
                return 1

            # Batch
            cmd = ' '.join((os.path.join(self.art_directory, 'art-task-grid.sh'), '--skip-setup' if self.skip_setup else '', self.submit_directory, script_directory, package, job_type, sequence_tag, str(number_of_batch_tests), self.get_nightly_release_short(), self.project, self.platform, self.nightly_tag, outfile))
            log.info("batch: %s", cmd)

            if not no_action:
                (exit_code, out, err) = run_command(cmd, env=env)
                if exit_code != 0:
                    log.error("art-task-grid failed %d", exit_code)
                    print out
                    print err
                else:
                    jediID = self.get_jedi_id(err)
                    if jediID > 0:
                        result[jediID] = (package, "", outfile)
                log.info(out)

        # submit single tests
        index = 1
        for test_name in self.get_files(test_directory, job_type, "single", self.nightly_release, self.project, self.platform):
            job = os.path.join(test_directory, test_name)
            header = ArtHeader(job)
            inds = header.get(ArtHeader.ART_INPUT)
            nFiles = header.get(ArtHeader.ART_INPUT_NFILES)
            split = header.get(ArtHeader.ART_INPUT_SPLIT)

            outfile_test = self.get_outfile(user, package, sequence_tag, str(index))
            if len(outfile_test) > MAX_OUTFILE_LEN:
                log.error("ERROR: OutFile string length > %d : %s ", MAX_OUTFILE_LEN, outfile_test)
                return 1

            # Single
            cmd = ' '.join((os.path.join(self.art_directory, 'art-task-grid.sh'), '--skip-setup' if self.skip_setup else '', '--test-name ' + test_name, '--inDS ' + inds, '--nFiles ' + str(nFiles) if nFiles > 0 else '', self.submit_directory, script_directory, package, job_type, sequence_tag, str(split), self.get_nightly_release_short(), self.project, self.platform, self.nightly_tag, outfile_test))
            log.info("single: %s", cmd)

            if not no_action:
                (exit_code, out, err) = run_command(cmd, env=env)
                if exit_code != 0:
                    log.error("art-task-grid failed %d", exit_code)
                    print out
                    print err
                else:
                    jediID = self.get_jedi_id(err)
                    if jediID > 0:
                        result[jediID] = (package, test_name, outfile_test)

                log.info(out)

            index += 1

        return result

    def job(self, package, job_type, sequence_tag, index_type, index_or_name, out):
        """TBD."""
        log = logging.getLogger(MODULE)
        log.info('Running art job grid')

        log.info("%s %s %s %s %s %s %s %s", self.nightly_release, self.project, self.platform, self.nightly_tag, package, job_type, str(index_or_name), out)

        test_directories = self.get_test_directories(self.get_script_directory())
        test_directory = test_directories[package]
        if index_type == "batch":
            test_list = self.get_files(test_directory, job_type, "batch", self.nightly_release, self.project, self.platform)

            # minus one for grid
            index = int(index_or_name)
            test_name = test_list[index - 1]
        else:
            test_name = index_or_name

        log.info("art-job-name: %s", test_name)

        test_file = os.path.join(test_directory, test_name)
        # arguments are SCRIPT_DIRECTORY, PACKAGE, TYPE, TEST_NAME, NIGHTLY_RELEASE, PROJECT, PLATFORM, NIGHTLY_TAG
        command = ' '.join((test_file, self.get_script_directory(), package, job_type, test_name, self.nightly_release, self.project, self.platform, self.nightly_tag))

        log.debug(test_name)
        log.debug(test_directory)
        log.debug(command)

        # run the test
        env = os.environ.copy()
        env['PATH'] = '.:' + env['PATH']
        (exit_code, output, error) = run_command(command, env=env)
        print output
        if (exit_code != 0):
            log.error("Test %s failed %d", str(index_or_name), exit_code)
            print error
        # NOTE: exit_code always 0
        print error

        # gather results
        result = {}
        result['name'] = test_name
        result['exit_code'] = exit_code
        result['test_directory'] = test_directory
        result['result'] = self.get_art_results(output)

        # write out results
        with open(os.path.join(ArtGrid.ART_JOB), 'w') as jobfile:
            json.dump(result, jobfile, sort_keys=True, indent=4, ensure_ascii=False)
            log.info("Wrote %s", ArtGrid.ART_JOB)

        # grab the content of "jobReport.json", add the art dictionary and write it back
        if os.path.isfile(ArtGrid.JOB_REPORT):
            with open(ArtGrid.JOB_REPORT, 'r+') as json_file:
                info = json.load(json_file)
                info[ArtGrid.JOB_REPORT_ART_KEY] = result
                # write out results
                json_file.seek(0)
                json.dump(info, json_file, sort_keys=True, indent=4, ensure_ascii=False)
                json_file.truncate()
                log.info("Updated %s", ArtGrid.JOB_REPORT)

        # pick up the outputs
        tar_file = tarfile.open(out, mode='w')

        # pick up explicitly named output files
        with open(test_file, "r") as f:
            for line in f:
                # remove comments
                line = line.split('#', 1)[0]
                out_names = re.findall(r"--output[^\s=]*[= ]*(\S*)", line)
                log.debug(out_names)
                for out_name in out_names:
                    out_name = out_name.strip('\'"')
                    if os.path.exists(out_name):
                        log.info('Tar file contain: %s', out_name)
                        tar_file.add(out_name)

        # pick up art-header named outputs
        for path_name in ArtHeader(test_file).get('art-output'):
            for out_name in glob.glob(path_name):
                log.info('Tar file contains: %s', out_name)
                tar_file.add(out_name)

        tar_file.close()
        # Always return 0
        return 0

    def get_grid_map(self, user, package, sequence_tag=0, nightly_tag=None):
        """Return grid map of test_name to grid_index."""
        log = logging.getLogger(MODULE)
        scope = '.'.join(('user', user))

        outfile = self.get_outfile(user, package, sequence_tag=sequence_tag, nightly_tag=nightly_tag)
        log.debug("outfile %s", outfile)
        jsons = self.get_rucio_map(scope, outfile, ArtGrid.JSON)
        logs = self.get_rucio_map(scope, outfile, ArtGrid.LOG)

        result = {}
        for grid_index in logs:
            rucio_name = jsons[grid_index]['rucio_name'] if grid_index in jsons else None
            rucio_log_name = logs[grid_index]['rucio_name'] if grid_index in logs else None
            test_name = self.get_test_name(rucio_name, rucio_log_name)
            if test_name is None:
                # log.warning("JSON Lookup failed for test %s", rucio_log_name if rucio_name is None else rucio_name)
                continue

            result[test_name] = int(grid_index)
        return result

    def list(self, package, job_type, index_type, json_format, user, nogrid):
        """TBD."""
        log = logging.getLogger(MODULE)
        user = ArtGrid.ARTPROD if user is None else user

        # make sure script directory exist
        self.exit_if_no_script_directory()

        if not nogrid:
            log.info("Getting grid map...")
            grid_map = self.get_grid_map(user, package)

        log.info("Getting test names...")
        test_names = self.get_list(self.get_script_directory(), package, job_type, index_type)
        json_array = []
        for test_name in test_names:
            name = os.path.splitext(test_name)[0]
            json_array.append({
                'name': name,
                'grid_index': str(grid_map[name]) if not nogrid and name in grid_map else '-1'
            })

        if json_format:
            json.dump(json_array, sys.stdout, sort_keys=True, indent=4)
            return 0

        i = 0
        for entry in json_array:
            print str(i) + ' ' + entry['name'] + (' ' + entry['grid_index'])
            i += 1

        # print warnings
        if not nogrid:
            for entry in json_array:
                if entry['grid_index'] < 0:
                    log.warning('test %s could not be found in json or log', entry['name'])

        return 0

    def log(self, package, test_name, user):
        """TBD."""
        log = logging.getLogger(MODULE)
        user = ArtGrid.ARTPROD if user is None else user

        # make sure script directory exist
        self.exit_if_no_script_directory()

        tar = self.open_tar(user, package, test_name, ArtGrid.LOG)
        if tar is None:
            log.error("No log tar file found")
            return 1

        for name in tar.getnames():
            if ArtGrid.ATHENA_STDOUT in name:
                f = tar.extractfile(name)
                content = f.read()
                print content
                break
        tar.close()
        return 0

    def output(self, package, test_name, user):
        """TBD."""
        log = logging.getLogger(MODULE)
        user = ArtGrid.ARTPROD if user is None else user

        # make sure script directory exist
        self.exit_if_no_script_directory()

        outfile = self.get_outfile(user, package)
        tar_dir = os.path.join(tempfile.gettempdir(), outfile + ArtGrid.OUTPUT)
        mkdir_p(tar_dir)

        tar = self.open_tar(user, package, test_name, ArtGrid.OUTPUT)
        if tar is None:
            log.error("No output tar file found")
            return 1

        tar.extractall(path=tar_dir)
        tar.close()
        print "Output extracted in", tar_dir
        return 0

    def compare(self, package, test_name, days, file_names, user):
        """TBD."""
        log = logging.getLogger(MODULE)
        user = ArtGrid.ARTPROD if user is None else user

        previous_nightly_tag = self.get_previous_nightly_tag(days)
        log.info("LOG Previous Nightly Tag: %s", str(previous_nightly_tag))
        print "PRINT Previous Nightly Tag", str(previous_nightly_tag)

        if previous_nightly_tag is None:
            log.error("No previous nightly tag found")
            return 1

        ref_dir = os.path.join('.', 'ref-' + previous_nightly_tag)
        mkdir_p(ref_dir)

        tar = self.open_tar(user, package, test_name, ArtGrid.OUTPUT, previous_nightly_tag)
        if tar is None:
            log.error("No comparison tar file found")
            return 1

        for member in tar.getmembers():
            if member.name in file_names:
                tar.extractall(path=ref_dir, members=[member])
        tar.close()

        result = 0
        for file_name in file_names:
            ref_file = os.path.join(ref_dir, file_name)
            if os.path.isfile(ref_file):
                print "art-compare:", previous_nightly_tag, file_name
                result |= self.compare_ref(file_name, ref_file, 10)
            else:
                log.error("%s not found in tar file", ref_file)
                result = 1
        return result

    def grid_option(self, config, package, key, option_key):
        """Based on config, return value for key, or ''.

        A global value is pre-pended if found. If not local value is found only the global value is returned, or ''.
        """
        if config is None:
            return ''

        global_value = config.get(key)
        if package not in config.keys():
            return '' if global_value is None else option_key + global_value

        value = config.get(package).get(key)

        if global_value is None:
            return '' if value is None else option_key + value
        else:
            return option_key + global_value + ('' if value is None else ', ' + value)

    def open_tar(self, user, package, test_name, extension, nightly_tag=None):
        """Open tar file for particular release."""
        log = logging.getLogger(MODULE)
        if not RUCIO:
            log.critical("RUCIO not available")
            exit(1)

        log.info("Getting grid map...")
        grid_map = self.get_grid_map(user, package, nightly_tag=nightly_tag)

        name = os.path.splitext(test_name)[0]
        if name not in grid_map:
            log.error("No log or tar found for package %s or test %s", package, test_name)
            return None

        grid_index = grid_map[name]
        log.info("Grid Index: %d", grid_index)

        scope = '.'.join(('user', user))
        outfile = self.get_outfile(user, package, nightly_tag=nightly_tag)
        rucio_map = self.get_rucio_map(scope, outfile, extension)
        if grid_index not in rucio_map:
            log.error("No entry in rucio map for %d", grid_index)
            return None

        rucio_name = rucio_map[grid_index]['rucio_name']
        log.info("RUCIO: %s", rucio_name)

        tmp_dir = tempfile.mkdtemp()
        atexit.register(shutil.rmtree, tmp_dir)

        tmp_tar = os.path.join(tmp_dir, os.path.basename(rucio_name))

        (exit_code, out, err) = run_command(' '.join(('xrdcp -N -f', rucio_name, tmp_dir)))
        if exit_code != 0:
            log.error("TAR Error: %s %d %s %s", rucio_name, exit_code, out, err)
            return None

        return tarfile.open(tmp_tar)

    def get_previous_nightly_tag(self, days):
        """TBD. 21:00 is cutoff time."""
        directory = os.path.join(ArtGrid.CVMFS_DIRECTORY, self.nightly_release)
        tags = os.listdir(directory)
        tags.sort(reverse=True)
        tags = [x for x in tags if re.match(r'\d{4}-\d{2}-\d{2}T\d{2}\d{2}', x)]
        found = False
        for tag in tags:
            if tag == self.nightly_tag:
                found = True
            elif found:
                # check this is within days... (cutoff is 21:00, just move by 3 hours to get full days)
                fmt = '%Y-%m-%dT%H%M'
                offset = datetime.timedelta(hours=3)
                nightly_tag_dt = datetime.datetime.strptime(self.nightly_tag, fmt) + offset
                from_dt = nightly_tag_dt.replace(hour=0, minute=0, second=0, microsecond=0) - datetime.timedelta(days=days)
                to_dt = from_dt + datetime.timedelta(days=1)
                tag_dt = datetime.datetime.strptime(tag, fmt) + offset
                within_days = from_dt <= tag_dt and tag_dt < to_dt
                target_exists = len(glob.glob(os.path.join(directory, tag, self.project, '*', 'InstallArea', self.platform))) > 0
                if within_days and target_exists:
                    return tag
        return None
