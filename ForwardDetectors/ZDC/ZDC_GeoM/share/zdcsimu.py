#  Read arguments from environment variables
import os
#serialNumber = os.environ['serialNumber']
#runNumber = os.environ['runNumber']
numEvents = 2


#--- Detector flags -------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.ZDC_setOn()
DetFlags.simulate.Truth_setOn()


from RngComps.RngCompsConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()


#--- AthenaCommon flags ---------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput= "Hits.pool.root"
athenaCommonFlags.EvtMax=int(numEvents)


#--- Simulation flags -----------------------------------------
from G4AtlasApps.SimFlags import SimFlags
SimFlags.SeedsG4 = 1114
SimFlags.SimLayout.set_On() # Use the default value


# for the next three lines look into Andrea Dellacqua's talk in the Forwardsimulation meeting of Nov23-2010
# http://indico.cern.ch/conferenceDisplay.py?confId=114120
SimFlags.ForwardDetectors.set_On()
SimFlags.ForwardDetectors=1
SimFlags.EventFilter.set_Off()
#SimFlags.WorldZRange.statusOn=True
#SimFlags.WorldZRange.set_Value(145000)


from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag = "OFLCOND-SIM-BS7T-00"
# Look into SimFlags.SimLayout for other possible values 
#SimFlags.SimLayout='ATLAS-GEO-06-00-00' # specific value 
#SimFlags.SimLayout.set_On()              # uses the default value 
SimFlags.MagneticField.set_Off()


# - uses single particle generator
SimFlags.KinematicsMode='SingleParticle'
# e-/+ = +/-11, mu-/+ = +/-13, pi+/- = +/-211, pi0=111, eta=221,
# neutron=2112, proton=2212, gamma 22
SimFlags.ParticlePDG='22'
#   set energy constant to 10 GeV
SimFlags.Energy=10000 
# set your own particle generator orders here.
# do this for example if you need generation at fixed pt
# PDG code will be set following user instructions in the SimFlags.ParticlePDG
# SimFlags.Energy will be ingored if you uncomment the following lines
SimFlags.ParticleGeneratorOrders={
    'vertX:' : ' flat -1.0  1.0',
    'vertY:' : ' flat -1.0  1.0',
    'vertZ:' : ' constant -139500.0',
    't:'     : ' constant 0.0',
    'eta:'   : ' constant -100.0',
    'phi:'   : ' flat  0 6.28318',
    'e:'     : ' constant 1360000'}
#          'pt:' : ' constant 50000'}


#---  Output printout level ----------------------------------- 
#output threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#you can override this for individual modules if necessary
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 2


#==============================================================
# Job configuration
# ***>> Do not add flags or simulation options below this line
#==============================================================
from G4AtlasApps import SimKernel


# The following lines store the desired container into the output file
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
stream1.OutputFile = "SimOut.pool.root"
stream1.ItemList += [ 'EventInfo#*', 'McEventCollection#*','ZDC_SimPixelHit_Collection#*','ZDC_SimStripHit_Collection#*' ]


#--- End jobOptions.G4Atlas_Sim.py file  ------------------------------
