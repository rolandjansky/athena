#!/usr/bin/env tdaq_python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# vim: set fileencoding=utf-8 :
# Created by Andre Anjos <andre.dos.anjos@cern.ch>
# Mon 10 Sep 2007 06:59:03 PM CEST 

"""Defines the default farm structure to be used in multinode partitions.

This module defines a dummy, default farm structure to be used in multinode
partitions. The definition of the farm objects can be found in the several
gen_* functions in the pm.multinode submodule.

For obvious reasons, all components will be put on the local computer.
"""
import pm.multinode
import pm.farm

includes = ['./pcatr-cluster.data.xml']
hosts = pm.farm.load(includes[0], short_names=True)

ctrl = hosts['pcatr14']
l2pu = [hosts['pcatr15'], hosts['pcatr16']]
l2sv = hosts['pcatr16']
ef = [hosts['pcatr17']]
dfm = hosts['pcatb148']
sfi = hosts['pcatb149']
pros = hosts['pcatb120']
sfo = hosts['pcatb122']
ros = 2 * [hosts['pcatr08']]

# prepares the LVL2 subsystem
l2pu_subfarm = pm.multinode.l2pu_subfarm(l2pu, default_host=ctrl)
l2_subfarm = pm.multinode.l2_subfarm(l2sv, [l2pu_subfarm], default_host=ctrl)
l2_farm = pm.multinode.l2_farm([l2_subfarm], default_host=ctrl)

# prepares the ROS subsystem
import robhit
ros_farm = pm.multinode.ros_farm_random(robhit.robhit, ros, default_host=ctrl)

# prepares the EB-EF subsystem
ef_subfarm = pm.multinode.ef_subfarm(ef, default_host=ctrl)
ebf_subfarm = pm.multinode.ebf_subfarm([sfi], [ef_subfarm], default_host=ctrl)
ebef_farm = pm.multinode.ebef_farm(dfm, [ebf_subfarm], sfo=[sfo], l2rh=pros,
    default_host=ctrl)

