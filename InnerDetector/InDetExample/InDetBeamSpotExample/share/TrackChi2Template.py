# $Id: TrackChi2Template.py 352081 2011-03-16 12:16:14Z gwilliam $
#
# Top-level job options file to calculate the beamspot from AOD files
# using a JobRunner.
#
# Written by Juerg Beringer in April 2008.
#
print "InDetBeamSpotExample INFO Using $Id: TrackChi2Template.py 352081 2011-03-16 12:16:14Z gwilliam $"


# Default values (please put a default for EACH jobConfig parameter
# so that the template can be used easily without JobRunner)
# See also fragments included below for further jobConfig parameters
if not 'jobConfig' in dir():                         jobConfig = {}
if not 'outputlevel' in jobConfig:                   jobConfig['outputlevel'] = WARNING
if not 'outputfileprefix' in jobConfig:              jobConfig['outputfileprefix'] = ''
if not 'outputfile' in jobConfig:                    jobConfig['outputfile'] = jobConfig['outputfileprefix']+'beamspot.db'
if not 'histfile' in jobConfig:                      jobConfig['histfile'] = jobConfig['outputfileprefix']+'nt.root'
if not 'LumiRange' in jobConfig:                     jobConfig['LumiRange'] = -1
if not 'LumiBlockRanges' in jobConfig:               jobConfig['LumiBlockRanges'] = []
if not 'UseLBFromViewed' in jobConfig:               jobConfig['UseLBFromViewed'] = True
if not 'UseLBFromAccepted' in jobConfig:             jobConfig['UseLBFromAccepted'] = False
if not 'MaxCount' in jobConfig:                      jobConfig['MaxCount'] = -1
if not 'RunRange' in jobConfig:                      jobConfig['RunRange'] = 1
if not 'EventRange' in jobConfig:                    jobConfig['EventRange'] = -1
if not 'TrackContainer' in jobConfig:                jobConfig['TrackContainer'] = 'TrackParticleCandidate'
if not 'nTracks_0' in jobConfig:                     jobConfig['nTracks_0'] = 500
if not 'nTracks_1' in jobConfig:                     jobConfig['nTracks_1'] = 2000
if not 'nTracks' in jobConfig:                       jobConfig['nTracks'] = 5000
if not 'PtCut' in jobConfig:                         jobConfig['PtCut'] = 2000.
if not 'nBLayer_Q_cut' in jobConfig:                 jobConfig['nBLayer_Q_cut'] = 1
if not 'PixShared_Q_cut' in jobConfig:               jobConfig['PixShared_Q_cut'] = 0
if not 'PixHoles_Q_cut' in jobConfig:                jobConfig['PixHoles_Q_cut'] = 0
if not 'ChiOvDof_Q_cut' in jobConfig:                jobConfig['ChiOvDof_Q_cut'] = 10.
if not 'doMonitoring' in jobConfig:                  jobConfig['doMonitoring'] = False
if not 'jobpostprocsteps' in jobConfig:              jobConfig['jobpostprocsteps'] = ''


# General job setup
include("InDetBeamSpotExample/AutoConfFragment.py")
include("InDetBeamSpotExample/ReadInDetRecFragment.py")
include("InDetBeamSpotExample/JobSetupFragment.py")


# Configure beam spot algorithm
#from InDetBeamSpotFinder.InDetBeamSpotFinderConf import InDet__InDetBeamSpotTrackChi2
ToolSvc += CfgMgr.InDet__InDetBeamSpotTrackChi2(name            = 'InDetBeamSpotTrackChi2',
                                                TrackContainer  = jobConfig['TrackContainer'],
                                                nTracks_0       = jobConfig['nTracks_0'],
                                                nTracks_1       = jobConfig['nTracks_1'],
                                                nTracks         = jobConfig['nTracks'],
                                                ErrorFactor_0   = 100.0,
                                                ErrorFactor_1   = 10.0,
                                                PtCut           = jobConfig['PtCut'],
                                                D0Match_0_Cut   = 1000.0,
                                                D0Match_1_Cut   = 10.0,
                                                D0Match_2_Cut   = 3.0,
                                                nBLayer_Q_cut   = jobConfig['nBLayer_Q_cut'],
                                                PixShared_Q_cut = jobConfig['PixShared_Q_cut'],
                                                PixHoles_Q_cut  = jobConfig['PixHoles_Q_cut'],
                                                ChiOvDof_Q_cut  = jobConfig['ChiOvDof_Q_cut'],
                                                OutputLevel     = min(INFO,jobConfig['outputlevel']))
print ToolSvc.InDetBeamSpotTrackChi2

#from InDetBeamSpotFinder.InDetBeamSpotFinderConf import InDet__InDetBeamSpotDbWriterTool
ToolSvc += CfgMgr.InDet__InDetBeamSpotDbWriterTool(name        = 'InDetBeamSpotDbWriterTool',
                                                   Tag         = "nominal",
                                                   OutputLevel = min(INFO,jobConfig['outputlevel']))
print ToolSvc.InDetBeamSpotDbWriterTool

#from InDetBeamSpotFinder.InDetBeamSpotFinderConf import InDet__InDetBeamSpotFinder as InDetBeamSpotFinder
topSequence += CfgMgr.InDet__InDetBeamSpotFinder(name                = 'InDetBeamSpotFinder',
                                                 BeamSpotTool        = ToolSvc.InDetBeamSpotTrackChi2,
                                                 BeamSpotWriterTool  = ToolSvc.InDetBeamSpotDbWriterTool,
                                                 MaxCount            = jobConfig['MaxCount'],
                                                 LumiRange           = jobConfig['LumiRange'],
                                                 LumiBlockRanges     = jobConfig['LumiBlockRanges'],
                                                 RunRange            = jobConfig['RunRange'],
                                                 EventRange          = jobConfig['EventRange'],
                                                 #ForceRunNumber      = 52280,
                                                 DoHists             = True,
                                                 WriteDb             = True,
                                                 UseDefaultValues    = True,
                                                 #WriteFailed         = True,
                                                 Default_SigmaX      = 30.0,
                                                 Default_SigmaY      = 30.0,
                                                 Default_SigmaZ      = 500.0,
                                                 Default_SigmaXY     = 0.0,
                                                 OutputLevel         = min(INFO,jobConfig['outputlevel']))
try:
    topSequence.InDetBeamSpotFinder.UseLBFromViewed   = jobConfig['UseLBFromViewed']
    topSequence.InDetBeamSpotFinder.UseLBFromAccepted = jobConfig['UseLBFromAccepted']
except:
    print 'ERROR: You are using an older version of InDetBeamSpotFinder - please update to InDetBeamSpotFinder-01-00-29 or later'
print topSequence.InDetBeamSpotFinder


# Output SQLite and ROOT files
#from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
ServiceMgr += CfgMgr.IOVDbSvc()
ServiceMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=%(outputfile)s;dbname=BEAMSPOT" % jobConfig

#from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += CfgMgr.THistSvc()
ServiceMgr.THistSvc.Output+=[ "INDETBEAMSPOTFINDER DATAFILE='%s' OPT='RECREATE'" % jobConfig['histfile'] ]


# Standard beam spot monitoring
if jobConfig['doMonitoring']:
    include("InDetBeamSpotExample/MonitoringFragment.py")


# Default postprocessing step(s)
jobConfig['jobpostprocsteps'] += ' MergeNt PlotBeamSpot LinkResults AveBeamSpot DQBeamSpot'


# Print jobConfig parameters
include("InDetBeamSpotExample/PrintConfFragment.py")
