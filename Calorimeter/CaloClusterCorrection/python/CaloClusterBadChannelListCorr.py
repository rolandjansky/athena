# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def CaloClusterBadChannelListCorr() :

      from CaloBadChannelTool.CaloBadChannelToolConf import CaloBadChanTool 
      theCaloBadChannelTool = CaloBadChanTool()

      from CaloClusterCorrection.CaloClusterCorrectionConf import CaloClusterBadChannelList
      corr = CaloClusterBadChannelList()
      #ToolSvc += corr
      corr.badChannelTool = theCaloBadChannelTool
      return corr
