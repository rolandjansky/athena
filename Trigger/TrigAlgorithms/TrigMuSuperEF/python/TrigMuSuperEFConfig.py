# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
from TrigMuSuperEFMonitoring import TrigMuSuperEFMonitoring
from TrigMuonEF.TrigMuonEFMonitoring import TrigMuonEFStandaloneToolMonitoring,TrigMuonEFCombinerToolMonitoring


from TrigMuonHypo.TrigMuonHypoConfig import TrigMuonEFCombinerHypoConfig
from TrigMuSuperEFConf import TrigMuSuperEF


#
# Default config: RoI based, Combined, TrigMuonEF only
# Old default:  RoI based, Combined, TrigMuonEF first, TrigMuGirl is TrigMuonEF fails. Should be put back once TrigMuGirl is back in release
#
class TrigMuSuperEFConfig(TrigMuSuperEF):
    __slots__ = ()

    def __init__(self,name="TrigMuSuperEF", **kwargs):
        kwargs.setdefault("doInsideOut", False)
        kwargs.setdefault("doOutsideIn", True)
        kwargs.setdefault("insideOutFirst", False)
        kwargs.setdefault("fullScan", False)
        kwargs.setdefault("StandaloneOnly", False)
        kwargs.setdefault("CombinerOnly", False)
        kwargs.setdefault("TMEF_standaloneTrackTool", "TrigMuonEFStandaloneTrackTool")
        kwargs.setdefault("MuonCombinedTool","TMEF_MuonCombinedTool")
        kwargs.setdefault("TrkToTrackParticleConvTool","TMEF_TrkToTrackParticleConvTool")
        kwargs.setdefault("MuonCreatorTool","TMEF_MuonCreatorTool")
        

        doTrigMuonEF     = kwargs["doOutsideIn"]
        doTrigMuGirl     = kwargs["doInsideOut"]
        doStandaloneOnly = kwargs["StandaloneOnly"]
        doFullScan       = kwargs["fullScan"]
        combinerOnly     = kwargs["CombinerOnly"]

        if doFullScan:
            kwargs.setdefault("IdTrackParticles",  "InDetTrigTrackingxAODCnv_FullScan_EFID")
        else:
            kwargs.setdefault("IdTrackParticles", "InDetTrigTrackingxAODCnv_Muon_EFID")
        
        # make instance
        super(TrigMuSuperEFConfig,self).__init__(name,**kwargs)

        # setup monitoring depending on configuration
        monTools = []
        # top level histograms use Combined Muons
        if not doStandaloneOnly:
            monTools.append( TrigMuSuperEFMonitoring() )

        # only add TrigMuonEF monitoring if it is run
        if doTrigMuonEF:
            if not combinerOnly:
                monTools.append( TrigMuonEFStandaloneToolMonitoring() )
            if not doStandaloneOnly:
                monTools.append( TrigMuonEFCombinerToolMonitoring() )

        # only add TrigMuGirl monitoring if it is run
        if doTrigMuGirl:
            from TrigMuGirl.TrigMuGirlMonitoring import TrigMuGirlToolMonitoring
            monTools.append( TrigMuGirlToolMonitoring() )

        # always add timing monitoring
        timetool =  TrigTimeHistToolConfig("Time") 
        timetool.NumberOfHistBins = 100
        timetool.TimerHistLimits = [0,1000]
        monTools.append( timetool )
        
        self.AthenaMonTools = monTools
    #  end of __init__       


#
# Several pre-configured RoI based variations
#
def TrigMuSuperEF_MGfirst(name="TrigMuSuperEF_MGfirst",**kwargs):
    kwargs.setdefault("doInsideOut", True)
    kwargs.setdefault("doOutsideIn", True)
    kwargs.setdefault("insideOutFirst", True)
    return TrigMuSuperEFConfig(name,**kwargs)


def TrigMuSuperEF_TMEFfirst(name="TrigMuSuperEF_TMEFfirst",**kwargs):
    kwargs.setdefault("doInsideOut", True)
    kwargs.setdefault("doOutsideIn", True)
    kwargs.setdefault("insideOutFirst", False)
    return TrigMuSuperEFConfig(name,**kwargs)


def TrigMuSuperEF_MGonly(name="TrigMuSuperEF_MGonly",**kwargs):
    kwargs.setdefault("doInsideOut", True)
    kwargs.setdefault("doOutsideIn", False)
    return TrigMuSuperEFConfig(name,**kwargs)


def TrigMuSuperEF_TMEFonly(name="TrigMuSuperEF_TMEFonly",**kwargs):
    kwargs.setdefault("doInsideOut", False)
    kwargs.setdefault("doOutsideIn", True)
    return TrigMuSuperEFConfig(name,**kwargs)


def TrigMuSuperEF_both(name="TrigMuSuperEF_both",**kwargs):
    kwargs.setdefault("doInsideOut", True)
    kwargs.setdefault("doOutsideIn", True)
    kwargs.setdefault("forceBoth", True)
    return TrigMuSuperEFConfig(name,**kwargs)


def TrigMuSuperEF_SAonly(name="TrigMuSuperEF_SAonly",**kwargs):
    kwargs.setdefault("doInsideOut", False)
    kwargs.setdefault("doOutsideIn", True)
    kwargs.setdefault("StandaloneOnly", True)
    return TrigMuSuperEFConfig(name,**kwargs)

def TrigMuSuperEF_TMEFCombinerOnly(name="TrigMuSuperEF_TMEFCombinerOnly",**kwargs):
    kwargs.setdefault("doInsideOut", False)
    kwargs.setdefault("doOutsideIn", True)
    kwargs.setdefault("CombinerOnly", True)
    return TrigMuSuperEFConfig(name,**kwargs)

#
# Full scan configs
#
def TrigMuSuperEF_FSCB(name="TrigMuSuperEF_FSCB",**kwargs):
    kwargs.setdefault("doInsideOut",False)
    kwargs.setdefault("doOutsideIn",True)
    kwargs.setdefault("fullScan",True)    
    kwargs.setdefault("IdTrackParticles", "InDetTrigTrackingxAODCnv_FullScan_EFID")
    return TrigMuSuperEFConfig(name,**kwargs)


def TrigMuSuperEF_FSSA(name="TrigMuSuperEF_FSSA",**kwargs):
    kwargs.setdefault("StandaloneOnly",True)
    return TrigMuSuperEF_FSCB(name,**kwargs)



class TrigMuSuperEFHypoConfig(TrigMuonEFCombinerHypoConfig):
    __slots__ = ()

    # overwrite the __new__ of TrigMuonEFCombinerHypeConfig so we can set our own instance name
    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s_%s' % (args[0],'Muon',args[1])] + list(args)
        # Note: skip the __new__ of TrigMuonEFCombinerHypeConfig, so go directly to its superclass !!!
        return super(TrigMuonEFCombinerHypoConfig, cls ).__new__( cls, *newargs, **kwargs )
   
