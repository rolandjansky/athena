###############################################################
#
# jobOptions example for using PionDecayer in threshold mode
# with Pt threshold at 2GeV
#
###############################################################
#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
# - Select detectors
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()

#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()
SimFlags.SimLayout.set_On()

## Global conditions tag
from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-SDR-BS7T-04-03"

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput = 'G4Hits.pool.root'
athenaCommonFlags.PoolEvgenInput = ['/afs/cern.ch/atlas/offline/ProdData/15.6.11.3/mu_E200_eta0-60-10000.evgen.pool.root']
athenaCommonFlags.EvtMax = 10

def load_pionDecayer():
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng.load_Dict("PionDecayerDict")
    pionDecayer = AtlasG4Eng.G4Eng.gbl.PionDecayer("PionDecayer")
    pionDecayer.switchOn()
    pionDecayer.SetPtmuon(2*GeV)

    # Uncomment to change decay volume
    #pionDecayer.SetIDETlength(340*cm)
    #pionDecayer.SetIDETradius(110*cm)
SimFlags.initFunctions.add_function("preInitPhysics", load_pionDecayer)

## Populate alg sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()
from G4AtlasApps.PyG4Atlas import PyG4AtlasAlg
topSeq += PyG4AtlasAlg()

