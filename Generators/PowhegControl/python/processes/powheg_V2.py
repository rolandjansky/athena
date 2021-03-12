# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from .powheg_base import PowhegBase
import glob

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
    def default_PDFs(self):
        """! Default PDFs for this process."""
        __PDF_list = list(range(260000, 260101)) # NNPDF30_nlo_as_0118 central with eigensets
        __PDF_list += [266000, 265000]     # NNPDF30_nlo_as_0119 and NNPDF30_nlo_as_0117
        __PDF_list += [13100, 25200]       # CT14nlo and MMHT2014nlo68clas118
        __PDF_list += list(range(90400, 90433))  # PDF4LHC15_nlo_30_pdfas with eigensets
        return __PDF_list

    @property
    def default_scales(self):
        """! Default scale variations for this process."""
        return [[1.0, 1.0, 1.0, 0.5, 0.5, 2.0, 2.0],\
                [1.0, 0.5, 2.0, 0.5, 1.0, 1.0, 2.0]]

    @property
    def files_for_cleanup(self):
        """! Wildcarded list of files created by this process that can be deleted."""
        return [
            "bornequiv",
            "FlavRegList",
            "fort.18",
            "mint_upb_btildeupb.top",
            "mint_upb_remnupb.top",
            "pwg*.top",
            "pwg*stat.dat",
            "pwgboundviolations*.dat",
            "pwgcounters*.dat",
            "pwgseeds.dat",
            "pwgubsigma.dat",
            "pwhg_checklimits",
            "realequiv*",
            "reweighting_input.xml",
            "virtequiv"
        ]

    @property
    def integration_file_names(self):
        """! Wildcarded list of integration files that might be created by this process."""
        return [
            "pwgbtildeupb*.dat",
            "pwgfullgrid*.dat",
            "pwggrid*.dat",
            "pwggridinfo*.dat",
            "pwgremnupb*.dat",
            "pwgubound*.dat",
            "pwgxgrid.dat",
        ]

    @property
    def mandatory_integration_file_names(self):
        """! Wildcarded list of integration files that are needed for this process."""
        return [
            "pwgbtildeupb*.dat",
            "pwgfullgrid*.dat",
            "pwggrid*.dat",
            "pwgremnupb*.dat",
            "pwgubound*.dat",
            "pwgxgrid.dat",
        ]

    @property
    def powheg_version(self):
        """! Version of PowhegBox process."""
        return "V2"

    def stage_is_completed(self, stage):
        """! Set whether the specified POWHEG-BOX generation stage is complete."""
        if stage == 1:
            required_files = ["pwg*xg*.dat"]
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
