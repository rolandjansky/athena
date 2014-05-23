from AthenaCommon.Logging import logging
mlog = logging.getLogger( 'RDOtoESD_Lucid: ' )

from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.RecFlags import rec
from RecExConfig.ObjKeyStore import objKeyStore

if rec.doESD: 
  if DetFlags.readRDOPool.Lucid_on():
    include("LUCID_RawDataByteStreamCnv/LUCID_DigitRawDataCnv.py")
  if globalflags.DataSource()=='geant4':
    objKeyStore.addStreamESD("LUCID_DigitContainer", "Lucid_Digits")
  objKeyStore.addStreamESD("LUCID_RawDataContainer", "Lucid_RawData")
