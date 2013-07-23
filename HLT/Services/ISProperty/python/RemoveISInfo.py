# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
#update is information for ISPropTestAlg
#
from ispy import *

#get partition
p = IPCPartition('MyPartition')

#get data object
obj = ISObject(p,'MyServer.MyTestAlg','ISPropTestAlg')
obj.remove()
