# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from .non_blocking_stream_reader import NonBlockingStreamReader
import subprocess

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class ProcessManager(object):
    """! Wrapper to handle multiple Powheg subprocesses.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, process_list):
        """! Constructor.

        @param process_list List of processes to manage.
        """
        self.__process_list = process_list
        self.__n_initial = len(process_list)

    def monitor(self):
        """! Monitor each of the managed processes and log when they are finished."""
        for idx, process in enumerate(self.__process_list):
            process.id_number = idx + 1
        while len(self.__process_list) > 0:
            for process in list(self.__process_list):
                if not process.has_output():
                    _return_code = process.return_code
                    self.__process_list.remove(process)
                    if _return_code == 0:
                        logger.info("Finished process #{}: there are now {}/{} running".format(process.id_number, len(self.__process_list), self.__n_initial))
                    else:
                        logger.warning("Process #{} terminated unexpectedly (return code {}): there are now {}/{} running".format(process.id_number, _return_code, len(self.__process_list), self.__n_initial))


class SingleProcessThread(object):
    """! Single executable running in a subprocess (usually PowhegBox).

    @author James Robinson  <james.robinson@cern.ch>
    """

    log_level = {"stdout": "info", "stderr": "error"}
    __output_prefix = "    | "
    __ignore_output = []

    def __init__(self, command_list, seed_index=None, stdin=None, ignore_output=None):
        """! Constructor.

        Setup underlying process together with non-blocking readers for stdout and stderr.

        @param command_list  Command that will be run (possibly with options).
        @param seed_index    Which seed from pwgseeds.dat to use.
        @param stdin         An open file handle providing input.
        @param ignore_output List of strings to filter out from messages.
        """
        if not isinstance(command_list, list):
            command_list = [command_list]
        command_list = [str(x) for x in command_list]
        # Set up messages to ignore
        if ignore_output is not None:
            self.__ignore_output = ignore_output
        # Usual case, where no open file handle is provided
        if stdin is None:
            self.__process = subprocess.Popen(command_list, stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            # Write seed to stdin
            if seed_index is not None:
                self.__output_prefix += "Process #{}: ".format(seed_index)
                self.__process.stdin.write(str(seed_index))
                self.__process.stdin.close()
                with open("pwgseeds.dat", "r") as seed_file:
                    random_seed_list = seed_file.read().splitlines()
                self.log("Providing random seed: {}".format(random_seed_list[seed_index - 1]))
        # Using an open file handle to provide input to stdin: remember to close this later
        else:
            self.__process = subprocess.Popen(command_list, stdout=subprocess.PIPE, stdin=stdin, stderr=subprocess.PIPE, text=True)
        # Setup non-blocking stream readers for stdout and stderr
        self.__stdout = NonBlockingStreamReader(self.__process.stdout)
        self.__stderr = NonBlockingStreamReader(self.__process.stderr)

    def has_output(self):
        """! Write queued output and return process status."""
        status = self.is_running()
        self.write_queued_output()
        return status

    def is_running(self):
        """! Check if the underlying process is running and finalise stream readers if not."""
        if self.__process.poll() is not None:  # process has ended
            for nbsr in ("stdout", "stderr"):
                getattr(self, nbsr).finalise()
            return False
        return True

    def log(self, message, log_level="info"):
        """! Write to the logger with appropriate log-level.

        @param message    The message to pass to the logger.
        @param log_level  Which level to log at.
        """
        for word in self.__ignore_output:
            while word in message.decode("utf-8"):
                message = message.replace(word, "")
        getattr(logger, log_level)("{}{}".format(self.__output_prefix, message.strip()))

    def write_queued_output(self):
        """! Pass queued output to the logger."""
        for stream in ["stdout", "stderr"]:
            while True:
                output, queue_size = getattr(self, stream).readline(timeout=0.1)
                if not (output is None or len(output) == 0):
                    self.log(output, self.log_level[stream])
                if queue_size == 0:
                    break

    @property
    def return_code(self):
        """! Return code of underlying process."""
        return self.__process.returncode

    @property
    def stdout(self):
        """! stdout stream from underlying process."""
        return self.__stdout

    @property
    def stderr(self):
        """! stderr stream from underlying process."""
        return self.__stderr
