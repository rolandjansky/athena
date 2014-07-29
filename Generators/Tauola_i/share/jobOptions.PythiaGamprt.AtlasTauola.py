###############################################################
#
# Job options file
#
#==============================================================
#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

include( "PartPropSvc/PartPropSvc.py" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
theApp.Dlls  += ["TruthExamples","Pythia_i","Tauola_i","Photos_i" ]
theApp.TopAlg = ["Pythia","Tauola","Photos","PrintMC"]
theApp.ExtSvc += ["AtRndmGenSvc"]
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel               = 2
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
AtRndmGenSvc = Service( "AtRndmGenSvc" )
AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532",
		      "TAUOLA 1048027510 345500367","TAUOLA_INIT 920021 3347532", 
		      "PHOTOS 2045027510 245500367","PHOTOS_INIT 930021 3447532"]
# AtRndmGenSvc.ReadFromFile = true;
Pythia = Algorithm( "Pythia" )
Pythia.PythiaCommand = ["pysubs msel 0",
			      "pysubs msub 1 1",  
                              "pysubs ckin 1 60.",
                              "pydat3 mdme 174 1 0",
                              "pydat3 mdme 175 1 0",
                              "pydat3 mdme 176 1 0",
                              "pydat3 mdme 177 1 0",
                              "pydat3 mdme 178 1 0",
                              "pydat3 mdme 179 1 0",
                              "pydat3 mdme 182 1 0",
                              "pydat3 mdme 183 1 0",
                              "pydat3 mdme 184 1 0",
                              "pydat3 mdme 185 1 0",
                              "pydat3 mdme 186 1 1",
                              "pydat3 mdme 187 1 0",
                              "pydat3 mdcy 15 1 0",
			      "pyinit pylistf 1",
			      "pyinit dumpr 1 5"]

Tauola = Algorithm( "Tauola" )
Tauola.TauolaCommand = [        "tauola polar 1",
			        "tauola radcor 1",
			        "tauola phox 0.01",
			        "tauola dmode 0",
			        "tauola jak1 0",
			        "tauola jak2 0"]

# DEFAULT GAMPRT
Tauola.TauolaCommand +=[        "usergamprt usergamprt 1 0.1800",
                                "usergamprt usergamprt 2 0.1751",
                                "usergamprt usergamprt 3 0.1110",
                                "usergamprt usergamprt 4 0.2515",
                                "usergamprt usergamprt 5 0.1790",
                                "usergamprt usergamprt 6 0.0071",
                                "usergamprt usergamprt 7 0.0134",
                                "usergamprt usergamprt 8 0.0450",
                                "usergamprt usergamprt 9 0.0100",
                                "usergamprt usergamprt 10 0.0009",
                                "usergamprt usergamprt 11 0.0004",
                                "usergamprt usergamprt 12 0.0003",
                                "usergamprt usergamprt 13 0.0005",
                                "usergamprt usergamprt 14 0.0015",
                                "usergamprt usergamprt 15 0.0015",
                                "usergamprt usergamprt 16 0.0015",
                                "usergamprt usergamprt 17 0.0005",
                                "usergamprt usergamprt 18 0.0050",
                                "usergamprt usergamprt 19 0.0055",
                                "usergamprt usergamprt 20 0.0017",
                                "usergamprt usergamprt 21 0.0013",
                                "usergamprt usergamprt 22 0.0010"]


# radiate only the tau-s; neccessary!!
# check joboptions.Pythia.AtlasTauolaPhotos.* if
# you want to use Photos for general photon FSR!!

Photos = Algorithm( "Photos" )
Photos.PhotosCommand = [        "photos pmode 2",
			        "photos xphcut 0.01",
			        "photos alpha -1.",
			        "photos interf 1",
			        "photos isec 1",
                                "photos itre 0",
                                "photos iexp 0",
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


#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
#==============================================================
#
# End of job options file
#
###############################################################
