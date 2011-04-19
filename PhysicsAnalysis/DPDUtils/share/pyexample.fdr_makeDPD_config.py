#####################################################################
#
# DPD Group Configuration example 
#
# Authors: Davide Costanzo, Nicolas Kerschen, Anastopoulos Christos
#====================================================================
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# configure athena for POOL file reading
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = ['AOD.pool.root']

#Jet Slimming
from DPDUtils.DpdLib import JetSlimmer
topSequence += JetSlimmer("slimJets")

#Track Slimming
from DPDUtils.DpdLib import TrackSlimmer
topSequence += TrackSlimmer("slimTracks")


#Electron Filter
from DPDUtils.DpdLib import ElectronFilter
topSequence += ElectronFilter("ElectronFilter")

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
StreamDPD = AthenaPoolOutputStream( "StreamDPD" )

## configure the thinning service
from AthenaServices.Configurables import ThinningSvc
svcMgr += ThinningSvc(Streams=['StreamDPD'])
                      
######################Write Item List##########################################
StreamDPD.ItemList =  ['EventInfo#*', 'TrackRecordCollection#*']
StreamDPD.ItemList += ['ElectronContainer#ElectronAODCollection']
StreamDPD.ItemList += ['egDetailContainer#egDetailAOD']
StreamDPD.ItemList += ['Analysis::MuonContainer#StacoMuonCollection']
StreamDPD.ItemList += ['JetKeyDescriptor#JetKeyMap']
StreamDPD.ItemList += ['JetCollection#Cone4H1TowerJets']
StreamDPD.ItemList += ['JetCollection#Cone4TruthJets']
StreamDPD.ItemList += ['Rec::TrackParticleContainer#TrackParticleCandidate']
StreamDPD.ItemList += ['Rec::TrackParticleContainer#StacoTrackParticles']
StreamDPD.ItemList += ['VxContainer#*']

StreamDPD.ForceRead=TRUE;  #force read of output data objs

#electron Filter 
StreamDPD.AcceptAlgs=["ElectronFilter"]

#=============================================================
#
# End of job options file
#
###############################################################
