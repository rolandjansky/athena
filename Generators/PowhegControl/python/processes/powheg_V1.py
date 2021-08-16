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
        @param warning_output list of patterns which if found in the output will be treated as warning in the log.
        @param error_output list of patterns which if found in the output will be treated as error in the log.
        @param info_output list of patterns which if found in the output will be treated as info in the log.
        """
        super(PowhegV1, self).__init__(base_directory, "POWHEG-BOX", executable_name, is_reweightable=False, warning_output = [], info_output = [], error_output = [], **kwargs)

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
        """! All files matching these patterns will be included in the gridpack."""
        return [
            "pwgbtildeupb*.dat",
            "pwgfullgrid.dat",
            "pwggrid*.dat",
            "pwgremnupb*.dat",
            "pwgstat*.dat",
            "pwgubound*.dat",
            "pwgxgrid*.dat"
        ]

    @property
    def mandatory_integration_file_names(self):
        """! Wildcarded list of integration files that are needed for this process."""
        """! If some of the patterns don't match any files before running, a warning will be made to inform that no pre-made integration grid will be used."""

        return self.integration_file_names

    @property
    def powheg_version(self):
        """! Version of PowhegBox process."""
        return "V1"
