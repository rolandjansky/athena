#!/usr/bin/env python
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
"""Class for grid submission."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import atexit
import concurrent.futures
import exceptions
import glob
import httplib
import json
import logging
import multiprocessing
import os
import re
# requests not available on lxplus, import only when needed
# import requests
import shutil
import sys
import tarfile
import tempfile
import time

from datetime import datetime
from datetime import timedelta

from art_base import ArtBase
from art_configuration import ArtConfiguration
from art_header import ArtHeader
from art_rucio import ArtRucio
from art_misc import count_files, cp, ls, mkdir, make_executable, rm, run_command, run_command_parallel, touch

MODULE = "art.grid"


def copy_job(art_directory, indexed_package, dst, no_unpack, tmp, seq):
    """
    Copy job to be run by executor.

    Needs to be defined outside a class.
    """
    log = logging.getLogger(MODULE)
    log.debug("job started %s %s %s %s %d", art_directory, indexed_package, dst, no_unpack, tmp, seq)
    (exit_code, out, err, command, start_time, end_time) = run_command(' '.join((os.path.join(art_directory, './art.py'), "copy", "--dst=" + dst, "--no-unpack" if no_unpack else "", "--tmp=" + tmp, "--seq=" + str(seq), indexed_package)))
    log.debug("job ended %s %s %s %s %d", art_directory, indexed_package, dst, no_unpack, tmp, seq)

    print "Copy job run with Exit Code:", exit_code
    print out
    print err
    sys.stdout.flush()

    return (indexed_package, exit_code, out, err, start_time, end_time)


class ArtGrid(ArtBase):
    """Class for grid submission."""

    CVMFS_DIRECTORY = '/cvmfs/atlas-nightlies.cern.ch/repo/sw'
    EOS_OUTPUT_DIR = '/eos/atlas/atlascerngroupdisk/data-art/grid-output'

    ARTPROD = 'artprod'
    JOB_REPORT = 'jobReport.json'
    JOB_REPORT_ART_KEY = 'art'
    INITIAL_RESULT_WAIT_INTERVAL = 30 * 60  # seconds, 30 mins
    RESULT_WAIT_INTERVAL = 5 * 60  # seconds, 5 mins
    KINIT_WAIT = 12  # 12 * RESULT_WAIT_INTERVAL, 1 hour

    def __init__(self, art_directory, nightly_release, project, platform, nightly_tag, script_directory=None, skip_setup=False, submit_directory=None, max_jobs=0):
        """Keep arguments."""
        super(ArtGrid, self).__init__(art_directory)
        self.nightly_release = nightly_release
        self.nightly_release_short = re.sub(r"-VAL-.*", "-VAL", self.nightly_release)
        self.project = project
        self.platform = platform
        self.nightly_tag = nightly_tag
        self.script_directory = script_directory
        self.skip_setup = skip_setup
        self.submit_directory = submit_directory
        self.max_jobs = multiprocessing.cpu_count() if max_jobs <= 0 else max_jobs

        self.rucio = ArtRucio(self.art_directory, self.nightly_release_short, project, platform, nightly_tag)

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
        mkdir(ART)

        # get the path of the python classes and support scripts
        art_python_directory = os.path.join(self.art_directory, art_python, 'ART')

        shutil.copy(os.path.join(self.art_directory, 'art.py'), run_dir)
        shutil.copy(os.path.join(self.art_directory, 'art-diff.py'), run_dir)
        shutil.copy(os.path.join(self.art_directory, 'art-internal.py'), run_dir)
        shutil.copy(os.path.join(self.art_directory, 'art-task-grid.sh'), run_dir)
        shutil.copy(os.path.join(self.art_directory, 'art-download.sh'), run_dir)
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

        make_executable(os.path.join(run_dir, 'art.py'))
        make_executable(os.path.join(run_dir, 'art-diff.py'))
        make_executable(os.path.join(run_dir, 'art-internal.py'))
        make_executable(os.path.join(run_dir, 'art-task-grid.sh'))
        make_executable(os.path.join(run_dir, 'art-download.sh'))

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

    def copy(self, indexed_package, dst=None, user=None, no_unpack=False, tmp=None, seq=0, keep_tmp=False):
        """Copy output from scratch area to eos area."""
        log = logging.getLogger(MODULE)
        tmp = tempfile.mkdtemp(prefix=indexed_package + '-') if tmp is None else tmp
        mkdir(tmp)

        if indexed_package is not None:
            return self.copy_package(indexed_package, dst, user, no_unpack, tmp, seq, keep_tmp)

        # make sure script directory exist
        self.exit_if_no_script_directory()

        # get the test_*.sh from the test directory
        test_directories = self.get_test_directories(self.get_script_directory())
        if not test_directories:
            log.warning('No tests found in directories ending in "test"')

        # copy results for all packages
        result = 0
        for indexed_package, root in test_directories.items():
            number_of_tests = len(self.get_files(root, "grid", "all", self.nightly_release, self.project, self.platform))
            if number_of_tests > 0:
                result |= self.copy_package(indexed_package, dst, user, no_unpack, tmp, seq, keep_tmp)
        return result

    def copy_package(self, indexed_package, dst, user, no_unpack, tmp, seq, keep_tmp):
        """Copy package to dst."""
        log = logging.getLogger(MODULE)
        real_user = os.getenv('USER', ArtGrid.ARTPROD)
        user = real_user if user is None else user
        default_dst = ArtGrid.EOS_OUTPUT_DIR if real_user == ArtGrid.ARTPROD else '.'
        dst = default_dst if dst is None else dst

        result = 0

        log.debug("Indexed Package %s", indexed_package)

        package = indexed_package.split('.')[0]
        nightly_tag = self.nightly_tag if seq == 0 else '-'.join((self.nightly_tag, str(seq)))
        dst_dir = os.path.join(dst, self.nightly_release, self.project, self.platform, nightly_tag, package)
        log.info("dst_dir %s", dst_dir)

        table = self.rucio.get_table(user, indexed_package, tmp=tmp)
        if not table:
            log.warning("Nothing to be copied")
            return result

        for entry in table:
            grid_index = entry['grid_index']
            log.debug("Grid Index %d", grid_index)

            # get the test name
            test_name = entry['job_name']
            if test_name is None:
                log.warning("JSON TestName not found for test with grid_index %d", grid_index)
                continue
            log.debug("Test_name %s", test_name)

            json_file = os.path.join(tmp, entry['outfile'] + "_EXT0", self.__get_rucio_name(user, entry, 'json'))
            json_dst = dst_dir.replace('/', '.')
            json_copying = json_file + ".copying_to" + json_dst
            json_copied = json_file + ".copied_to" + json_dst

            if os.path.isfile(json_copied):
                log.debug("Already copied: %d %s", grid_index, test_name)
            elif os.path.isfile(json_copying):
                log.debug("Still copying:  %d %s", grid_index, test_name)
            else:
                touch(json_copying)

                # create test directory
                test_dir = os.path.join(tmp, test_name)
                suffix = '-' + str(entry['grid_index'] - 1) if entry['single_index'] > 0 and entry['grid_index'] > 1 else ''
                test_dir += suffix
                mkdir(test_dir)

                # copy art-job.json
                result |= self.copy_json(json_file, test_dir)

                # copy and unpack log
                result |= self.copy_log(user, package, test_name, grid_index, test_dir, no_unpack, tmp)

                # copy results and unpack
                result |= self.copy_results(user, package, test_name, grid_index, test_dir, no_unpack, tmp)

                # copy to eos
                result |= self.copy_to_dst(test_name + suffix, test_dir, dst_dir)

                if result == 0:
                    rm(json_copying)
                    touch(json_copied)

                # cleanup
                if not keep_tmp:
                    shutil.rmtree(test_dir)

        return result

    def copy_json(self, json_file, test_dir):
        """Copy json."""
        log = logging.getLogger(MODULE)
        log.info("Copying JSON: %s", json_file)
        shutil.copyfile(json_file, os.path.join(test_dir, ArtRucio.ART_JOB))
        return 0

    def copy_log(self, user, package, test_name, grid_index, test_dir, no_unpack, tmp):
        """Copy and unpack log file."""
        log = logging.getLogger(MODULE)
        log.info("Copying LOG: %s %s", package, test_name)

        if no_unpack:
            tmp_tar = self.__get_tar(user, package, test_name, grid_index=grid_index, tmp=tmp, tar=False)
            cp(tmp_tar, test_dir)
            os.remove(tmp_tar)
        else:
            tmp_tar = self.__get_tar(user, package, test_name, grid_index=grid_index, tmp=tmp, tar=False)
            if tmp_tar is not None:
                tar = tarfile.open(tmp_tar)
                log.info("Unpacking LOG: %s", test_dir)
                logdir = None
                for member in tar.getmembers():
                    # does not work: tar.extractall()
                    tar.extract(member, path=test_dir)
                    logdir = member.name.split('/', 2)[0]

                tar.close()

                # rename top level log dir to logs
                if logdir is not None:
                    os.chdir(test_dir)
                    os.rename(logdir, "tarball_logs")

                os.remove(tmp_tar)
        return 0

    def copy_results(self, user, package, test_name, grid_index, test_dir, no_unpack, tmp):
        """Copy results and unpack."""
        log = logging.getLogger(MODULE)
        log.info("Copying TAR: %s %s", package, test_name)

        if no_unpack:
            tmp_tar = self.__get_tar(user, package, test_name, grid_index=grid_index, tmp=tmp)
            cp(tmp_tar, test_dir)
            os.remove(tmp_tar)
        else:
            tmp_tar = self.__get_tar(user, package, test_name, grid_index=grid_index, tmp=tmp)
            if tmp_tar is not None:
                tar = tarfile.open(tmp_tar)
                log.info("Unpacking TAR: %s", test_dir)
                tar.extractall(path=test_dir)
                tar.close()
                os.remove(tmp_tar)

        return 0

    def copy_to_dst(self, test_name, test_dir, dst_dir):
        """Copy to dst."""
        log = logging.getLogger(MODULE)

        # extra check if dst is already made
        dst_target = os.path.join(dst_dir, test_name)

        # create the directory
        if mkdir(dst_target) != 0:
            return 1

        exit_code = cp(test_dir, dst_target)

        # check number of source files
        nSrc = count_files(test_dir)
        nDst = count_files(dst_target)

        if nDst == nSrc:
            log.info("Number of files in Src (%d) and Dst (%d) are equal for %s", nSrc, nDst, test_name)
        else:
            log.warning("Number of files in Src (%d) and Dst (%d) differ for %s", nSrc, nDst, test_name)

        return exit_code

    def task_package(self, root, package, job_type, sequence_tag, inform_panda, no_action, config_file):
        """Submit a single package."""
        log = logging.getLogger(MODULE)
        result = {}
        number_of_tests = len(self.get_files(root, job_type, "all", self.nightly_release, self.project, self.platform))
        if number_of_tests > 0:
            print 'art-package:', package
            self.status('included')
            log.info('root %s with %d jobs', root, number_of_tests)
            log.info('Handling %s for %s project %s on %s', package, self.nightly_release, self.project, self.platform)

            run_dir = os.path.join(self.submit_directory, package, 'run')
            script_directory = self.copy_art('../python', run_dir)

            result = self.task(script_directory, package, job_type, sequence_tag, inform_panda, no_action, config_file)
        return result

    def task_list(self, job_type, sequence_tag, inform_panda, package=None, no_action=False, wait_and_copy=True, config_file=None):
        """Submit a list of packages."""
        log = logging.getLogger(MODULE)
        log.info("Inform Panda %s", inform_panda)

        # job will be submitted from tmp directory
        self.submit_directory = tempfile.mkdtemp(dir='.')

        # make sure tmp is removed afterwards
        atexit.register(shutil.rmtree, self.submit_directory, ignore_errors=True)

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
                    all_results.update(self.task_package(root, package, job_type, sequence_tag, inform_panda, no_action, config_file))
        else:
            # Submit single package
            root = test_directories[package]
            all_results.update(self.task_package(root, package, job_type, sequence_tag, inform_panda, no_action, config_file))

        if no_action:
            log.info("--no-action specified, so not waiting for results")
            return 0

        if len(all_results) == 0:
            log.warning('No tests found, nothing to submit.')
            return 0

        if not wait_and_copy:
            log.debug("No copying")
            return 0

        # wait for all results
        configuration = ArtConfiguration(config_file)

        executor = None
        future_set = []
        seq = None

        kinit_interval = ArtGrid.KINIT_WAIT  # ArtGrid.KINIT_WAIT * ArtGrid.RESULT_WAIT_INTERVAL
        result_wait_interval = ArtGrid.INITIAL_RESULT_WAIT_INTERVAL
        final_states = ["done", "finished", "failed", "aborted", "broken"]
        tmp = tempfile.mkdtemp(prefix=sequence_tag + '-')
        while len(all_results) > 0:
            log.debug("No of Results %d", len(all_results))
            log.debug("Waiting...")
            time.sleep(result_wait_interval)
            log.debug("Done Waiting")
            result_wait_interval = ArtGrid.RESULT_WAIT_INTERVAL
            kinit_interval -= 1
            if kinit_interval <= 0:
                os.system("kinit -R")
                kinit_interval = ArtGrid.KINIT_WAIT

            # force a copy of all_results since we are modifying all_results
            for jedi_id in list(all_results):
                package = all_results[jedi_id][0]
                # skip packages without copy
                if not configuration.get(self.nightly_release, self.project, self.platform, package, "copy"):
                    log.info("Copy not configured for %s - skipped", package)
                    del all_results[jedi_id]
                    continue

                # figure out the destination for the copy based on if the directory already exists, keep seq
                if seq is None:
                    dst = configuration.get(self.nightly_release, self.project, self.platform, package, "dst", ArtGrid.EOS_OUTPUT_DIR)
                    dst_dir = os.path.join(dst, self.nightly_release, self.project, self.platform, self.nightly_tag)
                    final_target = dst_dir
                    max_seq = 10
                    seq = 0
                    while ls(final_target) == 0 and seq < max_seq:
                        seq += 1
                        final_target = '-'.join((dst_dir, str(seq)))

                    if seq >= max_seq:
                        log.warning("Too many retries (>%d) to copy, removing job %d", max_seq, jedi_id)
                        del all_results[jedi_id]
                        continue

                    # create the directory
                    if mkdir(final_target) != 0:
                        log.warning("Could not create output dir %s, retrying later", final_target)
                        continue

                log.debug("Checking package %s for %s", package, str(jedi_id))
                status = self.task_status(jedi_id)
                if status is not None:

                    # job_name = all_results[jedi_id][1]
                    # outfile = all_results[jedi_id][2]
                    index = all_results[jedi_id][3]

                    # skip single jobs if status is not final
                    if (index > 0) and (status not in final_states):
                        continue

                    # create executor if not already done
                    if executor is None:
                        log.info("Executor started with %d threads", self.max_jobs)
                        executor = concurrent.futures.ThreadPoolExecutor(max_workers=self.max_jobs)

                    no_unpack = configuration.get(self.nightly_release, self.project, self.platform, package, "no_unpack", False)
                    indexed_package = package + ('.' + str(index) if index > 0 else '')
                    log.debug("Copy whatever ready from %s to %s using seq %d", indexed_package, dst, seq)
                    future_set.append(executor.submit(copy_job, self.art_directory, indexed_package, dst, no_unpack, tmp, seq))

                    # job in final state
                    if status in final_states:
                        # remove job from waiting queue
                        log.info("JediID %s finished with status %s", str(jedi_id), status)
                        del all_results[jedi_id]
                        log.info("Still waiting for results of %d jobs %s", len(all_results), all_results.keys())

                    log.debug("Still waiting for results of %d jobs %s", len(all_results), all_results.keys())

        if len(future_set) <= 0:
            log.info("No need to wait for any copy jobs")
            return 0

        # wait for all copy jobs to finish
        number_of_copy_jobs = len(future_set)
        log.info("Waiting for %d copy jobs to finish...", number_of_copy_jobs)
        for future in concurrent.futures.as_completed(future_set):
            (indexed_package, exit_code, out, err, start_time, end_time) = future.result()
            if exit_code == 0:
                log.debug("Copied %s exit_code: %d", indexed_package, exit_code)
                log.debug("  starting %s until %s", start_time.strftime('%Y-%m-%dT%H:%M:%S'), end_time.strftime('%Y-%m-%dT%H:%M:%S'))
            else:
                log.error("Failed to copy: %s exit_code: %d", indexed_package, exit_code)
                print err
                print out
            number_of_copy_jobs -= 1
            log.info("Still waiting for %d copy jobs to finish...", number_of_copy_jobs)

        log.info("All copy jobs finished.")
        return 0

    def task_status(self, jedi_id):
        """
        Wait for job to finish.

        Return final status of a task, or None if not finished
        """
        import requests
        import urllib3
        urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

        log = logging.getLogger(MODULE)

        # fake return for simulation
        if jedi_id == 0:
            return "done"

        try:
            payload = {'json': 'true'}
            url = 'https://bigpanda.cern.ch/task/' + str(jedi_id)
            r = requests.get(url, params=payload, verify=False)
            if r.status_code == requests.codes.ok:
                s = r.json()
                if (s is not None) and ('task' in s):
                    task = s['task']
                    if (task is not None) and ('status' in task):
                        status = task['status']
                        # if status in ["done", "finished", "failed", "aborted", "broken"]:
                        log.debug("Task: %s %s", str(jedi_id), str(status))
                        return status
        except requests.exceptions.RequestException, e:
            log.error('%s for %s status: %s', e, str(jedi_id), url)
        except httplib.IncompleteRead, e:
            log.error('%s for %s status: %s', e, str(jedi_id), url)
        return None

    def task_job(self, grid_options, sub_cmd, script_directory, sequence_tag, package, outfile, inform_panda, job_type='', number_of_tests=0, split=0, job_name='', inds=None, n_files=0, in_file=False, ncores=1, athena_mt=0, no_action=False):
        """
        Submit a batch or single job.

        Returns jedi_id or 0 if submission failed.

        # art-task-grid.sh [--no-action] batch <submit_directory> <script_directory> <sequence_tag> <package> <outfile> <inform_panda> <job_type> <number_of_tests>
        #
        # art-task-grid.sh [--no-action] single [--inds <input_file> --n-files <number_of_files> --split <split> --in] <submit_directory> <script_directory> <sequence_tag> <package> <outfile> <inform_panda> <job_name>
        """
        log = logging.getLogger(MODULE)
        cmd = ' '.join((os.path.join(self.art_directory, 'art-task-grid.sh'),
                        '--no-action' if no_action else '',
                        sub_cmd))

        if sub_cmd == 'single':
            cmd = ' '.join((cmd,
                            '--inds ' + str(inds) if inds is not None else '',
                            '--n-files ' + str(n_files) if inds is not None and n_files > 0 else '',
                            '--split ' + str(split) if inds is not None and split > 0 else '',
                            '--in' if inds is not None and str(in_file) else '',
                            '--ncore ' + str(ncores) if athena_mt == 0 and ncores > 1 else '',
                            '--athena_mt ' + str(athena_mt) if ncores == 1 and athena_mt > 0 else ''))

        cmd = ' '.join((cmd,
                        self.submit_directory,
                        script_directory,
                        sequence_tag,
                        package,
                        outfile,
                        str(inform_panda)))

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
        log.debug("Grid_options: %s", grid_options)
        env = os.environ.copy()
        env['PATH'] = '.:' + env['PATH']
        env['ART_GRID_OPTIONS'] = grid_options

        log.debug("ART_GRID_OPTIONS %s", env['ART_GRID_OPTIONS'])

        jedi_id = -1
        # run the command, no_action is forwarded and used inside the script
        (exit_code, out, err, command, start_time, end_time) = run_command(cmd, env=env)
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
        log = logging.getLogger(MODULE)
        if self.skip_setup:
            return ''

        configuration = ArtConfiguration(config_file)
        grid_options = configuration.get_option(self.nightly_release, self.project, self.platform, package, 'exclude-sites', '--excludedSite=')
        grid_options += ' ' + configuration.get_option(self.nightly_release, self.project, self.platform, package, 'sites', '--site=')
        log.info('grid_options: %s', grid_options)
        return grid_options

    def task(self, script_directory, package, job_type, sequence_tag, inform_panda, no_action=False, config_file=None):
        """
        Submit a task, consisting of multiple jobs.

        For 'single' jobs each task contains exactly one job.
        Returns a map of jedi_id to (package, test_name, out_file, seq)
        """
        log = logging.getLogger(MODULE)
        log.info('Running art task')

        grid_options = self.get_grid_options(package, config_file)

        test_directories = self.get_test_directories(self.get_script_directory())
        test_directory = test_directories[package]
        number_of_batch_tests = len(self.get_files(test_directory, job_type, "batch", self.nightly_release, self.project, self.platform))

        user = os.getenv('USER', 'artprod') if self.skip_setup else ArtGrid.ARTPROD
        outfile = self.rucio.get_outfile_name(user, package, sequence_tag)

        result = {}

        # submit batch tests, index = 0
        if number_of_batch_tests > 0:
            self.exit_if_outfile_too_long(outfile)

            # Batch
            log.info("Batch")
            jedi_id = self.task_job(grid_options, "batch", script_directory, sequence_tag, package, outfile, inform_panda, job_type=job_type, number_of_tests=number_of_batch_tests, no_action=no_action)
            if jedi_id > 0:
                result[jedi_id] = (package, "", outfile, 0, None)

        # submit single tests, index > 1
        index = 1
        for job_name in self.get_files(test_directory, job_type, "single", self.nightly_release, self.project, self.platform):
            job = os.path.join(test_directory, job_name)
            header = ArtHeader(job)
            inds = header.get(ArtHeader.ART_INPUT)
            n_files = header.get(ArtHeader.ART_INPUT_NFILES)
            split = header.get(ArtHeader.ART_INPUT_SPLIT)
            ncores = header.get(ArtHeader.ART_CORES)
            athena_mt = header.get(ArtHeader.ART_ATHENA_MT)

            outfile_test = self.rucio.get_outfile_name(user, package, sequence_tag, str(index))
            self.exit_if_outfile_too_long(outfile_test)

            # Single
            log.info("Single")
            jedi_id = self.task_job(grid_options, "single", script_directory, sequence_tag, package, outfile_test, inform_panda, split=split, job_name=job_name, inds=inds, n_files=n_files, in_file=True, ncores=ncores, athena_mt=athena_mt, no_action=no_action)

            if jedi_id > 0:
                result[jedi_id] = (package, job_name, outfile_test, index, None)

            index += 1

        return result

    def batch(self, sequence_tag, package, out, inform_panda, job_type, job_index):
        """Run a single job by job_index of a 'batch' submission."""
        log = logging.getLogger(MODULE)
        log.info('Running art grid batch')
        log.info("%s %s %s %s %s %s %s %s %s", self.nightly_release, self.project, self.platform, self.nightly_tag, package, job_type, str(job_index), out, inform_panda)

        test_directories = self.get_test_directories(self.get_script_directory())
        test_directory = test_directories[package]

        test_list = self.get_files(test_directory, job_type, "batch", self.nightly_release, self.project, self.platform)

        # NOTE: grid counts from 1
        index = int(job_index)
        job_name = test_list[index - 1]

        in_file = None

        return self.job(test_directory, package, job_name, job_type, out, inform_panda, in_file)

    def single(self, sequence_tag, package, out, inform_panda, job_name, in_file):
        """Run a single job by name of a 'single' submission."""
        log = logging.getLogger(MODULE)

        log.info('Running art grid single')
        log.info("%s %s %s %s %s %s %s %s %s", self.nightly_release, self.project, self.platform, self.nightly_tag, package, job_name, out, inform_panda, in_file)

        test_directories = self.get_test_directories(self.get_script_directory())
        test_directory = test_directories[package]

        job_type = 'grid'
        return self.job(test_directory, package, job_name, job_type, out, inform_panda, in_file)

    def job(self, test_directory, package, job_name, job_type, out, inform_panda, in_file):
        """Run a job."""
        log = logging.getLogger(MODULE)

        log.info("art-job-name: %s", job_name)
        panda_id = os.getenv('PandaID', '0')
        if inform_panda == 'True':
            # informing panda, ignoring errors for now
            self.inform_panda(panda_id, job_name, package)

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

        header = ArtHeader(test_file)
        athena_mt = header.get(ArtHeader.ART_ATHENA_MT)
        ncores = header.get(ArtHeader.ART_CORES)
        if athena_mt == 0 and ncores > 1:
            nthreads = header.get(ArtHeader.ART_INPUT_NFILES)
            (exit_code, output, error, command, start_time, end_time) = run_command_parallel(command, nthreads, ncores, env=env)
        else:
            (exit_code, output, error, command, start_time, end_time) = run_command(command, env=env)
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
        result['panda_id'] = panda_id

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
        files = set()

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
                        files.add(out_name)

        # pick up art-header named outputs
        for path_name in ArtHeader(test_file).get(ArtHeader.ART_OUTPUT):
            for out_name in glob.glob(path_name):
                files.add(out_name)

        tar_file = tarfile.open(out, mode='w')
        for file in files:
            log.info('Tar file contains: %s', file)
            tar_file.add(file)

        tar_file.close()
        # Always return 0
        return 0

    def inform_panda(self, panda_id, job_name, package):
        """Inform panda about the job we are running using panda ID."""
        log = logging.getLogger(MODULE)
        import requests

        url = "http://bigpanda.cern.ch/art/registerarttest/?json"
        n_attempts = 3
        timeout = 10

        payload = {}
        payload['pandaid'] = panda_id
        payload['testname'] = job_name
        payload['nightly_release_short'] = self.nightly_release_short
        payload['platform'] = self.platform
        payload['project'] = self.project
        payload['package'] = package
        payload['nightly_tag'] = self.nightly_tag

        headers = {'User-agent': 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36'}

        for i in range(0, n_attempts):
            reply = requests.post(url, data=payload, headers=headers, timeout=timeout, verify=False)
            try:
                reply = requests.post(url, data=payload, timeout=timeout, verify=False)
                log.info('Informed panda about %s %s %s', panda_id, job_name, package)
            except:
                log.warning('Exception occured for %s %s %s', panda_id, job_name, package)
                continue

            if reply.status_code == 200:
                try:
                    reply = reply.json()
                except:
                    log.error('The panda inform response was corrupted for %s %s %s', panda_id, job_name, package)
                    raise
                if 'exit_code' in reply and reply['exit_code'] == 0:
                    return True

        log.error('Panda could not be informed about %s %s %s', panda_id, job_name, package)
        return False

    def list(self, package, job_type, index_type, json_format, user):
        """List all jobs available."""
        user = ArtGrid.ARTPROD if user is None else user

        # make sure script directory exist
        self.exit_if_no_script_directory()

        json_array = []
        for entry in self.rucio.get_table(user, package):
            # print entry
            json_array.append({
                'name': entry['job_name'],
                'grid_index': entry['grid_index'],
                'job_index': entry['job_index'],
                'single_index': entry['single_index'],
                'file_index': entry['file_index'],
                'outfile': entry['outfile']
            })

        if json_format:
            json.dump(json_array, sys.stdout, sort_keys=True, indent=4)
            return 0

        i = 0
        print "Example FileName: user.artprod.atlas.21.0.Athena.x86_64-slc6-gcc62-opt.2018-02-25T2154.314889.TrigInDetValidation.<Single>"
        print "Example OutputName: user.artprod.<Job>.EXT1._<Grid>.tar.<File>"
        print
        print '{:-^5}'.format('Index'), \
              '{:-^60}'.format('Name'), \
              '{:-^6}'.format('Grid'), \
              '{:-^9}'.format('Job'), \
              '{:-^6}'.format('Single'), \
              '{:-^4}'.format('File'), \
              '{:-^80}'.format('FileName')

        for entry in json_array:
            print '{:5d}'.format(i), \
                  '{:60}'.format('None' if entry['name'] is None else entry['name']), \
                  '{:06d}'.format(entry['grid_index']), \
                  '{:9d}'.format(entry['job_index']), \
                  '{:6d}'.format(entry['single_index']), \
                  '{:4d}'.format(entry['file_index']), \
                  '{:80}'.format(entry['outfile'])
            i += 1

        return 0

    def log(self, package, test_name, user):
        """Print the log of a job."""
        log = logging.getLogger(MODULE)
        user = ArtGrid.ARTPROD if user is None else user

        # make sure script directory exist
        self.exit_if_no_script_directory()

        tmp_tar = self.__get_tar(user, package, test_name, tar=False)
        if tmp_tar is None:
            log.error("No log tar file found")
            return 1

        tar = tarfile.open(tmp_tar)
        for name in tar.getnames():
            if ArtRucio.ATHENA_STDOUT in name:
                f = tar.extractfile(name)
                content = f.read()
                print content
                break
        tar.close()
        os.remove(tmp_tar)
        return 0

    def output(self, package, test_name, user):
        """Download the output of a job."""
        log = logging.getLogger(MODULE)
        user = ArtGrid.ARTPROD if user is None else user

        # make sure script directory exist
        self.exit_if_no_script_directory()

        outfile = self.rucio.get_outfiles(user, package)[0]
        if not outfile.endswith(package):
            # remove .13
            outfile = os.path.splitext(outfile)[0]
        job_name = os.path.splitext(test_name)[0]
        tar_dir = os.path.join(tempfile.gettempdir(), outfile, job_name)
        mkdir(tar_dir)

        tmp_tar = self.__get_tar(user, package, test_name)
        if tmp_tar is None:
            log.error("No output tar file found")
            return 1

        tar = tarfile.open(tmp_tar)
        tar.extractall(path=tar_dir)
        tar.close()
        os.remove(tmp_tar)

        print "Output extracted in", tar_dir

        return 0

    def compare(self, package, test_name, days, user, files, entries=-1, mode='detailed', shell=False):
        """Compare current output against a job of certain days ago."""
        log = logging.getLogger(MODULE)
        user = ArtGrid.ARTPROD if user is None else user

        previous_nightly_tag = self.get_previous_nightly_tag(days)
        log.info("LOG Previous Nightly Tag: %s", str(previous_nightly_tag))

        if previous_nightly_tag is None:
            log.error("No previous nightly tag found")
            return 1

        ref_dir = os.path.join('.', 'ref-' + previous_nightly_tag)
        mkdir(ref_dir)

        log.info("Shell = %s", shell)
        tmp_tar = self.__get_tar(user, package, test_name, nightly_tag=previous_nightly_tag, shell=shell)
        if tmp_tar is None:
            log.error("No comparison tar file found")
            return 1

        tar = tarfile.open(tmp_tar)
        for member in tar.getmembers():
            tar.extractall(path=ref_dir, members=[member])
        tar.close()
        os.remove(tmp_tar)

        return self.compare_ref('.', ref_dir, files, entries, mode)

    def __get_tar(self, user, package, test_name, grid_index=-1, tmp=None, tar=True, nightly_tag=None, shell=False):
        """Open tar file for particular release."""
        log = logging.getLogger(MODULE)
        log.debug("Tar: %s", tar)
        tmp = tempfile.gettempdir() if tmp is None else tmp
        nightly_tag = self.nightly_tag if nightly_tag is None else nightly_tag
        job_name = os.path.splitext(test_name)[0]

        max_tries = 3
        wait_time = 5  # mins

        tries = max_tries
        while tries > 0:
            try:
                for entry in self.rucio.get_table(user, package, nightly_tag, shell, tmp):
                    if entry['job_name'] == job_name and (grid_index < 0 or entry['grid_index'] == grid_index):

                        log.debug("index %d", entry['grid_index'])
                        rucio_name = self.__get_rucio_name(user, entry, 'tar' if tar else 'log')

                        log.debug("RUCIO: %s", rucio_name)

                        tmp_dir = tempfile.mkdtemp()
                        atexit.register(shutil.rmtree, tmp_dir, ignore_errors=True)

                        log.debug("Shell = %s", shell)
                        exit_code = self.rucio.download(rucio_name, tmp_dir, shell)
                        if exit_code == 0:
                            tmp_tar = os.path.join(tmp_dir, 'user.' + user, rucio_name)
                            return tmp_tar

            except exceptions.Exception, e:
                log.warning('(Rucio) Exception: %s in %s', str(e.code), str(e))
                log.info("Waiting %d mins", wait_time)
                tries -= 1
                time.sleep(wait_time * 60)
                continue

            log.error("No log or tar found for package %s or test %s", package, test_name)
            return None

        log.error("Too many (%d) (Rucio) Exceptions", max_tries)
        return None

    def __get_rucio_name(self, user, entry, file_type):
        rucio_name = None
        if file_type == 'json':
            rucio_name = '.'.join(('user', user, str(entry['job_index']), 'EXT0', '_{0:06d}'.format(entry['grid_index']), 'art-job', 'json'))
        elif file_type == 'tar':
            rucio_name = '.'.join(('user', user, str(entry['job_index']), 'EXT1', '_{0:06d}'.format(entry['grid_index']), 'tar'))
        else:
            rucio_name = '.'.join((entry['outfile'], 'log', str(entry['job_index']), '{0:06d}'.format(entry['grid_index']), 'log.tgz'))

        if entry['file_index'] > 0:
            rucio_name = '.'.join((rucio_name, str(entry['file_index'])))

        return rucio_name

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
                offset = timedelta(hours=3)
                nightly_tag_dt = datetime.strptime(self.nightly_tag, fmt) + offset
                from_dt = nightly_tag_dt.replace(hour=0, minute=0, second=0, microsecond=0) - timedelta(days=days)
                to_dt = from_dt + timedelta(days=1)
                tag_dt = datetime.strptime(tag, fmt) + offset
                within_days = from_dt <= tag_dt and tag_dt < to_dt
                target_exists = len(glob.glob(os.path.join(directory, tag, self.project, '*', 'InstallArea', self.platform))) > 0
                if within_days and target_exists:
                    return tag
        return None

    def createpoolfile(self):
        """Create 'empty' poolfile catalog."""
        path = os.path.join('.', 'PoolFileCatalog.xml')
        with open(path, 'w+') as pool_file:
            pool_file.write('<!-- Edited By POOL -->\n')
            pool_file.write('<!DOCTYPE POOLFILECATALOG SYSTEM "InMemory">\n')
            pool_file.write('<POOLFILECATALOG>\n')
            pool_file.write('</POOLFILECATALOG>\n')

        return 0
