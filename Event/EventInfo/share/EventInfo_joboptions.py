#
# Access parameters for EventInfo classes
#
#   See EventInfoParams.py for details
#

include.block ( "EventInfo/EventInfo_joboptions.py" )

from EventInfo.EventInfoParams import EventInfoParams

# test values:
#print EventInfoParams.EventType.is_simulation_bit()
#print EventInfoParams.EventType.is_testbeam_bit()
#print EventInfoParams.EventType.is_calibration_bit()
