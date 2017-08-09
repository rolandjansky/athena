# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.JobProperties import jobproperties
import traceback
# import the base class
from eflowRec.eflowRecConf import eflowCellLevelSubtractionTool

from eflowRec.eflowRecFlags import jobproperties

class eflowCellLevelSubtractionToolDefault(eflowCellLevelSubtractionTool) :

    def __init__(self, name="eflowCellLevelSubtractionToolDefault"): 

        eflowCellLevelSubtractionTool.__init__( self, name )

        mlog = logging.getLogger( 'eflowCellLevelSubtractionToolDefault::__init__ ' )
        mlog.info("entering")

        self.SubtractionSigmaCut = 1.2
        self.ConsistencySigmaCut = 1.0

        from eflowRec.eflowRecFlags import jobproperties
        if jobproperties.eflowRecFlags.eflowAlgType == "EOverP":
            self.nMatchesInCellLevelSubtraction = -1
        else:
            self.nMatchesInCellLevelSubtraction = 1

        try:
            from eflowRec.eflowRecConf import PFTrackClusterMatchingTool
            MatchingTool = PFTrackClusterMatchingTool("CalObjBldMatchingTool")
            MatchingTool_Pull_02 = PFTrackClusterMatchingTool("MatchingTool_Pull_02")
            MatchingTool_Pull_015 = PFTrackClusterMatchingTool("MatchingTool_Pull_015")
        except:
            mlog.error("could not import eflowRec.PFTrackClusterMatchingTool")
            print traceback.format_exc()
            return False

        MatchingTool_Pull_015.TrackPositionType   = 'EM2EtaPhi' # str
        MatchingTool_Pull_015.ClusterPositionType = 'PlainEtaPhi' # str
        MatchingTool_Pull_015.DistanceType        = 'EtaPhiSquareDistance' # str
        MatchingTool_Pull_015.MatchCut = 0.15*0.15 # float
        self.PFTrackClusterMatchingTool_015 = MatchingTool_Pull_015
        
        MatchingTool_Pull_02.TrackPositionType   = 'EM2EtaPhi' # str
        MatchingTool_Pull_02.ClusterPositionType = 'PlainEtaPhi' # str
        MatchingTool_Pull_02.DistanceType        = 'EtaPhiSquareDistance' # str
        MatchingTool_Pull_02.MatchCut = 0.2*0.2 # float
        self.PFTrackClusterMatchingTool_02 = MatchingTool_Pull_02

        self.PFTrackClusterMatchingTool = MatchingTool

        if jobproperties.eflowRecFlags.eflowAlgType == "EOverP":
            MatchingTool.ClusterPositionType = 'PlainEtaPhi' # str
            MatchingTool.DistanceType        = 'EtaPhiSquareDistance'
            MatchingTool.MatchCut = 0.2*0.2 # float

        
        from eflowRec.eflowCellLevelSubtractionSetup import setup_eflowCellLevelSubtraction
        return setup_eflowCellLevelSubtraction(self, "EM", mlog)
    
        if jobproperties.eflowRecFlags.useUpdated2015ChargedShowerSubtraction == False:
            self.useUpdated2015ChargedShowerSubtraction = False
        

        gmFlag=""# default set to "golden1" for goldenMode. In the end this should come from this flag jobproperties.eflowRecFlags.goldenModeString()
        self.goldenModeString=gmFlag
        print "Configuring eflowCellLevelSubtractionTool with goldenModeString: ",gmFlag
        
