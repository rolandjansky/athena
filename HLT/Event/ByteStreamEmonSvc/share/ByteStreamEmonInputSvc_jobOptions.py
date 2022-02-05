#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''
This is a trivial old-JO (RecEx-style) wrapper around the new (ComponentAccumulator) config
of ByteStream reading from EMon. It can be deleted once the clients are adapted to new config.
'''

from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
from ByteStreamEmonSvc.EmonByteStreamConfig import EmonByteStreamCfg

CAtoGlobalWrapper(EmonByteStreamCfg, ConfigFlags)
