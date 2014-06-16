
from AthenaCommon.SystemOfUnits import *

from EventShapeRec.EventShapeRecConf import EventEtDensityTester

tester = EventEtDensityTester("EventEtDensityTester")

#from EventShapeTools.EventShapeToolsConf import EventEtClusterDensityTool
from EventShapeTools.EventEtClusterDensityConfig import *

# -- common tool settings
commonDict = dict(getDefaultEtDensityClusterDict())
commonDict['Algorithm']             = "kt"
commonDict['JetAreaMethod']         = "VoronoiArea"
commonDict['VoronoiEffectiveRfact'] = 1.0
commonDict['InclusivePtMin']        = 0.*GeV
commonDict['EtaMin']                = -2.
commonDict['EtaMax']                =  2.

# -- configuration 1: kT, R = 0.3, LW, Voronoi
cadict = dict(commonDict)
cadict['Radius']      = 0.3
cadict['SignalState'] = "CALIBRATED"
catool = configureJetAreaDensity("ClusterRhoKt3LCW",**cadict)
# -- configuration 2: kT, R = 0.4, LW, Voronoi
cbdict = dict(commonDict)
cbdict['Radius']      = 0.4
cbdict['SignalState'] = "CALIBRATED"
cbtool = configureJetAreaDensity("ClusterRhoKt4LCW",**cbdict)
# -- configuration 3: kT, R = 0.3, EM, Voronoi
ccdict = dict(commonDict)
ccdict['Radius']      = 0.3
ccdict['SignalState'] = "UNCALIBRATED"
cctool = configureJetAreaDensity("ClusterRhoKt3EM",**ccdict)
# -- configuration 4: kT, R = 0.4, EM, Voronoi
cddict = dict(commonDict)
cddict['Radius']      = 0.4
cddict['SignalState'] = "UNCALIBRATED"
cdtool = configureJetAreaDensity("ClusterRhoKt4EM",**cddict)

tester.EventEtDensityTools = [ catool.getFullName(), 
                               cbtool.getFullName(), 
                               cctool.getFullName(), 
                               cdtool.getFullName() ]
tester += [ catool, cbtool, cctool, cdtool ]

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += tester

