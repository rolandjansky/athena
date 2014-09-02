# $Id: MonitoringTemplate.py 360886 2011-04-21 15:05:50Z gwilliam $
#
# Top-level job options file to run beam spot monitoring using a JobRunner
# on ESD, AOD or beam spot DPD files.
#
# Written by Juerg Beringer in July 2008.
#
print "InDetBeamSpotExample INFO Using $Id: MonitoringTemplate.py 360886 2011-04-21 15:05:50Z gwilliam $"

# Default values (please put a default for EACH jobConfig parameter
# so that the template can be used easily without JobRunner)
# See also fragments included below for further jobConfig parameters
if not 'jobConfig' in dir():                         jobConfig = {}
if not 'outputlevel' in jobConfig:                   jobConfig['outputlevel'] = WARNING
if not 'jobpostprocsteps' in jobConfig:              jobConfig['jobpostprocsteps'] = ''
if not 'doVertexSplitting' in jobConfig:             jobConfig['doVertexSplitting'] = False

# General job setup
include("InDetBeamSpotExample/AutoConfFragment.py")
include("InDetBeamSpotExample/ReadInDetRecFragment.py")
include("InDetBeamSpotExample/JobSetupFragment.py")

if jobConfig['doVertexSplitting'] :
    include("InDetBeamSpotExample/VxSplitValFragment.py")

# DEBUG output level or lower only: run InDetBeamSpotReader
if jobConfig['outputlevel'] <= DEBUG:
    #from InDetBeamSpotFinder.InDetBeamSpotFinderConf import InDet__InDetBeamSpotReader
    topSequence += CfgMgr.InDet__InDetBeamSpotReader(name = 'InDetBeamSpotReader',
                                                     OutputLevel = INFO)
    print topSequence.InDetBeamSpotReader


# Monitoring fragment (if doMonitoringGlobal is set, monitoring instead be done
# by ReadInDetRecFragment.py above)
include("InDetBeamSpotExample/MonitoringFragment.py")


# Default postprocessing step(s)
jobConfig['jobpostprocsteps'] += ' LinkResults'


# Print jobConfig parameters
include("InDetBeamSpotExample/PrintConfFragment.py")
