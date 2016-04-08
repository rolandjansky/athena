theApp.Dlls += ["AnalysisTools"]
theApp.ExtSvc += [ "AANTEventSelector/EventSelector"]
theApp.EvtSel = "EventSelector"

# set C++->Python converter for TTree
import PyCintex
PyCintex.loadDictionary("AnalysisToolsDict")
aantGate = PyCintex.Namespace('').AANTTreeGate
def aantConvFunc():
    return aantGate.getTree()

EventSelector = Service( "EventSelector" )
EventSelector.Converter = "aantConvFunc"
    
