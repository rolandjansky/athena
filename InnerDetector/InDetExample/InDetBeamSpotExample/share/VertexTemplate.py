# Top-level job options file to run the vertex-based beamspot algorithm
# from AOD files using a JobRunner.
#
# Written by Juerg Beringer in July 2008.
#
printfunc ("InDetBeamSpotExample INFO Using VertexTemplate.py atlas/athena")

# Default values (please put a default for EACH jobConfig parameter
# so that the template can be used easily without JobRunner)
# See also fragments included below for further jobConfig parameters
if not 'jobConfig' in dir():                         jobConfig = {}

#Job options
if not 'outputlevel' in jobConfig:                   jobConfig['outputlevel'] = WARNING
if not 'outputfileprefix' in jobConfig:              jobConfig['outputfileprefix'] = ''
if not 'outputfile' in jobConfig:                    jobConfig['outputfile'] = jobConfig['outputfileprefix']+'beamspot.db'
if not 'histfile' in jobConfig:                      jobConfig['histfile'] = jobConfig['outputfileprefix']+'nt.root'
if not 'jobpostprocsteps' in jobConfig:              jobConfig['jobpostprocsteps'] = 'MergeNt PlotBeamSpot LinkResults AveBeamSpot DQBeamSpot'
if not 'doMonitoring' in jobConfig:                  jobConfig['doMonitoring'] = True
if not 'VertexNtuple' in jobConfig:                  jobConfig['VertexNtuple'] = True
if not 'WriteAllVertices' in jobConfig:              jobConfig['WriteAllVertices'] = False
if not 'VertexTreeName' in jobConfig:                jobConfig['VertexTreeName'] = 'Vertices'
if not 'BeamSpotToolList' in jobConfig:              jobConfig['BeamSpotToolList'] = ['InDetBeamSpotRooFit','InDetBeamSpotVertex']

#Event selection options
if not 'UseBCID' in jobConfig:                       jobConfig['UseBCID'] = []
if not 'UseFilledBCIDsOnly' in jobConfig:            jobConfig['UseFilledBCIDsOnly'] = True

#Vertex Selection Options
if not 'MinTracksPerVtx' in jobConfig:            jobConfig['MinTracksPerVtx'] = 5
if not 'MaxTracksPerVtx' in jobConfig:            jobConfig['MaxTracksPerVtx'] = 1000000
if not 'MinVtxNum' in jobConfig:                  jobConfig['MinVtxNum'] = 100
if not 'MaxVtxChi2' in jobConfig:                 jobConfig['MaxVtxChi2'] = 10
if not 'MaxTransverseErr' in jobConfig:           jobConfig['MaxTransverseErr'] = 1000000
if not 'VertexTypes' in jobConfig:                jobConfig['VertexTypes'] = ['PriVtx']
if not 'MinVtxProb' in jobConfig:                 jobConfig['MinVtxProb'] = .001
if not 'VertexContainer' in jobConfig:            jobConfig['VertexContainer'] = 'PrimaryVertices'

#Options for sorting vertices into fits
if not 'LumiRange' in jobConfig:                  jobConfig['LumiRange'] = 0
if not 'RunRange' in jobConfig:                   jobConfig['RunRange'] = 0

#if not 'GroupFitsByBCID' in jobConfig:               jobConfig['GroupFitsByBCID'] = False
#if not 'GroupFitsByPileup' in jobConfig:             jobConfig['GroupFitsByPileup'] = False
if not 'GroupFitsBy' in jobConfig:                jobConfig['GroupFitsBy'] = 'none'
if not 'EventRange' in jobConfig:                 jobConfig['EventRange'] = 0

#Fit Options
if not 'InitialKFactor' in jobConfig:             jobConfig['InitialKFactor'] = 1.0
if not 'ConstantKFactor' in jobConfig:            jobConfig['ConstantKFactor'] = False

#Fit Options for official fit only
if not 'MaxSigmaTr' in jobConfig:                    jobConfig['MaxSigmaTr'] = 20.
if not 'TruncatedRMS' in jobConfig:                  jobConfig['TruncatedRMS'] = True
if not 'SetInitialRMS' in jobConfig:                 jobConfig['SetInitialRMS'] = False
if not 'OutlierChi2Tr' in jobConfig:                 jobConfig['OutlierChi2Tr'] = 20.
if not 'BeamSpotToolList' in jobConfig:              jobConfig['BeamSpotToolList'] = ['InDetBeamSpotRooFit','InDetBeamSpotVertex']

#Fit Options for RooFit only
if not 'RooFitMaxTransverseErr' in jobConfig:        jobConfig['RooFitMaxTransverseErr'] = 0.05

if not 'FixWidth' in jobConfig:                      jobConfig['FixWidth'] =  False

# General job setup
from AthenaCommon.ConcurrencyFlags import jobproperties as jp
print ( 'Number of threads: ',jp.ConcurrencyFlags.NumThreads()  )   

