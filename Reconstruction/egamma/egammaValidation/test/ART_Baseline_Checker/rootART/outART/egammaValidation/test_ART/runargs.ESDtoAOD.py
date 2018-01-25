# Run arguments file auto-generated on Tue Jan 23 10:19:20 2018 by:
# JobTransform: ESDtoAOD
# Version: $Id: trfExe.py 792052 2017-01-13 13:36:51Z mavogel $
# Import runArgs class
from PyJobTransforms.trfJobOptions import RunArguments
runArgs = RunArguments()
runArgs.trfSubstepName = 'ESDtoAOD' 

runArgs.athenaopts = ['--preloadlib=/cvmfs/atlas-nightlies.cern.ch/repo/sw/21.0/2018-01-23T0022/AthenaExternals/21.0.61/InstallArea/x86_64-slc6-gcc62-opt/lib/libintlc.so.5:/cvmfs/atlas-nightlies.cern.ch/repo/sw/21.0/2018-01-23T0022/AthenaExternals/21.0.61/InstallArea/x86_64-slc6-gcc62-opt/lib/libimf.so']
runArgs.preExec = ['rec.doTrigger=False; rec.doTau=False ; from RecExConfig.RecAlgsFlags import recAlgs; recAlgs.doMuonSpShower=False ; rec.doBTagging=False ; recAlgs.doEFlow=False ; recAlgs.doEFlowJet=False ; recAlgs.doMissingET=False ; recAlgs.doMissingETSig=False ; from JetRec.JetRecFlags import jetFlags ; jetFlags.Enabled=False ; ']
runArgs.autoConfiguration = ['everything']

# Explicitly added to process all events in this step
runArgs.maxEvents = -1

# Input data
runArgs.inputESDFile = ['tmp.ESD']
runArgs.inputESDFileType = 'ESD'
runArgs.inputESDFileNentries = 2000L
runArgs.ESDFileIO = 'temporary'

# Output data
runArgs.outputAODFile = '/afs/cern.ch/work/m/mscodegg/public/QualiTask/athena/Reconstruction/egamma/egammaValidation/test/ART_Baseline_Checker/rootART/ART_Rec.root'
runArgs.outputAODFileType = 'AOD'

# Extra runargs

# Extra runtime runargs

# Literal runargs snippets
