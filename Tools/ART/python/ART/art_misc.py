#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""TBD."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import errno
import os
import shlex
import subprocess


def run_command(cmd, dir=None, shell=False, env=None):
    """Run the given command locally and returns the output, err and exit_code."""
    # print "Execute: " + cmd
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


def verify((exitcode, out, err)):
    """Check exitcode and print statement."""
    if exitcode == 0:
        print out
        return exitcode

    print "Error:", exitcode
    print "StdOut:", out
    print "StdErr:", err

    return exitcode


def redirect((exitcode, out, err)):
    """Check exitcode."""
    return exitcode


def make_executable(path):
    """Make file executable (chmod +x)."""
    mode = os.stat(path).st_mode
    mode |= (mode & 0o444) >> 2    # copy R bits to X
    os.chmod(path, mode)


def mkdir_p(path):
    """Make (missing) directories."""
    try:
        os.makedirs(path)
    except OSError as exc:  # Python >2.5
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise


def which(program):
    """TBD."""
    import os

    def is_exe(fpath):
        """TBD."""
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None
