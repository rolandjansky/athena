# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
import traceback
from AthenaCommon.AlgSequence import AlgSequence,AthSequencer

def PAU_ExtrapolateToCaloTool(theAtlasExtrapolator):

    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr
    
    from CaloDetDescr.CaloDepthToolBase import CaloDepthToolFactory
    theCaloDepthTool=CaloDepthToolFactory(depth='cscopt2')
    theCaloDepthTool.DepthChoice="cscopt2"
    # theCaloDepthTool=CaloDepthToolFactory(depth='cscopt')
    ToolSvc += theCaloDepthTool
    
    # add tool ExtrapolateTrackToCalo
    exToCalo = CfgMgr.ExtrapolateToCaloTool(name         = "PAUexToCalo",
                                            Extrapolator = theAtlasExtrapolator,
                                            CaloDepthTool=theCaloDepthTool.getFullName())
    ToolSvc+=exToCalo
    # print      exToCalo

    # exToCalo.CaloDepthTool.DepthChoice = "egparametrized"
    #
    # print "------------------------------------------------->>>extrapolToCalo<<<---------------------------------------------------"
    # print exToCalo.CaloDepthTool
    # print exToCalo.CaloDepthTool.DepthChoice
    # print exToCalo
    # print "------------------------------------------------->>>extrapolToCalo<<<---------------------------------------------------"

    return exToCalo
