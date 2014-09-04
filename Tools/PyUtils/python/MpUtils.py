# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: PyUtils/MpUtils.py
# @purpose: a set of little tools for multiprocessing
#           stolen from ClePy
#            http://pypi.python.org/pypi/clepy/0.1
#            MIT

def chunkify(s, chunksize):

    """
    Yield sequence s in chunks of size chunksize.

    >>> list(chunkify('abcdefg', 2))
    ['ab', 'cd', 'ef', 'g']

    >>> list(chunkify('abcdefg', 99))
    ['abcdefg']

    """

    for i in range(0, len(s), chunksize):
        yield s[i:i+chunksize]

from multiprocessing import Pipe, Process

class SubProcessIterator(object):
    """Instances of this class process iterators in separate processes."""
    def __init__(self, itertask, eoi='__eoi__'):
        """Create a new subprocess iterator.

        itertask : some iterable task to execute in a subprocess
        eoi : an end-of-iteration marker - returned from the subprocess
              to signal that iteration is complete.
        """
        self.client, self.master = Pipe()
        self.end_of_input = eoi
        pargs = [itertask, self.master, eoi]
        self.process = Process(target=self.work, args=pargs)
        self.started = False

    def _start(self):
        self.started = True
        self.process.start()

    @staticmethod
    def work(iterator, master, eoi):
        """The actual callable that is executed in the subprocess."""
        for chunk in iterator:
            master.send(chunk)
        master.send(eoi)

    def __iter__(self):
        if not self.started:
            self._start()
        return self

    def next(self):
        item = self.client.recv()
        if item != self.end_of_input:
            return item
        else:
            self.next = self._empty
            raise StopIteration

    def _empty(self, *args, **params):
        raise StopIteration

def piter(iterable, eoi=None):
    """Create a new subprocess iterator.

    iterable : some iterable task to execute in a subprocess
    eoi : an end-of-iteration marker - returned from the subprocess
          to signal that iteration is complete.
    """
    return SubProcessIterator(iterable, eoi=eoi)

