# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from JetMonitoring.JetMonitoringConf import HistoDefinitionTool

def createHistoDefTool( name, title=None,nbinsx=10, xlow=10.0, xup=1.0, nbinsy=10, ylow=0.0, yup=1.0, hname=None):
    """Short cut to return a HistoDefinitionTool from a compact list of arguments"""    
    if title is  None : title = name
    if hname is None :  hname = name
    name = "hdef_"+name # athena can causes conflicts when tools of different types have same names
    return HistoDefinitionTool(name,hname=hname, title=title,nbinsx=nbinsx, xlow=xlow, xup=xup, nbinsy=nbinsy, ylow=ylow, yup=yup)

def mergeHistoDefinition(hdef1, hdef2):
    """Merge 2 histo definitions (assumming 1D definition) into a 2D histo definiton
     hdef1/2 are either
       - HistoDefinitionTool instances
       - tuple in the form ('title', nbins, xlow, xup)
       returns a tuple form
    """
    def makeTuple(hdef):
        if isinstance(hdef,HistoDefinitionTool):
            return (hdef.title, hdef.nbinsx, hdef.xlow, hdef.xup)
        return hdef
    hdef1 = makeTuple(hdef1)
    hdef2 = makeTuple(hdef2)
    # rebuild a full 2D title specification
    topT1, xT1, n = hdef1[0].split(';')
    topT2, xT2, n = hdef2[0].split(';')
    title = ';'.join([topT2+' vs '+topT1 , xT1, xT2 ])
    # build (title, binning1+binning2 ) as a 2D binning def
    hdef = (title, ) + hdef1[1:] + hdef2[1:]
    return hdef
