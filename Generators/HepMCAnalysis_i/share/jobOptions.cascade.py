###############################################################
#
# JOB OPTIONS FILE. THIS FILE IS TO DEMONSTRATE HOW THE USER CAN
# CHANGE THE PYTHIA PARAMETERS. IT DOES NOT NECESARILY COMPLY TO
# THE ATLAS DEFAULTS FOR PHYSICS EVENT GENERATION.
#==============================================================
#--------------------------------------------------------------
import os
import random
from time import time

pid = os.getpid()
date = time()

seed = date*pid
random.seed(seed)

def randi():
    return str(random.randint(0,99999999))

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
ServiceMgr.AtRndmGenSvc.Seeds = ["CASCADE "+randi()+" "+randi(),
                                   "CASCADE_INIT "+randi()+" "+randi(),
                                   "PYTHIA "+randi()+" "+randi(),
                                   "PYTHIA_INIT "+randi()+" "+randi()]

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()
from Cascade_i.Cascade_iConf import Cascade
job += Cascade()

include("cascade.top.py")

from TruthExamples.TruthExamplesConf import DumpMC
job += DumpMC()

# start my own
#from HepMCAnalysis_i.HepMCAnalysis_iConf import HepMCAnalysis
from HepMCAnalysis_i.HepMCAnalysis_iConfig import HepMCAnalysis_i

myHepMCAnalysis = HepMCAnalysis_i("HepMCAnalysis_i", file = 'Cascade_Process_ttbar_test.root')
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
