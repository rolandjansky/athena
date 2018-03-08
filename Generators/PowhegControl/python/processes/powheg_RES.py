# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from .powheg_base import PowhegBase
import glob


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
    def files_for_cleanup(self):
        """! Wildcarded list of files created by this process that can be deleted."""
        return [
            "allborn_equiv",
            "FlavRegList",
            "mint*.top",
            "parameters.ol",
            "pwg*.top",
            "pwg*stat.dat",
            "pwgboundviolations*.dat",
            "pwgcounters*.dat",
            "pwgseeds.dat",
            "pwhg_checklimits",
            "sigreal_btl0_equiv",
            "sigregular_equiv",
            "sigvirtual_equiv"
        ]

    @property
    def integration_file_names(self):
        """! Wildcarded list of integration files that might be created by this process."""
        return [
            "pwg*upb*.dat",
            "pwg*xgrid*.dat",
            "pwgfullgrid*.dat",
            "pwggrid*.dat",
            "pwgubound*.dat"
        ]

    @property
    def powheg_version(self):
        """! Version of PowhegBox process."""
        return "RES"

    def stage_is_completed(self, stage):
        """! Set whether the specified POWHEG-BOX generation stage is complete."""
        if stage == 1:
            required_files = ["pwg*xgrid*.dat"]
        elif stage == 2:
            required_files = ["pwg*upb*.dat", "pwggrid*.dat"]
        elif stage == 3:
            required_files = ["pwgfullgrid*.dat", "pwgubound*.dat"]
        else:
            return False

        # Check that required files have been found
        for required_file in required_files:
            if not glob.glob(required_file):
                return False
        return True
