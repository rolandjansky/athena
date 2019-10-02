# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import datatable as dt
import numpy as np
def CVM(cvm, mean, var):
	z = (cvm - mean) / (45. * var)**0.5 + 1./6
	z_i = dt.cvm_ztable
	p_i = dt.cvm_ptable
	if z > z_i[-1]:
		return 1.
	elif z < z_i[0]:
		return 0.
	else:
		return np.interp(z, z_i, p_i)
