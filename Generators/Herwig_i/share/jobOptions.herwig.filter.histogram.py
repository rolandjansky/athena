###############################################################
#
# Job options file
#
#==============================================================
#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

# make sure we are loading the ParticleProperty service
from PartPropSvc.PartPropSvcConf import PartPropSvc
ServiceMgr += PartPropSvc()

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()

from Herwig_i.Herwig_iConf import Herwig
from TruthExamples.TruthExamplesConf import  HistSample
from GeneratorFilters.GeneratorFiltersConf import ZtoLeptonFilter
from GaudiAlg.GaudiAlgConf import Sequencer

theApp.HistogramPersistency = "HBOOK"
job +=  Sequencer("Generator")
job.Generator.Members = ["Herwig", "ZtoLeptonFilter", "HistSample"]
# this single should do the same
#job +=  Sequencer("Generator",Members=["Herwig", "ZtoLeptonFilter", "HistSample"])

from GaudiSvc.GaudiSvcConf import HistogramPersistencySvc
ServiceMgr += HistogramPersistencySvc()
ServiceMgr.HistogramPersistencySvc.OutputFile  = "herwig.hbook"
ServiceMgr.NTupleSvc.Output     = [ "FILE1 DATAFILE='herwigtuple1.hbook' TYP='HBOOK' OPT='NEW'" ]

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel               = FATAL
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 11000

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
job += Herwig()
job.Herwig.HerwigCommand = ["iproc 2150", "modpdf 10042", "autpdf HWLHAPDF", "ptmin 100.", "ptmax 200.", "emmin 80.", "emmax 100.", "msflag 0"]

#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
#==============================================================
#
# End of job options file
#
###############################################################
