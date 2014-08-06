# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#ToolSvc = Service('ToolSvc')
from InDetBeamSpotFinder.InDetBeamSpotFinderConf import InDet__InDetBeamSpotReader as InDetBeamSpotReader

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += InDetBeamSpotReader()

from IOVDbSvc.CondDB import conddb

from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
#conddb.Folders+=[ "INDET"   + "/Indet/Beampos" + "<tag>nominal</tag>"]
IOVDbSvc = Service("IOVDbSvc")
# IOVDbSvc.overrideTags += ["<prefix>/Indet/Beampos</prefix> <tag>nominal</tag>"]
# IOVDbSvc.overrideTags += ["<prefix>/Indet/Beampos</prefix>"]
# IOVDbSvc.dbConnection  = "impl=cool;techno=sqlite;schema=beamspot.db;X:TESTCOOL"
IOVDbSvc.Folders += ["<dbConnection>impl=cool;techno=sqlite;schema=beamspot.db;X:BEAMSPOT</dbConnection> /Indet/Beampos<tag>nominal</tag>"]
#IOVDbSvc.Folders += ["<dbConnection>sqlite://;schema=beamspot.db;dbname=BEAMSPOT</dbConnection> /Indet/Beampos<tag>nominal</tag>"]
