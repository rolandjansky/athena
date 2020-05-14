#                                                                
#Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
# 

import MdtRawDataMonitoring.MdtRawMonLabels as labels
from MdtRawDataMonitoring.MdtRawMonLabels import *

def getMDTLabel(x,y):
    labelx = getattr(labels, x)
    labely = getattr(labels, y)
    #print(labelx)
    #print(labely)
    return labelx,labely

def getMDTLabelx(x):
    labelx = getattr(labels, x)
    #print(labelx)
    return labelx


