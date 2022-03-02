# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#==============================================================
#
# Job options file for Geant4 Simulations
#
#==============================================================
theApp.setup( MONTECARLO )
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



theApp.ExtSvc += ["AtRndmGenSvc"]
from RngComps.RngCompsConf import AtRndmGenSvc
RndmGenSvc = AtRndmGenSvc()
RndmGenSvc.Seeds = ["SINGLE 2040160768 443921183"]
ServiceMgr += RndmGenSvc

# OUTPUT PRINTOUT LEVEL
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
# you can override this for individual modules if necessary
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel               = 3
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1

# -----------------
# -----------------
# ParticleGenerator
# -----------------

#ParticleGenerator = Algorithm("ParticleGenerator")

#ParticleGenerator.orders = [
# "pdgcode: constant  211",    
# "energy: constant 100000",   
#
# "vertX: flat   -5.0 5.0",
# "vertY: flat   -5.0 5.0",
# "vertZ: constant -21600.0",

# "t: constant -21600.0",
#
# "momX: fixed 0",
# "momY: fixed 0",
# "momZ: fixed 1"
#
#  ]
#-------------------------------------------------------------
# Configure algorithms and services
#-------------------------------------------------------------
# G4Svc
# -----
G4Svc = Service( "G4Svc" )
G4Svc.PhysicsList       =       "none"
G4Svc.SaveHits          =       False
G4Svc.Visualize         =       False
G4Svc.RunVerbosity      =           2
G4Svc.EventVerbosity    =           0
G4Svc.TrackingVerbosity =           0
G4Svc.Listener		=	False
G4Svc.InteractiveG4	=       False
G4Svc.FADSMacro		=  "H6G4LArCal2002.mac:PhysicsList.mac"

#--------------------------------------------------------------
