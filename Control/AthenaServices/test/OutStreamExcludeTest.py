# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )


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
theApp.EvtMax = 10


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = svcMgr.MessageSvc
MessageSvc.OutputLevel = Lvl.INFO
svcMgr.EventSelector.InputCollections = [ "rfio:/castor/cern.ch/grid/atlas/tzero/prod2/perm/fdr08_run2/physics_Egamma/0052301/fdr08_run2.0052301.physics_Egamma.recon.ESD.o3_f8/fdr08_run2.0052301.physics_Egamma.recon.ESD.o3_f8._lb0071._0001.1" ]
#svcMgr.EventSelector.InputCollections = [ "/afs/cern.ch/atlas/offline/ProdData/13.0.X/DC3.007218.singlepart_mu20_ATLAS-CSC-01-00-00_DEFAULT.esd.pool.root" ]
#svcMgr.EventSelector.InputCollections =inFileName

from JetRec.JetRecFlags import jetFlags
jetFlags.inputFileType = "AOD" #

from JetRec.JetGetters import *  


Kt4alg = make_StandardJetGetter('Kt', 0.4, 'LCTopo').jetAlgorithmHandle()
Kt4alg.OutputLevel = 2

Kt4talg = make_StandardJetGetter('Kt', 0.4, 'Truth').jetAlgorithmHandle()
Kt4talg.OutputLevel = 2

#from DPDUtils.DpdLib import ttbarFilter
#topSequence += ttbarFilter()

#topSequence.ttbarFilter.jetsName = 'Kt4LCTopoJets' # use new LC topo jets
#topSequence.ttbarFilter.maxDeltaRcx = 100.4       # effectively switch off
                                                   # cluster thinning

#print topSequence
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
StreamDPD.TakeItemsFromInput = True
#StreamDPD.ItemList += ['CaloClusterContainer#CaloCalTopoCluster']
#StreamDPD.ItemList += ['JetCollection#Cone4H1TowerJets']
#StreamDPD.ItemList += ['JetKeyDescriptor#JetKeyMap']
#StreamDPD.ItemList += ['Rec::TrackParticleContainer#TrackParticleCandidate']
#StreamDPD.ItemList += ['Rec::TrackParticleContainer#StacoTrackParticles']
#StreamDPD.ItemList += ['TruthParticleContainer#SpclMC']
#StreamDPD.ItemList += ['McEventCollection#GEN_AOD']
StreamDPD.ExcludeList =['CaloClusterContainer#CaloCalTopoCluster']
StreamDPD.ExcludeList+=['TrigRoiDescriptor#HLTAutoKey*']
StreamDPD.ExcludeList+=['98849495#ConvertedMBoySegments']
StreamDPD.ExcludeList+=['107385089#*']
StreamDPD.ExcludeList+=['Rec::TrackParticleContainer#TrackParticleCandidate']
StreamDPD.OutputLevel = DEBUG
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


