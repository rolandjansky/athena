#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs.

# This jobO should not be included more than once:
include.block( "HSG5DPDUtils/HSG5_gammaZbb_boosted_D3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName


# Set up a logger:
from AthenaCommon.Logging import logging
HSG5D3PD_msg = logging.getLogger( 'HSG5_gammaZbb_boosted_D3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WriteHSG5GAMZD3PD.isVirtual:
    HSG5D3PD_msg.error( "The HSG5 GAMZ D3PD stream can't be virtual! " +
                              "It's a configuration error!" )
    raise NameError( "HSG5 GAMZ D3PD set to be a virtual stream" )
    pass

# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

# Construct the stream and file names for the HSG5 D3PD:
streamName = prodFlags.WriteHSG5GAMZD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteHSG5GAMZD3PD )

HSG5D3PD_msg.info( "Configuring HSG5D3PD with streamName '%s' and fileName '%s'" % \
                   ( streamName, fileName ) )

HSG5D3PD_Stream = MSMgr.NewRootStream( streamName, fileName, "physics" )

# create stream
#from PhysicsD3PDMaker.SMWZD3PD import SMWZD3PD
#SMWZD3PD( HSG5D3PD_Stream )
if 'IS_SIMULATION' in inputFileSummary['evt_type']:
    from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
    D3PDMakerFlags.TruthWriteHadrons.set_Value_and_Lock(True)

# set flags
from HSG5DPDUtils.HSG5Flags import HSG5GAMZFlags
HSG5GAMZFlags.doPhotons = True

from HSG5DPDUtils.HSG5D3PD import HSG5D3PD
HSG5D3PD( HSG5D3PD_Stream,flags=HSG5GAMZFlags )

include("HSG5DPDUtils/FatJetsConfig.py")

from HSG5DPDUtils.JSD3PD import JSD3PD
for xx in myJetKeys:
   if xx[0] != None and xx[1] != None:
       JSD3PD(xx, HSG5D3PD_Stream )

# add filters
HSG5D3PD_Stream.AcceptAlgs=[]
HSG5D3PD_Stream.RequireAlgs=[]

include ("HSG5DPDUtils/gammaZbb_boostedFilter.py")

from EventCommonD3PDMaker.CutFlowMetadataConfig import CutFlowMetadataConfig
HSG5D3PD_Stream.MetadataTools += [CutFlowMetadataConfig(HSG5D3PD_Stream)]

ToolSvc.TrigDecisionTool.Navigation.OutputLevel=ERROR
