#example of personal topOptions
#
# to use it  
# athena >! athena.log
#  ( myTopOptions.py is defaulted through jobOptions.py soft link)
# 
# see RecExCommon/share/RecExCommon_flags.py for more available flags
# and https://twiki.cern.ch/twiki/bin/view/Atlas/RecExCommonFlags
# for more complete documentation.
#
# AllAlgs, DetDescrVersion and other flags 
# needs be set before the include, since several
# secondary flags are configured according to that one
#


DetDescrVersion="ATLAS-CSC-01-02-00" # default
# DetDescrVersion="ATLAS-DC3-02" # 11.0.42 files 
AllAlgs = False # if false, all algorithms are switched off by defaults 

doFastCaloSim = True
doAtlfast = True
doCaloCluster = True
doEmCluster = True
doCaloTopoCluster = True
doEgamma = True
doJetRec = True
#doTauRec = True

doCBNT = True
CBNTAthenaAware = False
doAODall = False

readESD = True
#OutputLevel = INFO

#doDumpTES = True
#doDumpPoolInputContent = True
#doDumpMC = True
#doDumpProperties = True
#doDumpTDS = True

if "PoolESDInput" not in dir() :
    PoolESDInput=["ZEBRA.P"]

if "EvtMax" not in dir() :
    EvtMax=100

# doTrigger = False # for example do not run trigger simulation
# doTruth=False
# number of event to process
# EvtMax=50


# include my own algorithm(s)
# UserAlgs=[ "MyPackage/MyAlgorithm_jobOptions.py" ] 


# By default write ESD, AOD and TAG simultenaously, but note that in production
#      these should be done in separate steps (results might not be bit-by-bit identical).
doWriteESD=False # uncomment if do not write ESD
doWriteAOD=False # uncomment if do not write AOD
doAOD=False # uncomment if do not run AOD making algorithms
doWriteTAG=False # uncomment if do not write TAG


# if needed to configure AOD building
# see https://twiki.cern.ch/twiki/bin/view/Atlas/UserAnalysisTest#The_AOD_Production_Flags
# from ParticleBuilderOptions.AODFlags import AODFlags


# DetFlags modifications are best set here (uncomment RecExCommon_flags first)
# include ("RecExCommon/RecExCommon_flags.py")
# switch off ID, calo, or muons
# DetFlags.ID_setOff()
# DetFlags.Calo_setOff()
# DetFlags.Muon_setOff()

# main jobOption
include ("FastCaloSim/FSRecExCommon_topOptions.py")
GlobalEventDataMaker.McLocation       = "GEN_EVENT"
AtlfastAodBuilder.TrackParticleCnvTool.TruthParticles    = "GEN_EVENT"
#AtlfastAodBuilder.TrackParticleCnvTool.AodOutput         = "TrackParticleCandidate"
AtlfastAodBuilder.MuonCnvTool.AodOutput         =  "AtlfastMuonCollection"
AtlfastAodBuilder.MuonCnvTool.AtlfastCollection =  "/Event/AtlfastMuons"
AtlfastAodBuilder.MuonCnvTool.TruthParticles    =  "GEN_EVENT"

# user property modifiers should be at the end to overridde all defaults
# InDetPrepRawDataTruthMaker=Algorithm("InDetPrepRawDataTruthMaker")
# InDetPrepRawDataTruthMaker.Enable=False

CaloCellMakerFS.OutputLevel = DEBUG
#ToolSvc.FastShowerCellBuilderTool.OutputLevel=DEBUG
