# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

if 'runArgs' not in dir():
   from PyJobTransformsCore.runargs import RunArguments
   runArgs = RunArguments()

topSequence += CfgMgr.LArReadCells()
svcMgr += CfgMgr.THistSvc()
if hasattr(runArgs,"outputNTUP_SAMPLESMONFile"):
   svcMgr.THistSvc.Output += ["SPLASH DATAFILE='"+runArgs.outputNTUP_SAMPLESMONFile+"' OPT='RECREATE'"]
else:   
   svcMgr.THistSvc.Output += ["SPLASH DATAFILE='ntuple.root' OPT='RECREATE'"]
topSequence.LArReadCells.etCut = 7500. # was 1000. originally
topSequence.LArReadCells.etCut2 = 7500.

