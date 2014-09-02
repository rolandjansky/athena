
# Typically this would be used in extension to the VertexTemplate in InDetBeamspotExample.
# Otherwise a simple config dict. is required


if not 'doVertexHists' in dir():
    doVertexHists = False
if not 'doBeamspotHist' in dir():
    doBeamspotHist = True

def createBeamspotFinder(config=jobConfig, containerName = "VxPrimaryCandidate",suffix=""):
    """ Set up a template for creating new beamspot finders. Options can be configures via the config dict """
    import AthenaCommon.CfgMgr as CfgMgr
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    # Extra options that may not be in default jobConfig

    if not 'MinVertexProb' in config:
        config['MinVertexProb'] = 0.01
    if not 'MaxVtxChi2' in config:
        config['MaxVtxChi2']    = 100 

    if not 'FixParK' in config:
        config['FixParK'] = False

    if not 'MaxSigmaTr' in config:
        config['MaxSigmaTr'] = 100.
    if not 'MaxVtxErrTr' in config:
        config['MaxVtxErrTr'] = 100.
    if not 'OutlierChi2Tr' in config:
        config['OutlierChi2Tr'] = 50.

    
    InDetBeamSpotVertex = CfgMgr.InDet__InDetBeamSpotVertex(name= 'InDetBeamSpotVertex_'+containerName+suffix,
                                                            VertexContainer = containerName,
                                                            VertexTypes     = config['VertexTypes'],
                                                            MinTracksPerVtx = config['MinTracksPerVtx'], 
                                                            MinVtxNum       = config['MinVtxNum'],
                                                            MaxOutlierLoops = 30,
                                                            OutlierMaxRejection = 30,
                                                            OutlierWidthFail= 5.1e-3, # in mm
                                                            OutlierRhoFail  = 0.8,
                                                            DoHists         = doVertexHists,
                                                            OutputLevel     = min(INFO,config['outputlevel']),
                                                            VertexTreeName  = "Vertices_"+containerName+suffix,
                                                            MinVertexProb   = config['MinVertexProb'],
                                                            MaxVtxChi2      = config['MaxVtxChi2'],
                                                            MaxSigmaTr      = config['MaxSigmaTr'] ,
                                                            MaxVtxErrTr     = config['MaxVtxErrTr'] ,
                                                            OutlierChi2Tr   = config['OutlierChi2Tr']
                                                            )
    ToolSvc += InDetBeamSpotVertex
    # Will be automatically printed as part of InDetBeamSpotFinder printout
    # print ToolSvc.InDetBeamSpotVertex
    
    # from InDetBeamSpotFinder.InDetBeamSpotFinderConf import InDet__InDetBeamSpotDbWriterTool
    InDetBeamSpotDbWriterTool = CfgMgr.InDet__InDetBeamSpotDbWriterTool(name        = 'InDetBeamSpotDbWriterTool_'+containerName+suffix,
                                                                        OutputLevel = min(INFO,config['outputlevel']),
                                                                        TreeName    = "COOLBeamspot_"+containerName+suffix,
                                                                        Tag         = containerName+suffix
                                                                        )
    ToolSvc += InDetBeamSpotDbWriterTool
    print ToolSvc.InDetBeamSpotDbWriterTool
    
    #from InDetBeamSpotFinder.InDetBeamSpotFinderConf import InDet__InDetBeamSpotFinder as InDetBeamSpotFinder
    topSequence += CfgMgr.InDet__InDetBeamSpotFinder(name                = 'InDetBeamSpotFinder_'+containerName+suffix,
                                                     BeamSpotTool        = InDetBeamSpotVertex,
                                                     BeamSpotWriterTool  = InDetBeamSpotDbWriterTool,
                                                     MaxCount            = config['MaxCount'],
                                                     LumiRange           = config['LumiRange'],
                                                     LumiBlockRanges     = config['LumiBlockRanges'],
                                                     RunRange            = config['RunRange'],
                                                     EventRange          = config['EventRange'],
                                                     #ForceRunNumber      = 52280,
                                                     DoHists             = doBeamspotHist,
                                                     WriteDb             = False,
                                                     UseDefaultValues    = True,
                                                     #WriteFailed         = True,
                                                     Default_SigmaX      = 30.0,
                                                     Default_SigmaY      = 30.0,
                                                     Default_SigmaZ      = 500.0,
                                                     Default_SigmaXY     = 0.0,
                                                     OutputLevel         = min(INFO,config['outputlevel']),
                                                     BeamSpotRootName    = "Beamspots_"+containerName+suffix
                                                     )
    try:
        topSequence.InDetBeamSpotFinder.UseLBFromViewed   = config['UseLBFromViewed']
        topSequence.InDetBeamSpotFinder.UseLBFromAccepted = config['UseLBFromAccepted']
    except:
        print 'ERROR: You are using an older version of InDetBeamSpotFinder - please update to InDetBeamSpotFinder-01-00-29 or later'
    print topSequence.InDetBeamSpotFinder
        
# Get the list of additional vertex collections in the input file
pvcollections_keys = []

input_file_items = inputFileSummary['eventdata_items']
for cxx_type,sg_key in input_file_items:
    if cxx_type != 'VxContainer':
        continue
    #if sg_key == 'VxPrimaryCandidate':
    #    continue  # ignore the default collection
    
    pvcollections_keys.append(sg_key)
print "JW Found VxContainers:"
print pvcollections_keys



# create the beamspot finder algorithms for each collection
if not 'doMultiVertex' in dir():
    doMultiVertex = True
if doMultiVertex:
    for i in pvcollections_keys:
        print "Creating beamspotFinding for collection:", i
        createBeamspotFinder(jobConfig, i, "")


doCutEvaluation = True

if doCutEvaluation:
    # set the default configuation
    config = jobConfig
    vx     = 'VxPrimaryCandidate'
    cuts = {}
    if vx not in pvcollections_keys:
        print "Collection", vx, "not found in file"
        raise NameError("Incorrect VxContainer key")
            
    
    #set some default values
    config['MinVertexProb'] =0.0001
    config['MaxVtxChi2']    = 1e6
    config['OutlierChi2Tr'] = 20
    
    print "JW Default Cut evaluation configuration:"
    print config
    
    #each cut is treated as separately
    cuts['MinVertexProb'] = [ 0,0.005,0.01,0.02,0.05, 0.1, 0.2, 0.3,0.4]
    cuts['MaxVtxChi2']    = [1.,2.,3.,3.5,4.,4.5]
    cuts['OutlierChi2Tr'] = range(3,21,1)
    
    keys = cuts.keys()
    print keys
    seq = []
    for k in keys:
        suffix = "CUT"
        for i in cuts[k]:
            suffix = "CUT"
            cstr = str(i)
            cstr = cstr.replace('.','p')
            cstr = cstr.replace(',','c')
            cstr = cstr.replace('-','m')
            cstr = cstr.replace('[','l')
            cstr = cstr.replace(']','r')
            suffix = suffix+"_"+k+"_"+cstr
            
            print suffix
            myConfig = config.copy()
            myConfig[k] = i

            createBeamspotFinder(myConfig,vx , suffix)

