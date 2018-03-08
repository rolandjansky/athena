# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from .powheg_base import PowhegBase


class PowhegV1(PowhegBase):
    """! Base class for PowhegBox V1 processes.

    All V1 processes inherit from this class.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, base_directory, executable_name, **kwargs):
        """! Constructor.

        @param base_directory  path to PowhegBox code.
        @param executable_name folder containing appropriate PowhegBox executable.
        """
        super(PowhegV1, self).__init__(base_directory, "POWHEG-BOX", executable_name, is_reweightable=False, **kwargs)

    @property
    def powheg_version(self):
        """! Version of PowhegBox process."""
        return "V1"
