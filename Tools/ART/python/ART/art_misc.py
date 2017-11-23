#!/usr/bin/env python
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""TBD."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import errno
import logging
import os
import shlex
import subprocess
import sys

MODULE = "art.misc"


def set_log(kwargs):
    """TBD."""
    level = logging.DEBUG if kwargs['verbose'] else logging.WARN if kwargs['quiet'] else logging.INFO
    log = logging.getLogger("art")
    log.setLevel(level)

    # create and attach new handler, disable propagation to root logger to avoid double messages
    handler = logging.StreamHandler(sys.stdout)
    format_string = "%(asctime)s %(name)15s.%(funcName)-15s %(levelname)8s %(message)s"
    date_format_string = None
    formatter = logging.Formatter(format_string, date_format_string)
    handler.setFormatter(formatter)
    log.addHandler(handler)
    log.propagate = False


def run_command(cmd, dir=None, shell=False, env=None):
    """
    Run the given command locally.

    The command runs as separate subprocesses for every piped command.
    Returns tuple of exit_code, output and err.
    """
    log = logging.getLogger(MODULE)
    log.debug("Execute: %s", cmd)
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


def is_exe(fpath):
    """Return True if fpath is executable."""
    return os.path.isfile(fpath) and os.access(fpath, os.X_OK)


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
