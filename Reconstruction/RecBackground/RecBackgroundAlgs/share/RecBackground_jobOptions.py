include.block ('RecBackgroundAlgs/RecBackground_jobOptions.py')

from RecExConfig.RecFlags import rec
from AthenaCommon.DetFlags import DetFlags
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

if rec.doInDet() and rec.doMuon() and rec.doCalo() and \
    DetFlags.detdescr.Muon_on() and DetFlags.detdescr.Calo_on() and DetFlags.detdescr.ID_on() :
  include ("LArCellRec/LArCollisionTime_jobOptions.py")
  from RecBackgroundAlgs.RecBackgroundAlgsConf import BeamBackgroundFiller
  BeamBackgroundFiller=BeamBackgroundFiller(cscSegmentContainerKey=("NCB_TrackMuonSegments" if MuonGeometryFlags.hasCSC() else ""))
  topSequence+=BeamBackgroundFiller

  from BCM_BackgroundAlgs.BCM_BackgroundAlgsConf import BcmCollisionTimeAlg
  BcmCollisionTimeAlg=BcmCollisionTimeAlg()
  topSequence+=BcmCollisionTimeAlg

  from RecBackgroundAlgs.RecBackgroundAlgsConf import BackgroundWordFiller
  BackgroundWordFiller=BackgroundWordFiller()
  topSequence+=BackgroundWordFiller
  BackgroundWordFiller.IsMC = (globalflags.DataSource.get_Value() != 'data')
