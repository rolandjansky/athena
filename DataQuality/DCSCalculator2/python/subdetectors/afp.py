# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import logging; log = logging.getLogger("DCSCalculator2.variable")

from DQUtils.sugar import make_iov_type, IOVSet

import DCSCalculator2.config as config
from DCSCalculator2.consts import ( RED, YELLOW )
from DCSCalculator2.libcore import map_channels
from DCSCalculator2.subdetector import DCSC_DefectTranslate_Subdetector
from DCSCalculator2.variable import DefectIOV, DCSC_Variable_With_Mapping, DCSC_Variable

class DCSC_Merged_Variable(DCSC_Variable):
    def __init__(self, evaluator, folders, **kwargs):
        mapping = kwargs.pop('mapping', None)
        folder_merge = ','.join(folders)
            
        super(DCSC_Merged_Variable, self).__init__(folder_merge, evaluator, **kwargs)
        self.folder_names = folders
        self.variable_channels_map = mapping
    
    def get_variable(self, name):
        for var in self.variables:
            if var.folder_name == name:
                return var
        raise RuntimeError("Folder '%s' not found" % name)

    def read(self, query_range, folder_base, folder_name):
        var_iovs = []
        folders = folder_name.split(',')

        for folder in folders:
            iovs = DCSC_Variable.read(self, query_range, folder_base, folder)
            if folder in self.variable_channels_map:
                iovs = map_channels(iovs, self.variable_channels_map[folder], folder)
            var_iovs.append(iovs)

        iovs = self.merge_input_variables(*var_iovs)

        if config.opts.check_input_time:
            self.print_time_info(iovs)
            
        if log.isEnabledFor(logging.INFO):
            input_hash = hash(iovs)
            self.input_hashes.append(input_hash)
            #log.info("  -> Input hash: % 09x (len=%i)", input_hash, len(iovs))
            
        return iovs

    def merge_input_variables(self, *inputs):
        type_bases = [iovs.iov_type for iovs in inputs]
        base_names = [base.__name__[:-4] for base in type_bases]
        attributes = [base.lower() for base in base_names]
        clazz = make_iov_type('MERGER_OF_' + '_AND_'.join(base_names), tuple(['channel'] + attributes))

        inputs_by_channel = [iovs.by_channel for iovs in inputs]
        all_channels = sorted(set(y for x in inputs_by_channel for y in x.keys()))

        result = []
        for channel in all_channels:
            c_inputs = [x[channel] for x in inputs_by_channel]
            result.extend(iov for iov in self.merge_inputs(clazz, channel, *c_inputs))

        return IOVSet(result, iov_type=clazz, origin=self.folder_names)

    def merge_inputs(self, clazz, channel, *inputs):
        result = []
        lengths = [len(iovs) for iovs in inputs]
        indices = [0 for x in inputs]
        while sum([1 for index,length in zip(indices,lengths) if index >= length]) < 1:
            iovs = [iovs[index] for iovs,index in zip(inputs,indices)]
            since = max(iov.since for iov in iovs)
            until = min(iov.until for iov in iovs)
            result.append(clazz(since, until, channel, *iovs))
            indices = [index+1 if until==iov.until else index for iov,index in zip(iovs,indices)]
        return IOVSet(result, iov_type=clazz, origin=self.folder_names)



HV_A_FAR, HV_A_NEAR, HV_C_FAR, HV_C_NEAR =  1,  5,  9, 13
LV_A_FAR, LV_A_NEAR, LV_C_FAR, LV_C_NEAR = 21, 25, 29, 33
TOF_A, TOF_C = 51, 59
STATION_A_FAR, STATION_A_NEAR, STATION_C_FAR, STATION_C_NEAR = 101, 105, 109, 113

HV_DEAD_BAND = 0.05
TOF_DEAD_BAND = 0.9

LV_CURRENT_LOW, LV_CURRENT_HIGH = 0.4, 0.6

def remove_None(value, default):
    return value if value is not None else default

