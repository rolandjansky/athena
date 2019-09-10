# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import ns

def CaloMBAverageToolDefault(name='CaloMBAverageToolDefault'):
    from CaloTools.CaloToolsConf import CaloMBAverageTool
    # get public tool LArOFCTool
    from LArRecUtils.LArOFCToolDefault import LArOFCToolDefault
    theOFCTool = LArOFCToolDefault()
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += theOFCTool
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.zeroLuminosity(): 
        NMinBias=0
        deltaBunch=1
    else:
        NMinBias=jobproperties.Beam.numberOfCollisions()
        deltaBunch=int(jobproperties.Beam.bunchSpacing()/( 25.*ns)+0.5)

    theTool = CaloMBAverageTool(name,
                                LArOFCTool = theOFCTool,
                                NMinBias = NMinBias,
                                deltaBunch = deltaBunch)
    return theTool
