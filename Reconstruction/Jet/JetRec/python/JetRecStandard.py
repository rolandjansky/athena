# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# JetRecStandard.py
#
# David Adams
# October 2014
# Updated March 2015
#
# Wrapper for JetRecStandardToolManager.py that first uses RecExCommon to set flags.
# High-level RecExCommon scripts should use this in place or JetRecStandardToolManager.
#
# Call with
#   from JetRec.JetRecStandard import jtm
#
# Jet flags should be set before making this call. Those for truth, clusters, tracks
# and muon segments will be set here iff they are not already set.

myname = "JetRecStandard: "
print myname + "Begin."

from RecExConfig.RecFlags import rec 
from InDetRecExample.InDetJobProperties import InDetFlags
from JetRec.JetRecFlags import jetFlags

# Function to display flag value and status.
def sflagstat(flag):
  return str(flag()) + " (status=" + str(flag.statusOn) + ")"

# Import the jet reconstruction control flags.
from JetRec.JetRecFlags import jetFlags
from RecExConfig.ObjKeyStore import cfgKeyStore
from AthenaCommon import Logging
jetlog = Logging.logging.getLogger('JetRec_jobOptions')

# Skip truth if rec says it is absent.
# No action if someone has already set the flag.
print myname + "Initial useTruth: " + sflagstat(jetFlags.useTruth)
print myname + "     rec.doTruth: " + str(rec.doTruth())
if not jetFlags.useTruth.statusOn:
  jetFlags.useTruth = rec.doTruth()
print myname + "  Final useTruth: " + sflagstat(jetFlags.useTruth)

# Skip use of topoclusters if not built
# No action if someone has already set the flag.
print myname + "Initial use topoclusters: " + str(jetFlags.useTopo())
if not jetFlags.useTopo.statusOn:
  jetFlags.useTopo = rec.doCalo()
print myname + "  Final use topoclusters: " + str(jetFlags.useTopo())

# Skip tracks if tracks or vertices are not present in the job.
# No action if someone has already set the flag.
haveTracks = cfgKeyStore.isInTransient('xAOD::TrackParticleContainer','InDetTrackParticles')
haveVertices = cfgKeyStore.isInTransient("xAOD::VertexContainer","PrimaryVertices")
recTracks = rec.doInDet()
recVertices = bool(InDetFlags.doVertexFinding) and (recTracks or haveTracks)
print myname + "Initial useTracks: " + sflagstat(jetFlags.useTracks)
print myname + "      rec doInDet: " + str(recTracks)
print myname + "  doVertexFinding: " + str(recVertices)
print myname + "      have tracks: " + str(haveTracks)
print myname + "    have vertices: " + str(haveVertices)
if not jetFlags.useTracks.statusOn:
  jetFlags.useTracks = (recTracks or haveTracks) and (recVertices or haveVertices)
print myname + "  Final useTracks: " + sflagstat(jetFlags.useTracks)

if not jetFlags.useVertices.statusOn:
  jetFlags.useVertices = (recVertices or haveVertices)
print myname + "   useVertices: " + sflagstat(jetFlags.useVertices)

# Disable usage of vertices in pflow jets, if we are using cosmic data.
from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Beam.beamType == 'cosmics':
  jetFlags.useVertices = False

# Skip pflow if we are not using tracks.
print myname + "Initial usePFlow: " + sflagstat(jetFlags.usePFlow)
if not jetFlags.usePFlow.statusOn:
  jetFlags.usePFlow = jetFlags.useTracks()
print myname + "Final usePFlow: " + sflagstat(jetFlags.usePFlow)

# Skip use of muon segments if not built.
# No action if someone has already set the flag.
print myname + "Initial use muon segments: " + sflagstat(jetFlags.useMuonSegments)
if not jetFlags.useMuonSegments.statusOn:
  jetFlags.useMuonSegments = rec.doMuon() and rec.doMuonCombined()
print myname + "  Final use muon segments: " + sflagstat(jetFlags.useMuonSegments)

# Use rec flag to control BTagging.
# No. Disable this unit we get support from Btagging to do this.
# No action if someone has already set the flag.
print myname + "Initial use Btagging: " + str(jetFlags.useBTagging)
print myname + "     rec do BTagging: " + str(rec.doBTagging())
if not jetFlags.useBTagging.statusOn:
  #jetFlags.useBTagging = rec.doBTagging()
  jetFlags.useBTagging = False
print myname + "  Final use Btagging: " + str(jetFlags.useBTagging)

# The following can be used to exclude tools from reconstruction.
if 0:
  jetFlags.skipTools = ["comshapes"]
jetlog.info( "Skipped tools: %s", jetFlags.skipTools())

from RecExConfig.RecAlgsFlags import recAlgs
if not recAlgs.doEFlow():
  jetFlags.usePFlow = False

# Set the list of rho calculations.
# If caller has set jetFlags.eventShapeTools(), then we use those values.
if jetFlags.eventShapeTools() == None:
  jetFlags.eventShapeTools = []
  if jetFlags.useTopo():
    jetFlags.eventShapeTools += ['emtopo', 'lctopo']
  if jetFlags.usePFlow():
    jetFlags.eventShapeTools += ['empflow']

# Import the jet tool manager.
from JetRec.JetRecStandardToolManager import jtm

print myname + "End."
