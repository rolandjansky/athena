#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
ART  - ATLAS Release Tester - Diff.

Usage:
  art-diff.py [--diff-type=<diff_type> --exclude=<pattern>... --platform=<platform> --platform-ref=<platform>] <nightly_release> <project> <nightly_tag> <nightly_release_ref> <platform_ref> <nightly_tag_ref> <package>
  art-diff.py [--diff-type=<diff_type> --exclude=<pattern>...] <dir> <ref_dir>

Options:
  --diff-type=<diff_type>    Type of diff (e.g. diff-pool or diff-root) [default: diff-pool]
  --exclude=<pattern>...     Exclude test files according to pattern
  -h --help                  Show this screen
  --platform=<platform>      Platform [default: x86_64-slc6-gcc62-opt]
  --platform-ref=<platform>  Reference Platform [default: x86_64-slc6-gcc62-opt]
  --version                  Show version

Arguments:
  dir                     Directory to compare
  nightly_release         Name of the nightly release (e.g. 21.0)
  nightly_release_ref     Reference Name of the nightly release (e.g. 21.0)
  nightly_tag             Nightly tag (e.g. 2017-02-26T2119)
  nightly_tag_ref         Reference Nightly tag (e.g. 2017-02-26T2119)
  package                 Package of the test (e.g. Tier0ChainTests)
  project                 Name of the project (e.g. Athena)
  project_ref             Reference Name of the project (e.g. Athena)
  ref_dir                 Directory to compare to
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

VERSION = "0.6.7"
ATHENA_STDOUT = "athena_stdout.txt"


class ArtDiff(object):
    """Class for comparing output files."""

    EOS_OUTPUT_DIR = '/eos/atlas/atlascerngroupdisk/data-art/grid-output'

    def __init__(self, arguments):
        """Constructor of ArtDiff."""
        diff_type = arguments['--diff-type']
        excludes = arguments['--exclude']
        if arguments['<dir>'] is None:
            nightly_release = arguments['<nightly_release>']
            project = arguments['<project>']
            platform = arguments['--platform']
            nightly_tag = arguments['<nightly_tag>']

            nightly_release_ref = arguments['<nightly_release_ref>']
            project_ref = arguments['<project_ref>']
            platform_ref = arguments['--platform_ref']
            nightly_tag_ref = arguments['<nightly_tag_ref>']

            package = arguments['<package>']

            exit(self.diff(nightly_release, project, platform, nightly_tag, nightly_release_ref, project_ref, platform_ref, nightly_tag_ref, package, diff_type, excludes))

        # directory compare
        directory = arguments['<dir>']
        ref_dir = arguments['<ref_dir>']
        exit(self.diff_dirs(directory, ref_dir, diff_type, excludes))

    def diff(self, nightly_release, project, platform, nightly_tag, nightly_release_ref, project_ref, platform_ref, nightly_tag_ref, package, diff_type, excludes=[]):
        """Run difference between two results."""
        val_dir = os.path.join(ArtDiff.EOS_OUTPUT_DIR, nightly_release, nightly_tag, project, platform, package)
        ref_dir = os.path.join(ArtDiff.EOS_OUTPUT_DIR, nightly_release_ref, nightly_tag_ref, project_ref, platform_ref, package)
        return self.diff_dirs(val_dir, ref_dir, diff_type, excludes)

    def diff_dirs(self, val_dir, ref_dir, diff_type, excludes=[]):
        """Run difference between two directories."""
        print "val_dir: %s" % val_dir
        print "ref_dir: %s" % ref_dir

        stat_per_chain = {}
        for test_name in os.listdir(val_dir):
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

            val_result = self.get_result(os.path.join(val_dir, test_name))
            ref_result = self.get_result(os.path.join(val_dir, test_name))
            for key, value in val_result.iteritems():
                if key in ref_result:
                    print "%-10s: ref: %d events, val: %d events" % (key, int(ref_result[key][1]), int(val_result[key][1]))

            test_dir = os.path.join(val_dir, test_name)
            test_patterns = ['*AOD*.pool.root', '*ESD*.pool.root', '*HITS*.pool.root', '*RDO*.pool.root', '*TAG*.root']
            test_files = []
            for test_pattern in test_patterns:
                test_files.extend(glob.glob(os.path.join(test_dir, test_pattern)))
            for test_file in test_files:
                extension = '.root'
                name = os.path.splitext(os.path.basename(test_file))[0]  # remove .root
                if name.endswith('.pool'):
                    extension = '.pool.root'
                    name = os.path.splitext(os.path.basename(name))[0]  # remove .pool
                val_file = os.path.join(val_dir, test_name, name + extension)
                ref_file = os.path.join(ref_dir, test_name, name + extension)
                print "val_file: %s" % val_file
                print "ref_file: %s" % ref_file

                if not os.path.exists(ref_file):
                    print "no test found in ref_dir to compare: %s" % ref_file
                    continue

                # add the test to the summary if it was not already there
                if test_name not in stat_per_chain:
                    stat_per_chain[test_name] = 0

                if extension == '.pool.root':
                    if diff_type == 'diff-pool':
                        stat_per_chain[test_name] |= self.diff_pool(val_file, ref_file)
                    else:
                        stat_per_chain[test_name] |= self.diff_root(val_file, ref_file)
                else:
                    stat_per_chain[test_name] |= self.diff_tag(val_file, ref_file)

        result = 0
        for filename, status in stat_per_chain.iteritems():
            if status:
                print "%-70s CHANGED" % filename
                result = 1
            else:
                print "%-70s IDENTICAL" % filename

        return result

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

    def diff_root(self, file_name, ref_file, entries=-1):
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
        print "Execute: %s" % cmd
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
    ArtDiff(arguments)
