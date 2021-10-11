#
#Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

import CscRawDataMonitoring.CscRawMonLabels as labels
import math

def getCSCLabelx(x):
	labelx = getattr(labels, x)
	return labelx

def getCSCEfficiency(num, num_err, den, den_err):
	epsilon = 1.e-3
	eff = 0
	err = 0
	if ( den > epsilon):
		eff = num / den
		err = math.fabs( ( (1.-2.*eff)*num_err*num_err + eff*eff*den_err*den_err )/(den*den) )

	return eff, err
