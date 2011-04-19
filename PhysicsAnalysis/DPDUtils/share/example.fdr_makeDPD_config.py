#####################################################################
#
# DPD Group Configuration example 
#
# Authors: Davide Costanzo, Nicolas Kerschen, Anastopoulos Christos
#====================================================================
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.Configurable import ConfigurablePyAlgorithm
#Jet Slimming
include( "DPDUtils/slimJets.py" )
slimJets=slimJets("slimJets")
topSequence += ConfigurablePyAlgorithm("slimJets")

#Track Slimming
include( "DPDUtils/slimTracks.py" )
slimTracks=slimTracks("slimTracks")
topSequence += ConfigurablePyAlgorithm("slimTracks")


#Electron Filter
#include( "DPDUtils/ElectronFilterAlgorithm.py" )
#filAlg=ElectronFilterAlgorithm("ElectronFilter")
#topSequence += ConfigurablePyAlgorithm("ElectronFilter")

include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
include( "CaloAthenaPool/CaloAthenaPool_joboptions.py" )
include( "TrkEventAthenaPool/TrkEventAthenaPool_joboptions.py" )
include( "RecAthenaPool/RecAthenaPool_joboptions.py" )
include( "ParticleEventAthenaPool/ParticleEventAthenaPool_joboptions.py" )
include( "G4SimAthenaPOOL/G4SimAthenaPOOL_joboptions.py" )
include ( "EventAthenaPool/EventAthenaPool_joboptions.py")
include ( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py")
include ("McParticleEventAthenaPool/McParticleEventAthenaPool_joboptions.py")
include( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" )
include ("TrackParticleTruthAthenaPool/TrackParticleTruthAthenaPool_joboptions.py")

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
StreamDPD = AthenaPoolOutputStream( "StreamDPD" )

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
#StreamDPD.AcceptAlgs=["ElectronFilter"]

#=============================================================
#
# End of job options file
#
###############################################################











