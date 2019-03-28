# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#
# Import MDT_Digitization job properties
#
from Digitization.DigitizationFlags import jobproperties
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ToolSvc, ServiceMgr

# The earliest bunch crossing time for which interactions will be sent
# to the MdtDigitizationTool.
def MDT_FirstXing():
    return -800

# The latest bunch crossing time for which interactions will be sent
# to the MdtDigitizationTool.
def MDT_LastXing():
    # was 800 for large time window
    return 150

def MdtDigitizationTool(name="MdtDigitizationTool",**kwargs):
   kwargs.setdefault("RndmSvc", jobproperties.Digitization.rndmSvc() )
   kwargs.setdefault("TwinRndmSvc", jobproperties.Digitization.rndmSvc())
   kwargs.setdefault("MaskedStations", [])
   kwargs.setdefault("UseDeadChamberSvc", True)
   kwargs.setdefault("DiscardEarlyHits", True)
   
   kwargs.setdefault("UseTof", jobproperties.Beam.beamType != "cosmics")
   
   if 'RT_Relation_DB_DigiTool' in jobproperties.Digitization.experimentalDigi():
      kwargs.setdefault("DigitizationTool","RT_Relation_DB_DigiTool")
   else:
      kwargs.setdefault("DigitizationTool","MDT_Response_DigiTool")

   if jobproperties.Digitization.specialConfiguration.statusOn:
      specialConfigDict = jobproperties.Digitization.specialConfiguration.get_Value()
      if 'MDT_QballConfig' in specialConfigDict.keys():
         kwargs.setdefault("DoQballCharge", (specialConfigDict['MDT_QballConfig'] == 'True'))
      else:
         kwargs.setdefault("DoQballCharge", False)
   else:
      kwargs.setdefault("DoQballCharge", False)

   if 'NewMerge' in jobproperties.Digitization.experimentalDigi():
       kwargs.setdefault("UseMcEventCollectionHelper",True)
   else:
       kwargs.setdefault("UseMcEventCollectionHelper",False)
   
   mdtRndm = kwargs.setdefault("RndmEngine","MDT_Digitization")
   mdtTwinRndm = kwargs.setdefault("TwinRndmEngine","MDT_DigitizationTwin")
   jobproperties.Digitization.rndmSeedList.addSeed(mdtRndm, 49261510, 105132394 )
   jobproperties.Digitization.rndmSeedList.addSeed(mdtTwinRndm, 393242561, 857132381 )

   if jobproperties.Digitization.doXingByXingPileUp():
      kwargs.setdefault("FirstXing", MDT_FirstXing() ) # this should match the range for the MDT in Digitization/share/MuonDigitization.py
      kwargs.setdefault("LastXing",  MDT_LastXing() )  # this should match the range for the MDT in Digitization/share/MuonDigitization.py

   return CfgMgr.MdtDigitizationTool(name,**kwargs)
      #return CfgMgr.MDT_PileUpTool(name,**kwargs)
   #else:
   #   return CfgMgr.MdtDigitizationTool(name,**kwargs)


def getMdtRange(name="MdtRange", **kwargs):
    # bunch crossing range in ns
    kwargs.setdefault('FirstXing', MDT_FirstXing() )
    kwargs.setdefault('LastXing',  MDT_LastXing() )
    kwargs.setdefault('CacheRefreshFrequency', 1.0 ) #default 0 no dataproxy reset
    kwargs.setdefault('ItemList', ["MDTSimHitCollection#MDT_Hits"] )
    return CfgMgr.PileUpXingFolder(name, **kwargs)


def RT_Relation_DB_DigiTool(name="RT_Relation_DB_DigiTool",**kwargs):
   global ToolSvc, ServiceMgr
   kwargs.setdefault("RndmSvc", jobproperties.Digitization.rndmSvc())
   mdtRndm = kwargs.setdefault("RndmEngine", "RTRelationDB")
   jobproperties.Digitization.rndmSeedList.addSeed(mdtRndm, 49261510,105132394 )
   
   return CfgMgr.RT_Relation_DB_DigiTool(name,**kwargs)


def MDT_Response_DigiTool(name="MDT_Response_DigiTool",**kwargs):
   kwargs.setdefault("RndmSvc", jobproperties.Digitization.rndmSvc())
   mdtRndm = kwargs.setdefault("RndmEngine", "MDTResponse")
   
   if jobproperties.Digitization.specialConfiguration.statusOn:
      specialConfigDict = jobproperties.Digitization.specialConfiguration.get_Value()
      if 'MDT_QballConfig' in specialConfigDict.keys():
         kwargs.setdefault("DoQballGamma", (specialConfigDict['MDT_QballConfig'] == 'True'))
      else:
         kwargs.setdefault("DoQballGamma", False)
   else:
      kwargs.setdefault("DoQballGamma", False)
   
   jobproperties.Digitization.rndmSeedList.addSeed(mdtRndm, 49261510,105132394 )
   return CfgMgr.MDT_Response_DigiTool(name,**kwargs)
   
def Mdt_OverlayDigitizationTool(name="Mdt_OverlayDigitizationTool",**kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("EvtStore", overlayFlags.evtStore())
    kwargs.setdefault("GetT0FromBD", overlayFlags.isDataOverlay())
    return MdtDigitizationTool(name,**kwargs)
