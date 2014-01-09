# EmbeddedTrackGenerator generates particles "embedded" in reconstructed jets.
# The jets are taken from an AOD file.
#
# One can use something like 
#
#    athena -c "EvtMax=5; OutputFile='test.pool.root'" EmbeddedTrackGenerator/jobOptions_embeddedTrackGeneration.py
#
# to set a few things that usually change often.
#
# Author: Andrei Gaponenko

from AthenaCommon.SystemOfUnits import MeV,GeV

from EmbeddedTrackGenerator.EmbeddedTrackGeneratorConf import *
embeddedGenerator = EmbeddedTrackGenerator()
#embeddedGenerator.OutputLevel = VERBOSE
#embeddedGenerator.RandomSeed = 9687
#embeddedGenerator.pdg_id = 13
#vertexedGenerator.generateAntiParticles = True
#embeddedGenerator.ptot = 10.*GeV
#embeddedGenerator.dR_max = 0.5


if 'InputCollections' not in dir():
    InputCollections = [ "rfio:/castor/cern.ch/grid/atlas/tier0/atlasmcdisk/valid1/AOD/valid1.005011.J2_pythia_jetjet.recon.AOD.e322_s412_r421_tid022153/AOD.022153._00001.pool.root.1" ]

# EvtMax+SkipEvents should not exceed the number of events in InputCollections.
if 'EvtMax' not in dir():
    EvtMax = 15

# The number of events from the data input file to skip
if 'SkipEvents' not in dir():
    SkipEvents = 0

# The number to assign to the first generated event
if 'FirstEvent' not in dir():
    FirstEvent = 1000

if 'RunNumber' not in dir():
    RunNumber = 6

if 'OutputFile' not in dir():
    OutputFile = "EmbeddedTracks.pool.root"
           
#==============================================================
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()
topSeq += embeddedGenerator

include ("EmbeddedTrackGenerator/EmbeddedGeneratorCommon.py")

# EOF
