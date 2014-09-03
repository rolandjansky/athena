# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *

def CaloNoiseToolDefault(flag='',name='CaloNoiseToolDefault'):

    # check if tool already exists
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if hasattr(svcMgr.ToolSvc, name):
        # re-use previously configured (public) tool
        return getattr(svcMgr.ToolSvc, name)

    mlog = logging.getLogger( 'CaloNoiseToolDefault' )

    if flag=='db' :
       _useDB = True
    elif flag=='tool' :
       _useDB = False
    else :
       # will put here logic to select according to global flag
       from AthenaCommon.GlobalFlags import globalflags
       from AthenaCommon.BeamFlags import jobproperties
       if globalflags.DataSource()=='data' and globalflags.DetGeo()!='ctbh6' and globalflags.DetGeo()!='ctbh8':
           _useDB = True
       elif globalflags.DataSource()=='geant4' and jobproperties.Beam.zeroLuminosity():
           _useDB = True
       else:
          # now uses also calonoisetoolDB for MC with pileup.. requires a new global tag for pileup reco to have the noise properly read
           _useDB = True

    if _useDB :
        mlog.info(" Using CaloNoiseToolDB" )
        from AthenaCommon.GlobalFlags import globalflags
        if globalflags.DataSource()=='data':
            from CaloTools.CaloNoiseToolDBData import CaloNoiseToolDBData
            theTool = CaloNoiseToolDBData(name)
        else:
            from CaloTools.CaloNoiseToolDBMC import CaloNoiseToolDBMC
            theTool = CaloNoiseToolDBMC(name)
        return theTool
    else :
        mlog.info("Using CaloNoiseTool")
        from CaloTools.CaloToolsConf import CaloNoiseTool
        # Tile configuration
        from TileConditions.TileInfoConfigurator import TileInfoConfigurator
        tileInfoConfigurator = TileInfoConfigurator()
        tileInfoConfigurator.setupCOOL()
        
        # CaloNoiseTool configuration
        WorkMode=1
        WithOF=True
        TileInfoName="TileInfo"
        # get public tool LArADC2MeVTool
        from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
        theADC2MeVTool = LArADC2MeVToolDefault()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += theADC2MeVTool
        # get public tool LArOFCTool
        from LArRecUtils.LArOFCToolDefault import LArOFCToolDefault
        theOFCTool = LArOFCToolDefault()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += theOFCTool
        from AthenaCommon.BeamFlags import jobproperties
        if jobproperties.Beam.zeroLuminosity(): 
            ReturnNoise="electronicNoise"
            NMinBias=0
            deltaBunch=1
        else:
            ReturnNoise="totalNoise"
            NMinBias=jobproperties.Beam.numberOfCollisions()
            deltaBunch=int(jobproperties.Beam.bunchSpacing()/( 25.*ns)+0.5)
        from AthenaCommon.GlobalFlags import globalflags
        if globalflags.DataSource()=='data':
            IsMC = False
            UseSymmetry= False
        else:
            IsMC = True
            UseSymmetry= True

        from LArROD.LArRODFlags import larRODFlags
        theTool = CaloNoiseTool(name,
                          WorkMode = WorkMode,
                          WithOF = WithOF,
                          TileInfoName = TileInfoName,
                          LArADC2MeVTool = theADC2MeVTool,
                          LArOFCTool = theOFCTool,
                          ReturnNoise = ReturnNoise,
                          NMinBias = NMinBias,
                          deltaBunch = deltaBunch,
                          IsMC = IsMC,
                          UseSymmetry = UseSymmetry,
                          firstSample=larRODFlags.firstSample())
        return theTool
