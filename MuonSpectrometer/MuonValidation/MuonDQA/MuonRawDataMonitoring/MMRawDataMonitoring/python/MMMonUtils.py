#                                                                
#Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# 

import MMRawDataMonitoring.MMRawMonLabels as labels
from MMRawDataMonitoring.MMRawMonLabels import *

def getMMLabel(x,y):

    labelx = getattr(labels, x)
    labely = getattr(labels, y)
    return labelx,labely

def getMMLabelY(y):

    labely = getattr(labels, y)
    return labely

def getMMLabelX(x):

    labelx = getattr(labels, x)
    return labelx
