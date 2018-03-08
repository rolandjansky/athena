# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl NonBlockingStreamReader
# Read an output stream without blocking
#
# Authors: James Robinson <james.robinson@cern.ch>
# based on: http://eyalarubas.com/python-subproc-nonblock.html

#! /usr/bin/env python
import threading
import Queue

class NonBlockingStreamReader :

  def __init__(self, input_stream):
    """Read from stream input_stream"""
    self._stream = input_stream
    self._queue = Queue.Queue()

    self._output_thread = threading.Thread( target=self.populate_queue, args=(self._stream, self._queue) )
    self._output_thread.daemon = True
    self._output_thread.start() # start collecting lines from the stream


  def populate_queue(self, stream, queue):
    """Collect lines from 'stream' and put them in 'queue'."""
    while True :
      line = stream.readline()
      if line :
        queue.put( line )
      else :
        pass


  def readline(self, timeout = None):
    """Collect lines from 'stream' and put them in 'queue'."""
    try:
      return self._queue.get( block=(timeout is not None), timeout=timeout ).rstrip()
    except Queue.Empty :
      return None
