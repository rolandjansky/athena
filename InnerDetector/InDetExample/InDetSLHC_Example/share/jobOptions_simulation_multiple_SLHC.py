#--------------------------------------------------------------
# Template jobOptions: SLHC 
#  - Simulation of multiple particles
#--------------------------------------------------------------

import os
SeedNumber = os.environ['SEEDNUMBER']

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-SDR-BS14T-ATLAS-00"

include("InDetSLHC_Example/preInclude.SLHC.py")

include("InDetSLHC_Example/preInclude.SiliconOnly.py")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolHitsOutput=os.environ['OUTFILENAME']
athenaCommonFlags.EvtMax=int(os.environ['EVTMAX'])

from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()
#simFlags.SimLayout='ATLAS-SLHC-00-00-00'
simFlags.SimLayout='ATLAS-SLHC-01-00-00_VALIDATION'
simFlags.SimLayout.set_On() 
simFlags.EventFilter.set_On()

simFlags.KinematicsMode.set_Value('SingleParticle')
# simFlags.ParticlePDG.set_Value('11') # <-- electron
#simFlags.ParticlePDG.set_Value('13') # <-- muon
#simFlags.MagneticField.set_Off()
simFlags.ParticlePDG.statusOn=False
simFlags.SeedsG4 = SeedNumber + "72" + SeedNumber
simFlags.Seeds = "SINGLE " + SeedNumber + "72" + SeedNumber + " " + SeedNumber + "41" + SeedNumber

from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["SINGLE " + SeedNumber + "72" + SeedNumber + " " + SeedNumber + "41" + SeedNumber]

PdgCode1 = ' constant ' + os.environ['PDGID']
PdgCode2 = ' constant -' + os.environ['PDGID']
EtaRange = ' flat ' + os.environ['ETAMIN'] + ' ' + os.environ['ETAMAX']
PhiRange = ' flat 0 6.283185'
PtSequence = ' sequence 2000.0, 4000.0, 6000.0, 15000.0, 50000.0, 100000.0'

simFlags.ParticleGeneratorOrders={
    'vertX:' : ' constant 0.0',
    'vertY:' : ' constant 0.0',
    'vertZ:' : ' constant 0.0',
    't:'     : ' constant 0.0',

    #---muon and anti-muon
    'pdgcode[0]:' : PdgCode1,
    'eta[0]:'     : EtaRange,
    'phi[0]:'     : PhiRange,
    'pt[0]:'      : PtSequence,
    'pdgcode[1]:' : PdgCode2,
    'eta[1]:'     : EtaRange,
    'phi[1]:'     : PhiRange,
    'pt[1]:'      : PtSequence,
    'pdgcode[2]:' : PdgCode1,
    'eta[2]:'     : EtaRange,
    'phi[2]:'     : PhiRange,
    'pt[2]:'      : PtSequence,
    'pdgcode[3]:' : PdgCode2,
    'eta[3]:'     : EtaRange,
    'phi[3]:'     : PhiRange,
    'pt[3]:'      : PtSequence,
    'pdgcode[4]:' : PdgCode1,
    'eta[4]:'     : EtaRange,
    'phi[4]:'     : PhiRange,
    'pt[4]:'      : PtSequence,
    'pdgcode[5]:' : PdgCode2,
    'eta[5]:'     : EtaRange,
    'phi[5]:'     : PhiRange,
    'pt[5]:'      : PtSequence,
    'pdgcode[6]:' : PdgCode1,
    'eta[6]:'     : EtaRange,
    'phi[6]:'     : PhiRange,
    'pt[6]:'      : PtSequence,
    'pdgcode[7]:' : PdgCode2,
    'eta[7]:'     : EtaRange,
    'phi[7]:'     : PhiRange,
    'pt[7]:'      : PtSequence,
    'pdgcode[8]:' : PdgCode1,
    'eta[8]:'     : EtaRange,
    'phi[8]:'     : PhiRange,
    'pt[8]:'      : PtSequence,
    'pdgcode[9]:' : PdgCode2,
    'eta[9]:'     : EtaRange,
    'phi[9]:'     : PhiRange,
    'pt[9]:'      : PtSequence,
    'pdgcode[10]:' : PdgCode1,
    'eta[10]:'     : EtaRange,
    'phi[10]:'     : PhiRange,
    'pt[10]:'      : PtSequence,
    'pdgcode[11]:' : PdgCode2,
    'eta[11]:'     : EtaRange,
    'phi[11]:'     : PhiRange,
    'pt[11]:'      : PtSequence,
    'pdgcode[12]:' : PdgCode1,
    'eta[12]:'     : EtaRange,
    'phi[12]:'     : PhiRange,
    'pt[12]:'      : PtSequence,
    'pdgcode[13]:' : PdgCode2,
    'eta[13]:'     : EtaRange,
    'phi[13]:'     : PhiRange,
    'pt[13]:'      : PtSequence,
    'pdgcode[14]:' : PdgCode1,
    'eta[14]:'     : EtaRange,
    'phi[14]:'     : PhiRange,
    'pt[14]:'      : PtSequence,
    'pdgcode[15]:' : PdgCode2,
    'eta[15]:'     : EtaRange,
    'phi[15]:'     : PhiRange,
    'pt[15]:'      : PtSequence,
    'pdgcode[16]:' : PdgCode1,
    'eta[16]:'     : EtaRange,
    'phi[16]:'     : PhiRange,
    'pt[16]:'      : PtSequence,
    'pdgcode[17]:' : PdgCode2,
    'eta[17]:'     : EtaRange,
    'phi[17]:'     : PhiRange,
    'pt[17]:'      : PtSequence,
    'pdgcode[18]:' : PdgCode1,
    'eta[18]:'     : EtaRange,
    'phi[18]:'     : PhiRange,
    'pt[18]:'      : PtSequence,
    'pdgcode[19]:' : PdgCode2,
    'eta[19]:'     : EtaRange,
    'phi[19]:'     : PhiRange,
    'pt[19]:'      : PtSequence
  }

from G4AtlasApps import SimKernel

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = WARNING

# SLHC Flags -----------------------------------------------------
from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
#SLHC_Flags.SLHC_Version = 'SLHC-19-20'
SLHC_Flags.SLHC_Version = os.environ['LAYOUT']

#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------
# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()
