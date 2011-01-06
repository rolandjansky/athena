#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# vim: set fileencoding=utf-8 :
# Created by Andre Anjos <andre.dos.anjos@cern.ch>
# Thu 04 Oct 2007 04:19:14 PM CEST 

"""The hltpm module contains helpers and applications to HLT-fy OKS databases.

This module contains functions and applications that allows users to enable OKS
partitions to run HLT-enabled software on L2PUs and EF/PTs. It is designed to
operate on the top of the normal partition making proposed by TDAQ's
PartitionMaker and operates at a higher level, after partition generation,
fine tunning the partition or segment to start HLT-enabled applications instead
of their TDAQ dummy counterparts.
"""

from config.dal import module as dal_module
from pm.dal import * 
HLTdal = dal_module('HLTdal', 'daq/schema/hlt.schema.xml', [DFdal, dal])

import common
