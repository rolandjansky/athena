###############################################################
# 20/01/2011 Andrzej Olszewski
# jobOptions to run Starlight generation
#==============================================================
# General Application Configuration options
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )
from AthenaCommon.Configurable import Configurable
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

# configuring the Athena application for a 'generator' job
import AthenaCommon.AtlasUnixGeneratorJob

# make sure we are loading the ParticleProperty service
from PartPropSvc.PartPropSvcConf import PartPropSvc
svcMgr += PartPropSvc()

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
## get a handle on the top sequence of algorithms
from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence()

from Starlight_i.Starlight_iConf import Starlight_i
topAlg += Starlight_i()

StoreGateSvc = svcMgr.StoreGateSvc
StoreGateSvc.Dump = TRUE

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
svcMgr.MessageSvc.OutputLevel  = 3
svcMgr.MessageSvc.defaultLimit = 100000

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10

# Set run number (default 0 causes problems)
svcMgr.EventSelector.RunNumber   = 1
svcMgr.EventSelector.FirstEvent  = 1

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------

# Use fixed seeds for reproducibility
seed1 = int(os.popen2("date +%s")[1].read())
seed1 &= 0xfffffffe

Starlight_i = Algorithm( "Starlight_i" )
Starlight_i.McEventKey = "GEN_EVENT"
# define parameters either via config file
#Starlight_i.ConfigFileName = "slight.in"
# define parameters in python for:
# --------------------------------
# gamma-gamma continuum:
# "productionMode 1", "prodParticleId 13" (muons) or 11 (electrons),
# ------------------------------------------------------------------
# J/Psi in gamma-Pomeron:
# "productionMode 2", "prodParticleId 443013" (muons) or 443011 (electrons),
# --------------------------------------------------------------------------
# Upsilon in gamma-Pomeron:
# "productionMode 2", "prodParticleId 553013" (muons) or 553011 (electrons),
# --------------------------------------------------------------------------
Starlight_i.Initialize = [
    "beam1Z 82", "beam1A 208", #Z,A of projectile
    "beam2Z 82", "beam2A 208", #Z,A of target
    "beamLorentzGamma 1482",   #Gamma of the colliding ions, for sqrt(nn)=2.76 TeV
    "maxW 10", #Max value of w
    "minW 2", #Min value of w
    "nmbWBins 400", #Bins n w
    "maxRapidity 8.", #max y
    "nmbRapidityBins 1000", #Bins n y
    "accCutPt 0", #Cut in pT? 0 = (no, 1 = yes)
    "minPt 1.0", #Minimum pT in GeV
    "maxPt 3.0", #Maximum pT in GeV
    "accCutEta 0", #Cut in pseudorapidity? (0 = no, 1 = yes)
    "minEta -10", #Minimum pseudorapidity
    "maxEta 10", #Maximum pseudorapidity
    "productionMode 1", #(1=2-phot,2=vmeson(narrow),3=vmeson(wide))
    "nmbEventsTot 1", #Number of events
    "prodParticleId 13", #Channel of interest
    "randomSeed "+str(seed1), #Random number seed
    "outputFormat 2", #Form of the output
    "beamBreakupMode 5", #Controls the nuclear breakup
    "interferenceEnabled 0", #Interference (0 = off, 1 = on)
    "interferenceStrength 1.", #% of intefernce (0.0 - 0.1)
    "coherentProduction 1", #Coherent=1,Incoherent=0
    "incoherentFactor 1.", #percentage of incoherence
    "bford 9.5",
    "maxPtInterference 0.24", #Maximum pt considered, when interference is turned on
    "nmbPtBinsInterference 120" #Number of pt bins when interference is turned on
    ] 

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )

theApp.Dlls   += [ "GeneratorObjectsAthenaPoolPoolCnv" ]
PoolSvc = Service( "PoolSvc" )

# Name of the output file
Stream1.OutputFile = "starlight.test.pool.root"

# 2101 == EventInfo
# 133273 == MCTruth (HepMC)
Stream1.ItemList += [ "2101#*" ]
Stream1.ItemList += [ "133273#*" ]

#==============================================================
#
# End of job options file
#
###############################################################
