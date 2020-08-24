#
#Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

import CscRawDataMonitoring.CscRawMonLabels as labels
from CscRawDataMonitoring.CscRawMonLabels import *

def getCSCLabelx(x):
	labelx = getattr(labels, x)
	return labelx