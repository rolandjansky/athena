#Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
include.block("CaloMonitoring/RecBackgroundForCaloMon_jobOptions.py")

from RecExConfig.RecFlags import rec
from AthenaCommon.DetFlags import DetFlags
if rec.doInDet() and rec.doMuon() and rec.doCalo() and \
   DetFlags.detdescr.Muon_on() and DetFlags.detdescr.Calo_on() and DetFlags.detdescr.ID_on() :
   from RecBackgroundAlgs.RecBackgroundAlgsConf import BeamBackgroundFiller
   printfunc ("create BeamBackgroundFiller")
   BeamBackgroundFiller=BeamBackgroundFiller("CSCVetoForCaloMon")
   printfunc ("set cutThetaCsc = 0")
   BeamBackgroundFiller.cutThetaCsc = 0
   printfunc ("set cutThetaMdtI = 90")
   BeamBackgroundFiller.cutThetaMdtI = 90
   printfunc ("add to top sequence")
   BeamBackgroundFiller.BeamBackgroundKey="CSCBackgroundForCaloMon"
   topSequence+=BeamBackgroundFiller

