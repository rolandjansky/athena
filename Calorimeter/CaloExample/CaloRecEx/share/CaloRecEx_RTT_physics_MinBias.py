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

runArgs.maxEvents = 100

#runArgs.conditionsTag = 'COMCOND-REPC-002-03'
runArgs.conditionsTag = 'COMCOND-REPC-000-00'

# Input file that contains BSs
runArgs.inputBSFile = ["/afs/cern.ch/user/c/cote/gencomm/NightlyTestInput/data09_900GeV.00141749.physics_MinBias.merge.RAW._lb0092._0001.1_10evts._0001.data" ]

# Configuration fragment for DPD.
#runArgs.autoConfiguration = ['FieldAndGeo', 'BeamType', 'ConditionsTag']

# Basic string
#runArgs.trigStream = 'IDCosmic'
runArgs.trigStream = 'MinBias'

runArgs.preExec = ['from RecExConfig.RecFlags  import rec','rec.doMonitoring=False','rec.doInDet=False','rec.doMuon=False','rec.doMuonCombined=False','rec.doEgamma=False','rec.doTau=False','rec.doTrigger=False','from LArROD.LArRODFlags import larRODFlags','larRODFlags.readDigits.set_Value_and_Lock(False)','doTopoClusterCBNT=True']

# Configuration fragment for DPD.
runArgs.preInclude = ['RecExCommon/RecoUsefulFlags.py', 'RecExCommission/RecExCommissionRepro.py', 'RecExCond/MinimalCommissioningSetup.py', 'RecJobTransforms/UseOracle.py']

# Configuration fragment for DPD.
#runArgs.postInclude = ['RecJobTransforms/reducedRegionSelectorPostConfig.py']

runArgs.postExec = ['from CaloRec.CaloRecConf import CBNTAA_CaloCluster','theCBNT_CaloClusterTopo=CBNTAA_CaloCluster("CBNT_CaloClusterTopo")','theCBNT_CaloClusterTopo.MaxCaloCluster = 2000']

# Outputfiles that contains ntuple
runArgs.outputCBNTFile = 'ntuple.root'

# 
runArgs.loglevel = 'INFO'

include ('RecJobTransforms/skeleton.RAWtoESD.py')

#print memory usage for all events (in case of memory leak crash)
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

print 'BEGIN_RTTINFO = MinBias_physics_MinBias'
import os
com="echo \"BEGIN_RTTINFO = MinBias_physics_MinBias\">&MYRTTINFOS.txt"
os.system(com)
