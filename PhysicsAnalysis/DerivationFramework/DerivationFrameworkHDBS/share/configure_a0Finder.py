#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
#====================================================================
# Collection of tools to run the a0Finder
#====================================================================

class HDBSa0FinderTools:

    def __init__(self, derivation = ""):

        if derivation == "":
            print('--------> FATAL: HDBSa0FinderTools, "derivation" string not set!')
            import sys
            sys.exit()


        prefix = derivation+"a0FinderTools"

        from AthenaCommon.AppMgr import ToolSvc

        # set up extrapolator
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        self.Extrapolator = AtlasExtrapolator(name = prefix+"_AtlasExtrapolator")
        ToolSvc += self.Extrapolator
        print(self.Extrapolator)

        from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter 
        self.InDetSecVxFitterTool = Trk__TrkVKalVrtFitter(name                = prefix+"_Fitter",
                                                 Extrapolator        = self.Extrapolator,
                                                 MakeExtendedVertex  = True,
                                                 FirstMeasuredPoint  = False)
        ToolSvc += self.InDetSecVxFitterTool
        print(self.InDetSecVxFitterTool)


        from InDetConversionFinderTools.InDetConversionFinderToolsConf import InDet__VertexPointEstimator 
        self.InDetSecVtxPointEstimator = InDet__VertexPointEstimator(name = prefix+"_PointEstimator",
                                                                MinDeltaR              = [-10000.,-10000.,-10000.],
                                                                MaxDeltaR              = [10000.,10000.,10000.],
                                                                MaxPhi                 = [10000., 10000., 10000.],
                                                                MaxChi2OfVtxEstimation = 2000.)
        ToolSvc += self.InDetSecVtxPointEstimator
        print(self.InDetSecVtxPointEstimator)




      