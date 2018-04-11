#!/usr/bin/env python
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
"""Miscellaneous functions."""

__author__ = "Tulay Cuhadar Donszelmann <tcuhadar@cern.ch>"

import concurrent.futures
import errno
import logging
import os
import shlex
import subprocess
import sys

from datetime import datetime

MODULE = "art.misc"


def set_log(kwargs):
    """Set the default log level and message format depending on --verbose or --quiet options."""
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


def get_atlas_env():
    """Get all environment variables."""
    log = logging.getLogger(MODULE)
    try:
        nightly_release = os.environ['AtlasBuildBranch']
        project = os.environ['AtlasProject']
        platform = os.environ[project + '_PLATFORM']
        nightly_tag = os.environ['AtlasBuildStamp']
        return (nightly_release, project, platform, nightly_tag)
    except KeyError, e:
        log.critical("Environment variable not set %s", e)
        sys.exit(1)


def run_command(cmd, dir=None, shell=False, env=None, verbose=True):
    """
    Run the given command locally.

    The command runs as separate subprocesses for every piped command.
    Returns tuple of exit_code, output and err.
    """
    # leave at print for basic debugging, log sometimes lost
    start_time = datetime.now()
    if verbose:
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
    end_time = datetime.now()

    return (exit_code, str(output), str(err), cmd, start_time, end_time)


def run_command_parallel(cmd, nthreads, ncores, dir=None, shell=False, env=None, verbose=True):
    """
    Run the given command locally in parallel.

    The command runs as separate subprocesses for every piped command.
    Returns tuple of exit_code, output and err.
    """
    start_time = datetime.now()
    log = logging.getLogger(MODULE)
    ncores = min(ncores, nthreads)

    if env is None:
        env = os.environ.copy()

    env['ArtThreads'] = str(nthreads)
    env['ArtCores'] = str(ncores)

    # Results
    full_exit_code = 0
    full_out = ''
    full_err = ''

    # Start
    env['ArtProcess'] = "start"
    (exit_code, out, err, command, start_time_start, end_time_start) = run_command(cmd, dir=dir, shell=shell, env=env, verbose=verbose)
    full_exit_code = full_exit_code if exit_code == 0 else exit_code
    full_out += "-+-art-process start out " + start_time_start.strftime('%Y-%m-%dT%H:%M:%S') + "\n"
    full_out += out
    full_out += "---art-process start out " + end_time_start.strftime('%Y-%m-%dT%H:%M:%S') + "\n"

    full_err += "-+-art-process start err " + start_time_start.strftime('%Y-%m-%dT%H:%M:%S') + "\n"
    full_err += err
    full_err += "---art-process start err " + end_time_start.strftime('%Y-%m-%dT%H:%M:%S') + "\n"

    log.info("Creating executor with cores: %d", ncores)
    executor = concurrent.futures.ThreadPoolExecutor(ncores)
    future_set = []

    # Processing
    log.info("Running threads: %d", nthreads)
    for index in range(nthreads):
        process_env = env.copy()
        process_env['ArtProcess'] = str(index)
        future_set.append(executor.submit(run_command, cmd, dir=dir, shell=shell, env=process_env, verbose=verbose))

    log.info("Waiting for threads to finish...")
    concurrent.futures.wait(future_set)
    for index, future in enumerate(future_set):
        (exit_code, out, err, command, start_time_process, end_time_process) = future.result()
        full_exit_code = full_exit_code if exit_code == 0 else exit_code
        full_out += "-+-art-process " + str(index) + " out " + start_time_process.strftime('%Y-%m-%dT%H:%M:%S') + "\n"
        full_out += out
        full_out += "---art-process " + str(index) + " out " + end_time_process.strftime('%Y-%m-%dT%H:%M:%S') + "\n"

        full_err += "-+-art-process " + str(index) + " err " + start_time_process.strftime('%Y-%m-%dT%H:%M:%S') + "\n"
        full_err += err
        full_err += "---art-process " + str(index) + " err " + end_time_process.strftime('%Y-%m-%dT%H:%M:%S') + "\n"

    # End
    env['ArtProcess'] = "end"
    (exit_code, out, err, command, start_time_end, end_time_end) = run_command(cmd, dir=dir, shell=shell, env=env, verbose=verbose)
    full_exit_code = full_exit_code if exit_code == 0 else exit_code
    full_out += "-+-art-process end out " + start_time_end.strftime('%Y-%m-%dT%H:%M:%S') + "\n"
    full_out += out
    full_out += "---art-process end out " + end_time_end.strftime('%Y-%m-%dT%H:%M:%S') + "\n"

    full_err += "-+-art-process end err " + start_time_end.strftime('%Y-%m-%dT%H:%M:%S') + "\n"
    full_err += err
    full_err += "---art-process end err " + end_time_end.strftime('%Y-%m-%dT%H:%M:%S') + "\n"

    end_time = datetime.now()

    return (full_exit_code, full_out, full_err, cmd, start_time, end_time)


def is_exe(path):
    """Return True if path is executable."""
    return os.path.isfile(path) and os.access(path, os.X_OK)


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
    """Show which program is actually found on the PATH."""
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
