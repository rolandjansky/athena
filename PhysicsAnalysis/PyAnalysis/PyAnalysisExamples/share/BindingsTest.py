######################################################################
#
# This script demonstrates how to call a C++ class from Python
# Also how to use PyROOT is shown
#
######################################################################


# load dict for MyCutClass
pylcgdict.load_library("libPyAnalysisExamplesDict")

# instantiate a C++ class from python
cut = g.MyCutClass(30*GeV)

# init application mgr
theApp.initialize()

#import ROOT
import ROOT

# create hist
ha = ROOT.TH1F ('accept','Test Acc',100,0,100000)
hr = ROOT.TH1F ('reject','Test Rej',100,0,100000)

# loop over 5 events
for ievent in range(5):
    # run 1 event
    theApp.nextEvent()
    
    # get ElectronContainer
    econ = PyParticleTools.getElectrons("ElectronCollection")
    for ie in range(len(econ)):
        eobj = econ[ie]
        if cut.isAccepted(eobj):
            print "accept %f" % eobj.pt()
            ha.Fill(eobj.pt())
        else:
            print "reject %f" % eobj.pt()
            hr.Fill(eobj.pt())            

ha.Draw()
