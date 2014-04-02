#####################################################################
#  ConfiguredInDetAlignTrackSelTool
#####################################################################
# Python Setup Class InDetAlignGenTools/InDetAlignTrackSelTool
#
# Sergio Gonzalez-Sevilla
# Date: 21/12/2005
#
#####################################################################

class ConfiguredInDetAlignTrackSelTool :
    def __init__ (self,
                  instname='InDetAlignTrackSelTool', 
                  propagatorType='RungeKutta') :

        # Load the Dlls
        if 'InDetAlignGenTools' not in theApp.Dlls:
           theApp.Dlls += [ 'InDetAlignGenTools' ]

        # Set names
        self.__instname__ = instname
        self.__svcname__  = 'InDetAlignTrackSelTool'

        self.__toolname__ = 'ToolSvc.'+self.__instname__
        self.__thistool__ = Service( self.__toolname__ )
        
        # Set options for tool
        self.__thistool__.MinMomentum = 0
        self.__thistool__.MinPt       = 2
        self.__thistool__.MaxShared   = 0
        self.__thistool__.MaxHoles    = 1
        self.__thistool__.MinChi2Prob = 0.2

        ToolSvc = Service("ToolSvc")

        # Propagator
        if propagatorType is "STEP":
            from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as Propagator
        else:
            from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
        InDetAlignTrackSelPropagator = Propagator(name = 'InDetAlignTrackSelPropagator')
        ToolSvc += InDetAlignTrackSelPropagator
        print InDetAlignTrackSelPropagator

        # Navigator
        from TrkExTools.TrkExToolsConf import Trk__Navigator
        InDetAlignTrackSelNavigator = Trk__Navigator(name = 'InDetAlignTrackSelNavigator')
        ToolSvc += InDetAlignTrackSelNavigator
        print InDetAlignTrackSelNavigator
                
        # Material updator
        from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
        InDetAlignTrackSelMaterialUpdator = Trk__MaterialEffectsUpdator(name = "InDetAlignTrackSelMaterialEffectsUpdator")
        ToolSvc += InDetAlignTrackSelMaterialUpdator
        print InDetAlignTrackSelMaterialUpdator
        
        # Extrapolator
        from TrkExTools.TrkExToolsConf import Trk__Extrapolator
        InDetAlignTrackSelExtrapolator = Trk__Extrapolator(name                   = 'InDetAlignTrackSelExtrapolator', 
                                                           Propagators            = [ InDetAlignTrackSelPropagator.getType() ],
                                                           PropagatorInstances    = [ InDetAlignTrackSelPropagator.getName() ],
                                                           Navigator              = InDetAlignTrackSelNavigator,
                                                           MaterialEffectsUpdator = InDetAlignTrackSelMaterialUpdator)
        ToolSvc += InDetAlignTrackSelExtrapolator
        print InDetAlignTrackSelExtrapolator             

        # associator
        from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
        InDetAlignTrackSelAssociationTool = InDet__InDetPRD_AssociationToolGangedPixels()
        ToolSvc += InDetAlignTrackSelAssociationTool

        # summary
        from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
        InDetAlignTrackSelSummaryTool = InDet__InDetTrackSummaryHelperTool(name         = "InDetAlignTrackSelSummaryTool",
                                                                           Extrapolator = InDetAlignTrackSelExtrapolator,
                                                                           AssoTool     = InDetAlignTrackSelAssociationTool,
                                                                           DoSharedHits = True)
        ToolSvc += InDetAlignTrackSelSummaryTool
        print InDetAlignTrackSelSummaryTool          
             
    # Set message level
    def msgStreamLevel(self, level):
        self.__thistool__.OutputLevel = level

    # Set track cuts: min Momentum(GeV)
    def MinMomentum(self,minMom=0):
        self.__thistool__.MinMomentum=minMom
       
    # Set track cuts: min pt(GeV)
    def MinPt(self,minpt=2):
        self.__thistool__.MinPt=minpt

    # Set track cuts: max shared hits
    def MaxShared(self,maxshared=0):
        self.__thistool__.MaxShared=maxshared

    # Set track cuts: max holes
    def MaxHoles(self,maxholes=1):
        self.__thistool__.MaxHoles=maxholes

    # Set track cuts: min chi2Prob
    def MinChi2Prob(self,minchi2prob=0.2):
        self.__thistool__.MinChi2Prob=minchi2prob
                
    # Set track cuts: all together
    def SetCuts(self,minMom=0,minpt=2,maxshared=0,maxholes=1,minchi2prob=0.2):
        self.__thistool__.MinMomentum=minMom
        self.__thistool__.MinPt=minpt
        self.__thistool__.MaxShared=maxshared
        self.__thistool__.MaxHoles=maxholes
        self.__thistool__.MinChi2Prob=minchi2prob

    # Return method for the service name
    def name(self):
        return self.__svcname__
 
    # Return method for the instance name
    def instance(self):
        return self.__instname__
                
    # Output info
    def printInfo(self) :
        print '*****  ConfiguredInDetAlignTrackSelTool ***************************************'
        print '* - ToolName:     ', self.__svcname__,' (Instance: ', self.__instname__, ')'
        print '* --------------------------------------------------------------------------------'
        print '* - Selected track cuts:'
        print '*     o Min Momentum(GeV) = ', self.__thistool__.MinMomentum
        print '*     o Min Pt(GeV) = ', self.__thistool__.MinPt
        print '*     o Max Shared = ', self.__thistool__.MaxShared
        print '*     o Max Holes  = ', self.__thistool__.MaxHoles
        print '*     o Min Chi2Prob  = ', self.__thistool__.MinChi2Prob
        print '*******************************************************************************'
 
