# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: VertexTemplate.py 601782 2014-06-14 00:00:08Z btamadio $
#
# Top-level job options file to run the vertex-based beamspot algorithm
# from AOD files using a JobRunner.
#
# Written by Juerg Beringer in July 2008.
#
print "InDetBeamSpotExample INFO Using $Id: VertexTemplate.py 601782 2014-06-14 00:00:08Z btamadio $"



# Default values (please put a default for EACH jobConfig parameter
# so that the template can be used easily without JobRunner)
# See also fragments included below for further jobConfig parameters
if not 'jobConfig' in dir():                         jobConfig = {}
if not 'outputlevel' in jobConfig:                   jobConfig['outputlevel'] = WARNING
if not 'WriteCOOL' in jobConfig:                     jobConfig['WriteCOOL'] = True if jobConfig.get('outputfile',False) else False 
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
if not 'SeparateByBCID' in jobConfig:                jobConfig['SeparateByBCID'] = False
if not 'VertexContainer' in jobConfig:               jobConfig['VertexContainer'] = 'VxPrimaryCandidate'
if not 'VertexTypes' in jobConfig:                   jobConfig['VertexTypes'] = ['PriVtx']
if not 'MinTracksPerVtx' in jobConfig:               jobConfig['MinTracksPerVtx'] = 5
if not 'MaxTransverseErr' in jobConfig:              jobConfig['MaxTransverseErr'] = 1000000
if not 'MinVtxNum' in jobConfig:                     jobConfig['MinVtxNum'] = 100
if not 'MaxSigmaTr' in jobConfig:                    jobConfig['MaxSigmaTr'] = 20.
if not 'OutlierChi2Tr' in jobConfig:                 jobConfig['OutlierChi2Tr'] = 20.
if not 'InitialKFactor' in jobConfig:                jobConfig['InitialKFactor'] = 1.0
if not 'ConstantKFactor' in jobConfig:               jobConfig['ConstantKFactor'] = False
if not 'doMonitoring' in jobConfig:                  jobConfig['doMonitoring'] = True
if not 'jobpostprocsteps' in jobConfig:              jobConfig['jobpostprocsteps'] = ''
if not 'VertexNtuple' in jobConfig:                  jobConfig['VertexNtuple'] = True
if not 'TruncatedRMS' in jobConfig:                  jobConfig['TruncatedRMS'] = True
if not 'SetInitialRMS' in jobConfig:                 jobConfig['SetInitialRMS'] = False


# JobOptions to specify event filtering:
# bcidList    - List of BCIDs to use
# lbList      - List of LBs to use
# lbData      - Name of file with lumi scan info or direct list of lumi scan info


# General job setup
include("InDetBeamSpotExample/AutoConfFragment.py")
include("InDetBeamSpotExample/ReadInDetRecFragment.py")
include("InDetBeamSpotExample/JobSetupFragment.py")

# Event filtering
if 'bcidList' in jobConfig:
    import InDetBeamSpotExample.FilterUtils as FilterUtils
    FilterUtils.filterSeq += FilterUtils.BCIDFilter(jobConfig['bcidList'])
if 'lbList' in jobConfig:
    import InDetBeamSpotExample.FilterUtils as FilterUtils
    FilterUtils.filterSeq += FilterUtils.LBFilter(jobConfig['lbList'])
if 'lbData' in jobConfig:
    import InDetBeamSpotExample.FilterUtils as FilterUtils
    FilterUtils.filterSeq += FilterUtils.LumiBlockFilter(jobConfig['lbData'])
if 'zRange' in jobConfig:
    import InDetBeamSpotExample.FilterUtils as FilterUtils
    FilterUtils.filterSeq += FilterUtils.ZFilter(jobConfig['zRange'])

# Configure beam spot algorithm
ToolSvc += CfgMgr.InDet__InDetBeamSpotVertex(name            = 'InDetBeamSpotVertex',
                                             MaxSigmaTr      = jobConfig['MaxSigmaTr'],
                                             MaxVtxErrTr     = 100.,
                                             OutlierChi2Tr   = jobConfig['OutlierChi2Tr'],
                                             MaxOutlierLoops = 30,
                                             OutlierMaxRejection = 30,
                                             OutlierWidthFail= 5.1e-3, # in mm
                                             OutlierRhoFail  = 0.8,
                                             InitParK        = jobConfig['InitialKFactor'],
                                             FixParK         = jobConfig['ConstantKFactor'],
                                             TruncatedRMS    = jobConfig['TruncatedRMS'],
                                             SetInitialRMS   = jobConfig['SetInitialRMS'],
                                             OutputLevel     = min(INFO,jobConfig['outputlevel']))

if jobConfig['SetInitialRMS']:
    ToolSvc.InDetBeamSpotVertex.InitParSigmaX   = 0.10
    ToolSvc.InDetBeamSpotVertex.InitParSigmaY   = 0.10

ToolSvc += CfgMgr.InDet__InDetBeamSpotDbWriterTool(name        = 'InDetBeamSpotDbWriterTool',
                                                   Tag         = "nominal",
                                                   WriteCOOL   = jobConfig['WriteCOOL'],
                                                   OutputLevel = min(INFO,jobConfig['outputlevel']))
print ToolSvc.InDetBeamSpotDbWriterTool


ToolSvc += CfgMgr.InDet__InDetBeamSpotRooFit(name            = 'InDetBeamSpotRooFit',
                                             OutputLevel     = min(INFO,jobConfig['outputlevel']))

if jobConfig['SetInitialRMS']:
    ToolSvc.InDetBeamSpotRooFit.InitParSigmaX   = 0.10
    ToolSvc.InDetBeamSpotRooFit.InitParSigmaY   = 0.10

topSequence += CfgMgr.InDet__InDetBeamSpotFinder(name                = 'InDetBeamSpotFinder',
                                                 BeamSpotToolList    = [ToolSvc.InDetBeamSpotRooFit,ToolSvc.InDetBeamSpotVertex],
                                                 BeamSpotWriterTool  = ToolSvc.InDetBeamSpotDbWriterTool,
                                                 MaxCount            = jobConfig['MaxCount'],
                                                 LumiRange           = jobConfig['LumiRange'],
                                                 LumiBlockRanges     = jobConfig['LumiBlockRanges'],
                                                 RunRange            = jobConfig['RunRange'],
                                                 EventRange          = jobConfig['EventRange'],
                                                 SeparateByBCID      = jobConfig['SeparateByBCID'],
                                                 UseFilledBCIDsOnly  = jobConfig['UseFilledBCIDsOnly'],
                                                 DoHists             = True,
                                                 WriteDb             = True,
                                                 UseDefaultValues    = True,
                                                 Default_SigmaX      = 30.0,
                                                 Default_SigmaY      = 30.0,
                                                 Default_SigmaZ      = 500.0,
                                                 Default_SigmaXY     = 0.0,                                                 
                                                 VertexTypes         = jobConfig['VertexTypes'],
                                                 MinTracksPerVtx     = jobConfig['MinTracksPerVtx'],
                                                 MinVtxNum           = jobConfig['MinVtxNum'],
                                                 MaxTransverseErr    = jobConfig['MaxTransverseErr'],
                                                 OutputLevel         = min(INFO,jobConfig['outputlevel']))


if jobConfig['UseFilledBCIDsOnly']:
    # Bunch crossing tool to allow selecting on non-empty BCIDs
    from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
    bunchCrossingTool =  BunchCrossingTool()
    #bunchCrossingTool.FilledBunchNames=[] 
    topSequence.InDetBeamSpotFinder.BCTool = bunchCrossingTool

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
