# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# JetRecStandard.py
#
# David Adams
# October 2014
#
# Wrapper for JetRecStandardToolManager.py that first uses RecExCommon to set flags.
# High-level RecExCommon scripts should use this in place or JetRecStandardToolManager.
#
# Call with
#   from JetRec.JetRecStandard import jtm

myname = "JetRecStandard: "
print myname + "Begin."

from RecExConfig.RecFlags import rec 
from JetRec.JetRecFlags import jetFlags

# Import the jet reconstruction control flags.
from JetRec.JetRecFlags import jetFlags
from RecExConfig.ObjKeyStore import cfgKeyStore
from AthenaCommon import Logging
jetlog = Logging.logging.getLogger('JetRec_jobOptions')

# Disable usage of vertices in pflow jets, if we are using cosmic data.
from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Beam.beamType == 'cosmics':
  jetFlags.useVertices = False

# Skip truth if rec says it is absent.
if not rec.doTruth():
  jetFlags.useTruth = False 
jetlog.info( " Truth enabled ? jetFlags.useTruth == %s", jetFlags.useTruth() )

#skip track if not built or not present in the job
if not rec.doInDet()  \
   and ( not cfgKeyStore.isInTransient("xAOD::VertexContainer","PrimaryVertices") \
         or not cfgKeyStore.isInTransient('xAOD::TrackParticleContainer','InDetTrackParticles') ):
    jetFlags.useTracks = False
jetlog.info( " Tracks enabled ? jetFlags.useTracks == %s", jetFlags.useTracks() )

#skip muon segment if not built
if not rec.doMuon() or not rec.doMuonCombined() :
  jetFlags.useMuonSegments = False
  print rec.doMuon() , rec.doMuonCombined()
jetlog.info( " MuonSegments enabled ? jetFlags.useMuonSegments == %s", jetFlags.useMuonSegments() )

#skip cluster  if not built
if not rec.doCalo():
  jetFlags.useTopo = False
jetlog.info( " TopCluster enabled ? jetFlags.useTopo == %s", jetFlags.useTopo() )
    
# The following can be used to exclude tools from reconstruction.
if 0:
  jetFlags.skipTools = ["comshapes"]
jetlog.info( "Skipped tools: %s", jetFlags.skipTools())

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
