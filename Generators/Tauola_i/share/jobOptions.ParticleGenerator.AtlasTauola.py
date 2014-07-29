###############################################################
#
# Job options file for testing ParticleGenerator.
#
#==============================================================
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#load relevant libraries
include( "PartPropSvc/PartPropSvc.py" )

theApp.Dlls  += [ "ParticleGenerator","Tauola_i","Photos_i","TruthExamples","Pythia_i" ]      
theApp.TopAlg = [ "ParticleGenerator","Tauola","Photos","PrintMC" ]

# The following is needed to load the Athena Random
# Number Generation Service.
theApp.ExtSvc += ["AtRndmGenSvc"]
AtRndmGenSvc = Service( "AtRndmGenSvc" )
AtRndmGenSvc.Seeds = ["SINGLE 2040160768 443921183",
                      "TAUOLA 530753022 122769126", "TAUOLA_INIT 772087946 292053749",
                      "PHOTOS 478375275 346831679", "PHOTOS_INIT 239264166 848234952"]

# AtRndmGenSvc.ReadFromFile = TRUE

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc.OutputLevel      = 1

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10 
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.Dump = FALSE

#--------------------------------------------------------------
# TauGenerator parameters
#--------------------------------------------------------------

# e- = 11, e+ = -11, mu- = 13, mu+ = -13, pi+ = 211, pi- = -211, pi0 = 111, geantino = 999
# See PDGTABLE or Rev.Part.Properties for the complete list.

# The orders are in the form of an array of strings.  Each string
# must contain exactly one command.  In most cases, if TauGenerator
# can't figure out what a command means, it will terminate during its
# initialization.  Energies are in GeV, distances and time (ct) are in mm.

# If there is more than one definition of 'TauGenerator.orders',
# only the last one will be used.

# For an example of how to use user-defined histograms to generate a
# distribution, see jobOptions_TauGeneratorExampleHistogram.py.

#--ParticleGenerator
ParticleGenerator = Algorithm( "ParticleGenerator" )
ParticleGenerator.orders = [
 "pdgcode: constant -15",
 "energy: constant 543000",
 "momX: fixed 1",
 "momY: Fixed 1",
 "momZ: fiXed 1"
]

#-- Tauola
Tauola = Algorithm( "Tauola" )
Tauola.TauolaCommand = [        "tauola polar 1",
                                "tauola radcor 1",
                                "tauola phox 0.01",
                                "tauola dmode 0",
                                "tauola jak1 0",
                                "tauola jak2 0"]
# radiate only the tau-s; neccessary!!

#-- Photos
Photos = Algorithm( "Photos" )
Photos.PhotosCommand = [        "photos pmode 2",
                                "photos xphcut 0.01",
                                "photos alpha -1.",
                                "photos interf 1",
                                "photos isec 1",
				"photos itre 0",
				"photos iexp 1",
                                "photos iftop 0"]

#-- PrintMC
PrintMC = Algorithm( "PrintMC" )
#event SVC key
PrintMC.McEventKey = "GEN_EVENT"
# do you want output at all? TRUE/FALSE
PrintMC.VerboseOutput = TRUE
# Event print style Vertex(traditional)/Barcode(barcode ordered)
PrintMC.PrintStyle = "Barcode"
# First and last event to print, if no last events => job end are printed
PrintMC.FirstEvent = 1
PrintMC.LastEvent = 10

#==============================================================
#
# End of job options file
#
###############################################################
