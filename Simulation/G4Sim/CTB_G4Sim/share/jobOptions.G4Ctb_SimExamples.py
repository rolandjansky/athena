#==============================================================
#
# Job options file for Geant4 Simulations
#
# CTB_G4Sim: CTB (2004) simulation production
#
__version__="$Revision: 1.12"
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


# - Option1: run using specfic CTB flags for combined layout
# ----------
#SimFlags.SimLayout.set_Value('ctbh8_combined')
#SimFlags.LArEMBenergyCor.set_On()
##SimFlags.CalibrationRun.set_Value('LAr+Tile')  # Calibration runs 
#SimFlags.Eta.set_Value(0.2)              # Sets eta value 
#SimFlags.MagnetMBPSIDBz.set_Value(-1.4)  # Constant fields

# - Option2: run using run-conditions for the CTB runs, 
#----------- only few runs availble (example with run 2345) 
SimFlags.LArEMBenergyCor.set_On()
SimFlags.RunConditions.set_Value('CTB_G4Sim.CtbRunConditionsData')
SimFlags.RunNumber.set_Value(2345)

# - Option3: run for LAr material studies 
#-----------
#SimFlags.SimLayout.set_Value('ctbh8_lar-material')
#SimFlags.LArEMBenergyCor.set_On()
#SimFlags.Eta.set_Value(0.4)
#SimFlags.LArMaterialAluThickness.set_Value(25.)


# - Option4:  Re-writes the constant magnetic field for 
#----------- the MBPSID with the map.  
#SimFlags.MagFieldMap.set_Value('mbps1-all-id-800-mbps2-muons-800.data')

#--- Generator flags ------------------------------------------
SimFlags.Seeds.set_Value('SINGLE 2000160768 643921183')
SimFlags.ParticlePDG.set_Value('11')
SimFlags.Energy.set_Value(54000)

# - Ex1: how to set the full set of orders for the 
#------- generator:
#SimFlags.ParticleGeneratorOrders.set_Value({'vertX:' : \
#' constant -27500.0','vertY:' :' flat   -10.0 15.0',\
#'vertZ:' : ' flat   -15.0 15.0','t:' :' constant -27500.0',\
#'momX:' : ' fixed 1','momY:' : ' fixed 0','momZ:' : ' fixed 0'})

# - Ex2:  how to run with a gaussian energy distribution: 
#-------
#SimFlags.ParticleGeneratorOrders.Value['energy:']=' gauss 54000 20'

# - Ex3: how to run with any energy distribution:
#------- 
# In this case you have to customize the ParticleGenerator (see 
# in the generator section)
SimFlags.ParticleGeneratorOrders()['energy:']=' histogram 2857'

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

theApp.initialize()
#>## this starts the space to customize the simulation ####### 

# - Ex1: change the verbosity 
#-------
#SimFlags.G4Commands += ['/tracking/verbose 1']

# - Ex2: change the Pixel position 
#------
#pixel_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-300,0.,8)
#ctb_pixel.df.MoveTo(pixel_position)

#<## this ends the space to customize the simulation #########

#--- Generator  ----------------------------------------------
##############################################################
#### NOTE: this is not woorking since 11.3.0 due to problems 
#### in the ParticleGenerator code.   
##############################################################
#>## this starts the space to customize ParticleGenerator #### 
# basically to use your own distributions for the different 
# ParticleGenerator orders:

# - Ex1: import a distribution from a root file  
#------- 
#from G4AtlasApps.atlas_utilhisto import HistoAtHistoSvc 
#h1=HistoAtHistoSvc('2857')
#h1.retrieve_historoot("spectrum_function_2857.root",'h2857')
#h1.dump_histo(theApp)

# - Ex2: build the distribution   
#------- 
from CTB_G4Sim.CtbPhotonESpectrumExample import build_spectrum
h2=build_spectrum(theApp,2857)
##<## this ends the space to customize ParticleGenerator ######

theApp.nextEvent( theApp.EvtMax )

#--- End jobOptions.G4Ctb_SimProd.py file  --------------------
