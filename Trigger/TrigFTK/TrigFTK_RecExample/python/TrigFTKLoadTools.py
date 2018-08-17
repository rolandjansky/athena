# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""                                                                                                 load common instances of FTK tools/services
jiri.masik@manchester.ac.uk
"""                                                                                                                                                                                                      

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# SiLorentzAngleTool for SCT
from AthenaCommon.AppMgr import ToolSvc
if not hasattr(ToolSvc, "SCTLorentzAngleTool"):
    from SiLorentzAngleSvc.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
    sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup()

from TrigFTK_RecExample.TrigFTK_DataProviderSvc_Config import TrigFTK_DataProviderSvc
theFTK_DataProviderSvc = TrigFTK_DataProviderSvc("TrigFTK_DataProviderSvc",
                                                 SCTLorentzAngleTool = ToolSvc.SCTLorentzAngleTool)
svcMgr += theFTK_DataProviderSvc


