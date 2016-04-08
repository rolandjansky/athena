# read AANT
include( "AnalysisTools/AANTEventSelector_jobOptions.py" )

EventSelector = Service( "EventSelector" )
EventSelector.InputCollections = [ "user.KyleCranmer.eleJetStudy.Wenu.cone7.etoe.AANT1._00003.root" ]

# callback
def selection(tree):
    print "PyExecute begin"
    print tree.All_E.size()
    for i in range (tree.All_E.size()):
        print tree.All_E[i]
    if tree.All_E.size() > 2:
        return True
    return False

EventSelector.Selection = "selection"

# Number of Events to process
theApp.EvtMax = 10
