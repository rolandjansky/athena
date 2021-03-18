# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# Minimal job options to test BeamSpot update. Usage:
#   1) Create bytestream input file: BeamSpotUpdate.py RAW.data
#   2) Run athenaHLT using new file from 1) and these job options
#
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AlgSequence import AthSequencer, AlgSequence

athenaCommonFlags.isOnline=True

globalflags.InputFormat='bytestream'
globalflags.DataSource='data'
globalflags.DetDescrVersion='ATLAS-R2-2016-01-00-01'
globalflags.ConditionsTag='CONDBR2-HLTP-2018-01'

from IOVDbSvc.CondDB import conddb
svcMgr.IOVDbSvc.GlobalTag=globalflags.ConditionsTag()

conddb.addFolder('beampos.db', '/Indet/Onl/Beampos <key>/Indet/Beampos</key> <tag>IndetBeamposOnl-HLT-UPD1-001-00</tag>',
                 className='AthenaAttributeList', extensible=True)

# Create topSequence
from TrigP1Test.BeamSpotUpdate import BeamSpotWriteAlg
from InDetBeamSpotFinder.InDetBeamSpotFinderConf import InDet__InDetBeamSpotReader
topSequence = AlgSequence()
topSequence += BeamSpotWriteAlg()
topSequence += InDet__InDetBeamSpotReader(VxContainer = "")

# Create conditions algorithms
from BeamSpotConditions.BeamSpotConditionsConf import BeamSpotCondAlg
condSeq = AthSequencer('AthCondSeq')
condSeq += BeamSpotCondAlg('BeamSpotCondAlg')
condSeq.BeamSpotCondAlg.OutputLevel = DEBUG

from TrigServices.TrigServicesConfig import enableCOOLFolderUpdates
enableCOOLFolderUpdates(svcMgr.HltEventLoopMgr.CoolUpdateTool)
