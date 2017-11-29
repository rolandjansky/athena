#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""TBD."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import atexit
import datetime
import glob
import json
import os
import re
import shutil
import sys
import tarfile
import tempfile


from art_base import ArtBase
from art_header import ArtHeader
from art_misc import mkdir_p, make_executable, check, run_command


class ArtGrid(ArtBase):
    """TBD."""

    def __init__(self, art_directory, nightly_release, project, platform, nightly_tag, script_directory=None, skip_setup=False, submit_directory=None):
        """TBD."""
        self.art_directory = art_directory
        self.nightly_release = nightly_release
        self.project = project
        self.platform = platform
        self.nightly_tag = nightly_tag
        self.script_directory = script_directory
        self.skip_setup = skip_setup
        self.submit_directory = submit_directory

        self.cvmfs_directory = '/cvmfs/atlas-nightlies.cern.ch/repo/sw'

    def get_script_directory(self):
        """On demand script directory, only to be called if directory exists."""
        if self.script_directory is None:
            self.script_directory = self.cvmfs_directory
            self.script_directory = os.path.join(self.script_directory, self.nightly_release, self.nightly_tag, self.project)
            self.script_directory = os.path.join(self.script_directory, os.listdir(self.script_directory)[0])  # e.g. 21.0.3
            self.script_directory = os.path.join(self.script_directory, os.listdir(self.script_directory)[0], self.platform)  # InstallArea/x86_64-slc6-gcc62-opt
        return self.script_directory

    def is_script_directory_in_cvmfs(self):
        """Return true if the script directory is in cvmfs."""
        return self.get_script_directory().startswith(self.cvmfs_directory)

    def task_list(self, type, sequence_tag):
        """TBD."""
        # job will be submitted from tmp directory
        submit_directory = tempfile.mkdtemp(dir='.')

        # make sure tmp is removed afterwards
        atexit.register(shutil.rmtree, submit_directory)

        config = None if self.skip_setup else self.get_config()

        # make sure script directory exist
        if not os.path.isdir(self.get_script_directory()):
            print 'ERROR: script directory does not exist: %s' % self.get_script_directory()
            print 'art-status: error'
            sys.stdout.flush()
            exit(1)

        # get the test_*.sh from the test directory
        test_directories = self.get_test_directories(self.get_script_directory())
        if not test_directories:
            print 'WARNING: No tests found in directories ending in "test"'
            sys.stdout.flush()

        for package, root in test_directories.items():
            if self.excluded(config, package):
                print 'Excluding ' + package + ' for ' + self.nightly_release + ' project ' + self.project + ' on ' + self.platform
                print 'art-package: ' + package
                print 'art-status: excluded'
                sys.stdout.flush()
            else:
                shell_files = self.get_files(root, type)
                number_of_tests = len(shell_files)
                if number_of_tests > 0:
                    print 'art-package: ' + package
                    print 'art-status: included'
                    print 'root' + root
                    print 'Handling ' + package + ' for ' + self.nightly_release + ' project ' + self.project + ' on ' + self.platform
                    print "Number of tests: " + str(number_of_tests)
                    sys.stdout.flush()
                    submit_dir = os.path.join(submit_directory, package)
                    run = os.path.join(submit_dir, "run")
                    ART = os.path.join(run, "ART")
                    mkdir_p(ART)

                    # get the path of the python classes and support scripts
                    art_python_directory = os.path.join(self.art_directory, '..', 'python', 'ART')

                    shutil.copy(os.path.join(self.art_directory, 'art.py'), run)
                    shutil.copy(os.path.join(self.art_directory, 'art-get-tar.sh'), run)
                    shutil.copy(os.path.join(self.art_directory, 'art-internal.py'), run)
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

                    make_executable(os.path.join(run, 'art.py'))
                    make_executable(os.path.join(run, 'art-get-tar.sh'))
                    make_executable(os.path.join(run, 'art-internal.py'))

                    # copy a local test directory if needed (only for 'art grid')
                    if not self.is_script_directory_in_cvmfs():
                        local_test_dir = os.path.join(run, os.path.basename(os.path.normpath(self.get_script_directory())))
                        print "Copying script directory for grid submission to ", local_test_dir
                        shutil.copytree(self.get_script_directory(), local_test_dir)

                    command = os.path.join(self.art_directory, 'art-internal.py') + ' task grid ' + ('--skip-setup' if self.skip_setup else '') + ' ' + submit_directory + ' ' + self.get_script_directory() + ' ' + package + ' ' + type + ' ' + sequence_tag + ' ' + self.nightly_release + ' ' + self.project + ' ' + self.platform + ' ' + self.nightly_tag
                    print command
                    sys.stdout.flush()

                    env = os.environ.copy()
                    env['PATH'] = '.:' + env['PATH']
                    out = check(run_command(command, env=env))
                    print out
                    sys.stdout.flush()
        return 0

    def task(self, package, type, sequence_tag):
        """TBD."""
        print 'Running art task'
        sys.stdout.flush()

        number_of_tests = len(self.get_list(self.get_script_directory(), package, type))

        config = None if self.skip_setup else self.get_config()
        grid_options = self.grid_option(config, package, 'grid-exclude-sites', '--excludedSite=')
        grid_options += ' ' + self.grid_option(config, package, 'grid-sites', '--site=')

        print self.nightly_release + " " + self.project + " " + self.platform + " " + self.nightly_tag + " " + sequence_tag + " " + package + " " + type + " " + str(number_of_tests) + "  " + grid_options
        sys.stdout.flush()

        # run task from Bash Script as is needed in ATLAS setup
        # FIXME we need to parse the output
        env = os.environ.copy()
        env['PATH'] = '.:' + env['PATH']
        env['ART_GRID_OPTIONS'] = grid_options
        out = check(run_command(os.path.join(self.art_directory, 'art-task-grid.sh') + " " + ('--skip-setup' if self.skip_setup else '') + " " + self.submit_directory + " " + self.get_script_directory() + " " + package + " " + type + " " + sequence_tag + " " + str(number_of_tests) + " " + self.nightly_release + " " + self.project + " " + self.platform + " " + self.nightly_tag, env=env))
        print out
        sys.stdout.flush()
        return 0

    def job(self, package, type, sequence_tag, index, out):
        """TBD."""
        print 'Running art job grid'
        sys.stdout.flush()

        index = int(index)

        print self.nightly_release + " " + self.project + " " + self.platform + " " + self.nightly_tag + " " + package + " " + type + " " + str(index) + " " + out
        sys.stdout.flush()

        test_directories = self.get_test_directories(self.get_script_directory())
        test_dir = test_directories[package]
        test_list = self.get_files(test_dir, type)

        # minus one for grid
        test_name = test_list[index - 1]
        test_file = os.path.join(test_dir, test_name)
        com = '%s %s %s %s %s %s %s %s %s' % (test_file, self.get_script_directory(), package, type, test_name, self.nightly_release, self.project, self.platform, self.nightly_tag)

        print test_name
        print test_dir
        print com
        sys.stdout.flush()

        # run the test
        env = os.environ.copy()
        env['PATH'] = '.:' + env['PATH']
        print check(run_command(com, env=env))
        sys.stdout.flush()

        # pick up the outputs
        tar_file = tarfile.open(out, mode='w')

        # pick up explicitly named output files
        with open(test_file, "r") as f:
            for line in f:
                out_names = re.findall(r"--output[^\s=]*[= ]*(\S*)", line)
                print out_names
                for out_name in out_names:
                    out_name = out_name.strip('\'"')
                    if os.path.exists(out_name):
                        print 'Tar file contain: ', out_name
                        tar_file.add(out_name)

        # pick up art-header named outputs
        for path_name in ArtHeader(test_file).get('art-output'):
            for out_name in glob.glob(path_name):
                print 'Tar file contain: ', out_name
                tar_file.add(out_name)

        tar_file.close()
        return 0

    def list(self, package, type, json_format=False):
        """TBD."""
        jobs = self.get_list(self.get_script_directory(), package, type)
        if json_format:
            json.dump(jobs, sys.stdout, sort_keys=True, indent=4)
            return 0
        i = 1
        for job in jobs:
            print str(i) + ' ' + job
            i += 1
        sys.stdout.flush()
        return 0

    def log(self, package, test_name):
        """TBD."""
        tar = self.get_tar(package, test_name, '.log')

        for name in tar.getnames():
            if 'athena_stdout.txt' in name:
                f = tar.extractfile(name)
                content = f.read()
                print content
                break
        tar.close()
        return 0

    def output(self, package, test_name, file_name):
        """TBD."""
        tar = self.get_tar(package, test_name, '_EXT0')

        for member in tar.getmembers():
            if file_name in member.name:
                tar.extractall(path='.', members=[member])
                break
        tar.close()
        return 0

    def compare(self, package, test_name, days, file_names):
        """TBD."""
        previous_nightly_tag = self.get_previous_nightly_tag(days)
        print "Previous Nightly Tag:", str(previous_nightly_tag)
        if previous_nightly_tag is None:
            print "ERROR: No previous nightly tag found"
            # do not flag as error, to make sure tar file gets uploaded
            return 0

        ref_dir = os.path.join('.', 'ref-' + previous_nightly_tag)
        mkdir_p(ref_dir)

        tar = self.get_tar(package, test_name, '_EXT0', previous_nightly_tag)
        if tar is None:
            print "ERROR: No comparison tar file found"
            # do not flag as error, to make sure tar file gets uploaded
            return 0

        for member in tar.getmembers():
            if member.name in file_names:
                tar.extractall(path=ref_dir, members=[member])
        tar.close()

        for file_name in file_names:
            ref_file = os.path.join(ref_dir, file_name)
            if os.path.isfile(ref_file):
                print "art-compare: " + previous_nightly_tag + " " + file_name
                self.compare_ref(file_name, ref_file, 10)
            else:
                print "ERROR:", ref_file, "not found in tar file"
        # do not flag as error, to make sure tar file gets uploaded
        return 0

    #
    # Protected Methods
    #
    def excluded(self, config, package):
        """Based on config, decide if a release is excluded from testing."""
        # NOTE changes in here should be reflected in art-submit/art-gitlab.py
        if config is None:
            return False

        if self.nightly_release not in config['releases'].keys():
            return True

        if self.project not in config['releases'][self.nightly_release].keys():
            return True

        if self.platform not in config['releases'][self.nightly_release][self.project].keys():
            return True

        # no packages listed -> included
        if config['releases'][self.nightly_release][self.project][self.platform] is None:
            return False

        # package not listed -> included
        if package not in config['releases'][self.nightly_release][self.project][self.platform].keys():
            return False

        return config['releases'][self.nightly_release][self.project][self.platform][package].get('excluded', False)

    def grid_option(self, config, package, key, option_key):
        """Based on config, return value for key, or ''."""
        if config is None:
            return ''

        if self.nightly_release not in config['releases'].keys():
            return ''

        if self.project not in config['releases'][self.nightly_release].keys():
            return ''

        if self.platform not in config['releases'][self.nightly_release][self.project].keys():
            return ''

        if config['releases'][self.nightly_release][self.project][self.platform] is None:
            return ''

        prefix = config.get(key, None)
        if package not in config['releases'][self.nightly_release][self.project][self.platform].keys():
            return '' if prefix is None else option_key + prefix

        value = config['releases'][self.nightly_release][self.project][self.platform][package].get(key, None)

        if prefix is None:
            return '' if value is None else option_key + value
        else:
            return option_key + prefix + ('' if value is None else ', ' + value)

    def get_tar(self, package, test_name, extension, nightly_tag=None):
        """Open tar file for particular release."""
        if nightly_tag is None:
            nightly_tag = self.nightly_tag

        try:
            type = self.get_type(self.get_test_directories(self.get_script_directory())[package], test_name)
            print "Type:", type
            files = self.get_list(self.get_script_directory(), package, type)
            number_of_tests = len(files)
            index = files.index(test_name)
            print "Index:", index
        except KeyError:
            print package, "does not exist in tests of ", self.get_script_directory()
            return None

        try:
            tmpdir = os.environ['TMPDIR']
        except KeyError:
            tmpdir = '.'
        print "Using", tmpdir, "for tar file download"

        # run in correct environment
        env = os.environ.copy()
        env['PATH'] = '.:' + env['PATH']

        # Grid counts from 1, retries will up that number every time by total number of jobs
        index += 1
        retries = 3
        retry = 0

        (code, out, err) = (0, "", "")
        while retry < retries:
            try_index = (retry * number_of_tests) + index
            print (retry + 1), ": Get tar for index ", try_index
            # run art-get-tar.sh
            (code, out, err) = run_command(os.path.join(self.art_directory, "art-get-tar.sh") + " " + package + " " + str(try_index) + " _EXT0 " + self.nightly_release + " " + self.project + " " + self.platform + " " + nightly_tag, dir=tmpdir, env=env)
            if code == 0 and out != '':

                match = re.search(r"TAR_NAME=(.*)", out)
                if match:
                    tar_name = match.group(1)
                    print "Matched TAR_NAME ", tar_name

                    if tar_name != "":
                        print "Tar Name:", tar_name
                        break

            retry += 1

        if retry >= retries:
            print "Code: " + str(code)
            print "Err: " + err
            print "Out: " + out
            return None

        return tarfile.open(os.path.join(tmpdir, tar_name.replace(':', '/', 1)))

    def get_previous_nightly_tag(self, days):
        """TBD. 21:00 is cutoff time."""
        directory = os.path.join(self.cvmfs_directory, self.nightly_release)
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
