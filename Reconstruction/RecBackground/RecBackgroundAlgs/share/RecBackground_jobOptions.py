include.block ('RecBackgroundAlgs/RecBackground_jobOptions.py')

from RecExConfig.RecFlags import rec
from AthenaCommon.DetFlags import DetFlags
if rec.doInDet() and rec.doMuon() and rec.doCalo() and \
    DetFlags.detdescr.Muon_on() and DetFlags.detdescr.Calo_on() and DetFlags.detdescr.ID_on() :
  from RecBackgroundAlgs.RecBackgroundAlgsConf import BeamBackgroundFiller
  BeamBackgroundFiller=BeamBackgroundFiller()
  topSequence+=BeamBackgroundFiller

  from BCM_BackgroundAlgs.BCM_BackgroundAlgsConf import BcmCollisionTimeAlg
  BcmCollisionTimeAlg=BcmCollisionTimeAlg()
  topSequence+=BcmCollisionTimeAlg

  from RecBackgroundAlgs.RecBackgroundAlgsConf import BackgroundWordFiller
  BackgroundWordFiller=BackgroundWordFiller()
  topSequence+=BackgroundWordFiller
