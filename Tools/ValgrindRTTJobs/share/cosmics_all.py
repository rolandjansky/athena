# Run arguments file auto-generated on Tue Jun 16 00:11:03 2009 by:
# JobTransform: RAWtoESD
# Version: RecJobTransforms-00-08-26

# Authors: David Cote <david.cote@cern.ch>

#################################################################################
from PyJobTransformsCore.runargs import RunArguments

runArgs = RunArguments()

# Input file that contains BSs
runArgs.inputBSFile = ['/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/CosmicATN/daq.ATLAS.0091900.physics.IDCosmic.LB0001.SFO-1._0001.10EVTS.data']

# Basic string
runArgs.trigStream = 'IDCosmic'

# List of strings separated by commas
runArgs.autoConfiguration = ['FieldAndGeo', 'BeamType', 'ConditionsTag']

# Maximum number of events to process
runArgs.maxEvents = 2

# List of strings separated by commas
runArgs.preInclude = ['RecExCommission/MinimalCommissioningSetup.py']

# List of strings separated by commas
runArgs.postInclude = ['RecJobTransforms/reducedRegionSelectorPostConfig.py', 'RecExCommission/RecExCommission_RemoveTRTBSErr.py']

# Output file that contains ESD's
runArgs.outputESDFile = 'myESD.pool.root'

# Transfrom a string into generic multi-line python fragment where each line is an element in a list.
runArgs.preExec = ['rec.doTrigger=False', 'rec.doPerfMon=False', 'rec.doWriteESD.unlock()', 'rec.doWriteESD.set_Value_and_Lock(False)', 'rec.doWriteESD.lock()', 'rec.doEdmMonitor.set_Value_and_Lock(False)' , 'rec.doJetMissingETTag=False' ]

runArgs.postExec = ['aas.TimeOut=0*Units.s']
# 
runArgs.loglevel = 'INFO'

include ("RecJobTransforms/skeleton.RAWtoESD.py")

