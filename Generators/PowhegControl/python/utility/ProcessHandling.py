# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl ProcessHandling
#  Utility classes to handle Powheg processes
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
import subprocess
from AthenaCommon.Logging import logging
from NonBlockingStreamReader import NonBlockingStreamReader

# Initialise logging handler
logger = logging.getLogger("PowhegControl")

# A wrapper to handle multiple Powheg subprocesses
class ProcessManager(object):

    def __init__(self, process_list):
        self.__process_list = process_list
        self.__n_initial = len(process_list)

    # Monitor each of the managed processes and log when they are finished
    def monitor(self):
        for idx, process in enumerate(self.__process_list):
            process.id_number = idx + 1
        while len(self.__process_list) > 0:
            for process in list(self.__process_list):
                if not process.has_output():
                    self.__process_list.remove(process)
                    logger.info("Finished process #{}: there are now {}/{} running".format(process.id_number, len(self.__process_list), self.__n_initial))


# A single Powheg subprocess
class SingleProcessThread(object):
    log_level = {"stdout": "info", "stderr": "error"}
    __output_prefix = ""

    # Setup underlying process together with non-blocking readers for stdout and stderr
    def __init__(self, command_list, seed_index=None, stdin=None):
        if not isinstance(command_list, list):
            command_list = [command_list]
        command_list = [str(x) for x in command_list]
        # Usual case, where no open file handle is provided
        if stdin is None:
            self.__process = subprocess.Popen(command_list, stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE)
            # Write seed to stdin
            if seed_index is not None:
                self.__output_prefix = "Process #{}: ".format(seed_index)
                self.__process.stdin.write(str(seed_index))
                self.__process.stdin.close()
                with open("pwgseeds.dat", "rb") as seed_file:
                    random_seed_list = seed_file.read().splitlines()
                self.log("Providing random seed: {}".format(random_seed_list[seed_index - 1]))
        # Using an open file handle to provide input to stdin: remember to close this later
        else:
            self.__process = subprocess.Popen(command_list, stdout=subprocess.PIPE, stdin=stdin, stderr=subprocess.PIPE)
        # Setup non-blocking stream readers for stdout and stderr
        self.__stdout = NonBlockingStreamReader(self.__process.stdout)
        self.__stderr = NonBlockingStreamReader(self.__process.stderr)

    # Write queued output and return process status
    def has_output(self):
        status = self.is_running()
        self.write_queued_output()
        return status

    # Check if the underlying process is running and finalise stream readers if not
    def is_running(self):
        if self.__process.poll() is not None:  # process has ended
            for nbsr in ("stdout", "stderr") :
                getattr(self, nbsr).finalise()
            return False
        return True

    # Write to the logger with appropriate log-level
    def log(self, message, log_level="info"):
        getattr(logger, log_level)("{}{}".format(self.__output_prefix, message))

    # Pass queued output to the logger
    def write_queued_output(self):
        for stream in ["stdout", "stderr"]:
            while True:
                output, queue_size = getattr(self, stream).readline(timeout=0.1)
                if not (output is None or len(output) == 0):
                    self.log(output, self.log_level[stream])
                if queue_size == 0:
                    break

    # stdout stream from underlying process
    @property
    def stdout(self):
        return self.__stdout

    # stderr stream from underlying process
    @property
    def stderr(self):
        return self.__stderr
