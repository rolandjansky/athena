# Author: Michel Trottier-McDonald
#
# Date:   June 2009
#
# Last Modified by: Jyothsna Komaragiri on 7th July 2009
#
# Athena Jop options file for FindFactors Athena algorithm

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

### Event selector
import AthenaPoolCnvSvc.ReadAthenaPool

### to read a list of ESD/AOD
fileIN = open("list_AntiKt.txt", "r") 
line = fileIN.readline()
ServiceMgr.EventSelector.InputCollections.append(line.rstrip('\n'))
while line:
   line = fileIN.readline()
   ServiceMgr.EventSelector.InputCollections.append(line.rstrip('\n'))
   print line

### Read a single AOD input file
#ServiceMgr.EventSelector.InputCollections = ['/cluster/data01/michel/data/15.0.1/AODs/AOD.Ztt.AntiKt4H1Topo.01.root']

from TauTools.TauToolsConf import FindFactors
job += FindFactors()

job.FindFactors.TruthParticleContainer  = "SpclMC" #######"INav4MomTruthEvent" for ESD
job.FindFactors.TauJetContainer         = "TauRecContainer"
job.FindFactors.DeltaRMatchCut          = 0.2

job.FindFactors.ROOTFileName            = "TauCalibrationTools.root"

job.FindFactors.OutputLevel             = INFO

theApp.EvtMax = 500
