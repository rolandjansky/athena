# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Lucid's criteria are that:

1) all non-excluded modules are good.
2) The magnet calculator does not report that it is ramping
"""

from DQUtils import process_iovs
from DQUtils.sugar import IOVSet

from DCSCalculator2.lib import DCSC_Subdetector_DefectsOnly, DCSC_Defect_Global_Variable
from DCSCalculator2.variable import DefectIOV

from DCSCalculator2.subdetectors.magnets import Magnet_Currents, Magnets

class Lucid_Magnets(DCSC_Defect_Global_Variable):

    def __init__(self):
        self.folder_name = None # unused
 
    # Hmmm, magnets get run twice because of this
    def calculate_good_iovs(self, lbtime, subdetector):
        magnets = Magnets()
        result = magnets.run(lbtime)
        by_defect = result.by_channel

        toroid_ramp = by_defect.get("GLOBAL_TOROID_RAMPING", IOVSet())
        solenoid_ramp = by_defect.get("GLOBAL_SOLENOID_RAMPING", IOVSet())

        self.input_hashes = [hash(toroid_ramp), hash(solenoid_ramp)]
        result = IOVSet()

        events = process_iovs(toroid_ramp, solenoid_ramp)
        for since, until, (toroid, solenoid) in events:
            if toroid or solenoid:
                result.add(since, until, "LCD_MAGNETS_RAMPING", True, "")

        self.iovs = result.solidify(DefectIOV)
 
        return self

class Lucid_Voltage(DCSC_Defect_Global_Variable):
    """
    Require that /LUCID/DCS/(LV|HV) has powerStatus == True for all modules
    which are not in the excluded_channels set
    """

    def __init__(self, folder_name, defect_name, excluded_channels=None):
        super(Lucid_Voltage, self).__init__(folder_name, None)
        self.defect_name = defect_name
        self.excluded_channels = excluded_channels

    def make_good_iovs(self, iovs):
 
        # Filter out channels which are permanently dead
        excluded_channels = self.excluded_channels
        if excluded_channels:
            iovs = iovs.empty(i for i in iovs 
                              if i.channel not in excluded_channels)
            
        chans, iovsets = iovs.chans_iovsets
        
        result = IOVSet()
        for since, until, states in process_iovs(*iovsets):
            if not all(s and s.powerStatus for s in states):
                result.add(since, until, self.defect_name, True, "")
 
        return result.solidify(DefectIOV)

class Lucid(DCSC_Subdetector_DefectsOnly):
    folder_base = '/LUCID/DCS'
 
    variables = [
        # 20170317 - LUCID hasn't worked in Run 2
        #Lucid_Magnets(),
        #Lucid_Voltage("LV", "LCD_LV_OFF"),
        #Lucid_Voltage("HV", "LCD_HV_OFF", excluded_channels=set((5, 16, 19, 24))),
    ]
