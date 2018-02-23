# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

#from TriggerJobOpts.TriggerFlags import TriggerFlags
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
from TrigMuonEFTagandProbeMonitoring import TrigMuonEFTagandProbeMonitoring
#from TrigMuonEF.TrigMuonEFMonitoring import TrigMuonEFStandaloneToolMonitoring,TrigMuonEFCombinerToolMonitoring,TrigMuonEFStandaloneToolValidationMonitoring,TrigMuonEFCombinerToolValidationMonitoring
#from TrigMuonEF.TrigMuonEFConf import TrigMuonEFTrackIsolationTool

#from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoConfig
from TrigMuSuperEFConf import TrigMuonEFTagandProbe #Conf is based on algorithm package not individual algs
#from AthenaCommon.BeamFlags import jobproperties

#from AthenaCommon.CfgGetter import getPublicTool
#from AthenaCommon import CfgMgr
#from AthenaCommon.SystemOfUnits import mm
                                                                                                                               
# Default config: RoI based, Combined, TrigMuonEF only
 
class TrigMuonEFTagandProbeConfig(TrigMuonEFTagandProbe): #making python class from Conf import TaP object
    __slots__ = () #Saves space by not using memory slots rather than a  dictionary for storing class attributes

# These are methods tied to the class we are building
# self is a reference to the object we passed to the class
# we need this as python is not object orientated so we can't assume this is assumed like it is in c++
# __init__ is essentially pythons version of a constructor, this method gets called whenever our base class is used and it takes self as an implicit argument to know what to build as well as any other arguments that we might want to pass when calling our base class method
# **kwargs is a 'magic' argument variable which stands in for arguments when we don't know how many are going to be passed, Kwargs over args is for arguments in python list format with associated key strings    

# Right now there are no kwargs to set up for TaP
    def __init__(self,name="TrigMuonEFTagandProbe",**kwargs): 

        #Set out actions associated with class construction



        # turn on seeded data decoding by defualt       
        #TriggerFlags.MuonSlice.doEFRoIDrivenAccess = True



        # make instance of TrigMuonEFTagandProbe class when constructor is called
        # Super allows direct use of inherited methods and attributes from the parent class, TrigMuonEFTagandProbe in this case without having to directly reference this class, this is useful for multiple inheritance 

        super(TrigMuonEFTagandProbeConfig,self).__init__(name,**kwargs)

        # setup monitoring depending on configuration
        monTools = []

        # top level histograms use Combined Muons
        monTools.append( TrigMuonEFTagandProbeMonitoring() )



#        from AthenaCommon.CfgGetter import getPublicTool,getPublicToolClone
#        self.StauCreatorTool = getPublicToolClone("TMEF_StauCreatorTool","TMEF_MuonCreatorTool",BuildStauContainer=True)


 
        # always add timing monitoring
        timetool =  TrigTimeHistToolConfig("Time")
        timetool.NumberOfHistBins = 100
        timetool.TimerHistLimits = [0,1000]
        monTools.append( timetool )

        self.AthenaMonTools = monTools
    #  end of __init__


#
# Space for Several pre-configured RoI based variations
# Only base class needed for now though
#
def TrigMuonEFTagandProbe(name="TrigMuonEFTagandProbe",**kwargs):
    return TrigMuonEFTagandProbeConfig(name,**kwargs)

