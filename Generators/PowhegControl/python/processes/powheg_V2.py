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

        # # Universal options for all V2 processes
        # self.add_keyword("LOevents", [0, 1][is_LO])
        # self.add_keyword("btildeborn")
        # self.add_keyword("btildecoll")
        # self.add_keyword("btildereal")
        # self.add_keyword("btildevirt")
        # self.add_keyword("check_bad_st2")
        # self.add_keyword("compute_rwgt")
        # self.add_keyword("doublefsr")
        # self.add_keyword("evenmaxrat")
        # self.add_keyword("fastbtlbound")
        # self.add_keyword("fixedgrid")
        # self.add_keyword("fullrwgt")
        # self.add_keyword("fullrwgtmode")
        # self.add_keyword("itmx1rm")
        # self.add_keyword("itmx2rm")
        # self.add_keyword("lhrwgt_descr")
        # self.add_keyword("lhrwgt_group_combine")
        # self.add_keyword("lhrwgt_group_name")
        # self.add_keyword("lhrwgt_id")
        # self.add_keyword("minlo")
        # self.add_keyword("ncall1rm")
        # self.add_keyword("ncall2rm")
        # self.add_keyword("ncallfrominput")
        # self.add_keyword("noevents")
        # self.add_keyword("novirtual")
        # self.add_keyword("parallelstage")
        # self.add_keyword("stage2init")
        # self.add_keyword("storeinfo_rwgt")
        # self.add_keyword("storemintupb")
        # self.add_keyword("xgriditeration")

    @property
    def powheg_version(self):
        """! Version of PowhegBox process."""
        return "V2"
