# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

""" Instantiates tools for back tracking and L2
"""

__author__ = "J. Masik"
__version__= "$Revision$"
__doc__    = "InDetTrigConfigRecLoadToolsBack"


from AthenaCommon.AppMgr import ToolSvc

from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
from InDetTrigRecExample.InDetTrigConfigRecLoadTools import \
    InDetTrigPatternPropagator

# configure tools used
# TRT detector elements road builder
#
from TRT_DetElementsRoadTool_xk.TRT_DetElementsRoadTool_xkConf import InDet__TRT_DetElementsRoadMaker_xk
InDetTrigTRTDetElementsRoadMaker =  InDet__TRT_DetElementsRoadMaker_xk(name   = 'InDetTrigTRTRoadMaker',
                                                                       MagneticFieldMode     = 'MapSolenoid',
                                                                       RoadWidth             = 20.,
                                                                       PropagatorTool        = InDetTrigPatternPropagator)

ToolSvc += InDetTrigTRTDetElementsRoadMaker
if (InDetTrigFlags.doPrintConfigurables()):
  print (     InDetTrigTRTDetElementsRoadMaker)
