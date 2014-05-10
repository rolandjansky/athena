#
# Job options to test ROBRequestAlgo
#

include('TriggerRelease/runHLT_standalone.py')

from TrigGenericAlgs.test_roblist import robs
from TrigGenericAlgs.test_rosmap import ros_rob_map

rob_ros_map = {}
for ros,roblist in ros_rob_map.iteritems():
   for rob in roblist:
      rob_ros_map[rob] = ros

topSequence.TrigSteer_L2.DummyROBRequest.OutputLevel = DEBUG      
topSequence.TrigSteer_L2.DummyROBRequest.enabledROBs = robs
topSequence.TrigSteer_L2.DummyROBRequest.RobRosMapping = rob_ros_map
topSequence.TrigSteer_L2.DummyROBRequest.ROBRequest = ['0x41#2',
                                                       '0x410000',
                                                       'ROS-0x1c01#2',
                                                       'ROS-0x1c93, ROS-0x1c65#4',
                                                       'ROS-0xffff']
