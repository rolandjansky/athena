# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinderBase

class TrigFastTrackFinder(TrigFastTrackFinderBase):
  def __init__(self, name="TrigTrackFinder_Muon", type="muon"):
    remap  = {
      "muon"     : "Muon",
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

    TrigFastTrackFinderBase.__init__(self, name, remap[hacktype])
    


