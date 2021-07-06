# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
import datetime
import threading

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class RepeatingTimer(threading.Timer):
    """! Helper class to run repeating timing output in an additional thread.

    @author http://stackoverflow.com/questions/5429577/
    """

    def run(self):
        """! Override Thread.run with desired behaviour."""
        while True:
            self.finished.wait(self.interval)
            if self.finished.is_set():
                return
            else:
                self.function(*self.args, **self.kwargs)

    @staticmethod
    def readable_duration(duration):
        """! Return duration in a human-readable form.

        @param duration Time interval to print.
        """
        if not isinstance(duration, datetime.timedelta):
            duration = datetime.timedelta(seconds=duration)
        h, m, s = str(duration).split(":")
        if h != "0":
            return "{}h {:>02}m {:>02}s".format(h, m, int(float(s)))
        if m != "00":
            return "{}m {:>02}s".format(int(float(m)), int(float(s)))
        return "{:.2f}s".format(float(s))


class HeartbeatTimer(RepeatingTimer):
    """! Recurring heartbeat that emits a message to console and to file.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, interval, output_file=None):
        """! Constructor.

        @param interval     Time interval between output messages in seconds.
        @param output_file  Log file to write to.
        """
        self.start_time = datetime.datetime.now()
        self.output_file = output_file
        super(HeartbeatTimer, self).__init__(interval, lambda: self.__emit_heartbeat())

    def __emit_heartbeat(self):
        """! Output a heartbeat message."""
        duration = RepeatingTimer.readable_duration(datetime.datetime.now() - self.start_time)
        message = "Heartbeat: Powheg generation has been running for {} in total".format(duration)
        logger.info(message)
        if self.output_file is not None:
            try:
                with open(self.output_file, "w") as f:
                    f.write(message)
            except IOError as detail:
                logger.error("I/O error: {}".format(detail))
