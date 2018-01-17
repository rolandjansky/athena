#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
ART  - ATLAS Release Tester - Diff.

Usage:
  art-diff.py [--diff-type=<diff_type> --exclude=<pattern>... --platform-ref=<platform> --entries=<entries>] <nightly_release_ref> <platform_ref> <nightly_tag_ref> <package>
  art-diff.py [--diff-type=<diff_type> --exclude=<pattern>... --entries=<entries>] <path> <ref_path>

Options:
  --diff-type=<diff_type>    Type of diff (e.g. diff-pool or diff-root) [default: diff-pool]
  --entries=<entries>        Only diff over number of entries [default: -1]
  --exclude=<pattern>...     Exclude test files according to pattern
  -h --help                  Show this screen
  --platform-ref=<platform>  Reference Platform [default: x86_64-slc6-gcc62-opt]
  --test-name=<test_name>    Test name to compare
  --version                  Show version

Arguments:
  path                       Directory or File to compare
  nightly_release_ref        Reference Name of the nightly release (e.g. 21.0)
  nightly_tag_ref            Reference Nightly tag (e.g. 2017-02-26T2119)
  package                    Package of the test (e.g. Tier0ChainTests)
  project_ref                Reference Name of the project (e.g. Athena)
  ref_path                   Directory or File to compare to

Environment:
  AtlasBuildBranch           Name of the nightly release (e.g. 21.0)
  AtlasProject               Name of the project (e.g. Athena)
  <AtlasProject>_PLATFORM    Platform (e.g. x86_64-slc6-gcc62-opt)
  AtlasBuildStamp            Nightly tag (e.g. 2017-02-26T2119)
