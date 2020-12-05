# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def FastCaloSimFactory(name="FastCaloSimFactory", **kwargs):

    from AthenaCommon.Logging import logging
    mlog = logging.getLogger( 'FastCaloSimFactory::configure:' )

    from AthenaCommon.AppMgr import ToolSvc

    #########################################################################################################
    
    #from FastCaloSim.FastCaloSimConf import FastShowerCellBuilderTool
    #theFastShowerCellBuilderTool=FastShowerCellBuilderTool()

    mlog.info("now configure the non-interacting propagator...")
    from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator
    niPropagator = Trk__STEP_Propagator()
    niPropagator.MaterialEffects = False 
    ToolSvc+=niPropagator    
    mlog.info("configure nono-interacting propagator finished")

    mlog.info("now configure the TimedExtrapolator...")
    from TrkExTools.TimedExtrapolator import TimedExtrapolator
    timedExtrapolator=TimedExtrapolator()
    timedExtrapolator.STEP_Propagator = niPropagator
    timedExtrapolator.ApplyMaterialEffects = False
    ToolSvc+=timedExtrapolator
    #theFastShowerCellBuilderTool.Extrapolator=timedExtrapolator
    mlog.info("configure TimedExtrapolator finished")

    from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags 
    #theFastShowerCellBuilderTool.CaloEntrance = TrkDetFlags.InDetContainerName()

    kwargs.setdefault("CaloEntrance", TrkDetFlags.InDetContainerName())
    kwargs.setdefault("Extrapolator", timedExtrapolator)
    
    from FastCaloSim.FastCaloSimConf import FastShowerCellBuilderTool
    theFastShowerCellBuilderTool=FastShowerCellBuilderTool(name, **kwargs)

    #########################################################################################################
    #theFastShowerCellBuilderTool.Invisibles=[12, 14, 16, 1000022]
    
    #########################################################################################################

    try:
        ParticleParametrizationFileName=theFastShowerCellBuilderTool.ParticleParametrizationFileName
    except Exception:
        ParticleParametrizationFileName=""
    
    if ParticleParametrizationFileName=="" and len(theFastShowerCellBuilderTool.AdditionalParticleParametrizationFileNames)==0:
        ParticleParametrizationFileName="FastCaloSim/v1/ParticleEnergyParametrization.root"

    theFastShowerCellBuilderTool.ParticleParametrizationFileName=ParticleParametrizationFileName     
    mlog.info("ParticleParametrizationFile=%s",ParticleParametrizationFileName)

    #from AthenaCommon.AppMgr import theApp
    #svcMgr = theApp.serviceMgr()
    #svcMgr.MessageSvc.debugLimit   = 100000000

    mlog.info("all values:")
    mlog.info (theFastShowerCellBuilderTool)

    return theFastShowerCellBuilderTool
