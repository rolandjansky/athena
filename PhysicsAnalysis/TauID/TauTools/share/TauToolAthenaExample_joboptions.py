# Author: Jyothsna Komaragiri and Dugan O'Neil
# Date:   September 2007.
# Athena Jop options file to run on 14.x.x samples

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool

# Read a single AOD input file
ServiceMgr.EventSelector.InputCollections = ['/cluster/data01/jyothsna/005188tid006904/AOD.pool_0_0.root']

##############################
### uncomment the below to read a list of AODs
#fileIN = open("list_5188_5107_e322_s461_r500_AOD.txt", "r")
#line = fileIN.readline()
#ServiceMgr.EventSelector.InputCollections.append(line.rstrip('\n'))
#while line:
#  line = fileIN.readline()
#  ServiceMgr.EventSelector.InputCollections.append(line.rstrip('\n'))
#  print line
##############################

#
# The following lines take care of tau EDM backwards compatibility
#
from TauTools.TauToolsConf import TauCommonDetailsMerger
job += TauCommonDetailsMerger("TauCommonDetailsMerger",
                              SkipExtraDetails=False)# Set false for  AOD's (since there are no extra details)

# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
job.TauCommonDetailsMerger.OutputLevel = INFO

# Add top algorithms to be run
from TauTools.TauToolsConf import TauToolAthenaExample
job += TauToolAthenaExample()

job.TauToolAthenaExample.MCParticleContainer  = "SpclMC" #######"INav4MomTruthEvent" for ESD
job.TauToolAthenaExample.TauJetContainer      = "TauRecContainer"
job.TauToolAthenaExample.DeltaRCut            = 0.2

job.TauToolAthenaExample.OutputLevel  = INFO #DEBUG

##########################################
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc = Service( "MessageSvc" )
ServiceMgr.MessageSvc.OutputLevel = ERROR

# Number of Events to process
theApp.EvtMax = -1
# theApp.EvtMax = 10

###################### Detail time measurement and auditors
# Use auditors

theApp.AuditAlgorithms=True
theApp.Dlls += [ "GaudiAud" ]

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("ChronoAuditor")()

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE

#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = True 
#ServiceMgr.MessageSvc.OutputLevel = DEBUG
