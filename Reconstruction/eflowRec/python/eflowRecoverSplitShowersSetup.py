# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging  # loads logger                           
import traceback # to allow printout of trace back
from RecExConfig.Configured import Configured # import base class 

def setup_eflowRecoverSplitShowers(Configured, nameModifier,mlog):

    if nameModifier != "EM" and nameModifier != "LC":
        mlog.error("Invalid calorimeter scale was specified : should be LC or EM, but was "+nameModifier)
        return False

    if "EM" == nameModifier:

        try:
            from eflowRec.eflowRecConf import eflowCellEOverPTool_mc12_JetETMiss
            CellEOverPTool=eflowCellEOverPTool_mc12_JetETMiss()
        except:
            mlog.error("could not import eflowRec.eflowCellEOverPTool_mc12")
            print traceback.format_exc()
            return False
        
        Configured.eflowCellEOverPTool=CellEOverPTool

    elif "LC" == nameModifier:
        try:
            from eflowRec.eflowRecConf import eflowCellEOverPTool_mc12_LC
            CellEOverPTool=eflowCellEOverPTool_mc12_LC()
        except:
            mlog.error("could not import eflowRec.eflowCellEOverPTool_mc12_LC")
            print traceback.format_exc()
            return False

        Configured.eflowCellEOverPTool=CellEOverPTool
    
 
    from eflowRec.eflowRecFlags import jobproperties
    
    if jobproperties.eflowRecFlags.recoverIsolatedTracks == True:
        Configured.RecoverIsolatedTracks = True

    if jobproperties.eflowRecFlags.useUpdated2015ChargedShowerSubtraction == False:
        Configured.useUpdated2015ChargedShowerSubtraction = False
        
    try:
        from eflowRec.eflowRecConf import PFTrackClusterMatchingTool
        MatchingTool = PFTrackClusterMatchingTool()
    except:
        mlog.error("could not import eflowRec.PFTrackClusterMatchingTool")
        print traceback.format_exc()
        return False
    MatchingTool.TrackPositionType   = 'EM2EtaPhi' # str
    MatchingTool.ClusterPositionType = 'PlainEtaPhi' # str
    MatchingTool.DistanceType        = 'EtaPhiSquareDistance' # str
    MatchingTool.MatchCut = 0.2*0.2 # float
    Configured.PFTrackClusterMatchingTool = MatchingTool

    return True
