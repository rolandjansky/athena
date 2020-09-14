from PyKernel import PyKernel

from PyKernel.PyKernel import plot,plot2,fill,fill2,dumpSG,book,bookProf
from PyKernel.PyKernel import plotProf,fillProf,unregister,dumpHist,retrieveHist

import cppyy
cppyy.load_library("liblibPyKernelDict")

# Bind the C++ global namespace to the Python variable g 
g = cppyy.gbl

## temporary hack to 'fix' #58078
if 0:
    # set ROOT stream
    import re
    import ROOT
    rootStream=None
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    EventSelector = svcMgr.EventSelector
    if hasattr(EventSelector,"CollectionType") and  EventSelector.CollectionType == "ExplicitROOT":
        filename = EventSelector.InputCollections[0]
        if not re.search('\.root$',filename):
            filename = filename+'.root'
        file = ROOT.TFile(filename)
        rootStream=ROOT.gROOT.FindObject('CollectionTree')

    # this must be the last one
    PyKernel.init(theApp,rootStream)

    pass # temporary hack
