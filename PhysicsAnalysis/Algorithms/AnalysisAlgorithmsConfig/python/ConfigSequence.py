# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

class ConfigSequence:
    """a sequence of ConfigBlock objects

    This could in principle just be a simple python list, and maybe we
    change it to that at some point (10 Mar 22).  Having it as its own
    class allows to implement some helper functions.

    This implements an interface similar to ConfigBlock, but it
    doesn't derive from it, as ConfigBlock will likely gain
    functionality in the future that wouldn't work for a sequence (or
    wouldn't work in the same way).

    """

    def __init__ (self) :
        self._blocks = []


    def append (self, block) :
        """append a configuration block to the sequence"""

        self._blocks.append (block)


    def collectReferences (self, config) :
        """call collectReferences() on all blocks

        This registers references to the containers being processed in
        the individual blocks, allowing them to match up along the
        sequence when creating the algorithms.
        """
        for block in self._blocks:
            block.collectReferences (config)


    def makeAlgs (self, config) :
        """call makeAlgs() on all blocks

        This will create the actual algorithm configurables based on
        how the blocks are configured right now.
        """
        for block in self._blocks:
            block.makeAlgs (config)


    def fullConfigure (self, config) :
        """do the full configuration on this sequence

        This sequence needs to be the only sequence, i.e. it needs to
        contain all the blocks that will be configured, as it will
        perform all configuration steps at once.
        """
        self.collectReferences (config)
        self.makeAlgs (config)
