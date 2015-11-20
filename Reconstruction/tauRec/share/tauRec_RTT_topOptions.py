
DetDescrVersion="ATLAS-CSC-01-02-00"


doESD = True # if false, all algorithms are switched off by defaults 
###########################
donewTracking=True
doxKalman=True
doiPatRec=True
doEmCluster=True
doCaloCluster=True
doCaloTopoCluster=True

doMoore=False
doMuonboy=False
doConversion=False
doBtagging=False
doEgamma=True
doJetRec=True
doTauRec=True
doMuonIDStandAlone=False
doMuonIDCombined=False
doMuidLowPt=False
doMuGirl=False
doStaco=False
doMuTag=False
doTileMuID=False
doMissingET=False
doObjMissingET=False
doEFlow=False
doEFlowJet= False
doTrigger=False
doAtlfast=False
############################
doWriteESD = False
doWriteTAG = False
doWriteAOD = True
doTrigger = False
doHist = False
doAOD = True
doCBNT=True

# number of event to process
#EvtMax= 5 
EvtMax=-1

if not 'BTaggingFlags' in dir():
    from BTagging.BTaggingFlags import BTaggingFlags
    BTaggingFlags.JetFitterTag=False
    #BTaggingFlags.OutputLevel=INFO

#disable atlfast
from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.FastSimulation=False

include ("RecExCommon/RecExCommon_flags.py")
#DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

include ("RecExCommon/RecExCommon_topOptions.py")
#CBNT_TruthParticle.NMaxTruthParticles = 3000

