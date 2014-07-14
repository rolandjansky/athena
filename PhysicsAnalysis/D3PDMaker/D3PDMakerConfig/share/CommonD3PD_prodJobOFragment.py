# $Id: CommonD3PD_prodJobOFragment.py 540332 2013-03-08 19:19:14Z lezhou $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "Common D3PD cart".

# This jobO should not be included more than once:
include.block( "D3PDMakerConfig/CommonD3PD_prodJobOFragment.py" )

# Common import(s):
from D3PDMakerConfig import D3PDProdFlags
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

from D3PDMakerConfig.CommonD3PDMakerFlags  import *
CommonD3PDMakerFlagsSetDefaults()

# this flag here does not work, if you want to switch off TruthWriteHadrons, do truthParticleConfig here.
# the situation here is complicated...
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags,_string_prop
D3PDMakerFlags.TruthWriteHadrons.set_Value_and_Lock(False)

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
# do truthParticleConfig here will drop all the truth hadrons
#from TruthD3PDAnalysis.truthParticleConfig import truthParticleConfig
#truthParticleConfig (seq = topSequence, prefix = 'mc_')

from RecExConfig.RecFlags import rec
if rec.doTruth():
       from TauD3PDMaker.TauD3PDMakerConf import TruthTausToSG
       topSequence += TruthTausToSG('truthtaus')
       topSequence.truthtaus.truth_container = 'SpclMC'
       if rec.readESD:
              topSequence.truthtaus.truth_container = 'INav4MomTruthEvent'

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.TruthWriteHadrons = True


############ begin for b-tagging ###########
from JetRec.JetRecFlags import jetFlags
jetFlags.doBTagging = True

# switch on branches for Higgs group
from JetTagD3PDMaker.JetTagD3PDMakerFlags    import JetTagD3PDFlags
from JetTagD3PDMaker.JetTagD3PDMakerKeys                import JetTagD3PDKeys
JetTagD3PDFlags.AddTwoTrackVertexInJetsObject = True
JetTagD3PDFlags.JFTwoTrackVertexAssocLabel.set_Value_and_Lock(JetTagD3PDKeys.JFTwoTrackVertexGetterLabel())
JetTagD3PDFlags.JetFitterVertexFinderInfo.set_Value_and_Lock(True)

from BTagging.BTaggingFlags import BTaggingFlags
######### setup local calib db ###########
#BTaggingFlags.CalibrationFromLocalReplica = True
#BTaggingFlags.CalibrationFolderRoot = '/GLOBAL/BTagCalib/'
#BTaggingFlags.CalibrationTag = 'BTagCalibALL-07-06'
#########################################
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
BTaggingFlags.Enable("SoftMuChi2")
BTaggingFlags.Runmodus      = "analysis"
BTaggingFlags.PoolInputType = "AOD"
BTaggingFlags.Active=True
BTaggingFlags.MV3_bVSu=False
BTaggingFlags.MV3_bVSc=False
BTaggingFlags.MV3_cVSu=False
BTaggingFlags.writeSecondaryVertices=True
BTaggingFlags.MVb=True


BTaggingFlags.Jets = ["AntiKt4TopoEMNew", 
                      "AntiKt4LCTopoNew", "AntiKt5LCTopoNew",
                      "AntiKt4Track","AntiKt3Track","AntiKt2Track","Kt3Track",
                      "AntiKt4LCTopo"]


if rec.doTruth() and CommonD3PDMakerFlags.doRecTruthJet():
       BTaggingFlags.Jets += ["AntiKt4TruthNew"] 

#if CommonD3PDMakerFlags.doRecJet() and CommonD3PDMakerFlags.doLCTopoJet():
#       BTaggingFlags.Jets += ["AntiKt5LCTopoNew","AntiKt7LCTopoNew","AntiKt2LCTopoNew","AntiKt3LCTopoNew"]

from D3PDMakerConfig.CommonJSjets import  getGroomingTools
from D3PDMakerConfig.CommonGroomedJetsConfig import getGroomedJetsConfig
dictsConfig = getGroomedJetsConfig()
sj_n = []
for dC in dictsConfig:
       myjetfinder = dC[0]["JetFinder"]
       myjetdr = dC[0]["JetdR"]
       myjetinput = dC[0]["JetInput"]
       if "Track" in myjetinput or "Truth" in myjetinput :
             continue
       myGroomingTools, catNames = getGroomingTools(dC[1])
       sj_n += [(myjetfinder+"%d"%(myjetdr*10) + myjetinput+x) for x in catNames if "Subjets" in x]

groomedJetKeys = []

# don't btag this collection
#if "CamKt12LCTopoTrimmedSubjetsPtFrac5SmallR30" in sj_n :
#       sj_n.remove("CamKt12LCTopoTrimmedSubjetsPtFrac5SmallR30")

BTaggingFlags.Jets += sj_n
print "cccccc sj_n ", sj_n

