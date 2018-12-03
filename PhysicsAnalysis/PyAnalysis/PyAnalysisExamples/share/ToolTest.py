######################################################################
#
# Test for AlgTool
#
######################################################################

# load dict for MyCutClass
import cppyy
cppyy.loadDictionary("PyAnalysisExamplesDict")

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# add my Algorithm
from PyAnalysisExamples.PyAnalysisExamplesConf import AthPyEx__MySelectionAlg
topSequence += AthPyEx__MySelectionAlg("MySelectionAlg")

# add my Tool
from PyAnalysisExamples.PyAnalysisExamplesConf import AthPyEx__MyTool
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += AthPyEx__MyTool("MyTool")

# init application mgr
theApp.initialize()

# retrieve AlgTool
tool = theApp.toolsvc().retrieve(ToolSvc.MyTool.getFullJobOptName())._itool
tool.setPython(tool)

# set proxy
def convFunc():
    ptr = cppyy.libPyROOT.MakeNullPointer(g.MyObj)
    proxy = g.PyReverseProxy.getProxy("MyObj_Proxy")
    proxy.getObj(ptr)
    return ptr
proxy = g.PyReverseProxy.getProxy("MyObj_Proxy")
proxy.setFunc(convFunc)

counter=567
# callback
def py_execute(obj):
    print "PyExecute begin"
    print "   name : %s" % obj.name()
    print "      A : %s" % obj.getA()
    print "setA float in Python"
    obj.setA(1.22)
    print "setA int in Python"    
    global counter
    obj.setA(counter)
    counter += 1
    print "PyExecute end"    
    return True
tool.py_execute = py_execute


# run 1 event
theApp.nextEvent()

# set different parameter to AlgTool
tool.setA(50)

# run 1 event
theApp.nextEvent()


