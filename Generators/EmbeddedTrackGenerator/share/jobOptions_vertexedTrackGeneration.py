# VertexedTrackGenerator generates particles originating at reconstructed vertex(es) position(s).
# The input vertexes are taken from an AOD file.
#
# One can use something like 
#
#    athena -c "EvtMax=5; OutputFile='test.pool.root'" EmbeddedTrackGenerator/jobOptions_vertexedTrackGeneration.py
#
# to set a few things that usually change often.
#
# Author: Andrei Gaponenko

from AthenaCommon.SystemOfUnits import MeV,GeV

from EmbeddedTrackGenerator.EmbeddedTrackGeneratorConf import VertexedTrackGenerator
vertexedGenerator = VertexedTrackGenerator()
#vertexedGenerator.OutputLevel = VERBOSE
#vertexedGenerator.RandomSeed = 9688
#vertexedGenerator.pdg_id = 13
#vertexedGenerator.generateAntiParticles = True
#vertexedGenerator.ptMin = 500*MeV
#vertexedGenerator.ptMax = 15*GeV

if 'InputCollections' not in dir():
    InputCollections = [ "rfio:/castor/cern.ch/user/a/andr/ovl/valid1.005001.pythia_minbias.recon.AOD.e322_s412_r421_tid022144/AOD.022144._00001.pool.root.1" ]

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
    RunNumber = 5

if 'OutputFile' not in dir():
    OutputFile = "VertexedTracks.pool.root"
           
#==============================================================
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()
topSeq += vertexedGenerator

include ("EmbeddedTrackGenerator/EmbeddedGeneratorCommon.py")

# EOF
