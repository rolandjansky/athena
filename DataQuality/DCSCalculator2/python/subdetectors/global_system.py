# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DCSCalculator2.lib import DCSC_Subdetector_DefectsOnly, DCSC_Defect_Global_Variable, connect_adjacent_iovs_defect
from DCSCalculator2.variable import DefectIOV
from DQUtils import process_iovs
from DQUtils.sugar import IOVSet, RunLumi
from DQUtils.iov_truncator import truncate_to_atlas_runs

class TDAQ_Ready(DCSC_Defect_Global_Variable):
    """
    Overloads calculate_good_iovs
    """
    input_db = 'COOLONL_TDAQ/CONDBR2'
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
    Overloads calculate_good_iovs & quantize
    Latter because this is a strange COOL folder that is timewise but already quantized
    as run-LB
    """
    input_db = 'COOLOFL_TRIGGER/CONDBR2'
    #fetch_args = dict(tag="OflLumiAcct-13TeV-004")
    fetch_args = dict(tag="OflLumiAcct-001")
    deadfraction_threshold = 0.9
    
    def make_good_iovs(self, iovs):
        return IOVSet(list(connect_adjacent_iovs_defect(self.tdaq_busy_generator(iovs))))

    def tdaq_busy_generator(self, iovs):
        events = process_iovs(iovs)
	counter=0

        for since, until, (state,) in events:
            if state.Run == 0 or state.Run is None: continue
	    #print state
            if state is not None:
                if state.LiveFraction is None:
                    deadfrac=1
                    print 'WARNING: LiveFraction is "None" for', state.Run, state.LumiBlock 
                else:
                    deadfrac = 1-state.LiveFraction
                if deadfrac < self.deadfraction_threshold:
                    continue
                yield DefectIOV(RunLumi(state.Run, state.LumiBlock), 
                                RunLumi(state.Run, state.LumiBlock+1), 
                                'GLOBAL_BUSY', True,
                                comment='Average live fraction %.1f%%' % ((1-deadfrac)*100))
		counter +=1
	
	counter_max=counter
	counter=0
	#print counter_max
	events = process_iovs(iovs)
        for since, until, (state,) in events:
	    if state is not None and state.Run is not None:
                deadfrac = 1-state.LiveFraction
                if deadfrac < self.deadfraction_threshold:
                    continue
	        #print state.Run
	        counter +=1
	        if state.Run == 0 and counter <counter_max:
		    print 'ERROR: Wrong run number in LumiAccounting; here is the IOV: '
                    print state
		    #print list(iovs)
		    continue

    def quantize(self, lbtime, iovs):
        return iovs

class LUMI_EmittanceScan(DCSC_Defect_Global_Variable):
    """
    Overloads calculate_good_iovs & quantize
    Latter because this is a strange COOL folder that is timewise but already quantized
    as run-LB
    """
    input_db = 'COOLONL_TDAQ/CONDBR2'
    
    def make_good_iovs(self, iovs):
        return IOVSet(list(connect_adjacent_iovs_defect(self.emittance_generator(iovs))))

    def emittance_generator(self, iovs):
        events = process_iovs(iovs)
	counter=0

        for since, until, (state,) in events:
	    #print state, state.RunLB & 0xffffffff if state.RunLB else 0
            if state is not None and state.RunLB is not None:
                thisrun = state.RunLB >>32
                # pseudo-LB and not to be trusted
                if thisrun == 0: continue
                thisLB = state.RunLB & 0xffffffff
                yield DefectIOV(RunLumi(thisrun, thisLB), 
                                RunLumi(thisrun, thisLB+1), 
                                'LUMI_EMITTANCESCAN', True,
                                comment='Emittance scan'
                                )

    def quantize(self, lbtime, iovs):
        return iovs

class Global(DCSC_Subdetector_DefectsOnly):
    #__DISABLED__ = True
    folder_base = ''
    
    variables = [
        TDAQ_Ready('/TDAQ/RunCtrl/DataTakingMode', lambda x: True),
        #TDAQ_Busy('/TRIGGER/LUMI/PerBcidDeadtime', lambda x: True),
        TDAQ_Busy('/TRIGGER/OFLLUMI/LumiAccounting', lambda x: True),
        LUMI_EmittanceScan('/TDAQ/OLC/LHC/SCANDATA', lambda x: True),
    ]

    def __init__(self, tolerance=2):
        pass
    
    def run(self, lbtime, run_iovs):
        self.evaluate_inputs(lbtime)
        return IOVSet(sum((truncate_to_atlas_runs(var.iovs)[0] if len(var.iovs) > 0 else []
                          for var in self.variables), []))