"""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import fnmatch
import glob
import os
import re
import shlex
import subprocess
import sys

from ART.docopt import docopt

VERSION = "0.7.8"
ATHENA_STDOUT = "athena_stdout.txt"
DEFAULT_ENTRIES = -1


class ArtDiff(object):
    """Class for comparing output files."""

    EOS_OUTPUT_DIR = '/eos/atlas/atlascerngroupdisk/data-art/grid-output'

    def __init__(self):
        """Constructor of ArtDiff."""

    def parse(self, arguments):
        """Called from comandline."""
        diff_type = arguments['--diff-type']
        entries = arguments['--entries']
        excludes = arguments['--exclude']
        if arguments['<nightly_release_ref>'] is not None:
            try:
                nightly_release = os.environ['AtlasBuildBranch']
                project = os.environ['AtlasProject']
                platform = os.environ[project + '_PLATFORM']
                nightly_tag = os.environ['AtlasBuildStamp']
                return (nightly_release, project, platform, nightly_tag)
            except KeyError, e:
                print "Environment variable not set", e
                sys.exit(1)

            nightly_release_ref = arguments['<nightly_release_ref>']
            project_ref = arguments['<project_ref>']
            platform_ref = arguments['--platform_ref']
            nightly_tag_ref = arguments['<nightly_tag_ref>']

            package = arguments['<package>']

            exit(self.diff(nightly_release, project, platform, nightly_tag, nightly_release_ref, project_ref, platform_ref, nightly_tag_ref, package, diff_type, excludes, entries=entries))

        # directory compare
        path = arguments['<path>']
        ref_path = arguments['<ref_path>']

        if os.path.isfile(path):
            # file compare
            if not os.path.isfile(ref_path):
                print "Error: <ref_path> should be a file, if <path> is a file."
                sys.exit(1)

            exit(self.diff_file(path, ref_path, diff_type, entries=entries))

        if os.path.isfile(ref_path):
            print "Error: <ref_path> should be a directory, if <path> is a directory."
            sys.exit(1)

        # check if path contains "test_" entries
        if len(glob.glob(os.path.join(path, 'test_*'))) > 0:
            # directory compare
            exit(self.diff_dirs(path, ref_path, diff_type, excludes, entries=entries))

        # single test compare
        exit(self.diff_test(path, ref_path, diff_type, entries=entries))

    def diff(self, nightly_release, project, platform, nightly_tag, nightly_release_ref, project_ref, platform_ref, nightly_tag_ref, package, diff_type, excludes=[], entries=DEFAULT_ENTRIES):
        """Run difference between two results."""
        path = os.path.join(ArtDiff.EOS_OUTPUT_DIR, nightly_release, nightly_tag, project, platform, package)
        ref_path = os.path.join(ArtDiff.EOS_OUTPUT_DIR, nightly_release_ref, nightly_tag_ref, project_ref, platform_ref, package)
        return self.diff_dirs(path, ref_path, diff_type, excludes, entries=entries)

    def diff_dirs(self, path, ref_path, diff_type, excludes=[], entries=DEFAULT_ENTRIES):
        """Run difference between two directories."""
        print "    path: %s" % path
        print "ref_path: %s" % ref_path

        stat_per_chain = {}
        for test_name in os.listdir(path):
            # skip tests in pattern
            exclude_test = False
            for exclude in excludes:
                if fnmatch.fnmatch(test_name, exclude):
                    print "Excluding test %s according to pattern %s" % (test_name, exclude)
                    exclude_test = True
                    break
            if exclude_test:
                continue

            print "******************************************"
            print "Test: %s" % test_name
            print "******************************************"
            stat_per_chain[test_name] = self.diff_test(os.path.join(path, test_name), os.path.join(ref_path, test_name), diff_type, entries=entries)

        result = 0
        for test_name, status in stat_per_chain.iteritems():
            if status:
                print "%-70s CHANGED" % test_name
                result = 1
            else:
                print "%-70s IDENTICAL" % test_name

        return result

    def diff_test(self, path, ref_path, diff_type, entries=DEFAULT_ENTRIES):
        """Run differences between two directories."""
        result = self.get_result(path)
        ref_result = self.get_result(ref_path)
        for key, value in result.iteritems():
            if key in ref_result:
                print "%-10s: ref: %d events, val: %d events" % (key, int(ref_result[key][1]), int(result[key][1]))

        test_dir = path
        test_patterns = ['*AOD*.pool.root', '*ESD*.pool.root', '*HITS*.pool.root', '*RDO*.pool.root', '*TAG*.root']
        # get files in all patterns
        test_files = []
        for test_pattern in test_patterns:
            test_files.extend(glob.glob(os.path.join(test_dir, test_pattern)))
        # run test over all files
        result = 0
        for test_file in test_files:
            basename = os.path.basename(test_file)
            val_file = os.path.join(path, basename)
            ref_file = os.path.join(ref_path, basename)
            print "val_file: %s" % val_file
            print "ref_file: %s" % ref_file

            result |= self.diff_file(val_file, ref_file, diff_type, entries=entries)

        return result

    def diff_file(self, path, ref_path, diff_type, entries=DEFAULT_ENTRIES):
        """Compare two files."""
        if not os.path.exists(ref_path):
            print "no test found in ref_dir to compare: %s" % ref_path
            return 0

        if fnmatch.fnmatch(path, '*TAG*.root'):
            return self.diff_tag(path, ref_path)

        if diff_type == 'diff-pool':
            return self.diff_pool(path, ref_path)

        return self.diff_root(path, ref_path, entries)

    def get_result(self, directory):
        """
        Return map [ESD|AOD,...] -> (success, succeeded event count).

        find event counts in logfile
        'Event count check for AOD to TAG passed: all processed events found (500 output events)'
        'Event count check for BS to ESD failed: found 480 events, expected 500'
        """
        result = {}
        for entry in os.listdir(directory):
            if re.match(r"tarball_PandaJob_(\d+)_(\w+)", entry):
                logfile = os.path.join(directory, entry, ATHENA_STDOUT)
                with open(logfile, "r") as f:
                    for line in f:
                        match = re.search(r"Event count check for \w+ to (\w+) (passed|failed):[^\d]+(\d+)", line)
                        if match:
                            result[match.group(1)] = (match.group(2), match.group(3))
        return result

    def diff_tag(self, file_name, ref_file):
        """TBD."""
        (code, out, err) = self.run_command("diffTAGTree.py " + file_name + " " + ref_file)
        if code != 0:
            print "Error: %d" % code
            print err

        print out
        return code

    def diff_pool(self, file_name, ref_file):
        """TBD."""
        import PyUtils.PoolFile as PF

        # diff-pool
        df = PF.DiffFiles(refFileName=ref_file, chkFileName=file_name, ignoreList=['RecoTimingObj_p1_RAWtoESD_timings', 'RecoTimingObj_p1_ESDtoAOD_timings'])
        df.printSummary()
        stat = df.status()
        print stat
        del df

        return stat

    def diff_root(self, file_name, ref_file, entries):
        """TBD."""
        # diff-root
        (code, out, err) = self.run_command("acmd.py diff-root " + file_name + " " + ref_file + " --error-mode resilient --ignore-leaves RecoTimingObj_p1_HITStoRDO_timings RecoTimingObj_p1_RAWtoESD_mems RecoTimingObj_p1_RAWtoESD_timings RAWtoESD_mems RAWtoESD_timings ESDtoAOD_mems ESDtoAOD_timings HITStoRDO_timings RAWtoALL_mems RAWtoALL_timings RecoTimingObj_p1_RAWtoALL_mems RecoTimingObj_p1_RAWtoALL_timings RecoTimingObj_p1_EVNTtoHITS_timings --entries " + str(entries))
        if code != 0:
            print "Error: %d" % code
            print err

        print out
        return code

    def run_command(self, cmd, dir=None, shell=False, env=None):
        """
        Run the given command locally.

        The command runs as separate subprocesses for every piped command.
        Returns tuple of exit_code, output and err.
        """
        print "Execute:", cmd
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
                p[i] = subprocess.Popen(shlex.split(cmd_part), stdin=None, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=dir, shell=shell, env=env)
            else:
                p[i] = subprocess.Popen(shlex.split(cmd_part), stdin=p[i - 1].stdout, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=dir, shell=shell, env=env)
            i = i + 1
        (output, err) = p[i - 1].communicate()
        exit_code = p[0].wait()

        return exit_code, str(output), str(err)


if __name__ == '__main__':
    if sys.version_info < (2, 7, 0):
        sys.stderr.write("You need python 2.7 or later to run this script\n")
        exit(1)

    arguments = docopt(__doc__, version=os.path.splitext(os.path.basename(__file__))[0] + ' ' + VERSION)
    ArtDiff().parse(arguments)
