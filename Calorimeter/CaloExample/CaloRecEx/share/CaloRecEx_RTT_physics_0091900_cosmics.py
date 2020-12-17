import os

print ('Hostname :')
os.system('hostname')
print ('CPU infos :')
os.system('cat /proc/cpuinfo')
print ('MEM infos :')
os.system('cat /proc/meminfo')

# Run arguments file auto-generated on Mon Mar 16 15:14:57 2009 by:
# JobTransform: RAWtoESD
# Version: RecJobTransforms-00-08-08

# Authors: David Cote <david.cote@cern.ch>

#################################################################################

from PyJobTransformsCore.runargs import RunArguments
runArgs = RunArguments()

# Maximum number of events to process
runArgs.maxEvents = 150

runArgs.conditionsTag = 'COMCOND-REPC-002-03'

# Input file that contains BSs
runArgs.inputBSFile = ["root://castoratlas//castor/cern.ch/grid/atlas/DAQ/2008/91900/physics_IDCosmic/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0002.data"]

# Configuration fragment for DPD.
#runArgs.autoConfiguration = ['FieldAndGeo', 'BeamType', 'ConditionsTag']

# Basic string
runArgs.trigStream = 'IDCosmic'

runArgs.preExec = ['from RecExConfig.RecFlags  import rec','rec.doMonitoring=False','rec.doInDet=False','rec.doMuon=False','rec.doMuonCombined=False','rec.doEgamma=False','rec.doTau=False','rec.doTrigger=False','doTopoClusterCBNT=True','rec.doFloatingPointException=False']

# Configuration fragment for DPD.
runArgs.preInclude = ['RecExCommon/RecoUsefulFlags.py', 'RecExCommission/RecExCommissionRepro.py', 'RecExCond/MinimalCommissioningSetup.py', 'RecJobTransforms/UseOracle.py']

runArgs.postExec = ['from CaloRec.CaloRecConf import CBNTAA_CaloCluster','theCBNT_CaloClusterTopo=CBNTAA_CaloCluster("CBNT_CaloClusterTopo")','theCBNT_CaloClusterTopo.MaxCaloCluster = 2000']

# Outputfiles that contains ntuple
runArgs.outputCBNTFile = 'ntuple.root'

#
runArgs.loglevel = 'INFO'

include ('RecJobTransforms/skeleton.RAWtoESD.py')

#print memory usage for all events (in case of memory leak crash)
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

print 'BEGIN_RTTINFO = muons_physics_0091900_cosmics'
import os
com="echo \"BEGIN_RTTINFO = muons_physics_0091900_cosmics\">&MYRTTINFOS.txt"
os.system(com)
