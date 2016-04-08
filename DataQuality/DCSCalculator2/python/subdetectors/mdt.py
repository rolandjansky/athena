# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from collections import namedtuple
from pkg_resources import resource_string

from DQUtils.events import process_iovs_mc
from DQUtils.sugar import define_iov_type, RunLumi, IOVSet

from .lib import (DCSC_Subdetector, DCSC_Variable, 
                  DCSC_Variable_With_Mapping,
                  DCSC_DefectTranslate_Subdetector,
                  make_multi_mapping, GoodIOV, OUT_OF_CONFIG, map_channels)

MDTBA, MDTBC, MDTEA, MDTEC = 302, 303, 304, 305

class DCSC_Variable_MDT_Config(DCSC_Variable_With_Mapping):
    """
    Make GoodIOVS from the DROPPEDCH Folder
    """    
    is_config_variable = True
    
    def make_good_iovs(self, iovs):
        """
        Beware, deceptive function name. Here we are really 
        making "BAD" (OUT_OF_CONFIG) iovs.
        
        Modules to be dropped are named by string in the Chambers_disabled field
        of the DROPPEDCH folder. This variable is also a variable_with_mapping
        which later turns that string into a channelid consistent with the
        high-voltage.
        """
        result = IOVSet(iov_type=GoodIOV)
        
        for iov in iovs:
            since, until = iov.since, iov.until
            for chamber in (x for x in iov.Chambers_disabled.split(" ") if x):
                channel_1 = "PS_STATE__%s_1" % chamber  
                channel_2 = "PS_STATE__%s_2" % chamber
                result.append(GoodIOV(since, until, channel_1, OUT_OF_CONFIG))
                result.append(GoodIOV(since, until, channel_2, OUT_OF_CONFIG))
        
        return result

class DCSC_Variable_MDT_HV(DCSC_Variable):

    @define_iov_type
    def PSHVMLSTATE_VALUE(channel, fsm_currentState, V0Setpoint, V1Setpoint):
        "Stores the intermediate state of a hv channel"

    def merge_info(self, pshv_iovs, v0sp_iovs, v1sp_iovs):
        """
        Combines PSHVMLSTATE, PSV0SETPOINTS and PSV1SETPOINTS into one type of
        time-wise IoV.
        """
        
        events = process_iovs_mc(pshv_iovs, v0sp_iovs, v1sp_iovs)
        
        result = IOVSet()
        for since, until, channel, (pshv_iov, v0sp_iov, v1sp_iov) in events:
            if any((pshv_iov._is_empty, v0sp_iov._is_empty, v1sp_iov._is_empty)):
                # If any of hv, v0setpoint or v1setpoint has an IoV-hole, then
                # we don't generate an IoV.
                continue
            fsmState = pshv_iov.fsm_currentState
            v0sp     = v0sp_iov.readBackSettings_v0
            v1sp     = v1sp_iov.readBackSettings_v1
            result.add(since, until, channel, fsmState, v0sp, v1sp)

        return result.solidify(self.PSHVMLSTATE_VALUE)
    
    def read(self, query_range, folder_base, folder_name):
        """
        Need to read multiple folders to evaluate this variable, and map
        the SETPOINTS onto HV channels, before merging them into one type of
        IoV
        """
        super_read = super(DCSC_Variable_MDT_HV, self).read
        pshv_iovs = super_read(query_range, folder_base, folder_name)
        
        v0sp_iovs = super_read(query_range, folder_base, "PSV0SETPOINTS")
        v1sp_iovs = super_read(query_range, folder_base, "PSV1SETPOINTS")
        
        v0sp_iovs = map_channels(v0sp_iovs, self.setpoints_map, "PSV0SETPOINTS")
        v1sp_iovs = map_channels(v1sp_iovs, self.setpoints_map, "PSV1SETPOINTS")
        
        return self.merge_info(pshv_iovs, v0sp_iovs, v1sp_iovs)

