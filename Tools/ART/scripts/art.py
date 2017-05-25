#!/usr/bin/env python
"""TBD."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"
__doc__ = """ART - ATLAS Nightly Release Tester script"""

import argparse
import datetime
import errno
import fnmatch
import glob
import os
import re
import requests
# import scandir
import shlex
import shutil
import subprocess
import sys
import tarfile
import time
import yaml


def run_command(cmd, dir=None, shell=False):
    """Run the given command locally and returns the output, err and exit_code."""
    print "Execute: " + cmd
    if "|" in cmd:
        cmd_parts = cmd.split('|')
    else:
        cmd_parts = []
        cmd_parts.append(cmd)
    i = 0
    p = {}
    for cmd_part in cmd_parts:
        cmd_part = cmd_part.strip()
        if i == 0:
            p[i] = subprocess.Popen(shlex.split(cmd_part), stdin=None, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=dir, shell=shell)
        else:
            p[i] = subprocess.Popen(shlex.split(cmd_part), stdin=p[i - 1].stdout, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=dir, shell=shell)
        i = i + 1
    (output, err) = p[i - 1].communicate()
    exit_code = p[0].wait()

    return exit_code, str(output), str(err)


def check((exitcode, out, err)):
    """Check exitcode and print statement and exit if needed."""
    if exitcode == 0:
        print err
        return out

    print "Error:", exitcode
    print "StdOut:", out
    print "StdErr:", err

    print 'art-status: error'

    exit(exitcode)


def make_executable(path):
    """TBD."""
    mode = os.stat(path).st_mode
    mode |= (mode & 0o444) >> 2    # copy R bits to X
    os.chmod(path, mode)


def mkdir_p(path):
    """TBD."""
    try:
        os.makedirs(path)
    except OSError as exc:  # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise


def count_string_occurrence(path, string):
    """TBD."""
    for file in glob.iglob(path):
        print file
        sys.stdout.flush()
        f = open(file)
        contents = f.read()
        f.close()
        return contents.count(string)
    return 0


class Art(object):
    """TBD."""

    SCRIPT_DIRECTORY = '/cvmfs/atlas-nightlies.cern.ch/repo/sw'
    NIGHTLY_RELEASE_HELP = "name of the nightly tag (e.g. 21.0)"
    PROJECT_HELP = "name of the project (e.g. Athena)"
    PLATFORM_HELP = "platform (e.g. x86_64-slc6-gcc62-opt)"
    NIGHTLY_TAG_HELP = "nightly tag (e.g. 2017-02-26T2119)"
    SEQUENCE_TAG_HELP = "sequence tag (e.g. 0 or JOB_ID)"
    PACKAGE_HELP = "package of the test"

    # use epilog
    def __init__(self):
        """TBD."""
        parser = argparse.ArgumentParser(
            description='ART - ATLAS Release Tester',
            usage='''art <command> [<args>]

Possible art commands are:
   submit       Submit tests to the grid
   retrieve     Wait for tests to finish and retrieve the results
   included     Check if a release and platform is included
   wait_for     Wait for the release to be available
   task         Runs a single task, consisting of given number of jobs
   job          Runs a single job, given a particular index
   list         Lists the jobs of a package
   log          Show the log of a job
   output       Get the output of a job
   compare      Compare the output of a job
''')
        parser.add_argument("-v", "--verbose", help="increase output verbosity", action="store_true")
        parser.add_argument('command', help='Subcommand to run')
        # parse_args defaults to [1:] for args, but you need to
        # exclude the rest of the args too, or validation will fail
        args = parser.parse_args(sys.argv[1:2])
        if not hasattr(self, args.command):
            print 'ERROR: Unrecognized command \'' + args.command + '\''
            parser.print_help()
            sys.stdout.flush()
            exit(1)
        # use dispatch pattern to invoke method with same name
        getattr(self, args.command)()

    def get_config(self):
        """TBD."""
        config_file = open("art-configuration.yml", "r")
        config = yaml.load(config_file)
        config_file.close()
        return config

    def excluded(self, config, nightly_release, project, platform, package):
        """TBD."""
        if nightly_release not in config.keys():
            return True

        if project not in config[nightly_release].keys():
            return True

        if platform not in config[nightly_release][project].keys():
            return True

        excludes = config[nightly_release][project][platform]
        if excludes is not None and package in excludes:
            return True

        return False

    def included(self):
        """TBD."""
        parser = argparse.ArgumentParser(
            description='Check if a release and platform is included')
        parser.add_argument('nightly_release', help=Art.NIGHTLY_RELEASE_HELP)
        parser.add_argument('project', help=Art.PROJECT_HELP)
        parser.add_argument('platform', help=Art.PLATFORM_HELP)
        parser.add_argument('test_name', nargs='?', help="name of the test", default="__name_never_used__")
        # now that we're inside a subcommand, ignore the first two args
        args = parser.parse_args(sys.argv[2:])
        print 'Running art included'
        sys.stdout.flush()

        if self.excluded(self.get_config(), args.nightly_release, args.project, args.platform, args.test_name):
            print 'Excluding ' + 'all' + ' for ' + args.nightly_release + ' project ' + args.project + ' on ' + args.platform
            print 'art-status: excluded'
            sys.stdout.flush()
            exit(1)
        else:
            print 'art-status: included'
            exit(0)

    # @profile
    def get_test_directories(self, nightly_release, project, platform, nightly_tag):
        """
        Search from 'SCRIPT_DIRECTORY/<nightly_release>/<nightly_tag>/<project>' for '<package>/test' directories.

        The directories need to have the <platform> in their name.
        A dictionary key=<package>, value=<directory> is returned
        """
        result = {}
        search = os.path.join(Art.SCRIPT_DIRECTORY, nightly_release, nightly_tag, project)
        search = os.path.join(search, os.listdir(search)[0])  # e.g. 21.0.3
        search = os.path.join(search, os.listdir(search)[0], platform)  # InstallArea/x86_64-slc6-gcc62-opt
        for root, dirs, files in os.walk(search):
            if root.endswith('/test'):
                package = os.path.basename(os.path.dirname(root))
                result[package] = root
        return result

    def get_files(self, directory, queue):
        """Return a list of all test files matching 'test_*.sh' of given 'queue'."""
        result = list()
        if directory is not None:
            files = os.listdir(directory)
            files.sort()
            for fname in files:
                if fnmatch.fnmatch(fname, 'test_*.sh'):
                    full_name = os.path.join(directory, fname)
                    for line in open(full_name, "r"):
                        line_match = re.match(r'#art-(\w+):\s+(.+)$', line)
                        if line_match and line_match.group(1) == 'queue':
                            # accept tests with long queue
                            if line_match.group(2) == queue:
                                result.append(fname)
        return result

    def wait_for(self):
        """TBD."""
        parser = argparse.ArgumentParser(
            description='Wait for the release to be available')
        parser.add_argument('nightly_release', help=Art.NIGHTLY_RELEASE_HELP)
        parser.add_argument('project', help=Art.PROJECT_HELP)
        parser.add_argument('platform', help=Art.PLATFORM_HELP)
        parser.add_argument('nightly_tag', help=Art.NIGHTLY_TAG_HELP)
        # now that we're inside a subcommand, ignore the first two args
        args = parser.parse_args(sys.argv[2:])
        print 'Running art wait_for'
        sys.stdout.flush()

        directory = os.path.join(Art.SCRIPT_DIRECTORY, args.nightly_release, args.nightly_tag)
        path = os.path.join(directory, args.nightly_release + "__" + args.project + "__" + args.platform + "*" + args.nightly_tag + "__*.ayum.log")

        count = 0
        needed = 1
        value = count_string_occurrence(path, "Install looks to have been successful")
        print "art-status: waiting"
        print path
        print "count: " + str(value) + " mins: " + str(count)
        sys.stdout.flush()
        while (value < needed) and (count < 30):
            time.sleep(60)
            count += 1
            value = count_string_occurrence(path, "Install looks to have been successful")
            print "count: " + str(value) + " mins: " + str(count)
            sys.stdout.flush()

        if value < needed:
            print "art-status: no release"
            sys.stdout.flush()
            sys.exit(-2)

        print "art-status: setup"
        sys.stdout.flush()
        sys.exit(0)

    def submit(self):
        """TBD."""
        parser = argparse.ArgumentParser(
            description='Submit tests to the grid')
        parser.add_argument('nightly_release', help=Art.NIGHTLY_RELEASE_HELP)
        parser.add_argument('project', help=Art.PROJECT_HELP)
        parser.add_argument('platform', help=Art.PLATFORM_HELP)
        parser.add_argument('nightly_tag', help=Art.NIGHTLY_TAG_HELP)
        parser.add_argument('sequence_tag', help=Art.SEQUENCE_TAG_HELP)
        # now that we're inside a subcommand, ignore the first two args
        args = parser.parse_args(sys.argv[2:])
        print 'Running art submit'
        sys.stdout.flush()

        # get the path of the art.py script
        art_dir = os.path.dirname(os.path.realpath(sys.argv[0]))

        # job will be submitted from tmp directory
        submit_directory = 'tmp'

        # make sure tmp is removed
        if os.path.exists(submit_directory):
            shutil.rmtree(submit_directory)

        config = self.get_config()

        # make sure script directory exist
        if not os.path.isdir(Art.SCRIPT_DIRECTORY):
            print 'ERROR: script directory does not exist: %s' % Art.SCRIPT_DIRECTORY
            print 'art-status: error'
            sys.stdout.flush()
            exit(1)

        # get the test_*.sh from the test directory
        test_directories = self.get_test_directories(args.nightly_release, args.project, args.platform, args.nightly_tag)
        if not test_directories:
            print 'No tests found in directories ending in "test"'
            sys.stdout.flush()

        for package, root in test_directories.items():
            if self.excluded(config, args.nightly_release, args.project, args.platform, package):
                print 'Excluding ' + package + ' for ' + args.nightly_release + ' project ' + args.project + ' on ' + args.platform
                print 'art-package: ' + package
                print 'art-status: excluded'
                sys.stdout.flush()
            else:
                shell_files = self.get_files(root, 'long')
                number_of_tests = len(shell_files)
                if number_of_tests > 0:
                    print 'art-package: ' + package
                    print 'art-status: included'
                    print 'root' + root
                    print 'Handling ' + package + ' for ' + args.nightly_release + ' project ' + args.project + ' on ' + args.platform
                    print "Number of tests: " + str(number_of_tests)
                    sys.stdout.flush()
                    submit_dir = os.path.join(submit_directory, package)
                    run = os.path.join(submit_dir, "run")
                    mkdir_p(run)

                    shutil.copy(os.path.join(art_dir, 'art.py'), run)
                    make_executable(os.path.join(run, 'art.py'))

                    command = os.path.join(art_dir, 'art.py') + ' task ' + args.nightly_release + ' ' + args.project + ' ' + args.platform + ' ' + args.nightly_tag + ' ' + args.sequence_tag + ' ' + package + ' ' + str(number_of_tests)
                    print command
                    sys.stdout.flush()
                    out = check(run_command(command))
                    print out
                    sys.stdout.flush()

    def retrieve(self):
        """TBD."""
        parser = argparse.ArgumentParser(
            description='Wait for tests to finish and retrieve the results')
        parser.add_argument('job_id', help="id of the job to retrieve")
        # now that we're inside a subcommand, ignore the first two args
        args = parser.parse_args(sys.argv[2:])
        job_id = args.job_id

        url = 'http://bigpanda.cern.ch/task/' + job_id + '/?json'
        print 'Running art retrieve on ' + url
        sys.stdout.flush()

        response = requests.get(url)
        if response.status_code != 200:
            print 'ERROR http status code ' + str(response.status_code)
            sys.stdout.flush()
            exit(1)

        data = response.json()

        task = data['taskparams']['taskName']
        task_match = re.match(r'^(\w+)\.(\w+)\.(\w+)\.(\w+).([\w\.-]+)\.([\w-]+)\.([\w-]+)\.(\d+)/', task)
        if not task_match:
            print 'ERROR cannot decode task: ' + task
            sys.stdout.flush()
            exit(1)

        # job_type = task_match.group(1)
        # user = task_match.group(2)
        # experiment = task_match.group(3)
        # job = task_match.group(4)
        # nightly_release = task_match.group(5)
        # platform = task_match.group(6)
        # nightly_release = task_match.group(7)
        # build_id = task_match.group(8)

        status = data['task']['status']
        dsinfo = data['task']['dsinfo']
        print 'Total / Done / Failed:  ' + str(dsinfo['nfiles']) + ' / ' + str(dsinfo['nfilesfinished']) + ' / ' + str(dsinfo['nfilesfailed'])
        sys.stdout.flush()

        # Add other final states here
        print 'Status: ' + status
        sys.stdout.flush()

        if status in ['done']:
            exit(0)

        if status in ['finished', 'failed']:
            exit(1)

        if status in ['broken', 'aborted']:
            exit(2)

        # please re-call later
        exit(-1)

    def task(self):
        """TBD.

        arguments: SCRIPT_DIRECTORY, NIGHTLY_RELEASE, PROJECT, PLATFORM, NIGHTLY_TAG, SEQUENCE_TAG, PACKAGE, NUMBER_OF_TESTS

        example: /cvmfs/atlas-nightlies.cern.ch/repo/sw/21.0/2017-02-26T2119 21.0 Athena x86_64-slc6-gcc62-opt 2017-02-26T2119 316236 Tier0ChainTests 32
        """
        parser = argparse.ArgumentParser(
            description='Runs a single task, consisting of given number of jobs')
        parser.add_argument('nightly_release', help=Art.NIGHTLY_RELEASE_HELP)
        parser.add_argument('project', help=Art.PROJECT_HELP)
        parser.add_argument('platform', help=Art.PLATFORM_HELP)
        parser.add_argument('nightly_tag', help=Art.NIGHTLY_TAG_HELP)
        parser.add_argument('sequence_tag', help=Art.SEQUENCE_TAG_HELP)
        parser.add_argument('package', help=Art.PACKAGE_HELP)
        parser.add_argument('number_of_tests', help="number of tests")
        # now that we're inside a subcommand, ignore the first two args
        args = parser.parse_args(sys.argv[2:])
        print 'Running art task'
        sys.stdout.flush()

        # get the path of the art.py script
        art_dir = os.path.dirname(os.path.realpath(sys.argv[0]))

        print args.nightly_release + " " + args.project + " " + args.platform + " " + args.nightly_tag + " " + args.sequence_tag + " " + args.package + " " + str(args.number_of_tests)
        sys.stdout.flush()

        # run task from Bash Script as is needed in ATLAS setup
        # FIXME we need to parse the output
        out = check(run_command(os.path.join(art_dir, 'art-task-grid.sh') + " " + args.nightly_release + " " + args.project + " " + args.platform + " " + args.nightly_tag + " " + args.sequence_tag + " " + args.package + " " + str(args.number_of_tests)))
        print out
        sys.stdout.flush()

    def job(self):
        """TBD.

        arguments: NIGHTLY_RELEASE, PROJECT, PLATFORM, NIGHTLY_TAG, PACKAGE, INDEX, OUTPUT

        Tests are called with the following parameters:
        SCRIPT_DIRECTORY, NIGHTLY_RELEASE, PROJECT, PLATFORM, NIGHTLY_TAG, PACKAGE, TEST_NAME
        """
        parser = argparse.ArgumentParser(
            description='Runs a single job, given an index')
        parser.add_argument('nightly_release', help=Art.NIGHTLY_RELEASE_HELP)
        parser.add_argument('project', help=Art.PROJECT_HELP)
        parser.add_argument('platform', help=Art.PLATFORM_HELP)
        parser.add_argument('nightly_tag', help=Art.NIGHTLY_TAG_HELP)
        parser.add_argument('package', help=Art.PACKAGE_HELP)
        parser.add_argument('index', help="index of the test, for grid starting from 1")
        parser.add_argument('output', help="output filename")
        # now that we're inside a subcommand, ignore the first two args
        args = parser.parse_args(sys.argv[2:])
        print 'Running art job'
        sys.stdout.flush()

        index = int(args.index)

        print args.nightly_release + " " + args.project + " " + args.platform + " " + args.nightly_tag + " " + args.package + " " + str(index) + " " + args.output
        sys.stdout.flush()

        test_directories = self.get_test_directories(args.nightly_release, args.project, args.platform, args.nightly_tag)
        test_dir = test_directories[args.package]
        test_list = self.get_files(test_dir, 'long')

        # minus one for grid
        test_name = test_list[index - 1]
        test_file = os.path.join(test_dir, test_name)
        com = '%s %s %s %s %s %s %s %s' % (test_file, Art.SCRIPT_DIRECTORY, args.nightly_release, args.project, args.platform, args.nightly_tag, args.package, test_name)

        print test_name
        print test_dir
        print com
        sys.stdout.flush()

        # run the test
        out = check(run_command(com))
        print out
        sys.stdout.flush()

        # pick up the output
        # FIXME for other outputs
        tar_file = tarfile.open(args.output, mode='w')
        with open(test_file, "r") as f:
            for line in f:
                for word in line.split():
                    out_name = re.findall("--output.*=(.*)", word)
                    if (out_name):
                        if os.path.exists(out_name[0]):
                            tar_file.add(out_name[0])
        tar_file.close()

    def list(self):
        """TBD."""
        parser = argparse.ArgumentParser(
            description='Lists the jobs of a package')
        parser.add_argument('nightly_release', help=Art.NIGHTLY_RELEASE_HELP)
        parser.add_argument('project', help=Art.PROJECT_HELP)
        parser.add_argument('platform', help=Art.PLATFORM_HELP)
        parser.add_argument('nightly_tag', help=Art.NIGHTLY_TAG_HELP)
        parser.add_argument('package', help=Art.PACKAGE_HELP)
        args = parser.parse_args(sys.argv[2:])
        print 'Running art list'
        sys.stdout.flush()

        jobs = self.get_list(args.nightly_release, args.project, args.platform, args.nightly_tag, args.package)
        i = 1
        for job in jobs:
            print str(i) + ' ' + job
            sys.stdout.flush()
            i += 1

    def get_tar(self, nightly_release, project, platform, nightly_tag, package, test_name, extension):
        """TBD."""
        index = self.get_list(nightly_release, project, platform, nightly_tag, package).index(test_name)
        # Grid counts from 1
        index += 1

        container = 'user.tcuhadar.atlas.art.' + nightly_release + '.' + project + '.' + platform + '.' + nightly_tag + '.*.' + package + extension
        print container

        out = check(run_command("rucio list-dids " + container + " --filter type=container | grep " + nightly_tag + " | sort -r | cut -d ' ' -f 2 | head -n 1"))
        print out

        out = check(run_command("rucio list-files --csv " + out + " | grep " + "{0:0>6}".format(index)))
        print out

        tar_name = out.split(',')[0]
        out = check(run_command("rucio download " + tar_name))
        print out

        return tarfile.open(tar_name.replace(':', '/', 1))

    def log(self):
        """TBD."""
        parser = argparse.ArgumentParser(
            description='Show the log of a job')
        parser.add_argument('nightly_release', help=Art.NIGHTLY_RELEASE_HELP)
        parser.add_argument('project', help=Art.PROJECT_HELP)
        parser.add_argument('platform', help=Art.PLATFORM_HELP)
        parser.add_argument('nightly_tag', help=Art.NIGHTLY_TAG_HELP)
        parser.add_argument('package', help=Art.PACKAGE_HELP)
        parser.add_argument('test_name', help='name of the test')
        args = parser.parse_args(sys.argv[2:])
        print 'Running art log'
        sys.stdout.flush()

        tar = self.get_tar(args.nightly_release, args.project, args.platform, args.nightly_tag, args.package, args.test_name, '.log')

        for name in tar.getnames():
            if 'athena_stdout.txt' in name:
                f = tar.extractfile(name)
                content = f.read()
                print content
                break
        tar.close()

    def output(self):
        """TBD."""
        parser = argparse.ArgumentParser(
            description='Get the output of a job')
        parser.add_argument('nightly_release', help=Art.NIGHTLY_RELEASE_HELP)
        parser.add_argument('project', help=Art.PROJECT_HELP)
        parser.add_argument('platform', help=Art.PLATFORM_HELP)
        parser.add_argument('nightly_tag', help=Art.NIGHTLY_TAG_HELP)
        parser.add_argument('package', help=Art.PACKAGE_HELP)
        parser.add_argument('test_name', help='name of the test')
        parser.add_argument('file_name', help='name of the output file')
        args = parser.parse_args(sys.argv[2:])
        print 'Running art output'
        sys.stdout.flush()

        tar = self.get_tar(args.nightly_release, args.project, args.platform, args.nightly_tag, args.package, args.test_name, '_EXT0')

        for member in tar.getmembers():
            if args.file_name in member.name:
                tar.extractall(path='.', members=[member])
                break
        tar.close()

    def compare(self):
        """TBD."""
        parser = argparse.ArgumentParser(
            description='Compare the output of a job')
        parser.add_argument('nightly_release', help=Art.NIGHTLY_RELEASE_HELP)
        parser.add_argument('project', help=Art.PROJECT_HELP)
        parser.add_argument('platform', help=Art.PLATFORM_HELP)
        parser.add_argument('nightly_tag', help=Art.NIGHTLY_TAG_HELP)
        parser.add_argument('package', help=Art.PACKAGE_HELP)
        parser.add_argument('test_name', help='name of the test')
        parser.add_argument('days', help='number of days back for reference file')
        parser.add_argument('file_names', nargs='+', help='name of the output file')
        args = parser.parse_args(sys.argv[2:])
        print 'Running art compare'
        sys.stdout.flush()

        previous_nightly_tag = self.get_previous_nightly_tag(args.nightly_release, args.nightly_tag, args.project, int(args.days))

        ref_dir = os.path.join('.', 'ref-' + previous_nightly_tag)
        mkdir_p(ref_dir)

        tar = self.get_tar(args.nightly_release, args.project, args.platform, previous_nightly_tag, args.package, args.test_name, '_EXT0')
        for member in tar.getmembers():
            if member.name in args.file_names:
                tar.extractall(path=ref_dir, members=[member])
        tar.close()

        for file_name in args.file_names:
            print "art-compare: " + previous_nightly_tag + " " + file_name
            ref_file = os.path.join(ref_dir, file_name)

            out = check(run_command("acmd.py diff-root " + file_name + " " + ref_file + " --error-mode resilient --ignore-leaves RecoTimingObj_p1_HITStoRDO_timings RecoTimingObj_p1_RAWtoESD_mems RecoTimingObj_p1_RAWtoESD_timings RAWtoESD_mems RAWtoESD_timings ESDtoAOD_mems ESDtoAOD_timings HITStoRDO_timings RAWtoALL_mems RAWtoALL_timings RecoTimingObj_p1_RAWtoALL_mems RecoTimingObj_p1_RAWtoALL_timings --entries 10"))
            print out
            sys.stdout.flush()

    def get_previous_nightly_tag(self, nightly_release, nightly_tag, project, days):
        """TBD. 21:00 is cutoff time."""
        directory = os.path.join(Art.SCRIPT_DIRECTORY, nightly_release)
        tags = os.listdir(directory)
        tags.sort(reverse=True)
        tags = [x for x in tags if re.match(r'\d{4}-\d{2}-\d{2}T\d{2}\d{2}', x)]
        # print tags
        found = False
        for tag in tags:
            if tag == nightly_tag:
                found = True
            elif found:
                # check this is within days... (cutoff is 21:00, just move by 3 hours to get full days)
                fmt = '%Y-%m-%dT%H%M'
                offset = datetime.timedelta(hours=3)
                nightly_tag_dt = datetime.datetime.strptime(nightly_tag, fmt) + offset
                from_dt = nightly_tag_dt.replace(hour=0, minute=0, second=0, microsecond=0) - datetime.timedelta(days=days)
                to_dt = from_dt + datetime.timedelta(days=1)
                tag_dt = datetime.datetime.strptime(tag, fmt) + offset
                if from_dt <= tag_dt and tag_dt < to_dt and os.path.isdir(os.path.join(directory, tag, project)):
                    return tag
        return None

    def get_list(self, nightly_release, project, platform, nightly_tag, package):
        """TBD."""
        test_directories = self.get_test_directories(nightly_release, project, platform, nightly_tag)
        test_dir = test_directories[package]
        return self.get_files(test_dir, 'long')


if __name__ == '__main__':
    Art()
