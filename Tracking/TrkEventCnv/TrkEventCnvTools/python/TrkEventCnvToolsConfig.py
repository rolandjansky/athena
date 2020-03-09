# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# default configuration of TrkEventCnvTools
# example of a configuration in class deriving from a Configurable

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.AppMgr import ToolSvc
from RecExConfig import RecFlags as rec

from TrkEventCnvTools.TrkEventCnvToolsConf import Trk__EventCnvSuperTool
TrkEventCnvSuperTool = Trk__EventCnvSuperTool(name="EventCnvSuperTool")
ToolSvc+=TrkEventCnvSuperTool
TrkEventCnvSuperTool.DoMuons = rec.doMuon() and DetFlags.detdescr.Muon_on()
TrkEventCnvSuperTool.DoID    = rec.doInDet() and DetFlags.detdescr.ID_on()
