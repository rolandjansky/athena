#!/usr/bin/env tdaq_python
# vim: set fileencoding=utf-8 :
# Created by Andre Anjos <andre.dos.anjos@cern.ch>
# Mon 10 Sep 2007 06:59:03 PM CEST 

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

"""Defines the default farm structure to be used in multinode partitions.

This module defines a dummy, default farm structure to be used in multinode
partitions. The definition of the farm objects can be found in the several
gen_* functions in the pm.multinode submodule.

For obvious reasons, all components will be put on the local computer.
"""

from pm.partition.default_farm import *

# Overwrite the default ROS configuration
import robhit
ros_farm = pm.multinode.ros_farm_random(robhit.robhit, [localhost])


