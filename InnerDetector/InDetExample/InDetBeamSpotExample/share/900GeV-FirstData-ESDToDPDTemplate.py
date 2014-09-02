# $Id: 900GeV-FirstData-ESDToDPDTemplate.py 252805 2009-12-04 23:30:38Z atlidbs $
#
# Top-level job options file to produce minimal AOD files suitable for beamspot
# determination using a JobRunner from ESDs. In order to account for a beam constraint
# or new alignment constants, pattern recognition and tracking can be redone. The beam
# spot constraint will be disabled. This template uses InDetBeamSpotExample/ReadInDetRecFragment.py,
# which is based on InDetRecExample/ReadInDet_jobOptions.py.
#
# Written by Juerg Beringer in November 2009.
#
print "InDetBeamSpotExample INFO Using $Id: 900GeV-FirstData-ESDToDPDTemplate.py 252805 2009-12-04 23:30:38Z atlidbs $"


# Default values (please put a default for EACH jobConfig parameter
# so that the template can be used easily without JobRunner)
# See also fragments included below for further jobConfig parameters
if not 'jobConfig' in dir():                         jobConfig = {}
if not 'doPrimaryVertexing' in jobConfig:            jobConfig['doPrimaryVertexing'] = True
if not 'primaryVertexSetup' in jobConfig:            jobConfig['primaryVertexSetup'] = 'DefaultFastFinding'
if not 'primaryVertexCutSetup' in jobConfig:         jobConfig['primaryVertexCutSetup'] = 'StartUp'
if not 'doOtherPrimaryVertexers' in jobConfig:       jobConfig['doOtherPrimaryVertexers'] = False
if not 'doMonitoring' in jobConfig:                  jobConfig['doMonitoring'] = True
if not 'doMonitoringGlobal' in jobConfig:            jobConfig['doMonitoringGlobal'] = True


# If we doOtherPrimaryVertexers make sure that primaryVertexSetup is not set (ie use default: AdaptiveMultiFinding)
if jobConfig['doOtherPrimaryVertexers']:
  if 'primaryVertexSetup' in jobConfig:
    del jobConfig['primaryVertexSetup']
    print 'WARNING: Running other vertexers - resetting primaryVertexSetup to default'


# General job setup
include("InDetBeamSpotExample/AutoConfFragment.py")
include("InDetBeamSpotExample/ReadInDetRecFragment.py")
include("InDetBeamSpotExample/JobSetupFragment.py")
topSequence.AthEventCounter.Frequency = 10


# Special configuration for beamspot work
ToolSvc.InDetDetailedTrackSelectorTool.nHitPix = 0
ToolSvc.InDetDetailedTrackSelectorTool.nHitSct = 4
ToolSvc.InDetDetailedTrackSelectorTool.nHitSi = 0
#ToolSvc.InDetDetailedTrackSelectorTool.nHitTrt = 10
#ToolSvc.InDetDetailedTrackSelectorTool.nHitTrt = 0
ToolSvc.InDetDetailedTrackSelectorTool.pTMin = 0
#ToolSvc.InDetDetailedTrackSelectorTool.useTrackQualityInfo = False
#ToolSvc.InDetDetailedTrackSelectorTool.IPd0Max = 5
#ToolSvc.InDetDetailedTrackSelectorTool.sigIPd0Max = 5.0
#ToolSvc.InDetDetailedTrackSelectorTool.d0significanceMax = -1
#ToolSvc.InDetDetailedTrackSelectorTool.sigIPz0Max = 10.0

InDetPriVxFinderTool.enableMultipleVertices = 0

print ToolSvc.InDetDetailedTrackSelectorTool


# If desired, run other primary vertexers using InDetVertexRTT/InDetVertexRTT_RerunOtherPrimarVertexers.py
if jobConfig['doOtherPrimaryVertexers']:
  include("InDetVertexRTT/InDetVertexRTT_RerunOtherPrimarVertexers.py")
  StreamAOD.ItemList += [ 'VxContainer#VxPrimaryCandidate*' ]


# Standard beam spot monitoring
if jobConfig['doMonitoring']:
    include("InDetBeamSpotExample/MonitoringFragment.py")


# Print jobConfig parameters
include("InDetBeamSpotExample/PrintConfFragment.py")
