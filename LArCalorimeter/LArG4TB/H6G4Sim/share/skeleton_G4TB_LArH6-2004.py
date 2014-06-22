# get the logger
from AthenaCommon.Logging import *
ctbG4log = logging.getLogger('ctbG4')

ctbG4log.info('****************** STARTING CTB H6 G4 ******************')
#==============================================================
# Job definition parameters:
#==============================================================
from AthenaCommon.AthenaCommonFlags import jobproperties
#Jobs should stop if an include fails.
jobproperties.AthenaCommonFlags.AllowIgnoreConfigError=True


PoolHitsOutput = runArgs.outputHitsFile
SingleRun = runArgs.IsSingleRun
doFilter = runArgs.DoFiltering
doBirk = runArgs.DoBirk
ctbG4log.info( "*** IsSingleRun:  "+str(SingleRun))
ctbG4log.info( "*** doFiletring:  "+str(doFilter))
ctbG4log.info( "*** doBirkLaw:    "+str(doBirk))

if SingleRun:
  XCryo = float(runArgs.xCryo)
  YTable = float(runArgs.yTable)
  BeamE = float(runArgs.beamEnergy)
  EvtMax = runArgs.maxEvents
  ctbG4log.info( "*** XCryo:  "+str(XCryo))
  ctbG4log.info( "*** YTable: "+str(YTable))
  ctbG4log.info( "*** Energy: "+str(BeamE))
else:   
  from H6G4Sim.h6prod_getxy import *
  iCryo = runArgs.xCryo - 1
  if iCryo < 0:
    ctbG4log.info( "*** Negative iCryo!  ")
    iCryo = 0
  x,y,nevt = getXYInPolygon(iCryo, runArgs.maxEvents)
  XCryo = float(x)
  YTable = float(y)
  BeamE = str(runArgs.beamEnergy)
  EvtMax = nevt
  ctbG4log.info( "*** XCryo:  "+str(iCryo)+" "+str(XCryo))
  ctbG4log.info( "*** YTable: "+str(iCryo)+" "+str(YTable))
  ctbG4log.info( "*** Energy: "+str(BeamE))

XSmear = float(runArgs.xSmear)
YSmear = float(runArgs.ySmear)
G4Seed = runArgs.randomSeed
GSeed1 = str(runArgs.genSeed1)
GSeed2 = str(runArgs.genSeed2)
ParticlePDG = str(runArgs.beamParticle)
PhysicsList = runArgs.PhysicsList

ctbG4log.info( "*** Events:   "+str(EvtMax))
ctbG4log.info( "*** Part.:    "+ParticlePDG)
ctbG4log.info( "*** PhysList: "+PhysicsList)
ctbG4log.info( "*** XSmear:   "+str(XSmear))
ctbG4log.info( "*** YSmear:   "+str(YSmear))
ctbG4log.info( "*** GSeed1:   "+GSeed1)
ctbG4log.info( "*** GSeed2:   "+GSeed2)
ctbG4log.info( "*** G4Seed:    "+str(G4Seed))
ctbG4log.info( "*** PoolHitsOutput: "+PoolHitsOutput)


include("H6G4Sim/jobOptions.G4TB_LArH6-2004-prod.py")

