import MdtRawDataMonitoring.MdtRawMonLabels as labels
from MdtRawDataMonitoring.MdtRawMonLabels import *

def getMDTLabel(x,y):
    labelx = getattr(labels, x)
    labely = getattr(labels, y)
    #print(labelx)
    #print(labely)
    return labelx,labely
