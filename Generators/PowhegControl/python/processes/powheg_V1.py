# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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
    def default_PDFs(self):
        """! Default PDFs for this process."""
        return 260000 # Use NNPDF30_nlo_as_0118 central value

    @property
    def default_scales(self):
        """! Default scale variations for this process."""
        return [1.0, 1.0] # Do not set any scale variations

    @property
    def files_for_cleanup(self):
        """! Wildcarded list of files created by this process that can be deleted."""
        return [
            "FlavRegList",
            "pwg*.top",
            "pwgseeds.dat",
            "pwgcounters*.dat",
            "pwgubsigma.dat",
            "pwhg_checklimits"
        ]

    @property
    def integration_file_names(self):
        """! Wildcarded list of integration files that might be created by this process."""
        return [
            "pwgbtildeupb*.dat",
            "pwgfullgrid.dat",
            "pwggrid*.dat",
            "pwgremnupb*.dat",
            "pwgstat*.dat",
            "pwgubound*.dat",
            "pwgxgrid.dat"
        ]

    @property
    def mandatory_integration_file_names(self):
        """! Wildcarded list of integration files that are needed for this process."""
        return self.integration_file_names

    @property
    def powheg_version(self):
        """! Version of PowhegBox process."""
        return "V1"
