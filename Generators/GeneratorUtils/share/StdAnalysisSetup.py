## Generic JO header for use in all (evgen) analysis job options

## Pull in *really* generic stuff shared with StdEvgenSetup.py
include("GeneratorUtils/StdJOSetup.py")

## Enable reading in from POOL evgen files, from INFILE if set
import AthenaPoolCnvSvc.ReadAthenaPool
if "INFILE" in dir():
    svcMgr.EventSelector.InputCollections = [INFILE]

## Import useful analysis stuff
from AthenaPython import PyAthena
from AthenaPython.PyAthena import StatusCode, McEventCollection, HepMC, CLHEP
import McParticleEvent.Pythonizations
from GeneratorModules.EvgenAnalysisAlg import EvgenAnalysisAlg
