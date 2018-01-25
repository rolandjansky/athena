# Run arguments file auto-generated on Tue Jan 23 09:57:00 2018 by:
# JobTransform: RAWtoESD
# Version: $Id: trfExe.py 792052 2017-01-13 13:36:51Z mavogel $
# Import runArgs class
from PyJobTransforms.trfJobOptions import RunArguments
runArgs = RunArguments()
runArgs.trfSubstepName = 'RAWtoESD' 

runArgs.preExec = ['rec.doTrigger=False; rec.doTau=False ; from RecExConfig.RecAlgsFlags import recAlgs; recAlgs.doMuonSpShower=False ; rec.doBTagging=False ; recAlgs.doEFlow=False ; recAlgs.doEFlowJet=False ; recAlgs.doMissingET=False ; recAlgs.doMissingETSig=False ; from JetRec.JetRecFlags import jetFlags ; jetFlags.Enabled=False ; ']
runArgs.autoConfiguration = ['everything']

# Explicitly added to process all events in this step
runArgs.maxEvents = -1

# Input data
runArgs.inputRDOFile = ['/afs/.cern.ch/work/m/mscodegg/public/QualiTask/mc16_13TeV.423001.ParticleGun_single_photon_egammaET.recon.RDO.e3566_s3113_r9388/RDO.11223039._000027.pool.root.1']
runArgs.inputRDOFileType = 'RDO'
runArgs.inputRDOFileNentries = 2000L
runArgs.RDOFileIO = 'input'

# Output data
runArgs.outputESDFile = 'tmp.ESD'
runArgs.outputESDFileType = 'ESD'

# Extra runargs

# Extra runtime runargs

# Literal runargs snippets
