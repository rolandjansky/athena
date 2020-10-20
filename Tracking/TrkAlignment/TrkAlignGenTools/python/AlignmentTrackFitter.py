# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


## @file:   AlignTrackFitter.py
## @brief:  Used to create and configure the track fitter for alignment.
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009

from AthenaCommon.Logging import logging
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align

## AlignmentTrackFitter determines which type of fitter and configuration to 
#  use based on AlignmentFlags and creates the fitter 
class AlignmentTrackFitter () :

    _nInitialized = 0

    ## constructor
    def __init__(self):
        mlog = logging.getLogger( 'AlignmentTrackFitter::__init__' )
        self._nInitialized += 1
        if self._nInitialized == 1:
            mlog.debug(" : First instance->configure()" )
        else:
            mlog.debug("already initialized" )
            return
        
        fitterType = align.fitterType()
        if fitterType == 'iPat' :
            self.set_iPatFitter()
        elif fitterType == 'GlobalChi2' :
            self.setGlobalChi2Fitter()
        elif fitterType == 'MCTB' :
            self.setMCTBFitter()
        elif fitterType == 'CombMuonTrackFitter' :
            self.setCombMuonTrackFitter()
        else: 
            mlog.warning("no fitter!")
            return
        
    ## set CombMuonTrackFitter
    def setCombMuonTrackFitter(self) :
        mlog = logging.getLogger( 'AlignmentTrackFitter::setCombMuonTrackFitter' )
        from AthenaCommon.Include import include
        include('MuonCombinedRecExample/CombinedMuonFitTools.py') 
        
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc.MuonCombinedTrackFitter.FillDerivativeMatrix=True 

        self._fitter = ToolSvc.MuonCombinedTrackFitter
        self._fitter.OutputLevel = align.OutputLevel()

        if align.useSLFitter() :
            mlog.warning("SL Fitter set to default (curved) fitter!")
            self._slfitter = ToolSvc.MuonCombinedTrackFitter

        #self._fitter.OutputLevel = VERBOSE

        #self._fitter.GetMaterialFromTrack = True
        #self._fitter.MboyMat = False
        #self._fitter.FillDerivativeMatrix = True
        #self._fitter.MaxIterations = 50

    ## set global chi2 fitter
    def setGlobalChi2Fitter(self) :
        mlog = logging.getLogger( 'AlignmentTrackFitter::setGlobalChi2Fitter' )
        from AthenaCommon.AppMgr import ToolSvc
        from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter
        Fitter = Trk__GlobalChi2Fitter()
        ToolSvc += Fitter
        self._fitter = Fitter
        if align.useSLFitter() :
            mlog.warning("SL Fitter set to default (curved) fitter!")
            self._slfitter = Fitter

    ## set iPatFitter
    def set_iPatFitter(self) :
        from AthenaCommon.AppMgr import ToolSvc

        from AthenaCommon.Include import include
        include('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')
        
        # set up the extrapolator and fitter as used by Muid
        from TrkExRungeKuttaIntersector.TrkExRungeKuttaIntersectorConf import \
            Trk__IntersectorWrapper as Propagator
        MuidPropagator = Propagator(name = 'MuidPropagator')
        ToolSvc += MuidPropagator
        
        from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
        MuidMaterialUpdator = Trk__MaterialEffectsUpdator(name = "MuidMaterialEffectsUpdator")
        ToolSvc += MuidMaterialUpdator
        
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from TrkExTools.TrkExToolsConf import Trk__Navigator
        MuidNavigator = Trk__Navigator(name                = 'MuidNavigator',
                                       TrackingGeometrySvc = svcMgr.AtlasTrackingGeometrySvc)
        ToolSvc += MuidNavigator
        
        
        from TrkExTools.TrkExToolsConf import Trk__Extrapolator
        MuidExtrapolator = Trk__Extrapolator( 
            name = 'MuidExtrapolator', 
            Propagators = [ MuidPropagator ],
            MaterialEffectsUpdators = [ MuidMaterialUpdator ],
            Navigator=MuidNavigator)
        ToolSvc += MuidExtrapolator
        
        # material allocation
        from TrkiPatFitter.TrkiPatFitterConf import Trk__MaterialAllocator
        MuidMaterialAllocator = Trk__MaterialAllocator(
            name                      = 'MuidMaterialAllocator',
            AggregateMaterial         = True,
            Extrapolator              = MuidExtrapolator,
            TrackingGeometrySvc       = svcMgr.AtlasTrackingGeometrySvc)
        ToolSvc += MuidMaterialAllocator
        
        
        from TrkiPatFitter.TrkiPatFitterConf import Trk__iPatGlobalFitter
        Fitter = Trk__iPatGlobalFitter(
            name='iPatGlobalFitter',
        #from TrkiPatFitter.TrkiPatFitterConf import Trk__iPatFitter
        #Fitter = Trk__iPatFitter(
        #    name='iPatFitter',
            AggregateMaterial       = True,
            AsymmetricCaloEnergy    = False,
            FullCombinedFit         = True,
            MaterialAllocator       = MuidMaterialAllocator,         
            OutputLevel = align.OutputLevel())
            #OutputLevel = VERBOSE)
        
        ToolSvc += Fitter    
        self._fitter = ToolSvc.iPatGlobalFitter
        #self._fitter = ToolSvc.iPatFitter
        
        if align.useSLFitter() :
            SLFitter = Trk__iPatGlobalFitter(
                name                    = 'iPatGlobalSLFitter',
            #SLFitter = Trk__iPatFitter(
            #    name                    = 'iPatSLFitter',
                AggregateMaterial       = True,
                FullCombinedFit         = True,
                LineFit                 = True,
                MaterialAllocator       = MuidMaterialAllocator,
                OutputLevel = align.OutputLevel())
            ToolSvc += SLFitter
            self._slfitter = ToolSvc.iPatGlobalSLFitter
            #self._slfitter = ToolSvc.iPatSLFitter
        
    ## creates MCTBFitter (for standalone muon tracks)
    def setMCTBFitter (self) :

        from AthenaCommon.AppMgr import ToolSvc

        # this assumes that MCTBFitter has already been set
        from MuonRecExample import MuonRecTools
        #ToolSvc.MCTBFitter.OutputLevel = align.OutputLevel()
        #ToolSvc.MCTBSLFitter.OutputLevel = align.OutputLevel()

        MCTBFitter = MuonRecTools.getPublicToolClone("myMCTBFitter","MCTBFitterMaterialFromTrack")
        MCTBFitter.FillDerivativeMatrix = True
        MCTBFitter.OutputLevel = align.OutputLevel()
        #MCTBFitter.OutputLevel = VERBOSE
        #MCTBFitter.GetMaterialFromTrack = True
        #MCTBFitter.PrintDerivatives = True
        ToolSvc += MCTBFitter
        self._fitter = ToolSvc.myMCTBFitter
        
        if align.useSLFitter() :
            MCTBSLFitter = MuonRecTools.getPublicToolClone("myMCTBSLFitter","MCTBSLFitterMaterialFromTrack")
            MCTBSLFitter.FillDerivativeMatrix = True
            MCTBSLFitter.OutputLevel = align.OutputLevel()
            #MCTBSLFitter.OutputLevel = VERBOSE
            #MCTBSLFitter.GetMaterialFromTrack = True
            ##rdh MCTBSLFitter.Momentum = 0
            #MCTBSLFitter.PrintDerivatives = True
            ToolSvc += MCTBSLFitter
            self._slfitter = ToolSvc.myMCTBSLFitter

    ## returns fitter
    def fitter (self) :
        return self._fitter
          
    ## returns straight-line version of fitter
    def slfitter (self) :
        if align.useSLFitter() :
            return self._slfitter
        else:
            return
        
