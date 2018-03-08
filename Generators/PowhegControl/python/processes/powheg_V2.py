# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from .powheg_base import PowhegBase


class PowhegV2(PowhegBase):
    """! Base class for PowhegBox V2 processes.

    All V2 processes inherit from this class.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, executable_name, **kwargs):
        """! Constructor.

        @param base_directory  path to PowhegBox code.
        @param executable_name folder containing appropriate PowhegBox executable.
        @param is_LO           True if this is a leading-order process.
        """
        super(PowhegV2, self).__init__(base_directory, "POWHEG-BOX-V2", executable_name, **kwargs)

    @property
    def powheg_version(self):
        """! Version of PowhegBox process."""
        return "V2"
