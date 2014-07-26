#
# Job options file
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

# make sure we are loading the ParticleProperty service
from PartPropSvc.PartPropSvcConf import PartPropSvc
ServiceMgr += PartPropSvc()

# Add POOL persistency
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
Stream1 = AthenaPoolOutputStream( "StreamEVGEN" )
Stream1.ItemList += [ "2101#*", "133273#*" ]
Stream1.OutputFile = "exhumetest.pool.root"

# This is an artefact of the old version. I am not sure if it is needed
#include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topAlg=AlgSequence("TopAlg")

# get the configurable class
from Exhume_i.Exhume_iConf import ExHuME
ExHuME.HiggsDecay          = 15 #PDG code for higgs decay
ExHuME.MassMin          = 115.0 #PDG code for higgs decay
ExHuME.MassMax          = 125.0 #PDG code for higgs decay
ExHuME.UsePhotos=True
ExHuME.UseTauola=True
#ExHuME.Process             = "GG"
#ExHuME.ExhumeRunControl    = "ExhumeRunControlParams"
#ExHuME.ETMin               = 50 
#ExHuME.CosThetaMax         = 0.9 #max cos theta in COM.Large vale will lower speed

topAlg += ExHuME()


#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
MessageSvc.OutputLevel = WARNING

#include Tauola & Photos
# ... Tauola
from Tauola_i.Tauola_iConf import Tauola
topAlg += Tauola()
Tauola = topAlg.Tauola
Tauola.TauolaCommand = [        "tauola polar 1",
	                        "tauola radcor 1",
	                        "tauola phox 0.01",
	                        "tauola dmode 0",
	                        "tauola jak1 0",
	                        "tauola jak2 0"]
# ... Photos
from Photos_i.Photos_iConf import Photos
topAlg += Photos()
Photos = topAlg.Photos
Photos.PhotosCommand = [       "photos pmode 1",
                               "photos xphcut 0.01",
	                       "photos alpha -1.",
	                       "photos interf 1",
	                       "photos isec 1",
                               "photos itre 0",
	                       "photos iexp 1",
	                       "photos iftop 0"]
	

#include ( "EvgenJobOptions/MC8_Tauola_Fragment.py" )
#include ( "EvgenJobOptions/MC8_Photos_Fragment.py" )

# The following is needed to load the Athena Random
# Number Generation Service.
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["ExhumeRand 4475757 4747848","PYTHIA 89899 512872","TAUOLA 1048027510 345500367","TAUOLA_INIT 920021 3347532", 
"PHOTOS 2045027510 245500367","PHOTOS_INIT 930021 3447532"]
# ServiceMgr.AtRndmGenSvc.ReadFromFile = TRUE

from TruthExamples.TruthExamplesConf import TestHepMC
topAlg += TestHepMC()
topAlg.TestHepMC.McEventKey = "GEN_EVENT"

from TruthExamples.TruthExamplesConf import PrintMC
topAlg += PrintMC()
PrintMC.McEventKey='GEN_EVENT'
PrintMC.VerboseOutput=TRUE
PrintStyle = 'Barcode'

Stream1.RequireAlgs +=["TestHepMC"]


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is 10)
theApp.EvtMax = 20

# Set run number (default 0 causes problems)
ServiceMgr.EventSelector.RunNumber = 1337

#--------------------------------------------------------------
# other
#--------------------------------------------------------------
ServiceMgr.AthenaSealSvc.CheckDictionary = TRUE


#==============================================================
#
# End of job options file
#
###############################################################
 
