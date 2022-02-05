# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DCSCalculator2.lib import DCSC_Subdetector, DCSC_Defect_Global_Variable
from DCSCalculator2.variable import DefectIOV
from DQUtils import process_iovs
from DQUtils.sugar import IOVSet

TOLERANCE_SOLENOID = 5 # Amperes
TOLERANCE_TOROID = 10 # Amperes

class Magnet_Currents(DCSC_Defect_Global_Variable):
    """
    Overloads calculate_good_iovs
    """
    def make_good_iovs(self, iovs):
        atlsol_iovs = self.magnet_iov_generator(iovs, 'GLOBAL_SOLENOID', 1, 2,
                                                TOLERANCE_SOLENOID)
        atltor_iovs = self.magnet_iov_generator(iovs, 'GLOBAL_TOROID', 3, 4,
                                                TOLERANCE_TOROID)
        return IOVSet(list(atlsol_iovs) + list(atltor_iovs))

    def magnet_iov_generator(self, iovs, system, 
                             measured_channel, desired_channel, tolerance):
                             
        measured_iovs = iovs.select_channels(measured_channel)
        desired_iovs  = iovs.select_channels(desired_channel)
        
        events = process_iovs(measured_iovs, desired_iovs)
        
        for since, until, (measured, desired) in events:

            # 28-05-2015: excluding empty 'desired' value, because how do we make
            # a decision without an expectation? Should debug this some more, as
            # the issue came up in 2015 run 253014.
            if measured is not None and desired is not None and not desired._is_empty:
                # NOTE: if measured is 'empty', this is always true
                if measured.value is None:
                    continue
                elif measured.value <= tolerance:
                    # Magnet off
                    defect = system + '_OFF'

                elif abs(measured.value - desired.value) <= tolerance:
                    if ((system == 'GLOBAL_SOLENOID' and abs(desired.value - 7730.) > tolerance)
                        or (system == 'GLOBAL_TOROID' and abs(desired.value - 20400.) > tolerance)):
                        # Magnet has non-nominal current
                        defect = system + '_NOTNOMINAL'
                    else:
                        defect = None
                else:
                    # Magnet is ramping
                    defect = system + '_RAMPING'

                if defect is None:
                    continue

                mcurrent = '%.1f' % measured.value if measured.value is not None else 'None'
                scurrent = '%.1f' % desired.value if desired.value is not None else 'None'
                yield DefectIOV(since, until, defect, True,
                                comment='Measured current: %s, Set current: %s' % (mcurrent, scurrent))


class Magnets(DCSC_Subdetector):
    #__DISABLED__ = True
    folder_base = '/EXT/DCS/MAGNETS'
    
    variables = [
        Magnet_Currents('SENSORDATA', lambda x: True)
    ]

    def __init__(self, tolerance=2):
        pass
    
    def run(self, lbtime, run_iovs=None):
        """
        The magnets are very different to all other systems. There is no need
        to run the usual processing, since the variables spit out iovs in one
        step.
        """
        self.evaluate_inputs(lbtime)
        return self.variables[0].iovs
