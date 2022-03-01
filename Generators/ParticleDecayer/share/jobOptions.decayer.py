###############################################################
#
# Job options file for testing ParticleGenerator.
#
#==============================================================

import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#load relevant libraries
from PartPropSvc.PartPropSvcConf import PartPropSvc
ServiceMgr += PartPropSvc()

# The following is needed to load the Athena Random
# Number Generation Service.
from RngComps.RngCompsConf import AtRanluxGenSvc
ServiceMgr += AtRanluxGenSvc()
ServiceMgr.AtRanluxGenSvc.Seeds = ["SINGLE 2040160768 443921183"]
# AtRanluxGenSvc.ReadFromFile = TRUE

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel      = VERBOSE

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 100000

#--------------------------------------------------------------
# ParticleGenerator parameters
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()

# get the configurable class
from ParticleGenerator.ParticleGeneratorConf import ParticleGenerator
job += ParticleGenerator()


# For VERBOSE output from ParticleGenerator.
job.ParticleGenerator.OutputLevel = DEBUG 

#This is an example of configuration of ParticleGenerator
job.ParticleGenerator.orders = [
  "pdgcode[0]:  constant 999",
  "et[0]: flat 10000. 100000.",
  #"vertZ[0]: gaussian 0 50",
  "eta[0]: flat -2.5 2.5",
  "phi[0]: flat -3.14159 3.14159"
  ### and if you want to generate more than one particle...
  #"pdgcode[1]:  constant 999",
  #"e[1]: flat 10000. 120000.",
  #"eta[1]: flat -2.5 2.5",
  #"phi[1]: flat -3.14159 3.14159"
  #"pdgcode[2]:  constant 999",
  #"e[2]: flat 10000. 120000.",
  #"eta[2]: flat -2.5 2.5",
  #"phi[2]: flat -3.14159 3.14159"
  ]

#--------------------------------------------------------------
# ParticleDecayer parameters
#--------------------------------------------------------------
from ParticleDecayer.ParticleDecayerConf import ParticleDecayer
job += ParticleDecayer()
job.ParticleDecayer.OutputLevel = DEBUG 
job.ParticleDecayer.McEventCollection = "GEN_EVENT"

##### example to configure ParticleDecayer to produce LeptonJets with two dark photons (gd) from the decay of a dark scalar (s)
#####
#####   s-> gd + gd 
#####       gd -> l + l
#####
job.ParticleDecayer.LJType = 2 #LeptonJet type: 1 = one dark photon per LeptonJet, 2 = two dark photons per LeptonJet
job.ParticleDecayer.ScalarMass = 5000 #new mass of the scalar in MeV
job.ParticleDecayer.ScalarPDGID = 700021 #new PDGID of the scalar
job.ParticleDecayer.ParticleID = 999 #We use "geantino" to indicate dark scalar 
job.ParticleDecayer.ParticleMass = 600 #new mass of the dark photon in MeV 
job.ParticleDecayer.ParticleLifeTime = 100 #new ctau of the dark photon in mm 
job.ParticleDecayer.ParticlePolarization = -1 #polarization of the dark photon: 0 = isotropic decay, -1 = tranverse, 1 = longitudinal
job.ParticleDecayer.OppositePolarization = False
job.ParticleDecayer.ParticlePDGID = 700022 #Use this new code for dark photon

##### example to configure ParticleDecayer to produce LeptonJets with ionly one dark photon (gd)
#####
#####  gd -> l + l
#####
#job.ParticleDecayer.LJType = 1 #LeptonJet type: 1 = one dark photon per LeptonJet, 2 = two dark photons per LeptonJet
#job.ParticleDecayer.ParticleID = 999 #We use "geantino" to indicate dark photon 
#job.ParticleDecayer.ParticleMass = 250 #new mass of the dark photon in MeV
#job.ParticleDecayer.ParticleLifeTime = 100 #new ctau of the dark photon in mm 
#job.ParticleDecayer.ParticlePolarization = 1 #polarization of the dark photon: 0 = isotropic decay, -1 = tranverse, 1 = longitudinal 
#job.ParticleDecayer.ParticlePDGID = 700022 #Use this new code for dark photon


#decay mode of the dark photon(s): you need to specify the XX% electrons, YY% muons, ZZ% pions. NB: sum must be 1!!))
job.ParticleDecayer.DecayBRElectrons = 0.30 #dark photon BR to electrons
job.ParticleDecayer.DecayBRMuons     = 0.30 #dark photon BR to muons
job.ParticleDecayer.DecayBRPions     = 0.40 #dark photon BR to pions

job.ParticleDecayer.DoUniformDecay               = False  # Uniform in decay radius
job.ParticleDecayer.DoExponentialDecay           = True
job.ParticleDecayer.ExpDecayDoVariableLifetime   = True
job.ParticleDecayer.ExpDecayPercentageToKeep     = 0.8
job.ParticleDecayer.ExpDecayDoTruncateLongDecays = True

job.ParticleDecayer.BarrelRadius         = 8.e3 # in mm
job.ParticleDecayer.EndCapDistance       = 11.e3 # in mm
job.ParticleDecayer.ThetaEndCapBarrel    = 0.628796286 #0.439067982 = eta of 1.5

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
Stream1.OutputFile = "ParticleDecayer.EVNT.pool.root"
Stream1.ItemList += [ "2101#*", "133273#*" ]

#==============================================================
#
# End of job options file
#
###############################################################