def generate_mdt_mappings():
    """
    Creates dictionaries representing the mappings for different systems onto
    the HV module numbering
    """
    def fix_line(line):
        hv, lv, jtag, name, output_channel, sp_channel = line.split()
        if hv == "0" or output_channel == "0":
            # Ignore these lines
            return None
        return int(hv), int(lv), int(jtag), name, int(output_channel), int(sp_channel)

    mdtcodes = resource_string("DCSCalculator2.subdetectors.data", 
                               "mdt_codes.dat").strip().split("\n")
    lines = [l for l in [fix_line(x) for x in mdtcodes if x] if l]
    
    hv_to_output = make_multi_mapping((hv,   output_channel) for hv, lv, jtag, name, output_channel, sp in lines)
    lv_to_hv     = make_multi_mapping((lv,   hv)             for hv, lv, jtag, name, output_channel, sp in lines)
    jtag_to_hv   = make_multi_mapping((jtag, hv)             for hv, lv, jtag, name, output_channel, sp in lines)
    name_to_hv   = make_multi_mapping((name, hv)             for hv, lv, jtag, name, output_channel, sp in lines)
    sp_to_hv     = make_multi_mapping((sp,   hv)             for hv, lv, jtag, name, output_channel, sp in lines)
    
    return hv_to_output, lv_to_hv, jtag_to_hv, name_to_hv, sp_to_hv
    
def check_HV_fsm_and_voltage(iov):
    """
    Check for the fsm_currentState and then V1 or V0 Setpoint
    """
    return ((iov.fsm_currentState == "ON"      and iov.V1Setpoint > 3050) or
            (iov.fsm_currentState == "STANDBY" and iov.V0Setpoint > 3050))

from sys import argv

if "mdt-without-setpoint" in argv:
    # Old fsm test
    fsm_test = lambda iov: iov.fsm_currentState == "ON"
else:
    fsm_test = check_HV_fsm_and_voltage

class MDT(DCSC_DefectTranslate_Subdetector):
    """
    The MDTs need two pieces of specialisation:
    
    1. Channel numbering
    
       This requires a channel mapping to map channelids from different
       folders onto a consistent numbering scheme. Here we use the high-voltage
       channelid as the "master" channel. Everything else is mapped on to this.
    
    2. Dropped chambers
    
    3. Setpoints
    """
    folder_base = "/MDT/DCS"
    
    def __init__(self, *args, **kwargs):
        """
        Initialise mappings
        """
        kwargs['keep_dcsofl'] = True
        super(MDT, self).__init__(*args, **kwargs)
        
        mappings = generate_mdt_mappings()
        hv_to_output, lv_to_hv, jtag_to_hv, name_to_hv, sp_to_hv = mappings
        
        # Reorder the mapping for HV
        self.input_to_output_map = hv_to_output
        self.mapping = {}
        for key, value in self.input_to_output_map.iteritems():
            self.mapping.setdefault(value, []).append(key)
            
        self.set_input_mapping("PSLVCHSTATE", lv_to_hv)
        self.set_input_mapping("JTAGCHSTATE", jtag_to_hv)
        self.set_input_mapping("DROPPEDCH",   name_to_hv)
        
        self.get_variable("PSHVMLSTATE").setpoints_map = sp_to_hv

        self.translators = [MDT.color_to_defect_translator(flag, defect)
                            for flag, defect in ((MDTBA, 'MS_MDT_BA_STANDBY_HV'),
                                                 (MDTBC, 'MS_MDT_BC_STANDBY_HV'),
                                                 (MDTEA, 'MS_MDT_EA_STANDBY_HV'),
                                                 (MDTEC, 'MS_MDT_EC_STANDBY_HV'))]
    
    variables = [
        DCSC_Variable_MDT_HV      ("PSHVMLSTATE", check_HV_fsm_and_voltage),
        DCSC_Variable_With_Mapping("PSLVCHSTATE", lambda iov: iov.fsm_currentState == "ON"),
        DCSC_Variable_With_Mapping("JTAGCHSTATE", lambda iov: iov.fsm_currentState == "INITIALIZED"),
        
        # Evaluator not required because make_good_iovs overloaded above
        DCSC_Variable_MDT_Config("DROPPEDCH", None),
    ]
    
    # If you change this please consult with the Muon groups.
    # It was decided to make it the same across CSC, MDT, RPC and TGC.
    dead_fraction_caution = None
    dead_fraction_bad = 0.1

