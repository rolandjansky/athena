# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# Params for EventInfo classes
#
#   Set values for EventType bits
#     To use one accesses each bit value as:
#
#     EventInfoParams.EventType.is_simulation_bit()
#     EventInfoParams.EventType.is_testbeam_bit()
#     EventInfoParams.EventType.is_calibration_bit()
#
#     for example:
#       bit = EventInfoParams.EventType.is_calibration_bit()
#

class EventInfoParams:

    # EventType
    class EventType:

        is_simulation  = 0
        is_testbeam    = 1
        is_calibration = 2
    
        def Print (cls):
            for attr in dir(cls)[:]:
                print attr

        def is_simulation_bit (self):
            return self.is_simulation

        def is_testbeam_bit (self):
            return self.is_testbeam
        
        def is_calibration_bit (self):
            return self.is_calibration

        # Define class methods to allow use of 
        #   EventInfoParams.EventType.is_simulation_bit()
        is_simulation_bit  = classmethod(is_simulation_bit)
        is_testbeam_bit    = classmethod(is_testbeam_bit)
        is_calibration_bit = classmethod(is_calibration_bit)


