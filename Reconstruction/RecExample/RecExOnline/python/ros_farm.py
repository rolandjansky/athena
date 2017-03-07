#!/usr/bin/env tdaq_python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# vim: set fileencoding=utf-8 :
# Created by Andre Anjos <andre.dos.anjos@cern.ch>
# Wed 19 Sep 2007 11:54:23 AM CEST 

"""This is an example of the ROS farm configuration on ATLAS Point 1.
"""

import pm.multinode
import pm.farm

# We prefer to randomly split the available ROSs using a particular rob
# list. These lists can be generated on-the-fly, using the EventApps bindings
# to scan datafiles, or if you prefer, just import the output of
# rosconf-from-data.py --py. We prefer that later because it is faster.
from robhit import robhit
ros_farm = pm.multinode.ros_farm_random(robhit,[pm.farm.local_computer(), pm.farm.local_computer()])

# if you want to pretty print in the end and to verify this module is
# actually loadable, uncomment the following line
# print pm.multinode.prettyprint(ros_farm)
