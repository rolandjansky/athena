# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

def CaloClusterBadChannelListCorr() :

      theCaloBadChannelTool = CompFactory.CaloBadChanTool() # CaloBadChannelTool

      corr = CompFactory.CaloClusterBadChannelList() # CaloClusterCorrection
      corr.badChannelTool = theCaloBadChannelTool
      return corr
