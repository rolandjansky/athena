# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Include import include
#from AthenaCommon.GlobalFlags import *
from AthenaCommon.JobProperties import jobproperties
import traceback
import sys
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

        ## DigiTruth tests
        doDigiTruthFlag = False
        try:
            from Digitization.DigitizationFlags import digitizationFlags
            doDigiTruthFlag = digitizationFlags.doDigiTruth()
        except:
            log = logging.getLogger('CaloClusterTopoGetter')
            log.info('Unable to import DigitizationFlags in CaloClusterTopoGetter. Expected in AthenaP1')

        if doDigiTruthFlag:
            self.DoDigiHSTruth = True

