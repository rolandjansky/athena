# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
Tools configurations for ISF for ISF_ParSimServicesConfig
Miha Muskinja, 14/01/2015
"""

from AthenaCommon.CfgGetter import getPublicTool

from ISF_ParSimServices.ISF_ParSimJobProperties import ISF_ParSimFlags
from ISF_ParSimServices.ISF_ParSimTuning        import ISF_ParSimTuningFlags


def getISPtoPerigeeTool(name="ISF_ISPtoPerigeeTool", **kwargs):
    kwargs.setdefault("OutputLevel"                , ISF_ParSimFlags.OutputLevelGeneral() )

    from ISF_ParSimTools.ISF_ParSimToolsConf import iParSim__ISPtoPerigeeTool
    return iParSim__ISPtoPerigeeTool(name, **kwargs )


def getDefaultSmearer(name="ISF_DefaultSmearer", **kwargs):
    kwargs.setdefault("OutputLevel"            , ISF_ParSimFlags.OutputLevelGeneral() )

    from ISF_ParSimTools.ISF_ParSimToolsConf import iParSim__DefaultSmearer
    return iParSim__DefaultSmearer(name, **kwargs )

def getMuonSmearer(name="ISF_MuonSmearer", **kwargs):
    kwargs.setdefault("OutputLevel"               , ISF_ParSimFlags.OutputLevelGeneral() )
    kwargs.setdefault("MuonSmearerMode"           , ISF_ParSimTuningFlags.MuonSmearerMode() )
    kwargs.setdefault("filenameMC12MuonPtBins"    , ISF_ParSimTuningFlags.MC12MuonSmearerPtBinsFileName() )
    kwargs.setdefault("filenameMC12MuonEtaBins"   , ISF_ParSimTuningFlags.MC12MuonSmearerEtaBinsFileName() )
    kwargs.setdefault("filenamesMC12MuonSmearer"  , ISF_ParSimTuningFlags.MC12MuonSmearerParametrisationFileNames() )

    from ISF_ParSimTools.ISF_ParSimToolsConf import iParSim__MuonSmearer
    return iParSim__MuonSmearer(name, **kwargs )

def getElectronSmearer(name="ISF_ElectronSmearer", **kwargs):
    kwargs.setdefault("OutputLevel"              , ISF_ParSimFlags.OutputLevelGeneral() )

    from ISF_ParSimTools.ISF_ParSimToolsConf import iParSim__ElectronSmearer
    return iParSim__ElectronSmearer(name, **kwargs )

def getPionSmearer(name="ISF_PionSmearer", **kwargs):
    kwargs.setdefault("OutputLevel"      , ISF_ParSimFlags.OutputLevelGeneral() )

    from ISF_ParSimTools.ISF_ParSimToolsConf import iParSim__PionSmearer
    return iParSim__PionSmearer(name, **kwargs )

def getTrackParticleSmearer(name="ISF_TrackParticleSmearer", **kwargs):
    kwargs.setdefault("OutputLevel"                        , ISF_ParSimFlags.OutputLevelGeneral() )
    kwargs.setdefault("ISPtoPerigeeTool"                   , getPublicTool('ISF_ISPtoPerigeeTool') )
    kwargs.setdefault("ChargedSmearers"                    , [ getPublicTool('ISF_DefaultSmearer' ),
                                                               getPublicTool('ISF_MuonSmearer'    ),
                                                               getPublicTool('ISF_ElectronSmearer'),
                                                               getPublicTool('ISF_PionSmearer'    )] )

    from ISF_ParSimTools.ISF_ParSimToolsConf import iParSim__TrackParticleSmearer
    return iParSim__TrackParticleSmearer(name, **kwargs )

def getParametricSimServiceID(name="ISF_ParametricSimSvc", **kwargs):
    kwargs.setdefault("Identifier"                       , "ParametricSim")
    kwargs.setdefault("OutputLevel"                      , ISF_ParSimFlags.OutputLevelGeneral() )
    kwargs.setdefault("ParticleSmearers"                 , [getPublicTool('ISF_TrackParticleSmearer')] )

   
    from ISF_ParSimServices.ISF_ParSimServicesConf import iParSim__ParametricSimSvc
    return iParSim__ParametricSimSvc(name, **kwargs )
