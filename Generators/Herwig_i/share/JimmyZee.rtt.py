###############################################################
#
# Job options file
#
#==============================================================
# Private Application Configuration option
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = DEBUG

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 5
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["JIMMY 390020611 821000366", "JIMMY_INIT 820021 2347532"]

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()
from Herwig_i.Herwig_iConf import Herwig
job +=Herwig()
job.Herwig.HerwigCommand = [ "iproc 11351",
                       "modpdf 10042",
                       "autpdf HWLHAPDF",
                       "emmin 60.",
                       "msflag 1",
                       "jmbug 0",
                       "jmueo 1",
                       "ptjim 4.91",
                       "jmrad 73 1.8",
                       "pltcut 0.0000000000333",
                       "ptmin 10.",
                       "prsof 0",
                       "rmass 198 80.425",
                       "rmass 199 80.425",
                       "rmass 200 91.19",
                       "gamw 2.124",
                       "gamz 2.495",
                       "taudec TAUOLA",
                       "clpow 1.20"]
                       
from Tauola_i.Tauola_iConf import Tauola
job +=Tauola()
job.Tauola.TauolaCommand = [    "tauola polar 1",
                                "tauola radcor 1",
                                "tauola phox 0.01",
                                "tauola dmode 0",
                                "tauola jak1 0",
                                "tauola jak2 0"]
                                
from Photos_i.Photos_iConf import Photos
job +=Photos()
job.Photos.PhotosCommand = [ "photos pmode 1",
                             "photos xphcut 0.01",
                             "photos alpha -1.",
                             "photos interf 1",
                             "photos isec 1",
                             "photos itre 0",
                             "photos iexp 1",
                             "photos iftop 0"]

from GeneratorFilters.GeneratorFiltersConf import LeptonFilter
job +=LeptonFilter()
job.LeptonFilter.Ptcut = 10000.
job.LeptonFilter.Etacut = 2.7

from TruthExamples.TruthExamplesConf import DumpMC
job += DumpMC()

#==============================================================
#
# End of job options file
#
###############################################################
