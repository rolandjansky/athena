# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from .powheg_base import PowhegBase


class PowhegRES(PowhegBase):
    """! Base class for PowhegBox RES processes.

    All RES processes inherit from this class.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, executable_name, **kwargs):
        """! Constructor.

        @param base_directory  path to PowhegBox code.
        @param executable_name folder containing appropriate PowhegBox executable.
        @param is_LO           True if this is a leading-order process.
        """
        super(PowhegRES, self).__init__(base_directory, "POWHEG-BOX-RES", executable_name, **kwargs)

    @property
    def powheg_version(self):
        """! Version of PowhegBox process."""
        return "RES"
