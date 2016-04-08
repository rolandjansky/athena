# $Id: TopBoostJSD3PD_prodJobOFragment.py 511453 2012-07-26 14:45:05Z ethompso $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "Qcd D3PD cart".

# This jobO should not be included more than once:
include.block( "BoostedTopD3PDMaker/TopBoostJSD3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
TopBoostJSD3PDStream_msg = logging.getLogger( 'TopBoostJSD3PD_prodJobOFragment' )

from D3PDMakerConfig.D3PDMakerFlags         import D3PDMakerFlags,_string_prop
from AthenaCommon.AlgSequence               import *


## not behaving well..savannah bug 91703
## truth bug
#if rec.doTruth():
#  from McParticleAlgs.ReOrderMcEventCollection import  FixMcEventCollection
#  topSequence.insert(1, FixMcEventCollection() )

preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())

#######
from BoostedTopD3PDMaker.BoostJSjets import createJSJets

from BoostedTopD3PDMaker.GroomedJetsConfig import getGroomedJetsConfig
dictsConfig = getGroomedJetsConfig()

myJetKeys = []

for dC in dictsConfig:
   xx = createJSJets(dC[0], dC[1], preseq)
   myJetKeys += [xx]

#########

# Configure algorithms needed before TopInputD3PDMaker is run
include("TopInputsD3PDMaker/PrepareTopInputs.py")


# Check if the configuration makes sense:
if prodFlags.WriteTopBoostJSD3PD.isVirtual:
    TopBoostJSD3PDStream_msg.error( "The TopBoostJSD3PD stream can't be virtual! " +
                              "It's a configuration error!" )
    raise NameError( "TopBoostJSD3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the JS D3PD:
streamName = prodFlags.WriteTopBoostJSD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteTopBoostJSD3PD )
TopBoostJSD3PDStream_msg.info( "Configuring TopBoostJSD3PD with streamName '%s' and fileName '%s'" % \
                         ( streamName, fileName ) )

# Create the D3PD stream itself:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
TopBoostJSD3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" )

# Now add all the content to this stream:

from BoostedTopD3PDMaker.MinimalistD3PD import MinimalistD3PD
MinimalistD3PD( TopBoostJSD3PDStream)

##overwrite JetEFTrigPattern here
#if not hasattr( D3PDMakerFlags, "JetEFTrigPattern" ):
#  _string_prop( "JetEFTrigPattern", "EF_[2-9]?f?j[0-9]*.*" )

#new miniiso code which uses standard isolation tools
miniisoinclude = ["miniiso10","miniiso15"]

from TopInputsD3PDMaker.topInputsD3PD import topInputsD3PD
topInputsD3PD( TopBoostJSD3PDStream ,EFJet_include=[],  Muon_include = ["EFCBInfoIndex", "EFMEInfoIndex", "L2CBInfoIndex", "L1InfoIndex"] + miniisoinclude,
                                                     Electron_include = miniisoinclude)
###
from BoostedTopD3PDMaker.BoostJSD3PD import JSD3PD

for xx in myJetKeys:
   if xx[0] != None and xx[1] != None:
     JSD3PD(xx, TopBoostJSD3PDStream )


## Silent TrigNavigation Warnings
if D3PDMakerFlags.DoTrigger():
  from AthenaCommon.AppMgr import ToolSvc
  if hasattr(ToolSvc, 'TrigDecisionTool'):
     ToolSvc.TrigDecisionTool.Navigation.OutputLevel=5 ## ERROR 
