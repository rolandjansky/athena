# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def CaloIsolationWrapperTool(PAUcaloIsolationTool=None,
                             DoJetAreaCorrections=True,
                             DoSmearing=True,
                             DoScaling=True):
    from AthenaCommon.AppMgr import ToolSvc

    # Get the energy-density correction tool, and configure the low-pt jet finder if necessary
    if PAUcaloIsolationTool == None:
        from PhotonAnalysisUtils.CaloIsolationTool import CaloIsolationTool
        EDcaloIsoTool=CaloIsolationTool(DoJetAreaCorrections=DoJetAreaCorrections)
    else:
        EDcaloIsoTool = PAUcaloIsolationTool

    if DoJetAreaCorrections:
        from PhotonAnalysisUtils.LowPtJetFinder import LowPtJetFinder
        LowPtJetFinder()

    # get the tool that will calculate the raw topo-isolation energy for egamma objects
    from egammaCaloTools.egammaTopoIsoToolBase   import egammaTopoIsoToolBase
    egammaTopoIsoTool = egammaTopoIsoToolBase("egammaEMTopoIsoToolBase")
    egammaTopoIsoTool.UseEMScale = True # this is the default value, but set it anyway
    ToolSvc += egammaTopoIsoTool


    IsMC11 = False
    IsMC12 = False
    IsData11 = False
    IsData12 = False

    from RecExConfig.InputFilePeeker import inputFileSummary
    if (inputFileSummary['evt_type'][0]!='IS_SIMULATION'):
        if int(inputFileSummary['run_number'][0]) > 200000:
            IsMC12 = True
        else:
            IsMC11 = True
    else:
        if int(inputFileSummary['run_number'][0]) > 200000:
            IsData12 = True
        else:
            IsData11 = True
            
        
    
    # get the tool that wraps the calls to CaloIsoCorrection, for convenience
    from egammaAnalysisTools.egammaAnalysisToolsConf import CaloIsolationWrapperTool as CIWT
    caloIsoTool=CIWT(PAUcaloIsolationTool=EDcaloIsoTool,
                     TopoIsoTool = egammaTopoIsoTool,
                     IsMC11 = IsMC11,
                     IsMC12 = IsMC12,
                     IsData11 = IsData11,
                     IsData12 = IsData12,
                     DoSmearing = DoSmearing,
                     DoScaling  = DoScaling)
    caloIsoTool.TopoIsoFromAOD = True # take the topo-iso info from the AOD if it's available

    ToolSvc += caloIsoTool
    
    return caloIsoTool