class AFP(DCSC_DefectTranslate_Subdetector):
    folder_base = '/AFP/DCS'
    variables = [
        DCSC_Merged_Variable(
            lambda iov: -remove_None(iov.hv.voltage, 0) > iov.hv_voltage_set.voltageSet - HV_DEAD_BAND,
            ['SIT/HV', 'SIT/HV_VOLTAGE_SET'],
            mapping={
                'SIT/HV': dict(zip([6,7,1,2,8,3,4,9,10,11,12,5,13,14,15,16], range(1,17)))
            }
        ),
        DCSC_Variable_With_Mapping('SIT/LV', lambda iov: LV_CURRENT_LOW <= remove_None(iov.current, 0) <= LV_CURRENT_HIGH),
        DCSC_Merged_Variable(
            lambda iov: -remove_None(iov.tof.pmt_voltage, 0) > iov.tof_pmt_voltage_set.pmt_voltageSet - TOF_DEAD_BAND,
            ['TOF', 'TOF_PMT_VOLTAGE_SET'],
            mapping={
                'TOF': {1: TOF_A, 2: TOF_C},
                'TOF_PMT_VOLTAGE_SET': {1: TOF_A, 2: TOF_C},
            }
        ),
        DCSC_Variable_With_Mapping('STATION', lambda iov: iov.inphysics is True),
    ]

    dead_fraction_caution = 0.01
    dead_fraction_bad = 0.75

    mapping = {
        HV_A_FAR:  range( 1,  5),
        HV_A_NEAR: range( 5,  9),
        HV_C_FAR:  range( 9, 13),
        HV_C_NEAR: range(13, 17),
        
        LV_A_FAR:  range(21, 25),
        LV_A_NEAR: range(25, 29),
        LV_C_FAR:  range(29, 33),
        LV_C_NEAR: range(33, 37),

        TOF_A: [TOF_A], TOF_C: [TOF_C],

        STATION_A_FAR: [STATION_A_FAR], STATION_A_NEAR: [STATION_A_NEAR],
        STATION_C_FAR: [STATION_C_FAR], STATION_C_NEAR: [STATION_C_NEAR],
    }

    @staticmethod
    def color_to_defect_translator(channel, defect, color, comment):
        def translator_core(iovs):
            return [DefectIOV(iov.since, iov.until, defect, True,
                            comment=comment(iov))
                    for iov in iovs if iov.channel == channel
                    and iov.Code == color]
        return translator_core

    def __init__(self, *args, **kwargs):
        super(AFP, self).__init__(*args, **kwargs)
        self.set_input_mapping('SIT/LV', dict(zip(list(range(9, 17)) + list(range(1, 9)), range(21, 37))))
        self.set_input_mapping('STATION', dict(zip([1, 2, 3, 4], [STATION_C_FAR, STATION_C_NEAR, STATION_A_FAR, STATION_A_NEAR])))
        self.translators = [AFP.color_to_defect_translator(*cdcc)
                            for cdcc in [
                                (HV_C_FAR,  'AFP_C_FAR_SIT_PARTIALLY_OPERATIONAL_HV',  YELLOW, AFP.comment_HV),
                                (HV_C_FAR,  'AFP_C_FAR_SIT_NOT_OPERATIONAL_HV',        RED,    AFP.comment_HV),
                                (HV_C_NEAR, 'AFP_C_NEAR_SIT_PARTIALLY_OPERATIONAL_HV', YELLOW, AFP.comment_HV),
                                (HV_C_NEAR, 'AFP_C_NEAR_SIT_NOT_OPERATIONAL_HV',       RED,    AFP.comment_HV),
                                (HV_A_FAR,  'AFP_A_FAR_SIT_PARTIALLY_OPERATIONAL_HV',  YELLOW, AFP.comment_HV),
                                (HV_A_FAR,  'AFP_A_FAR_SIT_NOT_OPERATIONAL_HV',        RED,    AFP.comment_HV),
                                (HV_A_NEAR, 'AFP_A_NEAR_SIT_PARTIALLY_OPERATIONAL_HV', YELLOW, AFP.comment_HV),
                                (HV_A_NEAR, 'AFP_A_NEAR_SIT_NOT_OPERATIONAL_HV',       RED,    AFP.comment_HV),

                                (LV_C_FAR,  'AFP_C_FAR_SIT_PARTIALLY_OPERATIONAL_LV',  YELLOW, AFP.comment_LV),
                                (LV_C_FAR,  'AFP_C_FAR_SIT_NOT_OPERATIONAL_LV',        RED,    AFP.comment_LV),
                                (LV_C_NEAR, 'AFP_C_NEAR_SIT_PARTIALLY_OPERATIONAL_LV', YELLOW, AFP.comment_LV),
                                (LV_C_NEAR, 'AFP_C_NEAR_SIT_NOT_OPERATIONAL_LV',       RED,    AFP.comment_LV),
                                (LV_A_FAR,  'AFP_A_FAR_SIT_PARTIALLY_OPERATIONAL_LV',  YELLOW, AFP.comment_LV),
                                (LV_A_FAR,  'AFP_A_FAR_SIT_NOT_OPERATIONAL_LV',        RED,    AFP.comment_LV),
                                (LV_A_NEAR, 'AFP_A_NEAR_SIT_PARTIALLY_OPERATIONAL_LV', YELLOW, AFP.comment_LV),
                                (LV_A_NEAR, 'AFP_A_NEAR_SIT_NOT_OPERATIONAL_LV',       RED,    AFP.comment_LV),

                                (TOF_A, 'AFP_A_FAR_TOF_NOT_OPERATIONAL_HV', RED, AFP.comment_TOF),
                                (TOF_C, 'AFP_C_FAR_TOF_NOT_OPERATIONAL_HV', RED, AFP.comment_TOF),

                                (STATION_C_FAR,  'AFP_C_FAR_IN_GARAGE',  RED, AFP.comment_STATION),
                                (STATION_C_NEAR, 'AFP_C_NEAR_IN_GARAGE', RED, AFP.comment_STATION),
                                (STATION_A_FAR,  'AFP_A_FAR_IN_GARAGE',  RED, AFP.comment_STATION),
                                (STATION_A_NEAR, 'AFP_A_NEAR_IN_GARAGE', RED, AFP.comment_STATION),
                            ]]

    @staticmethod
    def comment_HV(iov):
        planes = iov.NConfig - iov.NWorking
        return '%d SiT plane%s out of nominal voltage' % (planes, '' if planes == 1 else 's')

    @staticmethod
    def comment_LV(iov):
        planes = iov.NConfig - iov.NWorking
        return '%d SiT plane%s not configured for data-taking' % (planes, '' if planes == 1 else 's')

    @staticmethod
    def comment_TOF(iov):
        return 'ToF out of nominal voltage'

    @staticmethod
    def comment_STATION(iov):
        return 'Station not in physics'

    def merge_variable_states(self, states):
        """
        Merge input channel states across variables, taking the worst.
        
        Ignore configuration variables and variables without channel.
        """

        # Remove variables without channel
        states = [state for state in states if state and state.channel is not None]
                
        # More simplistic way of doing the above, but cannot handle config vars:
        return min(state.good for state in states) if len(states) > 0 else None
