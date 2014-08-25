# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from TrackToCalo.TrackToCaloConf import ExtrapolateToCaloTool

class ExtrapolateToCaloToolBase ( ExtrapolateToCaloTool )  :

    def __init__(self):
        mlog = logging.getLogger( name+'(Base)::__init__ dummy for instance copy' )
        mlog.info('entering')
        ExtrapolateToCaloTool.__init__( self) # call base class constructor
        pass

    def __init__(self, name="ExtrapolateToCaloToolBaseUNSET", depth="showerdefault", straightLine=False):

        ExtrapolateToCaloTool.__init__( self,name) # call base class constructor

        from AthenaCommon.Logging import logging
        mlog = logging.getLogger( name+'(Base)::__init__ ' )
        mlog.info("entering")


        if name=="ExtrapolateToCaloToolBaseUNSET":
            mlog.info ("name should be explicitly set")
            return

        # this creates CaloDepthTool and a CaloSurfaceBuilder where the depth is set to the given value:
        from CaloTrackingGeometry.CaloSurfaceBuilderBase import CaloSurfaceBuilderFactory
        theCaloSurfaceBuilder = CaloSurfaceBuilderFactory(depth)
        # set as public tool
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += theCaloSurfaceBuilder
        # use the CaloSurfaceBuilder and the CaloDepthTool
        self.CaloSurfaceBuilder = theCaloSurfaceBuilder
        self.CaloDepthTool = theCaloSurfaceBuilder.CaloDepthTool

        #if not straightLine:
            #theExtrapolator=AtlasExtrapolator(name = 'TrackToCaloExtrapolator')
            #theExtrapolator.DoCaloDynamic = False  # this turns off dynamic calculation of eloss in calorimeters
                                                   ## all left to MaterialEffects/EnergyLossUpdators

            #from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator as MaterialEffectsUpdator
            #AtlasMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'AtlasMaterialEffectsUpdator')
            #ToolSvc += AtlasMaterialEffectsUpdator #default material effects updator
            #NoElossMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'NoElossMaterialEffectsUpdator')
            #NoElossMaterialEffectsUpdator.EnergyLoss = False
            #ToolSvc += NoElossMaterialEffectsUpdator

            ##setup MaterialEffectsUpdator arrays
            #MyUpdators = []
            #MyUpdators += [AtlasMaterialEffectsUpdator] # for ID
            #MyUpdators += [NoElossMaterialEffectsUpdator] # for Calo
            ##MyUpdators += [NoElossMaterialEffectsUpdator] # for muon

            #MySubUpdators = []
            #MySubUpdators += [AtlasMaterialEffectsUpdator.name()] # for ID
            #MySubUpdators += [NoElossMaterialEffectsUpdator.name()] # for Calo
            #MySubUpdators += [NoElossMaterialEffectsUpdator.name()] # for muon

            #theExtrapolator.MaterialEffectsUpdators = MyUpdators
            #theExtrapolator.SubMEUpdators = MySubUpdators
        #else:
            ##theExtrapolator=AtlasExtrapolator("ATLASExtrapolatorStraightLine")
            ##from TrkExSlPropagator.TrkExSlPropagatorConf import Trk__StraightLinePropagator as Propagator
            ##SlPropagator =  Propagator(name='TTC_SlPropagator')
            ##ToolSvc += SlPropagator
            ##theExtrapolator.Propagators = [ SlPropagator ]
            
            #theExtrapolator=AtlasExtrapolator("TrackToCaloExtrapolatorStraightLine")
            #from TrkExSlPropagator.TrkExSlPropagatorConf import Trk__StraightLinePropagator as Propagator
            #SlPropagator =  Propagator(name='TTC_SlPropagator')
            #ToolSvc += SlPropagator
            #theExtrapolator.Propagators = [ SlPropagator ]

        #Get the extrapolator
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator

        theExtrapolator=AtlasExtrapolator()

        ## need to add to ToolSvc before putting in ToolHandle
        ToolSvc+=theExtrapolator
        self.Extrapolator=theExtrapolator



# factory : autogenerates the name
# usage :
# from TrackToCalo.ExtrapolateToCaloToolBase import ExtrapolateToCaloToolFactory
# exToCalo = ExtrapolateToCaloToolFactory(depth="showerdefault")
# ToolSvc+=exToCalo
def ExtrapolateToCaloToolFactory( depth="showerdefault", straightLine=False ):
    # build the tool name by appending  depth valure
    #if straightLine:
        #straightName="Straight"
    #else:
        #straightName=""
    #return ExtrapolateToCaloToolBase( "ExtrapolateToCaloTool"+depth+straightName, depth=depth,straightLine=straightLine )
    return ExtrapolateToCaloToolBase( "ExtrapolateToCaloTool"+depth, depth=depth ) 
