# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigInDetRecoTools.TrigInDetRecoToolsConf import TrigL2PattRecoStrategyA
from TrigInDetRecoTools.ConfiguredSettings import SettingsForStrategyA

from IDScanZFinder.IDScanZFinderConf import IDScanZFinder
from IDScanHitFilter.IDScanHitFilterConf import IDScanHitFilter
from TrigInDetRecoTools.TrigInDetRecoToolsConf import TrigL2DupTrackRemovalTool

from AthenaCommon.Logging import logging  
log = logging.getLogger("FactoryForStrategyA")

      
class FactoryForStrategyA() :
    __slots__=[]
    def __init__(self, name = 'FactoryForStrategyA') :
        self.strategy = None
        self.settings = SettingsForStrategyA()

    def createStrategy(self, instName) :
        if instName in self.settings.allowedInstanceNames :

            self.strategy = TrigL2PattRecoStrategyA("StrategyA_"+instName)
            from AthenaCommon.AppMgr import ToolSvc

            self.strategy.ZFinderMode        = self.settings[('ZFinderMode',instName)]   
            self.strategy.TrueVertexLocation = "TrueTrigVertexColl"
            self.strategy.FindMultipleZ      = self.settings[('FindMultipleZ',instName)]
            self.strategy.DoZF_Only          = self.settings[('DoZF_Only',instName)]
            
            zfinderTool = IDScanZFinder(name          = self.settings[('namezf',instName)], 
                                        FullScanMode  = self.settings[('FullScanMode',instName)] ,
                                        NumberOfPeaks = self.settings[('NumberOfPeaks',instName)],
                                        PhiBinSize    = self.settings[('PhiBinSizeZ',instName)] ,
                                        UseOnlyPixels = self.settings[('UseOnlyPixels',instName)],
                                        MinZBinSize   = self.settings[('MinZBinSize',instName)],
                                        ZBinSizeEtaCoeff = self.settings[('ZBinSizeEtaCoeff',instName)],
                                        DPhiDEta      = self.settings[('DPhiDEta',instName)],
                                        ChargeAware   = self.settings[('ChargeAware',instName)],
                                        ZHistoPerPhi  = self.settings[('ZHistoPerPhi',instName)],
                                        PreferCentralZ = self.settings[('PreferCentralZ',instName)],
                                        VrtxDistCut   = self.settings[('VrtxDistCut',instName)] ,
                                        NeighborMultiplier   = self.settings[('NeighborMultiplier',instName)] ,
                                        nVrtxSeparation = self.settings[('nVrtxSeparation',instName)],
                                        VrtxMixing      = self.settings[('VrtxMixing',instName)],  
                                        TripletMode     = self.settings[('TripletMode',instName)],
                                        nFirstLayers    = self.settings[('nFirstLayers',instName)],
                                        WeightThreshold = self.settings[('WeightThreshold',instName)]
                                        )
            
            zfinderTool.TripletDZ              =      self.settings[('TripletDZ',instName)]              
            zfinderTool.TripletDK              =      self.settings[('TripletDK',instName)]              
            zfinderTool.TripletDP              =      self.settings[('TripletDP',instName)]              
            #zfinderTool.useRoiDescriptorWidths    =   self.settings[('useRoiDescriptorWidths',instName)] 



            hitfilterTool = IDScanHitFilter(name= self.settings[('namehit',instName)], 
                                            fullScan  = self.settings[('FullScanMode',instName)] ,
                                            phiHalfWidth = self.settings[('phiHalfWidth',instName)],
                                            etaHalfWidth = self.settings[('etaHalfWidth',instName)],
                                            PhiBinSize = self.settings[('PhiBinSize',instName)],
                                            EtaBinSize = self.settings[('EtaBinSize',instName)],
                                            LayerThreshold = self.settings[('LayerThreshold',instName)],
                                            LooseTightBoundary = self.settings[('LooseTightBoundary',instName)],
                                            EnhanceLayer0 = self.settings[('EnhanceLayer0',instName)],
                                            RemoveDuplicates = self.settings[('RemoveDuplicates',instName)],
                                            CloneRemovalCut = self.settings[('CloneRemovalCut',instName)],
                                            UsePtSign = self.settings[('UsePtSign',instName)],)
            
            hitfilterTool.dPhidRBinSize_tight = self.settings[('dPhidRBinSize_tight',instName)]
            hitfilterTool.phi0Bins_tight = self.settings[('phi0Bins_tight',instName)]
            hitfilterTool.etaCutInner_tight = self.settings[('etaCutInner_tight',instName)]
            hitfilterTool.etaCutOuter_tight = self.settings[('etaCutOuter_tight',instName)]
            hitfilterTool.d0Cut_tight = self.settings[('d0Cut_tight',instName)]
            hitfilterTool.d0Cut_loose = self.settings[('d0Cut_loose',instName)]
            hitfilterTool.etaCutInner_loose = self.settings[('etaCutInner_loose',instName)]
            hitfilterTool.etaCutOuter_loose = self.settings[('etaCutOuter_loose',instName)]
            hitfilterTool.dPhidRBinSize_loose = self.settings[('dPhidRBinSize_loose',instName)]
            hitfilterTool.phi0Bins_loose = self.settings[('phi0Bins_loose',instName)]
            hitfilterTool.deltadzdrCut_loose = self.settings[('deltadzdrCut_loose',instName)]
            hitfilterTool.deltadzdrCut_tight = self.settings[('deltadzdrCut_tight',instName)]
            hitfilterTool.pTcutInMeV = self.settings[('pTmin',instName)]


            duptrkremovalTool = TrigL2DupTrackRemovalTool(name="DupTrackRemovalTool", mode = 2)

            ToolSvc += zfinderTool
            self.strategy.TrigZFinder=zfinderTool
            ToolSvc += hitfilterTool
            self.strategy.TrigHitFilter = hitfilterTool
            ToolSvc += duptrkremovalTool
            self.strategy.DupTrackRemovalTool = duptrkremovalTool
           
            self.strategy.AdjustLayerThreshold = False
            self.strategy.DetMaskCheck         = True
            self.strategy.DetMaskCheckOnEvent  = False
            self.strategy.UseBeamSpot          = True

            return self.strategy
        else :
            print "Instance "+instName+" of the Strategy A is not supported !"
            return None
