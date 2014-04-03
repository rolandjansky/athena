# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################
# ConfMuonTrackThroughCalo module
#
# it inherits from MuonTrackThroughCalo and performs 
# standard configuration
#
######################################################

# import the include statement
from AthenaCommon.Include import Include, IncludeError, include

# import the ToolSvc
from AthenaCommon.AppMgr import ToolSvc

# import the Extrapolator configurable
from MuonTrackThroughCalo.MuonTrackThroughCaloConf import Rec__MuonTrackThroughCalo

# define the class
class ConfMuonTrackThroughCalo( Rec__MuonTrackThroughCalo ):
    # constructor
    def __init__(self,name = 'MuonTrackThroughCalo'):

        include('CaloMaterialEffectsOnTrackProvider/CaloMaterialEffectsOnTrackProvider.py')
        from __main__ import ToolSvc
        from CaloMaterialEffectsOnTrackProvider.ConfCaloMaterialEffectsOnTrackProvider import ConfCaloMaterialEffectsOnTrackProvider as ConfMEOTP

        CaloMEOTP = ConfMEOTP('CaloMaterialEffectsOnTrackProvider')
        ToolSvc += CaloMEOTP

        #create appropriate extrapolators
        from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
        MuonCaloDynamMaterialUpdator            = Trk__MaterialEffectsUpdator()
        ToolSvc += MuonCaloDynamMaterialUpdator

        MyUpdators = []
        MyUpdators += [ MuonCaloDynamMaterialUpdator ]

        MySubUpdators = []
        MySubUpdators += [ MuonCaloDynamMaterialUpdator.name() ]
        MySubUpdators += [ MuonCaloDynamMaterialUpdator.name() ]
        MySubUpdators += [ MuonCaloDynamMaterialUpdator.name() ]

        caloCombiName = name+'_CombiExtrapolator'
        from TrkExTools.UserExtrapolator import UserExtrapolator
        MuonCaloDynamicCombiExtrapolator = UserExtrapolator(name = caloCombiName)
        newMEOTP = MuonCaloDynamicCombiExtrapolator.configureCaloMaterialEffects()
        newMEOTP.UseMeasurement = True
        from MuonElossCalculators.MuonElossCalculatorsConf import CombinedEnergyLossCalculator as Calculator
        CombinedCalculator = Calculator('CombinedMean')
        ToolSvc += CombinedCalculator
        newMEOTP.EnergyLossCalculator = CombinedCalculator
        MuonCaloDynamicCombiExtrapolator.MaterialEffectsUpdators = MyUpdators
        MuonCaloDynamicCombiExtrapolator.SubMEUpdators = MySubUpdators
        ToolSvc += MuonCaloDynamicCombiExtrapolator

        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        MuonParamExtrapolator = AtlasExtrapolator()
        ToolSvc += MuonParamExtrapolator

        StaticExtrapolator = AtlasExtrapolator(name = "StaticExtrapolator")
        StaticExtrapolator.DoCaloDynamic = False
        ToolSvc += StaticExtrapolator

        # call the base class constructor
        Rec__MuonTrackThroughCalo.__init__(self,name,StaticExtrapolator = StaticExtrapolator, ParamExtrapolator = MuonParamExtrapolator, CombiExtrapolator = MuonCaloDynamicCombiExtrapolator )

    def getCombiExtrapolator(self):
        caloCombiName = self.getName()+'_CombiExtrapolator'
        from TrkExTools.UserExtrapolator import UserExtrapolator
        MuonCaloDynamicCombiExtrapolator = UserExtrapolator(name = caloCombiName)
        return MuonCaloDynamicCombiExtrapolator
