###############################################################
#
# JOB OPTIONS FILE. THIS FILE IS TO DEMONSTRATE HOW THE USER CAN
# CHANGE THE PYTHIA PARAMETERS. IT DOES NOT NECESARILY COMPLY TO
# THE ATLAS DEFAULTS FOR PHYSICS EVENT GENERATION.
#==============================================================
#--------------------------------------------------------------
# Private Application Configuration option
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = DEBUG

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1000
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["HERWIG 390020611 821000366",
                                 "HERWIG_INIT 820021 2347532"]

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()
from Herwig_i.Herwig_iConf import Herwig
job +=Herwig()

include("herwig.common_14TeV.py")
include("herwig.top.py")

#sj#from TruthExamples.TruthExamplesConf import DumpMC
#sj#job += DumpMC()

# start my own
#from HepMCAnalysis_i.HepMCAnalysis_iConf import HepMCAnalysis
from HepMCAnalysis_i.HepMCAnalysis_iConfig import HepMCAnalysis_i

myHepMCAnalysis = HepMCAnalysis_i("HepMCAnalysis_i", file = 'Herwig_Process_ttbar_test.root')
#myHepMCAnalysis.JetAnalysis=True
#myHepMCAnalysis.WplusJetAnalysis=True
#myHepMCAnalysis.ZAnalysis=True
#myHepMCAnalysis.ZtautauAnalysis=True
#myHepMCAnalysis.WtaunuAnalysis=True
myHepMCAnalysis.ttbarAnalysis=True
#myHepMCAnalysis.bbbarAnalysis=True
#myHepMCAnalysis.UEAnalysis=True
#myHepMCAnalysis.EtmissAnalysis=True
#myHepMCAnalysis.ElasScatAnalysis=True
#myHepMCAnalysis.UserAnalysis=True

job += myHepMCAnalysis
# end my own

#
# End of job options file
#
###############################################################
