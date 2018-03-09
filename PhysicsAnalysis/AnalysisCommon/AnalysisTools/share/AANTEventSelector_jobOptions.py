theApp.Dlls += ["AnalysisTools"]
theApp.ExtSvc += [ "AANTEventSelector/EventSelector"]
theApp.EvtSel = "EventSelector"

# set C++->Python converter for TTree
import cppyy
cppyy.loadDictionary("AnalysisToolsDict")
aantGate = cppyy.Namespace('').AANTTreeGate
def aantConvFunc():
    return aantGate.getTree()

EventSelector = Service( "EventSelector" )
EventSelector.Converter = "aantConvFunc"
    
