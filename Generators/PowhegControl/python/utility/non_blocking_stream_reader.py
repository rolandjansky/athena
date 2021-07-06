# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import threading
import queue as Queue


class NonBlockingStreamReader(object):
    """! Read an output stream without blocking.

    see http://eyalarubas.com/python-subproc-nonblock.html
    @author James Robinson <james.robinson@cern.ch>
    """

    def __init__(self, input_stream):
        """! Set up input stream, output queue and process to transfer between them."""
        self._stream = input_stream
        self._queue = Queue.Queue()
        self._running = True

        self._output_thread = threading.Thread(target=self.populate_queue, args=(self._stream, self._queue))
        self._output_thread.daemon = True
        self._output_thread.start()  # start collecting lines from the stream

    def populate_queue(self, stream, queue):
        """! Collect lines from stream and put them in queue."""
        while self._running:
            line = stream.readline()
            if line:
                queue.put(line)

    def readline(self, timeout=None):
        """! Return lines from queue."""
        try:
            output = self._queue.get(block=(timeout is not None), timeout=timeout).rstrip()
            return (output, self._queue.qsize())
        except Queue.Empty:
            return (None, 0)

    def finalise(self):
        """! Release thread resources on finalise."""
        self._stream.flush()
        self._running = False
        self._output_thread.join()
