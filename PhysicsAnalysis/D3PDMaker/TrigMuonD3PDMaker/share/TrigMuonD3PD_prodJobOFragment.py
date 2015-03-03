# $Id: TrigMuonD3PD_prodJobOFragment.py 564014 2013-10-03 10:30:48Z mcorradi $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "TrigMuonD3PD"

# This jobO should not be included more than once:
include.block( "TrigMuonD3PDMaker/TrigMuonD3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
TrigMuonD3PDStream_msg = logging.getLogger( 'TrigMuonD3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WriteTrigMuonD3PD.isVirtual:
    TrigMuonD3PDStream_msg.error( "The TrigMuon D3PD stream can't be virtual! " +
                                  "It's a configuration error!" )
    raise NameError( "TrigMuon D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the SUSY D3PD:
streamName = prodFlags.WriteTrigMuonD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteTrigMuonD3PD )
TrigMuonD3PDStream_msg.info( "Configuring TrigMuonD3PD with streamName '%s' and fileName '%s'" % \
                             ( streamName, fileName ) )

# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
d3pdalg = MSMgr.NewRootStream( streamName, fileName, "trigger" )

#
# Set up event filtering for the D3PD stream:
#
from RecExConfig.RecFlags import rec
if ( ( not rec.doTruth() ) and
     ( not rec.triggerStream() == "Muons" ) and
     ( not rec.triggerStream() == "L1Muon" ) and
     ( not rec.triggerStream() == "EnhancedBias" ) and
     ( not rec.triggerStream() == "ZeroBias" ) ):
    #
    # Only execute the filtering for data streams that are not "physics_Muons" or
    # "physics_L1Muon".
    # M.Corradi : added also physics_EnhancedBias and physics_ZeroBias streams
    #
    TrigMuonD3PDStream_msg.info( "Configuring event filtering for the output D3PD" )
    TrigMuonD3PDStream_msg.debug( "triggerStream = " + rec.triggerStream() )
    from AthenaCommon.AlgSequence import AlgSequence
    algSeq = AlgSequence()
    #
    # Set up a filter that accepts all events with at least one "good" offline
    # muon:
    #
    # for Muons:
    __trigMuonD3PDMuonsFilterName = "TrigMuonD3PDMuonsFilter"
    if not hasattr( algSeq, __trigMuonD3PDMuonsFilterName ):
        import AthenaCommon.SystemOfUnits as Units
        from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
        algSeq += D2PDMuonSelector( __trigMuonD3PDMuonsFilterName,
                                    inputCollection          = 'Muons',
                                    minNumberPassed          = 1,
                                    ptMin                    = 3.0*Units.GeV,)
        pass
    # for Staco muon:
    __trigMuonD3PDStacoMuonFilterName = "TrigMuonD3PDStacoMuonFilter"
    if not hasattr( algSeq, __trigMuonD3PDStacoMuonFilterName ):
        import AthenaCommon.SystemOfUnits as Units
        from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
        algSeq += D2PDMuonSelector( __trigMuonD3PDStacoMuonFilterName,
                                    inputCollection          = 'StacoMuonCollection',
                                    minNumberPassed          = 1,
                                    ptMin                    = 3.0*Units.GeV,)
        pass
    # for Muid muon:
    __trigMuonD3PDMuidMuonFilterName = "TrigMuonD3PDMuidMuonFilter"
    if not hasattr( algSeq, __trigMuonD3PDMuidMuonFilterName ):
        import AthenaCommon.SystemOfUnits as Units
        from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
        algSeq += D2PDMuonSelector( __trigMuonD3PDMuidMuonFilterName,
                                    inputCollection          = 'MuidMuonCollection',
                                    minNumberPassed          = 1,
                                    ptMin                    = 3.0*Units.GeV,)
        pass

    #
    # Set up a filter that accepts all events passing a muon trigger:
    #
    __trigMuonD3PDTriggerFilterName = "TrigMuonD3PDTriggerFilter"
    if not hasattr( algSeq, __trigMuonD3PDTriggerFilterName ):
        from PrimaryDPDMaker.TriggerFilter import TriggerFilter
        algSeq += TriggerFilter( __trigMuonD3PDTriggerFilterName,
                                 trigger = "L1_[0-9]?MU[0-9]*.*|(L2|EF)_[0-9]?mu[0-9]*.*" )
        pass
    d3pdalg.AddAcceptAlgs( [ __trigMuonD3PDStacoMuonFilterName,
                             __trigMuonD3PDMuidMuonFilterName,
                             __trigMuonD3PDTriggerFilterName   ] )
else:
    TrigMuonD3PDStream_msg.info( "Not configuring event filtering for the output D3PD" )
    pass

# Set which HLT chains to care about:
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.MuonL1TrigPattern = "L1_MU0|L1_MU6|L1_MU10|L1_MU11|L1_MU15|L1_MU20"
D3PDMakerFlags.MuonL2TrigPattern = 'L2_[0-9]?mu[0-9]+(|T|i|i1|i2|i3|i4|_MG|_OR|_MSonly|_MSonly_barrel)(|_loose|_medium|_tight|_L1[0-9]?J[0-9]+)(_[0-9]?mu[0-9]+_EFFS|_MGonly|_TMEFonly|_MGfirst|_TMEFfirst|_both)?'
D3PDMakerFlags.MuonEFTrigPattern = 'EF_[0-9]?mu[0-9]+(|T|i|i1|i2|i3|i4|_MG|_OR|_MSonly|_MSonly_barrel)(|_loose|_medium|_tight|_L1[0-9]?J[0-9]+)(_[0-9]?mu[0-9]+_EFFS|_MGonly|_TMEFonly|_MGfirst|_TMEFfirst|_both)?'

# import cfgKeyStore to be used
from RecExConfig.ObjKeyStore import cfgKeyStore

# Save the full trigger muon information:
from TrigMuonD3PDMaker.TrigMuonD3PD import TrigMuonD3PDObjects
TrigMuonD3PDObjects( d3pdalg, level = 10 )

# Add some basic event-level information:
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
d3pdalg += EventInfoD3PDObject( 0 )

#Save all of bit information of trigger decision 
from TriggerD3PDMaker.TrigDecisionD3PDObject import TrigDecisionD3PDObject
d3pdalg += TrigDecisionD3PDObject( 10 )

# Add Primary Vertex Information
from TrackD3PDMaker.VertexD3PDObject import PrimaryVertexD3PDObject
d3pdalg += PrimaryVertexD3PDObject(0, sgkey = D3PDMakerFlags.VertexSGKey(), prefix = 'vxp_')

#Add Offline Muon Information
from MuonD3PDMaker.MuonD3PDObject import MuonD3PDObject
from MuonD3PDMaker.MuonSegmentD3PDObject import MuonSegmentD3PDObject
oflMuonIncludes = [ 'MuonHitSummary', 'MuonSpectrometerHitSummary', 'EFCBInfoIndex',
                    'EFMGInfoIndex', 'EFMEInfoIndex', 'L2CBInfoIndex', 'L1InfoIndex' ]
oflMuonExcludes = [ 'EFCBInfo', 'EFMGInfo', 'EFMEInfo', 'L2CBInfo', 'L1Info' ]
d3pdalg += MuonD3PDObject( 1, sgkey='Muons', prefix='mu_', include=oflMuonIncludes,
                           exclude=oflMuonExcludes ) 

if cfgKeyStore.isInInput( "Analysis::MuonContainer", "MuidMuonCollection" ):
    d3pdalg += MuonD3PDObject( 1, sgkey='MuidMuonCollection', prefix='mu_muid_', include=oflMuonIncludes,
                               exclude=oflMuonExcludes ) 
    pass

if cfgKeyStore.isInInput( "Analysis::MuonContainer", "StacoMuonCollection" ):
    d3pdalg += MuonD3PDObject( 1, sgkey='StacoMuonCollection', prefix='mu_staco_', include=oflMuonIncludes,
                               exclude=oflMuonExcludes )
    pass

if cfgKeyStore.isInInput( "Analysis::MuonContainer", "CaloMuonCollection" ):
    d3pdalg += MuonD3PDObject( 1, sgkey='CaloMuonCollection', prefix='mu_calo_', include=oflMuonIncludes,
                               exclude=oflMuonExcludes )
    pass

d3pdalg += MuonSegmentD3PDObject(1, sgkey='MooreSegments', prefix='mooreseg_',
                                 include = ['MooreSegmentT0'],
                                 exclude = ['MuonboySegmentT0'] )

d3pdalg += MuonSegmentD3PDObject(1, sgkey='ConvertedMBoySegments', prefix='mboyseg_',
                                 include = ['MuonboySegmentT0'],
                                 exclude = ['MooreSegmentT0'] )
#
# Add TileCal Information
# from CaloD3PDMaker.TileDetailsD3PDObject import *
# d3pdalg += TileDetailsD3PDObject(10) 
#

# We have to force the job to load the correct cabling
# services. These lines should make sure of that:
#
TrigMuonD3PDStream_msg.info( "Forcing the configuration of the RPC and TGC cabling services" )
from AthenaCommon.DetFlags import DetFlags
DetFlags.readRIOPool.RPC_setOn()
DetFlags.readRIOPool.TGC_setOn()
import MuonCnvExample.MuonCablingConfig

#
# For the information available only in ESD:
#
if rec.readESD():
    #Add TGC information
    from TrigMuonD3PDMaker.TgcD3PD import TgcD3PD
    TgcD3PD( d3pdalg, 10 )

    #Add RPC information
    from TrigMuonD3PDMaker.RpcCoinDataD3PDObject import RpcCoinDataD3PDObject
    d3pdalg += RpcCoinDataD3PDObject( 10 )
    from TrigMuonD3PDMaker.RpcPadD3PDObject import RpcPadD3PDObject
    d3pdalg += RpcPadD3PDObject( 10 )

    #Add MuCTPi information
    from TriggerD3PDMaker.MuCTPID3PDObject import MuCTPID3PDObject
    d3pdalg += MuCTPID3PDObject( 10 )

    #Add muon extrapolation info using TrigEffJpsiTools
    from AthenaCommon.AppMgr import ToolSvc
    #-------------------------------------------------------------
    # TrigEffJpsiTools
    #-------------------------------------------------------------
    from TrigEffJpsiTools.TrigEffJpsiToolsConf import TrigEffJpsiTools
    if not hasattr( ToolSvc, "TrigEffJpsiTools" ):
        ToolSvc += TrigEffJpsiTools("TrigEffJpsiTools")

    if cfgKeyStore.isInInput( "Analysis::MuonContainer", "MuidMuonCollection" ) \
      and cfgKeyStore.isInInput( "Analysis::MuonContainer", "StacoMuonCollection" ):
        from TrigMuonD3PDMaker.L1TgcTrkExtrapolation_D3PDObject import L1TgcStacoExtrapolation_D3PDObject
        d3pdalg += L1TgcStacoExtrapolation_D3PDObject(level=10, L1TgcStacoExtrapolation_OnlyToTriggerPlane = True )
        from TrigMuonD3PDMaker.L1TgcTrkExtrapolation_D3PDObject import L1TgcMuidExtrapolation_D3PDObject
        d3pdalg += L1TgcMuidExtrapolation_D3PDObject(level=10, L1TgcMuidExtrapolation_OnlyToTriggerPlane = True )
    else:
        from TrigMuonD3PDMaker.L1TgcTrkExtrapolation_D3PDObject import L1TgcMuonsExtrapolation_D3PDObject
        d3pdalg += L1TgcMuonsExtrapolation_D3PDObject(level=10, L1TgcMuonsExtrapolation_OnlyToTriggerPlane = True )

    pass


# Add the cut-flow metadata
from EventCommonD3PDMaker.CutFlowMetadataConfig import CutFlowMetadataConfig
d3pdalg.MetadataTools += [ CutFlowMetadataConfig( d3pdalg ) ]

# Add the luminosity metadata
from EventCommonD3PDMaker.LBMetadataConfig import LBMetadataConfig
d3pdalg.MetadataTools += [ LBMetadataConfig() ]

# Monte Carlo Stuff
if rec.doTruth():
    from TruthD3PDMaker.TruthParticleD3PDObject import TruthParticleD3PDObject
    d3pdalg += TruthParticleD3PDObject( 1 )
    from MuonD3PDMaker.TruthMuonD3PDObject import TruthMuonD3PDObject
    d3pdalg += TruthMuonD3PDObject( 2 )
    pass

# Silence the trigger navigation warnings:
from AthenaCommon.AppMgr import ToolSvc
if not hasattr( ToolSvc, "TrigDecisionTool" ):
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
    pass
ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR
