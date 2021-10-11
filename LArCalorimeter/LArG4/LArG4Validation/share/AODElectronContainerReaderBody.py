# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


from LArG4Validation.LArG4ValidationConf import AODReader
topSequence += AODReader()

topSequence.AODReader.OutputLevel = WARNING

AthenaEventLoopMgr = Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel = ERROR

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = args

theApp.Dlls += [ 'RootHistCnv' ]
theApp.HistogramPersistency = 'ROOT'
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE DATAFILE=\'"+"ntuple.root"+"\'  TYP='ROOT'  OPT='NEW'" ]
