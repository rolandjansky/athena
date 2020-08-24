# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# $Id: LArClusterSwGetters.py,v 1.4 2008/04/14 16:24:22 laplace Exp $
#
# File: LArClusterRec/python/LArClusterSwGetters.py
# Created: Mar 2007, sss
# Purpose: Define the Getter's for EM sliding window reconstruction.
#

from CaloRec.CaloClusterSwGetterBase import make_CaloClusterSwGetter

# Helper so that we don't need to type the name twice.
def _make_getter (*args, **kw):
    if 'corrversion' not in kw:
        # Temporary until metatool / ToolWithConstant issues are worked out.
        kw = kw.copy()
        from egammaRec.egammaRecFlags import jobproperties
        kw['corrversion'] = jobproperties.egammaRecFlags.clusterCorrectionVersion()
    g = make_CaloClusterSwGetter (*args, **kw)
    globals()[g.__name__] = g
    return g

# Define getters for electrons and photons, for 5x5, 3x5, and 3x7 clusters.

_make_getter ('LArClusterSwEle55Getter', 'ele', 5, 5, towrite = False)
_make_getter ('LArClusterSwGam55Getter', 'gam', 5, 5, towrite = False)

_make_getter ('LArClusterSwEle35Getter', 'ele', 3, 5, towrite = True)
_make_getter ('LArClusterSwGam35Getter', 'gam', 3, 5, towrite = False)

_make_getter ('LArClusterSwEle37Getter', 'ele', 3, 7, towrite = False)
_make_getter ('LArClusterSwGam37Getter', 'gam', 3, 7, towrite = False)

_make_getter ('LArClusterSw711GetterNocorr', 'ele', 7, 11,
              suffix = 'Nocorr', corrversion = 'none')
