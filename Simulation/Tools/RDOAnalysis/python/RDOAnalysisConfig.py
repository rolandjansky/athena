# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def RDOAnalysisBaseCfg(configFlags,name,**kwargs):
    
    result = ComponentAccumulator()
    
    THistSvc= CompFactory.THistSvc
    histsvc = THistSvc(name="THistSvc")
    result.addService(histsvc)
    
    return result

def ITkStripRDOAnalysisCfg(configFlags,name="ITkStripRDOAnalysis",**kwargs):

    result = RDOAnalysisBaseCfg(configFlags,name,**kwargs)

    kwargs.setdefault("NtupleFileName","/"+name+"/")
    kwargs.setdefault("HistPath","/"+name+"/")

    ITkStripRDOAnalysis = CompFactory.StripRDOAnalysis(name,**kwargs)
    result.addEventAlgo(ITkStripRDOAnalysis)
    result.getService("THistSvc").Output = [ name+" DATAFILE='"+name+".root' OPT='RECREATE'" ] 
    
    return result

def ITkPixelRDOAnalysisCfg(configFlags,name="ITkPixelRDOAnalysis",**kwargs):
  
    result = RDOAnalysisBaseCfg(configFlags,name,**kwargs)

    kwargs.setdefault("NtupleFileName","/"+name+"/")
    kwargs.setdefault("HistPath","/"+name+"/")

    ITkPixelRDOAnalysis = CompFactory.ITkPixelRDOAnalysis(name,**kwargs)
    result.addEventAlgo(ITkPixelRDOAnalysis)
    result.getService("THistSvc").Output = [ name+" DATAFILE='"+name+".root' OPT='RECREATE'" ] 
    
    return result
