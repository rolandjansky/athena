include.block( "MissingETPerformance/BasicPlots_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import \
     BasicPlotsTool as ConfiguredBasicPlotsTool
BasicPlotsTool = ConfiguredBasicPlotsTool(

    plotJetVariables      = False,
    plotClusterVariables  = False,
    iset             = 5001,
    nbinp            = 100,
    suptmi           = 500.,
    nbinE            = 100,
    lowET            = 0.,
    nbinpxy          = 100,
    suptmixy         = 250.,
    nbinphi          = 63,
    binphi           = 3.15,
    nbinsig          = 100,
    binsig           = 50.,
    nbineta          = 50,
    bineta           = 5.0,
    regions_ncells   = 60000,
    regions_nbins_ncells = 6000,
    suET             = 2500.,
    nbinJetEt        = 500,
    lowerbinJetEt    = 0,
    upperbinJetEt    = 500.,
    nbinClusterEt    = 200,
    lowerbinClusterEt= 0.,
    upperbinClusterEt= 200.,
    nbinBCID         = 10000,
    lowBCID          = 0.,
    upBCID           = 500.,
    nbinMU           = 100,
    lowMU            =  0.,
    upMU             = 50.,
    EventInfoKey     = "ByteStreamEventInfo",
    FolderName       = "",
    )

ToolSvc += BasicPlotsTool
print      BasicPlotsTool