BTaggingFlags.JetsWithInfoPlus = BTaggingFlags.Jets[:]
BTaggingFlags.CalibrationChannelAliases += [ 'AntiKt4TopoEMNew->AntiKt4TopoEM',
                                             'AntiKt4LCTopoNew->AntiKt4LCTopo',
                                             'AntiKt5LCTopoNew->AntiKt4LCTopo',
                                             'AntiKt4TruthNew->AntiKt4TopoEM',
                                             'AntiKt4Track->AntiKt4TopoEM',
                                             'AntiKt3Track->AntiKt4TopoEM',
                                             'AntiKt2Track->AntiKt4TopoEM',
                                             'Kt3Track->AntiKt4TopoEM']
for sj in sj_n:
       BTaggingFlags.CalibrationChannelAliases += [sj+"->AntiKt4LCTopo" ]

BTaggingFlags.JetFitterCharm=True
JetTagD3PDFlags.Taggers+=["JetFitterCharm", "MVb", "MV1c", "MV2c00", "MV2c10", "MV2c20"] #, "Gaia", "IP3DNeg", "SV1Flip", "JetFitterCOMBNNIP3DNeg", "MV1Flip"]
JetTagD3PDFlags.JetFitterCharmTagInfo=True

BTaggingFlags.MV1c = True
BTaggingFlags.MV2c00 = True
BTaggingFlags.MV2c10 = True
BTaggingFlags.MV2c20 = True
BTaggingFlags.Gaia = True
# BTaggingFlags.GaiaNeg = True

#BTaggingFlags.IP3DNeg = True
#BTaggingFlags.SV1Flip = True
#BTaggingFlags.JetFitterCOMBNNIP3DNeg = True
#BTaggingFlags.MV1Flip = True

include("BTagging/BTagging_LoadTools.py")
# this has overwritten RetagJets, reset the flag
BTaggingFlags.RetagJets = ["AntiKt4TopoEMNew", 
                           "AntiKt4LCTopoNew", "AntiKt5LCTopoNew",
                           "AntiKt4Track", "AntiKt3Track","AntiKt2Track","Kt3Track"]

#include("BTagging/BTagging_jobOptions.py") # delay scheduling of btagging *after* jets are re-run in PrepareCommonD3PD.py
############ end for b-tagging ###########


# turn off since it will affect track parameters at Primary Vertex
#from MuonD3PDMaker.MuonD3PDMakerFlags import MuonD3PDMakerFlags
#MuonD3PDMakerFlags.doSingleMuons = True

# Set up a logger:
from AthenaCommon.Logging import logging
CommonD3PDStream_msg = logging.getLogger( 'CommonD3PD_prodJobOFragment' )

#from D3PDMakerConfig.D3PDMakerFlags         import D3PDMakerFlags,_string_prop
from AthenaCommon.AlgSequence               import AlgSequence

preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())


#include("BTagging/BTagging_LoadTools.py")
#include("BTagging/BTagging_jobOptions.py")

include("D3PDMakerConfig/PrepareCommonD3PD.py")


# Diphoton algorithm
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import PAUflags
PAUflags.PhotonContainer = D3PDMakerFlags.PhotonSGKey()
from AthenaCommon.AlgSequence import AlgSequence
PAUflags.AlgSequence = AlgSequence().getName()
from PhotonAnalysisUtils.DiphotonAnalysisGetter import DiphotonAnalysisGetter
dip = DiphotonAnalysisGetter()


# Check if the configuration makes sense:
if prodFlags.WriteCommonD3PD.isVirtual:
    CommonD3PDStream_msg.error( "The Common D3PD stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "Common D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the SUSY D3PD:
streamName = prodFlags.WriteCommonD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteCommonD3PD )
CommonD3PDStream_msg.info( "Configuring CommonD3PD with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )

from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doMissingET.set_Value_and_Lock(True)

# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
CommonD3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" )
CommonTriggerD3PDStream = MSMgr.NewRootStream( streamName + ":" + streamName +
                                                "TrigDec", fileName,
                                                "physicsTrigDec" )

# Configure branches
from AthenaCommon.AppMgr import ServiceMgr
from D3PDMakerRoot.D3PDMakerRootConf import D3PD__RootD3PDSvc
ServiceMgr += D3PD__RootD3PDSvc( "COMMON_D3PDSvc")
include("D3PDMakerConfig/CommonD3PD_VetoBranches.py")
ServiceMgr.COMMON_D3PDSvc.VetoedNames += CommonD3PD_VetoBranches
CommonD3PDStream.D3PDSvc = ServiceMgr.COMMON_D3PDSvc


# Now add all the content to this stream:
from D3PDMakerConfig.commonD3PD import commonD3PD
commonD3PD_args = globals().get('commonD3PD_args', {})
commonD3PD( CommonD3PDStream, CommonTriggerD3PDStream, **commonD3PD_args )


from D3PDMakerConfig.CommonJSD3PD import CommonJSD3PD
for xx in groomedJetKeys:
       print "cccccc xx = ", xx
       if xx[0] != None and xx[1] != None:
              CommonJSD3PD(xx, CommonD3PDStream )
