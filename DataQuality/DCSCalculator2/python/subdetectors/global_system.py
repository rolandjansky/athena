# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DCSCalculator2.lib import DCSC_Subdetector_DefectsOnly, DCSC_Defect_Global_Variable
from DCSCalculator2.variable import DefectIOV
from DQUtils import process_iovs
from DQUtils.sugar import IOVSet
from DQUtils.iov_truncator import truncate_to_atlas_runs
from DQUtils.iov_arrangement import connect_adjacent_iovs

class TDAQ_Ready(DCSC_Defect_Global_Variable):
    """
    Overloads calculate_good_iovs
    """
    input_db = 'COOLONL_TDAQ/COMP200'
    timewise_folder = False
    def make_good_iovs(self, iovs):
        return IOVSet(list(self.tdaq_ready_generator(iovs)))

    def tdaq_ready_generator(self, iovs):
        #state = iovs.select_channels(0)
        
        events = process_iovs(iovs)
        
        for since, until, (state,) in events:
            
            if state is not None:
                if state.ReadyForPhysics == 1:
                    continue

                yield DefectIOV(since, until, 'GLOBAL_NOTREADY', True,
                                comment='ReadyForPhysics not set')

class TDAQ_Busy(DCSC_Defect_Global_Variable):
    """
    Overloads calculate_good_iovs
    """
    input_db = 'COOLONL_TRIGGER/COMP200'
    timewise_folder = False
    deadfraction_threshold = 0.9
    
    def make_good_iovs(self, iovs):
        return IOVSet(list(connect_adjacent_iovs(self.tdaq_busy_generator(iovs))))

    def tdaq_busy_generator(self, iovs):
        events = process_iovs(iovs)
        
        for since, until, (state,) in events:
            if state is not None and state.TurnCounter > 0:
                blob = state.LowPriority
                bitzero = float(ord(blob[0]) | (ord(blob[1]) << 8)
                           | (ord(blob[2]) << 16))
                deadfrac = bitzero/state.TurnCounter
                if deadfrac < self.deadfraction_threshold:
                    continue

                yield DefectIOV(since, until, 'GLOBAL_BUSY', True,
                                comment='Average live fraction %.1f%%' % ((1-deadfrac)*100))

class Global(DCSC_Subdetector_DefectsOnly):
    #__DISABLED__ = True
    folder_base = ''
    
    variables = [
        TDAQ_Ready('/TDAQ/RunCtrl/DataTakingMode', lambda x: True),
        TDAQ_Busy('/TRIGGER/LUMI/PerBcidDeadtime', lambda x: True)
    ]

    def __init__(self, tolerance=2):
        pass
    
    def run(self, lbtime, run_iovs):
        self.evaluate_inputs(lbtime)
        return IOVSet(sum((truncate_to_atlas_runs(var.iovs)[0]
                          for var in self.variables), []))
