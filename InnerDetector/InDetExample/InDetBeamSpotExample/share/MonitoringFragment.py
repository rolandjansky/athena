# $Id: MonitoringFragment.py 452329 2011-08-05 09:09:52Z gwilliam $
#
# Job option fragment to add beam spot monitoring to JobRunner templates.
# NOTE: You should configure the beam spot service as desired when using this fragment.
#
# Written by Juerg Beringer in November 2009.
#
print "InDetBeamSpotExample INFO Using $Id: MonitoringFragment.py 452329 2011-08-05 09:09:52Z gwilliam $"

# Default values (please put a default for EACH jobConfig parameter
# so that the template can be used easily without JobRunner)
if not 'jobConfig' in dir():                              jobConfig = {}
if not 'outputfileprefix' in jobConfig:                   jobConfig['outputfileprefix'] = ''
if not 'monfile' in jobConfig:                            jobConfig['monfile'] = jobConfig['outputfileprefix']+'beamspotmonitoring.root'
if not 'doMonitoringGlobal' in jobConfig:                 jobConfig['doMonitoringGlobal'] = False
if not 'TrackContainer' in jobConfig:                     jobConfig['TrackContainer'] = 'TrackParticleCandidate'
if not 'VertexContainer' in jobConfig:                    jobConfig['VertexContainer'] = 'VxPrimaryCandidate'
if not 'VertexContainerWithBeamConstraint' in jobConfig:  jobConfig['VertexContainerWithBeamConstraint'] = False
if not 'MinTracksPerVtx' in jobConfig:                    jobConfig['MinTracksPerVtx'] = 5
if not 'MinTrackPt' in jobConfig:                         jobConfig['MinTrackPt'] = 500.
if not 'useBeamSpot' in jobConfig:
    # change to True as soon as I have PrimaryVertexMonitoring in as well
    jobConfig['useBeamSpot'] = jobConfig.get('beamspottag','')!='' or jobConfig.get('beamspotfile','')!=''
if not 'jobpostprocsteps' in jobConfig:              jobConfig['jobpostprocsteps'] = ''

# Beam spot monitoring tool from InDetAlignmentMonitoring
#from InDetAlignmentMonitoring.InDetAlignmentMonitoringConf import InDetAlignMonBeamSpot
#ToolSvc += CfgMgr.InDetAlignMonBeamSpot(name = "InDetAlignMonBeamSpot",
#                                        trackContainerName = jobConfig['TrackContainer'],
#                                        vxContainerName = jobConfig['VertexContainer'],
#                                        vxContainerWithBeamConstraint = jobConfig['VertexContainerWithBeamConstraint'])
#print ToolSvc.InDetAlignMonBeamSpot


# If doMonitoringGlobal is set, use standard global monitoring from InDetRecExample. Otherwise
# we set up our own beam spot monitoring that does also work on DPDs.
if not jobConfig['doMonitoringGlobal']:
    # Beam spot monitoring tool from GlobalMonitoring
    #from InDetGlobalMonitoring.InDetGlobalMonitoringConf import InDetGlobalBeamSpotMonTool
    ToolSvc += CfgMgr.InDetGlobalBeamSpotMonTool(name = "InDetGlobalBeamSpotMonTool",
                                                 trackContainerName = jobConfig['TrackContainer'],
                                                 vxContainerName = jobConfig['VertexContainer'],
                                                 vxContainerWithBeamConstraint = jobConfig['VertexContainerWithBeamConstraint'],
                                                 minTracksPerVtx=jobConfig['MinTracksPerVtx'],
                                                 minTrackPt = jobConfig['MinTrackPt'],
                                                 useBeamspot = jobConfig['useBeamSpot'])
    if jobConfig['UseFilledBCIDsOnly']:
        # Selection on non-empty BCIDs
        include("AthenaMonitoring/FilledBunchFilterTool_jobOptions.py")
        #monFilledBunchFilterTool.bunchCrossingTool.FilledBunchNames=[] 
        ToolSvc.InDetGlobalBeamSpotMonTool.FilterTools.append(monFilledBunchFilterTool)

    print ToolSvc.InDetGlobalBeamSpotMonTool
    

    # Monitoring manager and output file
    #from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
    topSequence += CfgMgr.AthenaMonManager(name = "BeamSpotMonManager",
                                           FileKey = "BeamSpotMonitoring",
                                           ManualDataTypeSetup = True,
                                           DataType            = "userDefined", # use this for collision data for now
                                           Environment         = "user",
                                           ManualRunLBSetup    = True,
                                           Run                 = 1,
                                           LumiBlock           = 1)
    #topSequence.BeamSpotMonManager.AthenaMonTools += [ ToolSvc.InDetAlignMonBeamSpot ]
    topSequence.BeamSpotMonManager.AthenaMonTools += [ ToolSvc.InDetGlobalBeamSpotMonTool ]
    print topSequence.BeamSpotMonManager


    #from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += CfgMgr.THistSvc()
    ServiceMgr.THistSvc.Output += [ "BeamSpotMonitoring DATAFILE='%s' OPT='RECREATE'" % jobConfig['monfile'] ]

    # Default postprocessing step(s)
    jobConfig['jobpostprocsteps'] += ' PlotBeamSpotMon'
