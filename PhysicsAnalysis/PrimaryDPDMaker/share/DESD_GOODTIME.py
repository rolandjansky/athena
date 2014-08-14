##-----------------------------------------------------------------------------
## Name:  DESD_CollCand.py
##
## Author:   Karsten Koeneke <karsten.koeneke_at_cern.ch>
## Created:  August 2009
##
## Description:  DESD for events with good timing.
##
## $Id$
##-----------------------------------------------------------------------------

# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/DESD_GOODTIME.py")


## for messaging
from AthenaCommon.Logging import logging

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from RecExConfig.RecFlags import rec

## This handles multiple output streams
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr





# ============================================================================
# Create a sub-sequence where all needed algorithms will run in
# ============================================================================
import AthenaCommon.CfgMgr as CfgMgr
desdGoodTimeSequence = CfgMgr.AthSequencer( 'DESD_COLLCAND_Sequence',
                                            StopOverride = True )
topSequence += desdGoodTimeSequence




##============================================================================
## Define the skimming (event selection) for the DESD_COLLCAND output stream
##============================================================================
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
# Get the needed CaloTimeFilterTool
include("LArCellRec/LArCollisionTime_jobOptions.py")
from PrimaryDPDMaker.PrimaryDPDMakerConf import CaloTimeFilterTool
ToolSvc += CaloTimeFilterTool() 
CaloTimeFilterTool.timeDiffCut     = primDPD.WriteDESD_CollCandStream.TimeDiffCut
CaloTimeFilterTool.MinCellsPerSide = primDPD.WriteDESD_CollCandStream.MinCellsPerSide

# Get the needed MBTSTimeFilterTool
from PrimaryDPDMaker.PrimaryDPDMakerConf import MBTSTimeFilterTool
ToolSvc += MBTSTimeFilterTool()      
MBTSTimeFilterTool.ChargeThreshold   = primDPD.WriteDESD_CollCandStream.ChargeThreshold
MBTSTimeFilterTool.MinHitsPerSide    = primDPD.WriteDESD_CollCandStream.MinHitsPerSide
MBTSTimeFilterTool.MaxTimeDifference = primDPD.WriteDESD_CollCandStream.MaxTimeDifference

# Create the collision filter
from PrimaryDPDMaker.PrimaryDPDMakerConf import CollisionFilterAlg
desdGoodTimeSequence += CollisionFilterAlg("CaloTimeFilterInCollisionStream")
desdGoodTimeSequence.CaloTimeFilterInCollisionStream.DoCaloTimeFilter = True
desdGoodTimeSequence.CaloTimeFilterInCollisionStream.DoMBTSTimeFilter = False

# Create the collision filter
from PrimaryDPDMaker.PrimaryDPDMakerConf import CollisionFilterAlg
desdGoodTimeSequence += CollisionFilterAlg("MBTSTimeFilterInCollisionStream")
desdGoodTimeSequence.MBTSTimeFilterInCollisionStream.DoCaloTimeFilter = False
desdGoodTimeSequence.MBTSTimeFilterInCollisionStream.DoMBTSTimeFilter = True

# Create the trigger filter
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")
from PrimaryDPDMaker.TriggerFilter import TriggerFilter
desdGoodTimeSequence += TriggerFilter( "TriggerFilterInCollissionStream",
                              trigger = primDPD.WriteDESD_CollCandStream.Trigger
                              )

# Create the combined decission
desdGoodTimeSequence += LogicalFilterCombiner("StreamDESD_COLLCAND_AcceptEvent")
desdGoodTimeSequence.StreamDESD_COLLCAND_AcceptEvent.cmdstring = "CaloTimeFilterInCollisionStream or MBTSTimeFilterInCollisionStream or TriggerFilterInCollissionStream"
from PrimaryDPDMaker.PrimaryDPDFlags import desdEventSkimmingFilterNamesList
desdEventSkimmingFilterNamesList.append( "StreamDESD_COLLCAND_AcceptEvent" )




##====================================================================
## Define this output stream
##====================================================================
streamName = primDPD.WriteDESD_CollCandStream.StreamName
fileName   = buildFileName( primDPD.WriteDESD_CollCandStream )
if primDPD.WriteDESD_CollCandStream.isVirtual or primDPD.isVirtual() :
    DESD_CollCandStream = MSMgr.NewVirtualStream( streamName, fileName )
    pass    
else :
    DESD_CollCandStream = MSMgr.NewPoolStream( streamName, fileName )
    pass

    
if primDPD.ApplySkimming() :
    DESD_CollCandStream.AcceptAlgs( [ "StreamDESD_COLLCAND_AcceptEvent" ] )
    pass


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
# Take all items from the input, except for the ones listed in the excludeList
excludeList = []
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )

