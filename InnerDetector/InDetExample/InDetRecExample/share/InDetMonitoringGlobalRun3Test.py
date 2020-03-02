#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
@file InDetMonitoringGlobalRun3Test.py
@brief Top configuration of InDetMonitoringGlobalRun3Test in Run 3 style but in Run 2 environment
'''

doInDetGlobalTrackMonAlg           = True
doInDetGlobalPrimaryVertexMonAlg   = True

from InDetGlobalMonitoringRun3Test.InDetGlobalMonitoringRun3TestConf import InDetGlobalTrackMonAlg
from InDetGlobalMonitoringRun3Test.InDetGlobalTrackMonAlgCfg import InDetGlobalTrackMonAlgCfg

from InDetGlobalMonitoringRun3Test.InDetGlobalMonitoringRun3TestConf import InDetGlobalPrimaryVertexMonAlg
from InDetGlobalMonitoringRun3Test.InDetGlobalPrimaryVertexMonAlgCfg import InDetGlobalPrimaryVertexMonAlgCfg


from InDetRecExample.InDetKeys import InDetKeys                                                                                     


kwargsInDetGlobalTrackMonAlg = {
    'DoIBL' : True,           #InDetFlags.doIBL(), #Turn on/off IBL histograms
    'TrackName'  : 'CombinedInDetTracks',  #InDetKeys.Tracks()
    'TrackName2' : 'CombinedInDetTracks',  #
    'TrackName3' : 'CombinedInDetTracks',  #
}

kwargsInDetGlobalPrimaryVertexMonAlg = { 
    'vxContainerName'                      : 'PrimaryVertices', #InDetKeys.xAODVertexContainer(),
    'vxContainerNameWithOutBeamConstraint' : 'VxPrimaryCandidateWithBeamConstraint', #InDetKeys.PrimaryVerticesWithoutBeamConstraint(),
    'vxContainerNameSplit'                 : 'VxPrimaryCandidateSplitStream', #InDetKeys.PrimaryVerticesSplitStream(),
    'doEnhancedMonitoring'                 : False # InDetFlags.doMonitoringPrimaryVertexingEnhanced()
}

from AthenaMonitoring.DQMonFlags import DQMonFlags

# old magic
from AthenaMonitoring import AthMonitorCfgHelperOld
helper = AthMonitorCfgHelperOld(DQMonFlags, "InDetGlobalMonitoringRun3Test")


if doInDetGlobalTrackMonAlg:
  inDetGlobalTrackMonAlg = helper.addAlgorithm(InDetGlobalTrackMonAlg, 'InDetGlobalTrackMonAlg')
  for k, v in kwargsInDetGlobalTrackMonAlg.items():
      setattr(inDetGlobalTrackMonAlg, k, v)

  inDetGlobalTrackMonAlg.TrackSelectionTool.UseTrkTrackTools = True
  inDetGlobalTrackMonAlg.TrackSelectionTool.CutLevel         = "TightPrimary"
  inDetGlobalTrackMonAlg.TrackSelectionTool.maxNPixelHoles   = 1
  inDetGlobalTrackMonAlg.TrackSelectionTool.minPt            = 5000
  inDetGlobalTrackMonAlg.TrackSelectionTool.TrackSummaryTool = InDetTrackSummaryTool
  inDetGlobalTrackMonAlg.TrackSelectionTool.Extrapolator     = InDetExtrapolator

  inDetGlobalTrackMonAlg.Tight_TrackSelectionTool.UseTrkTrackTools = True
  inDetGlobalTrackMonAlg.Tight_TrackSelectionTool.CutLevel         = "TightPrimary"
  inDetGlobalTrackMonAlg.Tight_TrackSelectionTool.minPt            = 5000
  inDetGlobalTrackMonAlg.Tight_TrackSelectionTool.TrackSummaryTool = InDetTrackSummaryTool
  inDetGlobalTrackMonAlg.Tight_TrackSelectionTool.Extrapolator     = InDetExtrapolator

  InDetGlobalTrackMonAlgCfg(helper, inDetGlobalTrackMonAlg, **kwargsInDetGlobalTrackMonAlg)

if doInDetGlobalPrimaryVertexMonAlg:
  myInDetGlobalPrimaryVertexMonAlg = helper.addAlgorithm(InDetGlobalPrimaryVertexMonAlg, 'InDetGlobalPrimaryVertexMonAlg')
  for k, v in kwargsInDetGlobalPrimaryVertexMonAlg.items():
      setattr(myInDetGlobalPrimaryVertexMonAlg, k, v)
  InDetGlobalPrimaryVertexMonAlgCfg(helper, myInDetGlobalPrimaryVertexMonAlg, **kwargsInDetGlobalPrimaryVertexMonAlg)

topSequence += helper.result()
