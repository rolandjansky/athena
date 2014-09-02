# $Id: ESDToDPDTemplate.py 323418 2010-09-23 23:28:02Z beringer $
#
# Top-level job options file to produce minimal AOD files suitable for beamspot
# determination using a JobRunner from ESDs. In order to account for a beam constraint
# or new alignment constants, pattern recognition and tracking can be redone. The beam
# spot constraint will be disabled. This template uses InDetBeamSpotExample/ReadInDetRecFragment.py,
# which is based on InDetRecExample/ReadInDet_jobOptions.py.
#
# Written by Juerg Beringer in November 2009.
#
print "InDetBeamSpotExample INFO Using $Id: ESDToDPDTemplate.py 323418 2010-09-23 23:28:02Z beringer $"


# Default values (please put a default for EACH jobConfig parameter
# so that the template can be used easily without JobRunner)
# See also fragments included below for further jobConfig parameters
if not 'jobConfig' in dir():                         jobConfig = {}
if not 'doPatternRecoAndTracking' in jobConfig:      jobConfig['doPatternRecoAndTracking'] = False
if not 'doPrimaryVertexing' in jobConfig:            jobConfig['doPrimaryVertexing'] = True

if not 'doOtherPrimaryVertexers' in jobConfig:       jobConfig['doOtherPrimaryVertexers'] = False
if not 'doForceWriteDPD' in jobConfig:               jobConfig['doForceWriteDPD'] = True
if not 'doMonitoring' in jobConfig:                  jobConfig['doMonitoring'] = True
if not 'doMonitoringGlobal' in jobConfig:            jobConfig['doMonitoringGlobal'] = True
if not 'hasMag' in jobConfig:                        jobConfig['hasMag'] = True
if not 'hasSCT' in jobConfig:                        jobConfig['hasSCT'] = True
if not 'hasSCTStandby' in jobConfig:                 jobConfig['hasSCTStandby'] = False
if not 'hasPix' in jobConfig:                        jobConfig['hasPix'] = True
if not 'looseCuts' in jobConfig:                     jobConfig['looseCuts'] = False
if not 'jobpostprocsteps' in jobConfig:              jobConfig['jobpostprocsteps'] = ''

if not 'commissioning' in jobConfig:                 jobConfig['commissioning'] = True
# if not 'primaryVertexSetup' in jobConfig:            jobConfig['primaryVertexSetup'] = 'DefaultFullFinding'
# if not 'primaryVertexCutSetup' in jobConfig:         jobConfig['primaryVertexCutSetup'] = 'StartUp'
# if not 'doPrimaryVertex3DFinding' in jobConfig:      jobConfig['doPrimaryVertex3DFinding'] = True

# Used only if present (through ReadInDetRecFragment.py):
# - primaryVertexSetup        (e.g. AdaptiveMultiFinding,DefaultFastFinding, ...)
# - primaryVertexCutSetup     (e.g. StartUp, ...)
# - doPrimaryVertex3DFinding  (True)


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


# If InDetDetailedTrackSelectorTool was configured by ReadInDetFragment, apply
# special configuration for beamspot work
if hasattr(ToolSvc,'InDetDetailedTrackSelectorTool'):
  if not jobConfig['hasPix']:
    ToolSvc.InDetDetailedTrackSelectorTool.nHitPix = 0
    ToolSvc.InDetDetailedTrackSelectorTool.nHitSi = 0
  if not jobConfig['hasSCT']:
      ToolSvc.InDetDetailedTrackSelectorTool.nHitSct = 0
      ToolSvc.InDetDetailedTrackSelectorTool.nHitSi = 0      
  if jobConfig['hasSCTStandby']:
    # SCT @ 20V - require SCT hits (even if just at 20V)
    ToolSvc.InDetDetailedTrackSelectorTool.nHitSct = 4
    ToolSvc.InDetDetailedTrackSelectorTool.nHitSi = 0
  #ToolSvc.InDetDetailedTrackSelectorTool.nHitTrt = 10
  #ToolSvc.InDetDetailedTrackSelectorTool.nHitTrt = 0
  if not jobConfig['hasMag']:
    ToolSvc.InDetDetailedTrackSelectorTool.pTMin = 0
  #ToolSvc.InDetDetailedTrackSelectorTool.useTrackQualityInfo = False
  #ToolSvc.InDetDetailedTrackSelectorTool.IPd0Max = 25
  #ToolSvc.InDetDetailedTrackSelectorTool.sigIPd0Max = 5.0
  #ToolSvc.InDetDetailedTrackSelectorTool.d0significanceMax = -1
  #ToolSvc.InDetDetailedTrackSelectorTool.sigIPz0Max = 10.0
  if jobConfig['looseCuts']:
    # Very loose cuts to find beam-gas vertices
    ToolSvc.InDetDetailedTrackSelectorTool.IPd0Max = 25
    ToolSvc.InDetDetailedTrackSelectorTool.d0MaxPreselection = 25
    ToolSvc.InDetDetailedTrackSelectorTool.IPz0Max = 500

  print ToolSvc.InDetDetailedTrackSelectorTool


# If InDetPriVxFinderTool is configured, print its configuration
if 'InDetPriVxFinderTool' in dir():
  print InDetPriVxFinderTool


# If desired, run other primary vertexers using InDetVertexRTT/InDetVertexRTT_RerunOtherPrimarVertexers.py
if jobConfig['doOtherPrimaryVertexers']:
  include("InDetVertexRTT/InDetVertexRTT_RerunOtherPrimarVertexers.py")
  StreamAOD.ItemList += [ 'VxContainer#VxPrimaryCandidate*' ]


# Standard beam spot monitoring
if jobConfig['doMonitoring']:
  include("InDetBeamSpotExample/MonitoringFragment.py")

# Default postprocessing step(s)
jobConfig['jobpostprocsteps'] += ' LinkResults'

# Print jobConfig parameters
include("InDetBeamSpotExample/PrintConfFragment.py")
