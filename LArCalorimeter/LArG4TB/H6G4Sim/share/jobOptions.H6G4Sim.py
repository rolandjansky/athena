#==============================================================
#
# Job options file for Geant4 Simulations
#
#==============================================================
theApp.setup( MONTECARLO )
include( "RDBAccessSvc/RDBAccessSvcDevdb_jobOptions.py" )

include( "NovaCnvSvc/NovaCnvSvc_jobOptions.py" )

include( "NovaConverters/NovaConverters_jobOptions.py" )

include( "PartPropSvc/PartPropSvc.py" )

# hit athenapool converter
# Remove this comment if you want to write out the hits
include( "AtlasG4Sim/HitAthenaPoolWriteOptions.py" )

# instanciate explicitely the DetDescr manager here
include ("DetDescrCnvSvc/DetStore_joboptions.py")

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DecodeIdDict = TRUE 
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )

include( "G4Svc/jobOptions.G4Svc.py" )

# GeoModel initialization
include( "AtlasGeoModel/GeoModelCommon.py" )

# OUTPUT PRINTOUT LEVEL
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
# you can override this for individual modules if necessary
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel               = 4
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 100
#-------------------------------------------------------------
# Configure algorithms and services
#-------------------------------------------------------------
# G4Svc
# -----
G4Svc = Service( "G4Svc" )
G4Svc.PhysicsList       =       "none"
G4Svc.SaveHits          =       FALSE
G4Svc.Visualize         =       FALSE
G4Svc.RunVerbosity      =           2
G4Svc.EventVerbosity    =           0
G4Svc.TrackingVerbosity =           0
G4Svc.Listener		=	FALSE
G4Svc.InteractiveG4	=       FALSE
G4Svc.FADSMacro		=  "env.mac:H6G4LArCal.mac:PhysicsList.mac:Filters.mac"
# ParticleGun
# -----------------
import AthenaCommon.AtlasUnixGeneratorJob
include("GeneratorUtils/StdEvgenSetup.py")
import ParticleGun as PG
try: 
    ParticleGeneratorPDG=' constant '+ParticlePDG
    particl = int(ParticlePDG)
except:
    log.warning('tbH6Generator: You are running geantino')
    ParticleGeneratorPDG=' constant 999'
    particl = 999
# - Energy 
try:
    ParticleGeneratorEnergy=' constant '+ repr(Energy)
    energy = float(Energy)
except:
    log.warning('Unknown energy. Set 200 GeV')
    ParticleGeneratorEnergy=' constant 200000'
    energy = 200000.

pg = PG.ParticleGun()
ps = PG.ConstEnergySampler(pid=particl, energy=energy, eta=PG.UniformSampler(-6.0, 6.0))
pg.samplers.append(ps)
topSeq += pg
#--------------------------------------------------------------
