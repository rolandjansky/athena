
# This jobO should not be included more than once:

include.block( "D3PDMakerConfig/PrepareCommonD3PD.py")

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doV0Finder.set_Value_and_Lock(True)
InDetFlags.doSimpleV0Finder.set_Value_and_Lock(True)
from InDetRecExample.InDetKeys import InDetKeys

#Run correct TauDiscriminant
from TauDiscriminant.TauDiscriGetter import TauDiscriGetter
TauDiscriGetter(do_upstream_algs=True)


from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags


# for missing Et info
from MissingETD3PDMaker.MissingETD3PDMakerFlags  import * 
MissingETD3PDMakerFlags.doMissingETRegions=True 
MissingETD3PDMakerFlags.doCellOutEflow=True 
MissingETD3PDMakerFlags.doCellOutMiniJet=True 
    

# Enable saving the object metadata for D3PDReader code
# generation:
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.SaveObjectMetadata = True
D3PDMakerFlags.TruthWriteHadrons.set_Value_and_Lock(True) 
D3PDMakerFlags.MakeEgammaUserData.set_Value_and_Lock(True)
D3PDMakerFlags.FilterCollCand = False
D3PDMakerFlags.DoTrigger = True
D3PDMakerFlags.RerunTauID = True
D3PDMakerFlags.DoTJVA = True
D3PDMakerFlags.RerunTauRec = True

from RecExConfig.RecFlags  import rec
from AthenaCommon.AlgSequence import AlgSequence

topSequence = AlgSequence()

# for QCD Preprocessing
from QcdD3PDMaker.QcdD3PDMakerConf import QcdD3PD__QcdD3PDPreProcessing 
QcdD3PDPreProcessingAlg = QcdD3PD__QcdD3PDPreProcessing( name = "QcdD3PDPreProcessingAlg", 
                                                         OutputLevel = INFO ) 
topSequence += QcdD3PDPreProcessingAlg 


# if including track jets need to call in configuration setup file from share/
from TopInputsD3PDMaker.TopD3PDFlags import TopD3PDFlags


# #Make Additional MEt Containers
# include("D3PDMakerConfig/extraMissingEt.py")

#Configure TrackIsolation Tool
include("SUSYD3PDMaker/TrackIsolationTool_jobOptions.py")

if rec.doTruth():
    from egammaD3PDAnalysis.egammaUserDataConfig import egammaUserDataConfig
    from TopTruthSelection.TopTruthSelectionConf import TopTruthInputsSelector
    
    # Add TruthParticle containers for top decay products.
    truthTopInputs = TopTruthInputsSelector("truthTopInputs")
    truthTopInputs.InputContainerName = "SpclMC"
    truthTopInputs.TopVertexContainerName = "TruthTopVertexContainer"
    truthTopInputs.WVertexContainerName   = "TruthWVertexContainer"
    truthTopInputs.ZVertexContainerName   = "TruthZVertexContainer"
    truthTopInputs.TopDecayContainerName  = "TruthTopDecayContainer"
    truthTopInputs.WDecayContainerName    = "TruthWDecayContainer"
    truthTopInputs.ZDecayContainerName    = "TruthZDecayContainer"
    truthTopInputs.WTauContainerName      = "TruthWTauContainer"
    truthTopInputs.ZTauContainerName      = "TruthZTauContainer"
    topSequence += truthTopInputs

    


## from QcdD3PDMakerPreSetup.common.py
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from RecExConfig.RecAlgsFlags import recAlgs
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Logging import logging
from RecExConfig.ObjKeyStore import objKeyStore

#Suppressing output for warning (especially for jet tools)
svcMgr.MessageSvc.warningLimit=10

#no skimming
primDPD.ApplySkimming=False

#not make DESD for default
primDPD.MakePrimaryDPD=False

rec.oldFlagTopSteering.set_Value_and_Lock(True)

jetFlags.noStandardConfig=True  

    
# to apply the new JES in Aug 2012
from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
DBReplicaSvc = DBReplicaSvc(UseCOOLSQLite=False)
svcMgr += DBReplicaSvc
svcMgr.PoolSvc.SortReplicas=True
from JetRec.JetRecFlags import jetFlags
jetFlags.CalibrationDBSuffix.set_Value_and_Lock("-Aug-2012") # new calibration


if (CommonD3PDMakerFlags.doRecJet() or CommonD3PDMakerFlags.doRecTruthJet()) \
   and objKeyStore.isInInput("CaloClusterContainer","CaloCalTopoCluster"):

    include("D3PDMakerConfig/makeCommonD3PDJets.py")    
    #BTaggingFlags.Jets += BTaggingFlags.RetagJets
    include("BTagging/BTagging_jobOptions.py") # moved from CommonD3PD_prodJobOFragment.py

#JetTruthLabel
#from JetRec.JetMomentGetter import make_JetMomentGetter
#from JetMomentTools.SetupJetMomentTools import *
#jTLtool = getJetTruthLabelTool()
#make_JetMomentGetter( 'AntiKt4TopoEMNewJets' , [jTLtool] )
#make_JetMomentGetter( 'AntiKt6TopoEMNewJets' , [jTLtool] )
#make_JetMomentGetter( 'AntiKt4LCTopoNewJets' , [jTLtool] )
#make_JetMomentGetter( 'AntiKt6LCTopoNewJets' , [jTLtool] )

# Make Additional MEt Containers
include("D3PDMakerConfig/extraMissingEt.py")

# not needed anymore
#include ("PhysicsD3PDMaker/MyJetMake.py")

#jet re-calibration
if CommonD3PDMakerFlags.doRecalibJet():
    include("D3PDMakerConfig/JetReCalib.py")

#fix JVF
if CommonD3PDMakerFlags.doJVFFix():
    include("D3PDMakerConfig/CommonJVFFix.py")


