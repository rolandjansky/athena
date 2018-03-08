# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl NonBlockingStreamReader
# Read an output stream without blocking
#
# Authors: James Robinson <james.robinson@cern.ch>
# based on: http://eyalarubas.com/python-subproc-nonblock.html

#! /usr/bin/env python
import threading
import Queue

class NonBlockingStreamReader(object) :

  ## Set up input stream, output queue and process to transfer between them
  def __init__(self, input_stream):
    self._stream = input_stream
    self._queue = Queue.Queue()
    self._running = True

    self._output_thread = threading.Thread( target=self.populate_queue, args=(self._stream, self._queue) )
    self._output_thread.daemon = True
    self._output_thread.start() # start collecting lines from the stream


  ## Collect lines from stream and put them in queue
  def populate_queue(self, stream, queue):
    while self._running :
      line = stream.readline()
      if line : queue.put( line )


  ## Return lines from queue
  def readline(self, timeout=None):
    try:
      output = self._queue.get( block=(timeout is not None), timeout=timeout ).rstrip()
      return (output,self._queue.qsize())
    except Queue.Empty :
      return (None,0)


  ## Release thread resources on finalise
  def finalise( self ) :
    self._stream.flush()
    self._running = False
    self._output_thread.join()
