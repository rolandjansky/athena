# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
from TrigMuonEFTagandProbeMonitoring import TrigMuonEFTagandProbeMonitoring

from TrigMuSuperEFConf import TrigMuonEFTagandProbe #Conf is based on algorithm package not individual algs
                                                                                                                               
# Default config: Online Z->mumu HLT based Tag and Probe efficiency monitoring
 
class TrigMuonEFTagandProbeConfig(TrigMuonEFTagandProbe): #making python class from Conf import TaP object
    __slots__ = () #Saves space by not using memory slots rather than a  dictionary for storing class attributes

# These are methods tied to class being built
# self is a reference to the class object itself
# __init__ is essentially pythons version of a constructor, gets called whenever base class is used and takes self as an implicit argument to know what to build as well as any other arguments passed when calling base class method
# **kwargs is a 'magic' argument variable which stands in for unknown numbers of arguments when we don't know how many are going to be passed, used for arguments in python list format with associated key strings

# Currently no kwargs to set up for TaP
    def __init__(self,name="TrigMuonEFTagandProbe",**kwargs): 

        #Set out actions associated with class construction


        # make instance of TrigMuonEFTagandProbe class when constructor is called
        # Super allows direct use of inherited methods and attributes from the parent class, TrigMuonEFTagandProbe in this case, without having to directly reference this class, useful for multiple inheritance 

        super(TrigMuonEFTagandProbeConfig,self).__init__(name,**kwargs)

        # setup monitoring depending on configuration
        monTools = []

        monTools.append( TrigMuonEFTagandProbeMonitoring() )

        # always add timing monitoring
        timetool =  TrigTimeHistToolConfig("Time")
        timetool.NumberOfHistBins = 100
        timetool.TimerHistLimits = [0,1000]
        monTools.append( timetool )

        self.AthenaMonTools = monTools
    #  end of __init__


#
# Space for Several pre-configured TaP variations
# Only base class needed for now
#
def TrigMuonEFTagandProbe(name="TrigMuonEFTagandProbe",**kwargs):
    return TrigMuonEFTagandProbeConfig(name,**kwargs)