include("InDetBeamSpotExample/AutoConfFragment.py")
include("InDetBeamSpotExample/ReadInDetRecFragment.py")
include("InDetBeamSpotExample/JobSetupFragment.py")

if 'UseBCID' in jobConfig and jobConfig['UseBCID'] != []:
    import InDetBeamSpotExample.FilterUtils as FilterUtils
    FilterUtils.filterSeq += FilterUtils.BCIDFilter(jobConfig['UseBCID'])
if 'lbList' in jobConfig:
    import InDetBeamSpotExample.FilterUtils as FilterUtils
    FilterUtils.filterSeq += FilterUtils.LBFilter(jobConfig['lbList'])
if 'lbData' in jobConfig:
    import InDetBeamSpotExample.FilterUtils as FilterUtils
    FilterUtils.filterSeq += FilterUtils.LumiBlockFilter(jobConfig['lbData'])
# if 'zRange' in jobConfig:
#     import InDetBeamSpotExample.FilterUtils as FilterUtils
#     FilterUtils.filterSeq += FilterUtils.ZFilter(jobConfig['zRange'])

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
                                             FixWidth        = jobConfig['FixWidth'],
                                             TruncatedRMS    = jobConfig['TruncatedRMS'],
                                             SetInitialRMS   = jobConfig['SetInitialRMS'],
                                             OutputLevel     = min(INFO,jobConfig['outputlevel']))


printfunc (ToolSvc.InDetBeamSpotVertex)

ToolSvc += CfgMgr.InDet__InDetBeamSpotRooFit(name            = 'InDetBeamSpotRooFit',
                                             OutputLevel     = min(INFO,jobConfig['outputlevel']),
                                             InitialKFactor  = jobConfig['InitialKFactor'],
                                             ConstantKFactor = jobConfig['ConstantKFactor'],
                                             vtxResCut       = jobConfig['RooFitMaxTransverseErr'])

printfunc (ToolSvc.InDetBeamSpotRooFit)
topSequence += CfgMgr.InDet__InDetBeamSpotFinder(name                = 'InDetBeamSpotFinder',
                                                 #job options
                                                 OutputLevel         = min(INFO,jobConfig['outputlevel']),
                                                 VertexNtuple   = jobConfig['VertexNtuple'],
                                                 WriteAllVertices    = jobConfig['WriteAllVertices'],
                                                 VertexTreeName      = jobConfig['VertexTreeName'],
                                                 #Fitting tools to use
                                                 BeamSpotToolList    = [getattr(ToolSvc,tool) for tool in jobConfig['BeamSpotToolList']],
                                                 #Event selection options
                                                 UseBCID       = jobConfig['UseBCID'],
                                                 UseFilledBCIDsOnly  = jobConfig['UseFilledBCIDsOnly'],
                                                 #Vertex Selection options
                                                 VertexContainer     = jobConfig['VertexContainer'],
                                                 MinTracksPerVtx     = jobConfig['MinTracksPerVtx'],
                                                 MaxTracksPerVtx     = jobConfig['MaxTracksPerVtx'],
                                                 MinVtxNum           = jobConfig['MinVtxNum'],
                                                 MaxVtxChi2       = jobConfig['MaxVtxChi2'],
                                                 MaxTransverseErr  = jobConfig['MaxTransverseErr'],
                                                 VertexTypes         = jobConfig['VertexTypes'],
                                                 MinVtxProb       = jobConfig['MinVtxProb'],
                                                 #Beamspot Sorting options
                                                 LumiRange        = jobConfig['LumiRange'],
                                                 RunRange       = jobConfig['RunRange'],
                                                 GroupFitsBy         = jobConfig['GroupFitsBy'],
                                                 EventRange     = jobConfig['EventRange'])


from LumiBlockComps.BunchCrossingCondAlgDefault import BunchCrossingCondAlgDefault
BunchCrossingCondAlgDefault()

ServiceMgr += CfgMgr.THistSvc()
ServiceMgr.THistSvc.Output+=[ "INDETBEAMSPOTFINDER DATAFILE='%s' OPT='RECREATE'" % jobConfig['histfile'] ]

# Standard beam spot monitoring
if jobConfig['doMonitoring']:
    include("InDetBeamSpotExample/MonitoringFragment.py")

# Default postprocessing step(s)
jobConfig['jobpostprocsteps'] += ' MergeNt BeamSpotNt PlotBeamSpot LinkResults AveBeamSpot DQBeamSpot'

# Print jobConfig parameters
include("InDetBeamSpotExample/PrintConfFragment.py")

print(topSequence)
from AthenaCommon.AlgSequence import dumpMasterSequence
dumpMasterSequence()

#New conditions required for Run 2 data
include("PixelConditionsAlgorithms/postInclude.UsePixelModuleLevelMask.py")
