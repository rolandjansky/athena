# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# default configuration of TrkEventCnvTools
# example of a configuration in class deriving from a Configurable

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.AppMgr import ToolSvc
from TrkEventCnvTools.TrkEventCnvToolsConf import Trk__EventCnvSuperTool
TrkEventCnvSuperTool = Trk__EventCnvSuperTool(name="EventCnvSuperTool")
ToolSvc+=TrkEventCnvSuperTool
TrkEventCnvSuperTool.DoMuons = DetFlags.detdescr.Muon_on()
TrkEventCnvSuperTool.DoID    = DetFlags.detdescr.ID_on()
