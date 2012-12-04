# read AOD
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
EventSelector = Service( "EventSelector" )

# The AOD input file
EventSelector.InputCollections = [ "AOD.pool.root" ]
#EventSelector.InputCollections = [ "reconstruction.root" ]

# POOL converters
include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
include( "TrkEventAthenaPool/TrkEventAthenaPool_joboptions.py" )
include( "RecAthenaPool/RecAthenaPool_joboptions.py" )
include( "ParticleEventAthenaPool/ParticleEventAthenaPool_joboptions.py" )

# DLLs
theApp.Dlls += [ "CBNT_Athena", "CBNT_Utils" ]
theApp.Dlls   += [ "AnalysisTest" ]

# Algs
theApp.TopAlg += [ "CBNT_Athena/CBNT_AOD" ]
CBNT_AOD = Algorithm( "CBNT_AOD" )
CBNT_AOD.members = [
    "ReadMuon",
    "ReadElectron",
    "ReadPhoton",
    "ReadBJet",
    "ReadParticleJet",
    "ReadTauJet",
    "ReadTruthParticle",
    "ReadTruthParticleJet"
#    "ReadTrackParticle"
#    "ReadVxContainer"
#    "Readegamma"
#    "ReadTau"    
    ]

# options for Algs
ReadMuon = Algorithm( "ReadMuon" )
ReadMuon.NtupleLocID   = "/FILE1/Muon/100"
ReadMuon.NtuplePrefix  = "Muon"
ReadMuon.ContainerName = "MuonCollection"

ReadElectron = Algorithm( "ReadElectron" )
ReadElectron.NtupleLocID   = "/FILE1/Electron/101"
ReadElectron.NtuplePrefix  = "Electron"
ReadElectron.ContainerName = "ElectronCollection"

ReadPhoton = Algorithm( "ReadPhoton" )
ReadPhoton.NtupleLocID   = "/FILE1/Photon/102"
ReadPhoton.NtuplePrefix  = "Photon"
ReadPhoton.ContainerName = "PhotonCollection"

ReadBJet = Algorithm( "ReadBJet" )
ReadBJet.NtupleLocID   = "/FILE1/BJet/103"
ReadBJet.NtuplePrefix  = "BJet"
ReadBJet.ContainerName = "BCandidates"

ReadParticleJet = Algorithm( "ReadParticleJet" )
ReadParticleJet.NtupleLocID   = "/FILE1/ParticleJet/104"
ReadParticleJet.NtuplePrefix  = "ParticleJet"
ReadParticleJet.ContainerName = "ParticleJetContainer"

ReadTauJet = Algorithm( "ReadTauJet" )
ReadTauJet.NtupleLocID   = "/FILE1/TauJet/105"
ReadTauJet.NtuplePrefix  = "TauJet"
ReadTauJet.ContainerName = "TauJetCollection"

ReadTruthParticle = Algorithm( "ReadTruthParticle" )
ReadTruthParticle.NtupleLocID   = "/FILE1/TruthParticle/106"
ReadTruthParticle.NtuplePrefix  = "TruthParticle"
ReadTruthParticle.ContainerName = "SpclMC"

ReadTruthParticleJet = Algorithm( "ReadTruthParticleJet" )
ReadTruthParticleJet.NtupleLocID   = "/FILE1/TruthParticleJet/107"
ReadTruthParticleJet.NtuplePrefix  = "TruthParticleJet"
ReadTruthParticleJet.ContainerName = "TruthParticleJetContainer"

ReadTrackParticle = Algorithm( "ReadTrackParticle" )
ReadTrackParticle.NtupleLocID   = "/FILE1/TrackParticle/108"
ReadTrackParticle.NtuplePrefix  = "TrackParticle"
ReadTrackParticle.ContainerName = "TrackParticleCandidate" #"MooreTrackParticles" 

ReadTau = Algorithm( "ReadTau" )
ReadTau.NtupleLocID   = "/FILE1/tauContainer/109"
ReadTau.NtuplePrefix  = "tauContainer"
ReadTau.ContainerName = "MYtauContainer"

ReadVxContainer = Algorithm( "ReadVxContainer" )
ReadVxContainer.NtupleLocID   = "/FILE1/Vx/110"
ReadVxContainer.NtuplePrefix  = "Vx"
ReadVxContainer.ContainerName = "VxPrimaryCandidate"

Readegamma = Algorithm( "Readegamma" )
Readegamma.NtupleLocID   = "/FILE1/egamma/111"
Readegamma.NtuplePrefix  = "egamma"
Readegamma.ContainerName = "egammaCollection"

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO

# message level for ReadXYZ
#ReadMuon.OutputLevel     = DEBUG
#ReadElectron.OutputLevel = DEBUG

# Number of Events to process
theApp.EvtMax = 1000000

# Root Ntuple output
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='AODTest.root' OPT='NEW'" ]
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile = "hist.root";

