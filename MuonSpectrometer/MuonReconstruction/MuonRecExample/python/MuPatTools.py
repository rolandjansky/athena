# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Configuration of tools for muon track reconstruction"""

###############################################################
#
# Configuration for MuPat
#
#==============================================================

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam
from AthenaCommon.BFieldFlags import jobproperties
from AthenaCommon import CfgMgr

from RecExConfig.RecFlags import rec

from MuonRecFlags import muonRecFlags
from MuonStandaloneFlags import muonStandaloneFlags

from MuonRecUtils import logMuon,ConfiguredBase

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,getService,getServiceClone
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

#==============================================================

# call  setDefaults to update flags
muonRecFlags.setDefaults()

class MuPatCandidateTool(CfgMgr.Muon__MuPatCandidateTool,ConfiguredBase):
    __slots__ = ()

    def __init__(self,name='MuPatCandidateTool',**kwargs):
        self.applyUserDefaults(kwargs,name)
        if not MuonGeometryFlags.hasCSC():
            kwargs["CscRotCreator"] = ""
        super(MuPatCandidateTool,self).__init__(name,**kwargs)        


class MuPatHitTool(CfgMgr.Muon__MuPatHitTool,ConfiguredBase):
    __slots__ = ()
    
    def __init__(self,name="MuPatHitTool",**kwargs):
        self.applyUserDefaults(kwargs,name)
        if not muonRecFlags.doCSCs():
            # overwrite whatever is set
            kwargs["CscRotCreator"] = ""
        super(MuPatHitTool,self).__init__(name,**kwargs)
        getPublicTool("ResidualPullCalculator")


MuPatHitTool.setDefaultProperties(
    CscRotCreator = ("FixedErrorMuonClusterOnTrackCreator" if MuonGeometryFlags.hasCSC() else ""),
    MdtRotCreator = "MdtDriftCircleOnTrackCreatorPreFit" )
# end of class MuPatHitTool

