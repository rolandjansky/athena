# Remove all objects that were copied to the temporary background
# storegate BkgEvent_2_SG
# author: Piyali Banerjee Piyali.Banerjee@cern.ch 
# date: February 2011

from OverlayCommonAlgs.OverlayCommonAlgsConf import RemoveTempBkgObjects

if not "topSequence" in dir():    
   from AthenaCommon.AlgSequence import AlgSequence
   topSequence = AlgSequence()

topSequence += RemoveTempBkgObjects('RemoveTempBkgObjects')
topSequence.RemoveTempBkgObjects.TempBkgStore = 'StoreGateSvc/BkgEvent_2_SG'
topSequence.RemoveTempBkgObjects.BackgroundIsData = readBS
