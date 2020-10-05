######################################################################
#
# Retrieve a transient EDM object
#
# This script demonstrates how to see outputs of an Algorithm
#   interactively
#
######################################################################

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# add my Algorithm
from PyAnalysisExamples.PyAnalysisExamplesConf import AthPyEx__MySelectionAlg
topSequence += AthPyEx__MySelectionAlg("MySelectionAlg")

# set parameters
myNewKey = "MyNewCollection"
MySelectionAlg = topSequence.MySelectionAlg
MySelectionAlg.ThrPT = 30*GeV
MySelectionAlg.NewKey = myNewKey

# init application mgr
theApp.initialize()

# run 1 event
theApp.nextEvent()

# get transient Electrons which are recorded into SG by MySelectonAlg
econ = PyParticleTools.getElectrons(myNewKey)
for ie in range(len(econ)):
    print "pt %f" % econ[ie].pt()

# load dict for MyDetObj
pylcgdict.load_library("libPyAnalysisExamplesDict")

# retrieve an object from DetectorStore
obj = PyKernel.retrieveDet(g.MyDetObj,'myDetObj')
print obj.name()
