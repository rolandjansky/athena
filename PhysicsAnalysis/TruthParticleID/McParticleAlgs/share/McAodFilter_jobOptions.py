# Job options fragment to build a Filtered GenEvent
# author : Sebastien Binet <binet@cern.ch>
#################################################
# import the data types 
import EventKernel.ParticleDataType

# get a handle on the job sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# configure McAodFilter
from McParticleAlgs.McParticleAlgsConf import McAodFilter
job += McAodFilter()

# Filters

# Filter which looks for Z0 decaying into a b and a bbar
job.McAodFilter.McVtxFilterTool.DecayPatterns  = [ "23 -> -5 + 5" ]

# Filter looking for a Z0 decaying either into a e+e- pair or a muon pair
job.McAodFilter.McVtxFilterTool.DecayPatterns += [ "23 -> -11|-13 + 11|13" ]

# this is the decay pattern for a t->W+ b
job.McAodFilter.McVtxFilterTool.DecayPatterns += [ "6 ->24 + -5" ]

# Filter looking for a (stable, ie:no end_vertex) gamma
# Hopefully a cut in pT would/should be applied in a coming version
job.McAodFilter.McVtxFilterTool.DecayPatterns += [ "->22" ]
