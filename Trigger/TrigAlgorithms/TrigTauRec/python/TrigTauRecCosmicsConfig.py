# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" TrigTauRecCosmics """

__author__  = 'S.Xella, O.Igonkina, S.Tsuno'
__version__=""
__doc__="Configuration of Cosmics TrigTauRec"

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *

from TrigTauRec.TrigTauRecConf import TrigTauRecMerged
from TrigTauRec.TrigTauRecConfig import TrigTauRecMerged_Tau
from TrigTauRec.TrigTauRecConfig import TrigTauRecMerged_Tau2012

from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
TauSelectExtrapolator = AtlasExtrapolator(name="TrigTauCosmicsExtrapolator")
TauSelectExtrapolator.DoCaloDynamic = False

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += TauSelectExtrapolator

class TrigTauRecCosmics_Tau (TrigTauRecMerged_Tau) :
        __slots__ = [ '_mytools']
        def __init__(self, name = "TrigTauRecCosmics_Tau"):
            super( TrigTauRecCosmics_Tau , self ).__init__( name )
            self.trkcone = 9999.0
            from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
            trigTauCosmicsTrackSelectTool = InDet__InDetDetailedTrackSelectorTool("TrigTauCosmicsTrackSelect")

            trigTauCosmicsTrackSelectTool.pTMin                = 1000.
            trigTauCosmicsTrackSelectTool.IPd0Max              = 9999.0 
            trigTauCosmicsTrackSelectTool.d0MaxPreselection    = 9999.0
            trigTauCosmicsTrackSelectTool.IPz0Max              = 9999.
            trigTauCosmicsTrackSelectTool.useTrackSummaryInfo  = True
            trigTauCosmicsTrackSelectTool.nHitBLayer           = 0
            trigTauCosmicsTrackSelectTool.nHitPix              = 0
            trigTauCosmicsTrackSelectTool.nHitSct              = 0
            trigTauCosmicsTrackSelectTool.nHitSi               = 0
            trigTauCosmicsTrackSelectTool.nHitTrt              = 0
            trigTauCosmicsTrackSelectTool.nHitTrtPlusOutliers  = 0
            trigTauCosmicsTrackSelectTool.TrtMaxEtaAcceptance  = 1.9
            trigTauCosmicsTrackSelectTool.useSharedHitInfo     = False
            trigTauCosmicsTrackSelectTool.useTrackQualityInfo  = True
            trigTauCosmicsTrackSelectTool.fitChi2OnNdfMax      = 9999.0 
            trigTauCosmicsTrackSelectTool.TrackSummaryTool     = None

            trigTauCosmicsTrackSelectTool.Extrapolator         = TauSelectExtrapolator


            from AthenaCommon.AppMgr import ToolSvc
            ToolSvc +=trigTauCosmicsTrackSelectTool
   
            self.Tools['TrigTau_TauTrackFinder'].TrackSelectorToolTau = trigTauCosmicsTrackSelectTool
            self.Tools['TrigTau_TauTrackFinder'].MaxJetDrTau = 9999.0   # CoreTrackDist
            self.Tools['TrigTau_TauTrackFinder'].MaxJetDrWide = 9999.0   # WideTrackDist

class TrigTauRecCosmics_Tau2012 (TrigTauRecMerged_Tau2012) :
        __slots__ = [ '_mytools']
        def __init__(self, name = "TrigTauRecCosmics_Tau2012"):
            super( TrigTauRecCosmics_Tau2012 , self ).__init__( name )
            self.trkcone = 9999.0
            from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
            trigTauCosmicsTrackSelectTool = InDet__InDetDetailedTrackSelectorTool("TrigTau2012CosmicsTrackSelect")

            trigTauCosmicsTrackSelectTool.pTMin                = 1000.
            trigTauCosmicsTrackSelectTool.IPd0Max              = 9999.0 
            trigTauCosmicsTrackSelectTool.d0MaxPreselection    = 9999.0
            trigTauCosmicsTrackSelectTool.IPz0Max              = 9999.
            trigTauCosmicsTrackSelectTool.useTrackSummaryInfo  = True
            trigTauCosmicsTrackSelectTool.nHitBLayer           = 0
            trigTauCosmicsTrackSelectTool.nHitPix              = 0
            trigTauCosmicsTrackSelectTool.nHitSct              = 0
            trigTauCosmicsTrackSelectTool.nHitSi               = 0
            trigTauCosmicsTrackSelectTool.nHitTrt              = 0
            trigTauCosmicsTrackSelectTool.nHitTrtPlusOutliers  = 0
            trigTauCosmicsTrackSelectTool.TrtMaxEtaAcceptance  = 1.9
            trigTauCosmicsTrackSelectTool.useSharedHitInfo     = False
            trigTauCosmicsTrackSelectTool.useTrackQualityInfo  = True
            trigTauCosmicsTrackSelectTool.fitChi2OnNdfMax      = 9999.0 
            trigTauCosmicsTrackSelectTool.TrackSummaryTool     = None

            trigTauCosmicsTrackSelectTool.Extrapolator         = TauSelectExtrapolator


            from AthenaCommon.AppMgr import ToolSvc
            ToolSvc +=trigTauCosmicsTrackSelectTool

            self.Tools['TrigTau_TauTrackFinder'].TrackSelectorToolTau = trigTauCosmicsTrackSelectTool
            self.Tools['TrigTau_TauTrackFinder'].MaxJetDrTau = 9999.0   # CoreTrackDist
            self.Tools['TrigTau_TauTrackFinder'].MaxJetDrWide = 9999.0   # WideTrackDist
            self.Tools['TrigTau_TauTrackFinder'].removeTracksOutsideZ0wrtLeadTrk = False
            #self.useTauPVTool = False # dont't print out deltaZ0 cuts


#end
