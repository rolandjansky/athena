#==============================================================
#
# Job options file for Geant4 Simulations
#
# CTB_G4Sim: CTB (2004) simulation production
#
__version__="$Revision: 1.31 $"
#==============================================================

#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
# - Select detectors 
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.LAr_setOn()
DetFlags.em_setOn()
DetFlags.Tile_setOn()
DetFlags.Muon_setOff() # muons are not supported in >=13.0.0
# - MCTruth
DetFlags.simulate.Truth_setOn()

#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput='ctb_MyOutputFile.root'
athenaCommonFlags.EvtMax=3

#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import SimFlags
SimFlags.import_JobProperties('G4AtlasApps.ctb_flags') # - specfic CTB flags

SimFlags.LArEMBenergyCor.set_On()
SimFlags.RunConditions.set_Value('CTB_G4Sim.CtbRunConditionsData')
SimFlags.RunNumber.set_Value(2107)
# If we want to use Beam conditions per run instead the standard 
# flat 3x3 spot  
#SimFlags.BeamConditions.set_Value(True)

#  set the following to True to activate 0.15 X0 of far upstream 
# material (requested by LAr)
SimFlags.LArFarUpstreamMaterial.set_Value(False)
# set the following to 'LAr', 'Tile' or 'LAr+Tile' to activate
# calibration hits
SimFlags.CalibrationRun.set_Value('')

#--- Generator flags ------------------------------------------
SimFlags.Seeds.set_Value('SINGLE 2000160768 643921183')
SimFlags.ParticlePDG.set_Value('11')
SimFlags.Energy.set_Value(54000)

#---  Output printout level ----------------------------------- 
#output threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 4

#==============================================================
#==============================================================
# Job configuration
# ***>> Do not add flags or simulation options below this line
#==============================================================
#==============================================================

#--- CTB setup description  ----------------------------------
from G4AtlasApps import SimKernel
# enter interactive mode
#theApp.initialize()
#>## this starts the space to customize the simulation ####### 

#<## this ends the space to customize the simulation #########


# start run after the interactive mode
#theApp.nextEvent( theApp.EvtMax )

#--- End jobOptions.G4Ctb_SimProd.py file  --------------------
