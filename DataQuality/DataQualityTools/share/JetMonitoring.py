#svcMgr.MessageSvc.OutputLevel = DEBUG

from JetRec.JetRecConf import JetMomentCalculator

MomentCalculator = JetMomentCalculator( "MomentCalculator" )
MomentCalculator.CalculatorToolNames = []
MomentCalculator.OutputLevel = VERBOSE

from JetRecTools.JetRecToolsConf import JetClusterEMFractionTool
from JetSimTools.JetSimToolsConf import JetTruthEMFractionTool

MomentCalculator.JetCollectionName = "Cone7TopoJets"
MomentCalculator.JetMomentMapName =  "Cone7H1TopoMap"

if(True):
    clem = JetClusterEMFractionTool("ClusterEMFraction")
    MomentCalculator.CalculatorToolNames += [ clem.getFullName() ]
    
if(False):
    MomentCalculator.CalculatorToolNames += [
        "JetTrackEMFractionTool/TrackEMFraction" ]
    MomentCalculator.TrackEMFraction.TrackFilterTool = "JetTrackEtaFilter/TEFilter"
    MomentCalculator.TrackEMFraction.TrackAssociationTool = "JetTrackToCaloAssoc/TAssoc"
    MomentCalculator.TrackEMFraction.TrackContainerKey = "TrackParticleCandidate"
    MomentCalculator.TrackEMFraction.TAssoc.TrackExtrapolatorToolName="ExtrapolTrackToCaloTool/caloObjBuilderTrackToCaloTool"
    MomentCalculator.TrackEMFraction.ComputeTrackMoments=True
    include( "TrackToCalo/ExtrapolToCaloTool_joboptions.py" )
    myExtrapolator = ConfiguredExtrapolator()
    
if(True):
    trem = JetTruthEMFractionTool("TruthEMFraction")
    MomentCalculator.CalculatorToolNames += [ trem.getFullName() ]

print MomentCalculator

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += MomentCalculator

import math

# -- 1-dim binning
radBins  = [  0.0,        2.0,      0.025     ] # radial bins
eBins    = [  0.0*GeV, 1500.0*GeV, 10.0*GeV   ] # energy bins
etaBins  = [ -5.0,        5.0,      0.1       ] # eta bins
phiBins  = [ -math.pi,   math.pi,   0.1       ] # phi bins
massBins = [  0.0*GeV,  100.0*GeV,  1.0* GeV  ] # mass bins

# -- 2-dim binning
radBinsHD  = [  0.0,        2.0,      0.1      ]
eBinsHD    = [  0.0*GeV, 1500.0*GeV, 20.0*GeV  ]
etaBinsHD  = [ -5.0,        5.0,      0.1      ]
phiBinsHD  = [ -math.pi,   math.pi,   0.1      ]
massBinsHD = [  0.0*GeV,  100.0*GeV,  2.0*GeV  ]

from JetRec.JetGetters import *
from JetMonitoring.JetMonitoringConf import JetDisplayTool
from JetMonitoring.JetMonitoringConf import JetEtFlowMonTool
from JetMonitoring.JetMonitoringConf import JetMomentMonitorTool
from JetMonitoring.JetMonitoringConf import JetMonitorTool
from JetMonitoring.JetMonitoringConf import JetManagedMonTool
from JetMonitoring.JetMonitoringConf import JetMonTool

# momon.MomentBinning = [
#     "TRACK_E 0.0 600 60"
#     "CALOAREA 0.0 1.2 120"
#     "EM_FRACTION_MCTRUTH 0.0 1.2 120"
#     ]


# momon2.MomentBinning = [
#     "TRACK_E 0.0 600 60"
#     "CALOAREA 0.0 1.2 120"
#     "EM_FRACTION_MCTRUTH 0.0 1.2 120"
#     ]

shmon = JetEtFlowMonTool("JetShapeMonitor")
shmon.HistogramPath = "/JetMonitor/Cone7TopoJets"
shmon.RadBinning1D =radBins

kimon = JetMonitorTool("JetKineMonitor")
kimon.HistogramPath = "/JetMonitor/Cone7TopoJets"
kimon.EnergyUnitOfMeasure = GeV
kimon.EnergyBinning1D = eBins
kimon.EtBinning1D     = eBins
kimon.EtaBinning1D    = etaBins
kimon.PhiBinning1D    = phiBins
kimon.RadBinning1D    = radBins
kimon.MassBinning1D   = massBins
kimon.EnergyBinning2D = eBinsHD
kimon.EtBinning2D     = eBinsHD
kimon.EtaBinning2D    = etaBinsHD
kimon.PhiBinning2D    = phiBinsHD
kimon.RadBinning2D    = radBinsHD
kimon.MassBinning2D   = massBinsHD

displ = JetManagedMonTool( "JetManagedMonTool" )

displ.ToolNames = [ shmon.getFullName(), kimon.getFullName()]
displ.JetCollection = "Cone7TopoJets"
#displ.OutputLevel = VERBOSE
displ += shmon
displ += kimon
#displ += momon
ToolSvc += displ

print displ

# get the prime minister aeh... 
pm = AthenaMonManager( "PrimaryManager" )
pm.AthenaMonTools += [ displ.getFullName() ]

print pm

#shmon.OutputLevel = VERBOSE
#kimon.OutputLevel = VERBOSE
