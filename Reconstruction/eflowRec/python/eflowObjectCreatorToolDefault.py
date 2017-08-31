# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.JobProperties import jobproperties
import traceback
# import the base class
from eflowRec.eflowRecConf import eflowObjectCreatorTool

class eflowObjectCreatorToolDefault(eflowObjectCreatorTool) :

    def __init__(self, name="eflowObjectCreatorToolDefault"): 

        eflowObjectCreatorTool.__init__( self, name )

        mlog = logging.getLogger( 'eflowObjectCreatorToolDefault::__init__ ' )
        mlog.info("entering")

        try:
            from AthenaCommon.AppMgr import ToolSvc
        except:
            mlog.error("could not import ToolSvc")
            print traceback.format_exc()
            return False
        
        try:
            from TrackVertexAssociationTool.TrackVertexAssociationToolConf import CP__TightTrackVertexAssociationTool        
            PFlowTrackVertexAssociationTool = CP__TightTrackVertexAssociationTool(name="PFlowTightCPTool", dzSinTheta_cut=2.0, doPV=True)
        except:
            mlog.error("could not import CP__TightTrackVertexAssociationTool")
            print traceback.format_exc()
            return False

        ToolSvc += PFlowTrackVertexAssociationTool
        
        self.TrackVertexAssociationTool = PFlowTrackVertexAssociationTool
        
        from eflowRec.eflowRecFlags import jobproperties
        gmFlag=""# default set to "golden1" for goldenMode. In the end this should come from this flag jobproperties.eflowRecFlags.goldenModeString()
        self.goldenModeString=gmFlag
        print "Configuring eflowObjectCreatorTool with goldenModeString: ",gmFlag

        if jobproperties.eflowRecFlags.useAODReductionClusterMomentList == True:
            self.UseAODReductionMomentList = True
