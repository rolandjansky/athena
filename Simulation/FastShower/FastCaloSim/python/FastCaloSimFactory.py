# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Constants import *
from RecExConfig.Configured import Configured

def FastCaloSimFactory():
    from AthenaCommon.Logging import logging
    mlog = logging.getLogger( 'FastCaloSimFactory::configure:' )

    from CaloRec.CaloCellFlags import jobproperties
    from AthenaCommon.AppMgr import ToolSvc

    mlog.info("now configure theFastShowerCellBuilderTool...")
    from FastCaloSim.FastCaloSimConf import FastShowerCellBuilderTool
    theFastShowerCellBuilderTool=FastShowerCellBuilderTool()

    # FastShowerCellBuilderTool looks for the extrapolators with fixed names. Should be changed...
    #########################################################################################################
    mlog.info("now configure ExtrapolateToCaloTools...")
    from TrackToCalo.ExtrapolateToCaloToolBase import ExtrapolateToCaloToolFactory
    
    theFSElectronExtrapolTrackToCaloTool        =ExtrapolateToCaloToolFactory(depth="middle",straightLine=False)
    theFastShowerCellBuilderTool.electronExtrapolTool=theFSElectronExtrapolTrackToCaloTool.getFullName()
    ToolSvc+=theFSElectronExtrapolTrackToCaloTool
    
    theFSElectronExtrapolTrackToCaloToolEntrance=ExtrapolateToCaloToolFactory(depth="entrance",straightLine=False)
    theFastShowerCellBuilderTool.electronExtrapolToolEntrance=theFSElectronExtrapolTrackToCaloToolEntrance.getFullName()
    ToolSvc+=theFSElectronExtrapolTrackToCaloToolEntrance

    theFSPhotonExtrapolTrackToCaloTool          =ExtrapolateToCaloToolFactory(depth="middle",straightLine=True)
    theFastShowerCellBuilderTool.gammaExtrapolTool=theFSPhotonExtrapolTrackToCaloTool.getFullName()
    ToolSvc+=theFSPhotonExtrapolTrackToCaloTool

    theFSPhotonExtrapolTrackToCaloToolEntrance  =ExtrapolateToCaloToolFactory(depth="entrance",straightLine=True)
    theFastShowerCellBuilderTool.gammaExtrapolToolEntrance=theFSPhotonExtrapolTrackToCaloToolEntrance.getFullName()
    ToolSvc+=theFSPhotonExtrapolTrackToCaloToolEntrance

    mlog.info("configure ExtrapolateToCaloTools finished")

    #########################################################################################################
    mlog.info("now configure theExtrapolator...")
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    theExtrapolator=AtlasExtrapolator()
    ToolSvc+=theExtrapolator
    theFastShowerCellBuilderTool.ExtrapolatorName=theExtrapolator.getType()
    theFastShowerCellBuilderTool.ExtrapolatorInstanceName=theExtrapolator.getName()
    mlog.info("configure theExtrapolator finished")

    #########################################################################################################
    from CaloTrackingGeometry.CaloSurfaceBuilderBase import CaloSurfaceBuilderFactory
    mlog.info("now configure theCaloSurfaceBuilderMiddle...")
    theCaloSurfaceBuilderMiddle=CaloSurfaceBuilderFactory(depth="middle")
    theFastShowerCellBuilderTool.CalosurfMiddleInstanceName=theCaloSurfaceBuilderMiddle.getName()
    ToolSvc+=theCaloSurfaceBuilderMiddle
    mlog.info("configure theCaloSurfaceBuilderMiddle finished")

    mlog.info("now configure theCaloSurfaceBuilderEntrance...")
    theCaloSurfaceBuilderEntrance=CaloSurfaceBuilderFactory(depth="entrance")
    theFastShowerCellBuilderTool.CalosurfEntranceInstanceName=theCaloSurfaceBuilderEntrance.getName()
    ToolSvc+=theCaloSurfaceBuilderEntrance
    mlog.info("configure theCaloSurfaceBuilderEntrance finished")

    #########################################################################################################
    #theFastShowerCellBuilderTool.Invisibles=[12, 14, 16, 1000022]
    
    #########################################################################################################
    import os

    try:
        ParticleParametrizationFileName=theFastShowerCellBuilderTool.ParticleParametrizationFileName
    except:
        ParticleParametrizationFileName=""
    
    if ParticleParametrizationFileName=="" and len(theFastShowerCellBuilderTool.AdditionalParticleParametrizationFileNames)==0:
        try:
            ATLASCALDATA=os.environ["ATLASCALDATA"]
            ParticleParametrizationFileName=ATLASCALDATA+"/FastCaloSim/v1/ParticleEnergyParametrization.root"
        except:
            ATLASCALDATA=""
            ParticleParametrizationFileName="ParticleEnergyParametrization.root"

    theFastShowerCellBuilderTool.ParticleParametrizationFileName=ParticleParametrizationFileName     
    mlog.info("ParticleParametrizationFile=%s",ParticleParametrizationFileName)

    #from AthenaCommon.AppMgr import theApp
    #svcMgr = theApp.serviceMgr()
    #svcMgr.MessageSvc.debugLimit   = 100000000

    mlog.info("all values:")
    print theFastShowerCellBuilderTool

    return theFastShowerCellBuilderTool



