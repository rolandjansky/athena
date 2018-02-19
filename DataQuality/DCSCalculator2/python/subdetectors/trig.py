# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DCSCalculator2.lib import DCSC_Subdetector_DefectsOnly, DCSC_Defect_Global_Variable, connect_adjacent_iovs_defect
from DCSCalculator2.variable import DefectIOV
from DQUtils import process_iovs
from DQUtils.sugar import IOVSet, RunLumi
from DQUtils.iov_truncator import truncate_to_run_iovs, make_run_iovs

class Online_Beamspot_Status(DCSC_Defect_Global_Variable):
    """
    Overloads calculate_good_iovs
    """
    input_db = 'COOLONL_INDET/CONDBR2'
    fetch_args = {'tag': "IndetBeamposOnl-HLT-UPD1-001-00"}
    timewise_folder = False
    def make_good_iovs(self, iovs):
        return IOVSet(list(connect_adjacent_iovs_defect(self.beamspot_generator(iovs))))

    def beamspot_generator(self, iovs):
        state = iovs.select_channels(0)
        events = process_iovs(iovs)
        for since, until, (state,) in events:
            if state is not None:
                #print since, until, state.status
                if state.status == 7 or state.status is None:
                    continue

                yield DefectIOV(since, until, 'TRIG_HLT_IDT_BSPOT_INVALID_STATUS', True,
                                comment='Bad online beamspot status')

class Trigger(DCSC_Subdetector_DefectsOnly):
    #__DISABLED__ = True
    folder_base = ''
    
    variables = [
        Online_Beamspot_Status('/Indet/Onl/Beampos', lambda x: True),
    ]

    def run(self, lbtime, run_iovs):
        self.evaluate_inputs(lbtime)
        return IOVSet(sum((truncate_to_run_iovs(
                        make_run_iovs([run_iovs]), var.iovs)[0]
                           for var in self.variables), []))
