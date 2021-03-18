# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ..configurable import Configurable
import os

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class ExternalBase(Configurable):
    """! Base class for external processes.

    All external processes inherit from this class.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, name, *executable_path):
        """! Constructor.

        @param name             name of this external process.
        @param executable_path  path to appropriate PowhegBox executable.
        """
        super(ExternalBase, self).__init__()

        ## Name of this external process
        self.name = name

        ## External executable that will be used
        if executable_path:
            self.executable = os.path.join(*executable_path)
        else:
            self.executable = None

    def needs_scheduling(self, process):
        """! Report whether this external process should be scheduled.

        @param process PowhegBox process.
        """
        raise AttributeError("Must be implemented by children!")
