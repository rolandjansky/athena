#########################################################
## This jobOptions demonstrates how to run reconstruction of
##  TCC and UFO objects.
##
## See also older (and probably deprecated) example for TCC only in TrackCaloClusterReco_Standalone.py and TrackCaloClusterReco_TrackClusterInfo.py
##########################################################


# ***************************
# Set-up input and athena geometry
import AthenaPoolCnvSvc.ReadAthenaPool

from AthenaCommon import CfgMgr

InputFiles = ["AOD.13485488._000001.pool.root.1"]

svcMgr.EventSelector.InputCollections = InputFiles

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections

# not sure the lines below are strictlyy necessary (?)
from AthenaCommon.GlobalFlags import globalflags
DetDescrVersion = 'ATLAS-R2-2016-01-00-01' #ATLAS-R2-2015-03-01-00
globalflags.DetDescrVersion = DetDescrVersion

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-MC15c-SDR-09')

print "globalflags.DetDescrVersion = ", globalflags.DetDescrVersion
# Set up geometry and BField
include("RecExCond/AllDet_detDescr.py")



# ***************************
# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()



# *********************************************
# *********************************************
# Setup TCC and UFO

# Set the source of cluster and tracks :
caloClusterName="CaloCalTopoClusters"
trackParticleName="InDetTrackParticles"

from TrackCaloClusterRecTools.TrackCaloClusterConfig import runTCCReconstruction, setupTrackCaloAssoc, runUFOReconstruction

# Schedule TCC in the topSequence
tccAlg = runTCCReconstruction(topSequence,ToolSvc, caloClusterName, trackParticleName)


# scheduling UFO is a bit more complex.
# We firts need to prepar a unified PFlow collection.
# We choose a LC scale pflow, with origin of neutrals corrected to the PV0

# we need to import the jet config helpers :
from JetRec.JetRecStandardToolManager import jtm
from JetRecTools.JetRecToolsConfig import ctm
from JetRecTools.JetRecToolsConf import  JetConstituentModSequence, SoftKillerWeightTool, ConstituentSubtractorTool, ChargedHadronSubtractionTool, CorrectPFOTool
  
ctm.add(CorrectPFOTool("correctpfoLC",   WeightPFOTool = jtm.pflowweighter, InputIsEM=True, CalibratePFO=True, CorrectNeutral=True,InputType = 3), alias='correctpfoLC')
ctm.add(ChargedHadronSubtractionTool("pfochs", InputType = 3), alias='pfochs')

import ROOT
from ROOT import xAOD
xAOD.Type.ObjectType


# here we prepare a tool building our unified PFlow container. The prefix of the output collection is 'PlainLC'
pflowLCSeq = ctm.buildConstitModifSequence( 'ConstitLCSeq',
                                              OutputContainer = 'PlainLC',
                                              InputContainer= 'JetETMiss',
                                              InputType = 3,
                                              modList = [ 'correctpfoLC', 'pfochs'] ,
)


# now schedule this tool as par of a JetAlgorithm (this doesn't run a jet alg, just the above pflowLCSeq tool)
from JetRec.JetRecConf import JetAlgorithm
clustSeqAlg = JetAlgorithm("ClusterModifiers", Tools = [pflowLCSeq])
topSequence += clustSeqAlg

# Finally we can run the UFO building taking our unified PFlow container as input
lcufoAlg = runUFOReconstruction(topSequence,ToolSvc, PFOPrefix="PlainLC")




# *********************************************
# *********************************************
# We create a DAOD output using the DerivationFramework
# We just put the container we created

from DerivationFrameworkCore.DerivationFrameworkMaster import *
streamName = 'StreamDAOD_TESTTCC'
fileName = "TestTCCDAOD.root"
TESTTCCStream = MSMgr.NewPoolRootStream( streamName, fileName )

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TESTTCCKernel")
TESTTCCStream.AcceptAlgs(["TESTTCCKernel"])

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
TESTTCCSlimmingHelper = SlimmingHelper("TESTTCCSlimmingHelper")




TESTTCCSlimmingHelper.AppendToDictionary["EventInfo"] = 'xAOD::EventInfo'
TESTTCCSlimmingHelper.AppendToDictionary['EventInfoAux'] = 'xAOD::EventAuxInfo'
TESTTCCSlimmingHelper.ExtraVariables +=[ 'EventInfo.mcChannelNumber.mcEventNumber.streamTagRobs.streamTagDets.mcEventWeights.averageInteractionsPerCrossing.actualInteractionsPerCrossing.eventNumber.runNumber.nPV' ]

cont = 'CaloCalTopoClusters'
TESTTCCSlimmingHelper.AppendToDictionary[cont] = 'xAOD::CaloClusterContainer'
TESTTCCSlimmingHelper.AppendToDictionary[cont+'Aux'] = 'xAOD::CaloClusterContainer'
TESTTCCSlimmingHelper.ExtraVariables +=[ cont+'.calE.calEta.calPhi.rawE' ]


# we can not output directly our PlainLC pflow container. Technically this container is a 'view' container
# and I'm not sure the DerivationFramework is able to deal with them. A solution is to add a custom alg which
# fully copies this view container into a new one, which we can then write out as below.
# cont = 'PlainLCPFO'
# TESTTCCSlimmingHelper.AppendToDictionary[cont] = 'xAOD::PFOContainer'
# TESTTCCSlimmingHelper.AppendToDictionary[cont+'Aux'] = 'xAOD::PFOAuxContainer'
# TESTTCCSlimmingHelper.ExtraVariables +=[ cont+'.pt.eta.phi.charged' ]

cont = 'TrackCaloClusters'
TESTTCCSlimmingHelper.AppendToDictionary[cont] = 'xAOD::TrackCaloClusterContainer'
TESTTCCSlimmingHelper.AppendToDictionary[cont+'Aux'] = 'xAOD::PFOAuxContainer'
TESTTCCSlimmingHelper.ExtraVariables +=[ cont+'.pt.eta.phi.taste' ]

cont = 'PlainLCUFO'
TESTTCCSlimmingHelper.AppendToDictionary[cont] = 'xAOD::TrackCaloClusterContainer'
TESTTCCSlimmingHelper.AppendToDictionary[cont+'Aux'] = 'xAOD::TrackCaloClusterAuxContainer'
TESTTCCSlimmingHelper.ExtraVariables +=[ cont+'.pt.eta.phi.taste' ]





TESTTCCSlimmingHelper.SmartCollections = [
]

TESTTCCSlimmingHelper.AppendContentToStream(TESTTCCStream)



# only a fraction of event 
theApp.EvtMax = 10 # -1 for all events
