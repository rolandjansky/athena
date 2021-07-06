##########################################################################
#
# Retrieve a persistified EDM object, then retrieve associated objects.
#
# This script demonstrates that the back-navigation is available in
#   an interactive session of athena.py
#
##########################################################################

# init application mgr
theApp.initialize()

# run 1 event
theApp.nextEvent()

# get ElectronContainer from AOD
econ = PyParticleTools.getElectrons("ElectronCollection")
print ("Electron")
print (econ.size())

for i in range(econ.size()):
    eobj = econ[i]
    print ("Electron pt:%f" % eobj.pt())
    # get TrackParticle via ElementLink
    tp = eobj.track()
    print ("TrackParticle px:%f" % tp.px())
    # get egamma via Navigation
    # this should be retrieved from ESD
    eg = eobj.eg()
    print ("egamma eta:%f" % eg.eta())
