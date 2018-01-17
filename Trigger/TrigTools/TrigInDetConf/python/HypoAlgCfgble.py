# Copyright (C) 1995-2017 CERN for the benefit of the ATLAS collaboration

from TrigFTKHypo.TrigFTKHypoConf import TrigFTKAvailableHypo

class TrigFTKAvailable(TrigFTKAvailableHypo):
  def __init__(self, name="TrigFTKAvailable_Muon_FTK", type="muon"):

    TrigFTKAvailableHypo.__init__(self, name)
