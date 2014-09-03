# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *

def CaloMBAverageToolDefault(name='CaloMBAverageToolDefault'):
    mlog = logging.getLogger( 'CaloMBAverageToolDefault' )

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
