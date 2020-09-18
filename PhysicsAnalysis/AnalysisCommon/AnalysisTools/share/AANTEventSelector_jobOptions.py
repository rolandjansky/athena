theApp.Dlls += ["AnalysisTools"]
theApp.ExtSvc += [ "AANTEventSelector/EventSelector"]
theApp.EvtSel = "EventSelector"

# set C++->Python converter for TTree
import cppyy
cppyy.load_library("libAnalysisToolsDict")
aantGate = cppyy.Namespace('').AANTTreeGate
def aantConvFunc():
    return aantGate.getTree()

EventSelector = Service( "EventSelector" )
EventSelector.Converter = "aantConvFunc"
    
