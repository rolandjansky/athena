# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinderBase

class TrigFastTrackFinder(TrigFastTrackFinderBase):
  def __init__(self, name="TrigTrackFinder_Muon", type="muon"):
    remap  = {
      "muon"     : "Muon",
      "muonCore" : "MuonCore",
      "muonIso"  : "MuonIso",
      "electron" : "eGamma",
      "tau"      : "Tau",
      "tauCore"  : "TauCore",
      "tauIso"   : "TauIso",
      "bjet"     : "Jet",
      "bjetVtx"  : "Jet",
      "fullScan" : "FullScan",
      "beamSpot" : "BeamSpot",
      "bphysics" : "Bphysics",
      "cosmics"  : "Cosmic",
      }

    #this should not happen here
    hacktype = type
    if 'Core' in name and 'Core' not in type:
        hacktype = type+'Core'
    elif 'Iso' in name and 'Iso' not in type:
        hacktype = type+'Iso'

    #more type hacking
    conftype = remap[hacktype]    
    if name.find('TrigFastTrackFinder_FTK')>-1:
      import re
      conftype = re.search("FTK.*$",name).group(0)
        
    TrigFastTrackFinderBase.__init__(self, name, conftype)
    

from InDetTrigPrepRawDataFormat.InDetTrigPrepRawDataFormatConf import InDet__FTK_TrackMaker

class FTK_TrackMaker(InDet__FTK_TrackMaker):
  def __init__(self, name="TrackMaker_Muon_FTK", type="muon"):

    InDet__FTK_TrackMaker.__init__(self, name)


