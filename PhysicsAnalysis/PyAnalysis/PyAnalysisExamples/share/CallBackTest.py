######################################################################
#
# This script demonstrates how to set a callback function
#
######################################################################


# load dict for MyCutClass
pylcgdict.load_library("libPyAnalysisExamplesDict")

# instantiate a C++ class from python
obj = g.CallBackTest()

# setup reverse proxy
g.PyReverseProxy(g.MyDetObj).setConv(g.PyReverseProxy(g.MyDetObj).getObj)

# define a python function
def aPyFunc(dobj):
    print "   aPyFunc called"
    print dobj.name()
    
# set callback
obj.setCallBack(aPyFunc)

# test
obj.test()
