#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""Class for grid submission."""

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

from art_base import ArtBase
from art_configuration import ArtConfiguration
from art_header import ArtHeader
from art_rucio import ArtRucio
from art_misc import mkdir_p, make_executable, run_command

MODULE = "art.grid"


class ArtGrid(ArtBase):
    """Class for grid submission."""

    CVMFS_DIRECTORY = '/cvmfs/atlas-nightlies.cern.ch/repo/sw'
    EOS_MGM_URL = 'root://eosatlas.cern.ch/'
    EOS_OUTPUT_DIR = '/eos/atlas/atlascerngroupdisk/data-art/grid-output'

    ARTPROD = 'artprod'
    JOB_REPORT = 'jobReport.json'
    JOB_REPORT_ART_KEY = 'art'
    RESULT_WAIT_INTERVAL = 5 * 60

    def __init__(self, art_directory, nightly_release, project, platform, nightly_tag, script_directory=None, skip_setup=False, submit_directory=None):
        """Keep arguments."""
        super(ArtGrid, self).__init__(art_directory)
        self.nightly_release = nightly_release
        self.project = project
        self.platform = platform
        self.nightly_tag = nightly_tag
        self.script_directory = script_directory
        self.skip_setup = skip_setup
        self.submit_directory = submit_directory

        self.rucio = ArtRucio()

    def status(self, status):
        """Print status for usage in gitlab-ci."""
        print 'art-status:', status

    def get_script_directory(self):
        """Return calculated script directory, sometimes overriden by commandline."""
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

    def exit_if_outfile_too_long(self, outfile_test):
        """Exit with ERROR if outfile too long."""
        log = logging.getLogger(MODULE)
        MAX_OUTFILE_LEN = 132
        if len(outfile_test) > MAX_OUTFILE_LEN:
            log.error('OutFile string length > %d: %s', MAX_OUTFILE_LEN, outfile_test)
            exit(1)

    def copy_art(self, art_python, run_dir):
        """Copy all art files to the the run directory. Returns final script directory to be used."""
        log = logging.getLogger(MODULE)
        ART = os.path.join(run_dir, "ART")
        mkdir_p(ART)

        # get the path of the python classes and support scripts
        art_python_directory = os.path.join(self.art_directory, art_python, 'ART')

        shutil.copy(os.path.join(self.art_directory, 'art.py'), run_dir)
        shutil.copy(os.path.join(self.art_directory, 'art-diff.py'), run_dir)
        shutil.copy(os.path.join(self.art_directory, 'art-get-input.sh'), run_dir)
        shutil.copy(os.path.join(self.art_directory, 'art-internal.py'), run_dir)
        shutil.copy(os.path.join(self.art_directory, 'art-task-grid.sh'), run_dir)
        shutil.copy(os.path.join(art_python_directory, '__init__.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'art_base.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'art_build.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'art_configuration.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'art_grid.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'art_header.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'art_misc.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'art_rucio.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'docopt.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'docopt_dispatch.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'parallelScheduler.py'), ART)
        shutil.copy(os.path.join(art_python_directory, 'serialScheduler.py'), ART)

        make_executable(os.path.join(run_dir, 'art.py'))
        make_executable(os.path.join(run_dir, 'art-diff.py'))
        make_executable(os.path.join(run_dir, 'art-get-input.sh'))
        make_executable(os.path.join(run_dir, 'art-internal.py'))
        make_executable(os.path.join(run_dir, 'art-task-grid.sh'))

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
        return match.group(1) if match else -1

    def get_nightly_release_short(self):
        """Return  a short version of the nightly release."""
        return re.sub(r"-VAL-.*", "-VAL", self.nightly_release)

    def copy(self, package, dst=None, user=None):
        """Copy output from scratch area to eos area."""
        log = logging.getLogger(MODULE)
        real_user = os.getenv('USER', ArtGrid.ARTPROD)
        user = real_user if user is None else user
        default_dst = ArtGrid.EOS_OUTPUT_DIR if real_user == ArtGrid.ARTPROD else '.'
        dst = default_dst if dst is None else dst

        if package is not None:
            log.info("Copy %s", package)
            outfile = self.rucio.get_outfile(user, package, self.get_nightly_release_short(), self.project, self.platform, self.nightly_tag)
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
                outfile = self.rucio.get_outfile(user, package, self.get_nightly_release_short(), self.project, self.platform, self.nightly_tag)
                log.info("Copying from %s", outfile)

                result |= self.copy_output(outfile, dst)
        return result

    def copy_output(self, outfile, dst):
        """Copy outfile to dst."""
        log = logging.getLogger(MODULE)

        cleanup = False

        result = 0
        outfile_pattern = r"([^\.]+)\.([^\.]+)\.([^\.]+)\.(.+)\.([^\.]+)\.([^\.]+)\.([^\.]+)\.([^\.]+)\.([^\.\n]+)"
        match = re.search(outfile_pattern, outfile)
        if not match:
            log.error("%s does not match pattern", outfile)
            return 1
        (user_type, user, experiment, nightly_release, project, platform, nightly_tag, sequence_tag, package) = match.groups()
        dst_dir = os.path.join(dst, nightly_release, nightly_tag, project, platform, package)
        log.info("%s", dst_dir)

        tmp_dir = tempfile.mkdtemp()
        if cleanup:
            atexit.register(shutil.rmtree, tmp_dir)

        tmp_json = os.path.join(tmp_dir, ArtRucio.ART_JOB)
        tmp_log = os.path.join(tmp_dir, ArtRucio.LOG_TGZ)
        tmp_tar = os.path.join(tmp_dir, ArtRucio.JOB_TAR)

        for index in self.rucio.get_indices(user, outfile + ArtRucio.OUTPUT):
            # get the test name
            test_name = self.rucio.get_job_name(user, index, package, sequence_tag, nightly_release, project, platform, nightly_tag)
            if test_name is None:
                log.error("JSON Lookup Error for test %d", index)
                result = 1
                continue

            # create tmp test directory
            test_dir = os.path.join(tmp_dir, test_name)
            mkdir_p(test_dir)

            # copy art-job.json, ignore error
            log.info("Copying JSON: %d %s", index, outfile + ArtRucio.JSON)
            if self.rucio.xrdcp(self.rucio.get_rucio_name(user, outfile + ArtRucio.JSON, index), tmp_json, force=True) == 0:
                shutil.copyfile(tmp_json, os.path.join(test_dir, ArtRucio.ART_JOB))

            # copy and unpack log
            log.info("Copying LOG: %d %s", index, outfile + ArtRucio.LOG)
            if self.rucio.xrdcp(self.rucio.get_rucio_name(user, outfile + ArtRucio.LOG, index), tmp_log, force=True) != 0:
                log.error("Log Unpack Error")
                result = 1
            else:
                log.info("Unpacking LOG: %s %s", index, test_dir)
                tar = tarfile.open(tmp_log)
                for member in tar.getmembers():
                    tar.extract(member, path=test_dir)
                # does not work: tar.extractall()
                tar.close()

            log.info("Copying TAR: %d %s", index, outfile + ArtRucio.OUTPUT)

            # copy results and unpack
            if self.rucio.xrdcp(self.rucio.get_rucio_name(user, outfile + ArtRucio.OUTPUT, index), tmp_tar, force=True) != 0:
                log.error("TAR Error")
                result = 1
            else:
                log.info("Unpacking TAR: %d %s to %s", index, tmp_tar, test_dir)
                tar = tarfile.open(tmp_tar)
                tar.extractall(path=test_dir)
                tar.close()

            # copy to eos
            dst_target = os.path.join(dst_dir, test_name)
            if dst_target.startswith('/eos'):
                # mkdir_cmd = 'eos ' + ArtGrid.EOS_MGM_URL + ' mkdir -p'
                mkdir_cmd = None
                xrdcp_target = ArtGrid.EOS_MGM_URL + dst_target + '/'
            else:
                mkdir_cmd = 'mkdir -p'
                xrdcp_target = dst_target
            log.info("Copying to DST: %d %s", index, xrdcp_target)

            if mkdir_cmd is not None:
                (exit_code, out, err) = run_command(' '.join((mkdir_cmd, dst_target)))
                if exit_code != 0:
                    log.error("Mkdir Error: %d %s %s", exit_code, out, err)
                    result = 1

            cmd = ' '.join(('xrdcp -N -r -p -v', test_dir, xrdcp_target))
            log.info("using: %s", cmd)
            (exit_code, out, err) = run_command(cmd)
            if exit_code not in [0, 51, 54]:
                # 0 all is ok
                # 51 File exists
                # 54 is already copied
                log.error("XRDCP to EOS Error: %d %s %s", exit_code, out, err)
                result = 1

            # cleanup
            if cleanup:
                shutil.rmtree(test_dir)

        return result

    def task_package(self, root, package, job_type, sequence_tag, no_action, config_file):
        """Submit a single package."""
        log = logging.getLogger(MODULE)
        result = {}
        number_of_tests = len(self.get_files(root, job_type, "all", self.nightly_release, self.project, self.platform))
        if number_of_tests > 0:
            print 'art-package:', package
            self.status('included')
            log.info('root %s', root)
            log.info('Handling %s for %s project %s on %s', package, self.nightly_release, self.project, self.platform)
            log.info("Number of tests: %d", number_of_tests)

            run_dir = os.path.join(self.submit_directory, package, 'run')
            script_directory = self.copy_art('../python', run_dir)

            result = self.task(script_directory, package, job_type, sequence_tag, no_action, config_file)
        return result

    def task_list(self, job_type, sequence_tag, package=None, no_action=False, wait_and_copy=True, config_file=None):
        """Submit a list of packages."""
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

        configuration = None if self.skip_setup else ArtConfiguration(config_file)

        all_results = {}

        if package is None:
            # submit tasks for all packages
            for package, root in test_directories.items():
                if configuration is not None and configuration.get(self.nightly_release, self.project, self.platform, package, 'exclude', False):
                    log.warning("Package %s is excluded", package)
                else:
                    all_results.update(self.task_package(root, package, job_type, sequence_tag, no_action, config_file))
        else:
            # Submit single package
            root = test_directories[package]
            all_results.update(self.task_package(root, package, job_type, sequence_tag, no_action, config_file))

        if no_action:
            log.info("--no-action specified, so not waiting for results")
            return 0

        if len(all_results) == 0:
            log.warning('No tests found, nothing to submit.')
            return 0

        # wait for all results
        if wait_and_copy:
            configuration = ArtConfiguration(config_file)
            while len(all_results) > 0:
                time.sleep(ArtGrid.RESULT_WAIT_INTERVAL)
                # force a copy of all_results since we are modifying all_results
                for jedi_id in list(all_results):
                    package = all_results[jedi_id][0]
                    # skip packages without copy
                    if not configuration.get(self.nightly_release, self.project, self.platform, package, "copy"):
                        del all_results[jedi_id]
                        continue

                    status = self.task_status(jedi_id)
                    if status is not None:
                        log.info("JediID %s finished with status %s", str(jedi_id), status)
                        if status in ['finished', 'done']:
                            dst = configuration.get(self.nightly_release, self.project, self.platform, package, "dst", ArtGrid.EOS_OUTPUT_DIR)
                            log.info("Copy %s to %s", package, dst)
                            self.copy(package, dst)
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

    def task_job(self, grid_options, sub_cmd, script_directory, sequence_tag, package, outfile, job_type='', number_of_tests=0, split=0, job_name='', inds='', n_files=0, in_file=False, no_action=False):
        """
        Submit a single job.

        Returns jedi_id or 0 if submission failed.

        # art-task-grid.sh [--no-action --skip-setup] batch <submit_directory> <script_directory> <sequence_tag> <package> <outfile> <job_type> <number_of_tests>
        #
        # art-task-grid.sh [--no-action --skip-setup] single [--inds <input_file> --n-files <number_of_files> --split <split> --in] <submit_directory> <script_directory> <sequence_tag> <package> <outfile> <job_name>
        """
        log = logging.getLogger(MODULE)
        cmd = ' '.join((os.path.join(self.art_directory, 'art-task-grid.sh'),
                        '--no-action' if no_action else '',
                        '--skip-setup' if self.skip_setup else '',
                        sub_cmd))

        if sub_cmd == 'single':
            cmd = ' '.join((cmd,
                            '--inds ' + inds if inds != '' else '',
                            '--n-files ' + str(n_files) if n_files > 0 else '',
                            '--split ' + str(split) if split > 0 else ''
                            '--in' if in_file else ''))

        cmd = ' '.join((cmd,
                        self.submit_directory,
                        script_directory,
                        sequence_tag,
                        package,
                        outfile))

        if sub_cmd == 'batch':
            cmd = ' '.join((cmd,
                            job_type,
                            str(number_of_tests)))
        elif sub_cmd == 'single':
            cmd = ' '.join((cmd,
                            job_name))
        else:
            log.critical("Invalid sub_cmd %s", sub_cmd)
            exit(1)

        log.info("cmd: %s", cmd)

        # run task from Bash Script as is needed in ATLAS setup
        # FIXME we need to parse the output
        env = os.environ.copy()
        env['PATH'] = '.:' + env['PATH']
        env['ART_GRID_OPTIONS'] = grid_options

        jedi_id = -1
        # run the command, no_action is forwarded and used inside the script
        (exit_code, out, err) = run_command(cmd, env=env)
        if exit_code != 0:
            log.error("art-task-grid failed %d", exit_code)
            print err
        else:
            jedi_id = 0 if no_action else self.get_jedi_id(err)
        print out

        log.info('jedi_id: %s', str(jedi_id))
        return jedi_id

    def get_grid_options(self, package, config_file):
        """Return grid options for a package."""
        if self.skip_setup:
            return ''

        configuration = ArtConfiguration(config_file)
        grid_options = configuration.get_option(self.nightly_release, self.project, self.platform, package, 'exclude-sites', '--excludedSite=')
        grid_options += ' ' + configuration.get_option(self.nightly_release, self.project, self.platform, package, 'sites', '--site=')
        return grid_options

    def task(self, script_directory, package, job_type, sequence_tag, no_action=False, config_file=None):
        """
        Submit a task, consisting of multiple jobs.

        For 'single' jobs each task contains exactly one job.
        Returns a map of jedi_id to (package, test_name, out_file)
        """
        log = logging.getLogger(MODULE)
        log.info('Running art task')

        grid_options = self.get_grid_options(package, config_file)

        test_directories = self.get_test_directories(self.get_script_directory())
        test_directory = test_directories[package]
        number_of_batch_tests = len(self.get_files(test_directory, job_type, "batch", self.nightly_release, self.project, self.platform))

        user = os.getenv('USER', 'artprod') if self.skip_setup else ArtGrid.ARTPROD
        outfile = self.rucio.get_outfile(user, package, self.get_nightly_release_short(), self.project, self.platform, self.nightly_tag, sequence_tag)

        result = {}

        # submit batch tests
        if number_of_batch_tests > 0:
            self.exit_if_outfile_too_long(outfile)

            # Batch
            log.info("Batch")
            jedi_id = self.task_job(grid_options, "batch", script_directory, sequence_tag, package, outfile, job_type=job_type, number_of_tests=number_of_batch_tests, no_action=no_action)
            if jedi_id > 0:
                result[jedi_id] = (package, "", outfile)

        # submit single tests
        index = 1
        for job_name in self.get_files(test_directory, job_type, "single", self.nightly_release, self.project, self.platform):
            job = os.path.join(test_directory, job_name)
            header = ArtHeader(job)
            inds = header.get(ArtHeader.ART_INPUT)
            n_files = header.get(ArtHeader.ART_INPUT_NFILES)
            split = header.get(ArtHeader.ART_INPUT_SPLIT)

            outfile_test = self.rucio.get_outfile(user, package, self.get_nightly_release_short(), self.project, self.platform, self.nightly_tag, sequence_tag, str(index))
            self.exit_if_outfile_too_long(outfile_test)

            # Single
            log.info("Single")
            jedi_id = self.task_job(grid_options, "single", script_directory, sequence_tag, package, outfile_test, split=split, job_name=job_name, inds=inds, n_files=n_files, in_file=True, no_action=no_action)

            if jedi_id > 0:
                result[jedi_id] = (package, job_name, outfile_test)

            index += 1

        return result

    def batch(self, sequence_tag, package, out, job_type, job_index):
        """Run a single job by job_index of a 'batch' submission."""
        log = logging.getLogger(MODULE)
        log.info('Running art grid batch')
        log.info("%s %s %s %s %s %s %s %s", self.nightly_release, self.project, self.platform, self.nightly_tag, package, job_type, str(job_index), out)

        test_directories = self.get_test_directories(self.get_script_directory())
        test_directory = test_directories[package]

        test_list = self.get_files(test_directory, job_type, "batch", self.nightly_release, self.project, self.platform)

        # FIXME ??? minus one for grid
        index = int(job_index)
        job_name = test_list[index - 1]

        in_file = None

        return self.job(test_directory, package, job_name, job_type, out, in_file)

    def single(self, sequence_tag, package, out, job_name, in_file):
        """Run a single job by name of a 'single' submission."""
        log = logging.getLogger(MODULE)

        log.info('Running art grid single')
        log.info("%s %s %s %s %s %s %s %s", self.nightly_release, self.project, self.platform, self.nightly_tag, package, job_name, out, in_file)

        test_directories = self.get_test_directories(self.get_script_directory())
        test_directory = test_directories[package]

        job_type = 'grid'
        return self.job(test_directory, package, job_name, job_type, out, in_file)

    def job(self, test_directory, package, job_name, job_type, out, in_file):
        """Run a single job."""
        log = logging.getLogger(MODULE)
        log.info("art-job-name: %s", job_name)
        test_file = os.path.join(test_directory, job_name)

        # Tests are called with arguments: PACKAGE TEST_NAME SCRIPT_DIRECTORY TYPE [IN_FILE]
        script_directory = self.get_script_directory()
        command = ' '.join((test_file, package, job_name, script_directory, job_type, in_file if in_file is not None else ''))

        log.debug(job_name)
        log.debug(test_directory)
        log.debug(command)

        # run the test
        env = os.environ.copy()
        env['PATH'] = '.:' + env['PATH']
        env['ArtScriptDirectory'] = script_directory
        env['ArtPackage'] = package
        env['ArtJobType'] = job_type
        env['ArtJobName'] = job_name
        if in_file is not None:
            env['ArtInFile'] = in_file
        (exit_code, output, error) = run_command(command, env=env)
        print output
        if (exit_code != 0):
            log.error("Test %s failed %d", job_name, exit_code)
            print error
        # NOTE: exit_code always 0
        print error

        # gather results
        result = {}
        result['name'] = job_name
        result['exit_code'] = exit_code
        result['test_directory'] = test_directory
        result['result'] = ArtBase.get_art_results(output)

        # write out results
        with open(os.path.join(ArtRucio.ART_JOB), 'w') as jobfile:
            json.dump(result, jobfile, sort_keys=True, indent=4, ensure_ascii=False)
            log.info("Wrote %s", ArtRucio.ART_JOB)

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
        else:
            with open(ArtGrid.JOB_REPORT, 'w') as json_file:
                info = {}
                info[ArtGrid.JOB_REPORT_ART_KEY] = result
                json.dump(info, json_file, sort_keys=True, indent=4, ensure_ascii=False)
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

    def list(self, package, job_type, index_type, json_format, user, nogrid):
        """List all jobs available."""
        log = logging.getLogger(MODULE)
        user = ArtGrid.ARTPROD if user is None else user

        # make sure script directory exist
        self.exit_if_no_script_directory()

        log.info("Getting test names...")
        test_names = self.get_list(self.get_script_directory(), package, job_type, index_type)
        json_array = []
        for test_name in test_names:
            job_name = os.path.splitext(test_name)[0]
            json_array.append({
                'name': job_name,
                'grid_index': str(self.rucio.get_index(user, '*', package, job_name, self.get_nightly_release_short(), self.project, self.platform, self.nightly_tag)) if not nogrid else '-1'
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
        """Print the log of a job."""
        log = logging.getLogger(MODULE)
        user = ArtGrid.ARTPROD if user is None else user

        # make sure script directory exist
        self.exit_if_no_script_directory()

        tar = self.open_tar(user, package, test_name, ArtRucio.LOG)
        if tar is None:
            log.error("No log tar file found")
            return 1

        for name in tar.getnames():
            if ArtRucio.ATHENA_STDOUT in name:
                f = tar.extractfile(name)
                content = f.read()
                print content
                break
        tar.close()
        return 0

    def output(self, package, test_name, user):
        """Download the putput of a job."""
        log = logging.getLogger(MODULE)
        user = ArtGrid.ARTPROD if user is None else user

        # make sure script directory exist
        self.exit_if_no_script_directory()

        outfile = self.rucio.get_outfile(user, package, self.get_nightly_release_short(), self.project, self.platform, self.nightly_tag)
        tar_dir = os.path.join(tempfile.gettempdir(), outfile + ArtRucio.OUTPUT)
        mkdir_p(tar_dir)

        tar = self.open_tar(user, package, test_name, ArtRucio.OUTPUT)
        if tar is None:
            log.error("No output tar file found")
            return 1

        tar.extractall(path=tar_dir)
        tar.close()
        print "Output extracted in", tar_dir
        return 0

    def compare(self, package, test_name, days, user, entries=-1):
        """Compare current output against a job of certain days ago."""
        log = logging.getLogger(MODULE)
        user = ArtGrid.ARTPROD if user is None else user

        previous_nightly_tag = self.get_previous_nightly_tag(days)
        log.info("LOG Previous Nightly Tag: %s", str(previous_nightly_tag))

        if previous_nightly_tag is None:
            log.error("No previous nightly tag found")
            return 1

        ref_dir = os.path.join('.', 'ref-' + previous_nightly_tag)
        mkdir_p(ref_dir)

        tar = self.open_tar(user, package, test_name, ArtRucio.OUTPUT, previous_nightly_tag)
        if tar is None:
            log.error("No comparison tar file found")
            return 1

        for member in tar.getmembers():
            tar.extractall(path=ref_dir, members=[member])
        tar.close()

        return self.compare_ref('.', ref_dir, entries)

    def open_tar(self, user, package, test_name, extension, nightly_tag=None):
        """Open tar file for particular release."""
        log = logging.getLogger(MODULE)
        job_name = os.path.splitext(test_name)[0]
        if nightly_tag is None:
            nightly_tag = self.nightly_tag

        grid_index = self.rucio.get_index(user, '*', package, job_name, self.get_nightly_release_short(), self.project, self.platform, nightly_tag)
        if grid_index < 0:
            log.error("No log or tar found for package %s or test %s", package, test_name)
            return None

        log.info("Grid Index: %d", grid_index)

        outfile = self.rucio.get_outfile(user, package, self.get_nightly_release_short(), self.project, self.platform, nightly_tag)

        rucio_name = self.rucio.get_rucio_name(user, outfile + extension, grid_index)
        if rucio_name is None:
            log.error("No rucio_name for %d", grid_index)
            return None
        log.info("RUCIO: %s", rucio_name)

        tmp_dir = tempfile.mkdtemp()
        atexit.register(shutil.rmtree, tmp_dir)

        tmp_tar = os.path.join(tmp_dir, os.path.basename(rucio_name))

        if self.rucio.xrdcp(rucio_name, tmp_tar) != 0:
            log.error("TAR Error: %s", rucio_name)
            return None

        return tarfile.open(tmp_tar)

    def get_previous_nightly_tag(self, days):
        """
        Return the nightly tag of given days ago.

        21:00 is the cutoff time. Any submission before 21:00 counts as the previous day.
        """
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
