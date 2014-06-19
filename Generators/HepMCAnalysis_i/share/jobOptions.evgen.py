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

## load pool support
import AthenaPoolCnvSvc.ReadAthenaPool

## input
#svcMgr.EventSelector.InputCollections = [ inputFile ]
# example
#svcMgr.EventSelector.InputCollections = [ '/atlas/discs/pool02/sjohnert/evgen/mc08.105411.GMSB2_jimmy_susy.evgen.EVNT.e352_tid024612/EVNT.024612._00001.pool.root.3', '/atlas/discs/pool02/sjohnert/evgen/mc08.105411.GMSB2_jimmy_susy.evgen.EVNT.e352_tid024612/EVNT.024612._00002.pool.root.3' ]
svcMgr.EventSelector.InputCollections = [ '/afs/cern.ch/user/n/nkondras/Files/mc12_8TeV.107652.AlpgenJimmy_AUET2CTEQ6L1_ZeeNp2.evgen.EVNT.e1571_tid01025118_00/EVNT.01025118._000076.pool.root.1' ]
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10, -1 is all)
theApp.EvtMax = -1
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
# ServiceManager.AtRndmGenSvc.ReadFromFile = true;

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()

### include print MC
#from TruthExamples.TruthExamplesConf import PrintMC
#topSequence += PrintMC()

### include dump MC
#from TruthExamples.TruthExamplesConf import DumpMC
#job += DumpMC()

# start my own
#from HepMCAnalysis_i.HepMCAnalysis_iConf import HepMCAnalysis
from HepMCAnalysis_i.HepMCAnalysis_iConfig import HepMCAnalysis_i

myHepMCAnalysis = HepMCAnalysis_i("HepMCAnalysis_i", file = 'EvGen_test.root')
myHepMCAnalysis.JetAnalysis=False
myHepMCAnalysis.WplusJetAnalysis=False
myHepMCAnalysis.ZAnalysis=False
myHepMCAnalysis.ZtautauAnalysis=False
myHepMCAnalysis.WtaunuAnalysis=False
myHepMCAnalysis.ttbarAnalysis=False
myHepMCAnalysis.bbbarAnalysis=False
myHepMCAnalysis.UEAnalysis=False
myHepMCAnalysis.EtmissAnalysis=False
myHepMCAnalysis.ElasScatAnalysis=False
#myHepMCAnalysis.UserAnalysis=True

myHepMCAnalysis.ParticleContentAnalysis=True
myHepMCAnalysis.LeptonJetAnalysis=True
myHepMCAnalysis.PdfAnalysis=True

job += myHepMCAnalysis
# end my own

#
# End of job options file
#
###############################################################
