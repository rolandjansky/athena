
#For override of vertex position

#old way, MC11, used to be a postInclude
#from AthenaCommon.AlgSequence import AlgSequence
#job = AlgSequence()
#job.VertexPositionGenerator.VertexOverrideFile = "vtx.txt"

#new way, MC12, should be a preInclude
from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()
SimFlags.VertexOverrideFile.set_Value_and_Lock("vtx.txt")
# Can also add "preInclude=SimulationJobOptions/preInclude.VerboseVertexPositioner.py"


