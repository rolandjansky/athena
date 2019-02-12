#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

import AthenaCommon.AtlasUnixGeneratorJob

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from SGComps.SGCompsConf import SGInputLoader
job += SGInputLoader(OutputLevel=INFO, ShowEventDump=False)
job.SGInputLoader.Load = [ ('EventInfo','McEventInfo') ]

from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
job += xAODMaker__EventInfoCnvAlg()

from AthExHive.AthExHiveConf import *
job += HiveAlgA(OutputLevel=DEBUG,Time=20)
job += HiveAlgB(OutputLevel=DEBUG,Time=10)
job += HiveAlgC(OutputLevel=DEBUG,Time=190)
job += HiveAlgD(OutputLevel=DEBUG,Time=10)
job += HiveAlgE(OutputLevel=DEBUG,Time=30)
job += HiveAlgG(OutputLevel=DEBUG,Time=10)
job += HiveAlgF(OutputLevel=DEBUG,Time=30)

#from StoreGate.StoreGateConf import StoreGateSvc
#svcMgr.StoreGateSvc.Dump=True

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 100

