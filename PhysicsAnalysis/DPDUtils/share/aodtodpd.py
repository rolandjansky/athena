# This is the job options that does AOD based skimming and thinning
###############################################################
#
# Job options file
#
# Authors: Davide Costanzo, Stathes Paganis, Sven Menke
# Anastopoulos Christos, Sebastien Binet
#==============================================================

import AthenaCommon.Constants as Lvl
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp

## load POOL support
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.AthenaSealSvc.OutputLevel = Lvl.ERROR

## general job configuration
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

###############################
# Load thinning service
###############################

from AthenaServices.Configurables import ThinningSvc
svcMgr += ThinningSvc()
# the list of output streams to thin
svcMgr.ThinningSvc.Streams += ['StreamDPD']

#Needed for lxplus ?
include("RecExCond/AllDet_detDescr.py")
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = -1


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel = Lvl.INFO
svcMgr.EventSelector.InputCollections = [ "AOD.pool.root" ]
#svcMgr.EventSelector.InputCollections =inFileName

from JetRec.JetRecFlags import jetFlags
jetFlags.inputFileType = "AOD" #

from JetRec.JetGetters import *  


Kt4alg = make_StandardJetGetter('Kt', 0.4, 'LCTopo').jetAlgorithmHandle()
Kt4alg.OutputLevel = 2

Kt4talg = make_StandardJetGetter('Kt', 0.4, 'Truth').jetAlgorithmHandle()
Kt4talg.OutputLevel = 2

from DPDUtils.DpdLib import ttbarFilter
topSequence += ttbarFilter()

topSequence.ttbarFilter.jetsName = 'Kt4LCTopoJets' # use new LC topo jets
#topSequence.ttbarFilter.maxDeltaRcx = 100.4       # effectively switch off
                                                   # cluster thinning

print topSequence
#==============================================================
#
# End of job options file
#
###############################################################
#--------------------------------------------------------------
#---   Secondary Write portion  ----- Don't change it !!!
#--------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
StreamDPD = AthenaPoolOutputStream("StreamDPD")
StreamDPD.ItemList =  ['EventInfo#*', 'TrackRecordCollection#*']
StreamDPD.ItemList += ['ElectronContainer#ElectronAODCollection']
StreamDPD.ItemList += ['egDetailContainer#egDetailAOD']
StreamDPD.ItemList += ['MissingET#MET_RefFinal']
StreamDPD.ItemList += ['MissingET#MET_LocHadObj']
StreamDPD.ItemList += ['CaloClusterContainer#CaloCalTopoCluster']
StreamDPD.ItemList += ['Analysis::MuonContainer#StacoMuonCollection']
StreamDPD.ItemList += ['JetCollection#Kt4LCTopoJets']
StreamDPD.ItemList += ['JetCollection#Kt4TruthJets']
#StreamDPD.ItemList += ['JetCollection#Cone4H1TowerJets']
StreamDPD.ItemList += ['JetKeyDescriptor#JetKeyMap']
StreamDPD.ItemList += ['Rec::TrackParticleContainer#TrackParticleCandidate']
StreamDPD.ItemList += ['Rec::TrackParticleContainer#StacoTrackParticles']
StreamDPD.ItemList += ['TruthParticleContainer#SpclMC']
StreamDPD.ItemList += ['McEventCollection#GEN_AOD']

StreamDPD.OutputFile = "SkimmedThin.AOD.pool.root"
#StreamDPD.OutputFile = outFileName
StreamDPD.ForceRead=TRUE;  #force read of output data objs
StreamDPD.AcceptAlgs=["ttbarFilter"]


ChronoStatSvc = Service( "ChronoStatSvc" )
ChronoStatSvc.ChronoDestinationCout = True

ChronoStatSvc.PrintUserTime     = True
ChronoStatSvc.PrintSystemTime   = True
ChronoStatSvc.PrintEllapsedTime = True
AthenaPoolCnvSvc = Service( "AthenaPoolCnvSvc" )
AthenaPoolCnvSvc.UseDetailChronoStat = True

#MessageSvc = Service("MessageSvc")
#MessageSvc.OutputLevel = DEBUG
#MessageSvc.OutputLevel = ERROR


