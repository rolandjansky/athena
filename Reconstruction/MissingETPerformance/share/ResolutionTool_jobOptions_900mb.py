include.block( "MissingETPerformance/ResolutionTool_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import \
     ResolutionTool as ConfiguredResolutionTool
ResolutionTool = ConfiguredResolutionTool(

    numptt             =   20,
    startpt            =   5.,
    binpt              =   5.,     
    scares             =   100.,     
    curvasres          =   10.,  
    fitrange           =   3,     
    ns                 =   2.,          
    percerr            =   0.15,
    entries            =   100,    

    iset               =   5001,
    nbinres1           =   100,
    scares1            =   50.,
    nbinres2           =   100,
    scares2            =   500.,
    nbinres3           =   200,
    scares3            =   100.,
    nbinphires1        =   315,

    lowET              = -5.,
    suET               = 95.,

    useTruth           =   0,
    usesumet           =   2,

    )

ToolSvc += ResolutionTool
print      ResolutionTool
